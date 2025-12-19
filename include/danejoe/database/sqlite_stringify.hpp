/**
 * @file sqlite_stringify.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief SQLite字符串化接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include "danejoe/condition/single_interval.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqliteStringify
     * @brief SQLite字符串化接口
     */
    struct SqliteStringify : public ISqlStringify
    {
        /**
         * @brief 格式化数据类型
         * @param type 数据类型
         * @return 字符串
         */
        std::optional<std::string> try_format_data_type(DataType type) const override;
        /**
         * @brief 构建条件字符串
         * @param item 条件信息
         * @return 字符串
         */
        std::optional<std::string> try_format_condition(const SqlConditionItem& item) const override;
        /**
         * @brief 构建范围条件字符串
         * @param item 条件信息
         * @return 字符串
         */
        std::optional<std::string> try_format_range_condition(const SqlConditionItem& item)const;
        /**
         * @brief 构建文本条件字符串
         * @param item 条件信息
         * @return 字符串
         */
        std::optional<std::string> try_format_text_condition(const SqlConditionItem& item)const;
        /**
         * @brief 构建整数区间条件字符串
         * @param interval 区间
         * @param column_name 列名称
         * @return 字符串
         */
        std::optional<std::string>  try_format_integer_interval(const SingleInterval<int64_t>& interval, const std::string& column_name)const;
    };
}
