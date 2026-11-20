#include "ave/temporal_frame_types.hpp"

#include <sstream>
#include <unordered_map>

namespace ave {

namespace {

std::string normalizePreset(const std::string& input) {
    std::string out;
    for (char ch : input) {
        if (ch == '-' || ch == ' ' || ch == '.') {
            out.push_back('_');
        } else {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }
    return out;
}

}  // namespace

double Rational::toDouble() const {
    if (denominator == 0) return 0.0;
    return static_cast<double>(numerator) / static_cast<double>(denominator);
}

bool Rational::isValid() const {
    return denominator != 0;
}

bool operator==(const Rational& a, const Rational& b) {
    return a.numerator == b.numerator && a.denominator == b.denominator;
}

bool operator!=(const Rational& a, const Rational& b) {
    return !(a == b);
}

bool FrameRef::isValid() const {
    return !path.empty() && width > 0 && height > 0 && index >= 0;
}

bool operator==(const FrameRef& a, const FrameRef& b) {
    return a.path == b.path && a.width == b.width && a.height == b.height &&
           a.timestamp == b.timestamp && a.index == b.index;
}

bool operator!=(const FrameRef& a, const FrameRef& b) {
    return !(a == b);
}

bool TemporalFrameWindow::isValid() const {
    if (!current.isValid()) return false;
    if (!timebase.isValid()) return false;
    for (const auto& f : history) {
        if (!f.isValid()) return false;
    }
    for (const auto& f : future) {
        if (!f.isValid()) return false;
    }
    return true;
}

bool MotionField::isValid() const {
    return width > 0 && height > 0 &&
           static_cast<int>(vectors.size()) == width * height * 2;
}

float MotionField::vx(int x, int y) const {
    return vectors[(y * width + x) * 2];
}

float MotionField::vy(int x, int y) const {
    return vectors[(y * width + x) * 2 + 1];
}

bool ImagePlane::isValid() const {
    return width > 0 && height > 0 && channels > 0 &&
           static_cast<int>(pixels.size()) == width * height * channels;
}

size_t ImagePlane::expectedByteCount() const {
    return static_cast<size_t>(width) * static_cast<size_t>(height) *
           static_cast<size_t>(channels);
}

bool DebugArtifact::isValid() const {
    return !name.empty() && data.isValid();
}

bool TemporalFramePacket::isValid() const {
    if (!window.isValid()) return false;
    if (motionPrevToCurrent.has_value() && !motionPrevToCurrent->isValid()) return false;
    if (motionCurrentToNext.has_value() && !motionCurrentToNext->isValid()) return false;
    if (depth.has_value() && !depth->isValid()) return false;
    if (reactiveMask.has_value() && !reactiveMask->isValid()) return false;
    if (transparencyCompositionMask.has_value() && !transparencyCompositionMask->isValid()) return false;
    if (occlusionMask.has_value() && !occlusionMask->isValid()) return false;
    if (confidenceMap.has_value() && !confidenceMap->isValid()) return false;
    if (aiAllowedMask.has_value() && !aiAllowedMask->isValid()) return false;
    for (const auto& a : debugArtifacts) {
        if (!a.isValid()) return false;
    }
    return true;
}

std::string toString(TemporalForgePreset preset) {
    switch (preset) {
        case TemporalForgePreset::Accurate:     return "accurate";
        case TemporalForgePreset::Balanced:     return "balanced";
        case TemporalForgePreset::Recover:      return "recover";
        case TemporalForgePreset::AnimeLineArt: return "anime_line_art";
        case TemporalForgePreset::FilmPreserve: return "film_preserve";
        case TemporalForgePreset::CleanDigital: return "clean_digital";
    }
    return "unknown";
}

std::optional<TemporalForgePreset> temporalForgePresetFromString(const std::string& value) {
    static const std::unordered_map<std::string, TemporalForgePreset> map = {
        {"accurate", TemporalForgePreset::Accurate},
        {"balanced", TemporalForgePreset::Balanced},
        {"recover", TemporalForgePreset::Recover},
        {"anime_line_art", TemporalForgePreset::AnimeLineArt},
        {"film_preserve", TemporalForgePreset::FilmPreserve},
        {"clean_digital", TemporalForgePreset::CleanDigital},
    };
    auto it = map.find(normalizePreset(value));
    if (it == map.end()) return std::nullopt;
    return it->second;
}

const std::vector<std::string>& temporalForgePresetNames() {
    static const std::vector<std::string> names = {
        "accurate", "balanced", "recover", "anime_line_art", "film_preserve", "clean_digital"
    };
    return names;
}

std::string toString(const Rational& r) {
    return std::to_string(r.numerator) + "/" + std::to_string(r.denominator);
}

std::string toString(const FrameRef& f) {
    return f.path + " [" + std::to_string(f.width) + "x" + std::to_string(f.height) +
           "] ts=" + toString(f.timestamp) + " idx=" + std::to_string(f.index);
}

std::string toString(const TemporalFrameWindow& w) {
    std::string result = "TemporalFrameWindow(history=";
    result += std::to_string(w.history.size());
    result += ", current=";
    result += toString(w.current);
    result += ", future=";
    result += std::to_string(w.future.size());
    result += ", sceneReset=";
    result += (w.sceneResetBeforeCurrent ? "true" : "false");
    result += ", timebase=";
    result += toString(w.timebase);
    result += ")";
    return result;
}

std::string toString(const MotionField& m) {
    return "MotionField(" + std::to_string(m.width) + "x" + std::to_string(m.height) +
           ", vectors=" + std::to_string(m.vectors.size() / 2) + ")";
}

std::string toString(const ImagePlane& p) {
    return "ImagePlane(" + std::to_string(p.width) + "x" + std::to_string(p.height) +
           "x" + std::to_string(p.channels) + ", bytes=" + std::to_string(p.pixels.size()) + ")";
}

std::string toString(const DebugArtifact& a) {
    return "DebugArtifact(name=" + a.name + ", " + toString(a.data) + ")";
}

namespace {

std::string escapeValue(const std::string& s) {
    std::string out;
    for (char ch : s) {
        if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\\') {
            out += "\\\\";
        } else {
            out.push_back(ch);
        }
    }
    return out;
}

std::string unescapeValue(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            if (s[i + 1] == 'n') { out.push_back('\n'); ++i; }
            else if (s[i + 1] == '\\') { out.push_back('\\'); ++i; }
            else { out.push_back(s[i]); }
        } else {
            out.push_back(s[i]);
        }
    }
    return out;
}

