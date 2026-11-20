#include "ave/temporal_providers.hpp"

namespace ave {

std::string toString(ProviderDiagnostic::Severity severity) {
    switch (severity) {
        case ProviderDiagnostic::Severity::Info:    return "info";
        case ProviderDiagnostic::Severity::Warning: return "warning";
        case ProviderDiagnostic::Severity::Error:   return "error";
    }
    return "unknown";
}

ProviderResult<MotionField> NullMotionProvider::estimate(const TemporalFrameWindow&) {
    return ProviderResult<MotionField>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullMotionProvider: no motion estimation available (experimental placeholder)"
    });
}

ProviderResult<ImagePlane> NullDepthProvider::estimateDepth(const FrameRef&) {
    return ProviderResult<ImagePlane>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullDepthProvider: no depth estimation available (experimental placeholder)"
    });
}

ProviderResult<ImagePlane> NullMaskProvider::compute(const TemporalFramePacket&) {
    return ProviderResult<ImagePlane>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullMaskProvider: no mask computation available (experimental placeholder)"
    });
}

ProviderResult<ImagePlane> NullConfidenceProvider::computeConfidence(const TemporalFramePacket&) {
    return ProviderResult<ImagePlane>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullConfidenceProvider: no confidence computation available (experimental placeholder)"
    });
}

ProviderResult<FrameRef> NullTemporalReconstructor::reconstruct(const TemporalFramePacket&) {
    return ProviderResult<FrameRef>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullTemporalReconstructor: no temporal reconstruction available (experimental placeholder)"
    });
}

ProviderResult<FrameRef> NullMaskedAIRefiner::refine(
    const FrameRef&,
    const FrameRef&,
    const ImagePlane&,
    const ImagePlane&) {
    return ProviderResult<FrameRef>::failure({
        ProviderDiagnostic::Severity::Warning,
        "PROVIDER_NOT_IMPLEMENTED",
        "NullMaskedAIRefiner: no AI refinement available (experimental placeholder)"
    });
}

}  // namespace ave
