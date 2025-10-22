#include <filesystem>
#include <string>
#include <cstring>
#include <cstdint>
#include <variant>

#include "danejoe/database/sqlite_driver.hpp"
#include "danejoe/logger/logger_manager.hpp"

namespace fs=std::filesystem;

DaneJoe::SqliteDriver::SqliteDriver() {}

DaneJoe::SqliteDriver::~SqliteDriver(){}

void DaneJoe::SqliteStmtDeleter::operator()(sqlite3_stmt* stmt)
{
    sqlite3_finalize(stmt);
    stmt = nullptr;
}

bool DaneJoe::SqliteDriver::connect(const SqlConfig& config)
{
    if (m_db != nullptr)
    {
        DANEJOE_LOG_WARN("default", "SqliteDriver", "database already connected");
        return true;
    }
    DANEJOE_LOG_TRACE("default", "SqliteDriver", "database path: {}",config.path);
    fs::path path = config.path;
    if (!fs::exists(path.parent_path()))
    {
        fs::create_directories(path.parent_path());
    }
    int rc = sqlite3_open(config.path.c_str(), &m_db);
    if (rc != SQLITE_OK)
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database connect failed: {}",sqlite3_errmsg(m_db));
        return false;
    }
    return true;
}

bool DaneJoe::SqliteDriver::prepare(const std::string& sql)
{
    if (m_db == nullptr)
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database not connected");
        return false;
    }
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it != m_stmts.end())
    {
        return true;
    }
    sqlite3_stmt* select_stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &select_stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database prepare failed: {}", std::string(sqlite3_errmsg(m_db)));
        return false;
    }
    m_stmts[sql] = SqliteStmtPtr(select_stmt);
    return true;
}

bool DaneJoe::SqliteDriver::bind(const std::string& sql,int index, const SqlCell& cell)
{
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it == m_stmts.end())
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database not prepared");
        return false;
    }
    int rc=SQLITE_OK;
    switch (cell.type)
    {
    case DataType::Int8:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, static_cast<int32_t>(std::get<int8_t>(cell.data)));
        break;
    case DataType::UInt8:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, static_cast<int32_t>(std::get<uint8_t>(cell.data)));
        break;
    case DataType::Int16:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, static_cast<int32_t>(std::get<int16_t>(cell.data)));
        break;
    case DataType::UInt16:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, static_cast<int32_t>(std::get<uint16_t>(cell.data)));
        break;
    case DataType::Int32:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, std::get<int32_t>(cell.data));
        break;
    case DataType::UInt32:
        rc = sqlite3_bind_int64(stmt_it->second.get(), index, static_cast<int64_t>(std::get<uint32_t>(cell.data)));
        break;
    case DataType::Int64:
        rc = sqlite3_bind_int64(stmt_it->second.get(), index, std::get<int64_t>(cell.data));
        break;
    case DataType::UInt64:
        rc = sqlite3_bind_int64(stmt_it->second.get(), index, static_cast<int64_t>(std::get<uint64_t>(cell.data)));
        break;
    case DataType::Float:
        rc = sqlite3_bind_double(stmt_it->second.get(), index, static_cast<double>(std::get<float>(cell.data)));
        break;
    case DataType::Double:
        rc = sqlite3_bind_double(stmt_it->second.get(), index, std::get<double>(cell.data));
        break;
    case DataType::Bool:
        rc = sqlite3_bind_int(stmt_it->second.get(), index, std::get<bool>(cell.data) ? 1 : 0);
        break;
    case DataType::String:
        rc = sqlite3_bind_text(stmt_it->second.get(), index, std::get<std::string>(cell.data).c_str(), -1, (void(*)(void*))SQLITE_TRANSIENT);
        break;
    case DataType::ByteArray:
    {
        const auto& byte_array = std::get<std::vector<uint8_t>>(cell.data);
        rc = sqlite3_bind_blob(stmt_it->second.get(), index, byte_array.data(), static_cast<int>(byte_array.size()), (void(*)(void*))SQLITE_TRANSIENT);
        break;
    }
    case DataType::Null:
        rc = sqlite3_bind_null(stmt_it->second.get(), index);
        break;
    case DataType::Object:
    case DataType::Map:
    case DataType::Array:
    case DataType::Dictionary:
    case DataType::Unknown:
    default:
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "Unsupported data type");
        return false;
    }
    if (rc != SQLITE_OK)
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database bind failed: {}",std::string(sqlite3_errmsg(m_db)));
        return false;
    }
    return true;
}

