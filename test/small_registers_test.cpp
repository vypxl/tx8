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

TEST_F(SmallRegisters, inc) {
    std::string s = R"EOF(
lda 0xff
inc ab
sys &test_au ; 0x0
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7f
inc ab
sys &test_au ; 0x80
sys &test_r ; 0b10 (signed overflow)

lda 0xffff
inc as
sys &test_au ; 0x0
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7fff
inc as
sys &test_au ; 0x8000
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {0x0u, 0b01u, 0x80u, 0b10u, 0x0u, 0b01u, 0x8000u, 0b10u});
}

TEST_F(SmallRegisters, dec) {
    std::string s = R"EOF(
lda 0
dec ab
sys &test_au ; 0xff
sys &test_r ; 0b01 (unsigned overflow)

lda 0
dec as
sys &test_au ; 0xffff
sys &test_r ; 0b01 (unsigned overflow)

lda 0x80
dec ab
sys &test_au ; 0x7f
sys &test_r ; 0b10 (signed overflow)

lda 0x8000
dec as
sys &test_au ; 0x7fff
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {0xffu, 0b01u, 0xffffu, 0b01u, 0x7fu, 0b10u, 0x7fffu, 0b10u});
}

TEST_F(SmallRegisters, add) {
    std::string s = R"EOF(
lda 0xfe
add ab 3
sys &test_au ; 0x1
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7e
add ab 3
sys &test_au ; 0x81
sys &test_r ; 0b10 (signed overflow)

lda 0xfffe
add as 3
sys &test_au ; 0x1
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7ffe
add as 3
sys &test_au ; 0x8001
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {0x1u, 0b01u, 0x81u, 0b10u, 0x1u, 0b01u, 0x8001u, 0b10u});
}

TEST_F(SmallRegisters, sub) {
    std::string s = R"EOF(
lda 0x1
sub ab 3
sys &test_au ; 0xfe
sys &test_r ; 0b01 (unsigned overflow)

lda 0x81
sub ab 3
sys &test_au ; 0x7e
sys &test_r ; 0b10 (signed overflow)

lda 0x1
sub as 3
sys &test_au ; 0xfffe
sys &test_r ; 0b01 (unsigned overflow)

lda 0x8001
sub as 3
sys &test_au ; 0x7ffe
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {0xfeu, 0b01u, 0x7eu, 0b10u, 0xfffeu, 0b01u, 0x7ffeu, 0b10u});
}

TEST_F(SmallRegisters, mul) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
mul ab bs
sys &test_au ; 0x44
sys &test_r ; 0x11

lda 0x1234
ldb 0x55
mul as bb
sys &test_au ; 0x0b44
sys &test_r ; 0x6

hlt
)EOF";
    run_and_compare_num(s, {0x44u, 0x11u, 0x0b44u, 0x6u});
}

#pragma clang diagnostic pop
