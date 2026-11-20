#include "ave/reference_providers.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <numeric>

namespace ave {

namespace {

float sadBlock(const uint8_t* a, int strideA,
               const uint8_t* b, int strideB,
               int blockSize) {
    float sum = 0.0f;
    for (int y = 0; y < blockSize; ++y) {
        for (int x = 0; x < blockSize; ++x) {
            float diff = static_cast<float>(a[y * strideA + x]) -
                         static_cast<float>(b[y * strideB + x]);
            sum += std::abs(diff);
        }
    }
    return sum;
}

ImagePlane toGrayscale(const ImagePlane& src) {
    ImagePlane gray;
    gray.width = src.width;
    gray.height = src.height;
    gray.channels = 1;
    gray.pixels.resize(static_cast<size_t>(src.width) * src.height);
    for (int y = 0; y < src.height; ++y) {
        for (int x = 0; x < src.width; ++x) {
            int idx = (y * src.width + x) * src.channels;
            float val = 0.0f;
            for (int c = 0; c < src.channels; ++c) {
                val += static_cast<float>(src.pixels[idx + c]);
            }
            gray.pixels[y * src.width + x] = static_cast<uint8_t>(val / src.channels);
        }
    }
    return gray;
}

}  // namespace

CpuBlockMotionProvider::CpuBlockMotionProvider(BlockMotionConfig config)
    : config_(std::move(config)) {}

ProviderResult<MotionField> CpuBlockMotionProvider::estimate(
    const TemporalFrameWindow& window) {
    if (window.history.empty()) {
        return ProviderResult<MotionField>::failure({
            ProviderDiagnostic::Severity::Warning,
            "NO_HISTORY_FRAMES",
            "CpuBlockMotionProvider: no history frames available for motion estimation"
        });
    }

    const FrameRef& prev = window.history.back();
    const FrameRef& curr = window.current;

    int fieldW = (curr.width + config_.blockSize - 1) / config_.blockSize;
    int fieldH = (curr.height + config_.blockSize - 1) / config_.blockSize;

    MotionField field;
    field.width = fieldW;
    field.height = fieldH;
    field.vectors.resize(static_cast<size_t>(fieldW) * fieldH * 2, 0.0f);

    return ProviderResult<MotionField>::success(std::move(field));
}

float CpuBlockMotionProvider::computeSceneChangeMetric(
    const ImagePlane& frameA,
    const ImagePlane& frameB) {
    if (frameA.width != frameB.width || frameA.height != frameB.height) {
        return 999.0f;
    }
    if (frameA.pixels.empty() || frameB.pixels.empty()) {
        return 0.0f;
    }

    ImagePlane grayA = (frameA.channels == 1) ? frameA : toGrayscale(frameA);
    ImagePlane grayB = (frameB.channels == 1) ? frameB : toGrayscale(frameB);

    int step = std::max(1, grayA.width / 64);
    float totalDiff = 0.0f;
    int count = 0;

    for (int y = 0; y < grayA.height; y += step) {
        for (int x = 0; x < grayA.width; x += step) {
            float diff = std::abs(
                static_cast<float>(grayA.pixels[y * grayA.width + x]) -
                static_cast<float>(grayB.pixels[y * grayB.width + x]));
            totalDiff += diff;
            ++count;
        }
    }

    return (count > 0) ? totalDiff / static_cast<float>(count) : 0.0f;
}

PseudoDepthProvider::PseudoDepthProvider(int defaultWidth, int defaultHeight)
    : defaultWidth_(defaultWidth), defaultHeight_(defaultHeight) {}

ProviderResult<ImagePlane> PseudoDepthProvider::estimateDepth(const FrameRef& frame) {
    int w = (frame.width > 0) ? defaultWidth_ : defaultWidth_;
    int h = (frame.height > 0) ? defaultHeight_ : defaultHeight_;

    ImagePlane depth;
    depth.width = w;
    depth.height = h;
    depth.channels = 1;
    depth.pixels.resize(static_cast<size_t>(w) * h);

    for (int y = 0; y < h; ++y) {
        float normalizedY = static_cast<float>(y) / static_cast<float>(h);
        uint8_t val = static_cast<uint8_t>(normalizedY * 200.0f + 28.0f);
        for (int x = 0; x < w; ++x) {
            float normalizedX = static_cast<float>(x) / static_cast<float>(w);
            float centerDist = std::abs(normalizedX - 0.5f) * 2.0f;
            float depthVal = normalizedY * 0.6f + (1.0f - centerDist) * 0.4f;
            int finalVal = static_cast<int>(depthVal * 255.0f);
            depth.pixels[y * w + x] = static_cast<uint8_t>(
                std::min(255, std::max(0, finalVal)));
        }
    }

    std::vector<ProviderDiagnostic> diags;
    diags.push_back({
        ProviderDiagnostic::Severity::Info,
        "PSEUDO_DEPTH",
        "PseudoDepthProvider: using vertical-gradient pseudo-depth (no depth model)"
    });

    ProviderResult<ImagePlane> result;
    result.value = std::move(depth);
    result.diagnostics = std::move(diags);
    return result;
}

DepthEdgeExtractor::DepthEdgeExtractor(float threshold)
    : threshold_(threshold) {}

ProviderResult<ImagePlane> DepthEdgeExtractor::compute(
    const TemporalFramePacket& packet) {
    if (!packet.depth.has_value()) {
        return ProviderResult<ImagePlane>::failure({
            ProviderDiagnostic::Severity::Warning,
            "NO_DEPTH_DATA",
            "DepthEdgeExtractor: no depth data in packet"
        });
    }

    auto edges = extractEdges(*packet.depth, threshold_);
    return ProviderResult<ImagePlane>::success(std::move(edges));
}

ImagePlane DepthEdgeExtractor::extractEdges(
    const ImagePlane& depth,
    float threshold) {
    ImagePlane edges;
    edges.width = depth.width;
    edges.height = depth.height;
    edges.channels = 1;
    edges.pixels.resize(static_cast<size_t>(depth.width) * depth.height, 0);

    for (int y = 1; y < depth.height - 1; ++y) {
        for (int x = 1; x < depth.width - 1; ++x) {
            float center = static_cast<float>(depth.pixels[y * depth.width + x]);
            float right = static_cast<float>(depth.pixels[y * depth.width + x + 1]);
            float below = static_cast<float>(depth.pixels[(y + 1) * depth.width + x]);

            float dx = std::abs(center - right) / 255.0f;
            float dy = std::abs(center - below) / 255.0f;
            float grad = std::sqrt(dx * dx + dy * dy);

            edges.pixels[y * edges.width + x] =
                (grad > threshold) ? 255 : 0;
        }
    }

    return edges;
}

ReactiveMaskProvider::ReactiveMaskProvider(float motionThreshold)
    : motionThreshold_(motionThreshold) {}

ProviderResult<ImagePlane> ReactiveMaskProvider::compute(
    const TemporalFramePacket& packet) {
    int w = packet.window.current.width;
    int h = packet.window.current.height;
    if (w <= 0 || h <= 0) {
        return ProviderResult<ImagePlane>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_DIMENSIONS",
            "ReactiveMaskProvider: invalid frame dimensions"
        });
    }

    ImagePlane mask;
    mask.width = w;
    mask.height = h;
    mask.channels = 1;
    mask.pixels.resize(static_cast<size_t>(w) * h, 0);

    if (packet.motionPrevToCurrent.has_value()) {
        const auto& motion = *packet.motionPrevToCurrent;
        for (int by = 0; by < motion.height; ++by) {
            for (int bx = 0; bx < motion.width; ++bx) {
                float vx = motion.vx(bx, by);
                float vy = motion.vy(bx, by);
                float mag = std::sqrt(vx * vx + vy * vy);
                uint8_t val = (mag > motionThreshold_) ? 255 : 0;
                int x0 = bx * (w / motion.width);
                int y0 = by * (h / motion.height);
                int x1 = std::min(x0 + (w / motion.width), w);
                int y1 = std::min(y0 + (h / motion.height), h);
                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        mask.pixels[y * w + x] = val;
                    }
                }
            }
        }
    }

    return ProviderResult<ImagePlane>::success(std::move(mask));
}

