#include "MainWindow.h"

#include <QApplication>
#include <iostream>

#include "DataBase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DB.init("/home/cergol/work/homeAssistent/my_db.db");

    MainWindow w;
    w.show();
    return a.exec();
}
