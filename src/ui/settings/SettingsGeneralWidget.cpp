#include "SettingsGeneralWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>
#include "../../core/Settings.h"

SettingsGeneralWidget::SettingsGeneralWidget(QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _languageLayout = new QVBoxLayout();
    _languageGroup = new QGroupBox(this);
    _discordLayout = new QVBoxLayout();
    _discordGroup = new QGroupBox("Discord", this);
    _languageCombo = new QComboBox(this);
    _discordCheckbox = new QCheckBox("Discord Rich Presence", this);

    _languageGroup->setTitle(bc_translate("settings_general_language_title"));

    QDir langDir(":/lang/");
    foreach (QString lang, langDir.entryList(QDir::Files)) {
        _languageCombo->addItem(lang);
    }

    bc_settings *settings = bc_settings_get();
    _languageCombo->setCurrentText(QString(settings->language));
    _discordCheckbox->setChecked(settings->discord);

    free(settings);

    _languageLayout->addWidget(_languageCombo);
    _languageGroup->setLayout(_languageLayout);

    _discordLayout->addWidget(_discordCheckbox);
    _discordGroup->setLayout(_discordLayout);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 0);
    _layout->setAlignment(Qt::AlignTop);

    _layout->addWidget(_languageGroup, 0, 0, 1, 1);
    _layout->addWidget(_discordGroup, 1, 0, 1, 1);

    setLayout(_layout);

    connect(_languageCombo, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(onLanguageChange(const QString &)));
    connect(_discordCheckbox, SIGNAL(clicked(bool)), this,
            SLOT(onDiscordCheckboxClicked(bool)));
}

void SettingsGeneralWidget::onLanguageChange(const QString &lang) {
    bc_settings *settings = bc_settings_get();

    snprintf(settings->language, sizeof(settings->language), "%s",
             lang.toStdString().c_str());
    bc_settings_update(settings);

    free(settings);

    QApplication::quit();
}

void SettingsGeneralWidget::onDiscordCheckboxClicked(bool checked) {
    bc_settings *settings = bc_settings_get();
    settings->discord = checked;

    bc_settings_update(settings);

    free(settings);

    emit signal_toggleDiscordRPC();
}
