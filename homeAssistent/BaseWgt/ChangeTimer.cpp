#include "ChangeTimer.h"
#include "ui_ChangeTimer.h"

const QString ChangeTimer::POST_S = " s";
const QString ChangeTimer::POST_MS = " ms";
const QString ChangeTimer::POST_M = " m";


const int ChangeTimer::MS2S = 1000;
const int ChangeTimer::MS2M = 60 * 1000;

const QMap <QString, int> ChangeTimer::MULTY_MAP {{ChangeTimer::POST_MS, 1},
                                                {ChangeTimer::POST_S, ChangeTimer::MS2S},
                                                {ChangeTimer::POST_M, ChangeTimer::MS2M}};

ChangeTimer::ChangeTimer(int min, int max, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChangeTimer)
{
    ui->setupUi(this);

    tr(ChangeTimer::POST_MS.toUtf8().constData());
    tr(ChangeTimer::POST_S.toUtf8().constData());
    tr(ChangeTimer::POST_M.toUtf8().constData());

    ui->timer_dial->setRange(min, max);
    _postfix = tr(" ms");

    //setWindowFlag(Qt::WindowMinimizeButtonHint);
    //setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    connect (ui->timer_dial, &QDial::valueChanged,
            this, [=](int value){ ui->label->setText(QString::number(value) + tr(_postfix.toUtf8().constData()));});
}

ChangeTimer::~ChangeTimer() {
    delete ui;
}

void ChangeTimer::setTimer(const int time) {

    int val;

    if (time > MS2M) {

        _postfix = ChangeTimer::POST_M;
        val = time / MS2M;
    }
    else if(time > MS2S){
        _postfix = ChangeTimer::POST_S;
        val = time / MS2S;
    }
    else {
        _postfix = ChangeTimer::POST_MS;
        val = time;
    }
    ui->timer_dial->setValue(val);
}

int ChangeTimer::getTime(int &multy) {

    multy = MULTY_MAP[_postfix];
    return ui->timer_dial->value();
}
