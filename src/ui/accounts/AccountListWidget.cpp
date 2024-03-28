#include "AccountListWidget.h"
#include "AccountListItemWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#include "../../core/Betacraft.h"

AccountListWidget::AccountListWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _accountList = new QListWidget(this);
    _addAccountButton = new QPushButton(this);
    _addAccountWidget = new AddAccountWidget();

    _addAccountButton->setText(bc_translate("accounts_add_button"));

    _accountList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _accountList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _accountList->setContextMenuPolicy(Qt::CustomContextMenu);
    _accountList->viewport()->installEventFilter(this);

    populateList();

    _layout->addWidget(_addAccountButton, 0, 0, 1, 11);
    _layout->addWidget(_accountList, 1, 0, 1, 11);

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 5);

    setLayout(_layout);

    connect(_addAccountWidget, SIGNAL(signal_accountAddSuccess()), this,
            SLOT(onAccountAdded()));
    connect(_accountList, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(onAccountClicked(QListWidgetItem *)));
    connect(_addAccountButton, &QPushButton::released, this,
            [this]() { _addAccountWidget->show(); });
}

void AccountListWidget::onAccountAdded() {
    _addAccountWidget->close();

    emit signal_accountUpdate();
    populateList();
}

void AccountListWidget::onAccountClicked(QListWidgetItem *item) {
    QString uuidNew = item->data(Qt::UserRole).toString();

    if (uuidNew.compare(_selectedAccount) != 0) {
        bc_account_select(uuidNew.toStdString().c_str());
        bc_account_refresh();
        emit signal_accountUpdate();
        populateList();
    }
}

void AccountListWidget::populateList() {
    _accountList->clear();

    bc_account_array *account_list = bc_account_list();
    bc_account *account_selected = bc_account_select_get();

    if (account_selected != NULL) {
        _selectedAccount = QString(account_selected->uuid);
    } else {
        _selectedAccount = "";
    }

    for (int i = 0; i < account_list->len; i++) {
        QListWidgetItem *item = new QListWidgetItem();
        AccountListItemWidget *accountItem =
            new AccountListItemWidget(account_list->arr[i]);

        QVariant q;
        QString uuid(account_list->arr[i].uuid);
        q.setValue(uuid);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setSizeHint(accountItem->sizeHint());
        item->setData(Qt::UserRole, q);

        _accountList->addItem(item);
        _accountList->setItemWidget(item, accountItem);

        if (_selectedAccount.compare(account_list->arr[i].uuid) == 0) {
            accountItem->_name->setStyleSheet(
                "QLabel { color : rgba(0,0,0,255); }");
            accountItem->_uuid->setStyleSheet(
                "QLabel { color : rgba(0,0,0,255); }");
            item->setBackground(QBrush(QColor(86, 184, 91, 255)));
        }
    }

    free(account_list);
    free(account_selected);
}

void AccountListWidget::menuTrigger(QAction *action) {
    QString atype = action->whatsThis();
    QModelIndex modelIndex = action->data().toModelIndex();

    QString uuid =
        _accountList->itemFromIndex(modelIndex)->data(Qt::UserRole).toString();

    if (atype == "remove") {
        bc_account_remove(uuid.toStdString().c_str());
        emit signal_accountUpdate();
        populateList();
    }
}

bool AccountListWidget::eventFilter(QObject *source, QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease &&
        source == _accountList->viewport()) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::RightButton) {
            QListWidgetItem *item =
                _accountList->itemAt(mouseEvent->position().toPoint());

            if (item != NULL) {
                QModelIndex index = _accountList->indexFromItem(item);

                _actRemove =
                    new QAction(bc_translate("accounts_action_remove"), this);
                _actRemove->setData(index);
                _actRemove->setWhatsThis("remove");

                QMenu *newMenu = new QMenu();
                newMenu->addActions({_actRemove});

                connect(newMenu, SIGNAL(triggered(QAction *)), this,
                        SLOT(menuTrigger(QAction *)));
                newMenu->exec(mouseEvent->globalPosition().toPoint());

                return true;
            }
        }
    }

    return false;
}
