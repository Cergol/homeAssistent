#ifndef PCSTATS_H
#define PCSTATS_H

#include <QWidget>

#include <QTimer>
namespace Ui {
class PcStats;
}

class PcStats : public QWidget
{
    Q_OBJECT

public:
    explicit PcStats(QWidget *parent = nullptr);
    ~PcStats();

private:
    Ui::PcStats *ui;

    QTimer* _update_timer;

};

#endif // PCSTATS_H
