#ifndef INSTANCEEDITARGUMENTSWIDGET_H
#define INSTANCEEDITARGUMENTSWIDGET_H

#include <QWidget>

#include "../../core/Instance.h"

class QGridLayout;
class QTextEdit;
class QPushButton;
class QGroupBox;
class QLabel;

class InstanceEditArgumentsWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditArgumentsWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);
    bc_instance *getSettings();

  private slots:

  private:
    QGridLayout *_layout;
    QGroupBox *_javaArgumentsGroup;
    QGroupBox *_programArgumentsGroup;
    QGroupBox *_appletArgumentsGroup;
    QGridLayout *_javaArgumentsLayout;
    QGridLayout *_programArgumentsLayout;
    QGridLayout *_appletArgumentsLayout;
    QTextEdit *_javaArgumentsTextEdit;
    QTextEdit *_programArgumentsTextEdit;
    QLabel *_separateArgumentsLabel;
};

#endif
