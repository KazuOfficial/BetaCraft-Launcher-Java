#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMessageBox>
#include <QWidget>
#include <QtConcurrent>

#include "AboutWidget.h"
#include "ConsoleLogWidget.h"
#include "accounts/AccountListWidget.h"
#include "instances/InstanceListWidget.h"
#include "servers/ServerListWidget.h"
#include "settings/SettingsWidget.h"

class QPixmap;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QTabWidget;
class QTextEdit;
class QPushButton;
class QGridLayout;
class QKeyEvent;
class QProgressBar;
class QTimer;
class QProcess;
class InstanceList;

class MainWindow : public QWidget {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);

  protected:
    void keyPressEvent(QKeyEvent *e);

  private slots:
    void onInstanceUpdate();
    void onAccountUpdate();
    void launchingGameFinished();
    void onMenuIndexChanged(int index);
    void updateGameProgress();
    void onToggleTabs();
    void onToggleDiscordRPC();
    void launchGameJoinServer(const char *ip, const char *port);
    void updateCheck();

  private:
    QGridLayout *_mainLayout;
    QTextEdit *_changelog;
    QLabel *_logo;
    QPushButton *_playButton;
    QWidget *_bottomBackground;
    QTabWidget *_menu;
    InstanceListWidget *_instanceListWidget;
    ServerListWidget *_serverListWidget;
    AccountListWidget *_accountsWidget;
    SettingsWidget *_settingsWidget;
    AboutWidget *_aboutWidget;
    ConsoleLogWidget *_consoleLog;
    QLabel *_instanceLabel;
    QLabel *_accountLabel;
    QProgressBar *_progressBar;
    QFutureWatcher<void> _watcher;
    QFutureWatcher<int> _updateWatcher;
    QTimer *_gameProgressTimer;
    QTimer *_discordLoopTimer;
    QProcess *_gameProcess;
    QMessageBox *_messageBox;
    QString _username;
    QString _instanceSelectedVersion;
    QString _instanceSelectedName;
    int _instanceSelectedShowLog;
    int _instanceSelectedKeepOpen;
    void updateInstanceLabel();
    bool recommendedJavaCheck();
    void startDiscordRPC();
    void launchGame(const char *ip, const char *port);
    void connectSignalsToSlots();
    void initProgressBar();
    void initMainLayout();
    void initWindow();
    void initChangelog();
    void initPlayButton();
    void initMenu();
    void initObjects();
    void onStartup();
};

#endif
