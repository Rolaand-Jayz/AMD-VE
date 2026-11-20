#include <cassert>
#include <iostream>
#include <string>

#include "ave/preview_cache.hpp"

namespace {

int g_failures = 0;

void check(bool condition, const std::string& msg) {
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        ++g_failures;
    }
}

}  // namespace

ave::PreviewCacheKeyInputs makeBaseInputs() {
    ave::PreviewCacheKeyInputs inputs;
    inputs.sourcePath = "/tmp/test_video.mp4";
    inputs.sourceSizeBytes = 1024000;
    inputs.sourceMtimeEpoch = 1700000000;
    inputs.range = {0.0, 10.0, 30.0};
    inputs.modeId = "restore_video";
    inputs.familyId = "clearreality_fast";
    inputs.requestedCapabilities = {"denoise", "upscale"};
    inputs.strengthValues = {{"strength", 0.5}};
    inputs.backend = "migraphx";
    inputs.precision = "fp16";
    inputs.outputWidth = 3840;
    inputs.outputHeight = 2160;
    return inputs;
}

void testStableKey() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    check(ave::buildPreviewCacheKey(a) == ave::buildPreviewCacheKey(b),
          "identical inputs should produce identical keys");
}

void testDifferentSourcePath() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.sourcePath = "/tmp/other_video.mp4";
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different source path should produce different keys");
}

void testDifferentSourceSize() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.sourceSizeBytes = 2048000;
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different source size should produce different keys");
}

void testDifferentSourceMtime() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.sourceMtimeEpoch = 1700000001;
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different source mtime should produce different keys");
}

void testDifferentMode() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.modeId = "upscale_video";
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different mode should produce different keys");
}

void testDifferentFamily() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.familyId = "openproteus_quality";
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different family should produce different keys");
}

void testDifferentStrength() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.strengthValues["strength"] = 0.8;
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different strength should produce different keys");
}

void testDifferentBackend() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.backend = "vulkan";
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different backend should produce different keys");
}

void testDifferentResolution() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.outputWidth = 1920;
    b.outputHeight = 1080;
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different resolution should produce different keys");
}

void testDifferentTimeRange() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.range.startTimeSec = 5.0;
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different time range should produce different keys");
}

void testDifferentCapabilities() {
    auto a = makeBaseInputs();
    auto b = makeBaseInputs();
    b.requestedCapabilities = {"upscale"};
    check(ave::buildPreviewCacheKey(a) != ave::buildPreviewCacheKey(b),
          "different capabilities should produce different keys");
}

void testCacheStateRoundTrip() {
    check(ave::toString(ave::CacheState::Cached) == "cached", "Cached toString");
    auto parsed = ave::cacheStateFromString("cached");
    check(parsed.has_value() && parsed.value() == ave::CacheState::Cached,
          "cached fromString");
    check(ave::cacheStateFromString("nonexistent") == std::nullopt,
          "nonexistent cache state should return nullopt");
}

void testManifestSerializationRoundTrip() {
    ave::PreviewCacheManifest manifest;
    manifest.schemaVersion = 1;
    manifest.cacheKey = "abc123";
    manifest.sourcePath = "/tmp/test.mp4";
    manifest.range = {0.0, 10.0, 30.0};
    manifest.state = ave::CacheState::Cached;
    manifest.frames.push_back({0, 0.0, "/tmp/o_0.png", "/tmp/e_0.png", "", "hash0"});
    manifest.frames.push_back({1, 0.033, "/tmp/o_1.png", "/tmp/e_1.png", "", "hash1"});
    manifest.warnings.push_back("test warning");

    std::string serialized;
    check(ave::serializeCacheManifest(manifest, serialized), "serialization should succeed");

    std::string error;
    auto loaded = ave::deserializeCacheManifest(serialized, error);
    check(loaded.has_value(), "deserialization should succeed: " + error);
    check(loaded->cacheKey == "abc123", "round-trip cache key");
    check(loaded->sourcePath == "/tmp/test.mp4", "round-trip source path");
    check(loaded->state == ave::CacheState::Cached, "round-trip state");
    check(loaded->frames.size() == 2, "round-trip frame count");
    check(loaded->frames[0].frameIndex == 0, "round-trip frame 0 index");
    check(loaded->frames[1].ptsSeconds == 0.033, "round-trip frame 1 pts");
    check(loaded->warnings.size() == 1, "round-trip warning count");
}

void testManifestValidation() {
    auto inputs = makeBaseInputs();
    ave::PreviewCacheManifest manifest;
    manifest.schemaVersion = inputs.schemaVersion;
    manifest.cacheKey = ave::buildPreviewCacheKey(inputs);
    manifest.sourcePath = inputs.sourcePath;

    std::string reason;
    check(ave::validateCacheManifest(manifest, inputs, reason),
          "matching manifest should validate");

    auto changedInputs = inputs;
    changedInputs.sourcePath = "/tmp/other.mp4";
    check(!ave::validateCacheManifest(manifest, changedInputs, reason),
          "changed source path should fail validation");
    check(!reason.empty(), "mismatch reason should be non-empty");

    auto changedSchema = inputs;
    changedSchema.schemaVersion = 99;
    check(!ave::validateCacheManifest(manifest, changedSchema, reason),
          "changed schema version should fail validation");
}

int main() {
    testStableKey();
    testDifferentSourcePath();
    testDifferentSourceSize();
    testDifferentSourceMtime();
    testDifferentMode();
    testDifferentFamily();
    testDifferentStrength();
    testDifferentBackend();
    testDifferentResolution();
    testDifferentTimeRange();
    testDifferentCapabilities();
    testCacheStateRoundTrip();
    testManifestSerializationRoundTrip();
    testManifestValidation();

    if (g_failures > 0) {
        std::cerr << "\n" << g_failures << " test(s) FAILED." << std::endl;
        return 1;
    }
    std::cout << "All preview_cache_tests passed." << std::endl;
    return 0;
}
