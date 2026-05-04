#include "MainWindow.h"
#include "./ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // setStyleSheet(R"(background-color: rgba(255, 255, 255, 128)
    //               QMenu {
    //                   background-color: rgba(255, 255, 255, 128);
    //               border: 1px solid #C0C0C0;
    //               }
    //               QMenu::item {
    //                   padding: 2px 20px 2px 20px;
    //                       background-color: rgba(255, 255, 255, 188);
    //               }
    //               QMenu::item:selected {
    //                   background-color: rgba(34, 160, 191, 188);
    //                 color: white;
    //               })");
    setAttribute(Qt::WA_TranslucentBackground, true);

    _notes_table = new Notes();
    _notes_table->initTable();

    _whether_wgt = new WhetherWgt();
    _whether_wgt->setTimer(10* WhetherWgt::MS2S); //минута

    _monitor_wgt = new MonitorWgt();

    ui->main_layout->addWidget(_notes_table, 0, 0, 1, 2);
    ui->main_layout->addWidget(_whether_wgt, 1, 0);
    ui->main_layout->addWidget(_monitor_wgt, 1, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
