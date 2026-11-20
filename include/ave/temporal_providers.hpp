#pragma once

#include "ave/temporal_frame_types.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ave {

struct ProviderDiagnostic {
    enum class Severity { Info, Warning, Error };
    Severity severity = Severity::Info;
    std::string code;
    std::string message;
};

std::string toString(ProviderDiagnostic::Severity severity);

template <typename T>
struct ProviderResult {
    std::optional<T> value;
    std::vector<ProviderDiagnostic> diagnostics;

    bool ok() const { return value.has_value(); }

    static ProviderResult<T> success(T val) {
        return {std::move(val), {}};
    }

    static ProviderResult<T> failure(ProviderDiagnostic diag) {
        return {std::nullopt, {std::move(diag)}};
    }

    static ProviderResult<T> failure(std::vector<ProviderDiagnostic> diags) {
        return {std::nullopt, std::move(diags)};
    }
};

class MotionProvider {
  public:
    virtual ~MotionProvider() = default;
    virtual ProviderResult<MotionField> estimate(const TemporalFrameWindow& window) = 0;
};

class DepthProvider {
  public:
    virtual ~DepthProvider() = default;
    virtual ProviderResult<ImagePlane> estimateDepth(const FrameRef& frame) = 0;
};

class MaskProvider {
  public:
    virtual ~MaskProvider() = default;
    virtual ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) = 0;
};

class ConfidenceProvider {
  public:
    virtual ~ConfidenceProvider() = default;
    virtual ProviderResult<ImagePlane> computeConfidence(const TemporalFramePacket& packet) = 0;
};

class TemporalReconstructor {
  public:
    virtual ~TemporalReconstructor() = default;
    virtual ProviderResult<FrameRef> reconstruct(const TemporalFramePacket& packet) = 0;
};

class MaskedAIRefiner {
  public:
    virtual ~MaskedAIRefiner() = default;
    virtual ProviderResult<FrameRef> refine(
        const FrameRef& source,
        const FrameRef& temporalReconstruction,
        const ImagePlane& confidence,
        const ImagePlane& aiAllowedMask) = 0;
};

class NullMotionProvider final : public MotionProvider {
  public:
    ProviderResult<MotionField> estimate(const TemporalFrameWindow& window) override;
};

class NullDepthProvider final : public DepthProvider {
  public:
    ProviderResult<ImagePlane> estimateDepth(const FrameRef& frame) override;
};

class NullMaskProvider final : public MaskProvider {
  public:
    ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) override;
};

class NullConfidenceProvider final : public ConfidenceProvider {
  public:
    ProviderResult<ImagePlane> computeConfidence(const TemporalFramePacket& packet) override;
};

class NullTemporalReconstructor final : public TemporalReconstructor {
  public:
    ProviderResult<FrameRef> reconstruct(const TemporalFramePacket& packet) override;
};

class NullMaskedAIRefiner final : public MaskedAIRefiner {
  public:
    ProviderResult<FrameRef> refine(
        const FrameRef& source,
        const FrameRef& temporalReconstruction,
        const ImagePlane& confidence,
        const ImagePlane& aiAllowedMask) override;
};

}  // namespace ave
