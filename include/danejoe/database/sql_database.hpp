#pragma once

/**
 * @file sql_database.hpp
 * @brief 数据库接口
 * @author DaneJoe001
 * @version 0.1.1
 * @date 2025-10-24
 */

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <variant>

#include "danejoe/common/data_type.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 数据单元
     */
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
    /**
     * @struct SqlCell
     * @brief 数据单元
     */
    struct SqlCell
    {
        /// @brief 数据类型枚举
        DataType type;
        /// @brief 数据
        CellData data;
        /// @brief 列名
        std::string column_name;
    };
    /**
     * @struct SqlConfig
     * @brief 数据库配置
     */
    struct SqlConfig
    {
        /// @brief 数据库名称
        std::string database_name="default";
        /// @brief 数据库路径
        std::string path="./database/default.db";
        /// @brief 用户名
        std::string user_name;
        /// @brief 用户密码
        std::string user_password;
    };
    /**
     * @class ISqlDriver
     * @brief 数据库驱动接口
     */
    class ISqlDriver
    {
    public:
        /**
         * @brief 连接数据库
         * @param config 数据库配置
         * @return 是否成功
         */
        virtual bool connect(const SqlConfig& config)=0;
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return 是否成功
         */
        virtual bool prepare(const std::string& sql) = 0;
        /**
         * @brief 绑定SQL语句
         * @param sql SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return 是否成功
         */
        virtual bool bind(const std::string& sql, int index, const SqlCell& cell) = 0;
        /**
         * @brief 重置SQL语句
         * @param sql SQL语句
         */
        virtual void reset(const std::string& sql) = 0;
        /**
         * @brief 清除SQL语句绑定
         * @param sql SQL语句
         */
        virtual void clear_bindings(const std::string& sql) = 0;
        /**
         * @brief 执行查询
         * @param sql SQL语句
         * @return 查询结果
         */
        virtual std::vector<std::vector<SqlCell>> execute_query(const std::string& sql) = 0;
        /**
         * @brief 执行命令
         * @param sql SQL语句
         * @return 是否成功
         */
        virtual bool execute_command(const std::string& sql) = 0;
        /**
         * @brief 关闭数据库
         */
        virtual void close() = 0;
    };
    /**
     * @class SqlDatabase
     * @brief 数据库
     */
    class SqlDatabase
    {
    public:
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         */
        SqlDatabase(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 设置数据库配置
         * @param config 数据库配置
         */
        void set_config(const SqlConfig& config);
        /**
         * @brief 连接数据库
         * @return 是否成功
         */
        bool connect();
        /**
         * @brief 获取数据库驱动
         * @return 数据库驱动
         */
        std::shared_ptr<ISqlDriver> get_driver();
    private:
        /// @brief 数据库配置
        SqlConfig m_config;
        /// @brief 数据库驱动
        std::shared_ptr<ISqlDriver> m_driver;
    };
    /**
     * @class SqlQuery
     * @brief 查询
     */
    class SqlQuery
    {
    public:
        /**
         * @brief 构造函数
         * @param database 数据库
         */
        SqlQuery(std::shared_ptr<SqlDatabase> database);
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         */
        SqlQuery(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return 查询
         */
        SqlQuery& prepare(const std::string& sql);
        /**
         * @brief 绑定SQL语句
         * @tparam T 数据类型
         * @param index 参数索引
         * @param value 数据
         * @return 查询
         */
        template<class T>
        SqlQuery& bind(int index, T value)
        {
            SqlCell cell;
            cell.type = get_data_type<T>();
            cell.data = value;
            return bind(index, cell);
        }
        /**
         * @brief 绑定SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return 查询
         */
        SqlQuery& bind(int index, const SqlCell& cell);
        /**
         * @brief 清除SQL语句绑定
         */
        void clear_bindings();
        /**
         * @brief 重置SQL语句
         */
        void reset();
        /**
         * @brief 执行查询
         * @return 查询结果
         */
        std::vector<std::vector<SqlCell>> execute_query();
        /**
         * @brief 执行命令
         * @return 是否成功
         */
        bool execute_command();
    private:
        /// @brief SQL语句
        std::string m_sql;
        /// @brief 数据库驱动
        std::weak_ptr<ISqlDriver> m_driver;
    };
    /// @brief 查询指针(shared_ptr)
    using SqlQueryPtr = std::shared_ptr<SqlQuery>;
    /// @brief 数据库指针(shared_ptr)
    using SqlDatabasePtr = std::shared_ptr<SqlDatabase>;
    /// @brief 数据库驱动指针(shared_ptr)
    using ISqlDriverPtr = std::shared_ptr<ISqlDriver>;
}