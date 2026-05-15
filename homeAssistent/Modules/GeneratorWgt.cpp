#include "GeneratorWgt.h"
#include "ui_GeneratorWgt.h"

#include <QMenu>

#include "./BaseWgt/ChangeTimer.h"

GeneratorWgt::GeneratorWgt(QWidget *parent)
    : BaseWgt(tr("Generator"), parent)
    , ui(new Ui::GeneratorWgt)
{
    ui->setupUi(this);

    std::random_device rd;
    _generator.seed(rd());

    QString str_random = tr("Random ");
    ui->lbl_rnd_int->setText(str_random + "int");
    ui->lbl_rnd_double->setText(str_random + "double");
    ui->lbl_rnd_qstr->setText(str_random + "qstr");

    connect (this, &GeneratorWgt::intReady,
            this, [=](int val){

                ui->int_lbl->setText(QString::number(val));
            });
    connect (this, &GeneratorWgt::doubleReady,
            this, [=](double val){

                ui->double_lbl->setText(QString::number(val));
            });
    connect (this, &GeneratorWgt::strReady,
            ui->qstr_lbl, &QLabel::setText);

    _generate_timer = new QTimer();
    connect (_generate_timer, &QTimer::timeout,
            this, &GeneratorWgt::generate);

    connect(this, &GeneratorWgt::customContextMenuRequested,
            this, &GeneratorWgt::showCustomMenu);
    connect (ui->actionUpdate_timer, &QAction::triggered,
            this, [=](){

                ChangeTimer dlg(2, 60);
                dlg.setTimer(_generate_timer->interval());
                if (dlg.exec()) {

                    int mult, time;
                    time = dlg.getTime(mult);
                    setTimer(time * mult);
                    emit changeTimer(time);
                }
            });
}

GeneratorWgt::~GeneratorWgt() {

    if (_generate_timer) delete _generate_timer;
    delete ui;
}

void GeneratorWgt::init(int min, int max, int time_s) {

    setMinMax(min, max);
    setTimer(time_s * 1000);

    generate();
}

void GeneratorWgt::setMinMax(int min, int max) {

    QMutexLocker locker(&_mutex);

    _min = min;
    _max = max;
}

void GeneratorWgt::setTimer(int time_ms) {

    if (!_generate_timer) return;

    _generate_timer->setInterval(time_ms);
    _generate_timer->start();
}

void GeneratorWgt::showCustomMenu(const QPoint &pos) {

    QMenu contextMenu(tr("Context Menu"), this);

    contextMenu.addAction(ui->actionUpdate_timer);
    //contextMenu.addAction(ui->actionUpdate_whether);

    // Display the menu at the cursor's global position
    contextMenu.exec(this->mapToGlobal(pos));
}

void GeneratorWgt::generate() {

    QMutexLocker locker(&_mutex);

    //qDebug() << "generate " << _min << " " << _max;
    emit intReady(genInt());
    emit strReady(genQStr());
    emit doubleReady(getDouble());
}

double GeneratorWgt::getDouble() {

    std::uniform_real_distribution<double> range(static_cast<double>(_min), static_cast<double>(_max));
    return static_cast<double>(range(_generator));
}

int GeneratorWgt::genInt() {

    return rand()%(_max -_min) + _min;
}

QString GeneratorWgt::genQStr() {

    QString str_val;

    for (int i = _min; i < _max; i++) {

        QChar let(rand()%ALP_SIZE + FIRST_RU);

        if (rand()%2) let = let.toUpper();
        str_val +=let;
    }
    return str_val;
}
