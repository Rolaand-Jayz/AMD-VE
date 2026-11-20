#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ave {

struct Rational {
    int64_t numerator = 0;
    int64_t denominator = 1;

    double toDouble() const;
    bool isValid() const;
};

bool operator==(const Rational& a, const Rational& b);
bool operator!=(const Rational& a, const Rational& b);

struct FrameRef {
    std::string path;
    int width = 0;
    int height = 0;
    Rational timestamp;
    int64_t index = -1;

    bool isValid() const;
};

bool operator==(const FrameRef& a, const FrameRef& b);
bool operator!=(const FrameRef& a, const FrameRef& b);

struct TemporalFrameWindow {
    std::vector<FrameRef> history;
    FrameRef current;
    std::vector<FrameRef> future;
    bool sceneResetBeforeCurrent = false;
    Rational timebase;

    bool isValid() const;
};

struct MotionField {
    int width = 0;
    int height = 0;
    std::vector<float> vectors;

    bool isValid() const;

    float vx(int x, int y) const;
    float vy(int x, int y) const;
};

struct ImagePlane {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<uint8_t> pixels;

    bool isValid() const;
    size_t expectedByteCount() const;
};

struct DebugArtifact {
    std::string name;
    std::string description;
    ImagePlane data;

    bool isValid() const;
};

struct TemporalFramePacket {
    TemporalFrameWindow window;
    std::optional<MotionField> motionPrevToCurrent;
    std::optional<MotionField> motionCurrentToNext;
    std::optional<ImagePlane> depth;
    std::optional<ImagePlane> reactiveMask;
    std::optional<ImagePlane> transparencyCompositionMask;
    std::optional<ImagePlane> occlusionMask;
    std::optional<ImagePlane> confidenceMap;
    std::optional<ImagePlane> aiAllowedMask;
    std::vector<DebugArtifact> debugArtifacts;

    bool isValid() const;
};

enum class TemporalForgePreset {
    Accurate,
    Balanced,
    Recover,
    AnimeLineArt,
    FilmPreserve,
    CleanDigital
};

std::string toString(TemporalForgePreset preset);
std::optional<TemporalForgePreset> temporalForgePresetFromString(const std::string& value);
const std::vector<std::string>& temporalForgePresetNames();

std::string toString(const Rational& r);
std::string toString(const FrameRef& f);
std::string toString(const TemporalFrameWindow& w);
std::string toString(const MotionField& m);
std::string toString(const ImagePlane& p);
std::string toString(const DebugArtifact& a);

std::string serializePacket(const TemporalFramePacket& packet);
std::optional<TemporalFramePacket> deserializePacket(const std::string& data);

}  // namespace ave
