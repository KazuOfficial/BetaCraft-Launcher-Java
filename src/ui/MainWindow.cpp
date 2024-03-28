#include "MainWindow.h"

#include "Betacraft.h"
#include <QtWidgets>

#include "../core/Betacraft.h"
#include "../core/Discord.h"
#include "../core/JavaInstallations.h"
#include "../core/Network.h"
#include "../core/VersionList.h"

char _updateVersion[BETACRAFT_MAX_UPDATE_TAG_SIZE] = "";

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    initObjects();
    initChangelog();
    initPlayButton();
    initMenu();

    initProgressBar();
    initMainLayout();
    initWindow();

    connectSignalsToSlots();
    startDiscordRPC();
    onInstanceUpdate();
    onAccountUpdate();
    updateInstanceLabel();

    onStartup();
}

void MainWindow::onStartup() {
    if (betacraft_online) {
        QFuture<char *> future =
            QtConcurrent::run(bc_network_get,
                              "https://raw.githubusercontent.com/betacraftuk/"
                              "betacraft-launcher/v2/CHANGELOG.md",
                              "");
        _changelogWatcher.setFuture(future);

        QFuture<int> updateFuture =
            QtConcurrent::run(bc_update_check, _updateVersion);
        _updateWatcher.setFuture(updateFuture);

        bc_account_refresh();
        bc_account_register_forbidden_all();
    }
}

void MainWindow::initObjects() {
    _mainLayout = new QGridLayout(this);
    _menu = new QTabWidget(this);
    _bottomBackground = new QWidget(this);
    _progressBar = new QProgressBar(this);
    _changelog = new QTextEdit(this);
    _gameProgressTimer = new QTimer(this);
    _discordLoopTimer = new QTimer(this);
    _gameProcess = new QProcess(this);
    _messageBox = new QMessageBox(this);
    _logo = new QLabel(_bottomBackground);
    _playButton = new QPushButton(_bottomBackground);
    _instanceLabel = new QLabel(_bottomBackground);
    _accountLabel = new QLabel(_bottomBackground);

    _consoleLog = new ConsoleLogWidget();
    _instanceListWidget = new InstanceListWidget();
    _aboutWidget = new AboutWidget();
    _settingsWidget = new SettingsWidget();

    if (betacraft_online) {
        _accountsWidget = new AccountListWidget();
        _serverListWidget = new ServerListWidget();
    }
}

void MainWindow::initMenu() {
    _menu->setStyleSheet("QTabWidget::pane { border: 0; }");
    _menu->addTab(_changelog, bc_translate("tab_changelog"));
    _menu->addTab(_instanceListWidget, bc_translate("tab_instances"));

    if (betacraft_online) {
        _menu->addTab(_serverListWidget, bc_translate("tab_server_list"));
        _menu->addTab(_accountsWidget, bc_translate("tab_accounts"));
    }

    _menu->addTab(_settingsWidget, bc_translate("tab_settings"));
    _menu->addTab(_aboutWidget, bc_translate("tab_about"));

    if (betacraft_online) {
        _menu->addTab(new QWidget(this), bc_translate("tab_donate"));
    }
}

void MainWindow::initPlayButton() {
    _playButton->setFixedWidth(120);
    _playButton->setFixedHeight(50);
    _playButton->setText(bc_translate("play_button"));
}

void MainWindow::initChangelog() {
    _changelog->setReadOnly(true);
    _changelog->setStyleSheet(
        ".QTextEdit { background-image: url(':/assets/stone.png'); border: 0; "
        "color: #e0d0d0; font-size: 15px; padding-left: 10px; }");
    _changelog->viewport()->setCursor(Qt::ArrowCursor);

    if (!betacraft_online) {
        _changelog->setMarkdown("Offline");
        return;
    }

    _changelog->setMarkdown("Loading...");
}

