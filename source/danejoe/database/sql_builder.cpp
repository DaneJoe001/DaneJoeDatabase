#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/database/sql_builder.hpp"
#include "danejoe/database/sql_sign_config.hpp"

DaneJoe::SqlBuilder::SqlBuilder(const std::shared_ptr<const SqlTableItem> table_info, std::shared_ptr<ISqlStringify> sql_stringify)
    :m_table_info(table_info),
    m_sql_stringify(sql_stringify)
{}

void DaneJoe::SqlBuilder::set_stringer(std::shared_ptr<ISqlStringify> sql_stringify)
{
    m_sql_stringify = sql_stringify;
}

std::optional<std::string> DaneJoe::SqlBuilder::build_create_table_string()const
{
    if (!m_table_info->is_valid() || !m_sql_stringify)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build create table string: invalid table info or sql data type not set.");
        return std::nullopt;
    }
    SqlSignConfig sign_config = m_sql_stringify->get_sign_config();
    std::string fileds_string;
    std::string single_field_string;
    for (const auto& column : m_table_info->column_items)
    {
        auto type_string_opt = m_sql_stringify->format_data_type(column.data_type);
        if (!type_string_opt.has_value())
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build create table string: unknown data type for column {}.", column.column_name);
            return std::nullopt;
        }
        if (fileds_string != "")
        {
            fileds_string += sign_config.comma_sign + sign_config.space_sign;
        }
        single_field_string = std::format("{} {}", column.column_name, type_string_opt.value());
        if (column.is_primary_key)
        {
            single_field_string += " PRIMARY KEY";
        }
        if (column.is_not_null)
        {
            single_field_string += " NOT NULL";
        }
        if (column.is_unique)
        {
            single_field_string += " UNIQUE";
        }
        if (column.default_value.has_value())
        {
            single_field_string += " DEFAULT ?";
        }
        fileds_string += single_field_string;
    }
    std::string create_table_string = "CREATE TABLE";
    if (m_table_info->is_unique)
    {
        create_table_string += " IF NOT EXISTS";
    }
    create_table_string += std::format(" {} ({});", m_table_info->table_name, fileds_string);
    return create_table_string;
}
std::optional<std::string>  DaneJoe::SqlBuilder::build_condition_string(const std::vector<SqlConditionItem>& condition_items)const
{
    if (!m_table_info->is_valid() || !m_sql_stringify)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build condition string: invalid table info.");
        return std::nullopt;
    }
    // 获取符号配置
    SqlSignConfig sign_config = m_sql_stringify->get_sign_config();
    // 是否已设置排序
    bool is_set_order = false;
    // 条件字符串
    std::string condition_string;
    // 遍历条件项
    for (const auto& condition_item : condition_items)
    {
        // 检查是否设置该项条件
        if (!condition_item.is_set)
        {
            continue;
        }
        // 检查是否含有该列
        /// @todo 考虑跨表查询
        if (!m_table_info->has_column(condition_item.column_info))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build condition string: column {} does not exist in table {}.", condition_item.column_info.column_name, m_table_info->table_name);
            continue;
        }
        // 构建条件字符串
        auto single_condition_string_opt = m_sql_stringify->format_condition(condition_item);
        if (!single_condition_string_opt.has_value())
        {
            continue;
        }
        // 在条件字符串不为空时添加AND运算
        if (!condition_string.empty())
        {
            condition_string += sign_config.space_sign;
            condition_string += sign_config.and_sign;
            condition_string += sign_config.space_sign;
        }
        condition_string += single_condition_string_opt.value();
        if (condition_item.is_desc_order.has_value())
        {
            /// @brief 不允许反复设置排序
            if (is_set_order)
            {
                return std::nullopt;
            }
            if (condition_item.is_desc_order.value())
            {
                condition_string += " ORDER BY";
                condition_string += " " + condition_item.column_info.column_name;
                condition_string += " DESC";
            }
            else
            {
                condition_string += " ORDER BY";
                condition_string += " " + condition_item.column_info.column_name;
                condition_string += " ASC";
            }
        }
    }
    if (condition_string.empty())
    {
        return std::nullopt;
    }
    return condition_string;
}
std::optional<std::string>  DaneJoe::SqlBuilder::build_insert_string(const std::vector<SqlColumnItem>& dest_column_items)const
{
    if (!m_table_info->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build insert string: invalid table info.");
        return std::nullopt;
    }
    auto dest_column_items_opt = build_dest_columns_string(dest_column_items);
    if (!dest_column_items_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build insert string: invalid destination columns.");
        return std::nullopt;
    }
    std::string place_holders_string;
    /// @todo 在遍历时需检查对应列是否存在has_column
    for (const auto& column_item : dest_column_items)
    {
        if (!m_table_info->has_column(column_item))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build insert string: column {} does not exist in table {}.", column_item.column_name, m_table_info->table_name);
            continue;
        }
        if (!place_holders_string.empty())
        {
            place_holders_string += m_sql_stringify->get_sign_config().comma_sign;
            place_holders_string += m_sql_stringify->get_sign_config().space_sign;
        }
        place_holders_string += m_sql_stringify->get_sign_config().place_holder_sign;
    }
    return std::format("INSERT INTO {} ({}) VALUES ({});", m_table_info->table_name, dest_column_items_opt.value(), place_holders_string);
}
std::optional<std::string>  DaneJoe::SqlBuilder::build_select_string(
    const std::vector<SqlColumnItem>& dest_column_items,
    const std::vector<SqlConditionItem>& condition_items)const
{
    if (!m_table_info->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build select string: invalid table info.");
        return std::nullopt;
    }
    /// @todo 在遍历时需检查对应列是否存在has_column
    auto dest_column_items_opt = build_dest_columns_string(dest_column_items);
    std::string dest_column_items_string = dest_column_items_opt.has_value() ? dest_column_items_opt.value() : "*";
    auto condition_string_opt = build_condition_string(condition_items);
    if (condition_string_opt.has_value())
    {
        return std::format("SELECT {} FROM {} WHERE {}", dest_column_items_string, m_table_info->table_name, condition_string_opt.value());
    }
    else
    {
        return std::format("SELECT {} FROM {}", dest_column_items_string, m_table_info->table_name);
    }
}
std::optional<std::string>  DaneJoe::SqlBuilder::build_update_string(
    const std::vector<SqlColumnItem>& dest_column_items,
    const std::vector<SqlConditionItem>& condition_items, bool enable_all)const
{
    if (!m_table_info->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build update string: invalid table info.");
        return std::nullopt;
    }
    std::string dest_columns_string;
    for (const auto& column_item : dest_column_items)
    {
        if (!m_table_info->has_column(column_item))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build update string: column {} does not exist in table {}.", column_item.column_name, m_table_info->table_name);
            continue;
        }
        if (!dest_columns_string.empty())
        {
            dest_columns_string += m_sql_stringify->get_sign_config().comma_sign;
            dest_columns_string += m_sql_stringify->get_sign_config().space_sign;
        }
        dest_columns_string += std::format("{} = {}", column_item.column_name, m_sql_stringify->get_sign_config().place_holder_sign);
    }
    if (dest_columns_string.empty())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build update string: no valid destination columns.");
        return std::nullopt;
    }
    auto condition_string_opt = build_condition_string(condition_items);
    if (condition_string_opt.has_value())
    {
        return std::format("UPDATE {} SET {} WHERE {}", m_table_info->table_name, dest_columns_string, condition_string_opt.value());
    }
    else
    {
        if (enable_all)
        {
            return std::format("UPDATE {} SET {}", m_table_info->table_name, dest_columns_string);
        }
    }
    return std::nullopt;
}

