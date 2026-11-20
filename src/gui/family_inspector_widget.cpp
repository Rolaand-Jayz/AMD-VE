#include "family_inspector_widget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

namespace ave {

FamilyInspectorWidget::FamilyInspectorWidget(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* modeLabel = new QLabel(tr("<b>Mode</b>"));
    modeCombo_ = new QComboBox;
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0) {
            auto modeId = modeCombo_->itemData(index).toString().toStdString();
            activeModeId_ = modeId;
            emit modeChanged(modeId);
            refreshFamilyCombo();
            refreshCapabilityChips();
            refreshStrengthSliders();
        }
    });
    layout->addWidget(modeLabel);
    layout->addWidget(modeCombo_);

    purposeLabel_ = new QLabel;
    purposeLabel_->setWordWrap(true);
    purposeLabel_->setStyleSheet("color: gray; font-style: italic;");
    layout->addWidget(purposeLabel_);

    auto* familyLabel = new QLabel(tr("<b>Family</b>"));
    familyCombo_ = new QComboBox;
    connect(familyCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0) {
            auto familyId = familyCombo_->itemData(index).toString().toStdString();
            activeFamilyId_ = familyId;
            emit familyChanged(familyId);
            refreshCapabilityChips();
            refreshStrengthSliders();
        }
    });
    layout->addWidget(familyLabel);
    layout->addWidget(familyCombo_);

    familyPurposeLabel_ = new QLabel;
    familyPurposeLabel_->setWordWrap(true);
    familyPurposeLabel_->setStyleSheet("color: gray;");
    layout->addWidget(familyPurposeLabel_);

    capabilitiesGroup_ = new QGroupBox(tr("Capabilities"));
    auto* capsLayout = new QVBoxLayout(capabilitiesGroup_);
    capsLayout->addWidget(new QLabel(tr("Select capabilities to enable")));
    layout->addWidget(capabilitiesGroup_);

    strengthsGroup_ = new QGroupBox(tr("Strength"));
    auto* strLayout = new QVBoxLayout(strengthsGroup_);
    strLayout->addWidget(new QLabel(tr("Adjust strength per capability")));
    layout->addWidget(strengthsGroup_);

    advancedButton_ = new QPushButton(tr("Advanced"));
    advancedButton_->setCheckable(true);
    connect(advancedButton_, &QPushButton::toggled, this, [this](bool checked) {
        advancedGroup_->setVisible(checked);
        emit advancedToggled(checked);
    });
    layout->addWidget(advancedButton_);

    advancedGroup_ = new QGroupBox(tr("Advanced Settings"));
    auto* advLayout = new QVBoxLayout(advancedGroup_);
    advLayout->addWidget(new QLabel(tr("Raw model ID, backend, precision, planner preview")));
    advancedGroup_->setVisible(false);
    layout->addWidget(advancedGroup_);

    layout->addStretch();
    setMinimumWidth(250);
    setMaximumWidth(350);
}

void FamilyInspectorWidget::setModes(const std::vector<EnhancementModeDefinition>& modes) {
    modes_ = modes;
    refreshModeCombo();
}

void FamilyInspectorWidget::setFamilies(const std::vector<ModelFamilyMetadata>& families) {
    families_ = families;
    refreshFamilyCombo();
}

void FamilyInspectorWidget::setActiveMode(const std::string& modeId) {
    activeModeId_ = modeId;
    for (int i = 0; i < modeCombo_->count(); ++i) {
        if (modeCombo_->itemData(i).toString().toStdString() == modeId) {
            modeCombo_->setCurrentIndex(i);
            break;
        }
    }
}

void FamilyInspectorWidget::setActiveFamily(const std::string& familyId) {
    activeFamilyId_ = familyId;
    for (int i = 0; i < familyCombo_->count(); ++i) {
        if (familyCombo_->itemData(i).toString().toStdString() == familyId) {
            familyCombo_->setCurrentIndex(i);
            break;
        }
    }
}

