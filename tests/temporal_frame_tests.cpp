#include <cassert>
#include <iostream>
#include <string>

#include "ave/temporal_frame_types.hpp"

namespace {

int g_failures = 0;

void check(bool condition, const std::string& msg) {
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        ++g_failures;
    }
}

ave::FrameRef makeFrame(const std::string& path, int w, int h, int64_t idx,
                        int64_t tsNum, int64_t tsDen) {
    ave::FrameRef f;
    f.path = path;
    f.width = w;
    f.height = h;
    f.index = idx;
    f.timestamp = {tsNum, tsDen};
    return f;
}

ave::TemporalFrameWindow makeWindow() {
    ave::TemporalFrameWindow w;
    w.current = makeFrame("frame_005.png", 1920, 1080, 5, 5, 30);
    w.timebase = {1, 30};
    w.history = {
        makeFrame("frame_003.png", 1920, 1080, 3, 3, 30),
        makeFrame("frame_004.png", 1920, 1080, 4, 4, 30),
    };
    w.future = {
        makeFrame("frame_006.png", 1920, 1080, 6, 6, 30),
    };
    w.sceneResetBeforeCurrent = false;
    return w;
}

}  // namespace

void testFrameRefValid() {
    auto f = makeFrame("test.png", 640, 480, 0, 0, 30);
    check(f.isValid(), "valid FrameRef should be valid");

    ave::FrameRef empty;
    check(!empty.isValid(), "default FrameRef should be invalid");
}

void testFrameRefEquality() {
    auto a = makeFrame("a.png", 100, 100, 0, 0, 1);
    auto b = makeFrame("a.png", 100, 100, 0, 0, 1);
    auto c = makeFrame("b.png", 100, 100, 0, 0, 1);
    check(a == b, "identical FrameRefs should be equal");
    check(a != c, "different FrameRefs should not be equal");
}

void testRationalValid() {
    ave::Rational valid{1, 30};
    check(valid.isValid(), "1/30 should be valid");
    check(!ave::Rational{1, 0}.isValid(), "denominator 0 should be invalid");

    ave::Rational half{1, 2};
    check(half.toDouble() > 0.49 && half.toDouble() < 0.51, "1/2 ~= 0.5");
}

void testRationalEquality() {
    ave::Rational a{1, 30};
    ave::Rational b{1, 30};
    ave::Rational c{1, 24};
    check(a == b, "identical Rationals should be equal");
    check(a != c, "different Rationals should not be equal");
}

void testWindowValid() {
    auto w = makeWindow();
    check(w.isValid(), "complete window should be valid");

    ave::TemporalFrameWindow empty;
    check(!empty.isValid(), "empty window should be invalid (no current)");
}

void testWindowEmptyHistoryIsAllowed() {
    ave::TemporalFrameWindow w;
    w.current = makeFrame("frame.png", 1920, 1080, 0, 0, 30);
    w.timebase = {1, 30};
    check(w.isValid(), "window with no history/future should be valid");
}

void testWindowHistoryPreservesOrder() {
    auto w = makeWindow();
    check(w.history.size() == 2, "history should have 2 frames");
    check(w.history[0].index == 3, "history[0] should be frame 3");
    check(w.history[1].index == 4, "history[1] should be frame 4");
}

void testWindowSceneReset() {
    auto w = makeWindow();
    check(!w.sceneResetBeforeCurrent, "scene reset should be false by default");

    w.sceneResetBeforeCurrent = true;
    check(w.sceneResetBeforeCurrent, "scene reset should be true after setting");
    check(w.isValid(), "window with scene reset should still be valid");
}

void testMotionFieldValid() {
    ave::MotionField m;
    m.width = 2;
    m.height = 2;
    m.vectors = {1.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f, 0.0f};
    check(m.isValid(), "2x2 motion field with 8 floats should be valid");
    check(m.vx(0, 0) == 1.0f, "vx(0,0) should be 1.0");
    check(m.vy(0, 0) == 0.5f, "vy(0,0) should be 0.5");
    check(m.vx(1, 0) == 0.0f, "vx(1,0) should be 0.0");

    ave::MotionField empty;
    check(!empty.isValid(), "default MotionField should be invalid");
}

void testImagePlaneValid() {
    ave::ImagePlane p;
    p.width = 2;
    p.height = 2;
    p.channels = 3;
    p.pixels.resize(12, 128);
    check(p.isValid(), "2x2x3 image plane with 12 bytes should be valid");
    check(p.expectedByteCount() == 12, "expectedByteCount should be 12");

    ave::ImagePlane empty;
    check(!empty.isValid(), "default ImagePlane should be invalid");
}

void testDebugArtifactValid() {
    ave::DebugArtifact a;
    a.name = "motion_viz";
    a.description = "motion field visualization";
    a.data.width = 2;
    a.data.height = 2;
    a.data.channels = 3;
    a.data.pixels.resize(12, 0);
    check(a.isValid(), "valid debug artifact");

    ave::DebugArtifact empty;
    check(!empty.isValid(), "unnamed debug artifact should be invalid");
}

void testPacketMinimalValid() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();
    check(pkt.isValid(), "minimal packet with only window should be valid");
}

void testPacketWithOptionalFields() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();

    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f};
    pkt.motionPrevToCurrent = m;

    ave::ImagePlane depth;
    depth.width = 2; depth.height = 2; depth.channels = 1;
    depth.pixels.resize(4, 128);
    pkt.depth = depth;

    check(pkt.isValid(), "packet with motion and depth should be valid");
}