void MainWindow::initWindow() {
    QString winTitle = "Betacraft " + QString("(%1)").arg(BETACRAFT_VERSION);
    setWindowTitle(winTitle);
    resize(850, 480);
    setMinimumSize(850, 480);
    setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::initMainLayout() {
    _instanceLabel->setStyleSheet(
        ".QLabel { color: white; padding-bottom: 10px; }");
    _bottomBackground->setStyleSheet(
        ".QWidget { background-image: url(':/assets/dirt.png'); }");
    _logo->setPixmap(QPixmap(":/assets/logo.png"));

    _mainLayout->setSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _mainLayout->addWidget(_menu, 0, 0, 1, 11);
    _mainLayout->addWidget(_progressBar, 1, 0, 1, 11);
    _mainLayout->addWidget(_bottomBackground, 2, 0, 1, 11);
    _mainLayout->addWidget(_logo, 2, 1, 1, 1);
    _mainLayout->addWidget(_instanceLabel, 2, 1, 1, 1, Qt::AlignBottom);
    _mainLayout->addWidget(_playButton, 2, 9, 1, 1);

    _mainLayout->setColumnMinimumWidth(10, 30);
    _mainLayout->setColumnMinimumWidth(0, 30);

    _mainLayout->setRowMinimumHeight(0, 100);
    _mainLayout->setRowMinimumHeight(2, 100);

    setLayout(_mainLayout);
}

void MainWindow::initProgressBar() {
    _progressBar->setVisible(false);
    _progressBar->setTextVisible(true);
    _progressBar->setAlignment(Qt::AlignCenter);
    _progressBar->setValue(0);
    _progressBar->setRange(0, 100);
}

void MainWindow::loadChangelog() {
    char *response = _changelogWatcher.future().result();
    _changelog->setMarkdown(QString(response));
    free(response);
}

void MainWindow::connectSignalsToSlots() {
    connect(_gameProgressTimer, SIGNAL(timeout()), this,
            SLOT(updateGameProgress()));
    connect(_playButton, &QPushButton::released, this,
            [this]() { launchGame("", ""); });
    connect(_instanceListWidget, SIGNAL(signal_instanceUpdate()), this,
            SLOT(onInstanceUpdate()));
    connect(_menu, SIGNAL(currentChanged(int)), this,
            SLOT(onMenuIndexChanged(int)));
    connect(_discordLoopTimer, &QTimer::timeout, this,
            [this]() { bc_discord_loop(); });
    connect(_settingsWidget, SIGNAL(signal_toggleTabs()), this,
            SLOT(onToggleTabs()));
    connect(_settingsWidget, SIGNAL(signal_toggleDiscordRPC()), this,
            SLOT(onToggleDiscordRPC()));
    connect(&_watcher, &QFutureWatcher<void>::finished, this,
            &MainWindow::launchingGameFinished);
    connect(&_updateWatcher, &QFutureWatcher<int>::finished, this,
            &MainWindow::updateCheck);

    if (betacraft_online) {
        connect(&_changelogWatcher, &QFutureWatcher<char *>::finished, this,
                &MainWindow::loadChangelog);
        connect(_serverListWidget,
                SIGNAL(signal_serverGameLaunch(const char *, const char *)),
                this, SLOT(launchGameJoinServer(const char *, const char *)));
        connect(_accountsWidget, SIGNAL(signal_accountUpdate()), this,
                SLOT(onAccountUpdate()));
    }
}

void MainWindow::updateCheck() {
    int success = _updateWatcher.future().result();

    if (!success) {
        _messageBox->setText("Update check failed");
        _messageBox->setModal(true);
        _messageBox->show();
        return;
    }

    if (_updateVersion[0] != '\0') {
        QString url =
            "https://github.com/betacraftuk/betacraft-launcher/releases";
        QString message = bc_translate("update_notice_message");

        _messageBox->setWindowTitle("Betacraft");
        _messageBox->setText(message.arg(QString(_updateVersion)).arg(url));
        _messageBox->setModal(true);
        _messageBox->setTextFormat(Qt::RichText);

        _messageBox->show();
    }
}

void MainWindow::launchGameJoinServer(const char *ip, const char *port) {
    onInstanceUpdate();
    launchGame(ip, port);
}

void MainWindow::startDiscordRPC() {
    int discord = bc_discord_init();

    if (discord) {
        _discordLoopTimer->start(2000);
    }
}

void MainWindow::onToggleDiscordRPC() {
    if (_discordLoopTimer->isActive()) {
        _discordLoopTimer->stop();
        bc_discord_stop();
    } else {
        startDiscordRPC();
    }
}

void MainWindow::onMenuIndexChanged(int index) {
    if (betacraft_online) {
        switch (index) {
        case 2:
            _serverListWidget->initServerList();
            break;
        case 6:
            QDesktopServices::openUrl(
                QUrl("https://www.patreon.com/BetacraftLauncher"));
            _menu->setCurrentIndex(0);
            break;
        default:
            break;
        }
    }
}

void MainWindow::onToggleTabs() {
    _playButton->setDisabled(_playButton->isEnabled());
    _menu->tabBar()->setDisabled(_menu->tabBar()->isEnabled());
}

void MainWindow::launchingGameFinished() {
    _playButton->setDisabled(false);
    _menu->setDisabled(false);

    _progressBar->setVisible(0);
    _progressBar->setValue(0);
    _consoleLog->close();

    onAccountUpdate();
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return) {
        launchGame("", "");
    }
}

