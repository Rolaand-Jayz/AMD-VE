#include "ave/fusion_planner.hpp"
#include "ave/family_metadata.hpp"
#include "ave/mode_definition.hpp"

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace ave {

namespace {

int orderGroup(const std::string& cap) {
    static const std::unordered_map<std::string, int> order = {
        {"restore_compression", 0}, {"remove_artifacts", 0}, {"denoise", 0},
        {"deblur", 0}, {"dehalo", 0},
        {"color_fix", 1},
        {"upscale", 2},
        {"sharpen", 3},
        {"stereo_3d", 4},
        {"interpolate", 5},
    };
    auto it = order.find(cap);
    return it != order.end() ? it->second : 100;
}

[[maybe_unused]] bool isSubset(const std::vector<std::string>& subset, const std::vector<std::string>& superset) {
    for (const auto& s : subset) {
        bool found = false;
        for (const auto& sp : superset) {
            if (sp == s) { found = true; break; }
        }
        if (!found) return false;
    }
    return true;
}

bool setsEqual(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    if (a.size() != b.size()) return false;
    auto ac = a; auto bc = b;
    std::sort(ac.begin(), ac.end());
    std::sort(bc.begin(), bc.end());
    return ac == bc;
}

bool familySupportsCapability(const ModelFamilyMetadata& family, const std::string& cap) {
    for (const auto& c : family.capabilities) {
        if (c == cap) return true;
    }
    return false;
}

std::vector<std::string> resolveCapabilities(const PipelineIntent& intent,
                                             const EnhancementModeDefinition& mode,
                                             const ModelFamilyMetadata& family,
                                             [[maybe_unused]] std::vector<PlannerDiagnostic>& diagnostics) {
    std::vector<std::string> caps;

    if (!intent.requestedCapabilities.empty()) {
        caps = intent.requestedCapabilities;
    } else if (!mode.defaultCapabilities.empty()) {
        caps = mode.defaultCapabilities;
    } else {
        caps = family.safeDefaults.empty() ? family.capabilities : family.safeDefaults;
    }

    std::sort(caps.begin(), caps.end(), [](const std::string& a, const std::string& b) {
        return orderGroup(a) < orderGroup(b);
    });

    return caps;
}

}  // namespace

std::string toString(PlanNode::Type type) {
    switch (type) {
        case PlanNode::Type::OrdinaryStage:             return "ordinary_stage";
        case PlanNode::Type::FusedAIGroup:              return "fused_ai_group";
        case PlanNode::Type::TemporalForgeExperimental: return "temporal_forge_experimental";
    }
    return "unknown";
}

std::string toString(PlannerDiagnostic::Severity severity) {
    switch (severity) {
        case PlannerDiagnostic::Severity::Info:    return "info";
        case PlannerDiagnostic::Severity::Warning: return "warning";
        case PlannerDiagnostic::Severity::Error:   return "error";
    }
    return "unknown";
}

int capabilityOrder(const std::string& capability) {
    return orderGroup(capability);
}

PlannedPipeline FusionPlanner::plan(const PipelineIntent& intent) const {
    PlannedPipeline result;

    if (intent.rawModelId.has_value() && !intent.rawModelId->empty()) {
        result.nodes.push_back({PlanNode::Type::OrdinaryStage, "", {}, {*intent.rawModelId}});
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "RAW_MODEL_ADVANCED_PATH",
            "Raw model selected: " + *intent.rawModelId + "; family fusion disabled."});
        return result;
    }

    const auto* mode = findModeDefinition(intent.modeId);
    if (!mode) {
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Error, "MODE_UNKNOWN",
            "Unknown mode id: " + intent.modeId});
        return result;
    }

    if (mode->experimental) {
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Warning, "MODE_EXPERIMENTAL_DISABLED",
            "Mode '" + intent.modeId + "' is experimental and disabled by default."});
    }

    std::string familyId = intent.familyId.empty() ? mode->defaultFamilyId : intent.familyId;
    if (familyId.empty()) {
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "NO_FAMILY",
            "No family specified and mode has no default; emitting empty plan."});
        return result;
    }

    const auto* family = findFamilyMetadata(familyId);
    if (!family) {
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Error, "FAMILY_NOT_FOUND",
            "Family not found: " + familyId});
        return result;
    }

    std::vector<std::string> caps = resolveCapabilities(intent, *mode, *family, result.diagnostics);

    for (const auto& cap : caps) {
        if (!familySupportsCapability(*family, cap)) {
            result.diagnostics.push_back({PlannerDiagnostic::Severity::Error,
                "FUSION_UNSUPPORTED_CAPABILITY",
                "Capability '" + cap + "' not supported by family '" + familyId + "'"});
        }
    }

    if (mode->id == "temporal_forge_experimental") {
        result.nodes.push_back({PlanNode::Type::TemporalForgeExperimental, familyId, caps, family->modelIds});
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "FUSION_APPLIED",
            "Temporal Forge experimental group for capabilities: " +
            (caps.empty() ? "(none)" : caps.front())});
        return result;
    }

    if (!family->fused) {
        for (const auto& cap : caps) {
            result.nodes.push_back({PlanNode::Type::OrdinaryStage, familyId, {cap}, family->modelIds});
        }
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "FUSION_NOT_SUPPORTED",
            "Family '" + familyId + "' does not support fusion; emitting ordinary stages."});
        return result;
    }

    if (family->selective) {
        std::vector<std::string> validCaps;
        for (const auto& cap : caps) {
            if (familySupportsCapability(*family, cap)) {
                validCaps.push_back(cap);
            }
        }
        if (!validCaps.empty()) {
            result.nodes.push_back({PlanNode::Type::FusedAIGroup, familyId, validCaps, family->modelIds});
            result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "FUSION_APPLIED",
                "Fused selective group for " + std::to_string(validCaps.size()) + " capabilities."});
        }
        return result;
    }

    if (setsEqual(caps, family->capabilities) ||
        (!family->safeDefaults.empty() && setsEqual(caps, family->safeDefaults))) {
        result.nodes.push_back({PlanNode::Type::FusedAIGroup, familyId, caps, family->modelIds});
        result.diagnostics.push_back({PlannerDiagnostic::Severity::Info, "FUSION_APPLIED",
            "Fused non-selective group: requested set matches full family capabilities."});
        return result;
    }

    result.diagnostics.push_back({PlannerDiagnostic::Severity::Warning,
        "FUSION_PARTIAL_NOT_SELECTIVE",
        "Family '" + familyId + "' is fused but non-selective and requested capabilities (" +
        std::to_string(caps.size()) + ") do not match the full fused set (" +
        std::to_string(family->capabilities.size()) + "). Falling back to ordinary stages."});
    result.diagnostics.push_back({PlannerDiagnostic::Severity::Info,
        "FUSION_FALLBACK_TO_ORDINARY_STAGES",
        "Emitting ordinary stage nodes for non-matching partial fusion request."});

    for (const auto& cap : caps) {
        if (familySupportsCapability(*family, cap)) {
            result.nodes.push_back({PlanNode::Type::OrdinaryStage, familyId, {cap}, family->modelIds});
        }
    }

    return result;
}

}  // namespace ave
