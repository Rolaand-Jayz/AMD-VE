#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include "ave/reference_providers.hpp"
#include "ave/temporal_frame_types.hpp"
#include "ave/temporal_providers.hpp"

namespace {

int g_failures = 0;

void check(bool condition, const std::string& msg) {
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        ++g_failures;
    }
}

ave::FrameRef makeFrame(const std::string& path, int w, int h, int64_t idx) {
    ave::FrameRef f;
    f.path = path;
    f.width = w;
    f.height = h;
    f.index = idx;
    f.timestamp = {idx, 30};
    return f;
}

ave::TemporalFrameWindow makeWindowWithHistory() {
    ave::TemporalFrameWindow w;
    w.current = makeFrame("frame_005.png", 64, 64, 5);
    w.timebase = {1, 30};
    w.history = {
        makeFrame("frame_003.png", 64, 64, 3),
        makeFrame("frame_004.png", 64, 64, 4),
    };
    return w;
}

ave::TemporalFrameWindow makeWindowNoHistory() {
    ave::TemporalFrameWindow w;
    w.current = makeFrame("frame_001.png", 64, 64, 1);
    w.timebase = {1, 30};
    return w;
}

ave::ImagePlane makeTestImage(int w, int h, uint8_t fill = 128) {
    ave::ImagePlane p;
    p.width = w; p.height = h; p.channels = 3;
    p.pixels.resize(static_cast<size_t>(w) * h * 3, fill);
    return p;
}

ave::ImagePlane makeGradientDepth(int w, int h) {
    ave::ImagePlane p;
    p.width = w; p.height = h; p.channels = 1;
    p.pixels.resize(static_cast<size_t>(w) * h);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            p.pixels[y * w + x] = static_cast<uint8_t>(
                (static_cast<float>(y) / static_cast<float>(h)) * 255.0f);
        }
    }
    return p;
}

}  // namespace

void testBlockMotionReturnsField() {
    ave::CpuBlockMotionProvider provider;
    auto result = provider.estimate(makeWindowWithHistory());
    check(result.ok(), "CpuBlockMotionProvider should return ok with history");
    check(result.value.has_value(), "result should have value");
    check(result.value->width > 0, "motion field width should be > 0");
    check(result.value->height > 0, "motion field height should be > 0");
    check(result.value->isValid(), "motion field should be valid");

    int fieldW = (64 + 8 - 1) / 8;
    int fieldH = (64 + 8 - 1) / 8;
    check(result.value->width == fieldW, "motion field width should match block count");
    check(result.value->height == fieldH, "motion field height should match block count");
}

void testBlockMotionNoHistory() {
    ave::CpuBlockMotionProvider provider;
    auto result = provider.estimate(makeWindowNoHistory());
    check(!result.ok(), "CpuBlockMotionProvider should fail without history");
    check(result.diagnostics.size() >= 1, "should have diagnostic");
    check(result.diagnostics[0].code == "NO_HISTORY_FRAMES",
          "diagnostic code should be NO_HISTORY_FRAMES");
}

void testBlockMotionCustomConfig() {
    ave::BlockMotionConfig config;
    config.blockSize = 16;
    config.searchRadius = 32;
    config.sceneChangeThreshold = 20.0f;
    ave::CpuBlockMotionProvider provider(config);
    auto result = provider.estimate(makeWindowWithHistory());
    check(result.ok(), "CpuBlockMotionProvider with custom config should succeed");

    int fieldW = (64 + 16 - 1) / 16;
    int fieldH = (64 + 16 - 1) / 16;
    check(result.value->width == fieldW, "custom blockSize should change field width");
}

void testBlockMotionPolymorphism() {
    std::unique_ptr<ave::MotionProvider> provider =
        std::make_unique<ave::CpuBlockMotionProvider>();
    auto result = provider->estimate(makeWindowWithHistory());
    check(result.ok(), "polymorphic CpuBlockMotionProvider should work");
}

