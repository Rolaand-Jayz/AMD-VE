#include "ave/enhancement_profile.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "ave/mode_definition.hpp"
#include "ave/family_metadata.hpp"

namespace ave {

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> splitCsv(const std::string& value) {
    std::vector<std::string> result;
    std::istringstream stream(value);
    std::string token;
    while (std::getline(stream, token, ',')) {
        token = trim(token);
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}

std::string joinCsv(const std::vector<std::string>& values) {
    std::string result;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) result += ',';
        result += values[i];
    }
    return result;
}

std::string escapeValue(const std::string& value) {
    if (value.find_first_of("=\n\r") == std::string::npos && !value.empty()) {
        return value;
    }
    std::string escaped;
    escaped.push_back('"');
    for (char ch : value) {
        if (ch == '"') escaped.push_back('"');
        escaped.push_back(ch);
    }
    escaped.push_back('"');
    return escaped;
}

std::string unescapeValue(const std::string& value) {
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        return value;
    }
    std::string result;
    for (size_t i = 1; i + 1 < value.size(); ++i) {
        if (value[i] == '"' && i + 1 < value.size() && value[i + 1] == '"') {
            result.push_back('"');
            ++i;
        } else {
            result.push_back(value[i]);
        }
    }
    return result;
}

bool parseLine(const std::string& line, std::string& key, std::string& value) {
    auto pos = line.find('=');
    if (pos == std::string::npos) return false;
    key = trim(line.substr(0, pos));
    value = unescapeValue(trim(line.substr(pos + 1)));
    return !key.empty();
}

}  // namespace

bool EnhancementProfile::isValid(std::string& error) const {
    if (modeId.empty() && familyId.empty() && !rawModelId.has_value()) {
        error = "Profile must specify at least mode, family, or raw model";
        return false;
    }

    if (!modeId.empty()) {
        auto mode = findModeDefinition(modeId);
        if (mode == nullptr) {
            error = "Invalid mode: " + modeId;
            return false;
        }
    }

    if (!familyId.empty()) {
        auto family = findFamilyMetadata(familyId);
        if (family == nullptr) {
            error = "Invalid family: " + familyId;
            return false;
        }
    }

    return true;
}

bool EnhancementProfile::isEmpty() const {
    return modeId.empty() && familyId.empty() && !rawModelId.has_value()
        && capabilities.empty() && strengthValues.empty();
}

std::string serializeProfile(const EnhancementProfile& profile) {
    std::ostringstream out;
    out << "schema_version=" << profile.schemaVersion << "\n";
    if (!profile.modeId.empty()) {
        out << "mode=" << escapeValue(profile.modeId) << "\n";
    }
    if (!profile.familyId.empty()) {
        out << "family=" << escapeValue(profile.familyId) << "\n";
    }
    if (profile.rawModelId.has_value()) {
        out << "raw_model=" << escapeValue(*profile.rawModelId) << "\n";
    }
    if (!profile.capabilities.empty()) {
        out << "capabilities=" << escapeValue(joinCsv(profile.capabilities)) << "\n";
    }
    if (!profile.strengthValues.empty()) {
        std::ostringstream strengths;
        bool first = true;
        for (const auto& [k, v] : profile.strengthValues) {
            if (!first) strengths << ',';
            strengths << k << ':' << v;
            first = false;
        }
        out << "strengths=" << escapeValue(strengths.str()) << "\n";
    }
    if (!profile.backendPreference.empty()) {
        out << "backend=" << escapeValue(profile.backendPreference) << "\n";
    }
    if (profile.precision.has_value()) {
        out << "precision=" << escapeValue(*profile.precision) << "\n";
    }
    if (!profile.outputPath.empty()) {
        out << "output=" << escapeValue(profile.outputPath) << "\n";
    }
    return out.str();
}

std::optional<EnhancementProfile> deserializeProfile(const std::string& text, std::string& error) {
    EnhancementProfile profile;
    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::string key, value;
        if (!parseLine(line, key, value)) {
            error = "Malformed line: " + line;
            return std::nullopt;
        }

        if (key == "schema_version") {
            try { profile.schemaVersion = std::stoi(value); }
            catch (...) {
                error = "Invalid schema_version: " + value;
                return std::nullopt;
            }
        } else if (key == "mode") {
            profile.modeId = value;
        } else if (key == "family") {
            profile.familyId = value;
        } else if (key == "raw_model") {
            profile.rawModelId = value;
        } else if (key == "capabilities") {
            profile.capabilities = splitCsv(value);
        } else if (key == "strengths") {
            auto pairs = splitCsv(value);
            for (const auto& pair : pairs) {
                auto colonPos = pair.find(':');
                if (colonPos == std::string::npos) continue;
                std::string k = trim(pair.substr(0, colonPos));
                try {
                    double v = std::stod(trim(pair.substr(colonPos + 1)));
                    profile.strengthValues[k] = v;
                } catch (...) {}
            }
        } else if (key == "backend") {
            profile.backendPreference = value;
        } else if (key == "precision") {
            profile.precision = value;
        } else if (key == "output") {
            profile.outputPath = value;
        }
    }

    if (!profile.isValid(error)) {
        return std::nullopt;
    }

    return profile;
}

