#include "preview_canvas_widget.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QVBoxLayout>

namespace ave {

PreviewCanvasWidget::PreviewCanvasWidget(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* toolbar = new QHBoxLayout;
    modeCombo_ = new QComboBox;
    modeCombo_->addItem(tr("Split Wipe"), static_cast<int>(PreviewMode::SplitWipe));
    modeCombo_->addItem(tr("Side by Side"), static_cast<int>(PreviewMode::SideBySide));
    modeCombo_->addItem(tr("Original Only"), static_cast<int>(PreviewMode::OriginalOnly));
    modeCombo_->addItem(tr("Enhanced Only"), static_cast<int>(PreviewMode::EnhancedOnly));
    modeCombo_->addItem(tr("Freeze Compare"), static_cast<int>(PreviewMode::FreezeCompare));
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        auto mode = static_cast<PreviewMode>(modeCombo_->itemData(index).toInt());
        mode_ = mode;
        emit previewModeChanged(mode);
        update();
    });

    cacheStateLabel_ = new QLabel(tr("Cache: empty"));
    cacheStateLabel_->setStyleSheet("color: gray;");

    toolbar->addWidget(modeCombo_);
    toolbar->addStretch();
    toolbar->addWidget(cacheStateLabel_);
    mainLayout->addLayout(toolbar);

    placeholderLabel_ = new QLabel(tr("No preview available.\nLoad a video and select a segment to preview."));
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    placeholderLabel_->setStyleSheet("color: gray; font-size: 14pt;");
    placeholderLabel_->setMinimumSize(640, 360);
    mainLayout->addWidget(placeholderLabel_, 1);

    setMinimumSize(640, 400);
}

void PreviewCanvasWidget::setOriginalPixmap(const QPixmap& pixmap) {
    originalPixmap_ = pixmap;
    update();
}

void PreviewCanvasWidget::setEnhancedPixmap(const QPixmap& pixmap) {
    enhancedPixmap_ = pixmap;
    update();
}

void PreviewCanvasWidget::setPreviewMode(PreviewMode mode) {
    mode_ = mode;
    for (int i = 0; i < modeCombo_->count(); ++i) {
        if (modeCombo_->itemData(i).toInt() == static_cast<int>(mode)) {
            modeCombo_->setCurrentIndex(i);
            break;
        }
    }
    update();
}

void PreviewCanvasWidget::setDividerPosition(double pos) {
    dividerPos_ = qBound(0.0, pos, 1.0);
    update();
}

void PreviewCanvasWidget::setCacheState(const QString& state) {
    cacheStateLabel_->setText(tr("Cache: ") + state);
}

void PreviewCanvasWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    bool hasOriginal = !originalPixmap_.isNull();
    bool hasEnhanced = !enhancedPixmap_.isNull();

    if (!hasOriginal && !hasEnhanced) {
        return;
    }

    QRect drawRect = rect().adjusted(0, modeCombo_->height() + 4, 0, 0);

    auto drawCentered = [&](const QPixmap& pix) {
        QRect target = drawRect;
        QSize scaled = pix.size().scaled(target.size(), Qt::KeepAspectRatio);
        int x = target.x() + (target.width() - scaled.width()) / 2;
        int y = target.y() + (target.height() - scaled.height()) / 2;
        painter.drawPixmap(x, y, scaled.width(), scaled.height(), pix);
    };

    switch (mode_) {
        case PreviewMode::OriginalOnly:
            if (hasOriginal) drawCentered(originalPixmap_);
            break;
        case PreviewMode::EnhancedOnly:
            if (hasEnhanced) drawCentered(enhancedPixmap_);
            break;
        case PreviewMode::SplitWipe:
        case PreviewMode::FreezeCompare:
        case PreviewMode::SideBySide:
            if (hasOriginal) drawCentered(originalPixmap_);
            break;
    }

    if (hasOriginal || hasEnhanced) {
        placeholderLabel_->hide();
    }
}

void PreviewCanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == PreviewMode::SplitWipe && (event->buttons() & Qt::LeftButton)) {
        double pos = static_cast<double>(event->position().x()) / width();
        setDividerPosition(pos);
        emit dividerMoved(dividerPos_);
    }
}

void PreviewCanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (mode_ == PreviewMode::SplitWipe) {
        double pos = static_cast<double>(event->position().x()) / width();
        setDividerPosition(pos);
        emit dividerMoved(dividerPos_);
    }
}

}  // namespace ave
