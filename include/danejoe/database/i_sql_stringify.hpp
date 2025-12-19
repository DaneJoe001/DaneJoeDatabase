/**
 * @file i_sql_stringify.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库字符串化接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <optional>
#include <string>

#include "danejoe/common/core/data_type.hpp"
#include "danejoe/database/sql_condition.hpp"
#include "danejoe/database/sql_sign_config.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 数据库字符串化接口
     */
    class ISqlStringify
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~ISqlStringify() {}
        /**
         * @brief 格式化数据类型
         * @param type 数据类型
         * @return 字符串
         */
        virtual std::optional<std::string> try_format_data_type(DataType type) const = 0;
        /**
         * @brief 构建条件字符串
         * @param item 条件信息
         * @return 字符串
         */
        virtual std::optional<std::string> try_format_condition(const SqlConditionItem& item)const = 0;
        /**
         * @brief 设置符号配置
         * @param config 符号配置
         */
        void set_sign_config(const SqlSignConfig& config);
        /**
         * @brief 获取符号配置
         * @return 符号配置
         */
        SqlSignConfig get_sign_config();
    protected:
        /// @brief 符号配置
        SqlSignConfig m_sign_config;
    };
}
