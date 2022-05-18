#pragma once

#include "tx8/asm/assembler.h"
#include "tx8/core/cpu.h"
#include "tx8/core/log.h"
#include "tx8/core/stdlib.h"

#include <gtest/gtest.h>
#include <vector>

typedef enum tx_NumType {
    TX_NUM_UINT32,
    TX_NUM_INT32,
    TX_NUM_FLOAT32
} tx_NumType;

typedef struct tx_num32_with_type {
    tx_NumType type;
    tx_num32 num;
} tx_num32_with_type;

class VMTest : public ::testing::Test {
  public:
    static void append_uint(tx_CPU* cpu, void* vm);
    static void append_int(tx_CPU* cpu, void* vm);
    static void append_float(tx_CPU* cpu, void* vm);

  private:
    static void append_num(void* vm, tx_uint32 value, tx_NumType type);

  protected:
    tx_asm_Assembler      as;
    tx_CPU                cpu;
    std::vector<tx_num32_with_type> nums;

    VMTest();
    ~VMTest() override;

    void SetUp() override;

    void TearDown() override;

    void run_and_compare_str(std::string code, const std::string out, const std::string err = "");
    void run_and_compare_num(std::string code, std::vector<std::variant<tx_uint32, tx_int32, tx_float32>> expected, std::string err = "");

    void run_binary();

    bool run_code(std::string s);
};
