#include "Notes.h"
#include "ui_Notes.h"
#include "DataBase.h"

#include <QTableWidget>
#include <QFormLayout>
#include <QMenu>
#include <iostream>

#include <QDebug>

Notes::Notes(QWidget *parent)
    : BaseWgt(tr("Notes"), parent)
    , ui(new Ui::Notes),
    _headers_name{{NOTES_TABLE::ID_COLUMN, "id"},
                    {NOTES_TABLE::DATE_CREATE_COLUMN, tr("Date create")},
                    {NOTES_TABLE::DATE_UNTIL_COLUMN, tr("Date until")},
                    {NOTES_TABLE::NOTE_HEADER_COLUMN, tr("Note header")},
                    {NOTES_TABLE::NOTE_GROUP, tr("Note group")},
                    {NOTES_TABLE::NOTE_COLUMN, tr("Note")}} {

    ui->setupUi(this);

    connect(ui->table, &QTableWidget::customContextMenuRequested,
            this, &Notes::showCustomMenu);

    ui->table->setItemDelegate(new FontDelegate(ui->table));
    ui->table->setColumnCount(_headers_name.size());
    ui->table->setHorizontalHeaderLabels(_headers_name.values());
    ui->table->horizontalHeader()->setStyleSheet("background-color: rgb(255,255,255)");

    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::ID_COLUMN));
    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::DATE_CREATE_COLUMN));
    ui->table->hideColumn(static_cast<int>(NOTES_TABLE::DATE_UNTIL_COLUMN));
    //ui->table->hideColumn(static_cast<int>(NOTES_TABLE::NOTE_GROUP));

    ui->table->setStyleSheet(
        R"(QScrollBar:vertical {
        background: rgba(255, 255, 255, 0.6);
        width: 12px;
        margin: 0px;
        border-radius: 6px;
    }

    QScrollBar::handle:vertical {
        background: rgba(33, 150, 243, 0.8);
        min-height: 20px;
        border-radius: 6px;
        margin: 2px;
    }

    QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
    height: 0px;
    }

    QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
    background: none;
    }
)");

    // Create and add actions

    connect (ui->actionAdd_note, &QAction::triggered,
            this, [=](){

        AddNoteDialog dlg(this);
        if (dlg.exec()) {

            initTable();
        }
    });

    connect (ui->add_row_btn, &QPushButton::clicked,
            ui->actionAdd_note, &QAction::trigger);
}

Notes::~Notes() {

    delete ui;
}

void Notes::initTable() {

    ui->table->clearContents();
    ui->table->setRowCount(0);

    try {
        auto rows = DB.select("notes AS n INNER JOIN note_groups AS ng ON n.note_group = ng.id",
                              {"n.id", "n.date_create", "n.date_until", "n.note_header", "n.note", "ng.'group'"});

        foreach (DataBase::SQLiteRow db_row, rows) {

            int id;
            std::string date_create, date_until, note, note_header, group;

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
            group = std::get<std::string>(db_row.at(5));

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
            ui->table->setItem(last_row, static_cast<int>(NOTES_TABLE::NOTE_GROUP),
                               new QTableWidgetItem(QString::fromStdString(group)));

          //  ui->table->
        }
        ui->table->resizeColumnToContents(static_cast<int>(NOTES_TABLE::ID_COLUMN));
        ui->table->resizeColumnToContents(static_cast<int>(NOTES_TABLE::NOTE_GROUP));
        ui->table->resizeRowsToContents(); // Автоматически подбирает высоту

    }
    catch (std::runtime_error &err ){
        qDebug() << "db error " << err.what();
    }
}


void Notes::showCustomMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context Menu"), this);

    contextMenu.addAction(ui->actionAdd_note);
    contextMenu.addAction(ui->actionDelete_note);

    connect(ui->actionDelete_note, &QAction::triggered,
            this, [this, pos](){deleteNote(pos);});
    // Display the menu at the cursor's global position
    contextMenu.exec(this->mapToGlobal(pos));
}

void Notes::deleteNote(const QPoint &pos) {

    auto items = ui->table->selectedItems();
    if (items.size() < 1) return;

    int row_num = items.at(0)->row();

    QString id_str = ui->table->item(row_num, int(NOTES_TABLE::ID_COLUMN))->text();
    qDebug() << id_str;
    if (DB.exec("DELETE FROM notes WHERE id = " + id_str)) {
        ui->table->removeRow(row_num);
    }
}


AddNoteDialog::AddNoteDialog(QWidget *parent) :
    QDialog(parent) {

    setWindowTitle(tr("Create new note"));

    QFormLayout *lay = new QFormLayout(this);
    _line_edit = new QLineEdit();
    _plain_text = new QPlainTextEdit();
    _cmb_groups = new QComboBox();

    auto rows = DB.select("note_groups",
                          {"id", R"("group")"});
    foreach (DataBase::SQLiteRow db_row, rows) {

        int id;
        std::string group_name;

        id = std::get<int>(db_row.at(0));
        group_name = std::get<std::string>(db_row.at(1));

        _cmb_groups->addItem(QString::fromStdString(group_name), id);
    }

    Notes *notes_par = dynamic_cast<Notes*> (parent);
    lay->addRow(notes_par->_headers_name[NOTES_TABLE::NOTE_HEADER_COLUMN], _line_edit);
    lay->addRow(notes_par->_headers_name[NOTES_TABLE::NOTE_GROUP], _cmb_groups);
    lay->addRow(notes_par->_headers_name[NOTES_TABLE::NOTE_COLUMN], _plain_text);

    QPushButton *button = new QPushButton(tr("Create"), this);
    lay->addRow("",button);

    connect (button, &QPushButton::pressed, this,
            [=](){

        qDebug() << _line_edit->text() << "  " << _plain_text->toPlainText()
                 << " " << _cmb_groups->currentData().toInt();
        DB.insert("notes",
                  {{"note_header", _line_edit->text().toStdString()},
                   {"note", _plain_text->toPlainText().toStdString()},
                   {"note_group", _cmb_groups->currentData().toInt()}});
        accept();
    });

    setLayout(lay);
}


void FontDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QFont font = option.font;

    // Устанавливаем разные шрифты для разных колонок
    switch (index.column()) {
    case static_cast<int>(NOTES_TABLE::NOTE_HEADER_COLUMN): // Первая колонка — жирный шрифт
        font.setBold(true);
        font.setPointSize(font.pointSize() + 2);
        break;
    // case 1: // Вторая колонка — курсив
    //     font.setItalic(true);
    //     break;
    // case 2: // Третья колонка — увеличенный размер
    //     font.setPointSize(font.pointSize() + 2);
    //     break;
    default:
        break;
    }

    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignHCenter;
    opt.font = font;
    QStyledItemDelegate::paint(painter, opt, index);
}
