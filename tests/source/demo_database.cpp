#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/database/sqlite_driver.hpp"
#include "danejoe/database/sql_database.hpp"
#include "demo_database.hpp"

namespace {
// 打印查询结果（含 DataType、列名、值）
static void print_result(const std::vector<std::vector<DaneJoe::SqlCell>>& result) {
    std::cout << "Result size:" << result.size() << std::endl;
    for (const auto& row : result) {
        for (const auto& cell : row) {
            std::cout << "cell type: " << DaneJoe::to_string(cell.type) << std::endl;
            if (!cell.column_name.empty()) {
                std::cout << "column: " << cell.column_name << std::endl;
            }
            switch (cell.type) {
                case DaneJoe::DataType::Int8:
                    std::cout << "(int8)  " << +static_cast<int8_t>(std::get<int8_t>(cell.data)) << std::endl;
                    break;
                case DaneJoe::DataType::UInt8:
                    std::cout << "(uint8) " << +static_cast<uint8_t>(std::get<uint8_t>(cell.data)) << std::endl;
                    break;
                case DaneJoe::DataType::Int16:
                    std::cout << "(int16) " << std::get<int16_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::UInt16:
                    std::cout << "(uint16) " << std::get<uint16_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::Int32:
                    std::cout << "(int32) " << std::get<int32_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::UInt32:
                    std::cout << "(uint32) " << std::get<uint32_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::Int64:
                    std::cout << "(int64) " << std::get<int64_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::UInt64:
                    std::cout << "(uint64) " << std::get<uint64_t>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::Float:
                    std::cout << "(float) " << std::get<float>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::Double:
                    std::cout << "(double) " << std::get<double>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::Bool:
                    std::cout << "(bool)  " << (std::get<bool>(cell.data) ? "true" : "false") << std::endl;
                    break;
                case DaneJoe::DataType::String:
                    std::cout << "(text)  " << std::get<std::string>(cell.data) << std::endl;
                    break;
                case DaneJoe::DataType::ByteArray: {
                    const auto& bytes = std::get<std::vector<uint8_t>>(cell.data);
                    std::cout << "(blob)[" << bytes.size() << "]: ";
                    std::cout.setf(std::ios::hex, std::ios::basefield);
                    for (size_t i = 0; i < std::min<size_t>(bytes.size(), 16); ++i) {
                        std::cout << (int)bytes[i] << " ";
                    }
                    std::cout.setf(std::ios::dec, std::ios::basefield);
                    std::cout << (bytes.size() > 16 ? "..." : "") << std::endl;
                    break;
                }
                case DaneJoe::DataType::Null:
                    std::cout << "(null)" << std::endl;
                    break;
                default:
                    std::cout << "(unknown)" << std::endl;
                    break;
            }
        }
        std::cout << "----------" << std::endl;
    }
}
}

namespace demo {

void run_database_demo()
{
    DANEJOE_LOG_TRACE("default", "main", "Hello, world");

    auto to_utf8 = [](const char8_t* p) -> std::string {
        return std::string(reinterpret_cast<const char*>(p));
    };

    auto driver = std::make_shared<DaneJoe::SqliteDriver>();
    DaneJoe::SqlConfig cfg;
    cfg.path = "./database/test.db";
    DaneJoe::SqlDatabase db(driver);
    db.set_config(cfg);

    if (!db.connect()) {
        DANEJOE_LOG_ERROR("default", "main", "connect() failed");
        return;
    }

    {
        auto q = std::make_shared<DaneJoe::SqlQuery>(driver);
        q->prepare("CREATE TABLE IF NOT EXISTS test ("
                   "id INTEGER PRIMARY KEY, "
                   "name TEXT, "
                   "age INTEGER, "
                   "note BLOB, "
                   "score REAL, "
                   "flag INTEGER, "
                   "extra TEXT)");
        if (!q->execute_command()) {
            DANEJOE_LOG_ERROR("default", "main", "create table failed");
            return;
        }
    }

    {
        auto q = std::make_shared<DaneJoe::SqlQuery>(driver);
        q->prepare("DELETE FROM test");
        if (!q->execute_command()) {
            DANEJOE_LOG_ERROR("default", "main", "delete old data failed");
            return;
        }
    }

    {
        auto ins = std::make_shared<DaneJoe::SqlQuery>(driver);
        ins->prepare("INSERT INTO test (name, age, note, score, flag, extra) VALUES (?, ?, ?, ?, ?, ?)");

        auto do_insert = [&](const std::string& name, int64_t age,
                             const std::vector<uint8_t>& note,
                             double score, bool flag, const std::string& extra) {
            ins->clear_bindings();
            ins->bind(1, name);
            ins->bind(2, age);
            ins->bind(3, note);
            ins->bind(4, score);
            ins->bind(5, flag);
            ins->bind(6, extra);
            if (!ins->execute_command()) {
                DANEJOE_LOG_ERROR("default", "main", "insert failed: {}", name);
            }
            ins->reset();
        };

        do_insert("ASCII_User", 18, std::vector<uint8_t>{0x01,0x02,0x03,0x04}, 95.5, true, "extra-info");
        do_insert(to_utf8(u8"中文用户"), 28, std::vector<uint8_t>{}, 88.0, false, to_utf8(u8"备注-含中文"));
        do_insert(to_utf8(u8"emoji🙂用户"), 30, std::vector<uint8_t>{0xFF,0xEE,0xDD,0xCC,0xBB}, 77.25, true, to_utf8(u8"表情🙂OK"));
        do_insert(to_utf8(u8"空值测试"), 22, std::vector<uint8_t>{}, 60.0, false, std::string{});

        auto ins_null = std::make_shared<DaneJoe::SqlQuery>(driver);
        ins_null->prepare("INSERT INTO test (name, age, note, score, flag, extra) VALUES (?, ?, ?, ?, ?, NULL)");
        ins_null->bind(1, to_utf8(u8"含NULL"));
        ins_null->bind(2, 40);
        ins_null->bind(3, std::vector<uint8_t>{0x00});
        ins_null->bind(4, 0.0);
        ins_null->bind(5, false);
        (void)ins_null->execute_command();
    }

    {
        auto q = std::make_shared<DaneJoe::SqlQuery>(driver);
        q->prepare("SELECT id, name, age, note, score, flag, extra FROM test ORDER BY id ASC");
        auto res = q->execute_query();
        print_result(res);
    }

    {
        auto bad = std::make_shared<DaneJoe::SqlQuery>(driver);
        bad->prepare("SELEC bad FROM test");
        (void)bad->execute_query();
    }
}

} // namespace demo
