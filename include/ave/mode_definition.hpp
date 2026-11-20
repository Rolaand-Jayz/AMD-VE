#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ave {

enum class PreviewBehavior {
    SplitWipe,
    SbsPreview,
    MotionPreview,
    DebugOverlay,
    OriginalOnly,
    EnhancedOnly
};

std::string toString(PreviewBehavior behavior);
std::optional<PreviewBehavior> previewBehaviorFromString(const std::string& value);

enum class OutputBehavior {
    NormalVideo,
    SbsVideo,
    InterpolatedVideo,
    DebugExport
};

std::string toString(OutputBehavior behavior);
std::optional<OutputBehavior> outputBehaviorFromString(const std::string& value);

struct EnhancementModeDefinition {
    std::string id;
    std::string label;
    std::string purpose;
    std::string defaultFamilyId;
    std::vector<std::string> defaultCapabilities;
    std::vector<std::string> visibleControls;
    PreviewBehavior previewBehavior = PreviewBehavior::SplitWipe;
    OutputBehavior outputBehavior = OutputBehavior::NormalVideo;
    bool experimental = false;
    bool advancedOnly = false;
    std::string cliProfileKey;
};

const std::vector<EnhancementModeDefinition>& builtinModeDefinitions();
const EnhancementModeDefinition* findModeDefinition(const std::string& id);
std::vector<const EnhancementModeDefinition*> visibleModeDefinitions();
std::vector<const EnhancementModeDefinition*> advancedModeDefinitions();

bool validateModeDefinition(const EnhancementModeDefinition& mode, std::string& error);

}  // namespace ave
