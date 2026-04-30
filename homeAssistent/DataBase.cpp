#include "DataBase.h"

#include <stdexcept>
#include <iostream>

DataBase::DataBase() {

}


DataBase &DataBase::instance(){

    static DataBase instance;
    return instance;
}


bool DataBase::init(const std::string &db_path) {

    if (_db != nullptr) return false;

    int rc = sqlite3_open(db_path.c_str(), &_db);

    if (rc != SQLITE_OK)
        throw std::runtime_error( "Can't connect to sqlite3 database '" + db_path + "'");

    return true;
}


bool DataBase::insert(const std::string &table_name,
                      const std::map<std::string, SQLiteValue> &data) {

    if (data.empty())
        return false;

    std::string sql = "INSERT INTO " + table_name + " (";

    // Формируем список колонок
    bool first = true;
    for (const auto& [column, _] : data) {
        if (!first) sql += ", ";
        sql += column;
        first = false;
    }

    sql += ") VALUES (";

    // Формируем плейсхолдеры
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += "?";
    }
    sql += ")";

    // Подготавливаем выражение
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(_db));
    }

    // Привязываем значения
    int index = 1;
    for (const auto& [_, value] : data) {
        bindValue(stmt, index++, value);
    }

    // Выполняем вставку
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    return success;
}

std::vector<DataBase::SQLiteRow> DataBase::select(const std::string &table_name,
                                                  const std::vector<std::string> &columns,
                                                  const std::map<std::string, SQLiteValue> &conditions,
                                                  const std::string &additional_where,
                                                  const std::string &order_by,
                                                  int limit) {

    std::vector<SQLiteRow> result;

    // Формируем список колонок
    std::string columnsStr;
    if (columns.empty())
        columnsStr = "*";

    else {

        for (size_t i = 0; i < columns.size(); ++i) {
            if (i > 0) columnsStr += ", ";
            columnsStr += columns[i];
        }
    }

    std::string sql = "SELECT " + columnsStr + " FROM " + table_name;

    // Добавляем WHERE
    if (!conditions.empty() || !additional_where.empty()) {
        sql += " WHERE ";

        bool first = true;
        for (const auto& [column, _] : conditions) {
            if (!first) sql += " AND ";
            sql += column + " = ?";
            first = false;
        }

        if (!additional_where.empty()) {
            if (!first) sql += " AND ";
            sql += additional_where;
        }
    }

    // Добавляем ORDER BY и LIMIT
    if (!order_by.empty())
        sql += " ORDER BY " + order_by;

    if (limit != -1)
        sql += " LIMIT " + std::to_string(limit);

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(_db));
    }

    int index = 1;
    for (const auto& [_, value] : conditions) {
        bindValue(stmt, index++, value);
    }

    int colCount = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SQLiteRow row;
        for (int i = 0; i < colCount; i++) {
            row.push_back(extractValue(stmt, i));
        }
        result.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    return result;
}



void DataBase::bindValue(sqlite3_stmt* stmt,
                         int index,
                         const SQLiteValue& value) {

    std::visit([stmt, index](const auto& v) {

        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            sqlite3_bind_null(stmt, index);
        }

        else if constexpr (std::is_same_v<T, int>) {
            sqlite3_bind_int(stmt, index, v);
        }

        else if constexpr (std::is_same_v<T, double>) {
            sqlite3_bind_double(stmt, index, v);
        }

        else if constexpr (std::is_same_v<T, std::string>) {
            sqlite3_bind_text(stmt, index, v.c_str(), -1, SQLITE_TRANSIENT);
        }
    }, value);
}



DataBase::SQLiteValue DataBase::extractValue(sqlite3_stmt *stmt,
                                             int col) {

    int type = sqlite3_column_type(stmt, col);

    switch (type) {

    case SQLITE_INTEGER:
        return sqlite3_column_int(stmt, col);

    case SQLITE_FLOAT:
        return sqlite3_column_double(stmt, col);

    case SQLITE_TEXT: {

        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
        return std::string(text ? text : "");
    }

    case SQLITE_BLOB: {
        // Для BLOB возвращаем строковое представление размера
        int size = sqlite3_column_bytes(stmt, col);
        return std::string("[BLOB size: " + std::to_string(size) + "]");
    }

    case SQLITE_NULL:
    default:
        return nullptr;
    }
}

