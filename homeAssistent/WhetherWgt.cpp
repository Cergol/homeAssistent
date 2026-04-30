#include "WhetherWgt.h"
#include "ui_WhetherWgt.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QRegularExpression>

WhetherWgt::WhetherWgt(QWidget *parent)
    : BaseWgt("Whether", parent)
    , ui(new Ui::WhetherWgt) {

    ui->setupUi(this);
    updateWhether();
    _update_timer = new QTimer();
    connect(_update_timer, &QTimer::timeout, this, &WhetherWgt::updateWhether);
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

    // Подключаем сигнал завершения запроса к слоту
    QObject::connect(manager, &QNetworkAccessManager::finished,
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

                         ui->label->setText(_cur_whether);
                         ui->label_2->setText(_whether_like);
                         ui->label_3->setText(_whether_rain);
                     });

    // Отправляем GET запрос
    manager->get(QNetworkRequest(QUrl("https://yandex.ru/pogoda/ru?lon=30.3799&lat=60.0161")));
}

void WhetherWgt::parsePage(const QString& html_page) {

    QMutexLocker locker(&_mutex);

    QRegularExpression degree_sign(R"(<span class="AppFactTemperature_sign_.*">([+|-])</span>)");
    QRegularExpressionMatch match = degree_sign.match(html_page);
    if (match.hasMatch()) {
        QString matched = match.captured(1);
        _cur_whether = matched;
        qDebug() << "\nСодержимое:" << match.captured(0);
    }

    QRegularExpression degree(R"(<span class="AppFactTemperature_value_.+?">(\d+)<\/span>)");
    match = degree.match(html_page);
    if (match.hasMatch()) {
        QString matched = match.captured(1);
        _cur_whether += matched;
        qDebug() << "\nСодержимое:" << match.captured(0);
    }

    QRegularExpression warning(R"(<div class="AppFact_warning__.+?">(.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>.*?<\/div>))");
    match = warning.match(html_page);
    if (match.hasMatch()) {
        QString matched = match.captured(1);

        QRegularExpression first_warn(R"(<span class="AppFact_warning__first_text_.*">(.+)<\/span>)");
        QRegularExpressionMatch match_warn = first_warn.match(matched);
        if (match_warn.hasMatch()) {
            QString matched_first = match_warn.captured(1);
            _whether_like = matched_first.trimmed();
            qDebug() << "\nСодержимое _whether_like:" << match_warn.captured(0);
        }
        QRegularExpression sec_warn(R"(<div class="AppFact_warning__second_.*">(.+)<\/div>)");
        match_warn = sec_warn.match(matched);
        if (match_warn.hasMatch()) {
            QString matched_sec = match_warn.captured(1);
            _whether_rain = matched_sec.trimmed();
            qDebug() << "\nСодержимое _whether_rain:" << match_warn.captured(0);
        }
    }
}
