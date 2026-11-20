#include "ave/preview_cache.hpp"

#include <algorithm>
#include <sstream>
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

void hashCombine(std::size_t& seed, const std::string& value) {
    std::size_t h = std::hash<std::string>{}(value);
    seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

void hashCombine(std::size_t& seed, double value) {
    hashCombine(seed, std::to_string(value));
}

void hashCombine(std::size_t& seed, std::int64_t value) {
    hashCombine(seed, std::to_string(value));
}

void hashCombine(std::size_t& seed, std::uintmax_t value) {
    hashCombine(seed, std::to_string(value));
}

void hashCombine(std::size_t& seed, int value) {
    hashCombine(seed, std::to_string(value));
}

}  // namespace

std::string toString(CacheState state) {
    switch (state) {
        case CacheState::Empty:     return "empty";
        case CacheState::Analyzing: return "analyzing";
        case CacheState::Rendering: return "rendering";
        case CacheState::Cached:    return "cached";
        case CacheState::Warning:   return "warning";
        case CacheState::Invalid:   return "invalid";
    }
    return "unknown";
}

std::optional<CacheState> cacheStateFromString(const std::string& value) {
    static const std::unordered_map<std::string, CacheState> map = {
        {"empty",     CacheState::Empty},
        {"analyzing", CacheState::Analyzing},
        {"rendering", CacheState::Rendering},
        {"cached",    CacheState::Cached},
        {"warning",   CacheState::Warning},
        {"invalid",   CacheState::Invalid},
    };
    auto it = map.find(normalizeId(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

std::string buildPreviewCacheKey(const PreviewCacheKeyInputs& inputs) {
    std::size_t h = 0;
    hashCombine(h, std::to_string(inputs.schemaVersion));
    hashCombine(h, inputs.sourcePath);
    hashCombine(h, inputs.sourceSizeBytes);
    hashCombine(h, inputs.sourceMtimeEpoch);
    hashCombine(h, inputs.range.startTimeSec);
    hashCombine(h, inputs.range.endTimeSec);
    hashCombine(h, inputs.range.timebase);
    hashCombine(h, inputs.modeId);
    hashCombine(h, inputs.familyId);
    if (inputs.rawModelId.has_value()) {
        hashCombine(h, *inputs.rawModelId);
    }
    for (const auto& cap : inputs.requestedCapabilities) {
        hashCombine(h, cap);
    }
    for (const auto& [k, v] : inputs.strengthValues) {
        hashCombine(h, k);
        hashCombine(h, v);
    }
    hashCombine(h, inputs.backend);
    hashCombine(h, inputs.precision);
    hashCombine(h, inputs.outputWidth);
    hashCombine(h, inputs.outputHeight);

    std::ostringstream os;
    os << std::hex << h;
    return os.str();
}

bool validateCacheManifest(const PreviewCacheManifest& manifest,
                           const PreviewCacheKeyInputs& currentInputs,
                           std::string& mismatchReason) {
    const std::string currentKey = buildPreviewCacheKey(currentInputs);
    if (manifest.cacheKey != currentKey) {
        mismatchReason = "cache key mismatch: cached=" + manifest.cacheKey +
                         " current=" + currentKey;
        return false;
    }
    if (manifest.schemaVersion != currentInputs.schemaVersion) {
        mismatchReason = "schema version mismatch: cached=" +
                         std::to_string(manifest.schemaVersion) + " current=" +
                         std::to_string(currentInputs.schemaVersion);
        return false;
    }
    if (manifest.sourcePath != currentInputs.sourcePath) {
        mismatchReason = "source path mismatch";
        return false;
    }
    return true;
}

bool serializeCacheManifest(const PreviewCacheManifest& manifest,
                            std::string& output) {
    std::ostringstream os;
    os << "schema_version=" << manifest.schemaVersion << "\n";
    os << "cache_key=" << manifest.cacheKey << "\n";
    os << "source_path=" << manifest.sourcePath << "\n";
    os << "range_start=" << manifest.range.startTimeSec << "\n";
    os << "range_end=" << manifest.range.endTimeSec << "\n";
    os << "timebase=" << manifest.range.timebase << "\n";
    os << "state=" << toString(manifest.state) << "\n";
    os << "frame_count=" << manifest.frames.size() << "\n";
    for (const auto& frame : manifest.frames) {
        os << "frame=" << frame.frameIndex
           << "," << frame.ptsSeconds
           << "," << frame.originalFramePath
           << "," << frame.enhancedFramePath
           << "," << frame.frameHash
           << "\n";
    }
    for (const auto& w : manifest.warnings) {
        os << "warning=" << w << "\n";
    }
    output = os.str();
    return true;
}

std::optional<PreviewCacheManifest> deserializeCacheManifest(const std::string& input,
                                                             std::string& error) {
    PreviewCacheManifest manifest;
    std::istringstream is(input);
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);

        if (key == "schema_version") { manifest.schemaVersion = std::stoi(val); }
        else if (key == "cache_key") { manifest.cacheKey = val; }
        else if (key == "source_path") { manifest.sourcePath = val; }
        else if (key == "range_start") { manifest.range.startTimeSec = std::stod(val); }
        else if (key == "range_end") { manifest.range.endTimeSec = std::stod(val); }
        else if (key == "timebase") { manifest.range.timebase = std::stod(val); }
        else if (key == "state") {
            auto s = cacheStateFromString(val);
            if (!s.has_value()) {
                error = "invalid cache state: " + val;
                return std::nullopt;
            }
            manifest.state = *s;
        }
        else if (key == "frame") {
            CachedFrame frame;
            auto c1 = val.find(',');
            if (c1 == std::string::npos) continue;
            frame.frameIndex = std::stoll(val.substr(0, c1));
            auto c2 = val.find(',', c1 + 1);
            if (c2 == std::string::npos) continue;
            frame.ptsSeconds = std::stod(val.substr(c1 + 1, c2 - c1 - 1));
            auto c3 = val.find(',', c2 + 1);
            if (c3 == std::string::npos) continue;
            frame.originalFramePath = val.substr(c2 + 1, c3 - c2 - 1);
            auto c4 = val.find(',', c3 + 1);
            if (c4 == std::string::npos) continue;
            frame.enhancedFramePath = val.substr(c3 + 1, c4 - c3 - 1);
            frame.frameHash = val.substr(c4 + 1);
            manifest.frames.push_back(frame);
        }
        else if (key == "warning") { manifest.warnings.push_back(val); }
    }
    return manifest;
}

}  // namespace ave
