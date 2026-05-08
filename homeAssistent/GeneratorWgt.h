#ifndef GENERATORWGT_H
#define GENERATORWGT_H

#include <QWidget>
#include <QTimer>
#include <QMutex>
#include <QString>

#include "BaseWgt.h"

#include <random>

namespace Ui {
class GeneratorWgt;
}


// Генератор случайных значений
class GeneratorWgt : public BaseWgt
{
    Q_OBJECT

public:
    explicit GeneratorWgt(QWidget *parent = nullptr);
    ~GeneratorWgt();

    void init (int min = 0, int max = 10, int time_s = 10);

    void setMinMax(int min, int max);
    void setTimer(int time_ms);

signals:

    void intReady(int);
    void doubleReady(double);
    void strReady(QString);

    void changeTimer(int);

private slots:

    void showCustomMenu(const QPoint &pos);

    void generate();

private:

    double getDouble();
    int genInt();
    QString genQStr();

    Ui::GeneratorWgt *ui;
    QTimer* _generate_timer;
    QMutex  _mutex;

    std::mt19937 _generator;

    int _min, _max;

    const int ALP_SIZE = 32;
    const int FIRST_RU = 0x0430;
};

#endif // GENERATORWGT_H
