/**
 * @file sql_database.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>

#include "danejoe/database/sql_config.hpp"
#include "danejoe/database/i_sql_driver.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlDatabase
     * @brief 数据库
     */
    class SqlDatabase
    {
    public:
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         */
        SqlDatabase(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 设置数据库配置
         * @param config 数据库配置
         */
        void set_config(const SqlConfig& config);
        /**
         * @brief 连接数据库
         * @return true 连接成功
         * @return false 连接失败
         */
        bool connect();
        /**
         * @brief 获取数据库驱动
         * @return 数据库驱动
         */
        std::shared_ptr<ISqlDriver> get_driver();
    private:
        /// @brief 数据库配置
        SqlConfig m_config;
        /// @brief 数据库驱动
        std::shared_ptr<ISqlDriver> m_driver;
    };
    /// @brief 数据库指针(shared_ptr)
    using SqlDatabasePtr = std::shared_ptr<SqlDatabase>;

}
