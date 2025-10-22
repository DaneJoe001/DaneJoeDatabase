# DaneJoe Database

数据库抽象层（目前内置 SQLite3 驱动），依赖 `DaneJoe::Logger` 与 `DaneJoe::Commons`。

## 构建
```bash
cmake -S . -B build --preset gcc-debug -DBUILD_TESTING=ON
cmake --build build
```

## 运行示例/测试
```bash
ctest --test-dir build -V -R database\.demo
# 或直接运行
./build/library/database/tests/danejoe_database_demo
```

## 作为依赖使用
CMake:
```cmake
find_package(DaneJoeDatabase CONFIG REQUIRED)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE DaneJoe::Database)
```
