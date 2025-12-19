/**
 * @file sql_condition.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库条件信息
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>
#include <optional>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/database/sql_schema.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{

    /**
     * @brief 数据库条件信息
     */
    struct SqlConditionItem
    {
        /// @brief 是否设置
        bool is_set = false;
        /// @brief 列信息
        SqlColumnItem column_info;
        /// @brief 条件
        std::shared_ptr<ICondition> condition;
        /// @brief 是否降序
        std::optional<bool> is_desc_order;
    };
}
