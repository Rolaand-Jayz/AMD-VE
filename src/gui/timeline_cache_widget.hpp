#pragma once

#include <QWidget>

class QLabel;
class QSlider;
class QPushButton;

namespace ave {

class TimelineCacheWidget : public QWidget {
    Q_OBJECT

  public:
    explicit TimelineCacheWidget(QWidget* parent = nullptr);

    void setDuration(double seconds);
    void setCurrentPosition(double seconds);
    void setCachedRange(double startSec, double endSec);
    void setCacheState(const QString& state);

  signals:
    void positionChanged(double seconds);
    void segmentRenderRequested(double startSec, double endSec);

  private:
    QSlider* seekSlider_ = nullptr;
    QLabel* timeLabel_ = nullptr;
    QLabel* cacheStateLabel_ = nullptr;
    QPushButton* renderSegmentButton_ = nullptr;
    QLabel* cacheLaneLabel_ = nullptr;

    double duration_ = 0.0;
    double cachedStart_ = -1.0;
    double cachedEnd_ = -1.0;
};

}  // namespace ave
