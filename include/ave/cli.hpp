#pragma once

#include <optional>
#include <string>

#include "ave/enhancement_profile.hpp"
#include "ave/job.hpp"
#include "ave/temporal_frame_types.hpp"

namespace ave {

struct CliResult {
    bool shouldExit = false;
    int exitCode = 0;
    bool listBackends = false;
    bool listModes = false;
    std::optional<std::string> saveProfilePath;
    std::optional<EnhancementProfile> profile;
    std::optional<TemporalForgePreset> temporalPreset;
    std::optional<VideoJob> job;
};

CliResult parseCli(int argc, char** argv);
void printUsage(const std::string& executableName);

}  // namespace ave