void testSceneChangeMetricIdenticalFrames() {
    auto img = makeTestImage(16, 16, 100);
    float metric = ave::CpuBlockMotionProvider::computeSceneChangeMetric(img, img);
    check(metric < 0.01f, "identical frames should have near-zero scene change metric");
}

void testSceneChangeMetricDifferentFrames() {
    auto imgA = makeTestImage(16, 16, 0);
    auto imgB = makeTestImage(16, 16, 255);
    float metric = ave::CpuBlockMotionProvider::computeSceneChangeMetric(imgA, imgB);
    check(metric > 200.0f, "very different frames should have high scene change metric");
}

void testSceneChangeMetricDifferentSizes() {
    auto imgA = makeTestImage(16, 16, 128);
    auto imgB = makeTestImage(32, 32, 128);
    float metric = ave::CpuBlockMotionProvider::computeSceneChangeMetric(imgA, imgB);
    check(metric > 100.0f, "different-sized frames should return high metric");
}

void testPseudoDepthReturnsValidPlane() {
    ave::PseudoDepthProvider provider(32, 32);
    auto result = provider.estimateDepth(makeFrame("test.png", 1920, 1080, 0));
    check(result.ok(), "PseudoDepthProvider should return ok");
    check(result.value->width == 32, "depth width should be 32");
    check(result.value->height == 32, "depth height should be 32");
    check(result.value->channels == 1, "depth channels should be 1");
    check(result.value->isValid(), "depth plane should be valid");
}

void testPseudoDepthHasGradient() {
    ave::PseudoDepthProvider provider(32, 32);
    auto result = provider.estimateDepth(makeFrame("test.png", 640, 480, 0));
    check(result.ok(), "PseudoDepthProvider should succeed");

    uint8_t topVal = result.value->pixels[0];
    uint8_t botVal = result.value->pixels[31 * 32 + 16];
    check(botVal > topVal, "bottom of pseudo-depth should be brighter than top (gradient)");
}

void testPseudoDepthHasDiagnostic() {
    ave::PseudoDepthProvider provider(16, 16);
    auto result = provider.estimateDepth(makeFrame("test.png", 640, 480, 0));
    check(result.ok(), "PseudoDepthProvider should succeed");
    check(!result.diagnostics.empty(), "should have diagnostics");
    check(result.diagnostics[0].code == "PSEUDO_DEPTH",
          "diagnostic code should be PSEUDO_DEPTH");
    check(result.diagnostics[0].severity == ave::ProviderDiagnostic::Severity::Info,
          "pseudo-depth diagnostic should be Info severity");
}

void testPseudoDepthPolymorphism() {
    std::unique_ptr<ave::DepthProvider> provider =
        std::make_unique<ave::PseudoDepthProvider>();
    auto result = provider->estimateDepth(makeFrame("test.png", 640, 480, 0));
    check(result.ok(), "polymorphic PseudoDepthProvider should work");
}

