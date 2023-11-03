#include "ServerListItemWidget.h"

#include <QtWidgets>

ServerListItemWidget::ServerListItemWidget(bc_server s, std::unordered_map<QString, QByteArray> serverToIconMap, QWidget *parent)
    : QWidget{parent} {
    QString max = QString::number(s.max_players); 
    QString online = QString::number(s.online_players);
    QString finalc = (online + " / " + max);

    _layout = new QGridLayout(this);
    _image = new QLabel(this);
    _name = new QLabel(s.name, this);
    _version = new QLabel(s.connect_version, this);
    _players = new QLabel(finalc, this);
    _description = new QLabel(s.description, this);

    if (serverToIconMap.find(QString(s.connect_socket)) != serverToIconMap.end()) {
        QByteArray imageEncoded = serverToIconMap[s.connect_socket];
        QByteArray imageBytes = QByteArray::fromBase64(imageEncoded);

        QPixmap pic;
        pic.loadFromData(imageBytes, "PNG");

        _image->setPixmap(pic.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    _image->setStyleSheet(".QLabel { margin-right: 4px; }");
    _name->setStyleSheet(".QLabel { font-size: 15px; font-weight: bold; }");

    _layout->addWidget(_image, 0, 0, 3, 1, Qt::AlignLeft);
    _layout->addWidget(_name, 0, 1, 1, 1, Qt::AlignLeft);
    _layout->addWidget(_version, 0, 3, 1, 1, Qt::AlignRight);
    _layout->addWidget(_players, 1, 3, 1, 1, Qt::AlignRight);
    _layout->addWidget(_description, 1, 1, 1, 1, Qt::AlignLeft);

    _layout->setColumnStretch(2, 1);

    _layout->setAlignment(Qt::AlignTop);

    _layout->setSpacing(0);
    _layout->setContentsMargins(3, 3, 3, 3);

    setObjectName("server-list-item");

    setLayout(_layout);
}
