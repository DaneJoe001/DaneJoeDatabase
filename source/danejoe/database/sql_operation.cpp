#include "danejoe/database/sql_operation.hpp"

std::string DaneJoe::to_string(SqlOperation operation)
{
    switch (operation)
    {
    case SqlOperation::Select:
        return "SELECT";
    case SqlOperation::Insert:
        return "INSERT";
    case SqlOperation::Update:
        return "UPDATE";
    case SqlOperation::Delete:
        return "DELETE";
    default:
        return "UNKNOWN";
    }
}

DaneJoe::SqlOperation DaneJoe::to_sql_operation(const std::string& operation_string)
{
    if (operation_string == "SELECT")
    {
        return SqlOperation::Select;
    }
    else if (operation_string == "INSERT")
    {
        return SqlOperation::Insert;
    }
    else if (operation_string == "UPDATE")
    {
        return SqlOperation::Update;
    }
    else if (operation_string == "DELETE")
    {
        return SqlOperation::Delete;
    }
    else
    {
        return SqlOperation::Unknown;
    }
}

#ifdef SQL_EXAMPLE
std::string operation_string;
std::string condition_string;
std::string fields_string;
std::string table_name_string;
std::string placeholders_string;
std::vector<std::string> dest_field_vector;

/// @example 查询语句参考
/// @brief 插入操作（INSERT） INSERT INTO questions (question_text, question_type, difficulty, created_at, updated_at) VALUES (?, ?, ?, ?, ?); 
/// ------------------------- operation_sql INTO table_name (fields) VALUES (values);
std::string insert_sql_string =
operation_string +
"INTO" +
table_name_string +
"(" +
dest_field_vector[0] + "," +
dest_field_vector[1] + "," +
dest_field_vector[2] + "," +
dest_field_vector[3] + "," +
dest_field_vector[4] + "," +
dest_field_vector[5] +
")" +
"VALUES" +
"(" +
placeholders_string +
");";
/// @brief 更新操作（UPDATE） UPDATE questions SET question_text = ?, difficulty = ? WHERE question_id = ?; 
/// ------------------------- operation_sql table_name SET field1 = value1, field2 = value2 WHERE condition;
std::string update_sql_string =
operation_string +
table_name_string +
"SET" +
dest_field_vector[0] + " = ?, " +
dest_field_vector[1] + " = ? " +
"WHERE" +
condition_string + ";";
/// @brief 删除操作（DELETE） DELETE FROM questions WHERE question_id = ?; 
/// ------------------------- operation_sql FROM table_name WHERE condition;
std::string delete_sql_string = operation_string +
"FROM" +
table_name_string +
"WHERE" +
condition_string + ";";
/// @brief 查询所有问题 SELECT * FROM questions; 
/// ------------------------- operation_sql * FROM table_name;
std::string select_all_sql_string =
operation_string +
dest_field_vector[0] + "," +
dest_field_vector[1] + "," +
dest_field_vector[2] + "," +
dest_field_vector[3] + "," +
dest_field_vector[4] + "," +
dest_field_vector[5] +
"FROM" +
table_name_string + ";";
/// @brief 查询特定字段 SELECT question_id, question_text FROM questions; 
/// ------------------------- operation_sql fields FROM table_name;
std::string select_fields_sql_string =
operation_string +
dest_field_vector[0] + "," +
dest_field_vector[1] + "," +
dest_field_vector[2] + "," +
dest_field_vector[3] + "," +
dest_field_vector[4] + "," +
dest_field_vector[5] +
"FROM" +
table_name_string + ";";
/// @brief 根据 question_type 查询 SELECT * FROM questions WHERE question_type = ?; 
/// ------------------------- operation_sql * FROM table_name WHERE condition;
std::string select_by_question_type_sql_string = operation_string +
dest_field_vector[0] + "," +
dest_field_vector[1] + "," +
dest_field_vector[2] + "," +
dest_field_vector[3] + "," +
dest_field_vector[4] + "," +
dest_field_vector[5] +
"FROM" +
table_name_string + condition_string + ";";
/// @brief 根据 difficulty 查询 SELECT * FROM questions WHERE difficulty = ?; 
/// ------------------------- operation_sql * FROM table_name WHERE condition;
std::string select_by_difficulty_sql_string = operation_string + "* FROM" + table_name_string + condition_string + ";";
/// @brief 根据创建时间查询 SELECT * FROM questions WHERE created_at BETWEEN ? AND ?; 
/// ------------------------- operation_sql * FROM table_name WHERE condition;
std::string select_by_created_at_range_sql_string = operation_string + "* FROM" + table_name_string + condition_string + ";";
/// @brief 根据 question_id 范围查询 SELECT * FROM questions WHERE question_id BETWEEN ? AND ?; 
/// ------------------------- operation_sql * FROM table_name WHERE condition;
std::string select_by_question_id_range_sql_string = operation_string + "* FROM" + table_name_string + condition_string + ";";
/// @brief 结合多个条件查询 SELECT * FROM questions WHERE question_type = ? AND difficulty = ?; 
/// ------------------------- operation_sql * FROM table_name WHERE condition1 AND condition2;
std::string select_by_multiple_conditions_sql_string = operation_string + "* FROM" + table_name_string + condition_string + ";";
/// @brief 按 created_at 降序查询 SELECT * FROM questions ORDER BY created_at DESC; 
/// ------------------------- operation_sql * FROM table_name ORDER BY field DESC;
std::string select_order_by_created_at_desc_sql_string = operation_string + "* FROM" + table_name_string + "ORDER BY created_at DESC" + ";";
/// @brief 限制查询结果 SELECT * FROM questions WHERE question_type = ? LIMIT ?;
/// ------------------------- operation_sql * FROM table_name WHERE condition LIMIT number;
std::string select_with_limit_sql_string = operation_string + "* FROM" + table_name_string + condition_string + "LIMIT number" + ";";
#endif

