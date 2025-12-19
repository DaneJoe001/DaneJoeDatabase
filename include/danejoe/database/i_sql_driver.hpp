/**
 * @file i_sql_driver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库驱动接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>

#include "danejoe/database/sql_config.hpp"
#include "danejoe/database/sql_cell.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISqlDriver
     * @brief 数据库驱动接口
     */
    class ISqlDriver
    {
    public:
        /**
         * @brief 连接数据库
         * @param config 数据库配置
         * @return true 连接成功
         * @return false 连接失败
         */
        virtual bool connect(const SqlConfig& config) = 0;
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return true 准备成功
         * @return false 准备失败
         */
        virtual bool prepare(const std::string& sql) = 0;
        /**
         * @brief 绑定SQL语句
         * @param sql SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return true 绑定成功
         * @return false 绑定失败
         */
        virtual bool bind(const std::string& sql, int index, const SqlCell& cell) = 0;
        /**
         * @brief 重置SQL语句
         * @param sql SQL语句
         */
        virtual void reset(const std::string& sql) = 0;
        /**
         * @brief 清除SQL语句绑定
         * @param sql SQL语句
         */
        virtual void clear_bindings(const std::string& sql) = 0;
        /**
         * @brief 执行查询
         * @param sql SQL语句
         * @return 查询结果
         */
        virtual std::vector<std::vector<SqlCell>> execute_query(const std::string& sql) = 0;
        /**
         * @brief 执行命令
         * @param sql SQL语句
         * @return true 执行成功
         * @return false 执行失败
         */
        virtual bool execute_command(const std::string& sql) = 0;
        /**
         * @brief 关闭数据库
         */
        virtual void close() = 0;
    };
    /// @brief 数据库驱动指针(shared_ptr)
    using ISqlDriverPtr = std::shared_ptr<ISqlDriver>;
}
