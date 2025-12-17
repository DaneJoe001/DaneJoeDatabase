#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/common/core/data_type.hpp"

#include "danejoe/condition/range_condition.hpp"
#include "danejoe/database/sql_table_query.hpp"

DaneJoe::SqlTableQuery::SqlTableQuery(
    std::shared_ptr<ISqlStringify> stringify, const std::shared_ptr<const SqlTableItem> table_info, SqlQueryPtr query)
    : m_table_info(table_info), m_builder(table_info, stringify), m_query(query)
{}
bool DaneJoe::SqlTableQuery::create()
{
    if (!m_table_info)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to create table: Table info is empty.");
        return false;
    }
    if (!m_query)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to create table: Query is empty.");
        return false;
    }
    auto create_table_sql_opt = m_builder.build_create_table_string();
    if (!create_table_sql_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to create table: Failed to build create table string.");
        return false;
    }
    m_query->prepare(create_table_sql_opt.value());
    m_query->reset();
    return m_query->execute_command();
}
bool DaneJoe::SqlTableQuery::update(const std::vector<SqlCell>& cells, const std::vector<SqlConditionItem>& conditions)
{
    if (!m_table_info)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to update table: Table info is empty.");
        return false;
    }
    if (!m_query)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to update table: Query is empty.");
        return false;
    }
    std::vector<SqlColumnItem> dest_columns = get_dest_columns(cells);
    auto update_sql_opt = m_builder.build_update_string(dest_columns, conditions);
    if (!update_sql_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to update table: Failed to build update string.");
        return false;
    }
    m_query->prepare(update_sql_opt.value());
    m_query->reset();
    int param_index = 1;
    for (auto& cell : cells)
    {
        if (!m_table_info->has_column(cell.column_name))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to insert table: Failed to get column info.");
            continue;
        }
        m_query->bind(param_index++, cell);
    }
    for (auto& condition : conditions)
    {
        if (!condition.is_set)
        {
            continue;
        }
        if (!m_table_info->has_column(condition.column_info.column_name))
        {
            continue;
        }
        if (!condition.condition)
        {
            continue;
        }
        if (is_integer_type(condition.column_info.data_type))
        {
            auto range_condition = dynamic_pointer_cast<DaneJoe::IRangeCondition>(condition.condition);
            auto intervals = range_condition->get_integer_intervals();
            for (auto& interval : intervals)
            {
                auto left_ep = interval.get_left_endpoint();
                auto left_value_opt = left_ep.get_value();
                if (left_value_opt.has_value())
                {
                    m_query->bind(param_index++, left_value_opt.value());
                }
                auto right_ep = interval.get_right_endpoint();
                auto right_value_opt = right_ep.get_value();
                if (right_value_opt.has_value())
                {
                    m_query->bind(param_index++, right_value_opt.value());
                }
            }
        }
    }
    return m_query->execute_command();
}

bool DaneJoe::SqlTableQuery::insert(const std::vector<SqlCell>& cells)
{
    if (!m_table_info)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to insert table: Table info is empty.");
        return false;
    }
    if (!m_query)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to insert table: Query is empty.");
        return false;
    }
    std::vector<SqlColumnItem> dest_columns = get_dest_columns(cells);
    auto insert_sql_opt = m_builder.build_insert_string(dest_columns);
    if (!insert_sql_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to insert table: Failed to build insert string.");
        return false;
    }
    m_query->prepare(insert_sql_opt.value());
    m_query->reset();
    int param_index = 1;
    for (auto& cell : cells)
    {
        if (!m_table_info->has_column(cell.column_name))
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to insert table: Failed to get column info.");
            continue;
        }
        m_query->bind(param_index++, cell);
    }
    return m_query->execute_command();
}
bool DaneJoe::SqlTableQuery::remove(const std::vector<SqlConditionItem>& conditions)
{
    if (!m_table_info)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to remove table: Table info is empty.");
        return false;
    }
    if (!m_query)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to remove table: Query is empty.");
        return false;
    }
    auto delete_sql_opt = m_builder.build_delete_string(conditions);
    if (!delete_sql_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to remove table: Failed to build delete string.");
        return false;
    }
    m_query->prepare(delete_sql_opt.value());
    m_query->reset();
    int param_index = 1;
    for (auto& condition : conditions)
    {
        if (!condition.is_set)
        {
            continue;
        }
        if (!m_table_info->has_column(condition.column_info.column_name))
        {
            continue;
        }
        if (!condition.condition)
        {
            continue;
        }
        if (is_integer_type(condition.column_info.data_type))
        {
            auto range_condition = dynamic_pointer_cast<DaneJoe::IRangeCondition>(condition.condition);
            auto intervals = range_condition->get_integer_intervals();
            for (auto& interval : intervals)
            {
                auto left_ep = interval.get_left_endpoint();
                auto left_value_opt = left_ep.get_value();
                if (left_value_opt.has_value())
                {
                    m_query->bind(param_index++, left_value_opt.value());
                }
                auto right_ep = interval.get_right_endpoint();
                auto right_value_opt = right_ep.get_value();
                if (right_value_opt.has_value())
                {
                    m_query->bind(param_index++, right_value_opt.value());
                }
            }
        }
    }
    return m_query->execute_command();
}
std::vector<std::vector<DaneJoe::SqlCell>> DaneJoe::SqlTableQuery::select(const std::vector<SqlColumnItem>& dest_columns, const std::vector<SqlConditionItem>& conditions)
{
    if (!m_table_info)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to select table: Table info is empty.");
        return {};
    }
    if (!m_query)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to select table: Query is empty.");
        return {};
    }
    auto select_sql_opt = m_builder.build_select_string(dest_columns, conditions);
    if (!select_sql_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to select table: Failed to build select string.");
        return {};
    }
    m_query->prepare(select_sql_opt.value());
    m_query->reset();
    int param_index = 1;
    for (auto& condition : conditions)
    {
        if (!condition.is_set)
        {
            continue;
        }
        if (!m_table_info->has_column(condition.column_info.column_name))
        {
            continue;
        }
        if (!condition.condition)
        {
            continue;
        }
        if (is_integer_type(condition.column_info.data_type))
        {
            auto range_condition = dynamic_pointer_cast<DaneJoe::IRangeCondition>(condition.condition);
            auto intervals = range_condition->get_integer_intervals();
            for (auto& interval : intervals)
            {
                auto left_ep = interval.get_left_endpoint();
                auto left_value_opt = left_ep.get_value();
                if (left_value_opt.has_value())
                {
                    m_query->bind(param_index++, left_value_opt.value());
                }
                auto right_ep = interval.get_right_endpoint();
                auto right_value_opt = right_ep.get_value();
                if (right_value_opt.has_value())
                {
                    m_query->bind(param_index++, right_value_opt.value());
                }
            }
        }
    }
    return m_query->execute_query();
}

std::vector<DaneJoe::SqlColumnItem> DaneJoe::SqlTableQuery::get_dest_columns(const std::vector<SqlCell>& cells)const
{
    std::vector<SqlColumnItem> dest_columns;
    for (auto& cell : cells)
    {
        auto column_info = m_table_info->get_column_info(cell.column_name);
        if (!column_info.has_value())
        {
            DANEJOE_LOG_ERROR("default", "DaneJoe::SqlTableQuery", "Failed to update table: Failed to get column info.");
            continue;
        }
        dest_columns.push_back(column_info.value());
    }
    return dest_columns;
}