void MainWindow::updateGameProgress() {
    bc_download_progress downloadProgress = bc_network_progress;
    bc_progress gameProgress = bc_instance_run_progress();
    _progressBar->setValue(gameProgress.progress);

    if (_progressBar->value() == 100) {
        _progressBar->setFormat(bc_translate("running_game"));
        _gameProgressTimer->stop();

        if (_instanceSelectedShowLog) {
            _consoleLog->show();
        }

        if (!_instanceSelectedKeepOpen) {
            MainWindow::close();
        }
    } else {
        if (downloadProgress.filename[0] == '\0') {
            _progressBar->setFormat(bc_translate("running_game"));
            return;
        }

        QString progressString("");

        switch (gameProgress.download_type) {
        case BC_DOWNLOAD_TYPE_VERSION:
            progressString += bc_translate("downloading_version") + ": ";
            break;
        case BC_DOWNLOAD_TYPE_LIBRARIES:
            progressString += bc_translate("downloading_libraries") + ": ";
            break;
        case BC_DOWNLOAD_TYPE_ASSETS:
            progressString += bc_translate("downloading_assets") + ": ";
            break;
        default:
            progressString += bc_translate("downloading_undefined") + ": ";
            break;
        }

        progressString += QString(downloadProgress.filename).split('/').last();

        if (downloadProgress.nowDownloaded > 0) {
            progressString +=
                " - " +
                QString::number(downloadProgress.nowDownloadedMb, 'f', 2) +
                "MB";
        }

        if (downloadProgress.totalToDownload > 0) {
            progressString +=
                " / " +
                QString::number(downloadProgress.totalToDownloadMb, 'f', 2) +
                "MB";
        }

        if (gameProgress.cur > 0 && gameProgress.total > 0) {
            progressString += QString(" (%1 / %2)")
                                  .arg(gameProgress.cur)
                                  .arg(gameProgress.total);
        }

        _progressBar->setFormat(progressString);
    }
}

