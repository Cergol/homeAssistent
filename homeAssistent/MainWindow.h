#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "./Modules/Notes.h"
#include "./Modules/WhetherWgt.h"
#include "./Modules/MonitorWgt.h"
#include "./Modules/GeneratorWgt.h"
#include "./Modules/ZontWgt.h"
#include "./BaseWgt/ChangeTimer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    static const QString ORG_NAME;
    static const QString APP_NAME;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Notes* _notes_table;
    WhetherWgt* _whether_wgt;
    MonitorWgt* _monitor_wgt;
    GeneratorWgt* _generator_wgt;
    ZontWgt* _zont_wgt;
};
#endif // MAINWINDOW_H
