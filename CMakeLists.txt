cmake_minimum_required(VERSION 3.17)

project(tx8 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
SET(CMAKE_C_LINK_EXECUTABLE ${CMAKE_CXX_LINK_EXECUTABLE})

# Choose build type
set(default_build_type "Debug")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()


find_library(MATH_LIBRARY m)

add_library(tx8-core SHARED
        deps/kstring.c
        src/core/cpu.cpp
        src/core/debug.cpp
        src/core/stdlib.cpp
        src/core/log.cpp
        )
target_compile_options(tx8-core PRIVATE -Wall -Wextra -Werror)
target_include_directories(tx8-core PUBLIC include)
target_include_directories(tx8-core PRIVATE deps)
target_link_libraries(tx8-core PUBLIC ${MATH_LIBRARY})

add_library(tx8-asm SHARED
        src/asm/types.cpp
        src/asm/assembler.cpp
        ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.tab.c
        ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.yy.c)
target_compile_options(tx8-asm PRIVATE -Wall -Wextra -Werror)
target_include_directories(tx8-asm PRIVATE deps)
target_link_libraries(tx8-asm PRIVATE tx8-core)

# Custom commands for yacc / lex (actually bison/flex)
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.tab.c
        ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.tab.h
        COMMAND bison
        ARGS -o${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.tab.c --verbose -d
        ${CMAKE_SOURCE_DIR}/src/asm/tx8.y
        BYPRODUCTS ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.output
        DEPENDS ${CMAKE_SOURCE_DIR}/src/asm/tx8.y
)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.yy.c
        COMMAND flex
        ARGS -o${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.yy.c
        ${CMAKE_SOURCE_DIR}/src/asm/tx8.l
        DEPENDS ${CMAKE_SOURCE_DIR}/src/asm/tx8.l ${CMAKE_BINARY_DIR}/tmp_lexyacc/tx8.tab.h
)

# Google test
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tx8-test test/main.cpp test/VMTest.cpp test/VMTest.hpp test/signed_arithmetic_test.cpp test/integration_test.cpp test/bitwise_operations_test.cpp test/float_arithmetic_test.cpp test/unsigned_arithmetic_test.cpp test/miscellaneous_test.cpp test/small_registers_test.cpp)
target_include_directories(tx8-test PRIVATE deps)
target_link_libraries(tx8-test tx8-core tx8-asm gtest)

include(GoogleTest)
gtest_discover_tests(tx8-test)
