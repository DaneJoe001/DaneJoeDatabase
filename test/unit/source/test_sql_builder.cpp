#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "danejoe/condition/text_condition.hpp"
#include "danejoe/condition/range_condition.hpp"
#include "danejoe/database/sql_builder.hpp"
#include "danejoe/database/sqlite_stringify.hpp"

namespace
{

std::shared_ptr<DaneJoe::SqlTableItem> make_test_table_info()
{
    auto table = std::make_shared<DaneJoe::SqlTableItem>();
    table->table_name = "users";
    table->is_unique = true;

    DaneJoe::SqlColumnItem id;
    id.column_index = 0;
    id.column_name = "id";
    id.data_type = DaneJoe::DataType::Int64;
    id.is_primary_key = true;
    id.is_not_null = true;
    id.is_unique = true;

    DaneJoe::SqlColumnItem age;
    age.column_index = 1;
    age.column_name = "age";
    age.data_type = DaneJoe::DataType::Int64;
    age.is_primary_key = false;
    age.is_not_null = true;

    table->column_items.push_back(id);
    table->column_items.push_back(age);

    return table;
}

TEST(SqlBuilderTest, BuildCreateTableString_BuildsExpectedSql)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    const auto sql_opt = builder.build_create_table_string();
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY NOT NULL UNIQUE, age INTEGER NOT NULL);");
}

TEST(SqlBuilderTest, BuildInsertString_BuildsExpectedSql)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    std::vector<DaneJoe::SqlColumnItem> dest_columns;
    dest_columns.push_back(table->column_items[0]);
    dest_columns.push_back(table->column_items[1]);

    const auto sql_opt = builder.build_insert_string(dest_columns);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "INSERT INTO users (id, age) VALUES (?, ?);");
}

TEST(SqlBuilderTest, BuildSelectString_WithCondition_BuildsExpectedSql)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    std::vector<DaneJoe::SqlColumnItem> dest_columns;
    dest_columns.push_back(table->column_items[1]);

    auto range_condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>();
    range_condition->add_range(
        DaneJoe::SingleInterval<int64_t>({ 10, false }, { 20, false }),
        DaneJoe::ConditionRelation::Or);

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info = table->column_items[1];
    item.condition = range_condition;

    const auto sql_opt = builder.build_select_string(dest_columns, { item });
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "SELECT age FROM users WHERE ( age >= ? AND age <= ? )");
}

TEST(SqlBuilderTest, BuildSelectString_TextConditionCurrentlyIgnored_DoesNotAddWhere)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    std::vector<DaneJoe::SqlColumnItem> dest_columns;
    dest_columns.push_back(table->column_items[1]);

    DaneJoe::SqlColumnItem name;
    name.column_index = 2;
    name.column_name = "name";
    name.data_type = DaneJoe::DataType::String;
    name.is_primary_key = false;
    name.is_not_null = true;

    auto extended_table = std::make_shared<DaneJoe::SqlTableItem>(*table);
    extended_table->column_items.push_back(name);

    DaneJoe::SqlBuilder extended_builder(extended_table, stringify);

    auto text_condition = std::make_shared<DaneJoe::TextCondition>();

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info = name;
    item.condition = text_condition;

    const auto sql_opt = extended_builder.build_select_string(dest_columns, { item });
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "SELECT age FROM users");
}

TEST(SqlBuilderTest, BuildUpdateString_EnableAllWithoutCondition_BuildsExpectedSql)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    std::vector<DaneJoe::SqlColumnItem> dest_columns;
    dest_columns.push_back(table->column_items[1]);

    const auto sql_opt = builder.build_update_string(dest_columns, {}, true);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "UPDATE users SET age = ?");
}

TEST(SqlBuilderTest, BuildDeleteString_EnableAllWithoutCondition_BuildsExpectedSql)
{
    auto table = make_test_table_info();
    auto stringify = std::make_shared<DaneJoe::SqliteStringify>();
    DaneJoe::SqlBuilder builder(table, stringify);

    const auto sql_opt = builder.build_delete_string({}, true);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "DELETE FROM users");
}

} // namespace
