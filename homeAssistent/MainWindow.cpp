#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QSettings>

const QString MainWindow::ORG_NAME = "Cergol";
const QString MainWindow::APP_NAME = "homeAssistent";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //setAttribute(Qt::WA_TranslucentBackground, true);
    //setWindowOpacity(1);

    _notes_table = new Notes();
    _whether_wgt = new WhetherWgt();
    _monitor_wgt = new MonitorWgt();
    _generator_wgt = new GeneratorWgt();
    _zont_wgt = new ZontWgt();

    int wether_timer, generator_timer;

    QSettings settings(ORG_NAME, APP_NAME);
    settings.beginGroup(_whether_wgt->getModName());
    wether_timer = settings.value("timer", 10).toInt();
    settings.endGroup();

    settings.beginGroup(_whether_wgt->getModName());
    generator_timer = settings.value("timer", 20).toInt();
    settings.endGroup();

    _notes_table->initTable();
    _whether_wgt->setTimer(wether_timer * ChangeTimer::MS2M); //минута
    _generator_wgt->init(5, 20 , generator_timer);

    auto setTimerVal = [=](QString mod_name, int val) {

        QSettings settings(ORG_NAME, APP_NAME);
        settings.beginGroup(mod_name);
        settings.setValue("timer", val);
        settings.endGroup();
    };

    connect (_whether_wgt, &WhetherWgt::changeTimer,
            this, [=](int val) {
                setTimerVal(_whether_wgt->getModName(), val);
            });
    connect (_generator_wgt, &GeneratorWgt::changeTimer,
            this, [=](int val) {
                setTimerVal(_generator_wgt->getModName(), val);
            });

    ui->main_layout->addWidget(_notes_table, 0, 0, 1, 2);
    ui->main_layout->addWidget(_whether_wgt, 1, 0);
    ui->main_layout->addWidget(_monitor_wgt, 1, 1);
    ui->main_layout->addWidget(_generator_wgt, 2, 0, 1, 2);
    ui->main_layout->addWidget(_zont_wgt, 3, 0, 1, 2);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _zont_wgt;
}
