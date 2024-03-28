#include "SettingsWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

#include "../../core/Betacraft.h"

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget{parent} {
    _layout = new QGridLayout(this);
    _sidebar = new QListWidget(this);
    _menu = new QStackedWidget(this);
    _settingsGeneralWidget = new SettingsGeneralWidget(this);
    _settingsJavaWidget = new SettingsJavaWidget(this);
    _settingsGeneralItem = new QListWidgetItem();
    _settingsJavaItem = new QListWidgetItem();

    _sidebar->setStyleSheet("QListWidget { font-size: 12px; }");

    _settingsGeneralItem->setSizeHint(QSize(30, 30));
    _settingsGeneralItem->setText(bc_translate("tab_settings_general"));

    _settingsJavaItem->setSizeHint(QSize(30, 30));
    _settingsJavaItem->setText(bc_translate("tab_settings_java_installations"));

    _sidebar->addItem(_settingsGeneralItem);
    _sidebar->addItem(_settingsJavaItem);

    _menu->addWidget(_settingsGeneralWidget);
    _menu->addWidget(_settingsJavaWidget);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 5);

    _layout->addWidget(_sidebar, 0, 0, 1, 2);
    _layout->addWidget(_menu, 0, 2, 1, 20);

    setLayout(_layout);

    connect(_sidebar, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(onSidebarItemClicked(QListWidgetItem *)));
    connect(_settingsJavaWidget, SIGNAL(signal_toggleTabs()), this,
            SIGNAL(signal_toggleTabs()));
    connect(_settingsGeneralWidget, SIGNAL(signal_toggleDiscordRPC()), this,
            SIGNAL(signal_toggleDiscordRPC()));
}

void SettingsWidget::onSidebarItemClicked(QListWidgetItem *item) {
    _menu->setCurrentIndex(_sidebar->currentRow());
}

void SettingsWidget::setRecommendedJava(QString javaPath) {
    int rows = _settingsJavaWidget->_javaTreeItemModel->rowCount();

    for (int i = 0; i < rows; i++) {
        QStandardItem *item =
            _settingsJavaWidget->_javaTreeItemModel->item(i, 1);

        if (item->text().compare(javaPath) == 0) {
            QStandardItem *versionTextItem =
                _settingsJavaWidget->_javaTreeItemModel->item(i, 0);
            versionTextItem->setBackground(QBrush(QColor(86, 184, 91, 255)));
            item->setBackground(QBrush(QColor(86, 184, 91, 255)));
        }
    }
}

void SettingsWidget::downloadRecommendedJava(QString javaVersion) {
    _settingsJavaWidget->downloadRecommendedJava(javaVersion);
    _menu->setCurrentIndex(1);
}
