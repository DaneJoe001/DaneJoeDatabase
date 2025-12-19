/**
 * @file sqlite_driver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief SQLite 驱动
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>
#include <unordered_map>

#include "danejoe/database/i_sql_driver.hpp"

 // #include "danejoe/database/sql_database.hpp"

extern "C"
{
#include <sqlite3.h>
}

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @struct SqliteStmtDeleter
     * @brief SQLite 语句删除器
     */
    struct SqliteStmtDeleter
    {
        /**
         * @brief 删除 SQLite 语句
         * @param stmt SQLite 语句
         */
        void operator()(sqlite3_stmt* stmt);
    };
    /// @brief SQLite 语句指针(unique_ptr)
    using SqliteStmtPtr = std::unique_ptr<sqlite3_stmt, SqliteStmtDeleter>;
    /**
     * @class SqliteDriver
     * @brief SQLite 驱动
     */
    class SqliteDriver : public ISqlDriver
    {
    public:
        /**
         * @brief 构造函数
         */
        SqliteDriver();
        /**
         * @brief 析构函数
         */
        ~SqliteDriver();
        /**
         * @brief 连接数据库
         * @param config 数据库配置
         * @return true 连接成功
         * @return false 连接失败
         */
        bool connect(const SqlConfig& config)override;
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return true 准备成功
         * @return false 准备失败
         */
        bool prepare(const std::string& sql)override;
        /**
         * @brief 绑定SQL语句
         * @param sql SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return true 绑定成功
         * @return false 绑定失败
         */
        bool bind(const std::string& sql, int index, const SqlCell& cell)override;
        /**
         * @brief 重置SQL语句
         * @param sql SQL语句
         */
        void reset(const std::string& sql)override;
        /**
         * @brief 清除SQL语句绑定
         * @param sql SQL语句
         */
        void clear_bindings(const std::string& sql)override;
        /**
         * @brief 执行查询
         * @param sql SQL语句
         * @return 查询结果
         */
        std::vector<std::vector<SqlCell>> execute_query(const std::string& sql)override;
        /**
         * @brief 执行命令
         * @param sql SQL语句
         * @return true 执行成功
         * @return false 执行失败
         */
        bool execute_command(const std::string& sql)override;
        /**
         * @brief 关闭数据库
         */
        void close()override;
    private:
        /// @brief Sqlite 句柄指针
        sqlite3* m_db = nullptr;
        /// @brief 准备语句映射
        std::unordered_map<std::string, SqliteStmtPtr> m_stmts;
    };

}
