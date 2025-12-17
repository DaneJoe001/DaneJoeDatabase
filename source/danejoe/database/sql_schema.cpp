#include <format>

#include "danejoe/database/sql_schema.hpp"

bool DaneJoe::SqlColumnItem::operator==(const SqlColumnItem& other)const
{
    return column_index == other.column_index && column_name == other.column_name && data_type == other.data_type;
}

bool DaneJoe::SqlColumnItem::is_valid()const
{
    return column_index >= 0 && column_name != "" && data_type != DaneJoe::DataType::Unknown;
}

std::string DaneJoe::SqlColumnItem::to_string()const
{
    return std::format("SqlColumnItem: {{column_index:{}, column_name:{}, data_type:{}, is_primary_key:{}, is_not_null:{}, is_unique:{} }}",
        column_index,
        column_name,
        DaneJoe::to_string(data_type),
        is_primary_key ? "true" : "false",
        is_not_null ? "true" : "false",
        is_unique ? "true" : "false");
}

bool DaneJoe::SqlTableItem::is_valid()const
{
    bool is_valid = table_name != "" && !column_items.empty();
    for (const auto& column : column_items)
    {
        if (!column.is_valid())
        {
            is_valid = false;
            break;
        }
    }
    return is_valid;
}

bool DaneJoe::SqlTableItem::has_column(const std::string& column_name)const
{
    for (const auto& column : column_items)
    {
        if (column.column_name == column_name)
        {
            return true;
        }
    }
    return false;
}
bool DaneJoe::SqlTableItem::has_column(const SqlColumnItem& column_name)const
{
    for (const auto& column : column_items)
    {
        if (column == column_name)
        {
            return true;
        }
    }
    return false;
}

std::optional<DaneJoe::SqlColumnItem> DaneJoe::SqlTableItem::get_column_info(const std::string& column_name)const
{
    for (const auto& column : column_items)
    {
        if (column.column_name == column_name)
        {
            return column;
        }
    }
    return std::nullopt;
}
