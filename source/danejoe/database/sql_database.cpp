#include "danejoe/database/sql_database.hpp"
#include "danejoe/logger/logger_manager.hpp"

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
