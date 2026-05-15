#ifndef BARWGT_H
#define BARWGT_H

#include <QWidget>
#include <QtCharts/QChart>

#include "./BaseWgt/BaseWgt.h"

namespace Ui {
class BarWgt;
}

class BarWgt : public BaseWgt
{
    Q_OBJECT

public:
    explicit BarWgt(QWidget *parent = nullptr);
    ~BarWgt();

private:
    Ui::BarWgt *ui;
    //QtCharts::QChart *_bar;
};

#endif // BARWGT_H
