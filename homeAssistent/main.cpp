#include "MainWindow.h"

#include <QApplication>
#include <QTranslator>
#include <iostream>

#include "./BaseWgt/DataBase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    if (translator.load(":/tr/translations/homeAssistent_ru.qm")) {
        a.installTranslator(&translator);
    }
    DB.init("/home/cergol/work/homeAssistent/my_db.db");

    MainWindow w;
    w.show();
    //w.setWindowOpacity(0.8);
    return a.exec();
}