std::optional<std::string> DaneJoe::SqlBuilder::build_delete_string(const std::vector<SqlConditionItem>& condition_items, bool
    enable_all)const
{
    if (!m_table_info->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build delete string: invalid table info.");
        return std::nullopt;
    }
    auto condition_string_opt = build_condition_string(condition_items);
    if (condition_string_opt.has_value())
    {
        return std::format("DELETE FROM {} WHERE {}", m_table_info->table_name, condition_string_opt.value());
    }
    else
    {
        if (enable_all)
        {
            return std::format("DELETE FROM {}", m_table_info->table_name);
        }
    }
    return std::nullopt;
}

std::optional<std::string> DaneJoe::SqlBuilder::build_dest_columns_string(const std::vector<SqlColumnItem>& column_items)const
{
    if (!m_table_info->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build dest columns string: invalid table info.");
        return std::nullopt;
    }
    // 此处不能给*,*逻辑应该交由对应的build
    if (column_items.empty())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build dest columns string: no valid destination columns.");
        return std::nullopt;
    }
    std::string dest_columns_string;
    for (const auto& column : column_items)
    {
        if (!m_table_info->has_column(column))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlBuilder", "Failed to build dest columns string: column {} does not exist in table {}.", column.column_name, m_table_info->table_name);
            continue;
        }
        if (!dest_columns_string.empty())
        {
            dest_columns_string += m_sql_stringify->get_sign_config().comma_sign;
            dest_columns_string += m_sql_stringify->get_sign_config().space_sign;
        }
        dest_columns_string += column.column_name;
    }
    return dest_columns_string;
}
