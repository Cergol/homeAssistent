#include "BarWgt.h"
#include "ui_BarWgt.h"

BarWgt::BarWgt(QWidget *parent)
    : BaseWgt(tr("Bar"), parent)
    , ui(new Ui::BarWgt)
{
    ui->setupUi(this);

    _bar = new QtCharts::QChart();
}

BarWgt::~BarWgt()
{
    delete _bar;
    delete ui;
}
