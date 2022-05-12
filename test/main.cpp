#include <stdio.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <tx8/asm/assembler.h>
#include <tx8/asm/types.h>
#include "tx8/core/log.h"
#include <tx8/core/cpu.h>
#include <tx8/core/instruction.h>
#include <tx8/core/stdlib.h>
#include <tx8/core/types.h>

int tx_asm_yydebug;

class VMTest : public ::testing::Test {
  protected:
    tx_asm_Assembler as;
    tx_CPU cpu;

    VMTest() {

    }

    ~VMTest() override {

    }

    void SetUp() override {
        tx_asm_init_assembler(&as);
        tx_log_init_str();
        tx_log_init_str_err();
        tx_log_init_stdout();
    }

    void TearDown() override {
        tx_asm_destroy_assembler(&as);
        tx_destroy_cpu(&cpu);
        tx_log_clear_str();
        tx_log_clear_str_err();
    }

    void run_binary() {
        tx_uint32 rom_size;
        tx_uint8* rom      = tx_asm_assembler_generate_binary(&as, &rom_size);

        tx_init_cpu(&cpu, rom, rom_size);
        tx_cpu_use_stdlib(&cpu);
        cpu.debug = tx_asm_yydebug;
        free(rom);

        tx_run_cpu(&cpu);
    }

    void run_code(std::string s) {
        ASSERT_EQ(tx_asm_run_assembler_buffer(&as, (char*) s.c_str(), s.length()), 0);
        if (tx_asm_yydebug) {
            printf("labels:\n");
            tx_asm_assembler_print_labels(&as);

            printf("\ninstructions:\n");
            tx_asm_assembler_print_instructions(&as);
        }

        run_binary();
    }
};

TEST_F(VMTest, HelloWorld) {
    std::string s = R"EOF(
mov o 0xc03000

mov $0 0x48
mov $1 0x65
mov $2 0x6c
mov $3 0x6c
mov $4 0x6f
mov $5 0x20
mov $6 0x77
mov $7 0x6f
mov $8 0x72
mov $9 0x6c
mov $a 0x64
mov $b 0x21
mov $c 0

psh o
sys &println

hlt
)EOF";

    run_code(s);

    EXPECT_STREQ(tx_log_get_str(), "Hello world!\n");
    ASSERT_STREQ(tx_log_get_str_err(), nullptr);
}

// The fixture for testing class Foo.
class AsmTest : public ::testing::Test {
  protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    AsmTest() {
        // You can do set-up work for each test here.
    }

    ~AsmTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.
};

int main(int argc, char **argv) {
    tx_asm_yydebug = 0;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
