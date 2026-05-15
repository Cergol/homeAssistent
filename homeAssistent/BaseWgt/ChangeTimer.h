#ifndef CHANGETIMER_H
#define CHANGETIMER_H

#include <QDialog>
#include <QMap>
#include <QString>

namespace Ui {
class ChangeTimer;
}

class ChangeTimer : public QDialog
{
    Q_OBJECT

public:

    static const QString POST_S, POST_M, POST_MS;

    static const QMap <QString, int> MULTY_MAP;
    static const int MS2S;
    static const int MS2M;
    explicit ChangeTimer(int min, int max, QWidget *parent = nullptr);
    ~ChangeTimer();

    void setTimer(const int time);
    int getTime(int &multy);
private:
    Ui::ChangeTimer *ui;
    QString _postfix;
};

#endif // CHANGETIMER_H
