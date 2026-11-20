#include <cassert>
#include <iostream>
#include <set>
#include <string>

#include "ave/mode_definition.hpp"
#include "ave/family_metadata.hpp"

namespace {

int g_failures = 0;

void check(bool condition, const std::string& msg) {
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        ++g_failures;
    }
}

}  // namespace

// ── Mode definition tests (P1-E1-T1) ────────────────────────────────

void testRequiredModeIdsExist() {
    const auto& modes = ave::builtinModeDefinitions();
    std::set<std::string> ids;
    for (const auto& m : modes) {
        ids.insert(m.id);
    }

    check(ids.count("restore_video") == 1, "missing mode: restore_video");
    check(ids.count("upscale_video") == 1, "missing mode: upscale_video");
    check(ids.count("anime_line_art") == 1, "missing mode: anime_line_art");
    check(ids.count("stereo_3d_sbs") == 1, "missing mode: stereo_3d_sbs");
    check(ids.count("interpolate_motion") == 1, "missing mode: interpolate_motion");
    check(ids.count("advanced_legacy") == 1, "missing mode: advanced_legacy");
    check(ids.count("temporal_forge_experimental") == 1,
          "missing mode: temporal_forge_experimental");
}

void testModeIdsAreUnique() {
    const auto& modes = ave::builtinModeDefinitions();
    std::set<std::string> seen;
    for (const auto& m : modes) {
        check(seen.count(m.id) == 0, "duplicate mode id: " + m.id);
        seen.insert(m.id);
    }
}

void testModeSerializationRoundTrip() {
    const auto& modes = ave::builtinModeDefinitions();
    for (const auto& mode : modes) {
        const auto* found = ave::findModeDefinition(mode.id);
        check(found != nullptr, "findModeDefinition returned null for: " + mode.id);
        check(found->id == mode.id, "round-trip id mismatch for: " + mode.id);
        check(found->label == mode.label, "round-trip label mismatch for: " + mode.id);
        check(found->cliProfileKey == mode.cliProfileKey,
              "round-trip cliProfileKey mismatch for: " + mode.id);
    }
}

void testNonAdvancedModesResolveToFamily() {
    const auto& modes = ave::builtinModeDefinitions();
    for (const auto& mode : modes) {
        if (mode.advancedOnly) continue;
        if (mode.experimental) continue;
        check(!mode.defaultFamilyId.empty(),
              "non-advanced mode '" + mode.id + "' has empty defaultFamilyId");
        const auto* family = ave::findFamilyMetadata(mode.defaultFamilyId);
        check(family != nullptr,
              "mode '" + mode.id + "' default family '" + mode.defaultFamilyId + "' not found");
    }
}

void testTemporalForgeModeIsExperimental() {
    const auto* tf = ave::findModeDefinition("temporal_forge_experimental");
    check(tf != nullptr, "temporal_forge_experimental mode not found");
    check(tf->experimental, "temporal_forge_experimental must be experimental=true");
    check(!tf->advancedOnly, "temporal_forge_experimental should not be advancedOnly");
}

void testAdvancedLegacyMode() {
    const auto* adv = ave::findModeDefinition("advanced_legacy");
    check(adv != nullptr, "advanced_legacy mode not found");
    check(adv->advancedOnly, "advanced_legacy must be advancedOnly=true");
    check(!adv->experimental, "advanced_legacy should not be experimental");
    check(adv->defaultFamilyId.empty(),
          "advanced_legacy should have empty defaultFamilyId");
}

void testVisibleModesExcludeExperimentalAndAdvanced() {
    const auto visible = ave::visibleModeDefinitions();
    for (const auto* mode : visible) {
        check(!mode->advancedOnly,
              "visible mode '" + mode->id + "' should not be advancedOnly");
        check(!mode->experimental,
              "visible mode '" + mode->id + "' should not be experimental");
    }
    check(visible.size() >= 4,
          "expected at least 4 visible modes, got " + std::to_string(visible.size()));
}

void testAdvancedModesIncludeExperimentalAndAdvanced() {
    const auto advanced = ave::advancedModeDefinitions();
    std::set<std::string> ids;
    for (const auto* mode : advanced) {
        ids.insert(mode->id);
    }
    check(ids.count("advanced_legacy") == 1, "advanced should include advanced_legacy");
    check(ids.count("temporal_forge_experimental") == 1,
          "advanced should include temporal_forge_experimental");
}

void testPreviewBehaviorRoundTrip() {
    check(ave::toString(ave::PreviewBehavior::SplitWipe) == "split_wipe",
          "SplitWipe toString");
    check(ave::previewBehaviorFromString("split_wipe") == ave::PreviewBehavior::SplitWipe,
          "split_wipe fromString");
    check(ave::previewBehaviorFromString("nonexistent") == std::nullopt,
          "nonexistent preview behavior should return nullopt");
}