void DaneJoe::SqliteDriver::reset(const std::string& sql)
{
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it == m_stmts.end())
    {
        DANEJOE_LOG_WARN("default", "SqliteDriver", "database not prepared");
        return;
    }
    sqlite3_reset(stmt_it->second.get());
}

void DaneJoe::SqliteDriver::clear_bindings(const std::string& sql)
{
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it == m_stmts.end())
    {
        DANEJOE_LOG_WARN("default", "SqliteDriver", "database not prepared");
        return;
    }
    sqlite3_clear_bindings(stmt_it->second.get());
}

std::vector<std::vector<DaneJoe::SqlCell>> DaneJoe::SqliteDriver::execute_query(const std::string& sql)
{
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it == m_stmts.end())
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database not prepared");
        return {};
    }
    std::vector<std::vector<SqlCell>> result;
    int col_count = sqlite3_column_count(stmt_it->second.get());
    int rc = SQLITE_OK;
    do
    {
        std::vector<SqlCell> row(col_count);
        rc = sqlite3_step(stmt_it->second.get());
        if(rc!=SQLITE_ROW)
        {
            break;
        }
        for (int i = 0; i < col_count; i++)
        {
            row[i].column_name = sqlite3_column_name(stmt_it->second.get(), i);
            int col_type = sqlite3_column_type(stmt_it->second.get(), i);
            switch (col_type)
            {
                case SQLITE_INTEGER:
                    row[i].type = DataType::Int64;
                    row[i].data = static_cast<int64_t>(sqlite3_column_int64(stmt_it->second.get(), i));
                    break;
                case SQLITE_FLOAT:
                    row[i].type = DataType::Double;
                    row[i].data = sqlite3_column_double(stmt_it->second.get(), i);
                    break;
                case SQLITE_TEXT:
                    row[i].type = DataType::String;
                    row[i].data = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt_it->second.get(), i)));
                    break;
                case SQLITE_BLOB:
                    {
                        row[i].type = DataType::ByteArray;
                        const void* blob = sqlite3_column_blob(stmt_it->second.get(), i);
                        int byte_size = sqlite3_column_bytes(stmt_it->second.get(), i);
                        row[i].data = std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(blob), reinterpret_cast<const   uint8_t*>(blob) + byte_size);
                    }
                    break;
                case SQLITE_NULL:
                    row[i].type = DataType::Null;
                    row[i].data = nullptr;
                    break;
                default:
                    row[i].type = DataType::Unknown;
                    row[i].data = nullptr;
                    break;
            }
        }
        result.push_back(row);
    } while (rc == SQLITE_ROW);
    return result;
}

bool DaneJoe::SqliteDriver::execute_command(const std::string& sql)
{
    auto stmt_it = m_stmts.find(sql);
    if (stmt_it == m_stmts.end())
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database not prepared");
        return false;
    }
    int rc = sqlite3_step(stmt_it->second.get());
    if (rc != SQLITE_DONE)
    {
        DANEJOE_LOG_ERROR("default", "SqliteDriver", "database execute failed: {}",std::string(sqlite3_errmsg(m_db)));
        return false;
    }
    return true;
}

void DaneJoe::SqliteDriver::close()
{
    if (m_db != nullptr)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
    m_stmts.clear();
}
