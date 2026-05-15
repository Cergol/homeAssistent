#include "ZontWgt.h"
#include "ui_ZontWgt.h"


#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


ZontWgt::ZontWgt(QWidget *parent)
    : BaseWgt("Zont", parent)
    , ui(new Ui::ZontWgt)
{
    ui->setupUi(this);

    _cur_mod = -1;
    _manager = new QNetworkAccessManager();
    authorization();

    connect(ui->actionUpdate_monitor, &QAction::triggered,
           this, [=](){getDevices();});

    connect(this, &ZontWgt::readyShow,
            this, [=](){

        ui->lbl_mod_name->setText(_thermostat_modes_map[QString::number(_cur_mod)].name);

        QString tmp_text;
        for (auto it = _thermostat_modes_map[QString::number(_cur_mod)].target_temperature.begin();
             it != _thermostat_modes_map[QString::number(_cur_mod)].target_temperature.end();
             it++) {

            s_TERMOMETR temp_term; temp_term.slot = it.key();
            auto termometr_index = _termometr_list.indexOf(temp_term);
            tmp_text = tmp_text + _termometr_list[termometr_index].name + ": "
                       + QString::number(it.value()) + '\n';
        }
        ui->plain_tempreture->setPlainText(tmp_text);
    });
}

ZontWgt::~ZontWgt()
{
    delete ui;
    delete _manager;
}

void ZontWgt::showCustomMenu(const QPoint &pos) {

    QMenu contextMenu(tr("Context Menu"), this);
    contextMenu.addAction(ui->actionUpdate_monitor);
    contextMenu.exec(this->mapToGlobal(pos));
}

void ZontWgt::request() {

    QString api_name = "devices";
//     QNetworkRequest request(QUrl("https://my.zont.online/api/" + api_name));
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//     request.setAttribute();
//     // Отправляем GET
//     QNetworkReply *reply = _manager->post(request);

//     // Обработка ответа
//     QObject::connect(reply, &QNetworkReply::finished, [=]() {
//         if (reply->error() == QNetworkReply::NoError) {
//             QString content = QString::fromUtf8(reply->readAll());
//             qDebug() << "Response:" << content;
//         } else {
//             qDebug() << "Error:" << reply->errorString();
//         }
//         reply->deleteLater();
//         manager->deleteLater();
//         a.quit(); // Выходим из программы
//     });
 }

void ZontWgt::authorization() {

    QNetworkRequest request(QUrl("https://my.zont.online/api/get_authtoken"));
    request.setRawHeader("X-ZONT-Client", "golwww@yandex.ru");
    request.setRawHeader("Content-Type", "application/json");

    QString log_pwd = "CerGol:nfAxrp!vcbs99dG";

    request.setRawHeader("Authorization", "Basic " + log_pwd.toUtf8().toBase64());

    QJsonObject data;

    data["client_name"] = "homeAssistent";

    QJsonDocument doc(data);
    QNetworkReply *reply = _manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString content = QString::fromUtf8(reply->readAll());

            QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                _token = obj["token"].toString();

                getDevices();
            }
        } else {
            qDebug() << reply->error();
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


void ZontWgt::getDevices() {

    QNetworkRequest request(QUrl("https://my.zont.online/api/devices"));
    request.setRawHeader("X-ZONT-Client", "golwww@yandex.ru");
    request.setRawHeader("X-ZONT-Token", _token.toUtf8());
    request.setRawHeader("Content-Type", "application/json");

    QJsonObject data;
    data["load_io"] = false;

    QJsonDocument doc(data);
    QNetworkReply *reply = _manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString content = QString::fromUtf8(reply->readAll());
            QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

            //qDebug() << doc;
            if (doc.isObject()) {
                QJsonObject obj = doc.object();

                QJsonArray devices = obj[_devices_tag].toArray();

                for (auto device: devices) {

                    QJsonValue cur_mod_obj = device.toObject()[_cur_mod_tag];
                    QJsonValue mods_obj = device.toObject()[_modes_tag];
                    QJsonValue termometers_obj = device.toObject()[_thermometers_tag];
                    auto termometers_arr = termometers_obj.toArray();
                    _termometr_list.append(getTermometrList(termometers_arr));

                    if (!mods_obj.isUndefined()) {

                        auto obj = mods_obj.toObject();
                        _thermostat_modes_map = jsonToQMap(obj);
                        _cur_mod = cur_mod_obj.toInt();
                        emit readyShow();
                    }
                    else
                        qDebug() << "No modes found";
                }
            }
        } else {
            qDebug() << reply->error();
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


QList<ZontWgt::s_TERMOMETR> ZontWgt::getTermometrList(QJsonArray &arr) {

    QList<ZontWgt::s_TERMOMETR> list;
    // не ясно как связан термометр с зоной
    // zone_sensor ключ или значение? == thermometers.slot или functions.zone
    for (auto term: arr) {
        s_TERMOMETR s_term;

        s_term.name = term.toObject()["name"].toString();
        s_term.last_val = term.toObject()["last_value"].toDouble();
        s_term.slot = term.toObject()["slot"].toInt();

        list.append(s_term);
    }

    return list;
}


QMap <QString, ZontWgt::ZONT_MOD> ZontWgt::jsonToQMap(QJsonObject &obj) {

    QMap <QString, ZONT_MOD>  thermostat_mod_map;
    for (QString key: obj.keys()) {

        ZONT_MOD thermostat_mod;
        auto mod_obj = obj[key].toObject();

        thermostat_mod.name = mod_obj["name"].toString();
        auto zone_temp = mod_obj["zone_temp"].toObject();
        auto zone_sensors = mod_obj["zone_sensors"].toObject();
        //ключи должны совпадать

        QMap <int, double> map;

        for (QString tempr_key: zone_temp.keys()) {

            double target_temperature = zone_temp[tempr_key].toDouble();
            int termometr_id = zone_sensors[tempr_key].toInt();

            map[termometr_id] = target_temperature;
        }
        thermostat_mod.target_temperature = map;
        thermostat_mod_map[key] = thermostat_mod;
    }

    return thermostat_mod_map;
}
