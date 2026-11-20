#pragma once

#include <map>
#include <string>
#include <vector>

#include <QWidget>

#include "ave/mode_definition.hpp"
#include "ave/family_metadata.hpp"

class QComboBox;
class QLabel;
class QSlider;
class QPushButton;
class QGroupBox;

namespace ave {

class FamilyInspectorWidget : public QWidget {
    Q_OBJECT

  public:
    explicit FamilyInspectorWidget(QWidget* parent = nullptr);

    void setModes(const std::vector<ave::EnhancementModeDefinition>& modes);
    void setFamilies(const std::vector<ave::ModelFamilyMetadata>& families);
    void setActiveMode(const std::string& modeId);
    void setActiveFamily(const std::string& familyId);
    void setCapabilities(const std::vector<std::string>& caps);
    void setStrengthValues(const std::map<std::string, double>& values);

    std::string selectedModeId() const;
    std::string selectedFamilyId() const;
    std::vector<std::string> selectedCapabilities() const;
    std::map<std::string, double> strengthValues() const;

  signals:
    void modeChanged(const std::string& modeId);
    void familyChanged(const std::string& familyId);
    void capabilityToggled(const std::string& cap, bool enabled);
    void strengthChanged(const std::string& name, double value);
    void advancedToggled(bool visible);

  private:
    void refreshModeCombo();
    void refreshFamilyCombo();
    void refreshCapabilityChips();
    void refreshStrengthSliders();

    QComboBox* modeCombo_ = nullptr;
    QLabel* purposeLabel_ = nullptr;
    QComboBox* familyCombo_ = nullptr;
    QLabel* familyPurposeLabel_ = nullptr;
    QGroupBox* capabilitiesGroup_ = nullptr;
    QGroupBox* strengthsGroup_ = nullptr;
    QPushButton* advancedButton_ = nullptr;
    QGroupBox* advancedGroup_ = nullptr;

    std::vector<ave::EnhancementModeDefinition> modes_;
    std::vector<ave::ModelFamilyMetadata> families_;
    std::string activeModeId_;
    std::string activeFamilyId_;
    std::vector<std::string> capabilities_;
    std::map<std::string, double> strengthValues_;
};

}  // namespace ave