void testOutputBehaviorRoundTrip() {
    check(ave::toString(ave::OutputBehavior::SbsVideo) == "sbs_video",
          "SbsVideo toString");
    check(ave::outputBehaviorFromString("sbs_video") == ave::OutputBehavior::SbsVideo,
          "sbs_video fromString");
}

void testValidateModeDefinition() {
    ave::EnhancementModeDefinition bad;
    bad.id = "";
    bad.label = "Test";
    bad.purpose = "Test purpose";
    std::string error;
    check(!ave::validateModeDefinition(bad, error), "empty id should fail validation");
    check(!error.empty(), "error message should be non-empty for empty id");

    ave::EnhancementModeDefinition good;
    good.id = "test_mode";
    good.label = "Test Mode";
    good.purpose = "Testing";
    good.defaultFamilyId = "clearreality_fast";
    good.cliProfileKey = "test";
    check(ave::validateModeDefinition(good, error),
          "valid mode should pass: " + error);

    ave::EnhancementModeDefinition noFamily;
    noFamily.id = "no_family";
    noFamily.label = "No Family";
    noFamily.purpose = "Test";
    noFamily.defaultFamilyId = "";
    noFamily.cliProfileKey = "nofamily";
    check(!ave::validateModeDefinition(noFamily, error),
          "non-advanced mode without family should fail");
}

void testAllBuiltInModesAreValid() {
    const auto& modes = ave::builtinModeDefinitions();
    for (const auto& mode : modes) {
        std::string error;
        check(ave::validateModeDefinition(mode, error),
              "built-in mode '" + mode.id + "' should validate: " + error);
    }
}

// ── Family metadata tests (P1-E1-T2) ────────────────────────────────

void testFamilyIdsAreUnique() {
    const auto& families = ave::builtinFamilyMetadata();
    std::set<std::string> seen;
    for (const auto& f : families) {
        check(seen.count(f.familyId) == 0, "duplicate family id: " + f.familyId);
        seen.insert(f.familyId);
    }
}

void testFamilyForMode() {
    const auto* restore = ave::familyForMode("restore_video");
    check(restore != nullptr, "restore_video should resolve to a family");
    check(restore->familyId == "clearreality_fast",
          "restore_video should resolve to clearreality_fast, got: " + restore->familyId);

    const auto* upscale = ave::familyForMode("upscale_video");
    check(upscale != nullptr, "upscale_video should resolve to a family");
    check(upscale->familyId == "openproteus_quality",
          "upscale_video should resolve to openproteus_quality");

    const auto* advanced = ave::familyForMode("advanced_legacy");
    check(advanced == nullptr, "advanced_legacy should not resolve to a family");
}

void testVisibleFamiliesQuery() {
    const auto visible = ave::visibleFamilies();
    check(visible.size() >= 4,
          "expected at least 4 visible families, got " + std::to_string(visible.size()));
    for (const auto* f : visible) {
        check(f->visibleDefault,
              "visible family '" + f->familyId + "' should have visibleDefault=true");
        check(!f->advancedOnly,
              "visible family '" + f->familyId + "' should not be advancedOnly");
    }
}

void testAdvancedFamiliesQuery() {
    const auto advanced = ave::advancedFamilies();
    check(advanced.size() >= 1, "expected at least 1 advanced family");
    for (const auto* f : advanced) {
        check(f->advancedOnly,
              "advanced family '" + f->familyId + "' should have advancedOnly=true");
    }
}

void testValidateFamilyMetadata() {
    ave::ModelFamilyMetadata bad;
    bad.familyId = "";
    bad.familyName = "Test";
    bad.displayPurpose = "Test";
    std::string error;
    check(!ave::validateFamilyMetadata(bad, error), "empty familyId should fail");

    ave::ModelFamilyMetadata good;
    good.familyId = "test_family";
    good.familyName = "Test Family";
    good.displayPurpose = "Testing";
    good.capabilities = {"upscale"};
    good.safeDefaults = {"upscale"};
    check(ave::validateFamilyMetadata(good, error),
          "valid family should pass: " + error);

    ave::ModelFamilyMetadata selectiveNoFused;
    selectiveNoFused.familyId = "bad_selective";
    selectiveNoFused.familyName = "Bad Selective";
    selectiveNoFused.displayPurpose = "Test";
    selectiveNoFused.capabilities = {"upscale"};
    selectiveNoFused.safeDefaults = {"upscale"};
    selectiveNoFused.fused = false;
    selectiveNoFused.selective = true;
    check(!ave::validateFamilyMetadata(selectiveNoFused, error),
          "selective without fused should fail");

    ave::ModelFamilyMetadata safeNotSubset;
    safeNotSubset.familyId = "bad_safe";
    safeNotSubset.familyName = "Bad Safe";
    safeNotSubset.displayPurpose = "Test";
    safeNotSubset.capabilities = {"upscale"};
    safeNotSubset.safeDefaults = {"denoise"};
    check(!ave::validateFamilyMetadata(safeNotSubset, error),
          "safe_default not in capabilities should fail");
}