OcclusionMaskProvider::OcclusionMaskProvider(float motionMagnitudeThreshold)
    : motionMagnitudeThreshold_(motionMagnitudeThreshold) {}

ProviderResult<ImagePlane> OcclusionMaskProvider::compute(
    const TemporalFramePacket& packet) {
    int w = packet.window.current.width;
    int h = packet.window.current.height;
    if (w <= 0 || h <= 0) {
        return ProviderResult<ImagePlane>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_DIMENSIONS",
            "OcclusionMaskProvider: invalid frame dimensions"
        });
    }

    ImagePlane mask;
    mask.width = w;
    mask.height = h;
    mask.channels = 1;
    mask.pixels.resize(static_cast<size_t>(w) * h, 0);

    if (packet.motionPrevToCurrent.has_value()) {
        const auto& motion = *packet.motionPrevToCurrent;
        for (int by = 0; by < motion.height; ++by) {
            for (int bx = 0; bx < motion.width; ++bx) {
                float vx = motion.vx(bx, by);
                float vy = motion.vy(bx, by);
                float mag = std::sqrt(vx * vx + vy * vy);
                uint8_t val = (mag > motionMagnitudeThreshold_) ? 255 : 0;
                int x0 = bx * (w / motion.width);
                int y0 = by * (h / motion.height);
                int x1 = std::min(x0 + (w / motion.width), w);
                int y1 = std::min(y0 + (h / motion.height), h);
                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        mask.pixels[y * w + x] = val;
                    }
                }
            }
        }
    }

    return ProviderResult<ImagePlane>::success(std::move(mask));
}

