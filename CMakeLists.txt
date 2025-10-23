cmake_minimum_required(VERSION 3.20)
project(DaneJoeDatabase VERSION 0.1.1 LANGUAGES CXX)
option(DANEJOE_DATABASE_BUILD_TESTS "Build tests for DaneJoeDatabase" ON)

# @brief 依赖发现：优先通过 find_package 消费 Logger
# @note 若同一工程已添加 Logger 子目录，则后续通过 TARGET 分支链接
find_package(SQLite3 REQUIRED)
option(DANEJOE_ALLOW_FETCH "Allow fetching DaneJoe deps from remote if not found locally" OFF)

# Prefer already-defined targets when building in-tree
if(NOT TARGET DaneJoe::Logger)
  find_package(DaneJoeLogger CONFIG QUIET)
  if(NOT DaneJoeLogger_FOUND)
    set(_LOGGER_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/../logger")
    if(EXISTS "${_LOGGER_SRC_DIR}/CMakeLists.txt")
      add_subdirectory("${_LOGGER_SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/logger-build")
    elseif(DANEJOE_ALLOW_FETCH)
      include(FetchContent)
      FetchContent_Declare(
        DaneJoeLogger
        GIT_REPOSITORY https://github.com/DaneJoe001/DaneJoeLogger.git
        GIT_TAG v0.1.1
      )
      FetchContent_MakeAvailable(DaneJoeLogger)
    endif()
  endif()
endif()

if(NOT TARGET DaneJoe::Commons)
  find_package(DaneJoeCommon CONFIG QUIET)
  if(NOT DaneJoeCommon_FOUND)
    set(_COMMON_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/../common")
    if(EXISTS "${_COMMON_SRC_DIR}/CMakeLists.txt")
      add_subdirectory("${_COMMON_SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/common-build")
    elseif(DANEJOE_ALLOW_FETCH)
      include(FetchContent)
      FetchContent_Declare(
        DaneJoeCommon
        GIT_REPOSITORY https://github.com/DaneJoe001/DaneJoeCommon.git
        GIT_TAG v0.1.1
      )
      FetchContent_MakeAvailable(DaneJoeCommon)
    endif()
  endif()
endif()

# @brief 定义数据库抽象库目标
# @note 对外通过命名空间别名 `DaneJoe::Database` 暴露
add_library(DaneJoeDatabase
  "source/danejoe/database/sql_database_manager.cpp"
  "source/danejoe/database/sql_database.cpp"
  "source/danejoe/database/sqlite_driver.cpp"
)
add_library(DaneJoe::Database ALIAS DaneJoeDatabase)

# @brief 公开头文件目录
target_include_directories(DaneJoeDatabase
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# @brief 以目标特性声明 C++17
target_compile_features(DaneJoeDatabase PUBLIC cxx_std_20)

if(NOT TARGET DaneJoe::Logger)
  message(FATAL_ERROR "DaneJoe::Logger target not found. Provide installed package, local ../logger, or enable DANEJOE_ALLOW_FETCH.")
endif()
target_link_libraries(DaneJoeDatabase PUBLIC DaneJoe::Logger)

if(NOT TARGET DaneJoe::Commons)
  message(FATAL_ERROR "DaneJoe::Commons target not found. Provide installed package, local ../common, or enable DANEJOE_ALLOW_FETCH.")
endif()
target_link_libraries(DaneJoeDatabase PUBLIC DaneJoe::Commons)

# Ensure logger sees Commons headers when building in-tree
if(TARGET DaneJoeLogger)
  target_link_libraries(DaneJoeLogger PUBLIC DaneJoe::Commons)
endif()

# @brief 链接 SQLite3
target_link_libraries(DaneJoeDatabase PUBLIC SQLite::SQLite3)

# @brief 安装目标与头文件
include(GNUInstallDirs)
install(TARGETS DaneJoeDatabase
  EXPORT DaneJoeDatabaseTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# 生成版本头并安装
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/danejoe/version")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/danejoe/version/database_version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/danejoe/version/database_version.h"
  @ONLY
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/include/danejoe/version/database_version.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/danejoe/version")

# @brief 生成并安装 CMake 包配置（Config + Version + Targets）
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/DaneJoeDatabaseConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "cmake/DaneJoeDatabaseConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/DaneJoeDatabaseConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DaneJoeDatabase"
)

install(EXPORT DaneJoeDatabaseTargets
  NAMESPACE DaneJoe::
  FILE DaneJoeDatabaseTargets.cmake
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DaneJoeDatabase"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/DaneJoeDatabaseConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/DaneJoeDatabaseConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DaneJoeDatabase"
)

if(DANEJOE_DATABASE_BUILD_TESTS)
  include(CTest)
  enable_testing()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
  endif()
endif()
