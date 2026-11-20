#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ave {

enum class ContentType {
    General,
    LiveAction,
    Anime,
    Archival,
    Stereo,
    Motion,
    Experimental
};

std::string toString(ContentType ct);
std::optional<ContentType> contentTypeFromString(const std::string& value);

enum class Tier {
    S, A, B, C, D, Unknown
};

std::string toString(Tier tier);
std::optional<Tier> tierFromString(const std::string& value);

struct StrengthControl {
    std::string id;
    std::string label;
    float min = 0.0f;
    float max = 1.0f;
    float defaultValue = 0.5f;
    float step = 0.01f;
    std::string capability;
};

struct ModelFamilyMetadata {
    std::string familyId;
    std::string familyName;
    std::string displayPurpose;
    ContentType contentType = ContentType::General;
    Tier speedTier = Tier::Unknown;
    Tier qualityTier = Tier::Unknown;
    Tier migraphxFit = Tier::Unknown;
    bool visibleDefault = false;
    bool advancedOnly = false;
    std::vector<std::string> capabilities;
    std::vector<std::string> safeDefaults;
    bool fused = false;
    bool selective = false;
    std::vector<StrengthControl> strengthControls;
    std::vector<std::string> modelIds;
    std::string notes;
};

const std::vector<ModelFamilyMetadata>& builtinFamilyMetadata();
const ModelFamilyMetadata* findFamilyMetadata(const std::string& familyId);
std::vector<const ModelFamilyMetadata*> visibleFamilies();
std::vector<const ModelFamilyMetadata*> advancedFamilies();

bool validateFamilyMetadata(const ModelFamilyMetadata& family, std::string& error);

const ModelFamilyMetadata* familyForMode(const std::string& modeId);

}  // namespace ave
