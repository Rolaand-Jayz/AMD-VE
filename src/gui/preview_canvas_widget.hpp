#pragma once

#include <QWidget>

class QLabel;
class QComboBox;

namespace ave {

enum class PreviewMode {
    SplitWipe,
    SideBySide,
    OriginalOnly,
    EnhancedOnly,
    FreezeCompare
};

class PreviewCanvasWidget : public QWidget {
    Q_OBJECT

  public:
    explicit PreviewCanvasWidget(QWidget* parent = nullptr);

    void setOriginalPixmap(const QPixmap& pixmap);
    void setEnhancedPixmap(const QPixmap& pixmap);
    void setPreviewMode(PreviewMode mode);
    void setDividerPosition(double pos);
    void setCacheState(const QString& state);

  signals:
    void previewModeChanged(PreviewMode mode);
    void dividerMoved(double position);
    void segmentRenderRequested(double startTime, double endTime);

  private:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    QLabel* placeholderLabel_ = nullptr;
    QComboBox* modeCombo_ = nullptr;
    QLabel* cacheStateLabel_ = nullptr;
    PreviewMode mode_ = PreviewMode::SplitWipe;
    double dividerPos_ = 0.5;
    QPixmap originalPixmap_;
    QPixmap enhancedPixmap_;
};

}  // namespace ave
