#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include "ui/main/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
//    a.setStyle(QStyleFactory::create("Fusion"));

    QDir::setCurrent(qApp->applicationDirPath());
    MainWindow w;
    w.show();
    return a.exec();
}