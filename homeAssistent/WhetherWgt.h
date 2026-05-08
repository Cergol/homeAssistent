#ifndef WHETHERWGT_H
#define WHETHERWGT_H

#include <QWidget>
#include <QTimer>
#include <QMutex>
#include <QDial>
#include <QDialog>
#include <QLabel>

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

signals:

    void parseEnd();
    void changeTimer(int);
private slots:

    void showCustomMenu(const QPoint &pos);
private:

    void parsePage(const QString& html_page);

    QString _cur_whether;
    QString _whether_condition;
    QString _whether_precipitation;
    QMutex  _mutex;

    Ui::WhetherWgt *ui;
    QTimer* _update_timer;
};

#endif // WHETHERWGT_H
