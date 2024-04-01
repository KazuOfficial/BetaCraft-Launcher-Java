#include "SettingsJavaWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#ifdef _WIN32
QString _javaExecName = "java.exe (*.exe)";
#elif defined(__linux__) || defined(__APPLE__)
QString _javaExecName = "java (java *.plugin)";
#endif

#include "../../core/Betacraft.h"
#include "../../core/JavaInstallations.h"

char _currentDownloadUrl[256];
bool _downloadRecommendedJava = false;

SettingsJavaWidget::SettingsJavaWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _addInstallationButton = new QPushButton(this);
    _removeInstallationButton = new QPushButton(this);
    _installJavaButton = new QPushButton(this);
    _setAsDefaultButton = new QPushButton(this);
    _cancelDownloadButton = new QPushButton("Cancel", this);
    _javaInstallList = new QComboBox(this);
    _javaInstallLabel = new QLabel(this);
    _javaTreeView = new QTreeView(this);
    _javaTreeItemModel = new QStandardItemModel(this);
    _progressBar = new QProgressBar(this);
    _progressTimer = new QTimer(this);

    _addInstallationButton->setText(bc_translate("settings_java_add_button"));
    _removeInstallationButton->setText(
        bc_translate("settings_java_remove_button"));
    _installJavaButton->setText(
        bc_translate("settings_java_download_install_button"));
    _setAsDefaultButton->setText(
        bc_translate("settings_java_set_default_button"));
    _javaInstallLabel->setText(bc_translate("settings_java_download_label"));

    bc_jrepo_download_array *javaRepoList = bc_jrepo_get_all_system();

    for (int i = 0; i < javaRepoList->len; i++) {
        QVariant q;
        q.setValue(QString(javaRepoList->arr[i].url));
        _javaInstallList->addItem(QString(javaRepoList->arr[i].full_version),
                                  q);
    }

    free(javaRepoList);

    _javaTreeView->setIndentation(0);
    _javaTreeView->setHeader(new QHeaderView(Qt::Horizontal));
    _javaTreeView->header()->setSectionsClickable(true);
    _javaTreeView->header()->setDefaultAlignment(Qt::AlignLeft);
    _javaTreeView->header()->setStretchLastSection(true);
    _javaTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populateJavaTreeView();

    if (!betacraft_online) {
        _installJavaButton->setDisabled(1);
        _javaInstallList->setDisabled(1);
    }

    _progressBar->setVisible(false);
    _progressBar->setTextVisible(true);
    _progressBar->setAlignment(Qt::AlignCenter);
    _progressBar->setValue(0);
    _cancelDownloadButton->setVisible(0);

    _javaTreeView->setModel(_javaTreeItemModel);

    _layout->addWidget(_addInstallationButton, 0, 0, 1, 1);
    _layout->addWidget(_removeInstallationButton, 0, 1, 1, 1);
    _layout->addWidget(_javaInstallLabel, 0, 8, 1, 1, Qt::AlignRight);
    _layout->addWidget(_javaInstallList, 0, 9, 1, 1);
    _layout->addWidget(_installJavaButton, 0, 10, 1, 1);
    _layout->addWidget(_javaTreeView, 1, 0, 1, 11);
    _layout->addWidget(_progressBar, 2, 0, 1, 10);
    _layout->addWidget(_cancelDownloadButton, 2, 10, 1, 1);
    _layout->addWidget(_setAsDefaultButton, 3, 0, 1, 1);

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(5);
    _layout->setContentsMargins(10, 10, 10, 10);

    setLayout(_layout);

    connect(_installJavaButton, SIGNAL(released()), this,
            SLOT(onJavaInstallClicked()));
    connect(_setAsDefaultButton, SIGNAL(released()), this,
            SLOT(onSetAsDefaultClicked()));
    connect(_removeInstallationButton, SIGNAL(released()), this,
            SLOT(onRemoveButtonClicked()));
    connect(_addInstallationButton, SIGNAL(released()), this,
            SLOT(onAddButtonClicked()));
    connect(&_watcher, &QFutureWatcher<void>::finished, this,
            &SettingsJavaWidget::downloadFinished);
    connect(_progressTimer, SIGNAL(timeout()), this,
            SLOT(JavaInstallProgressUpdate()));
    // connect(_cancelDownloadButton, &QPushButton::released, this,
    //         [this]() { bc_network_cancel = 1; });
}

void SettingsJavaWidget::downloadRecommendedJava(QString javaVersion) {
    _downloadRecommendedJava = true;
    _javaInstallList->setCurrentText(javaVersion);
    onJavaInstallClicked();
}

