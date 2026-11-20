#include "top_rail_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>

namespace ave {

TopRailWidget::TopRailWidget(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);

    auto* inputLabel = new QLabel(tr("Input:"));
    inputPathEdit_ = new QLineEdit;
    inputPathEdit_->setReadOnly(true);
    inputPathEdit_->setPlaceholderText(tr("No file loaded"));
    auto* inputBrowse = new QPushButton(tr("Browse"));
    connect(inputBrowse, &QPushButton::clicked, this, &TopRailWidget::inputBrowseRequested);

    layout->addWidget(inputLabel);
    layout->addWidget(inputPathEdit_, 1);
    layout->addWidget(inputBrowse);

    auto* outputLabel = new QLabel(tr("Output:"));
    outputPathEdit_ = new QLineEdit;
    outputPathEdit_->setReadOnly(true);
    outputPathEdit_->setPlaceholderText(tr("Auto"));
    auto* outputBrowse = new QPushButton(tr("Browse"));
    connect(outputBrowse, &QPushButton::clicked, this, &TopRailWidget::outputBrowseRequested);

    layout->addWidget(outputLabel);
    layout->addWidget(outputPathEdit_, 1);
    layout->addWidget(outputBrowse);

    backendStatusLabel_ = new QLabel(tr("Backend: auto"));
    layout->addWidget(backendStatusLabel_);

    familyChipLabel_ = new QLabel;
    layout->addWidget(familyChipLabel_);

    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setMaximumWidth(200);
    progressBar_->setVisible(false);
    layout->addWidget(progressBar_);

    previewButton_ = new QPushButton(tr("Preview"));
    connect(previewButton_, &QPushButton::clicked, this, &TopRailWidget::previewRequested);
    layout->addWidget(previewButton_);

    renderButton_ = new QPushButton(tr("Render"));
    connect(renderButton_, &QPushButton::clicked, this, &TopRailWidget::renderRequested);
    layout->addWidget(renderButton_);

    pauseButton_ = new QPushButton(tr("Pause"));
    pauseButton_->setEnabled(false);
    connect(pauseButton_, &QPushButton::clicked, this, &TopRailWidget::pauseRequested);
    layout->addWidget(pauseButton_);

    cancelButton_ = new QPushButton(tr("Cancel"));
    cancelButton_->setEnabled(false);
    connect(cancelButton_, &QPushButton::clicked, this, &TopRailWidget::cancelRequested);
    layout->addWidget(cancelButton_);
}

void TopRailWidget::setInputPath(const QString& path) {
    inputPathEdit_->setText(path);
}

void TopRailWidget::setOutputPath(const QString& path) {
    outputPathEdit_->setText(path);
}

void TopRailWidget::setActiveFamily(const QString& familyId) {
    familyChipLabel_->setText(familyId.isEmpty() ? QString() : tr("Family: ") + familyId);
}

void TopRailWidget::setBackendStatus(const QString& status) {
    backendStatusLabel_->setText(tr("Backend: ") + status);
}

void TopRailWidget::setProgress(int overallPct, const QString& msg) {
    progressBar_->setVisible(overallPct > 0 || !msg.isEmpty());
    progressBar_->setValue(overallPct);
    progressBar_->setFormat(msg);
}

void TopRailWidget::setRunning(bool running) {
    previewButton_->setEnabled(!running);
    renderButton_->setEnabled(!running);
    pauseButton_->setEnabled(running);
    cancelButton_->setEnabled(running);
    progressBar_->setVisible(running);
}

}  // namespace ave
