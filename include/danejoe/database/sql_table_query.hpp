/**
 * @file sql_table_query.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库表查询
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include "danejoe/database/sql_query.hpp"
#include "danejoe/database/sql_builder.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlTableQuery
     * @brief 数据库表查询
     */
    class SqlTableQuery
    {
    public:
        /**
         * @brief 构造函数
         * @param stringify 字符串化接口
         * @param table_info 表信息
         * @param query 查询
         */
        SqlTableQuery(std::shared_ptr<ISqlStringify> stringify, const std::shared_ptr<const SqlTableItem> table_info, SqlQueryPtr query);
        /**
         * @brief 创建表
         * @return true 创建成功
         * @return false 创建失败
         */
        bool create();
        /**
         * @brief 更新
         * @param cells 单元格
         * @param conditions 条件
         * @return true 更新成功
         * @return false 更新失败
         */
        bool update(const std::vector<SqlCell>& cells, const std::vector<SqlConditionItem>& conditions);
        /**
         * @brief 插入
         * @param cells 单元格
         * @return true 插入成功
         * @return false 插入失败
         */
        bool insert(const std::vector<SqlCell>& cells);
        /**
         * @brief 删除
         * @param conditions 条件
         * @return true 删除成功
         * @return false 删除失败
         */
        bool remove(const std::vector<SqlConditionItem>& conditions);
        /**
         * @brief 查询
         * @param dest_columns 目标列
         * @param conditions 条件
         * @return 查询结果
         */
        std::vector<std::vector<SqlCell>> select(const std::vector<SqlColumnItem>& dest_columns, const std::vector<SqlConditionItem>& conditions);
    private:
        /**
         * @brief 获取目标列
         * @param cells 单元格
         * @return 目标列
         */
        std::vector<SqlColumnItem> get_dest_columns(const std::vector<SqlCell>& cells)const;
    private:
        /// @brief 表信息
        std::shared_ptr<const SqlTableItem> m_table_info = nullptr;
        /// @brief 构建器
        SqlBuilder m_builder;
        /// @brief 查询
        SqlQueryPtr m_query = nullptr;
    };
}
