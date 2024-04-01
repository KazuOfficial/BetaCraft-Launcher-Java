#ifndef SETTINGSJAVAWIDGET_H
#define SETTINGSJAVAWIDGET_H

#include <QWidget>
#include <QtConcurrent>

class QGridLayout;
class QPushButton;
class QComboBox;
class QLabel;
class QTreeView;
class QStandardItemModel;
class QProgressBar;
class QTimer;

class SettingsJavaWidget : public QWidget {
    Q_OBJECT
  public:
    explicit SettingsJavaWidget(QWidget *parent = nullptr);
    void downloadRecommendedJava(QString javaVersion);
    QStandardItemModel *_javaTreeItemModel;

  private slots:
    void onJavaInstallClicked();
    void onSetAsDefaultClicked();
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void downloadFinished();
    void JavaInstallProgressUpdate();

  signals:
    void signal_toggleTabs();

  private:
    QGridLayout *_layout;
    QPushButton *_addInstallationButton;
    QPushButton *_removeInstallationButton;
    QPushButton *_installJavaButton;
    QPushButton *_setAsDefaultButton;
    QPushButton *_cancelDownloadButton;
    QComboBox *_javaInstallList;
    QLabel *_javaInstallLabel;
    QTreeView *_javaTreeView;
    QFutureWatcher<void> _watcher;
    QProgressBar *_progressBar;
    QTimer *_progressTimer;
    void populateJavaTreeView();
};

#endif