void FamilyInspectorWidget::setCapabilities(const std::vector<std::string>& caps) {
    capabilities_ = caps;
    refreshCapabilityChips();
}

void FamilyInspectorWidget::setStrengthValues(const std::map<std::string, double>& values) {
    strengthValues_ = values;
    refreshStrengthSliders();
}

std::string FamilyInspectorWidget::selectedModeId() const {
    return activeModeId_;
}

std::string FamilyInspectorWidget::selectedFamilyId() const {
    return activeFamilyId_;
}

std::vector<std::string> FamilyInspectorWidget::selectedCapabilities() const {
    return capabilities_;
}

std::map<std::string, double> FamilyInspectorWidget::strengthValues() const {
    return strengthValues_;
}

void FamilyInspectorWidget::refreshModeCombo() {
    modeCombo_->clear();
    for (const auto& mode : modes_) {
        if (!mode.experimental) {
            modeCombo_->addItem(QString::fromStdString(mode.label),
                                QString::fromStdString(mode.id));
        }
    }
    if (modeCombo_->count() > 0) {
        modeCombo_->setCurrentIndex(0);
        activeModeId_ = modeCombo_->itemData(0).toString().toStdString();
        purposeLabel_->setText(QString::fromStdString(
            modes_.empty() ? std::string() : modes_.front().purpose));
    }
}

void FamilyInspectorWidget::refreshFamilyCombo() {
    familyCombo_->clear();
    auto* modePtr = findModeDefinition(activeModeId_);
    std::string defaultFamilyId = modePtr ? modePtr->defaultFamilyId : std::string();

    for (const auto& family : families_) {
        if (family.visibleDefault || family.familyId == defaultFamilyId) {
            familyCombo_->addItem(QString::fromStdString(family.familyName),
                                  QString::fromStdString(family.familyId));
        }
    }

    for (int i = 0; i < familyCombo_->count(); ++i) {
        if (familyCombo_->itemData(i).toString().toStdString() == defaultFamilyId) {
            familyCombo_->setCurrentIndex(i);
            break;
        }
    }
}

void FamilyInspectorWidget::refreshCapabilityChips() {
    auto* oldLayout = capabilitiesGroup_->layout();
    QLayoutItem* item;
    while ((item = oldLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (activeFamilyId_.empty()) {
        oldLayout->addWidget(new QLabel(tr("Select a family to see capabilities")));
        return;
    }

    auto* familyPtr = findFamilyMetadata(activeFamilyId_);
    if (!familyPtr) {
        oldLayout->addWidget(new QLabel(tr("Family not found")));
        return;
    }

    for (const auto& cap : familyPtr->capabilities) {
        auto* check = new QCheckBox(QString::fromStdString(cap));
        check->setChecked(true);
        connect(check, &QCheckBox::toggled, this, [this, cap](bool enabled) {
            emit capabilityToggled(cap, enabled);
        });
        oldLayout->addWidget(check);
    }
}

void FamilyInspectorWidget::refreshStrengthSliders() {
    auto* oldLayout = strengthsGroup_->layout();
    QLayoutItem* item;
    while ((item = oldLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (activeFamilyId_.empty()) return;

    auto* familyPtr = findFamilyMetadata(activeFamilyId_);
    if (!familyPtr) return;

    for (const auto& ctrl : familyPtr->strengthControls) {
        auto* label = new QLabel(QString::fromStdString(ctrl.label));
        auto* slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, 100);
        double defaultVal = ctrl.defaultValue;
        slider->setValue(static_cast<int>(defaultVal * 100.0));

        auto valLabel = new QLabel(QString::number(defaultVal, 'f', 2));
        connect(slider, &QSlider::valueChanged, this, [this, valLabel, id = ctrl.id](int value) {
            double d = value / 100.0;
            valLabel->setText(QString::number(d, 'f', 2));
            emit strengthChanged(id, d);
        });

        oldLayout->addWidget(label);
        oldLayout->addWidget(slider);
        oldLayout->addWidget(valLabel);
    }
}

}  // namespace ave
