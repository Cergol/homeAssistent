#include "Notes.h"
#include "ui_Notes.h"
#include "DataBase.h"

#include <QTableWidget>
#include <iostream>

#include <QDebug>

Notes::Notes(QWidget *parent)
    : BaseWgt("Note", parent)
    , ui(new Ui::Notes) {

    ui->setupUi(this);
    ui->table->setHorizontalHeaderLabels({"id", "date create", "date until", "note header", "note"});
    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::ID_COLUMN));
    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::DATE_CREATE_COLUMN));
    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::DATE_UNTIL_COLUMN));

    connect (ui->add_row, &QPushButton::pressed,
            this, [=](){

        AddNoteDialog dlg;
        if (dlg.exec()) {

            initTable();
        }
    });
}

Notes::~Notes() {

    delete ui;
}

void Notes::initTable() {

    ui->table->clearContents();
    ui->table->setRowCount(0);
    auto rows = DB.select("notes", {"id", "date_create", "date_until", "note_header", "note"});
    qDebug() << rows.size() << ui->table->rowCount();
    foreach (DataBase::SQLiteRow db_row, rows) {

        int id;
        std::string date_create, date_until, note, note_header;

        id = std::get<int>(db_row.at(0));
        date_create = std::get<std::string>(db_row.at(1));
        try {
            date_until = std::get<std::string>(db_row.at(2));
        }
        catch (std::bad_variant_access &err) {
            date_until = "";
        }
        note_header = std::get<std::string>(db_row.at(3));
        note = std::get<std::string>(db_row.at(4));

        int last_row = ui->table->rowCount();
        ui->table->insertRow(last_row);

        ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::ID_COLUMN),
                           new QTableWidgetItem(QString::number(id)));
        ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::DATE_CREATE_COLUMN),
                           new QTableWidgetItem(QString::fromStdString(date_create)));
        ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::DATE_UNTIL_COLUMN),
                           new QTableWidgetItem(QString::fromStdString(date_until)));
        ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::NOTE_HEADER_COLUMN),
                           new QTableWidgetItem(QString::fromStdString(note_header)));
        ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::NOTE_COLUMN),
                           new QTableWidgetItem(QString::fromStdString(note)));
      //  ui->table->
    }
}

AddNoteDialog::AddNoteDialog(QWidget *parent) :
    QDialog(parent) {

    QVBoxLayout *lay = new QVBoxLayout(this);
    _line_edit = new QLineEdit();
    _plain_text = new QPlainTextEdit();
    lay->addWidget(_line_edit);
    lay->addWidget(_plain_text);

    QPushButton *button = new QPushButton("Создать", this);
    lay->addWidget(button);

    connect (button, &QPushButton::pressed, this,
            [=](){

        qDebug() << _line_edit->text() << "  " << _plain_text->toPlainText();
        DB.insert("notes",{{"note_header", _line_edit->text().toStdString()}, {"note", _plain_text->toPlainText().toStdString()}});
        accept();
    });//            &AddNoteDialog::accept);

    setLayout(lay);
}

