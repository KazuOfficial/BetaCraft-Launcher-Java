#include "AddInstanceWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#include "../../core/Instance.h"

AddInstanceWidget::AddInstanceWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _instanceNameTextbox = new QLineEdit(this);
    _groupList = new QListWidget(this);
    _createButton = new QPushButton(this);
    _newGroupButton = new QPushButton(this);
    _newGroupTextbox = new QLineEdit(this);
    _versionWidget = new InstanceEditVersionWidget(this);

    _createButton->setText(bc_translate("instance_create_button"));
    _newGroupButton->setText(bc_translate("instance_group_new"));

    _layout->setAlignment(Qt::AlignTop);

    _layout->addWidget(new QLabel(bc_translate("instance_name"), this, {}), 0,
                       0, 1, 2);
    _layout->addWidget(_instanceNameTextbox, 1, 0, 1, 2);
    _layout->addWidget(new QLabel(bc_translate("instance_group"), this, {}), 2,
                       0, 1, 2);
    _layout->addWidget(_newGroupTextbox, 3, 0, 1, 1);
    _layout->addWidget(_newGroupButton, 3, 1, 1, 1);
    _layout->addWidget(_groupList, 4, 0, 1, 2);
    _layout->addWidget(_createButton, 5, 0, 1, 2);
    _layout->addWidget(_versionWidget, 0, 3, 6, 3);

    _layout->setColumnStretch(3, 3);

    _layout->setRowMinimumHeight(5, 0);

    _layout->setSpacing(5);
    _layout->setContentsMargins(10, 10, 10, 10);

    setLayout(_layout);
    setStyleSheet("QLabel { font-size: 14px; }");

    setWindowTitle(bc_translate("instance_creation_title"));
    resize(650, 500);
    setMinimumSize(650, 500);

    connect(_createButton, SIGNAL(released()), this,
            SLOT(onCreateButtonClicked()));
    connect(_newGroupButton, SIGNAL(released()), this,
            SLOT(onNewGroupButtonClicked()));

    setWindowModality(Qt::ApplicationModal);
}

void AddInstanceWidget::onCreateButtonClicked() {
    std::string instanceName =
        _instanceNameTextbox->text().trimmed().toStdString();

    if (!bc_instance_validate_name(instanceName.c_str())) {
        QMessageBox msg;
        msg.setModal(1);
        msg.setText(bc_translate("instance_name_requirements"));
        msg.exec();

        return;
    }

    QString versionSelected = _versionWidget->getSettings();

    if (!instanceName.empty() && !versionSelected.isNull()) {
        QString name = _instanceNameTextbox->text().trimmed();
        QString group;

        if (_groupList->currentRow() != -1) {
            group = _groupList->currentItem()->text();
        }

        bc_instance_create(name.toStdString().c_str(),
                           versionSelected.toStdString().c_str(),
                           group.isNull() ? NULL : group.toStdString().c_str());

        emit signal_instanceAdded();
    }
}

void AddInstanceWidget::onNewGroupButtonClicked() {
    if (!_newGroupTextbox->text().isEmpty()) {
        bc_instance_group_create(
            _newGroupTextbox->text().trimmed().toStdString().c_str());
        populateGroupList();
    }
}

void AddInstanceWidget::populateGroupList() {
    _groupList->clear();
    bc_instance_group_name_array *groups = bc_instance_group_name_get_all();

    _newGroupTextbox->setText("");

    for (int i = 0; i < groups->len; i++) {
        _groupList->addItem(groups->arr[i]);
    }

    free(groups);
}

void AddInstanceWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return) {
        onCreateButtonClicked();
    }
}
