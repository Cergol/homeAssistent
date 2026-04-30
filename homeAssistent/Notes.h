#ifndef NOTES_H
#define NOTES_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>

#include <QDialog>

#include <QLineEdit>
#include <QPlainTextEdit>

#include "BaseWgt.h"

namespace Ui {
class Notes;
}

class Notes : public BaseWgt
{
    Q_OBJECT

public:

    enum class NOTES_TABLE : int {
        ID_COLUMN = 0,
        DATE_CREATE_COLUMN = 1,
        DATE_UNTIL_COLUMN = 2,
        NOTE_HEADER_COLUMN = 3,
        NOTE_COLUMN = 4,
    };

    explicit Notes(QWidget *parent = nullptr);
    ~Notes();

    void initTable();

private:
    Ui::Notes *ui;

    //QTableView *_table;
    //QTableWidget *_table;
    //QTab
};

class AddNoteDialog : public QDialog {
    Q_OBJECT

public:
    AddNoteDialog(QWidget* parent = nullptr);//:QDialog(parent) {;}
    ~AddNoteDialog(){;}

private:
    QLineEdit *_line_edit;
    QPlainTextEdit *_plain_text;
};

#endif // NOTES_H
