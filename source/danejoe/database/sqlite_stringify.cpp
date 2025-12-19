#include "danejoe/common/core/data_type.hpp"
#include "danejoe/condition/range_condition.hpp"
#include "danejoe/database/sqlite_stringify.hpp"

std::optional<std::string> DaneJoe::SqliteStringify::try_format_data_type(DaneJoe::DataType type) const
{
    switch (type)
    {
    case DaneJoe::DataType::UInt8:
    case DaneJoe::DataType::Int8:
    case DaneJoe::DataType::UInt16:
    case DaneJoe::DataType::Int16:
    case DaneJoe::DataType::UInt32:
    case DaneJoe::DataType::Int32:
    case DaneJoe::DataType::UInt64:
    case DaneJoe::DataType::Int64:
        return "INTEGER";
    case DaneJoe::DataType::Float:
    case DaneJoe::DataType::Double:
        return "REAL";
    case DaneJoe::DataType::Bool:
        return "INTEGER";
    case DaneJoe::DataType::String:
        return "TEXT";
    case DaneJoe::DataType::ByteArray:
        return "BLOB";
    case DaneJoe::DataType::Null:
        return "NULL";
    default:
        return std::nullopt;
    }
}

std::optional<std::string> DaneJoe::SqliteStringify::try_format_condition(const SqlConditionItem& item)const
{
    if (is_integer_type(item.column_info.data_type))
    {
        return try_format_range_condition(item);
    }
    else if (is_string_type(item.column_info.data_type))
    {
        return try_format_text_condition(item);
    }
    else
    {
        return std::nullopt;
    }
}

std::optional<std::string> DaneJoe::SqliteStringify::try_format_range_condition(const SqlConditionItem& item)const
{
    if (!is_integer_type(item.column_info.data_type))
    {
        return std::nullopt;
    }
    auto condition = std::dynamic_pointer_cast<IRangeCondition>(item.condition);
    auto intervals = condition->get_integer_intervals();
    if (intervals.empty())
    {
        return std::nullopt;
    }
    std::string result;
    for (auto& interval : intervals)
    {
        auto interval_string_opt = try_format_integer_interval(interval, item.column_info.column_name);
        if (!interval_string_opt.has_value())
        {
            continue;
        }
        if (!result.empty())
        {
            result += m_sign_config.space_sign +
                m_sign_config.or_sign +
                m_sign_config.space_sign;
        }
        result += interval_string_opt.value();
    }
    return result;
}

std::optional<std::string> DaneJoe::SqliteStringify::try_format_integer_interval(const SingleInterval<int64_t>& interval, const std::string& column_name)const
{
    auto left_endpoint = interval.get_left_endpoint();
    auto right_endpoint = interval.get_right_endpoint();
    std::string left_condition;
    if (left_endpoint.get_value().has_value())
    {
        left_condition += column_name;
        left_condition += m_sign_config.space_sign;
        left_condition += left_endpoint.is_open() ?
            m_sign_config.greater_than_sign :
            m_sign_config.greater_than_sign +
            m_sign_config.equal_sign;
        left_condition += m_sign_config.space_sign;
        left_condition += m_sign_config.place_holder_sign;
    }
    std::string right_condition;
    if (right_endpoint.get_value().has_value())
    {
        right_condition += column_name;
        right_condition += m_sign_config.space_sign;
        right_condition += right_endpoint.is_open() ?
            m_sign_config.less_than_sign :
            m_sign_config.less_than_sign +
            m_sign_config.equal_sign;
        right_condition += m_sign_config.space_sign;
        right_condition += m_sign_config.place_holder_sign;
    }
    std::string result;
    if (left_condition.empty() && right_condition.empty())
    {
        return std::nullopt;
    }
    else if (!left_condition.empty() && !right_condition.empty())
    {
        result = left_condition +
            m_sign_config.space_sign +
            m_sign_config.and_sign +
            m_sign_config.space_sign +
            right_condition;
    }
    else
    {
        result = left_condition.empty() ? right_condition : left_condition;
    }
    return m_sign_config.left_parenthesis +
        m_sign_config.space_sign +
        result +
        m_sign_config.space_sign +
        m_sign_config.right_parenthesis;
}

std::optional<std::string> DaneJoe::SqliteStringify::try_format_text_condition(const SqlConditionItem& item)const
{
    if (!is_string_type(item.column_info.data_type))
    {
        return std::nullopt;
    }
    return std::nullopt;
}
