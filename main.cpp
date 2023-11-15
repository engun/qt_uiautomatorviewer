#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QTranslator>
#include "ui/main/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
//    a.setStyle(QStyleFactory::create("Fusion"));
    QDir::setCurrent(qApp->applicationDirPath());

    QTranslator translator;
    if (translator.load("./lang/" + QLocale().name() + ".qm")) {
        QCoreApplication::installTranslator(&translator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}