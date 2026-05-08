#include "PcStats.h"
#include "ui_PcStats.h"

#include <QFile>

PcStats::PcStats(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PcStats)
{
    ui->setupUi(this);
}

PcStats::~PcStats()
{
    delete ui;
}

