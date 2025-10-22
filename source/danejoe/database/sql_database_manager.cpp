#include "danejoe/database/sql_database_manager.hpp"
#include "danejoe/logger/logger_manager.hpp"

DaneJoe::SqlDatabaseManager::SqlDatabaseManager()
{
}
DaneJoe::SqlDatabaseManager::~SqlDatabaseManager() {}

DaneJoe::SqlDatabaseManager& DaneJoe::SqlDatabaseManager::get_instance()
{
    // 创建数据库管理器实例
    static SqlDatabaseManager instance;
    // 返回实例
    return instance;
}
std::shared_ptr<DaneJoe::SqlDatabase> DaneJoe:: SqlDatabaseManager::get_database(const std::string& database_name)
{
    // 查找数据库并获取迭代器
    auto database_it = m_databases.find(database_name);
    // 没找到则返回空值
    if (database_it == m_databases.end())
    {
        DANEJOE_LOG_TRACE("default", "DatabaseManager", "Failed to find database: {}", database_name);
        return nullptr;
    }
    return database_it->second;
}
void DaneJoe::SqlDatabaseManager::add_database(const std::string& database_name, std::shared_ptr<ISqlDriver> driver)
{
    // 先检查映射表中是否已存在该数据库
    auto database_it = m_databases.find(database_name);
    if (database_it != m_databases.end())
    {
        DANEJOE_LOG_INFO("default", "DatabaseManager", "Database {} already exisit!", database_name);
        return;
    }
    // 创建数据库
    m_databases[database_name] = std::make_shared<SqlDatabase>(driver);
}