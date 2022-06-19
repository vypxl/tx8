#pragma once

#include "tx8/asm/assembler.h"
#include "tx8/core/cpu.h"
#include "tx8/core/log.h"
#include "tx8/core/stdlib.h"

#include <gtest/gtest.h>
#include <vector>

using tx_num32_variant = std::variant<tx_uint32, tx_int32, tx_float32>;

class VMTest : public ::testing::Test {
  public:
    static void test_uint(tx_CPU* cpu, void* vm);
    static void test_int(tx_CPU* cpu, void* vm);
    static void test_float(tx_CPU* cpu, void* vm);
    static void test_au(tx_CPU* cpu, void* vm);
    static void test_ai(tx_CPU* cpu, void* vm);
    static void test_af(tx_CPU* cpu, void* vm);
    static void test_r(tx_CPU* cpu, void* vm);

  private:
    static void append_num(void* vm, tx_num32_variant value);

  protected:
    tx_asm_Assembler      as = {};
    tx_CPU                cpu = {};
    std::vector<tx_num32_variant> nums;

    VMTest();
    ~VMTest() override;

    void SetUp() override;

    void TearDown() override;

    void run_and_compare_str(const std::string& code, const std::string& out, const std::string& err = "");
    void run_and_compare_num(const std::string& code, const std::vector<tx_num32_variant>& expected, const std::string& err = "");

    void run_binary();

    bool run_code(const std::string& s);
};

class Signed : public VMTest {};
class Bitwise : public VMTest {};
class Float : public VMTest {};
class Unsigned : public VMTest {};
class Miscellaneous : public VMTest {};
class Integration : public VMTest {};
