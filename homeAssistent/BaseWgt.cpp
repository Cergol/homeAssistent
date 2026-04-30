#include "BaseWgt.h"
#include "ui_BaseWgt.h"

BaseWgt::BaseWgt(const QString& name, QWidget *parent)
    : _wgt_name(name),
    QGroupBox(parent),
    ui(new Ui::BaseWgt) {

    ui->setupUi(this);
    this->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid gray;
            border-radius: 5px;
            margin-top: 10px;
            padding: 5px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 5px;
        }
    )");
    setTitle(_wgt_name);
}

BaseWgt::~BaseWgt() {

    delete ui;
}
