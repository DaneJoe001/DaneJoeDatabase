/**
 * @file sql_sign_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 符号配置
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
     * @brief 符号配置
     */
    struct SqlSignConfig
    {
        /// @brief 占位符符号
        std::string place_holder_sign = "?";
        /// @brief 等于符号
        std::string equal_sign = "=";
        /// @brief 小于符号
        std::string less_than_sign = "<";
        /// @brief 大于符号
        std::string greater_than_sign = ">";
        /// @brief 逗号符号
        std::string comma_sign = ",";
        /// @brief 空格符号
        std::string space_sign = " ";
        /// @brief OR符号
        std::string or_sign = "OR";
        /// @brief AND符号
        std::string and_sign = "AND";
        /// @brief 左括号
        std::string left_parenthesis = "(";
        /// @brief 右括号
        std::string right_parenthesis = ")";
    };
}