void testDepthEdgeExtractorNoDepth() {
    ave::DepthEdgeExtractor extractor(0.1f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = extractor.compute(pkt);
    check(!result.ok(), "DepthEdgeExtractor should fail without depth");
    check(result.diagnostics[0].code == "NO_DEPTH_DATA",
          "diagnostic should be NO_DEPTH_DATA");
}

void testDepthEdgeExtractorWithDepth() {
    ave::DepthEdgeExtractor extractor(0.01f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    pkt.depth = makeGradientDepth(16, 16);
    auto result = extractor.compute(pkt);
    check(result.ok(), "DepthEdgeExtractor should succeed with depth");
    check(result.value->width == 16, "edge width should match depth width");
    check(result.value->height == 16, "edge height should match depth height");
}

void testDepthEdgeExtractorStaticMethod() {
    auto depth = makeGradientDepth(16, 16);
    auto edges = ave::DepthEdgeExtractor::extractEdges(depth, 0.01f);
    check(edges.width == 16, "extracted edges width should be 16");
    check(edges.height == 16, "extracted edges height should be 16");
    check(edges.channels == 1, "edges should be single channel");
    check(edges.isValid(), "edges should be valid");

    bool hasEdges = false;
    for (auto p : edges.pixels) {
        if (p > 0) { hasEdges = true; break; }
    }
    check(hasEdges, "gradient depth should produce some edges");
}

void testDepthEdgeExtractorUniformDepth() {
    ave::ImagePlane uniform;
    uniform.width = 16; uniform.height = 16; uniform.channels = 1;
    uniform.pixels.resize(256, 128);
    auto edges = ave::DepthEdgeExtractor::extractEdges(uniform, 0.01f);
    bool hasEdges = false;
    for (auto p : edges.pixels) {
        if (p > 0) { hasEdges = true; break; }
    }
    check(!hasEdges, "uniform depth should produce no edges");
}

void testDepthEdgeExtractorPolymorphism() {
    std::unique_ptr<ave::MaskProvider> provider =
        std::make_unique<ave::DepthEdgeExtractor>(0.1f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = provider->compute(pkt);
    check(!result.ok(), "polymorphic DepthEdgeExtractor should fail without depth");
}

void testReactiveMaskWithMotion() {
    ave::ReactiveMaskProvider provider(0.01f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    pkt.motionPrevToCurrent = m;
    auto result = provider.compute(pkt);
    check(result.ok(), "ReactiveMaskProvider should succeed with motion");
    check(result.value->width == 64, "reactive mask width should match frame");
    check(result.value->height == 64, "reactive mask height should match frame");
    bool hasReactive = false;
    for (auto p : result.value->pixels) { if (p > 0) { hasReactive = true; break; } }
    check(hasReactive, "high-motion areas should be marked reactive");
}

void testReactiveMaskNoMotion() {
    ave::ReactiveMaskProvider provider(0.01f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = provider.compute(pkt);
    check(result.ok(), "ReactiveMaskProvider should succeed without motion");
    bool allZero = true;
    for (auto p : result.value->pixels) { if (p > 0) { allZero = false; break; } }
    check(allZero, "without motion data, reactive mask should be all zeros");
}

void testOcclusionMaskWithMotion() {
    ave::OcclusionMaskProvider provider(2.0f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    pkt.motionPrevToCurrent = m;
    auto result = provider.compute(pkt);
    check(result.ok(), "OcclusionMaskProvider should succeed");
    bool hasOcclusion = false;
    for (auto p : result.value->pixels) { if (p > 0) { hasOcclusion = true; break; } }
    check(hasOcclusion, "high-motion areas should be occluded");
}

void testConfidenceMapHighMotion() {
    ave::ConfidenceMapProvider provider;
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {20.0f, 0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    pkt.motionPrevToCurrent = m;
    auto result = provider.computeConfidence(pkt);
    check(result.ok(), "ConfidenceMapProvider should succeed");
    bool hasLowConf = false;
    for (auto p : result.value->pixels) { if (p < 128) { hasLowConf = true; break; } }
    check(hasLowConf, "high motion should produce low confidence");
}

void testConfidenceMapSceneReset() {
    ave::ConfidenceMapProvider provider;
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    pkt.window.sceneResetBeforeCurrent = true;
    auto result = provider.computeConfidence(pkt);
    check(result.ok(), "ConfidenceMapProvider scene reset should succeed");
    bool allZero = true;
    for (auto p : result.value->pixels) { if (p > 0) { allZero = false; break; } }
    check(allZero, "scene reset should zero all confidence");
}

void testAiAllowedMaskFromConfidence() {
    ave::AiAllowedMaskProvider provider(0.3f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    ave::ImagePlane confMap;
    confMap.width = 64; confMap.height = 64; confMap.channels = 1;
    confMap.pixels.resize(64 * 64);
    for (int i = 0; i < 64 * 64; ++i) {
        confMap.pixels[i] = (i < 64 * 32) ? 50 : 200;
    }
    pkt.confidenceMap = confMap;
    auto result = provider.compute(pkt);
    check(result.ok(), "AiAllowedMaskProvider should succeed");
    check(result.value->pixels[0] == 255,
          "low-confidence area should allow AI");
    check(result.value->pixels[64 * 63] == 0,
          "high-confidence area should NOT allow AI");
}

void testAiAllowedMaskNoConfidence() {
    ave::AiAllowedMaskProvider provider(0.3f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = provider.compute(pkt);
    check(result.ok(), "AiAllowedMaskProvider without confidence should succeed");
    bool allAllow = true;
    for (auto p : result.value->pixels) { if (p != 255) { allAllow = false; break; } }
    check(allAllow, "without confidence map, AI should be allowed everywhere");
}

void testTemporalHistoryCacheAddAndEvict() {
    ave::TemporalHistoryCache cache(3);
    check(cache.maxFrames() == 3, "max frames should be 3");
    check(cache.entries().empty(), "new cache should be empty");

    ave::FrameRef f1 = makeFrame("f1.png", 64, 64, 1);
    ave::MotionField m1;
    m1.width = 1; m1.height = 1; m1.vectors = {0.0f, 0.0f};

    cache.addFrame(f1, {1, 30}, m1);
    check(cache.entries().size() == 1, "after 1 add, size should be 1");
    check(!cache.hadSceneReset(), "no scene reset after add");

    cache.addFrame(makeFrame("f2.png", 64, 64, 2), {2, 30}, m1);
    cache.addFrame(makeFrame("f3.png", 64, 64, 3), {3, 30}, m1);
    check(cache.entries().size() == 3, "after 3 adds, size should be 3");

    cache.addFrame(makeFrame("f4.png", 64, 64, 4), {4, 30}, m1);
    check(cache.entries().size() == 3, "after 4 adds with max 3, size should still be 3");
    check(cache.entries()[0].frame.path == "f2.png", "oldest should be evicted");
}

void testTemporalHistoryCacheSceneReset() {
    ave::TemporalHistoryCache cache(5);
    ave::MotionField m;
    m.width = 1; m.height = 1; m.vectors = {0.0f, 0.0f};
    cache.addFrame(makeFrame("f1.png", 64, 64, 1), {1, 30}, m);
    cache.markSceneReset();
    check(cache.entries().empty(), "scene reset should clear entries");
    check(cache.hadSceneReset(), "scene reset flag should be set");
}

void testTemporalHistoryCacheClear() {
    ave::TemporalHistoryCache cache(5);
    ave::MotionField m;
    m.width = 1; m.height = 1; m.vectors = {0.0f, 0.0f};
    cache.addFrame(makeFrame("f1.png", 64, 64, 1), {1, 30}, m);
    cache.clear();
    check(cache.entries().empty(), "clear should empty entries");
    check(!cache.hadSceneReset(), "clear should reset scene flag");
}

void testCpuTemporalReconstructorPassesCurrent() {
    ave::CpuTemporalReconstructor recon(0.5f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = recon.reconstruct(pkt);
    check(result.ok(), "CpuTemporalReconstructor should succeed");
    check(result.value->path == "frame_005.png",
          "reconstruction should return current frame");
}

void testCpuTemporalReconstructorSceneReset() {
    ave::CpuTemporalReconstructor recon(0.5f);
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    pkt.window.sceneResetBeforeCurrent = true;
    auto result = recon.reconstruct(pkt);
    check(result.ok(), "scene reset reconstruction should succeed");
    check(result.value->path == "frame_005.png",
          "on scene reset, should return current frame");
}

void testCpuTemporalReconstructorNoHistory() {
    ave::CpuTemporalReconstructor recon(0.5f);
    ave::TemporalFramePacket pkt;
    pkt.window.current = makeFrame("frame.png", 64, 64, 1);
    pkt.window.timebase = {1, 30};
    auto result = recon.reconstruct(pkt);
    check(result.ok(), "no-history reconstruction should succeed");
    check(result.value->path == "frame.png",
          "no history should return current frame");
}

void testCpuTemporalReconstructorPolymorphism() {
    std::unique_ptr<ave::TemporalReconstructor> recon =
        std::make_unique<ave::CpuTemporalReconstructor>();
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindowWithHistory();
    auto result = recon->reconstruct(pkt);
    check(result.ok(), "polymorphic CpuTemporalReconstructor should work");
}

void testConfidenceBlendedRefinerReturnsSource() {
    ave::ConfidenceBlendedRefiner refiner(0.7f);
    auto src = makeFrame("src.png", 64, 64, 0);
    auto temporal = makeFrame("temporal.png", 64, 64, 0);
    ave::ImagePlane conf;
    conf.width = 64; conf.height = 64; conf.channels = 1;
    conf.pixels.resize(64 * 64, 200);
    ave::ImagePlane aiMask;
    aiMask.width = 64; aiMask.height = 64; aiMask.channels = 1;
    aiMask.pixels.resize(64 * 64, 255);
    auto result = refiner.refine(src, temporal, conf, aiMask);
    check(result.ok(), "ConfidenceBlendedRefiner should succeed");
    check(result.value->path == "src.png",
          "refiner should return source frame");
}

void testConfidenceBlendedRefinerPolymorphism() {
    std::unique_ptr<ave::MaskedAIRefiner> refiner =
        std::make_unique<ave::ConfidenceBlendedRefiner>();
    auto src = makeFrame("src.png", 64, 64, 0);
    auto temporal = makeFrame("temporal.png", 64, 64, 0);
    ave::ImagePlane conf;
    conf.width = 64; conf.height = 64; conf.channels = 1;
    conf.pixels.resize(64 * 64, 128);
    ave::ImagePlane aiMask;
    aiMask.width = 64; aiMask.height = 64; aiMask.channels = 1;
    aiMask.pixels.resize(64 * 64, 255);
    auto result = refiner->refine(src, temporal, conf, aiMask);
    check(result.ok(), "polymorphic ConfidenceBlendedRefiner should work");
}

int main() {
    testBlockMotionReturnsField();
    testBlockMotionNoHistory();
    testBlockMotionCustomConfig();
    testBlockMotionPolymorphism();
    testSceneChangeMetricIdenticalFrames();
    testSceneChangeMetricDifferentFrames();
    testSceneChangeMetricDifferentSizes();
    testPseudoDepthReturnsValidPlane();
    testPseudoDepthHasGradient();
    testPseudoDepthHasDiagnostic();
    testPseudoDepthPolymorphism();
    testDepthEdgeExtractorNoDepth();
    testDepthEdgeExtractorWithDepth();
    testDepthEdgeExtractorStaticMethod();
    testDepthEdgeExtractorUniformDepth();
    testDepthEdgeExtractorPolymorphism();
    testReactiveMaskWithMotion();
    testReactiveMaskNoMotion();
    testOcclusionMaskWithMotion();
    testConfidenceMapHighMotion();
    testConfidenceMapSceneReset();
    testAiAllowedMaskFromConfidence();
    testAiAllowedMaskNoConfidence();
    testTemporalHistoryCacheAddAndEvict();
    testTemporalHistoryCacheSceneReset();
    testTemporalHistoryCacheClear();
    testCpuTemporalReconstructorPassesCurrent();
    testCpuTemporalReconstructorSceneReset();
    testCpuTemporalReconstructorNoHistory();
    testCpuTemporalReconstructorPolymorphism();
    testConfidenceBlendedRefinerReturnsSource();
    testConfidenceBlendedRefinerPolymorphism();

    if (g_failures > 0) {
        std::cerr << g_failures << " test(s) FAILED." << std::endl;
        return 1;
    }
    std::cout << "All reference provider tests passed." << std::endl;
    return 0;
}
