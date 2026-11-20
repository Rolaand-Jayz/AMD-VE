#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "ave/enhancement_profile.hpp"
#include "ave/fusion_planner.hpp"
#include "ave/mode_definition.hpp"
#include "ave/family_metadata.hpp"

namespace {

void testSerializeDeserializeBasic() {
    ave::EnhancementProfile profile;
    profile.modeId = "restore_video";
    profile.familyId = "clearreality_fast";
    profile.capabilities = {"deblur", "denoise"};
    profile.strengthValues = {{"deblur_strength", 0.8}, {"denoise_strength", 0.5}};
    profile.backendPreference = "migraphx";

    std::string text = ave::serializeProfile(profile);
    assert(!text.empty());
    assert(text.find("mode=restore_video") != std::string::npos);
    assert(text.find("family=clearreality_fast") != std::string::npos);
    assert(text.find("backend=migraphx") != std::string::npos);

    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->modeId == "restore_video");
    assert(loaded->familyId == "clearreality_fast");
    assert(loaded->capabilities.size() == 2);
    assert(loaded->backendPreference == "migraphx");
    assert(loaded->strengthValues.size() == 2);

    std::cout << "  PASS testSerializeDeserializeBasic\n";
}

void testSerializeDeserializeRoundTrip() {
    ave::EnhancementProfile original;
    original.modeId = "upscale_video";
    original.familyId = "openproteus_quality";
    original.capabilities = {"upscale", "sharpen"};
    original.strengthValues = {{"sharpness", 0.7}};
    original.backendPreference = "vulkan";
    original.precision = "fp16";
    original.outputPath = "/tmp/output.mp4";

    std::string text = ave::serializeProfile(original);
    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->modeId == original.modeId);
    assert(loaded->familyId == original.familyId);
    assert(loaded->capabilities == original.capabilities);
    assert(loaded->backendPreference == original.backendPreference);
    assert(loaded->precision.has_value() && *loaded->precision == "fp16");
    assert(loaded->outputPath == original.outputPath);

    std::cout << "  PASS testSerializeDeserializeRoundTrip\n";
}

void testDeserializeCommentsAndBlankLines() {
    std::string text =
        "# This is a comment\n"
        "\n"
        "mode=anime_line_art\n"
        "  \n"
        "# Another comment\n"
        "family=anime_line_art\n";

    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->modeId == "anime_line_art");
    assert(loaded->familyId == "anime_line_art");

    std::cout << "  PASS testDeserializeCommentsAndBlankLines\n";
}

void testDeserializeMalformedLine() {
    std::string text = "this_is_not_valid\n";

    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(!loaded.has_value());
    assert(!error.empty());

    std::cout << "  PASS testDeserializeMalformedLine\n";
}

void testProfileValidationEmpty() {
    ave::EnhancementProfile profile;
    std::string error;
    assert(!profile.isValid(error));
    assert(!error.empty());

    std::cout << "  PASS testProfileValidationEmpty\n";
}

void testProfileValidationInvalidMode() {
    ave::EnhancementProfile profile;
    profile.modeId = "nonexistent_mode";
    std::string error;
    assert(!profile.isValid(error));

    std::cout << "  PASS testProfileValidationInvalidMode\n";
}

void testProfileValidationValidMode() {
    ave::EnhancementProfile profile;
    profile.modeId = "restore_video";
    std::string error;
    assert(profile.isValid(error));

    std::cout << "  PASS testProfileValidationValidMode\n";
}

void testProfileValidationInvalidFamily() {
    ave::EnhancementProfile profile;
    profile.familyId = "nonexistent_family";
    std::string error;
    assert(!profile.isValid(error));

    std::cout << "  PASS testProfileValidationInvalidFamily\n";
}

void testProfileValidationValidFamily() {
    ave::EnhancementProfile profile;
    profile.familyId = "clearreality_fast";
    std::string error;
    assert(profile.isValid(error));

    std::cout << "  PASS testProfileValidationValidFamily\n";
}

void testIsEmpty() {
    ave::EnhancementProfile empty;
    assert(empty.isEmpty());

    ave::EnhancementProfile notEmpty;
    notEmpty.modeId = "restore_video";
    assert(!notEmpty.isEmpty());

    std::cout << "  PASS testIsEmpty\n";
}