void testAllBuiltInFamiliesAreValid() {
    const auto& families = ave::builtinFamilyMetadata();
    for (const auto& family : families) {
        std::string error;
        check(ave::validateFamilyMetadata(family, error),
              "built-in family '" + family.familyId + "' should validate: " + error);
    }
}

void testContentTypeRoundTrip() {
    check(ave::toString(ave::ContentType::LiveAction) == "live_action",
          "LiveAction toString");
    auto parsed = ave::contentTypeFromString("live_action");
    check(parsed.has_value() && parsed.value() == ave::ContentType::LiveAction,
          "live_action fromString");
    check(ave::contentTypeFromString("nonexistent") == std::nullopt,
          "nonexistent content type should return nullopt");
}

void testTierRoundTrip() {
    check(ave::toString(ave::Tier::A) == "A", "Tier::A toString");
    auto parsed = ave::tierFromString("A");
    check(parsed.has_value() && parsed.value() == ave::Tier::A, "tier A fromString");
    check(ave::tierFromString("unknown").value() == ave::Tier::Unknown,
          "tier unknown fromString");
}

// ── Visible family slate tests (P1-E1-T3) ───────────────────────────

void testVisibleFamilySlateExists() {
    const auto visible = ave::visibleFamilies();
    std::set<std::string> ids;
    for (const auto* f : visible) {
        ids.insert(f->familyId);
    }
    check(ids.count("clearreality_fast") == 1, "visible slate missing clearreality_fast");
    check(ids.count("openproteus_quality") == 1, "visible slate missing openproteus_quality");
    check(ids.count("anime_line_art") == 1, "visible slate missing anime_line_art");
    check(ids.count("depth_sbs") == 1, "visible slate missing depth_sbs");
    check(ids.count("motion_rife") == 1, "visible slate missing motion_rife");
}

void testAdvancedDoesNotLeakIntoVisible() {
    const auto visible = ave::visibleFamilies();
    for (const auto* f : visible) {
        check(f->familyId != "temporal_forge_experimental",
              "temporal_forge should not be in visible families");
        check(f->familyId != "advanced_legacy",
              "advanced_legacy should not be in visible families");
    }
}

void testModeDefaultFamiliesMatchSlate() {
    const auto visibleModes = ave::visibleModeDefinitions();
    for (const auto* mode : visibleModes) {
        if (mode->defaultFamilyId.empty()) continue;
        const auto* family = ave::findFamilyMetadata(mode->defaultFamilyId);
        check(family != nullptr,
              "mode '" + mode->id + "' default family '" + mode->defaultFamilyId + "' not found");
        check(family->visibleDefault || family->familyId == "temporal_forge_experimental",
              "mode '" + mode->id + "' maps to non-visible family '" + family->familyId + "'");
    }
}

void testRawModelIdStillResolves() {
    const auto* adv = ave::findModeDefinition("advanced_legacy");
    check(adv != nullptr, "advanced_legacy mode should exist");
    check(adv->advancedOnly, "advanced_legacy should be advancedOnly");

    const auto* advFam = ave::findFamilyMetadata("advanced_legacy");
    check(advFam != nullptr, "advanced_legacy family should exist");
    check(advFam->advancedOnly, "advanced_legacy family should be advancedOnly");
    check(advFam->capabilities.empty(),
          "advanced_legacy family should have empty capabilities");
}

int main() {
    testRequiredModeIdsExist();
    testModeIdsAreUnique();
    testModeSerializationRoundTrip();
    testNonAdvancedModesResolveToFamily();
    testTemporalForgeModeIsExperimental();
    testAdvancedLegacyMode();
    testVisibleModesExcludeExperimentalAndAdvanced();
    testAdvancedModesIncludeExperimentalAndAdvanced();
    testPreviewBehaviorRoundTrip();
    testOutputBehaviorRoundTrip();
    testValidateModeDefinition();
    testAllBuiltInModesAreValid();

    testFamilyIdsAreUnique();
    testFamilyForMode();
    testVisibleFamiliesQuery();
    testAdvancedFamiliesQuery();
    testValidateFamilyMetadata();
    testAllBuiltInFamiliesAreValid();
    testContentTypeRoundTrip();
    testTierRoundTrip();

    testVisibleFamilySlateExists();
    testAdvancedDoesNotLeakIntoVisible();
    testModeDefaultFamiliesMatchSlate();
    testRawModelIdStillResolves();

    if (g_failures > 0) {
        std::cerr << "\n" << g_failures << " test(s) FAILED." << std::endl;
        return 1;
    }
    std::cout << "All mode_family_contracts_tests passed." << std::endl;
    return 0;
}
