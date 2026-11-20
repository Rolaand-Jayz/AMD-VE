#include "ave/family_metadata.hpp"
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

std::string toString(ContentType ct) {
    switch (ct) {
        case ContentType::General:      return "general";
        case ContentType::LiveAction:   return "live_action";
        case ContentType::Anime:        return "anime";
        case ContentType::Archival:     return "archival";
        case ContentType::Stereo:       return "stereo";
        case ContentType::Motion:       return "motion";
        case ContentType::Experimental: return "experimental";
    }
    return "unknown";
}

std::optional<ContentType> contentTypeFromString(const std::string& value) {
    static const std::unordered_map<std::string, ContentType> map = {
        {"general",      ContentType::General},
        {"live_action",  ContentType::LiveAction},
        {"anime",        ContentType::Anime},
        {"archival",     ContentType::Archival},
        {"stereo",       ContentType::Stereo},
        {"motion",       ContentType::Motion},
        {"experimental", ContentType::Experimental},
    };
    auto it = map.find(normalizeId(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

std::string toString(Tier tier) {
    switch (tier) {
        case Tier::S:       return "S";
        case Tier::A:       return "A";
        case Tier::B:       return "B";
        case Tier::C:       return "C";
        case Tier::D:       return "D";
        case Tier::Unknown: return "unknown";
    }
    return "unknown";
}

std::optional<Tier> tierFromString(const std::string& value) {
    static const std::unordered_map<std::string, Tier> map = {
        {"s",       Tier::S},
        {"a",       Tier::A},
        {"b",       Tier::B},
        {"c",       Tier::C},
        {"d",       Tier::D},
        {"unknown", Tier::Unknown},
    };
    auto it = map.find(normalizeId(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

const std::vector<ModelFamilyMetadata>& builtinFamilyMetadata() {
    static const std::vector<ModelFamilyMetadata> families = {
        {
            "clearreality_fast", "ClearReality Fast",
            "Fast restoration of compressed live-action video",
            ContentType::LiveAction,
            Tier::A, Tier::B, Tier::A,
            true, false,
            {"restore_compression", "remove_artifacts", "denoise", "deblur"},
            {"restore_compression", "denoise"},
            false, false,
            {
                {"strength", "Enhancement Strength", 0.0f, 1.0f, 0.5f, 0.01f, "restore_compression"},
            },
            {},
            ""
        },
        {
            "openproteus_quality", "OpenProteus Quality",
            "High-quality general-purpose upscaling",
            ContentType::General,
            Tier::B, Tier::A, Tier::A,
            true, false,
            {"upscale"},
            {"upscale"},
            false, false,
            {},
            {},
            ""
        },
        {
            "anime_line_art", "Anime / Line Art",
            "Edge-aware enhancement for animation and drawn content",
            ContentType::Anime,
            Tier::B, Tier::A, Tier::B,
            true, false,
            {"denoise", "deblur", "upscale", "sharpen"},
            {"denoise", "upscale"},
            true, true,
            {
                {"strength", "Enhancement Strength", 0.0f, 1.0f, 0.5f, 0.01f, ""},
            },
            {},
            ""
        },
        {
            "depth_sbs", "Depth / SBS 3D",
            "Depth-based stereoscopic 3D conversion",
            ContentType::Stereo,
            Tier::C, Tier::B, Tier::C,
            true, false,
            {"stereo_3d"},
            {"stereo_3d"},
            false, false,
            {
                {"divergence", "3D Divergence", 0.0f, 2.0f, 1.0f, 0.01f, "stereo_3d"},
            },
            {},
            ""
        },
        {
            "motion_rife", "Motion / RIFE",
            "AI motion interpolation for smooth video playback",
            ContentType::Motion,
            Tier::A, Tier::A, Tier::A,
            true, false,
            {"interpolate"},
            {"interpolate"},
            false, false,
            {},
            {},
            ""
        },
        {
            "temporal_forge_experimental", "Temporal Forge Experimental",
            "Experimental multi-frame temporal enhancement",
            ContentType::Experimental,
            Tier::D, Tier::D, Tier::Unknown,
            false, true,
            {"restore_compression", "denoise", "upscale"},
            {"restore_compression", "denoise"},
            true, true,
            {
                {"strength", "Enhancement Strength", 0.0f, 1.0f, 0.5f, 0.01f, ""},
                {"motion_weight", "Motion Weight", 0.0f, 1.0f, 0.5f, 0.01f, ""},
            },
            {},
            "Experimental. Must not appear in production workflows unless explicitly enabled."
        },
        {
            "advanced_legacy", "Advanced / Legacy",
            "Manual model access for expert users",
            ContentType::General,
            Tier::Unknown, Tier::Unknown, Tier::Unknown,
            false, true,
            {},
            {},
            false, false,
            {},
            {},
            "Container for legacy and raw model access. Not a real family."
        },
    };
    return families;
}

const ModelFamilyMetadata* findFamilyMetadata(const std::string& familyId) {
    const auto& families = builtinFamilyMetadata();
    for (const auto& family : families) {
        if (family.familyId == familyId) return &family;
    }
    return nullptr;
}

std::vector<const ModelFamilyMetadata*> visibleFamilies() {
    std::vector<const ModelFamilyMetadata*> result;
    const auto& families = builtinFamilyMetadata();
    for (const auto& family : families) {
        if (family.visibleDefault) {
            result.push_back(&family);
        }
    }
    return result;
}

std::vector<const ModelFamilyMetadata*> advancedFamilies() {
    std::vector<const ModelFamilyMetadata*> result;
    const auto& families = builtinFamilyMetadata();
    for (const auto& family : families) {
        if (family.advancedOnly) {
            result.push_back(&family);
        }
    }
    return result;
}

bool validateFamilyMetadata(const ModelFamilyMetadata& family, std::string& error) {
    if (family.familyId.empty()) {
        error = "family_id must not be empty";
        return false;
    }
    if (family.familyName.empty()) {
        error = "family_name must not be empty for family '" + family.familyId + "'";
        return false;
    }
    if (family.displayPurpose.empty()) {
        error = "display_purpose must not be empty for family '" + family.familyId + "'";
        return false;
    }

    if (family.familyId != "advanced_legacy" && family.capabilities.empty()) {
        error = "capabilities must be non-empty for family '" + family.familyId +
                "' (only advanced_legacy may have empty capabilities)";
        return false;
    }

    for (const auto& safe : family.safeDefaults) {
        bool found = false;
        for (const auto& cap : family.capabilities) {
            if (cap == safe) { found = true; break; }
        }
        if (!found) {
            error = "safe_default '" + safe +
                    "' is not a subset of capabilities for family '" + family.familyId + "'";
            return false;
        }
    }

    if (family.selective && !family.fused) {
        error = "selective=true requires fused=true for family '" + family.familyId + "'";
        return false;
    }

    if (family.advancedOnly && family.visibleDefault) {
        error = "advanced_only=true conflicts with visible_default=true for family '" +
                family.familyId + "'";
        return false;
    }

    return true;
}

const ModelFamilyMetadata* familyForMode(const std::string& modeId) {
    const auto* mode = findModeDefinition(modeId);
    if (!mode) return nullptr;
    if (mode->defaultFamilyId.empty()) return nullptr;
    return findFamilyMetadata(mode->defaultFamilyId);
}

}  // namespace ave