void SettingsJavaWidget::JavaInstallProgressUpdate() {
    // bc_download_progress progress = bc_network_progress;
    //
    // QString progressString(progress.filename);
    // progressString = progressString.split('/').last();
    //
    // if (progress.totalToDownload > 0) {
    //     _progressBar->setRange(0, progress.totalToDownload);
    //     _progressBar->setValue(progress.nowDownloaded);
    //
    //     if (progress.totalToDownload == progress.nowDownloaded) {
    //         _progressBar->setFormat(bc_translate("settings_java_extracting"));
    //         return;
    //     }
    //
    //     progressString +=
    //         " - " + QString::number(progress.nowDownloadedMb, 'f', 2) + "MB";
    //     progressString +=
    //         " / " + QString::number(progress.totalToDownloadMb, 'f', 2) + "MB";
    // } else if (progress.nowDownloaded > 0) {
    //     _progressBar->setRange(0, 100);
    //     _progressBar->setValue(100);
    //
    //     progressString +=
    //         " - " + QString::number(progress.nowDownloadedMb, 'f', 2) + "MB";
    // }

    // QString filename(progress.filename);
    // filename = bc_translate("downloading_undefined") + " " + progressString;

    // _progressBar->setFormat(filename);
}

void SettingsJavaWidget::onJavaInstallClicked() {
    _installJavaButton->setDisabled(true);
    _installJavaButton->setText(bc_translate("settings_java_installing"));

    QString url = _javaInstallList->currentData(Qt::UserRole).value<QString>();
    snprintf(_currentDownloadUrl, sizeof(_currentDownloadUrl), "%s",
             url.toStdString().c_str());

    QFuture<void> future =
        QtConcurrent::run(bc_java_download, _currentDownloadUrl);
    _watcher.setFuture(future);

    _progressBar->setVisible(1);
    _cancelDownloadButton->setVisible(1);
    _progressTimer->start(1000);

    emit signal_toggleTabs();
}

void SettingsJavaWidget::downloadFinished() {
    populateJavaTreeView();

    _installJavaButton->setDisabled(false);
    _installJavaButton->setText(
        bc_translate("settings_java_download_install_button"));

    if (_downloadRecommendedJava) {
        _downloadRecommendedJava = false;

        int rowCount = _javaTreeView->model()->rowCount() - 1;

        QModelIndex index = _javaTreeItemModel->item(rowCount, 1)->index();
        _javaTreeView->setCurrentIndex(index);

        onSetAsDefaultClicked();
    }

    _progressBar->setVisible(0);
    _cancelDownloadButton->setVisible(0);
    _progressTimer->stop();

    emit signal_toggleTabs();
}

void SettingsJavaWidget::onSetAsDefaultClicked() {
    int row = _javaTreeView->currentIndex().row();

    if (row > -1) {
        QString path = _javaTreeItemModel->item(row, 1)->text();
        bc_jinst_select(path.toStdString().c_str());
        populateJavaTreeView();
    }
}

void SettingsJavaWidget::onAddButtonClicked() {
    QString title(bc_translate("settings_java_add_title"));
    QString path = QFileDialog::getOpenFileName(this, title, "", _javaExecName);

    if (!path.isNull()) {
        if (path.endsWith(QString(".plugin"))) {
            path.append("/Contents/Home/bin/java");
        }

        bc_jinst_add(path.toStdString().c_str());
        populateJavaTreeView();
    }
}

void SettingsJavaWidget::onRemoveButtonClicked() {
    int row = _javaTreeView->currentIndex().row();

    if (row > -1) {
        QString path = _javaTreeItemModel->item(row, 1)->text();
        bc_jinst_remove(path.toStdString().c_str());
        populateJavaTreeView();
    }
}

void SettingsJavaWidget::populateJavaTreeView() {
    _javaTreeItemModel->clear();

    QStringList headers;

    headers << bc_translate("settings_java_version_column");
    headers << bc_translate("settings_java_path_column");

    _javaTreeItemModel->setHorizontalHeaderLabels(headers);

    bc_jinst_array *javaInstallationsList = bc_jinst_get_all();
    char *selected = bc_jinst_select_get();

    for (int i = 0; i < javaInstallationsList->len; i++) {
        QStandardItem *version =
            new QStandardItem(QString(javaInstallationsList->arr[i].version));
        QStandardItem *path =
            new QStandardItem(QString(javaInstallationsList->arr[i].path));

        _javaTreeItemModel->setItem(i, 0, version);
        _javaTreeItemModel->setItem(i, 1, path);

        if (selected != NULL &&
            strcmp(selected, javaInstallationsList->arr[i].path) == 0) {
            version->setBackground(QBrush(QColor(86, 184, 91, 255)));
            path->setBackground(QBrush(QColor(86, 184, 91, 255)));
        }
    }

    free(javaInstallationsList);
    free(selected);
}
