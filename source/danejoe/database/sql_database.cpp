#include "danejoe/database/sql_database.hpp"
#include "danejoe/logger/logger_manager.hpp"

DaneJoe::SqlQuery::SqlQuery(std::shared_ptr<SqlDatabase> database)
{
    m_driver = database->get_driver();
}

DaneJoe::SqlQuery::SqlQuery(std::shared_ptr<ISqlDriver> driver)
{
    m_driver = driver;
}
DaneJoe::SqlQuery& DaneJoe::SqlQuery::prepare(const std::string& sql)
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "prepare sql failed: driver expired");
        return *this;
    }
    m_sql = sql;
    m_driver.lock()->prepare(m_sql);
    return *this;
}
DaneJoe::SqlQuery& DaneJoe::SqlQuery::bind(int index, const SqlCell& cell)
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "bind sql failed: driver expired");
        return *this;
    }
    m_driver.lock()->bind(m_sql, index, cell);
    return *this;
}

void DaneJoe::SqlQuery::clear_bindings()
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "clear_bindings sql failed: driver expired");
        return;
    }
    m_driver.lock()->clear_bindings(m_sql);
}

void DaneJoe::SqlQuery::reset()
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "reset sql failed: driver expired");
        return;
    }
    m_driver.lock()->reset(m_sql);
}

std::vector<std::vector<DaneJoe::SqlCell>> DaneJoe::SqlQuery::execute_query()
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "execute_query sql failed: driver expired");
        return std::vector<std::vector<DaneJoe::SqlCell>>();
    }
    return m_driver.lock()->execute_query(m_sql);
}

bool DaneJoe::SqlQuery::execute_command()
{
    if (m_driver.expired())
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlQuery", "execute_command sql failed: driver expired");
        return false;
    }
    return m_driver.lock()->execute_command(m_sql);
}

DaneJoe::SqlDatabase::SqlDatabase(std::shared_ptr<ISqlDriver> driver)
{
    m_driver = driver;
}
void DaneJoe::SqlDatabase::set_config(const SqlConfig& config)
{
    m_config = config;
}
bool DaneJoe::SqlDatabase::connect()
{
    if (!m_driver)
    {
        DANEJOE_LOG_ERROR("default", "DaneJoe::SqlDatabase", "connect sql failed: driver expired");
        return false;
    }
    return m_driver->connect(m_config);
}
std::shared_ptr<DaneJoe::ISqlDriver> DaneJoe::SqlDatabase::get_driver()
{
    return m_driver;
}