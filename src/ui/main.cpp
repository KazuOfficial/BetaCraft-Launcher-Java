#include "MainWindow.h"

#include "Betacraft.h"

#include <QApplication>
#include <QIcon>
#include <QTranslator>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <iostream>
#include <pwd.h>
#include <unistd.h>
#endif

#include "../core/Betacraft.h"
#include "../core/FileSystem.h"
#include "../core/JavaInstallations.h"

void copyLanguageFiles() {
    char *workDir = bc_file_directory_get_working();

    QString langRelative("lang/");

    QDir dir = QDir(QString(workDir));
    dir.mkdir(langRelative);

    QString langPath = dir.absoluteFilePath(langRelative);
    QString langRes(":/lang/");

    QDir langResSourceDir(langRes);

    foreach (QString f, langResSourceDir.entryList(QDir::Files)) {
        QString source = QString(langRes + QDir::separator() + f);
        QString dest = QString(langPath + f + QString(".json"));

        if (QFile::exists(dest)) {
            QFile::remove(dest);
        }

        QFile::copy(source, dest);
    }

    free(workDir);
}

void copyJavaRepo() {
    char *workDir = bc_file_directory_get_working();

    QDir dir = QDir(QString(workDir));
    QString source(":/java_repo.json");
    QString dest = dir.absoluteFilePath(QString("java_repo.json"));

    QFile::copy(source, dest);
    free(workDir);
}

void setWorkDir() {
#ifdef __APPLE__
    QString path =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    strcpy(application_support_path, path.toStdString().c_str());
    path += "/betacraft/";

    make_path(path.toStdString().c_str(), 0);
    chdir(path.toStdString().c_str());
#elif __linux__
    char workDir[PATH_MAX];
    struct passwd *pw = getpwuid(getuid());
    strcpy(workDir, pw->pw_dir);
    strcat(workDir, "/.local/share/betacraft/");

    make_path(workDir, 0);
    chdir(workDir);
#elif _WIN32
    make_path("betacraft/", 0);
    chdir("betacraft/");
#endif
}

int main(int argc, char *argv[]) {
    setWorkDir();

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/assets/favicon.ico"));
    app.setStyle("fusion");

    copyLanguageFiles();
    copyJavaRepo();
    bc_file_init();
    betacraft_online = 1;
    bc_jinst_system_check();
    bc_translate_init();

    MainWindow win;
    win.show();

    return app.exec();
}
