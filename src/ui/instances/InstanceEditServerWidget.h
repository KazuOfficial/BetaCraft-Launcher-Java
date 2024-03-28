#ifndef INSTANCEEDITSERVERWIDGET_H
#define INSTANCEEDITSERVERWIDGET_H

#include <QWidget>

#include "../../core/Instance.h"

class QGridLayout;
class QLineEdit;
class QGroupBox;
class QCheckBox;

class InstanceEditServerWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditServerWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);
    bc_instance *getSettings();

  private slots:

  private:
    QGridLayout *_layout;
    QGroupBox *_serverAddressGroup;
    QGridLayout *_serverAddressLayout;
    QCheckBox *_joinServerCheckbox;
    QLineEdit *_serverIpTextEdit;
    QLineEdit *_serverPortTextEdit;
};

#endif
