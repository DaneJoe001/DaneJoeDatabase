#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/database/sql_query.hpp"

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
        ADD_DIAG_ERROR("database", "Prepare failed: driver expired");
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
        ADD_DIAG_ERROR("database", "Bind failed: driver expired");
        return *this;
    }
    m_driver.lock()->bind(m_sql, index, cell);
    return *this;
}

void DaneJoe::SqlQuery::clear_bindings()
{
    if (m_driver.expired())
    {
        ADD_DIAG_ERROR("database", "Clear bindings failed: driver expired");
        return;
    }
    m_driver.lock()->clear_bindings(m_sql);
}

void DaneJoe::SqlQuery::reset()
{
    if (m_driver.expired())
    {
        ADD_DIAG_ERROR("database", "Reset failed: driver expired");
        return;
    }
    m_driver.lock()->reset(m_sql);
}

std::vector<std::vector<DaneJoe::SqlCell>> DaneJoe::SqlQuery::execute_query()
{
    if (m_driver.expired())
    {
        ADD_DIAG_ERROR("database", "Execute query failed: driver expired");
        return std::vector<std::vector<DaneJoe::SqlCell>>();
    }
    return m_driver.lock()->execute_query(m_sql);
}

bool DaneJoe::SqlQuery::execute_command()
{
    if (m_driver.expired())
    {
        ADD_DIAG_ERROR("database", "Execute command failed: driver expired");
        return false;
    }
    return m_driver.lock()->execute_command(m_sql);
}
