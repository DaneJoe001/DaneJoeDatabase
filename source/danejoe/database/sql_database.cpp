#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/database/sql_database.hpp"

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
        ADD_DIAG_ERROR("database", "Failed to connect database: driver is null");
        return false;
    }
    return m_driver->connect(m_config);
}
std::shared_ptr<DaneJoe::ISqlDriver> DaneJoe::SqlDatabase::get_driver()
{
    return m_driver;
}