void writeLine(std::string& out, const std::string& key, const std::string& value) {
    out += key;
    out += '=';
    out += escapeValue(value);
    out += '\n';
}

void writeLine(std::string& out, const std::string& key, int64_t value) {
    writeLine(out, key, std::to_string(value));
}

void writeLine(std::string& out, const std::string& key, bool value) {
    writeLine(out, key, std::string(value ? "true" : "false"));
}

struct LineReader {
    std::istringstream stream;
    bool next(std::string& key, std::string& value) {
        std::string line;
        if (!std::getline(stream, line)) return false;
        auto eq = line.find('=');
        if (eq == std::string::npos) return false;
        key = line.substr(0, eq);
        value = unescapeValue(line.substr(eq + 1));
        return true;
    }
};

static void serializeRational(std::string& out, const std::string& prefix, const Rational& r) {
    writeLine(out, prefix + ".numerator", r.numerator);
    writeLine(out, prefix + ".denominator", r.denominator);
}

void serializeFrameRef(std::string& out, const std::string& prefix, const FrameRef& f) {
    writeLine(out, prefix + ".path", f.path);
    writeLine(out, prefix + ".width", static_cast<int64_t>(f.width));
    writeLine(out, prefix + ".height", static_cast<int64_t>(f.height));
    writeLine(out, prefix + ".index", static_cast<int64_t>(f.index));
    serializeRational(out, prefix + ".timestamp", f.timestamp);
}

static void serializeFrameRefList(std::string& out, const std::string& prefix,
                           const std::vector<FrameRef>& frames) {
    writeLine(out, prefix + ".count", static_cast<int64_t>(frames.size()));
    for (size_t i = 0; i < frames.size(); ++i) {
        serializeFrameRef(out, prefix + "." + std::to_string(i), frames[i]);
    }
}

static void serializeMotionField(std::string& out, const std::string& prefix, const MotionField& m) {
    writeLine(out, prefix + ".width", static_cast<int64_t>(m.width));
    writeLine(out, prefix + ".height", static_cast<int64_t>(m.height));
    std::string vecStr;
    for (size_t i = 0; i < m.vectors.size(); ++i) {
        if (i > 0) vecStr += ',';
        vecStr += std::to_string(m.vectors[i]);
    }
    writeLine(out, prefix + ".vectors", vecStr);
}

