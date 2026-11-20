#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QProgressBar;

namespace ave {

class TopRailWidget : public QWidget {
    Q_OBJECT

  public:
    explicit TopRailWidget(QWidget* parent = nullptr);

    void setInputPath(const QString& path);
    void setOutputPath(const QString& path);
    void setActiveFamily(const QString& familyId);
    void setBackendStatus(const QString& status);
    void setProgress(int overallPct, const QString& msg);
    void setRunning(bool running);

  signals:
    void inputBrowseRequested();
    void outputBrowseRequested();
    void previewRequested();
    void renderRequested();
    void pauseRequested();
    void cancelRequested();

  private:
    QLineEdit* inputPathEdit_ = nullptr;
    QLineEdit* outputPathEdit_ = nullptr;
    QLabel* backendStatusLabel_ = nullptr;
    QLabel* familyChipLabel_ = nullptr;
    QPushButton* previewButton_ = nullptr;
    QPushButton* renderButton_ = nullptr;
    QPushButton* pauseButton_ = nullptr;
    QPushButton* cancelButton_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
};

}  // namespace ave
