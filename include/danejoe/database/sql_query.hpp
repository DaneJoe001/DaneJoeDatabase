/**
 * @file sql_query.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 查询
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>

#include "danejoe/database/i_sql_driver.hpp"
#include "danejoe/database/sql_database.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlQuery
     * @brief 查询
     */
    class SqlQuery
    {
    public:
        /**
         * @brief 构造函数
         * @param database 数据库
         */
        SqlQuery(std::shared_ptr<SqlDatabase> database);
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         */
        SqlQuery(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return 查询
         */
        SqlQuery& prepare(const std::string& sql);
        /**
         * @brief 绑定SQL语句
         * @tparam T 数据类型
         * @param index 参数索引
         * @param value 数据
         * @return 查询
         */
        template<class T>
        SqlQuery& bind(int index, T value)
        {
            SqlCell cell;
            cell.type = get_data_type<T>();
            cell.data = value;
            return bind(index, cell);
        }
        /**
         * @brief 绑定SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return 查询
         */
        SqlQuery& bind(int index, const SqlCell& cell);
        /**
         * @brief 清除SQL语句绑定
         */
        void clear_bindings();
        /**
         * @brief 重置SQL语句
         */
        void reset();
        /**
         * @brief 执行查询
         * @return 查询结果
         */
        std::vector<std::vector<SqlCell>> execute_query();
        /**
         * @brief 执行命令
         * @return true 执行成功
         * @return false 执行失败
         */
        bool execute_command();
    private:
        /// @brief SQL语句
        std::string m_sql;
        /// @brief 数据库驱动
        std::weak_ptr<ISqlDriver> m_driver;
    };
    /// @brief 查询指针(shared_ptr)
    using SqlQueryPtr = std::shared_ptr<SqlQuery>;
}
