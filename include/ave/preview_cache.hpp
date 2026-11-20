#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "ave/fusion_planner.hpp"

namespace ave {

struct TimeRange {
    double startTimeSec = 0.0;
    double endTimeSec = 0.0;
    double timebase = 30.0;
};

enum class CacheState {
    Empty,
    Analyzing,
    Rendering,
    Cached,
    Warning,
    Invalid
};

std::string toString(CacheState state);
std::optional<CacheState> cacheStateFromString(const std::string& value);

struct CachedFrame {
    std::int64_t frameIndex = 0;
    double ptsSeconds = 0.0;
    std::string originalFramePath;
    std::string enhancedFramePath;
    std::string debugOverlayPath;
    std::string frameHash;
};

struct PreviewCacheKeyInputs {
    int schemaVersion = 1;
    std::string sourcePath;
    std::uintmax_t sourceSizeBytes = 0;
    std::int64_t sourceMtimeEpoch = 0;
    TimeRange range;
    std::string modeId;
    std::string familyId;
    std::optional<std::string> rawModelId;
    std::vector<std::string> requestedCapabilities;
    std::map<std::string, double> strengthValues;
    std::string backend;
    std::string precision;
    int outputWidth = 0;
    int outputHeight = 0;
};

std::string buildPreviewCacheKey(const PreviewCacheKeyInputs& inputs);

struct PreviewCacheManifest {
    int schemaVersion = 1;
    std::string cacheKey;
    std::string sourcePath;
    TimeRange range;
    PipelineIntent intent;
    PlannedPipeline plan;
    std::vector<CachedFrame> frames;
    CacheState state = CacheState::Empty;
    std::vector<std::string> warnings;
};

bool validateCacheManifest(const PreviewCacheManifest& manifest,
                           const PreviewCacheKeyInputs& currentInputs,
                           std::string& mismatchReason);

bool serializeCacheManifest(const PreviewCacheManifest& manifest,
                            std::string& output);
std::optional<PreviewCacheManifest> deserializeCacheManifest(const std::string& input,
                                                             std::string& error);

}  // namespace ave
