#include "MonitorWgt.h"
#include "ui_MonitorWgt.h"

#if defined(__linux__)
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <physicalmonitorenumerationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <windows.h>
#pragma comment(lib, "dxva2.lib")
#endif

MonitorWgt::MonitorWgt(QWidget *parent)
    : BaseWgt("Monitor", parent)
    , ui(new Ui::MonitorWgt) {

    ui->setupUi(this);
    int cur_bright = getCurentBrightPers();
    ui->dial->setValue(cur_bright);
    connect(ui->dial, &QDial::valueChanged, this, [=](int value){setMonitorBrightness(uint8_t(value));});

}


MonitorWgt::~MonitorWgt() {

    delete ui;
}


int MonitorWgt::getCurentBrightPers() {

#if defined(__linux__)
    QProcess process;
    process.start("ddcutil", {"getvcp", "10"});
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    // Пример вывода: "VCP code 0x10 (Brightness): current value = 50, max value = 100"
    QRegularExpression re(R"(current value = (\d+), max value = (\d+))");
    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        float current = match.captured(1).toFloat();
        float max_val = match.captured(2).toFloat();
        return int((current / max_val) * 100.0f);
    }
#else
    HMONITOR hMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    if (!hMon) return -1;

    PHYSICAL_MONITOR physicalMonitors[3] = {};
    DWORD count = 0;

    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &count)) {
        std::cerr << "Ошибка: GetNumberOfPhysicalMonitorsFromHMONITOR\n";
        return -1;
    }

    if (!GetPhysicalMonitorsFromHMONITOR(hMon, count, physicalMonitors)) {
        std::cerr << "Ошибка: GetPhysicalMonitorsFromHMONITOR\n";
        return -1;
    }

    int brightness = -1;

    for (DWORD i = 0; i < count; ++i) {
        DWORD min, current, max;
        if (GetMonitorBrightness(physicalMonitors[i].hPhysicalMonitor, &min, &current, &max)) {
            // Нормализуем в проценты: от 0.0 до 100.0
            brightness = int(100.0f * (current - min) / (max - min));
            break; // Берём первый подходящий монитор
        }
    }

    // Освобождаем дескрипторы
    for (DWORD i = 0; i < count; ++i) {
        DestroyPhysicalMonitor(physicalMonitors[i].hPhysicalMonitor);
    }

    return brightness;
#endif

    return -1; // ошибка
}


void MonitorWgt::setMonitorBrightness(uint8_t bright_persent) {

#if defined(__linux__)
    QProcess::execute("ddcutil setvcp 10 " + QString::number(int(bright_persent)));
#else
    HMONITOR hMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    PHYSICAL_MONITOR monitors[3] = {};
    DWORD count = 0;

    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &count)) return;

    if (!GetPhysicalMonitorsFromHMONITOR(hMon, count, monitors)) return;

    for (DWORD i = 0; i < count; ++i) {
        DWORD min, current, max, brightness;
        GetMonitorBrightness(hMonitor, &min, &current, &max);
        brightness = min + ((max-min)*bright_persent);

        SetMonitorBrightness(monitors[i].hPhysicalMonitor, brightness);
        DestroyPhysicalMonitor(monitors[i].hPhysicalMonitor);
    }
#endif
}