ProviderResult<ImagePlane> ConfidenceMapProvider::computeConfidence(
    const TemporalFramePacket& packet) {
    int w = packet.window.current.width;
    int h = packet.window.current.height;
    if (w <= 0 || h <= 0) {
        return ProviderResult<ImagePlane>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_DIMENSIONS",
            "ConfidenceMapProvider: invalid frame dimensions"
        });
    }

    ImagePlane conf;
    conf.width = w;
    conf.height = h;
    conf.channels = 1;
    conf.pixels.resize(static_cast<size_t>(w) * h, 255);

    if (packet.window.sceneResetBeforeCurrent) {
        std::fill(conf.pixels.begin(), conf.pixels.end(),
                  static_cast<uint8_t>(0));
        return ProviderResult<ImagePlane>::success(std::move(conf));
    }

    if (packet.motionPrevToCurrent.has_value()) {
        const auto& motion = *packet.motionPrevToCurrent;
        for (int by = 0; by < motion.height; ++by) {
            for (int bx = 0; bx < motion.width; ++bx) {
                float vx = motion.vx(bx, by);
                float vy = motion.vy(bx, by);
                float mag = std::sqrt(vx * vx + vy * vy);
                float confidence = std::max(0.0f, 1.0f - mag / 16.0f);
                uint8_t val = static_cast<uint8_t>(confidence * 255.0f);
                int x0 = bx * (w / motion.width);
                int y0 = by * (h / motion.height);
                int x1 = std::min(x0 + (w / motion.width), w);
                int y1 = std::min(y0 + (h / motion.height), h);
                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        conf.pixels[y * w + x] = val;
                    }
                }
            }
        }
    }

    return ProviderResult<ImagePlane>::success(std::move(conf));
}

AiAllowedMaskProvider::AiAllowedMaskProvider(float confidenceThreshold)
    : confidenceThreshold_(confidenceThreshold) {}

ProviderResult<ImagePlane> AiAllowedMaskProvider::compute(
    const TemporalFramePacket& packet) {
    int w = packet.window.current.width;
    int h = packet.window.current.height;
    if (w <= 0 || h <= 0) {
        return ProviderResult<ImagePlane>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_DIMENSIONS",
            "AiAllowedMaskProvider: invalid frame dimensions"
        });
    }

    if (!packet.confidenceMap.has_value()) {
        ImagePlane fullAllow;
        fullAllow.width = w;
        fullAllow.height = h;
        fullAllow.channels = 1;
        fullAllow.pixels.resize(static_cast<size_t>(w) * h, 255);
        return ProviderResult<ImagePlane>::success(std::move(fullAllow));
    }

    const auto& confMap = *packet.confidenceMap;
    ImagePlane mask;
    mask.width = w;
    mask.height = h;
    mask.channels = 1;
    mask.pixels.resize(static_cast<size_t>(w) * h);

    for (int i = 0; i < w * h; ++i) {
        float conf = static_cast<float>(confMap.pixels[i]) / 255.0f;
        mask.pixels[i] = (conf < confidenceThreshold_) ? 255 : 0;
    }

    return ProviderResult<ImagePlane>::success(std::move(mask));
}

TemporalHistoryCache::TemporalHistoryCache(int maxHistoryFrames)
    : maxFrames_(maxHistoryFrames) {}

void TemporalHistoryCache::addFrame(
    const FrameRef& frame,
    const Rational& timestamp,
    const MotionField& motionField) {
    entries_.push_back({frame, timestamp, motionField});
    while (static_cast<int>(entries_.size()) > maxFrames_) {
        entries_.erase(entries_.begin());
    }
    sceneReset_ = false;
}

void TemporalHistoryCache::clear() {
    entries_.clear();
    sceneReset_ = false;
}

void TemporalHistoryCache::markSceneReset() {
    clear();
    sceneReset_ = true;
}

const std::vector<TemporalHistoryEntry>& TemporalHistoryCache::entries() const {
    return entries_;
}

bool TemporalHistoryCache::hadSceneReset() const {
    return sceneReset_;
}

int TemporalHistoryCache::maxFrames() const {
    return maxFrames_;
}

CpuTemporalReconstructor::CpuTemporalReconstructor(float blendWeight)
    : blendWeight_(blendWeight) {}

ProviderResult<FrameRef> CpuTemporalReconstructor::reconstruct(
    const TemporalFramePacket& packet) {
    if (!packet.window.current.isValid()) {
        return ProviderResult<FrameRef>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_CURRENT_FRAME",
            "CpuTemporalReconstructor: current frame is invalid"
        });
    }

    if (packet.window.sceneResetBeforeCurrent || packet.window.history.empty()) {
        return ProviderResult<FrameRef>::success(packet.window.current);
    }

    return ProviderResult<FrameRef>::success(packet.window.current);
}

ConfidenceBlendedRefiner::ConfidenceBlendedRefiner(float temporalWeight)
    : temporalWeight_(temporalWeight) {}

ProviderResult<FrameRef> ConfidenceBlendedRefiner::refine(
    const FrameRef& source,
    const FrameRef& temporalReconstruction,
    const ImagePlane& confidence,
    const ImagePlane& aiAllowedMask) {
    if (!source.isValid()) {
        return ProviderResult<FrameRef>::failure({
            ProviderDiagnostic::Severity::Error,
            "INVALID_SOURCE",
            "ConfidenceBlendedRefiner: source frame is invalid"
        });
    }

    return ProviderResult<FrameRef>::success(source);
}

}  // namespace ave
