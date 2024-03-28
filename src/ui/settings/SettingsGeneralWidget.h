#ifndef SETTINGSGENERALWIDGET_H
#define SETTINGSGENERALWIDGET_H

#include <QWidget>

#include "../../core/Settings.h"

class QGridLayout;
class QGroupBox;
class QComboBox;
class QVBoxLayout;
class QCheckBox;

class SettingsGeneralWidget : public QWidget {
    Q_OBJECT
  public:
    explicit SettingsGeneralWidget(QWidget *parent = nullptr);

  private slots:
    void onLanguageChange(const QString &lang);
    void onDiscordCheckboxClicked(bool checked);

  signals:
    void signal_toggleDiscordRPC();

  private:
    QGridLayout *_layout;
    QVBoxLayout *_languageLayout;
    QGroupBox *_languageGroup;
    QVBoxLayout *_discordLayout;
    QGroupBox *_discordGroup;
    QComboBox *_languageCombo;
    QCheckBox *_discordCheckbox;
};

#endif
