#include "ServerListWidget.h"
#include "ServerListItemWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#include "../../core/Instance.h"
#include "../../core/Mod.h"

bc_server_array _serverArray;
bool _serverListFetchPending = false;
std::unordered_map<QString, QByteArray> _serverToIconMap;
char selected_ip[64] = "";
char selected_port[16] = "";

ServerListWidget::ServerListWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _searchTextBox = new QLineEdit(this);
    _searchButton = new QPushButton(this);
    _serverList = new QListWidget(this);
    _serverListRefreshButton =
        new QPushButton(bc_translate("serverlist_refresh_button"), this);

    _searchButton->setText(bc_translate("general_search_button"));
    _searchTextBox->setPlaceholderText(
        bc_translate("general_search_placeholder"));

    _serverList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _serverList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    _layout->addWidget(_searchTextBox, 1, 0, 1, 10);
    _layout->addWidget(_searchButton, 1, 9, 1, 1);
    _layout->addWidget(_serverListRefreshButton, 1, 10, 1, 1);
    _layout->addWidget(_serverList, 2, 0, 1, 11);

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 5);

    setLayout(_layout);

    _serverArray.len = -1;

    connect(_searchButton, SIGNAL(released()), this, SLOT(onSearchButton()));
    connect(_serverListRefreshButton, &QPushButton::released, this, [this]() {
        _serverArray.len = -1;
        initServerList();
    });
    connect(_serverList, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(onServerClicked(QListWidgetItem *)));
    connect(&_serverArrayWatcher, &QFutureWatcher<int>::finished, this,
            [this]() {
                int success = _serverArrayWatcher.future().result();

                if (!success) {
                    _serverList->clear();
                    _serverList->addItem("Can't load server list");
                } else {
                    populateServerList();
                }

                _serverListFetchPending = false;
            });
}

void ServerListWidget::onServerClicked(QListWidgetItem *item) {
    std::pair<QString, QString> serverInfo =
        item->data(Qt::UserRole).value<std::pair<QString, QString>>();

    if (serverInfo.first == "")
        return;

    bc_instance *selectedInstance = bc_instance_select_get();

    if (selectedInstance == NULL) {
        QMessageBox msg;
        msg.setText(bc_translate("serverlist_err_instance_required"));
        msg.setModal(true);
        msg.exec();
        return;
    }

    if (serverInfo.second.compare(selectedInstance->version) != 0) {
        QMessageBox msg;
        msg.setText(bc_translate("serverlist_err_version_incompatible")
                        .arg(serverInfo.second));
        msg.setModal(true);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msg.exec();

        if (ret == QMessageBox::Yes) {
            strcpy(selectedInstance->version,
                   serverInfo.second.toStdString().c_str());
            bc_instance_update(selectedInstance);

            bc_mod_version_array *mods =
                bc_mod_list_installed(selectedInstance->path);

            if (mods->len > 0) {
                for (int i = 0; i < mods->len; i++) {
                    bc_mod_list_remove(selectedInstance->path,
                                       mods->arr[i].path);
                }
            }

            free(mods);
        } else
            return;
    }

    free(selectedInstance);

    QStringList ipSplit = serverInfo.first.split(':');

    if (!ipSplit.isEmpty()) {
        strcpy(selected_ip, ipSplit.first().toStdString().c_str());
        strcpy(selected_port, ipSplit.last().toStdString().c_str());
    }

    emit signal_serverGameLaunch(selected_ip, selected_port);
}

void ServerListWidget::addServerItem(bc_server server) {
    QListWidgetItem *item = new QListWidgetItem();
    ServerListItemWidget *serverItem =
        new ServerListItemWidget(server, _serverToIconMap);
    QVariant q;

    std::pair<QString, QString> serverInfo = {QString(server.connect_socket),
                                              QString(server.connect_version)};
    q.setValue(serverInfo);

    item->setData(Qt::UserRole, q);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setSizeHint(serverItem->sizeHint());

    _serverList->addItem(item);
    _serverList->setItemWidget(item, serverItem);
}

void ServerListWidget::initServerList() {
    if (_serverArray.len == -1 && !_serverListFetchPending) {
        _serverList->clear();
        _serverList->addItem("Loading...");

        _serverListFetchPending = true;
        QFuture<int> serverArrayFuture =
            QtConcurrent::run(bc_server_list, &_serverArray);
        _serverArrayWatcher.setFuture(serverArrayFuture);
    }
}

void ServerListWidget::populateServerList() {
    _serverList->clear();

    for (int i = 0; i < _serverArray.len; i++) {
        if (_serverArray.arr[i].icon[0] != '\0') {
            _serverToIconMap[_serverArray.arr[i].connect_socket] =
                QByteArray(_serverArray.arr[i].icon);
        }

        addServerItem(_serverArray.arr[i]);
    }
}

void ServerListWidget::onSearchButton() {
    _serverList->clear();

    QString search = _searchTextBox->text().trimmed().toLower();

    for (int i = 0; i < _serverArray.len; i++) {
        if (QString(_serverArray.arr[i].name)
                .contains(search, Qt::CaseInsensitive) ||
            QString(_serverArray.arr[i].description)
                .contains(search, Qt::CaseInsensitive) ||
            QString(_serverArray.arr[i].connect_version)
                .contains(search, Qt::CaseInsensitive)) {
            addServerItem(_serverArray.arr[i]);
        }
    }
}

void ServerListWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return) {
        onSearchButton();
    }
}
