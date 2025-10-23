#pragma once

/**
 * @file sql_database_manager.hpp
 * @brief 数据库管理器
 * @author DaneJoe001
 * @version 0.1.1
 * @date 2025-10-24
 */

#include <memory>
#include <string>
#include <optional>
#include <unordered_map>

#include "danejoe/database/sql_database.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @class SqlDatabaseManager
     * @brief 数据库管理器
     */
    class SqlDatabaseManager
    {
    public:
        /**
         * @brief 获取实例
         * @return 实例
         */
        static SqlDatabaseManager& get_instance();
        /**
         * @brief 获取数据库
         * @param database_name 数据库名称
         * @return 数据库
         */
        std::shared_ptr<SqlDatabase> get_database(const std::string& database_name);
        /**
         * @brief 添加数据库
         * @param database_name 数据库名称
         * @param database_type 数据库类型
         */
        void add_database(const std::string& database_name, std::shared_ptr<ISqlDriver> driver);
    private:
        /**
         * @brief 构造函数
         */
        SqlDatabaseManager();
        /**
         * @brief 析构函数
         */
        ~SqlDatabaseManager();
    private:
        /// @brief 数据库
        std::unordered_map<std::string, std::shared_ptr<SqlDatabase>> m_databases;
    };
}