#pragma once

#include "tx8/asm/assembler.h"
#include "tx8/core/cpu.h"
#include "tx8/core/log.h"
#include "tx8/core/stdlib.h"

#include <gtest/gtest.h>

class VMTest : public ::testing::Test {
  protected:
    tx_asm_Assembler as;
    tx_CPU           cpu;

    void SetUp() override;

    void TearDown() override;

    void run_and_compare(std::string code, std::string out, std::string err = "");

    void run_binary();

    bool run_code(std::string s);
};
