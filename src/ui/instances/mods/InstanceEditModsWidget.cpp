#include "InstanceEditModsWidget.h"
#include "../../Betacraft.h"

#include <QtWidgets>

bool isRepoWidgetVisible = false;

InstanceEditModsWidget::InstanceEditModsWidget(QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _modList = new QTreeWidget(this);
    _moveUpButton = new QPushButton(bc_translate("mods_control_move_up"), this);
    _moveDownButton =
        new QPushButton(bc_translate("mods_control_move_down"), this);
    _removeButton = new QPushButton(bc_translate("mods_control_remove"), this);
    _modRepoButton =
        new QPushButton(bc_translate("mods_control_mod_repository"), this);
    _installFabricButton =
        new QPushButton(bc_translate("mods_control_install_fabric"), this);
    _addToMinecraftJarButton =
        new QPushButton(bc_translate("mods_control_add_to_minecraftjar"), this);
    _replaceMinecraftJarButton = new QPushButton(
        bc_translate("mods_control_replace_minecraftjar"), this);
    _openMinecraftDirectoryButton =
        new QPushButton(bc_translate("mods_control_open_dotminecraft"), this);
    _editModGroup = new QGroupBox(this);
    _modLoaderGroup = new QGroupBox(this);
    _gameDirectoryGroup = new QGroupBox(this);
    _modRepoGroup = new QGroupBox(this);
    _editModLayout = new QGridLayout();
    _modLoaderLayout = new QGridLayout();
    _gameDirectoryLayout = new QGridLayout();
    _modRepoLayout = new QGridLayout();
    _progressBar = new QProgressBar(this);
    _progressTimer = new QTimer(this);
    _instanceEditModRepoWidget = new InstanceEditModRepoWidget();

    _modList->setIndentation(0);
    _modList->headerItem()->setText(0, bc_translate("mods_mod_name_column"));
    _modList->headerItem()->setText(1, bc_translate("mods_mod_version_column"));
    _modList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    _editModLayout->addWidget(_moveUpButton);
    _editModLayout->addWidget(_moveDownButton);
    _editModLayout->addWidget(_removeButton);
    _editModGroup->setLayout(_editModLayout);

    _modRepoLayout->addWidget(_modRepoButton);
    _modRepoGroup->setLayout(_modRepoLayout);

    _installFabricButton->setEnabled(0);

    _modLoaderLayout->addWidget(_installFabricButton);
    _modLoaderGroup->setLayout(_modLoaderLayout);

    _gameDirectoryLayout->addWidget(_addToMinecraftJarButton);
    _gameDirectoryLayout->addWidget(_replaceMinecraftJarButton);
    _gameDirectoryLayout->addWidget(_openMinecraftDirectoryButton);
    _gameDirectoryGroup->setLayout(_gameDirectoryLayout);

    _instanceEditModRepoWidget->setVisible(0);

    _progressBar->setVisible(false);
    _progressBar->setTextVisible(true);
    _progressBar->setAlignment(Qt::AlignCenter);
    _progressBar->setValue(0);

    _layout->addWidget(_instanceEditModRepoWidget, 0, 0, 11, 10);
    _layout->addWidget(_modList, 0, 0, 11, 10);
    _layout->addWidget(_editModGroup, 0, 11, 1, 1);
    _layout->addWidget(_modRepoGroup, 1, 11, 1, 1);
    _layout->addWidget(_modLoaderGroup, 2, 11, 1, 1);
    _layout->addWidget(_gameDirectoryGroup, 3, 11, 1, 1);
    _layout->addWidget(_progressBar, 12, 0, 1, 10);

    _layout->setSpacing(5);
    _layout->setContentsMargins(10, 10, 10, 10);

    setLayout(_layout);

    QSignalMapper *mapper = new QSignalMapper();
    mapper->setMapping(_moveUpButton, -1);
    mapper->setMapping(_moveDownButton, 1);

    connect(mapper, &QSignalMapper::mappedInt, this,
            &InstanceEditModsWidget::onMoveButtonClicked);
    connect(_modRepoButton, SIGNAL(released()), this,
            SLOT(onModRepoButtonClicked()));
    connect(_openMinecraftDirectoryButton, SIGNAL(released()), this,
            SLOT(onOpenMinecraftDirectoryClicked()));
    connect(_replaceMinecraftJarButton, SIGNAL(released()), this,
            SLOT(onReplaceMinecraftJarButtonClicked()));
    connect(_removeButton, SIGNAL(released()), this,
            SLOT(onRemoveButtonClicked()));
    connect(_addToMinecraftJarButton, SIGNAL(released()), this,
            SLOT(onAddToMinecraftJarButtonClicked()));
    connect(_moveUpButton, SIGNAL(released()), mapper, SLOT(map()));
    connect(_moveDownButton, SIGNAL(released()), mapper, SLOT(map()));
    connect(_instanceEditModRepoWidget, SIGNAL(signal_BackButtonClicked()),
            this, SLOT(onModRepoButtonClicked()));
    connect(_instanceEditModRepoWidget, SIGNAL(signal_ModDownloadStarted()),
            this, SLOT(onModDownloadStarted()));
    connect(_progressTimer, SIGNAL(timeout()), this,
            SLOT(ModInstallProgressBarUpdate()));
    connect(_instanceEditModRepoWidget,
            &InstanceEditModRepoWidget::signal_ModDownloadFinished, this,
            [this]() {
                _progressBar->setVisible(0);
                _progressTimer->stop();
                populateModList();
            });
}

