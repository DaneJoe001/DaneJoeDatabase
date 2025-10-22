#pragma once

#include <unordered_map>

#include "danejoe/database/sql_database.hpp"

extern "C"
{
#include <sqlite3.h>
}

namespace DaneJoe
{
    struct SqliteStmtDeleter
    {
        void operator()(sqlite3_stmt* stmt);
    };
    using SqliteStmtPtr = std::unique_ptr<sqlite3_stmt, SqliteStmtDeleter>;
    class SqliteDriver : public ISqlDriver
    {
    public:
        SqliteDriver();
        ~SqliteDriver();
        bool connect(const SqlConfig& config)override;
        bool prepare(const std::string& sql)override;
        bool bind(const std::string& sql,int index, const SqlCell& cell)override;
        void reset(const std::string& sql)override;
        void clear_bindings(const std::string& sql)override;
        std::vector<std::vector<SqlCell>> execute_query(const std::string& sql)override;
        bool execute_command(const std::string& sql)override;
        void close()override;
    private:
        /// @brief Sqlite 句柄指针
        sqlite3* m_db = nullptr;
        /// @brief 准备语句映射
        std::unordered_map<std::string, SqliteStmtPtr> m_stmts;
    };

}