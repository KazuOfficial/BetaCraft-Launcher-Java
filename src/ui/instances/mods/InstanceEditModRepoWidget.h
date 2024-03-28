#ifndef INSTANCEEDITMODREPOWIDGET_H
#define INSTANCEEDITMODREPOWIDGET_H

#include "InstanceEditModVersionsWidget.h"
#include <QWidget>

#include "../../../core/Instance.h"

class QGridLayout;
class QPushButton;
class QGroupBox;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;

class InstanceEditModRepoWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditModRepoWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);

  private slots:
    void modListItemAdd(bc_mod mod);
    void onModClicked(QTreeWidgetItem *item, int column);
    void onSearchButtonClicked();

  signals:
    void signal_BackButtonClicked();
    void signal_ModDownloadStarted();
    void signal_ModDownloadFinished();

  protected:
    void keyPressEvent(QKeyEvent *e);

  private:
    QGridLayout *_layout;
    QTreeWidget *_modList;
    QPushButton *_searchButton;
    QPushButton *_backButton;
    QPushButton *_changeRepoButton;
    QLineEdit *_searchTextBox;
    InstanceEditModVersionsWidget *_versionsWidget;
    bc_instance _instance;
};

#endif
