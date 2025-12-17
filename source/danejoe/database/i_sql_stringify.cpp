#include "danejoe/database/i_sql_stringify.hpp"

void DaneJoe::ISqlStringify::set_sign_config(const SqlSignConfig& config)
{
    m_sign_config = config;
}

DaneJoe::SqlSignConfig DaneJoe::ISqlStringify::get_sign_config()
{
    return m_sign_config;
}
