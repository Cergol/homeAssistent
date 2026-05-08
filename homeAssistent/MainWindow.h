#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Notes.h"
#include "WhetherWgt.h"
#include "MonitorWgt.h"
#include "GeneratorWgt.h"

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
};
#endif // MAINWINDOW_H
