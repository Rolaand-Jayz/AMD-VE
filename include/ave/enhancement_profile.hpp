#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "ave/fusion_planner.hpp"

namespace ave {

struct EnhancementProfile {
    std::string modeId;
    std::string familyId;
    std::optional<std::string> rawModelId;
    std::vector<std::string> capabilities;
    std::map<std::string, double> strengthValues;
    std::string backendPreference;
    std::optional<std::string> precision;
    std::string outputPath;
    int schemaVersion = 1;

    [[nodiscard]] bool isValid(std::string& error) const;
    [[nodiscard]] bool isEmpty() const;
};

std::string serializeProfile(const EnhancementProfile& profile);
std::optional<EnhancementProfile> deserializeProfile(const std::string& text, std::string& error);

std::optional<EnhancementProfile> loadProfileFromFile(const std::string& path, std::string& error);
bool saveProfileToFile(const std::string& path, const EnhancementProfile& profile, std::string& error);

PipelineIntent profileToPipelineIntent(const EnhancementProfile& profile);

void printModesList();
void printProfileSummary(const EnhancementProfile& profile);

}  // namespace ave