static void serializeImagePlane(std::string& out, const std::string& prefix, const ImagePlane& p) {
    writeLine(out, prefix + ".width", static_cast<int64_t>(p.width));
    writeLine(out, prefix + ".height", static_cast<int64_t>(p.height));
    writeLine(out, prefix + ".channels", static_cast<int64_t>(p.channels));
    writeLine(out, prefix + ".pixels_base64", "[binary:" + std::to_string(p.pixels.size()) + "]");
}

static void serializeDebugArtifact(std::string& out, const std::string& prefix, const DebugArtifact& a) {
    writeLine(out, prefix + ".name", a.name);
    writeLine(out, prefix + ".description", a.description);
    serializeImagePlane(out, prefix + ".data", a.data);
}

}  // namespace

std::string serializePacket(const TemporalFramePacket& packet) {
    std::string out;
    writeLine(out, "packet.version", static_cast<int64_t>(1));

    const auto& w = packet.window;
    serializeFrameRef(out, "window.current", w.current);
    serializeFrameRefList(out, "window.history", w.history);
    serializeFrameRefList(out, "window.future", w.future);
    writeLine(out, "window.sceneResetBeforeCurrent", w.sceneResetBeforeCurrent);
    serializeRational(out, "window.timebase", w.timebase);

    if (packet.motionPrevToCurrent.has_value()) {
        writeLine(out, "has_motionPrevToCurrent", true);
        serializeMotionField(out, "motionPrevToCurrent", *packet.motionPrevToCurrent);
    }
    if (packet.motionCurrentToNext.has_value()) {
        writeLine(out, "has_motionCurrentToNext", true);
        serializeMotionField(out, "motionCurrentToNext", *packet.motionCurrentToNext);
    }
    if (packet.depth.has_value()) {
        writeLine(out, "has_depth", true);
        serializeImagePlane(out, "depth", *packet.depth);
    }
    if (packet.reactiveMask.has_value()) {
        writeLine(out, "has_reactiveMask", true);
        serializeImagePlane(out, "reactiveMask", *packet.reactiveMask);
    }
    if (packet.transparencyCompositionMask.has_value()) {
        writeLine(out, "has_transparencyCompositionMask", true);
        serializeImagePlane(out, "transparencyCompositionMask", *packet.transparencyCompositionMask);
    }
    if (packet.occlusionMask.has_value()) {
        writeLine(out, "has_occlusionMask", true);
        serializeImagePlane(out, "occlusionMask", *packet.occlusionMask);
    }
    if (packet.confidenceMap.has_value()) {
        writeLine(out, "has_confidenceMap", true);
        serializeImagePlane(out, "confidenceMap", *packet.confidenceMap);
    }
    if (packet.aiAllowedMask.has_value()) {
        writeLine(out, "has_aiAllowedMask", true);
        serializeImagePlane(out, "aiAllowedMask", *packet.aiAllowedMask);
    }

    writeLine(out, "debugArtifacts.count", static_cast<int64_t>(packet.debugArtifacts.size()));
    for (size_t i = 0; i < packet.debugArtifacts.size(); ++i) {
        serializeDebugArtifact(out, "debugArtifact." + std::to_string(i), packet.debugArtifacts[i]);
    }

    return out;
}

