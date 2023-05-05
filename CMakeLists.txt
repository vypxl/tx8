cmake_minimum_required(VERSION 3.25)
include(FetchContent)

project(tx8 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_CXX_LINK_EXECUTABLE})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Wall -Wextra -Wno-unknown-pragmas -Werror)
endif()

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
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

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

# CLI11 (for command line parsing)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG main)

FetchContent_MakeAvailable(cli11)

# SDL2

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG release-2.26.5)

FetchContent_MakeAvailable(SDL2)
target_compile_options(SDL2-static PRIVATE -w)
target_compile_options(SDL2 PRIVATE -w)
target_compile_options(SDL2_test PRIVATE -w)

# tx8-core

add_library(
  tx8-core STATIC src/core/cpu.cpp src/core/gpu.cpp src/core/stdlib.cpp
                  src/core/log.cpp src/core/util.cpp)
target_include_directories(tx8-core PUBLIC include)
target_link_libraries(tx8-core PUBLIC fmt::fmt SDL2-static)

# tx8-asm

add_library(tx8-asm STATIC src/asm/assembler.cpp src/asm/lexer.cpp
                           src/asm/parser.cpp)
target_include_directories(tx8-asm PRIVATE)
target_link_libraries(tx8-asm PRIVATE tx8-core)

# tx8-cli

add_executable(tx8-cli src/cli/main.cpp)
target_include_directories(tx8-cli PRIVATE include)
target_link_libraries(tx8-cli PRIVATE tx8-core tx8-asm CLI11::CLI11 fmt::fmt)

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
  test/small_registers_test.cpp
  test/util_test.cpp)
target_include_directories(tx8-test PRIVATE)
target_link_libraries(tx8-test tx8-core tx8-asm gtest)

target_compile_options(gtest PRIVATE -Wno-error)

include(GoogleTest)
gtest_discover_tests(tx8-test)
add_test(NAME tx8-test COMMAND tx8-test)
