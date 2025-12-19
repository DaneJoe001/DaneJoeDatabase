/**
 * @file sql_builder.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Sql语句构建器
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "danejoe/database/sql_schema.hpp"
#include "danejoe/database/sql_condition.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 数据库构建器
     */
    class SqlBuilder
    {
    public:
        /**
         * @brief 构造函数
         * @param table_info 表信息
         * @param sql_stringify 字符串化接口
         */
        SqlBuilder(const std::shared_ptr<const SqlTableItem> table_info, std::shared_ptr<ISqlStringify> sql_stringify);
        /**
         * @brief 设置字符串化接口
         * @param sql_data_type 字符串化接口
         */
        void set_stringify(std::shared_ptr<ISqlStringify> sql_data_type);
        /**
         * @brief 构建创建表字符串
         * @return 字符串
         */
        std::optional<std::string> build_create_table_string()const;
        /**
         * @brief 构建条件字符串
         * @param condition_items 条件信息
         * @return 字符串
         */
        std::optional<std::string> build_condition_string(const std::vector<SqlConditionItem>& condition_items)const;
        /**
         * @brief 构建插入字符串
         * @param dest_column_items 目标列信息
         * @return 字符串
         */
        std::optional<std::string> build_insert_string(const std::vector<SqlColumnItem>& dest_column_items)const;
        /**
         * @brief 构建查询字符串
         * @param dest_column_items 目标列信息
         * @param condition_items 条件信息
         * @return 字符串
         */
        std::optional<std::string> build_select_string(
            const std::vector<SqlColumnItem>& dest_column_items,
            const std::vector<SqlConditionItem>& condition_items)const;
        /**
         * @brief 构建更新字符串
         * @param dest_column_items 目标列信息
         * @param condition_items 条件信息
         * @return 字符串
         */
        std::optional<std::string> build_update_string(
            const std::vector<SqlColumnItem>& dest_column_items,
            const std::vector<SqlConditionItem>& condition_items, bool enable_all = false)const;
        /**
         * @brief 构建删除字符串
         * @param condition_items 条件信息
         * @param enable_all 是否启用全部
         * @return 字符串
         */
        std::optional<std::string> build_delete_string(const std::vector<SqlConditionItem>& condition_items, bool
            enable_all = false)const;
    private:
        /**
         * @brief 构建目标列字符串
         * @param column_items 列信息
         * @return 字符串
         */
        std::optional<std::string> build_dest_columns_string(const std::vector<SqlColumnItem>& column_items)const;
    private:
        /// @brief 记录表信息
        std::shared_ptr<const SqlTableItem> m_table_info = nullptr;
        /// @brief 字符串化接口
        std::shared_ptr<ISqlStringify> m_sql_stringify = nullptr;
    };
}
