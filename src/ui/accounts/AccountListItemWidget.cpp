#include "AccountListItemWidget.h"

#include "../../core/Betacraft.h"

#include <QtWidgets>
#include <string>

AccountListItemWidget::AccountListItemWidget(bc_account a, QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _image = new QLabel(this);
    _name = new QLabel(a.username, this);
    _uuid = new QLabel(a.uuid, this);
    _image->setStyleSheet(".QLabel { margin-right: 4px; }");

    std::string avatar = bc_avatar_get(a.uuid);

    if (avatar != "Invalid UUID") {
        QPixmap pic;
        pic.loadFromData((const uchar *)avatar.c_str(), avatar.length(), "PNG");
        _image->setPixmap(
            pic.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    _name->setStyleSheet(".QLabel { font-size: 15px; font-weight: bold; }");

    _layout->addWidget(_image, 0, 0, 3, 1, Qt::AlignLeft);
    _layout->addWidget(_name, 0, 1, 1, 1, Qt::AlignLeft);
    _layout->addWidget(_uuid, 1, 1, 1, 1, Qt::AlignLeft);

    _layout->setColumnStretch(2, 1);

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(0);
    _layout->setContentsMargins(3, 3, 3, 3);

    setStyleSheet("QLabel { font-size: 11px; }");

    setLayout(_layout);
}
