#pragma once

#include "tx8/asm/assembler.h"
#include "tx8/core/cpu.h"
#include "tx8/core/log.h"
#include "tx8/core/stdlib.h"

#include <gtest/gtest.h>
#include <vector>

class VMTest : public ::testing::Test {
  public:
    static void append_float(tx_CPU* cpu, void* vm);

  protected:
    tx_asm_Assembler        as;
    tx_CPU                  cpu;
    std::vector<tx_float32> floats;

    VMTest();
    ~VMTest() override;

    void SetUp() override;

    void TearDown() override;

    void run_and_compare(std::string code, std::string out, std::string err = "");
    void run_and_compare_f(std::string code, std::vector<tx_float32> out, std::string err = "");

    void run_binary();

    bool run_code(std::string s);
};
