#pragma once

#include "tx8/asm/assembler.hpp"
#include "tx8/core/cpu.hpp"
#include "tx8/core/log.hpp"
#include "tx8/core/stdlib.hpp"

#include <gtest/gtest.h>
#include <vector>


class VMTest : public ::testing::Test { // NOLINT
  private:
    void append_num(tx::num32_variant value);

  protected:
    std::vector<tx::num32_variant> nums;

    VMTest();
    ~VMTest() override;

    void SetUp() override;

    void TearDown() override;

    void run_and_compare_str(const std::string& code, const std::string& out, const std::string& err = "");
    void run_and_compare_num(
        const std::string&                    code,
        const std::vector<tx::num32_variant>& expected,
        const std::string&                    err = ""
    );

    bool run_code(const std::string& s);

    void use_testing_stdlib(tx::CPU& cpu);
};

class Signed : public VMTest { };
class Bitwise : public VMTest { };
class Float : public VMTest { };
class Unsigned : public VMTest { };
class Miscellaneous : public VMTest { };
class Integration : public VMTest { };
class SmallRegisters : public VMTest { };
