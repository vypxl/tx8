#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(SmallRegisters, comparisons) {
    std::string s = R"EOF(
ld ab 5
ld bs 4
cmp ab bs
sys &test_r ; 1 (greater)

ld a 0x12345678
ld b 0xabcd5678
cmp as bs
sys &test_r ; 0 (equal)

ld a 0x4123
ld b 0x54123
cmp a bs
sys &test_r ; 0 (equal)

ld a 0x12
ld bb 0x12345612
ucmp a bb
sys &test_r ; 0 (equal)

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, 0u, 0u});
}

#pragma clang diagnostic pop
