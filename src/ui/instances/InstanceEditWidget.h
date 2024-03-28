#ifndef INSTANCEEDITWIDGET_H
#define INSTANCEEDITWIDGET_H

#include <QWidget>

#include "../../core/Instance.h"

#include "InstanceEditAppearanceWidget.h"
#include "InstanceEditArgumentsWidget.h"
#include "InstanceEditServerWidget.h"
#include "InstanceEditVersionWidget.h"
#include "mods/InstanceEditModsWidget.h"

class QGridLayout;
class QTabWidget;
class QPushButton;

class InstanceEditWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);

  private slots:
    void onInstanceSaveButtonClicked();

  signals:
    void signal_instanceSettingsSaved();

  private:
    QGridLayout *_layout;
    QTabWidget *_menu;
    InstanceEditAppearanceWidget *_instanceEditAppearanceWidget;
    InstanceEditVersionWidget *_instanceEditVersionWidget;
    InstanceEditArgumentsWidget *_instanceEditArgumentsWidget;
    InstanceEditModsWidget *_instanceEditModsWidget;
    InstanceEditServerWidget *_instanceEditServerWidget;
    QPushButton *_instanceSaveButton;
    QGridLayout *_instanceSaveButtonLayout;
    QWidget *_instanceSaveButtonWidget;
};

#endif
