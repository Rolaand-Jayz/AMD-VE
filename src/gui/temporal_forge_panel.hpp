#pragma once

#include <string>
#include <vector>

#include <QWidget>

#include "ave/temporal_frame_types.hpp"

class QComboBox;
class QCheckBox;
class QLabel;
class QGroupBox;

namespace ave {

class TemporalForgePanel : public QWidget {
    Q_OBJECT

  public:
    explicit TemporalForgePanel(QWidget* parent = nullptr);

    void setPreset(TemporalForgePreset preset);
    TemporalForgePreset selectedPreset() const;

    void setDebugArtifactEnabled(const std::string& artifactName, bool enabled);
    bool isDebugArtifactEnabled(const std::string& artifactName) const;
    std::vector<std::string> enabledDebugArtifacts() const;

  signals:
    void presetChanged(TemporalForgePreset preset);
    void debugArtifactToggled(const std::string& artifactName, bool enabled);

  private:
    void createPresetSelector();
    void createDebugOverlayGroup();

    QComboBox* presetCombo_ = nullptr;
    QLabel* presetDescriptionLabel_ = nullptr;
    QGroupBox* debugGroup_ = nullptr;

    struct DebugCheckEntry {
        std::string artifactName;
        QCheckBox* checkBox = nullptr;
    };
    std::vector<DebugCheckEntry> debugEntries_;

    TemporalForgePreset activePreset_ = TemporalForgePreset::Balanced;
};

}  // namespace ave