std::optional<EnhancementProfile> loadProfileFromFile(const std::string& path, std::string& error) {
    std::ifstream file(path);
    if (!file.is_open()) {
        error = "Cannot open profile file: " + path;
        return std::nullopt;
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return deserializeProfile(contents.str(), error);
}

bool saveProfileToFile(const std::string& path, const EnhancementProfile& profile, std::string& error) {
    std::string validationError;
    if (!profile.isValid(validationError)) {
        error = "Cannot save invalid profile: " + validationError;
        return false;
    }
    std::ofstream file(path);
    if (!file.is_open()) {
        error = "Cannot write profile file: " + path;
        return false;
    }
    file << serializeProfile(profile);
    return true;
}

PipelineIntent profileToPipelineIntent(const EnhancementProfile& profile) {
    PipelineIntent intent;
    intent.modeId = profile.modeId;
    intent.familyId = profile.familyId;
    intent.rawModelId = profile.rawModelId;
    intent.requestedCapabilities = profile.capabilities;
    intent.strengthValues = profile.strengthValues;
    return intent;
}

void printModesList() {
    std::cout << "Available modes:\n\n";
    for (const auto& mode : builtinModeDefinitions()) {
        std::cout << "  " << mode.id << "\n"
                  << "    " << mode.label << "\n"
                  << "    " << mode.purpose << "\n"
                  << "    Default family: " << mode.defaultFamilyId << "\n";
        if (mode.experimental) {
            std::cout << "    (experimental — must be explicitly enabled)\n";
        }
        std::cout << "\n";
    }
}

void printProfileSummary(const EnhancementProfile& profile) {
    std::cout << "Profile summary:\n"
              << "  Schema: " << profile.schemaVersion << "\n"
              << "  Mode: " << (profile.modeId.empty() ? "(none)" : profile.modeId) << "\n"
              << "  Family: " << (profile.familyId.empty() ? "(none)" : profile.familyId) << "\n";

    if (profile.rawModelId.has_value()) {
        std::cout << "  Raw model: " << *profile.rawModelId << "\n";
    }
    if (!profile.capabilities.empty()) {
        std::cout << "  Capabilities: ";
        for (size_t i = 0; i < profile.capabilities.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << profile.capabilities[i];
        }
        std::cout << "\n";
    }
    if (!profile.strengthValues.empty()) {
        std::cout << "  Strengths:\n";
        for (const auto& [k, v] : profile.strengthValues) {
            std::cout << "    " << k << " = " << v << "\n";
        }
    }
    if (!profile.backendPreference.empty()) {
        std::cout << "  Backend: " << profile.backendPreference << "\n";
    }
    if (profile.precision.has_value()) {
        std::cout << "  Precision: " << *profile.precision << "\n";
    }
    if (!profile.outputPath.empty()) {
        std::cout << "  Output: " << profile.outputPath << "\n";
    }

    std::string fusionError;
    FusionPlanner planner;
    auto intent = profileToPipelineIntent(profile);
    auto pipeline = planner.plan(intent);
    if (!pipeline.nodes.empty()) {
        std::cout << "\n  Planned pipeline:\n";
        for (const auto& node : pipeline.nodes) {
            std::cout << "    " << toString(node.type) << " -> " << node.familyId;
            if (node.type == PlanNode::Type::FusedAIGroup) std::cout << " [fused]";
            if (!node.modelIds.empty()) {
                std::cout << " (";
                for (size_t m = 0; m < node.modelIds.size(); ++m) {
                    if (m > 0) std::cout << ", ";
                    std::cout << node.modelIds[m];
                }
                std::cout << ")";
            }
            std::cout << "\n";
        }
        if (!pipeline.diagnostics.empty()) {
            std::cout << "\n  Diagnostics:\n";
            for (const auto& diag : pipeline.diagnostics) {
                std::cout << "    [" << diag.code << "] " << diag.message << "\n";
            }
        }
    } else {
        std::cout << "\n  No pipeline produced.\n";
    }
}

}  // namespace ave
