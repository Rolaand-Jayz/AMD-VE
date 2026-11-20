#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ave {

struct PipelineIntent {
    std::string modeId;
    std::string familyId;
    std::optional<std::string> rawModelId;
    std::vector<std::string> requestedCapabilities;
    std::map<std::string, double> strengthValues;
};

struct PlanNode {
    enum class Type { OrdinaryStage, FusedAIGroup, TemporalForgeExperimental } type;
    std::string familyId;
    std::vector<std::string> capabilities;
    std::vector<std::string> modelIds;
};

struct PlannerDiagnostic {
    enum class Severity { Info, Warning, Error } severity;
    std::string code;
    std::string message;
};

struct PlannedPipeline {
    std::vector<PlanNode> nodes;
    std::vector<PlannerDiagnostic> diagnostics;
    std::string stableHash;
};

class FusionPlanner {
  public:
    PlannedPipeline plan(const PipelineIntent& intent) const;
};

std::string toString(PlanNode::Type type);
std::string toString(PlannerDiagnostic::Severity severity);

int capabilityOrder(const std::string& capability);

}  // namespace ave
