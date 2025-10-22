#pragma once

/**
 * @file sql_database.hpp
 * @brief 数据库接口
 * @author DaneJoe001
 */

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <variant>

#include "danejoe/common/data_type.hpp"

namespace DaneJoe
{
    using CellData = ::std::variant<
        int8_t,
        uint8_t,
        int16_t,
        uint16_t,
        int32_t,
        uint32_t,
        int64_t,
        uint64_t,
        float,
        double,
        bool,
        ::std::string,
        ::std::vector<uint8_t>,
        ::std::nullptr_t
    >;
    struct SqlCell
    {
        DataType type;
        CellData data;
        std::string column_name;
    };

    struct SqlConfig
    {
        /// @brief 数据库名称。
        std::string database_name="default";
        /// @brief 数据库路径。
        std::string path="./database/default.db";
        /// @brief 用户名。
        std::string user_name;
        /// @brief 用户密码。
        std::string user_password;
    };

    class ISqlDriver
    {
    public:
        virtual bool connect(const SqlConfig& config)=0;
        virtual bool prepare(const std::string& sql) = 0;
        virtual bool bind(const std::string& sql, int index, const SqlCell& cell) = 0;
        virtual void reset(const std::string& sql) = 0;
        virtual void clear_bindings(const std::string& sql) = 0;
        virtual std::vector<std::vector<SqlCell>> execute_query(const std::string& sql) = 0;
        virtual bool execute_command(const std::string& sql) = 0;
        virtual void close() = 0;
    };

    class SqlDatabase
    {
    public:
        SqlDatabase(std::shared_ptr<ISqlDriver> driver);
        void set_config(const SqlConfig& config);
        bool connect();
        std::shared_ptr<ISqlDriver> get_driver();
    private:
        SqlConfig m_config;
        std::shared_ptr<ISqlDriver> m_driver;
    };

    class SqlQuery
    {
    public:
        SqlQuery(std::shared_ptr<SqlDatabase> database);
        SqlQuery(std::shared_ptr<ISqlDriver> driver);
        SqlQuery& prepare(const std::string& sql);
        template<class T>
        SqlQuery& bind(int index, T value)
        {
            SqlCell cell;
            cell.type = get_data_type<T>();
            cell.data = value;
            return bind(index, cell);
        }
        SqlQuery& bind(int index, const SqlCell& cell);
        void clear_bindings();
        void reset();
        std::vector<std::vector<SqlCell>> execute_query();
        bool execute_command();
    private:
        std::string m_sql;
        std::weak_ptr<ISqlDriver> m_driver;
    };


}