void testProfileToPipelineIntent() {
    ave::EnhancementProfile profile;
    profile.modeId = "restore_video";
    profile.familyId = "clearreality_fast";
    profile.capabilities = {"deblur", "denoise"};
    profile.strengthValues = {{"deblur_strength", 0.8}};

    auto intent = ave::profileToPipelineIntent(profile);
    assert(intent.modeId == "restore_video");
    assert(intent.familyId == "clearreality_fast");
    assert(intent.requestedCapabilities.size() == 2);
    assert(intent.strengthValues.size() == 1);

    std::cout << "  PASS testProfileToPipelineIntent\n";
}

void testSaveAndLoadProfile() {
    std::string tmpPath = std::filesystem::temp_directory_path() / "ave_test_profile.aveprofile";

    ave::EnhancementProfile profile;
    profile.modeId = "upscale_video";
    profile.familyId = "openproteus_quality";
    profile.capabilities = {"upscale"};
    profile.backendPreference = "migraphx";

    std::string saveError;
    assert(ave::saveProfileToFile(tmpPath, profile, saveError));
    assert(std::filesystem::exists(tmpPath));

    std::string loadError;
    auto loaded = ave::loadProfileFromFile(tmpPath, loadError);
    assert(loaded.has_value());
    assert(loaded->modeId == "upscale_video");
    assert(loaded->familyId == "openproteus_quality");
    assert(loaded->backendPreference == "migraphx");

    std::remove(tmpPath.c_str());

    std::cout << "  PASS testSaveAndLoadProfile\n";
}

void testLoadNonexistentProfile() {
    std::string error;
    auto loaded = ave::loadProfileFromFile("/nonexistent/path/profile.aveprofile", error);
    assert(!loaded.has_value());
    assert(!error.empty());

    std::cout << "  PASS testLoadNonexistentProfile\n";
}

void testSaveInvalidProfile() {
    std::string tmpPath = std::filesystem::temp_directory_path() / "ave_test_invalid.aveprofile";

    ave::EnhancementProfile invalid;
    std::string error;
    assert(!ave::saveProfileToFile(tmpPath, invalid, error));
    assert(!error.empty());

    std::remove(tmpPath.c_str());

    std::cout << "  PASS testSaveInvalidProfile\n";
}

void testSerializeEscaping() {
    ave::EnhancementProfile profile;
    profile.modeId = "restore_video";
    profile.outputPath = "file with=equals.mp4";

    std::string text = ave::serializeProfile(profile);
    assert(text.find("output=") != std::string::npos);

    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->outputPath == "file with=equals.mp4");

    std::cout << "  PASS testSerializeEscaping\n";
}

void testRawModelProfile() {
    ave::EnhancementProfile profile;
    profile.rawModelId = "custom_model_v2";
    std::string error;
    assert(profile.isValid(error));

    std::string text = ave::serializeProfile(profile);
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->rawModelId.has_value());
    assert(*loaded->rawModelId == "custom_model_v2");

    std::cout << "  PASS testRawModelProfile\n";
}

void testStrengthsDeserialization() {
    std::string text =
        "mode=restore_video\n"
        "family=clearreality_fast\n"
        "strengths=deblur_strength:0.8,denoise_strength:0.5\n";

    std::string error;
    auto loaded = ave::deserializeProfile(text, error);
    assert(loaded.has_value());
    assert(loaded->strengthValues.size() == 2);
    assert(loaded->strengthValues.count("deblur_strength") == 1);
    assert(loaded->strengthValues.count("denoise_strength") == 1);

    std::cout << "  PASS testStrengthsDeserialization\n";
}

}  // namespace

int main() {
    std::cout << "Running enhancement_profile_tests...\n";

    testSerializeDeserializeBasic();
    testSerializeDeserializeRoundTrip();
    testDeserializeCommentsAndBlankLines();
    testDeserializeMalformedLine();
    testProfileValidationEmpty();
    testProfileValidationInvalidMode();
    testProfileValidationValidMode();
    testProfileValidationInvalidFamily();
    testProfileValidationValidFamily();
    testIsEmpty();
    testProfileToPipelineIntent();
    testSaveAndLoadProfile();
    testLoadNonexistentProfile();
    testSaveInvalidProfile();
    testSerializeEscaping();
    testRawModelProfile();
    testStrengthsDeserialization();

    std::cout << "All enhancement_profile_tests passed.\n";
    return 0;
}
