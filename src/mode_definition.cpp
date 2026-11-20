#include "ave/mode_definition.hpp"

#include <algorithm>
#include <unordered_map>

namespace ave {

namespace {

std::string normalizeId(const std::string& input) {
    std::string out;
    for (char ch : input) {
        if (ch == '-' || ch == ' ') {
            out.push_back('_');
        } else {
            out.push_back(static_cast<char>(tolower(static_cast<unsigned char>(ch))));
        }
    }
    return out;
}

}  // namespace

std::string toString(PreviewBehavior behavior) {
    switch (behavior) {
        case PreviewBehavior::SplitWipe:    return "split_wipe";
        case PreviewBehavior::SbsPreview:   return "sbs_preview";
        case PreviewBehavior::MotionPreview:return "motion_preview";
        case PreviewBehavior::DebugOverlay: return "debug_overlay";
        case PreviewBehavior::OriginalOnly: return "original_only";
        case PreviewBehavior::EnhancedOnly: return "enhanced_only";
    }
    return "unknown";
}

std::optional<PreviewBehavior> previewBehaviorFromString(const std::string& value) {
    static const std::unordered_map<std::string, PreviewBehavior> map = {
        {"split_wipe",     PreviewBehavior::SplitWipe},
        {"sbs_preview",    PreviewBehavior::SbsPreview},
        {"motion_preview", PreviewBehavior::MotionPreview},
        {"debug_overlay",  PreviewBehavior::DebugOverlay},
        {"original_only",  PreviewBehavior::OriginalOnly},
        {"enhanced_only",  PreviewBehavior::EnhancedOnly},
    };
    auto it = map.find(normalizeId(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

std::string toString(OutputBehavior behavior) {
    switch (behavior) {
        case OutputBehavior::NormalVideo:       return "normal_video";
        case OutputBehavior::SbsVideo:          return "sbs_video";
        case OutputBehavior::InterpolatedVideo: return "interpolated_video";
        case OutputBehavior::DebugExport:       return "debug_export";
    }
    return "unknown";
}

std::optional<OutputBehavior> outputBehaviorFromString(const std::string& value) {
    static const std::unordered_map<std::string, OutputBehavior> map = {
        {"normal_video",       OutputBehavior::NormalVideo},
        {"sbs_video",          OutputBehavior::SbsVideo},
        {"interpolated_video", OutputBehavior::InterpolatedVideo},
        {"debug_export",       OutputBehavior::DebugExport},
    };
    auto it = map.find(normalizeId(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

const std::vector<EnhancementModeDefinition>& builtinModeDefinitions() {
    static const std::vector<EnhancementModeDefinition> modes = {
        {
            "restore_video", "Restore Video",
            "Remove compression artifacts, noise, and blur from live-action footage",
            "clearreality_fast",
            {"restore_compression", "remove_artifacts", "denoise", "deblur"},
            {"strength", "preview_split"},
            PreviewBehavior::SplitWipe, OutputBehavior::NormalVideo,
            false, false, "restore"
        },
        {
            "upscale_video", "Upscale Video",
            "Increase resolution with AI detail synthesis",
            "openproteus_quality",
            {"upscale"},
            {"target_width", "target_height", "preview_split"},
            PreviewBehavior::SplitWipe, OutputBehavior::NormalVideo,
            false, false, "upscale"
        },
        {
            "anime_line_art", "Anime / Line Art",
            "Enhance animation and drawn content with edge-aware processing",
            "anime_line_art",
            {"denoise", "deblur", "upscale", "sharpen"},
            {"strength", "preview_split"},
            PreviewBehavior::SplitWipe, OutputBehavior::NormalVideo,
            false, false, "anime"
        },
        {
            "stereo_3d_sbs", "2D to 3D SBS",
            "Convert 2D video to side-by-side stereoscopic 3D",
            "depth_sbs",
            {"stereo_3d"},
            {"divergence", "preview_sbs"},
            PreviewBehavior::SbsPreview, OutputBehavior::SbsVideo,
            false, false, "stereo3d"
        },
        {
            "interpolate_motion", "Interpolate / Smooth Motion",
            "Increase frame rate with AI motion interpolation",
            "motion_rife",
            {"interpolate"},
            {"target_fps", "scene_detect", "preview_motion"},
            PreviewBehavior::MotionPreview, OutputBehavior::InterpolatedVideo,
            false, false, "interpolate"
        },
        {
            "advanced_legacy", "Advanced / Legacy",
            "Manual stage-by-stage control using individual models and filters",
            "",
            {},
            {"stages", "model", "backend", "filter"},
            PreviewBehavior::SplitWipe, OutputBehavior::NormalVideo,
            false, true, "advanced"
        },
        {
            "temporal_forge_experimental", "Temporal Forge",
            "Experimental multi-frame temporal enhancement (requires explicit opt-in)",
            "temporal_forge_experimental",
            {"restore_compression", "denoise", "upscale"},
            {"strength", "motion_weight", "preview_debug"},
            PreviewBehavior::DebugOverlay, OutputBehavior::DebugExport,
            true, false, "temporal_forge"
        },
    };
    return modes;
}

const EnhancementModeDefinition* findModeDefinition(const std::string& id) {
    const auto& modes = builtinModeDefinitions();
    for (const auto& mode : modes) {
        if (mode.id == id) return &mode;
    }
    return nullptr;
}

std::vector<const EnhancementModeDefinition*> visibleModeDefinitions() {
    std::vector<const EnhancementModeDefinition*> result;
    const auto& modes = builtinModeDefinitions();
    for (const auto& mode : modes) {
        if (!mode.advancedOnly && !mode.experimental) {
            result.push_back(&mode);
        }
    }
    return result;
}

std::vector<const EnhancementModeDefinition*> advancedModeDefinitions() {
    std::vector<const EnhancementModeDefinition*> result;
    const auto& modes = builtinModeDefinitions();
    for (const auto& mode : modes) {
        if (mode.advancedOnly || mode.experimental) {
            result.push_back(&mode);
        }
    }
    return result;
}

bool validateModeDefinition(const EnhancementModeDefinition& mode, std::string& error) {
    if (mode.id.empty()) {
        error = "mode id must not be empty";
        return false;
    }
    if (mode.label.empty()) {
        error = "mode label must not be empty for id '" + mode.id + "'";
        return false;
    }
    if (mode.purpose.empty()) {
        error = "mode purpose must not be empty for id '" + mode.id + "'";
        return false;
    }
    if (!mode.advancedOnly && !mode.experimental && mode.defaultFamilyId.empty()) {
        error = "non-advanced, non-experimental mode '" + mode.id +
                "' must have a default_family_id";
        return false;
    }
    if (!mode.advancedOnly && !mode.experimental && mode.cliProfileKey.empty()) {
        error = "non-advanced, non-experimental mode '" + mode.id +
                "' must have a cli_profile_key";
        return false;
    }
    return true;
}

}  // namespace ave
