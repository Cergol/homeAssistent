#ifndef WHETHERWGT_H
#define WHETHERWGT_H

#include <QWidget>
#include <QTimer>
#include <QMutex>

#include "BaseWgt.h"

namespace Ui {
class WhetherWgt;
}

class WhetherWgt : public BaseWgt
{
    Q_OBJECT

public:
    explicit WhetherWgt(QWidget *parent = nullptr);
    ~WhetherWgt();

    void setTimer(int time_ms);
    void updateWhether();
private:



    void parsePage(const QString& html_page);

    QString _cur_whether;
    QString _whether_like;
    QString _whether_rain;
    QMutex  _mutex;

    Ui::WhetherWgt *ui;
    QTimer* _update_timer;
};

#endif // WHETHERWGT_H
