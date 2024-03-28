#include "InstanceEditWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#include "../../core/Network.h"

bc_instance instance;

InstanceEditWidget::InstanceEditWidget(QWidget *parent) : QWidget{parent} {
    char tr[256];

    _layout = new QGridLayout(this);
    _menu = new QTabWidget(this);
    _instanceSaveButtonLayout = new QGridLayout();
    _instanceSaveButton = new QPushButton(this);
    _instanceSaveButtonWidget = new QWidget(this);
    _instanceEditAppearanceWidget = new InstanceEditAppearanceWidget();
    _instanceEditArgumentsWidget = new InstanceEditArgumentsWidget();
    _instanceEditServerWidget = new InstanceEditServerWidget();

    _instanceSaveButton->setText(bc_translate("instance_save_button"));

    _instanceSaveButtonLayout->setSpacing(5);
    _instanceSaveButtonLayout->setContentsMargins(10, 10, 10, 10);
    _instanceSaveButtonLayout->addWidget(_instanceSaveButton);
    _instanceSaveButtonWidget->setLayout(_instanceSaveButtonLayout);

    _menu->setStyleSheet("QTabWidget::pane { border: 0; }");

    _menu->addTab(_instanceEditAppearanceWidget,
                  bc_translate("instance_tab_appearance"));

    if (betacraft_online) {
        _instanceEditVersionWidget = new InstanceEditVersionWidget();
        _instanceEditModsWidget = new InstanceEditModsWidget();

        _menu->addTab(_instanceEditVersionWidget,
                      bc_translate("instance_tab_version"));
        _menu->addTab(_instanceEditModsWidget,
                      bc_translate("instance_tab_mods"));
    }

    _menu->addTab(_instanceEditArgumentsWidget,
                  bc_translate("instance_tab_arguments"));
    _menu->addTab(_instanceEditServerWidget,
                  bc_translate("instance_tab_server"));

    _layout->addWidget(_menu, 0, 0, 1, 11);
    _layout->addWidget(_instanceSaveButtonWidget, 1, 0, 1, 11);

    setWindowTitle(bc_translate("instance_edit_title"));

    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    resize(600, 500);
    setMinimumSize(600, 500);
    setLayout(_layout);

    connect(_instanceSaveButton, SIGNAL(released()), this,
            SLOT(onInstanceSaveButtonClicked()));

    setWindowModality(Qt::ApplicationModal);
}

void InstanceEditWidget::onInstanceSaveButtonClicked() {
    bc_instance *appearanceSettings =
        _instanceEditAppearanceWidget->getSettings();

    if (!bc_instance_validate_name(appearanceSettings->name)) {
        QMessageBox msg;
        msg.setModal(1);
        msg.setText(bc_translate("instance_name_requirements"));
        msg.exec();

        delete appearanceSettings;
        return;
    }

    QString versionSelected = _instanceEditVersionWidget->getSettings();
    bc_mod_version_array *mods = _instanceEditModsWidget->getSettings();
    bc_instance *arguments = _instanceEditArgumentsWidget->getSettings();
    bc_instance *server = _instanceEditServerWidget->getSettings();

    snprintf(instance.name, sizeof(instance.name), "%s",
             appearanceSettings->name);
    instance.width = appearanceSettings->width;
    instance.height = appearanceSettings->height;
    instance.maximized = appearanceSettings->maximized;
    instance.fullscreen = appearanceSettings->fullscreen;
    instance.show_log = appearanceSettings->show_log;
    instance.keep_open = appearanceSettings->keep_open;

    snprintf(instance.program_args, sizeof(instance.program_args), "%s",
             arguments->program_args);
    snprintf(instance.jvm_args, sizeof(instance.jvm_args), "%s",
             arguments->jvm_args);

    instance.join_server = server->join_server;
    snprintf(instance.server_ip, sizeof(instance.server_ip), "%s",
             server->server_ip);
    snprintf(instance.server_port, sizeof(instance.server_port), "%s",
             server->server_port);

    bc_mod_list_installed_move(mods, instance.path);

    if (versionSelected.compare(instance.version) != 0 && mods->len > 0) {
        for (int i = 0; i < mods->len; i++) {
            bc_mod_list_remove(instance.path, mods->arr[i].path);
        }
    }

    snprintf(instance.version, sizeof(instance.version), "%s",
             versionSelected.toStdString().c_str());

    bc_instance_update(&instance);

    delete mods;
    delete appearanceSettings;
    delete arguments;
    delete server;

    emit signal_instanceSettingsSaved();
}

void InstanceEditWidget::setInstance(bc_instance i) {
    _menu->setCurrentIndex(0);
    instance = i;

    _instanceEditAppearanceWidget->setInstance(i);
    _instanceEditArgumentsWidget->setInstance(i);
    _instanceEditServerWidget->setInstance(i);

    if (betacraft_online) {
        _instanceEditVersionWidget->versionListInit();
        _instanceEditVersionWidget->setInstance(i);
        _instanceEditModsWidget->setInstance(i);
    }
}
