#include <gtest/gtest.h>

int tx_asm_yydebug;

int main(int argc, char** argv) {
    tx_asm_yydebug = 0;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
