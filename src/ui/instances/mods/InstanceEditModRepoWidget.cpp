#include "InstanceEditModRepoWidget.h"
#include "../../Betacraft.h"

#include <QtWidgets>

#include "../../../core/Mod.h"

bc_mod_array _modArray;

InstanceEditModRepoWidget::InstanceEditModRepoWidget(QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _searchTextBox = new QLineEdit(this);
    _searchButton =
        new QPushButton(bc_translate("general_search_button"), this);
    _backButton = new QPushButton(bc_translate("mods_repository_back"), this);
    _modList = new QTreeWidget(this);
    _versionsWidget = new InstanceEditModVersionsWidget();

    _modList->setIndentation(0);
    _modList->headerItem()->setText(0, bc_translate("mods_mod_name_column"));

    _searchTextBox->setPlaceholderText(
        bc_translate("general_search_placeholder"));

    _modList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _modList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    _layout->addWidget(_backButton, 0, 0, 1, 1);
    _layout->addWidget(_searchTextBox, 1, 0, 1, 10);
    _layout->addWidget(_searchButton, 1, 10, 1, 1);
    _layout->addWidget(_modList, 2, 0, 1, 11);

    _layout->setSpacing(5);

    _layout->setAlignment(Qt::AlignTop);

    setLayout(_layout);

    connect(_modList, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
            SLOT(onModClicked(QTreeWidgetItem *, int)));
    connect(_searchButton, SIGNAL(released()), this,
            SLOT(onSearchButtonClicked()));
    connect(_backButton, &QPushButton::released, this, [this]() {
        _versionsWidget->close();
        emit signal_BackButtonClicked();
    });
    connect(_versionsWidget,
            &InstanceEditModVersionsWidget::signal_ModDownloadStarted, this,
            [this]() {
                _versionsWidget->close();
                emit signal_ModDownloadStarted();
            });
    connect(_versionsWidget,
            &InstanceEditModVersionsWidget::signal_ModDownloadFinished, this,
            [this]() {
                _versionsWidget->close();
                emit signal_ModDownloadFinished();
            });
}

void InstanceEditModRepoWidget::onSearchButtonClicked() {
    _modList->clear();

    QString search = _searchTextBox->text().trimmed().toLower();

    for (int i = 0; i < _modArray.len; i++) {
        if (QString(_modArray.arr[i].name)
                .contains(search, Qt::CaseInsensitive)) {
            modListItemAdd(_modArray.arr[i]);
        }
    }
}

void InstanceEditModRepoWidget::setInstance(bc_instance instance) {
    _instance = instance;

    int rowCount = _modList->model()->rowCount();
    if (rowCount == 0) {
        bc_mod_array *mods = bc_mod_list(instance.version);
        _modArray.len = mods->len;

        for (int i = 0; i < _modArray.len; i++) {
            _modArray.arr[i] = mods->arr[i];

            modListItemAdd(_modArray.arr[i]);
        }

        free(mods);
    }
}

void InstanceEditModRepoWidget::modListItemAdd(bc_mod mod) {
    QTreeWidgetItem *item = new QTreeWidgetItem();

    QVariant versions;
    versions.setValue(mod.versions);

    item->setData(0, Qt::UserRole, versions);
    item->setText(0, mod.name);
    _modList->addTopLevelItem(item);
}

void InstanceEditModRepoWidget::onModClicked(QTreeWidgetItem *item,
                                             int column) {
    bc_mod_version_array versions =
        item->data(0, Qt::UserRole).value<bc_mod_version_array>();

    _versionsWidget->populateList(versions);
    _versionsWidget->setInstance(_instance);
    _versionsWidget->show();
}

void InstanceEditModRepoWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return) {
        onSearchButtonClicked();
    }
}
