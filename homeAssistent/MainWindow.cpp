#include "MainWindow.h"
#include "./ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _notes_table = new Notes();
    _notes_table->initTable();

    _whether_wgt = new WhetherWgt();
    _whether_wgt->setTimer(10*60 * 1000); //минута

    _monitor_wgt = new MonitorWgt();

    ui->main_layout->addWidget(_notes_table, 0, 0, 1, 2);
    ui->main_layout->addWidget(_whether_wgt, 1, 0);
    ui->main_layout->addWidget(_monitor_wgt, 1, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
