#include "AddAccountWidget.h"
#include "AddAccountMicrosoftWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

AddAccountWidget::AddAccountWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _menu = new QTabWidget(this);
    _microsoftWidget = new AddAccountMicrosoftWidget(this);

    _menu->setStyleSheet("QTabWidget::pane { border: 0; }");
    _menu->addTab(_microsoftWidget, bc_translate("accounts_microsoft_title"));

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _layout->addWidget(_menu, 0, 0, 1, 11);

    setLayout(_layout);

    setWindowTitle(bc_translate("accounts_creation_title"));
    resize(400, 200);
    setMinimumSize(400, 200);

    connect(_microsoftWidget, SIGNAL(signal_accountAddSuccess()), this,
            SIGNAL(signal_accountAddSuccess()));

    setWindowModality(Qt::ApplicationModal);
}
