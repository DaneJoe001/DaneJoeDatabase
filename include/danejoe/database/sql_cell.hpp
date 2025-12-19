/**
 * @file sql_cell.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据单元
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>

#include "danejoe/common/core/data_type.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 数据单元
     */
    using CellData = ::std::variant<
        int8_t,
        uint8_t,
        int16_t,
        uint16_t,
        int32_t,
        uint32_t,
        int64_t,
        uint64_t,
        float,
        double,
        bool,
        ::std::string,
        ::std::vector<uint8_t>,
        ::std::nullptr_t
    >;
    /**
     * @struct SqlCell
     * @brief 数据单元
     */
    struct SqlCell
    {
        /// @brief 数据类型枚举
        DataType type;
        /// @brief 数据
        CellData data;
        /// @brief 列名
        std::string column_name;
    };
}
