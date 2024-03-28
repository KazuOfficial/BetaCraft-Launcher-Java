#ifndef INSTANCEEDITMODSWIDGET_H
#define INSTANCEEDITMODSWIDGET_H

#include "InstanceEditModRepoWidget.h"
#include <QWidget>

#include "../../../core/Instance.h"

class QGridLayout;
class QTreeWidget;
class QPushButton;
class QGroupBox;
class QProgressBar;
class QTimer;

class InstanceEditModsWidget : public QWidget {
    Q_OBJECT
  public:
    explicit InstanceEditModsWidget(QWidget *parent = nullptr);
    void setInstance(bc_instance instance);
    bc_mod_version_array *getSettings();

  private slots:
    void onModRepoButtonClicked();
    void onOpenMinecraftDirectoryClicked();
    void onRemoveButtonClicked();
    void onMoveButtonClicked(int direction);
    void onAddToMinecraftJarButtonClicked();
    void onReplaceMinecraftJarButtonClicked();
    void onModDownloadStarted();
    void ModInstallProgressBarUpdate();

  protected:
  private:
    QGridLayout *_layout;
    InstanceEditModRepoWidget *_instanceEditModRepoWidget;
    QTreeWidget *_modList;
    QPushButton *_moveUpButton;
    QPushButton *_moveDownButton;
    QPushButton *_removeButton;
    QPushButton *_modRepoButton;
    QPushButton *_installFabricButton;
    QPushButton *_addToMinecraftJarButton;
    QPushButton *_openMinecraftDirectoryButton;
    QPushButton *_replaceMinecraftJarButton;
    QGroupBox *_editModGroup;
    QGroupBox *_modLoaderGroup;
    QGroupBox *_gameDirectoryGroup;
    QGroupBox *_modRepoGroup;
    QGridLayout *_editModLayout;
    QGridLayout *_modLoaderLayout;
    QGridLayout *_gameDirectoryLayout;
    QGridLayout *_modRepoLayout;
    QProgressBar *_progressBar;
    QTimer *_progressTimer;
    bc_instance _instance;
    void populateModList();
};

#endif
