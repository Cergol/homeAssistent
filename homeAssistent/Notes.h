#ifndef NOTES_H
#define NOTES_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>

#include <QDialog>

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QMap>
#include <QComboBox>
#include <QStyledItemDelegate>

#include "BaseWgt.h"

namespace Ui {
class Notes;
}

enum class NOTES_TABLE : int {
    ID_COLUMN = 0,
    DATE_CREATE_COLUMN,
    DATE_UNTIL_COLUMN,
    NOTE_HEADER_COLUMN,
    NOTE_GROUP,
    NOTE_COLUMN,
};

class FontDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FontDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};


class AddNoteDialog : public QDialog {
    Q_OBJECT

public:
    AddNoteDialog(QWidget* parent = nullptr);//:QDialog(parent) {;}
    ~AddNoteDialog(){;}

private:
    QLineEdit *_line_edit;
    QPlainTextEdit *_plain_text;
    QComboBox   *_cmb_groups;
};


class Notes : public BaseWgt
{
    Q_OBJECT

public:

    explicit Notes(QWidget *parent = nullptr);
    ~Notes();

    void initTable();
    void exec();
    const QMap <NOTES_TABLE, QString> _headers_name;
private slots:

           void showCustomMenu(const QPoint &pos);
private:

    void deleteNote(const QPoint &pos);

    QStandardItemModel* _model;
    Ui::Notes *ui;

    //QTableView *_table;
    //QTableWidget *_table;
    //QTab
};

#endif // NOTES_H