std::optional<TemporalFramePacket> deserializePacket(const std::string& data) {
    TemporalFramePacket packet;
    LineReader reader;
    reader.stream.str(data);

    std::unordered_map<std::string, std::string> entries;
    std::string key, value;
    while (reader.next(key, value)) {
        entries[key] = value;
    }

    auto getBool = [&](const std::string& k, bool def) -> bool {
        auto it = entries.find(k);
        if (it == entries.end()) return def;
        return it->second == "true";
    };
    auto getInt = [&](const std::string& k, int64_t def) -> int64_t {
        auto it = entries.find(k);
        if (it == entries.end()) return def;
        return std::stoll(it->second);
    };
    auto getStr = [&](const std::string& k, const std::string& def) -> std::string {
        auto it = entries.find(k);
        if (it == entries.end()) return def;
        return it->second;
    };

    auto& w = packet.window;

    w.current.path = getStr("window.current.path", "");
    w.current.width = static_cast<int>(getInt("window.current.width", 0));
    w.current.height = static_cast<int>(getInt("window.current.height", 0));
    w.current.index = getInt("window.current.index", -1);
    w.current.timestamp.numerator = getInt("window.current.timestamp.numerator", 0);
    w.current.timestamp.denominator = getInt("window.current.timestamp.denominator", 1);

    int64_t historyCount = getInt("window.history.count", 0);
    for (int64_t i = 0; i < historyCount; ++i) {
        FrameRef f;
        std::string p = "window.history." + std::to_string(i);
        f.path = getStr(p + ".path", "");
        f.width = static_cast<int>(getInt(p + ".width", 0));
        f.height = static_cast<int>(getInt(p + ".height", 0));
        f.index = getInt(p + ".index", -1);
        f.timestamp.numerator = getInt(p + ".timestamp.numerator", 0);
        f.timestamp.denominator = getInt(p + ".timestamp.denominator", 1);
        w.history.push_back(f);
    }

    int64_t futureCount = getInt("window.future.count", 0);
    for (int64_t i = 0; i < futureCount; ++i) {
        FrameRef f;
        std::string p = "window.future." + std::to_string(i);
        f.path = getStr(p + ".path", "");
        f.width = static_cast<int>(getInt(p + ".width", 0));
        f.height = static_cast<int>(getInt(p + ".height", 0));
        f.index = getInt(p + ".index", -1);
        f.timestamp.numerator = getInt(p + ".timestamp.numerator", 0);
        f.timestamp.denominator = getInt(p + ".timestamp.denominator", 1);
        w.future.push_back(f);
    }

    w.sceneResetBeforeCurrent = getBool("window.sceneResetBeforeCurrent", false);
    w.timebase.numerator = getInt("window.timebase.numerator", 0);
    w.timebase.denominator = getInt("window.timebase.denominator", 1);

    if (getBool("has_motionPrevToCurrent", false)) {
        MotionField m;
        m.width = static_cast<int>(getInt("motionPrevToCurrent.width", 0));
        m.height = static_cast<int>(getInt("motionPrevToCurrent.height", 0));
        std::string vecs = getStr("motionPrevToCurrent.vectors", "");
        if (!vecs.empty()) {
            std::istringstream vs(vecs);
            std::string tok;
            while (std::getline(vs, tok, ',')) {
                m.vectors.push_back(std::stof(tok));
            }
        }
        packet.motionPrevToCurrent = m;
    }

    if (getBool("has_motionCurrentToNext", false)) {
        MotionField m;
        m.width = static_cast<int>(getInt("motionCurrentToNext.width", 0));
        m.height = static_cast<int>(getInt("motionCurrentToNext.height", 0));
        std::string vecs = getStr("motionCurrentToNext.vectors", "");
        if (!vecs.empty()) {
            std::istringstream vs(vecs);
            std::string tok;
            while (std::getline(vs, tok, ',')) {
                m.vectors.push_back(std::stof(tok));
            }
        }
        packet.motionCurrentToNext = m;
    }

    auto maybeImage = [&](const std::string& hasKey, const std::string& prefix)
        -> std::optional<ImagePlane> {
        if (!getBool(hasKey, false)) return std::nullopt;
        ImagePlane p;
        p.width = static_cast<int>(getInt(prefix + ".width", 0));
        p.height = static_cast<int>(getInt(prefix + ".height", 0));
        p.channels = static_cast<int>(getInt(prefix + ".channels", 0));
        return p;
    };

    packet.depth = maybeImage("has_depth", "depth");
    packet.reactiveMask = maybeImage("has_reactiveMask", "reactiveMask");
    packet.transparencyCompositionMask = maybeImage("has_transparencyCompositionMask", "transparencyCompositionMask");
    packet.occlusionMask = maybeImage("has_occlusionMask", "occlusionMask");
    packet.confidenceMap = maybeImage("has_confidenceMap", "confidenceMap");
    packet.aiAllowedMask = maybeImage("has_aiAllowedMask", "aiAllowedMask");

    int64_t artifactCount = getInt("debugArtifacts.count", 0);
    for (int64_t i = 0; i < artifactCount; ++i) {
        DebugArtifact a;
        std::string p = "debugArtifact." + std::to_string(i);
        a.name = getStr(p + ".name", "");
        a.description = getStr(p + ".description", "");
        a.data.width = static_cast<int>(getInt(p + ".data.width", 0));
        a.data.height = static_cast<int>(getInt(p + ".data.height", 0));
        a.data.channels = static_cast<int>(getInt(p + ".data.channels", 0));
        packet.debugArtifacts.push_back(a);
    }

    return packet;
}

}  // namespace ave
