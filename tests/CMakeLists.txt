cmake_minimum_required(VERSION 3.20)

# 统一结构：main 仅入口 + include 声明 + 与头同名实现源
add_executable(danejoe_database_demo
  "${CMAKE_CURRENT_LIST_DIR}/source/main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/source/demo_database.cpp"
)

target_link_libraries(danejoe_database_demo PRIVATE DaneJoe::Database)
target_include_directories(danejoe_database_demo PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_test(NAME database.demo COMMAND danejoe_database_demo)