void testPacketSerializationRoundTrip() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();
    pkt.window.sceneResetBeforeCurrent = true;

    ave::MotionField m;
    m.width = 2; m.height = 2;
    m.vectors = {1.5f, -0.5f, 0.0f, 1.0f, -2.0f, 0.5f, 0.0f, -1.0f};
    pkt.motionPrevToCurrent = m;

    std::string data = ave::serializePacket(pkt);
    check(!data.empty(), "serialized packet should not be empty");

    auto restored = ave::deserializePacket(data);
    check(restored.has_value(), "deserialization should succeed");

    check(restored->window.current.path == "frame_005.png",
          "current frame path should round-trip");
    check(restored->window.current.width == 1920, "width should round-trip");
    check(restored->window.current.height == 1080, "height should round-trip");
    check(restored->window.current.index == 5, "index should round-trip");
    check(restored->window.current.timestamp.numerator == 5, "timestamp num should round-trip");
    check(restored->window.current.timestamp.denominator == 30, "timestamp den should round-trip");

    check(restored->window.history.size() == 2, "history count should round-trip");
    check(restored->window.history[0].path == "frame_003.png", "history[0] path should round-trip");
    check(restored->window.history[1].path == "frame_004.png", "history[1] path should round-trip");

    check(restored->window.future.size() == 1, "future count should round-trip");
    check(restored->window.future[0].path == "frame_006.png", "future[0] path should round-trip");

    check(restored->window.sceneResetBeforeCurrent, "scene reset should round-trip");
    check(restored->window.timebase.numerator == 1, "timebase num should round-trip");
    check(restored->window.timebase.denominator == 30, "timebase den should round-trip");

    check(restored->motionPrevToCurrent.has_value(), "motion should round-trip as present");
    check(restored->motionPrevToCurrent->width == 2, "motion width should round-trip");
    check(restored->motionPrevToCurrent->height == 2, "motion height should round-trip");
    check(restored->motionPrevToCurrent->vectors.size() == 8, "motion vector count should round-trip");
}

void testPacketMissingOptionalsRoundTrip() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();

    std::string data = ave::serializePacket(pkt);
    auto restored = ave::deserializePacket(data);
    check(restored.has_value(), "minimal packet deserialization should succeed");

    check(!restored->motionPrevToCurrent.has_value(), "motion should be absent");
    check(!restored->motionCurrentToNext.has_value(), "motion next should be absent");
    check(!restored->depth.has_value(), "depth should be absent");
    check(!restored->reactiveMask.has_value(), "reactiveMask should be absent");
    check(!restored->confidenceMap.has_value(), "confidenceMap should be absent");
    check(!restored->aiAllowedMask.has_value(), "aiAllowedMask should be absent");
}

void testDebugArtifactsSerialization() {
    ave::TemporalFramePacket pkt;
    pkt.window = makeWindow();

    ave::DebugArtifact art;
    art.name = "motion_field_visualization";
    art.description = "block motion vectors rendered as color";
    art.data.width = 4; art.data.height = 4; art.data.channels = 3;
    art.data.pixels.resize(48, 64);
    pkt.debugArtifacts.push_back(art);

    std::string data = ave::serializePacket(pkt);
    auto restored = ave::deserializePacket(data);
    check(restored.has_value(), "packet with debug artifacts should deserialize");
    check(restored->debugArtifacts.size() == 1, "debug artifact count should round-trip");
    check(restored->debugArtifacts[0].name == "motion_field_visualization",
          "artifact name should round-trip");
    check(restored->debugArtifacts[0].description == "block motion vectors rendered as color",
          "artifact description should round-trip");
    check(restored->debugArtifacts[0].data.width == 4, "artifact width should round-trip");
}

void testPresetStringConversions() {
    const auto& names = ave::temporalForgePresetNames();
    check(names.size() == 6, "should have 6 presets");

    for (const auto& name : names) {
        auto opt = ave::temporalForgePresetFromString(name);
        check(opt.has_value(), "preset '" + name + "' should parse");
        check(ave::toString(*opt) == name, "preset round-trip for '" + name + "'");
    }

    check(!ave::temporalForgePresetFromString("nonexistent").has_value(),
          "invalid preset should return nullopt");
}

void testToStringFunctions() {
    ave::Rational r{30000, 1001};
    check(ave::toString(r) == "30000/1001", "Rational toString");

    auto f = makeFrame("test.png", 640, 480, 7, 210, 30);
    std::string fs = ave::toString(f);
    check(fs.find("test.png") != std::string::npos, "FrameRef toString contains path");
    check(fs.find("640") != std::string::npos, "FrameRef toString contains width");
    check(fs.find("480") != std::string::npos, "FrameRef toString contains height");

    auto w = makeWindow();
    std::string ws = ave::toString(w);
    check(ws.find("TemporalFrameWindow") != std::string::npos, "window toString prefix");
}

int main() {
    testFrameRefValid();
    testFrameRefEquality();
    testRationalValid();
    testRationalEquality();
    testWindowValid();
    testWindowEmptyHistoryIsAllowed();
    testWindowHistoryPreservesOrder();
    testWindowSceneReset();
    testMotionFieldValid();
    testImagePlaneValid();
    testDebugArtifactValid();
    testPacketMinimalValid();
    testPacketWithOptionalFields();
    testPacketSerializationRoundTrip();
    testPacketMissingOptionalsRoundTrip();
    testDebugArtifactsSerialization();
    testPresetStringConversions();
    testToStringFunctions();

    if (g_failures > 0) {
        std::cerr << g_failures << " test(s) FAILED." << std::endl;
        return 1;
    }
    std::cout << "All temporal frame tests passed." << std::endl;
    return 0;
}
