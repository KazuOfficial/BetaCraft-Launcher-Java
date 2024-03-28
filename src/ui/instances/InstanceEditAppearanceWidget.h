#ifndef INSTANCEEDITAPPEARANCEWIDGET_H
#define INSTANCEEDITAPPEARANCEWIDGET_H

#include <QWidget>

#include "../../core/Instance.h"

class QGridLayout;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QCheckBox;
class QLabel;

class InstanceEditAppearanceWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditAppearanceWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);
    bc_instance *getSettings();

  private slots:
    void onBrowseButtonClicked();
    void onRevertDefaultButtonClicked();

  private:
    QGridLayout *_layout;
    QGroupBox *_instanceNameGroup;
    QLineEdit *_instanceNameTextbox;
    QCheckBox *_showGameLogCheckbox;
    QCheckBox *_keepOpenCheckbox;
    QGroupBox *_instanceIconGroup;
    QLabel *_instanceIcon;
    QPushButton *_instanceIconBrowseButton;
    QPushButton *_instanceIconDefaultButton;
    QGroupBox *_instanceGameGroup;
    QLabel *_instanceGameWidthLabel;
    QLabel *_instanceGameHeightLabel;
    QLineEdit *_instanceGameWidthTextbox;
    QLineEdit *_instanceGameHeightTextbox;
    QCheckBox *_instanceGameMaximizedCheckbox;
    QCheckBox *_instanceGameFullscreenCheckbox;
    QGridLayout *_instanceNameLayout;
    QGridLayout *_instanceIconLayout;
    QGridLayout *_instanceGameLayout;
    QGridLayout *_instanceCheckboxLayout;
    QGroupBox *_instanceCheckboxGroup;
};

#endif
