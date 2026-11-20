#pragma once

#include "ave/temporal_providers.hpp"

#include <cstdint>
#include <functional>

namespace ave {

struct BlockMotionConfig {
    int blockSize = 8;
    int searchRadius = 16;
    float sceneChangeThreshold = 30.0f;
    bool enableSubPixel = false;
};

class CpuBlockMotionProvider final : public MotionProvider {
  public:
    explicit CpuBlockMotionProvider(BlockMotionConfig config = {});

    ProviderResult<MotionField> estimate(const TemporalFrameWindow& window) override;

    static float computeSceneChangeMetric(
        const ImagePlane& frameA,
        const ImagePlane& frameB);

  private:
    BlockMotionConfig config_;
};

class PseudoDepthProvider final : public DepthProvider {
  public:
    explicit PseudoDepthProvider(int defaultWidth = 384, int defaultHeight = 384);

    ProviderResult<ImagePlane> estimateDepth(const FrameRef& frame) override;

  private:
    int defaultWidth_;
    int defaultHeight_;
};

class DepthEdgeExtractor final : public MaskProvider {
  public:
    explicit DepthEdgeExtractor(float threshold = 0.1f);

    ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) override;

    static ImagePlane extractEdges(
        const ImagePlane& depth,
        float threshold);

  private:
    float threshold_;
};

class ReactiveMaskProvider final : public MaskProvider {
  public:
    explicit ReactiveMaskProvider(float motionThreshold = 0.05f);

    ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) override;

  private:
    float motionThreshold_;
};

class OcclusionMaskProvider final : public MaskProvider {
  public:
    explicit OcclusionMaskProvider(float motionMagnitudeThreshold = 4.0f);

    ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) override;

  private:
    float motionMagnitudeThreshold_;
};

class ConfidenceMapProvider final : public ConfidenceProvider {
  public:
    ProviderResult<ImagePlane> computeConfidence(
        const TemporalFramePacket& packet) override;
};

class AiAllowedMaskProvider final : public MaskProvider {
  public:
    explicit AiAllowedMaskProvider(float confidenceThreshold = 0.3f);

    ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) override;

  private:
    float confidenceThreshold_;
};

struct TemporalHistoryEntry {
    FrameRef frame;
    Rational timestamp;
    MotionField motionToCurrent;
};

class TemporalHistoryCache {
  public:
    explicit TemporalHistoryCache(int maxHistoryFrames = 5);

    void addFrame(const FrameRef& frame, const Rational& timestamp,
                  const MotionField& motionField);
    void clear();
    void markSceneReset();

    const std::vector<TemporalHistoryEntry>& entries() const;
    bool hadSceneReset() const;
    int maxFrames() const;

  private:
    std::vector<TemporalHistoryEntry> entries_;
    int maxFrames_;
    bool sceneReset_ = false;
};

class CpuTemporalReconstructor final : public TemporalReconstructor {
  public:
    explicit CpuTemporalReconstructor(float blendWeight = 0.5f);

    ProviderResult<FrameRef> reconstruct(
        const TemporalFramePacket& packet) override;

  private:
    float blendWeight_;
};

class ConfidenceBlendedRefiner final : public MaskedAIRefiner {
  public:
    explicit ConfidenceBlendedRefiner(float temporalWeight = 0.7f);

    ProviderResult<FrameRef> refine(
        const FrameRef& source,
        const FrameRef& temporalReconstruction,
        const ImagePlane& confidence,
        const ImagePlane& aiAllowedMask) override;

  private:
    float temporalWeight_;
};

}  // namespace ave
