cmake_minimum_required(VERSION 3.25)
include(FetchContent)

project(tx8 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_CXX_LINK_EXECUTABLE})

add_compile_options(-Wall -Wextra -Werror -Wno-unknown-pragmas -fmodules -fPIC)

# Choose build type
set(DEFAULT_BUILD_TYPE "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

find_library(MATH_LIBRARY m)

# Dependencies

# fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master)
FetchContent_MakeAvailable(fmt)

# Google test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(GTEST_FORCE_SHARED_CRT
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# tx8-core
add_library(tx8-core SHARED src/core/cpu.cpp src/core/debug.cpp
                            src/core/stdlib.cpp src/core/log.cpp)
target_include_directories(tx8-core PUBLIC include)
target_link_libraries(tx8-core PUBLIC ${MATH_LIBRARY} fmt::fmt)

# tx8-asm
add_library(
  tx8-asm SHARED
  src/asm/assembler.cpp ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_parser.cpp
  ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_lexer.cpp)
target_include_directories(tx8-asm PRIVATE ${CMAKE_BINARY_DIR}/tmp_lexyacc)
target_link_libraries(tx8-asm PRIVATE tx8-core)
# Needed because yacc generates code with warnings :(
target_compile_options(
  tx8-asm PRIVATE -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
                  -Wno-unused-but-set-variable)

# Custom commands for yacc / lex (actually bison/flex)
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_parser.cpp
         ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_parser.hpp
  COMMAND bison ARGS -o${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_parser.cpp --verbose
          -d ${CMAKE_SOURCE_DIR}/src/asm/tx8.y
  COMMENT "Generating parser"
  DEPENDS ${CMAKE_SOURCE_DIR}/src/asm/tx8.y)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_lexer.cpp
  COMMAND flex ARGS -o${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_lexer.cpp
          ${CMAKE_SOURCE_DIR}/src/asm/tx8.l
  COMMENT "Generating lexer"
  DEPENDS ${CMAKE_SOURCE_DIR}/src/asm/tx8.l
          ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8_parser.hpp)

# Tests
enable_testing()

add_executable(
  tx8-test
  test/main.cpp
  test/VMTest.cpp
  test/VMTest.hpp
  test/signed_arithmetic_test.cpp
  test/integration_test.cpp
  test/bitwise_operations_test.cpp
  test/float_arithmetic_test.cpp
  test/unsigned_arithmetic_test.cpp
  test/miscellaneous_test.cpp
  test/small_registers_test.cpp)
target_include_directories(tx8-test PRIVATE ${CMAKE_BINARY_DIR}/tmp_lexyacc)
target_link_libraries(tx8-test tx8-core tx8-asm gtest)
# target_compile_options(tx8-test PUBLIC ${CXX_OPTS})

target_compile_options(gtest PRIVATE -Wno-error)

include(GoogleTest)
gtest_discover_tests(tx8-test)
