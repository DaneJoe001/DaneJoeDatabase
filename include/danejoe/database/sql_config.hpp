/**
 * @file sql_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库配置
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
     * @struct SqlConfig
     * @brief 数据库配置
     */
    struct SqlConfig
    {
        /// @brief 数据库名称
        std::string database_name = "default";
        /// @brief 数据库路径
        std::string path = "./database/default.db";
        /// @brief 用户名
        std::string user_name;
        /// @brief 用户密码
        std::string user_password;
    };
}
