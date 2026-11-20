#include <cassert>
#include <iostream>
#include <memory>
#include <string>

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

ave::TemporalFrameWindow makeWindow() {
    ave::TemporalFrameWindow w;
    w.current = makeFrame("frame_005.png", 1920, 1080, 5);
    w.timebase = {1, 30};
    w.history = {
        makeFrame("frame_003.png", 1920, 1080, 3),
        makeFrame("frame_004.png", 1920, 1080, 4),
    };
    return w;
}

ave::TemporalFramePacket makePacket() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();
    return pkt;
}

ave::ImagePlane makeMask() {
    ave::ImagePlane p;
    p.width = 4; p.height = 4; p.channels = 1;
    p.pixels.resize(16, 255);
    return p;
}

}  // namespace

void testNullMotionProviderReturnsDiagnostic() {
    ave::NullMotionProvider provider;
    auto result = provider.estimate(makeWindow());
    check(!result.ok(), "NullMotionProvider should not return ok");
    check(!result.value.has_value(), "NullMotionProvider value should be nullopt");
    check(result.diagnostics.size() == 1, "should have exactly 1 diagnostic");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "diagnostic code should be PROVIDER_NOT_IMPLEMENTED");
    check(result.diagnostics[0].severity == ave::ProviderDiagnostic::Severity::Warning,
          "null provider diagnostic should be Warning severity");
}

void testNullDepthProviderReturnsDiagnostic() {
    ave::NullDepthProvider provider;
    auto result = provider.estimateDepth(makeFrame("test.png", 640, 480, 0));
    check(!result.ok(), "NullDepthProvider should not return ok");
    check(result.diagnostics.size() == 1, "should have 1 diagnostic");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "diagnostic code should be PROVIDER_NOT_IMPLEMENTED");
}

void testNullMaskProviderReturnsDiagnostic() {
    ave::NullMaskProvider provider;
    auto result = provider.compute(makePacket());
    check(!result.ok(), "NullMaskProvider should not return ok");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "NullMaskProvider diagnostic code");
}

void testNullConfidenceProviderReturnsDiagnostic() {
    ave::NullConfidenceProvider provider;
    auto result = provider.computeConfidence(makePacket());
    check(!result.ok(), "NullConfidenceProvider should not return ok");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "NullConfidenceProvider diagnostic code");
}

void testNullTemporalReconstructorReturnsDiagnostic() {
    ave::NullTemporalReconstructor provider;
    auto result = provider.reconstruct(makePacket());
    check(!result.ok(), "NullTemporalReconstructor should not return ok");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "NullTemporalReconstructor diagnostic code");
}

void testNullMaskedAIRefinerReturnsDiagnostic() {
    ave::NullMaskedAIRefiner provider;
    auto src = makeFrame("src.png", 640, 480, 0);
    auto temporal = makeFrame("temporal.png", 640, 480, 0);
    auto conf = makeMask();
    auto aiMask = makeMask();
    auto result = provider.refine(src, temporal, conf, aiMask);
    check(!result.ok(), "NullMaskedAIRefiner should not return ok");
    check(result.diagnostics[0].code == "PROVIDER_NOT_IMPLEMENTED",
          "NullMaskedAIRefiner diagnostic code");
}

void testProviderResultOkFalseWhenEmpty() {
    ave::ProviderResult<ave::MotionField> result;
    check(!result.ok(), "default ProviderResult should not be ok");
    check(!result.value.has_value(), "default value should be nullopt");
}

void testProviderResultOkTrueWhenValue() {
    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f};

    auto result = ave::ProviderResult<ave::MotionField>::success(m);
    check(result.ok(), "ProviderResult with value should be ok");
    check(result.value.has_value(), "value should be present");
    check(result.value->width == 2, "value width should be 2");
    check(result.diagnostics.empty(), "success result should have no diagnostics");
}

void testProviderResultFailureFactory() {
    auto result = ave::ProviderResult<ave::MotionField>::failure({
        ave::ProviderDiagnostic::Severity::Error,
        "TEST_CODE",
        "test message"
    });
    check(!result.ok(), "failure result should not be ok");
    check(result.diagnostics.size() == 1, "failure should have 1 diagnostic");
    check(result.diagnostics[0].severity == ave::ProviderDiagnostic::Severity::Error,
          "severity should be Error");
    check(result.diagnostics[0].code == "TEST_CODE", "code should be TEST_CODE");
    check(result.diagnostics[0].message == "test message", "message should match");
}

void testProviderResultMultipleDiagnostics() {
    std::vector<ave::ProviderDiagnostic> diags;
    diags.push_back({ave::ProviderDiagnostic::Severity::Info, "INFO_1", "info 1"});
    diags.push_back({ave::ProviderDiagnostic::Severity::Warning, "WARN_1", "warn 1"});
    auto result = ave::ProviderResult<ave::MotionField>::failure(diags);
    check(!result.ok(), "multi-diag failure should not be ok");
    check(result.diagnostics.size() == 2, "should have 2 diagnostics");
}

void testDiagnosticSeverityToString() {
    check(ave::toString(ave::ProviderDiagnostic::Severity::Info) == "info",
          "Info toString");
    check(ave::toString(ave::ProviderDiagnostic::Severity::Warning) == "warning",
          "Warning toString");
    check(ave::toString(ave::ProviderDiagnostic::Severity::Error) == "error",
          "Error toString");
}

void testProviderPolymorphism() {
    std::unique_ptr<ave::MotionProvider> provider = std::make_unique<ave::NullMotionProvider>();
    auto result = provider->estimate(makeWindow());
    check(!result.ok(), "polymorphic NullMotionProvider should not return ok");
}

int main() {
    testNullMotionProviderReturnsDiagnostic();
    testNullDepthProviderReturnsDiagnostic();
    testNullMaskProviderReturnsDiagnostic();
    testNullConfidenceProviderReturnsDiagnostic();
    testNullTemporalReconstructorReturnsDiagnostic();
    testNullMaskedAIRefinerReturnsDiagnostic();
    testProviderResultOkFalseWhenEmpty();
    testProviderResultOkTrueWhenValue();
    testProviderResultFailureFactory();
    testProviderResultMultipleDiagnostics();
    testDiagnosticSeverityToString();
    testProviderPolymorphism();

    if (g_failures > 0) {
        std::cerr << g_failures << " test(s) FAILED." << std::endl;
        return 1;
    }
    std::cout << "All temporal provider tests passed." << std::endl;
    return 0;
}