void InstanceEditModsWidget::onReplaceMinecraftJarButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        tr(bc_translate("mods_control_replace_minecraftjar")
               .toStdString()
               .c_str()),
        "/", tr("Jar Files (*.jar)"));

    if (!path.isNull()) {
        bc_mod_replace_jar(path.toStdString().c_str(), _instance.path,
                           _instance.version);
        populateModList();
    }
}

void InstanceEditModsWidget::onAddToMinecraftJarButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        tr(bc_translate("mods_control_add_to_minecraftjar")
               .toStdString()
               .c_str()),
        "/", tr("Zip Files (*.zip)"));

    if (!path.isNull()) {
        bc_mod_add(path.toStdString().c_str(), _instance.path,
                   _instance.version);
        populateModList();
    }
}

void InstanceEditModsWidget::onMoveButtonClicked(int direction) {
    int row = _modList->currentIndex().row();
    int rowCount = _modList->model()->rowCount();
    int moveDirection = row + direction;

    if (moveDirection > -1 && moveDirection != rowCount) {
        if (rowCount > 2 && direction == 1)
            moveDirection--;
        QTreeWidgetItem *item = _modList->takeTopLevelItem(row);
        QTreeWidgetItem *itemPrev = _modList->takeTopLevelItem(moveDirection);

        _modList->insertTopLevelItem(moveDirection, item);
        _modList->insertTopLevelItem(row, itemPrev);
        _modList->setCurrentItem(item);
    };
}

bc_mod_version_array *InstanceEditModsWidget::getSettings() {
    bc_mod_version_array *order = new bc_mod_version_array;
    order->len = _modList->model()->rowCount();

    for (int i = 0; i < order->len; i++) {
        QModelIndex index = _modList->model()->index(i, 0);
        bc_mod_version mod = _modList->itemFromIndex(index)
                                 ->data(0, Qt::UserRole)
                                 .value<bc_mod_version>();
        order->arr[i] = mod;
    }

    return order;
}

void InstanceEditModsWidget::onRemoveButtonClicked() {
    if (_modList->currentIndex().row() > -1) {
        bc_mod_version v = _modList->currentItem()
                               ->data(0, Qt::UserRole)
                               .value<bc_mod_version>();
        bc_mod_list_remove(_instance.path, v.path);
        populateModList();
    }
}

void InstanceEditModsWidget::ModInstallProgressBarUpdate() {
    // bc_download_progress progress = bc_network_progress;
    //
    // if (progress.filename[0] == '\0') {
    //     _progressBar->setFormat(bc_translate("downloading_undefined") + "...");
    // }
    //
    // QString progressString(progress.filename);
    // progressString = progressString.split('/').last();
    //
    // if (progress.totalToDownload > 0) {
    //     _progressBar->setRange(0, progress.totalToDownload);
    //     _progressBar->setValue(progress.nowDownloaded);
    //     progressString +=
    //         " - " + QString::number(progress.nowDownloadedMb, 'f', 2) + "MB";
    //     progressString +=
    //         " / " + QString::number(progress.totalToDownloadMb, 'f', 2) + "MB";
    // } else if (progress.nowDownloaded > 0) {
    //     _progressBar->setRange(0, 100);
    //     _progressBar->setValue(100);
    //     progressString +=
    //         " - " + QString::number(progress.nowDownloadedMb, 'f', 2) + "MB";
    // }
    //
    // QString filename(progress.filename);
    // filename = bc_translate("downloading_undefined") + ": " + progressString;
    //
    // _progressBar->setFormat(filename);
}

void InstanceEditModsWidget::onModDownloadStarted() {
    onModRepoButtonClicked();

    _progressBar->setVisible(1);
    _progressTimer->start(1000);
}

void InstanceEditModsWidget::onModRepoButtonClicked() {
    _modList->setVisible(isRepoWidgetVisible);
    _editModGroup->setVisible(isRepoWidgetVisible);
    _modRepoGroup->setVisible(isRepoWidgetVisible);
    _modLoaderGroup->setVisible(isRepoWidgetVisible);
    _gameDirectoryGroup->setVisible(isRepoWidgetVisible);

    if (!isRepoWidgetVisible)
        _layout->setContentsMargins(0, 0, 0, 0);
    else
        _layout->setContentsMargins(10, 10, 10, 10);

    _instanceEditModRepoWidget->setVisible(!isRepoWidgetVisible);

    isRepoWidgetVisible = !isRepoWidgetVisible;
    populateModList();
}

void InstanceEditModsWidget::onOpenMinecraftDirectoryClicked() {
    QString path(_instance.path);
    path.chop(16); // chop bc_instance.json

    QUrl qurl(path);
    qurl.setScheme("file");
    QDesktopServices::openUrl(qurl);
}

void InstanceEditModsWidget::setInstance(bc_instance instance) {
    _instance = instance;

    _instanceEditModRepoWidget->setInstance(instance);
    populateModList();
}

void InstanceEditModsWidget::populateModList() {
    _modList->clear();
    bc_mod_version_array *mods = bc_mod_list_installed(_instance.path);

    for (int i = 0; i < mods->len; i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString(mods->arr[i].name));
        item->setText(1, QString(mods->arr[i].version));

        QVariant q;
        bc_mod_version modCopy = mods->arr[i];
        q.setValue(modCopy);
        item->setData(0, Qt::UserRole, q);

        _modList->addTopLevelItem(item);
    }

    free(mods);
}
