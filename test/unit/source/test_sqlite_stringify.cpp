#include <gtest/gtest.h>

#include <memory>

#include "danejoe/condition/text_condition.hpp"
#include "danejoe/condition/range_condition.hpp"
#include "danejoe/database/sqlite_stringify.hpp"

namespace
{

TEST(SqliteStringifyTest, TryFormatDataType_MapsToSqliteTypes)
{
    DaneJoe::SqliteStringify stringify;

    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Int32), std::optional<std::string>("INTEGER"));
    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::UInt64), std::optional<std::string>("INTEGER"));
    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Bool), std::optional<std::string>("INTEGER"));

    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Float), std::optional<std::string>("REAL"));
    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Double), std::optional<std::string>("REAL"));

    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::String), std::optional<std::string>("TEXT"));
    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::ByteArray), std::optional<std::string>("BLOB"));
    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Null), std::optional<std::string>("NULL"));

    EXPECT_EQ(stringify.try_format_data_type(DaneJoe::DataType::Unknown), std::nullopt);
}

TEST(SqliteStringifyTest, TryFormatCondition_IntegerRange_BuildsExpectedSql)
{
    DaneJoe::SqliteStringify stringify;

    auto range_condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>();
    range_condition->add_range(
        DaneJoe::SingleInterval<int64_t>({ 10, false }, { 20, false }),
        DaneJoe::ConditionRelation::Or);

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info.column_index = 0;
    item.column_info.column_name = "age";
    item.column_info.data_type = DaneJoe::DataType::Int64;
    item.condition = range_condition;

    const auto sql_opt = stringify.try_format_condition(item);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "( age >= ? AND age <= ? )");
}

TEST(SqliteStringifyTest, TryFormatCondition_IntegerRange_SupportsSingleSidedInterval)
{
    DaneJoe::SqliteStringify stringify;

    auto range_condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>();
    range_condition->add_range(
        DaneJoe::SingleInterval<int64_t>({ 5, false }, { std::nullopt, true }),
        DaneJoe::ConditionRelation::Or);

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info.column_index = 0;
    item.column_info.column_name = "age";
    item.column_info.data_type = DaneJoe::DataType::Int64;
    item.condition = range_condition;

    const auto sql_opt = stringify.try_format_condition(item);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "( age >= ? )");
}

TEST(SqliteStringifyTest, TryFormatCondition_IntegerRange_MultipleIntervalsJoinedByOr)
{
    DaneJoe::SqliteStringify stringify;

    auto range_condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>();
    range_condition->add_range(
        DaneJoe::SingleInterval<int64_t>({ 1, false }, { 2, false }),
        DaneJoe::ConditionRelation::Or);
    range_condition->add_range(
        DaneJoe::SingleInterval<int64_t>({ 10, false }, { 20, false }),
        DaneJoe::ConditionRelation::Or);

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info.column_index = 0;
    item.column_info.column_name = "age";
    item.column_info.data_type = DaneJoe::DataType::Int64;
    item.condition = range_condition;

    const auto sql_opt = stringify.try_format_condition(item);
    ASSERT_TRUE(sql_opt.has_value());
    EXPECT_EQ(sql_opt.value(), "( age >= ? AND age <= ? ) OR ( age >= ? AND age <= ? )");
}

TEST(SqliteStringifyTest, TryFormatCondition_TextCondition_CurrentlyReturnsNullopt)
{
    DaneJoe::SqliteStringify stringify;

    auto text_condition = std::make_shared<DaneJoe::TextCondition>();

    DaneJoe::SqlConditionItem item;
    item.is_set = true;
    item.column_info.column_index = 0;
    item.column_info.column_name = "name";
    item.column_info.data_type = DaneJoe::DataType::String;
    item.condition = text_condition;

    const auto sql_opt = stringify.try_format_condition(item);
    EXPECT_FALSE(sql_opt.has_value());
}

} // namespace
