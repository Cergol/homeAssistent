#ifndef MONITORWGT_H
#define MONITORWGT_H

#include <QWidget>


#include "BaseWgt.h"

namespace Ui {
class MonitorWgt;
}

class MonitorWgt : public BaseWgt
{
    Q_OBJECT

public:
    explicit MonitorWgt(QWidget *parent = nullptr);
    ~MonitorWgt();

private:

    int getCurentBrightPers();
    void setMonitorBrightness(uint8_t bright_persent);

    Ui::MonitorWgt *ui;
};

#endif // MONITORWGT_H
