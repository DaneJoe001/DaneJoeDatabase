/**
 * @file sql_operation.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库操作类型
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <string>

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 数据库操作类型
     */
    enum class SqlOperation
    {
        /// @brief 查询操作
        Select,
        /// @brief 插入操作
        Insert,
        /// @brief 更新操作
        Update,
        /// @brief 删除操作
        Delete,
        /// @brief 未知操作
        Unknown
    };
    /**
     * @brief 将数据库操作类型转换为字符串
     * @param operation 数据库操作类型
     * @return 字符串
     */
    std::string to_string(SqlOperation operation);
    /**
     * @brief 将字符串转换为数据库操作类型
     * @param operation_string 字符串
     * @return 数据库操作类型
     */
    SqlOperation to_sql_operation(const std::string& operation_string);
}
