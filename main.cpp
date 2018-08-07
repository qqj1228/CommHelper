#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QString path(QDir::currentPath());
    if (!QFileInfo(path).isWritable() || path == QDir::homePath ())
    {
        QDir dir(QDir::home());
        dir.mkdir("." CFG_APP_NAME);
        if (dir.cd("." CFG_APP_NAME)) {
            path = dir.absolutePath();
        }
    }

    QTranslator translator;
    translator.load("zh_CN.qm", path);
    QApplication a(argc, argv);
    a.installTranslator(&translator);
    MainWindow w;
    w.show();

    return a.exec();
}
