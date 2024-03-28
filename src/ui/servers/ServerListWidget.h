#ifndef SERVERLISTWIDGET_H
#define SERVERLISTWIDGET_H

#include <QWidget>
#include <QtConcurrent>

#include "../../core/Betacraft.h"

class QGridLayout;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class ServerListWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ServerListWidget(QWidget *parent = nullptr);
    void initServerList();

  private slots:
    void onSearchButton();
    void onServerClicked(QListWidgetItem *item);
    void populateServerList();

  signals:
    void signal_serverGameLaunch(const char *ip, const char *port);

  protected:
    void keyPressEvent(QKeyEvent *e);

  private:
    void addServerItem(bc_server server);
    QGridLayout *_layout;
    QLineEdit *_searchTextBox;
    QListWidget *_serverList;
    QPushButton *_serverListRefreshButton;
    QPushButton *_searchButton;
    QFutureWatcher<int> _serverArrayWatcher;
};

#endif
