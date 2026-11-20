#include "temporal_forge_panel.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace {

struct PresetInfo {
    ave::TemporalForgePreset preset;
    const char* label;
    const char* description;
};

const PresetInfo kPresets[] = {
    {ave::TemporalForgePreset::Accurate,     "Accurate",      "Maximum temporal quality. Slow, uses full history window."},
    {ave::TemporalForgePreset::Balanced,     "Balanced",      "Good quality/speed tradeoff. Default for general content."},
    {ave::TemporalForgePreset::Recover,      "Recover",       "Aggressive temporal reconstruction for degraded sources."},
    {ave::TemporalForgePreset::AnimeLineArt, "Anime / Line Art", "Optimized for cel animation and line-art content."},
    {ave::TemporalForgePreset::FilmPreserve, "Film Preserve",  "Conservative processing to preserve film grain and texture."},
    {ave::TemporalForgePreset::CleanDigital, "Clean Digital",  "For clean digital sources. Minimal temporal blending."},
};

const char* kDebugArtifacts[] = {
    "motion_field_visualization",
    "depth_visualization",
    "reactive_mask",
    "occlusion_mask",
    "confidence_map",
    "ai_allowed_mask",
    "scene_reset_markers",
    "history_weight_map",
};

const char* kDebugLabels[] = {
    "Motion Field",
    "Depth Map",
    "Reactive Mask",
    "Occlusion Mask",
    "Confidence Map",
    "AI Allowed Mask",
    "Scene Reset Markers",
    "History Weight Map",
};

}  // namespace

namespace ave {

TemporalForgePanel::TemporalForgePanel(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    createPresetSelector();
    createDebugOverlayGroup();

    mainLayout->addWidget(presetCombo_);
    mainLayout->addWidget(presetDescriptionLabel_);
    mainLayout->addStretch();
    mainLayout->addWidget(debugGroup_);
}

void TemporalForgePanel::createPresetSelector() {
    presetCombo_ = new QComboBox(this);
    for (const auto& p : kPresets) {
        presetCombo_->addItem(QString::fromUtf8(p.label),
                              QString::fromStdString(toString(p.preset)));
    }

    presetDescriptionLabel_ = new QLabel(this);
    presetDescriptionLabel_->setWordWrap(true);
    presetDescriptionLabel_->setStyleSheet("color: #888; font-size: 11px;");

    connect(presetCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                if (index < 0 || index >= 6) return;
                activePreset_ = kPresets[index].preset;
                presetDescriptionLabel_->setText(QString::fromUtf8(kPresets[index].description));
                emit presetChanged(activePreset_);
            });

    setPreset(TemporalForgePreset::Balanced);
}

void TemporalForgePanel::createDebugOverlayGroup() {
    debugGroup_ = new QGroupBox("Debug Overlays", this);
    auto* layout = new QVBoxLayout(debugGroup_);

    for (int i = 0; i < 8; ++i) {
        auto* cb = new QCheckBox(QString::fromUtf8(kDebugLabels[i]), debugGroup_);
        const std::string artifactName(kDebugArtifacts[i]);
        connect(cb, &QCheckBox::toggled, this, [this, artifactName](bool checked) {
            emit debugArtifactToggled(artifactName, checked);
        });
        layout->addWidget(cb);
        debugEntries_.push_back({artifactName, cb});
    }
}

void TemporalForgePanel::setPreset(TemporalForgePreset preset) {
    activePreset_ = preset;
    const std::string name = toString(preset);
    for (int i = 0; i < presetCombo_->count(); ++i) {
        if (presetCombo_->itemData(i).toString().toStdString() == name) {
            presetCombo_->setCurrentIndex(i);
            return;
        }
    }
}

TemporalForgePreset TemporalForgePanel::selectedPreset() const {
    return activePreset_;
}

void TemporalForgePanel::setDebugArtifactEnabled(const std::string& artifactName, bool enabled) {
    for (auto& entry : debugEntries_) {
        if (entry.artifactName == artifactName) {
            entry.checkBox->setChecked(enabled);
            return;
        }
    }
}

bool TemporalForgePanel::isDebugArtifactEnabled(const std::string& artifactName) const {
    for (const auto& entry : debugEntries_) {
        if (entry.artifactName == artifactName) {
            return entry.checkBox->isChecked();
        }
    }
    return false;
}

std::vector<std::string> TemporalForgePanel::enabledDebugArtifacts() const {
    std::vector<std::string> result;
    for (const auto& entry : debugEntries_) {
        if (entry.checkBox->isChecked()) {
            result.push_back(entry.artifactName);
        }
    }
    return result;
}

}  // namespace ave
