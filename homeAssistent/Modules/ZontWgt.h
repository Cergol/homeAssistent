#ifndef ZONTWGT_H
#define ZONTWGT_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QMutex>
#include <QList>
#include <QMap>

#include "./BaseWgt/BaseWgt.h"

namespace Ui {
class ZontWgt;
}

class ZontWgt : public BaseWgt
{
    Q_OBJECT

public:

    explicit ZontWgt(QWidget *parent = nullptr);
    ~ZontWgt();

    struct s_TERMOMETR {

        int slot;
        QString name;
        double last_val;

        bool operator == (const s_TERMOMETR& term) const {return term.slot == slot;}
    };

    struct ZONT_MOD {

        QString name;
        QMap <int, double> target_temperature;
        // int - ключ термометра, double - температура целевая
    };

signals:

    void readyShow();
private slots:

    void showCustomMenu(const QPoint &pos);
private:

    void request();

    void authorization();
    void getDevices();

    Ui::ZontWgt *ui;
    QNetworkAccessManager *_manager;

    int _cur_mod;

    QList <s_TERMOMETR> _termometr_list;
    QMap <QString, ZONT_MOD> _thermostat_modes_map;

    QMap <QString, ZONT_MOD> jsonToQMap(QJsonObject &obj);
    QList <s_TERMOMETR> getTermometrList(QJsonArray &arr);

    const QString _devices_tag = "devices";
    const QString _modes_tag = "thermostat_ext_modes_config";
    const QString _cur_mod_tag = "thermostat_ext_mode";
    const QString _thermometers_tag = "thermometers";

    QMutex _mutex;
    QString _token;
};

#endif // ZONTWGT_H