bool MainWindow::recommendedJavaCheck() {
    char *recommendedJavaVersion = bc_jrepo_get_recommended(
        _instanceSelectedVersion.toStdString().c_str());
    char *selectedJava = bc_jinst_select_get();

    char *parsedRecommended = bc_jrepo_parse_version(recommendedJavaVersion);
    char *parsedSelected = NULL;

    bool startGame = true;

    if (selectedJava != NULL) {
        bc_jinst *jinst = bc_jinst_get(selectedJava);
        parsedSelected = bc_jrepo_parse_version(jinst->version);
        free(jinst);
    }

    if (selectedJava == NULL ||
        strcmp(parsedRecommended, parsedSelected) != 0) {
        bc_jinst_array *jinstList = bc_jinst_get_all();

        char *matchingJavaInstallationPath = NULL;

        for (int i = 0; i < jinstList->len; i++) {
            char *parsedVersion =
                bc_jrepo_parse_version(jinstList->arr[i].version);

            if (strcmp(parsedRecommended, parsedVersion) == 0) {
                matchingJavaInstallationPath = jinstList->arr[i].path;
                free(parsedVersion);
                break;
            }

            free(parsedVersion);
        }

        QMessageBox messageBox;
        QString message = bc_translate("java_version_not_supported");

        message += matchingJavaInstallationPath == NULL
                       ? bc_translate("java_version_install_recommended")
                             .arg(recommendedJavaVersion)
                       : bc_translate("java_version_switch_to_recommended")
                             .arg(parsedRecommended);

        messageBox.setWindowTitle("Betacraft");
        messageBox.setText(message);
        messageBox.setModal(true);
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        int ret = messageBox.exec();

        if (ret == QMessageBox::Yes) {
            if (matchingJavaInstallationPath != NULL) {
                bc_jinst_select(matchingJavaInstallationPath);
            } else {
                _menu->setCurrentIndex(4);
                _settingsWidget->downloadRecommendedJava(
                    QString(recommendedJavaVersion));
                startGame = false;
            }
        }

        free(jinstList);
    }

    free(parsedRecommended);
    free(recommendedJavaVersion);

    if (selectedJava != NULL) {
        free(selectedJava);
        free(parsedSelected);
    }

    return startGame;
}

void MainWindow::launchGame(const char *ip, const char *port) {
    if (!bc_versionlist_download(
            _instanceSelectedVersion.toStdString().c_str()) ||
        !recommendedJavaCheck()) {
        return;
    }

    _menu->setCurrentIndex(0);
    _playButton->setDisabled(true);
    _menu->setDisabled(true);

    QFuture<void> future = QtConcurrent::run(bc_instance_run, ip, port);
    _watcher.setFuture(future);

    _progressBar->setVisible(1);
    _progressBar->setFormat(bc_translate("running_game_reading_version_file"));

    _gameProgressTimer->start(1000);

    QString userStatus = "Demo user";
    if (!_username.isNull()) {
        userStatus = _username;
    }

    bc_discord_activity_update(userStatus.toStdString().c_str(),
                               _instanceSelectedVersion.toStdString().c_str());
}

void MainWindow::onInstanceUpdate() {
    bc_instance *instance = bc_instance_select_get();

    if (instance != NULL) {
        _instanceSelectedVersion = QString(instance->version);
        _instanceSelectedName = QString(instance->name);
        _instanceSelectedShowLog = instance->show_log;
        _instanceSelectedKeepOpen = instance->keep_open;

        _playButton->setEnabled(1);
        free(instance);
    } else {
        _instanceSelectedVersion = NULL;
        _instanceSelectedName = NULL;
        _instanceSelectedShowLog = 0;
        _instanceSelectedKeepOpen = 0;
        _playButton->setEnabled(0);
    }

    updateInstanceLabel();
}

void MainWindow::onAccountUpdate() {
    bc_account *account = bc_account_select_get();

    if (account != NULL) {
        _username = QString(account->username);

        bc_discord_activity_update(_username.toStdString().c_str(),
                                   "Testing Betacraft v2");
        free(account);
    } else {
        bc_discord_activity_update("", "Testing Betacraft v2");
    }

    updateInstanceLabel();
}

void MainWindow::updateInstanceLabel() {
    QString label;

    if (!_username.isNull()) {
        label = _username;
    }

    if (!_instanceSelectedVersion.isNull()) {
        if (!_username.isNull()) {
            label += " | ";
        }

        label += QString("%1 (%2)")
                     .arg(_instanceSelectedName)
                     .arg(_instanceSelectedVersion);
    }

    _instanceLabel->setText(label);
}
