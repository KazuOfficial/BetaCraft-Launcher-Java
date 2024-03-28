#ifndef ACCOUNTLISTITEMWIDGET_H
#define ACCOUNTLISTITEMWIDGET_H

#include <QWidget>

#include "../../core/Account.h"

class QGridLayout;
class QLabel;
class QPushButton;

class AccountListItemWidget : public QWidget {
    Q_OBJECT
  public:
    explicit AccountListItemWidget(bc_account a, QWidget *parent = nullptr);
    QLabel *_name;
    QLabel *_uuid;

  private:
    QGridLayout *_layout;
    QLabel *_image;
};

#endif
