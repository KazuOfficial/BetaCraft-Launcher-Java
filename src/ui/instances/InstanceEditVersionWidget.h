#ifndef INSTANCEEDITVERSIONWIDGET_H
#define INSTANCEEDITVERSIONWIDGET_H

#include <QWidget>
#include <QtConcurrent>

#include "../../core/Instance.h"
#include "../../core/VersionList.h"

class QGridLayout;
class QPushButton;
class QLineEdit;
class QTabBar;
class QTreeView;
class QStandardItemModel;
class QModelIndex;

class InstanceEditVersionWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditVersionWidget(QWidget *parent = nullptr);
    QString getSettings();
    void setInstance(bc_instance instance);
    void resetVersionBackground(QString version);
    void clean();
    void versionListInit();

  private slots:
    void onSearchButtonClicked();
    void onMenuTabChanged(int index);

  protected:
    void keyPressEvent(QKeyEvent *e);

  private:
    QGridLayout *_layout;
    QPushButton *_searchButton;
    QLineEdit *_searchTextbox;
    QTabBar *_menu;
    QTreeView *_versionsTreeView;
    QStandardItemModel *_versionListAll;
    QStandardItemModel *_versionListRelease;
    QStandardItemModel *_versionListOldBeta;
    QStandardItemModel *_versionListOldAlpha;
    QStandardItemModel *_versionListSnapshot;
    QStandardItemModel *_versionListSearch;
    void populateVersionList();
    void setSelectedInstance();
    QString _version;
    QFutureWatcher<bc_versionlist *> _versionListWatcher;
};

#endif
