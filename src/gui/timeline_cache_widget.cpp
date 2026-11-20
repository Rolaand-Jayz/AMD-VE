#include "timeline_cache_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

namespace ave {

TimelineCacheWidget::TimelineCacheWidget(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);

    auto* controlsRow = new QHBoxLayout;

    seekSlider_ = new QSlider(Qt::Horizontal);
    seekSlider_->setRange(0, 1000);
    seekSlider_->setValue(0);
    connect(seekSlider_, &QSlider::sliderMoved, this, [this](int value) {
        if (duration_ > 0.0) {
            double pos = (static_cast<double>(value) / 1000.0) * duration_;
            emit positionChanged(pos);
        }
    });

    timeLabel_ = new QLabel(tr("00:00 / 00:00"));
    renderSegmentButton_ = new QPushButton(tr("Preview Segment"));
    connect(renderSegmentButton_, &QPushButton::clicked, this, [this]() {
        if (duration_ > 0.0) {
            double start = (static_cast<double>(seekSlider_->value()) / 1000.0) * duration_;
            double end = std::min(start + 10.0, duration_);
            emit segmentRenderRequested(start, end);
        }
    });

    controlsRow->addWidget(seekSlider_, 1);
    controlsRow->addWidget(timeLabel_);
    controlsRow->addWidget(renderSegmentButton_);
    layout->addLayout(controlsRow);

    auto* cacheRow = new QHBoxLayout;
    cacheStateLabel_ = new QLabel(tr("Cache: empty"));
    cacheStateLabel_->setStyleSheet("color: gray;");
    cacheLaneLabel_ = new QLabel;
    cacheLaneLabel_->setMinimumHeight(16);
    cacheLaneLabel_->setStyleSheet("background-color: #333; border: 1px solid #555;");
    cacheRow->addWidget(cacheStateLabel_);
    cacheRow->addWidget(cacheLaneLabel_, 1);
    layout->addLayout(cacheRow);

    setMaximumHeight(80);
}

void TimelineCacheWidget::setDuration(double seconds) {
    duration_ = seconds;
    int mins = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    timeLabel_->setText(QString("00:00 / %1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0')));
}

void TimelineCacheWidget::setCurrentPosition(double seconds) {
    if (duration_ > 0.0) {
        seekSlider_->blockSignals(true);
        seekSlider_->setValue(static_cast<int>((seconds / duration_) * 1000.0));
        seekSlider_->blockSignals(false);
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int totalMins = static_cast<int>(duration_) / 60;
        int totalSecs = static_cast<int>(duration_) % 60;
        timeLabel_->setText(QString("%1:%2 / %3:%4")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'))
            .arg(totalMins, 2, 10, QChar('0'))
            .arg(totalSecs, 2, 10, QChar('0')));
    }
}

void TimelineCacheWidget::setCachedRange(double startSec, double endSec) {
    cachedStart_ = startSec;
    cachedEnd_ = endSec;
    cacheStateLabel_->setText(tr("Cache: cached %1s-%2s")
        .arg(static_cast<int>(startSec))
        .arg(static_cast<int>(endSec)));
}

void TimelineCacheWidget::setCacheState(const QString& state) {
    cacheStateLabel_->setText(tr("Cache: ") + state);
}

}  // namespace ave
