#ifndef BASEWGT_H
#define BASEWGT_H

#include <QGroupBox>
#include <QVBoxLayout>
#include <QFrame>

namespace Ui {
class BaseWgt;
}


class BaseWgt : public QGroupBox
{
    Q_OBJECT
public:
    explicit BaseWgt(const QString& name, QWidget *parent = nullptr);
    ~BaseWgt();

    QString getModName() {return _wgt_name;}
protected:

    QString _wgt_name;
signals:

private:

    Ui::BaseWgt *ui;
};

#endif // BASEWGT_H
