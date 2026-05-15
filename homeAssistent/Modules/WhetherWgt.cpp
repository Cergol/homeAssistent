#include "WhetherWgt.h"
#include "ui_WhetherWgt.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QRegularExpression>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <QPushButton>

WhetherWgt::WhetherWgt(QWidget *parent)
    : BaseWgt(tr("Whether"), parent)
    , ui(new Ui::WhetherWgt) {

    ui->setupUi(this);
    updateWhether();
    _update_timer = new QTimer();
    connect(_update_timer, &QTimer::timeout, this, &WhetherWgt::updateWhether);

    connect(this, &WhetherWgt::customContextMenuRequested,
            this, &WhetherWgt::showCustomMenu);

    connect(ui->actionUpdate_whether, &QAction::triggered,
            this, &WhetherWgt::updateWhether);

    connect (ui->actionUpdate_timer, &QAction::triggered,
            this, [=](){

                ChangeTimer dlg(5, 60);
                dlg.setTimer(_update_timer->interval());
                if (dlg.exec()) {

                    int mult, time;
                    time = dlg.getTime(mult);
                    setTimer(time * mult);
                    emit changeTimer(time);
                }
            });

    connect (this, &WhetherWgt::parseEnd, [=](){

        QDateTime current = QDateTime::currentDateTime();
        ui->lbl_last_update->setText(QChar(0x21BA) + current.toString(" HH:mm"));
        ui->lbl_tempreture->setText(_cur_whether);
        ui->lbl_condition->setText(_whether_condition);
        ui->lbl_precipitation->setText(_whether_precipitation);
    });
}

WhetherWgt::~WhetherWgt() {
    delete _update_timer;
    delete ui;
}

void WhetherWgt::setTimer(int time_ms) {

    _update_timer->setInterval(time_ms);
    _update_timer->start();
}

void WhetherWgt::updateWhether() {

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("https://yandex.ru/pogoda/ru?lon=30.3799&lat=60.0161"));

    connect(manager, &QNetworkAccessManager::finished,
            [=](QNetworkReply *reply){

                if (reply->error() == QNetworkReply::NoError) {
                    QString html = QString::fromUtf8(reply->readAll());
                    // ЗДЕСЬ ПАРСИНГ
                    parsePage(html);

                } else {
                    qWarning() << "Ошибка:" << reply->errorString();
                }
                reply->deleteLater();
                manager->deleteLater();

            });
    // Подключаем сигнал завершения запроса к слоту

    manager->get(req);
}


void WhetherWgt::showCustomMenu(const QPoint &pos) {

    QMenu contextMenu(tr("Context Menu"), this);

    contextMenu.addAction(ui->actionUpdate_timer);
    contextMenu.addAction(ui->actionUpdate_whether);

    // Display the menu at the cursor's global position
    contextMenu.exec(this->mapToGlobal(pos));
}


void WhetherWgt::parsePage(const QString& html_page) {

    QtConcurrent::run([=](){

        QMutexLocker locker(&_mutex);

        QRegularExpression degree_sign(R"(<span class=\\?"AppFactTemperature_sign_.*">([+|-])</span>)");
        QRegularExpressionMatch match = degree_sign.match(html_page);
        if (match.hasMatch()) {
            QString matched = match.captured(1);
            _cur_whether = matched;
            //qDebug() << "\nСодержимое:" << match.captured(0);
        }

        QRegularExpression degree(R"(<span class=\\?"AppFactTemperature_value_.+?">(\d+)<\/span>)");
        match = degree.match(html_page);
        if (match.hasMatch()) {
            QString matched = match.captured(1);
            _cur_whether += matched;
            //qDebug() << "\nСодержимое:" << match.captured(0);
        }

        QRegularExpression warning(R"(<div class=\\?"AppFact_warning__.+?">(.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>))");
        match = warning.match(html_page);
        if (match.hasMatch()) {
            QString matched = match.captured(1);

            QRegularExpression first_warn(R"(<span class=\\?"AppFact_warning__first_text_.*?">(.+?)<\/span>)");
            QRegularExpressionMatch match_warn = first_warn.match(matched);
            if (match_warn.hasMatch()) {
                QString matched_first = match_warn.captured(1);
                _whether_condition = matched_first.trimmed();
                //qDebug() << "\nСодержимое _whether_like:" << match_warn.captured(0);
            }
            QRegularExpression sec_warn(R"(<div class=\\?"AppFact_warning__second_.*?">(.+?)<\/div>)");
            match_warn = sec_warn.match(matched);
            if (match_warn.hasMatch()) {
                QString matched_sec = match_warn.captured(1);
                _whether_precipitation = matched_sec.trimmed();
                //qDebug() << "\nСодержимое _whether_rain:" << match_warn.captured(0);
            }
        }
        emit parseEnd();
    });
}
