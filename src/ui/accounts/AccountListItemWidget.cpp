#include "AccountListItemWidget.h"

#include "../../core/Betacraft.h"

#include <QtWidgets>

AccountListItemWidget::AccountListItemWidget(bc_account a, QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _image = new QLabel(this);
    _name = new QLabel(a.username, this);
    _uuid = new QLabel(a.uuid, this);
    _image->setStyleSheet(".QLabel { margin-right: 4px; }");

    bc_memory avatar = bc_avatar_get(a.uuid);

    if (QString(avatar.response).compare("Invalid UUID") != 0) {
        QPixmap pic;
        pic.loadFromData((const uchar *)avatar.response, avatar.size, "PNG");
        _image->setPixmap(
            pic.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    free(avatar.response);

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
