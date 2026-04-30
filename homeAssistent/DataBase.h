#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <map>
#include <variant>
#include <vector>

#define DB static_cast<DataBase&>(DataBase::instance())

class DataBase
{
public:

    /// Тип для поддержки разных значений
    using SQLiteValue = std::variant<
        std::nullptr_t,    ///< NULL
        int,               ///< INTEGER
        double,            ///< REAL
        std::string       ///< TEXT
    >;
    using SQLiteRow = std::vector<SQLiteValue>;


    DataBase();
    ~DataBase() {sqlite3_close(_db);}

    static DataBase& instance();

    bool init(const std::string& db_path);

    bool insert(const std::string& table_name,
                const std::map<std::string, SQLiteValue> &data);

    /// универсальный метод получения значений из БД
    std::vector<DataBase::SQLiteRow> select(const std::string &table_name,
                                            const std::vector<std::string> &columns,
                                            const std::map<std::string, SQLiteValue> &conditions = {},
                                            const std::string &additional_where = "",
                                            const std::string &order_by = "",
                                            int limit = -1);
private:




    /// Вспомогательная функция для привязки значений разных типов
    void bindValue(sqlite3_stmt *stmt,
                   int index,
                   const SQLiteValue &value);

    /// Вспомогательная функция для извлечения значения из стейта
    DataBase::SQLiteValue extractValue(sqlite3_stmt *stmt,
                                       int col);
    sqlite3* _db;
};

#endif // DATABASE_H
