#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(SmallRegisters, sign_extend) {
    std::string s = R"EOF(
ld ab 1
lws a ab
sys &test_ai ; 1

ld ab -1
lws a ab
sys &test_ai ; -1

ld as 1
lws a as
sys &test_ai ; 1

ld as -1
lws a as
sys &test_ai ; -1

)EOF";
    run_and_compare_num(s, {1, -1, 1, -1});
}

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

ld a -1
ld b 0
cmp as b
sys &test_r ; -1 (smaller)

ld a 0x12
ld bb 0x12345612
ucmp a bb
sys &test_r ; 0 (equal)

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, 0u, -1u, 0u});
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

lda 2
ldb -3
add a bb
sys &test_ai ; -1
sys &test_r ; 0

hlt
)EOF";
    run_and_compare_num(s, {0x1u, 0b01u, 0x81u, 0b10u, 0x1u, 0b01u, 0x8001u, 0b10u, -1, 0u});
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

lda -2
ldb -3
sub ab bb
sys &test_au ; 0xffffff01
sys &test_r ; 0

hlt
)EOF";
    run_and_compare_num(s, {0xfeu, 0b01u, 0x7eu, 0b10u, 0xfffeu, 0b01u, 0x7ffeu, 0b10u, 0xffffff01u, 0u});
}

TEST_F(SmallRegisters, mul) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
mul ab bs
sys &test_au ; 0x44
sys &test_r ; 0

lda 0x1234
ldb 0x55
mul as bb
sys &test_au ; 0x0b44
sys &test_r ; 0

lda -11890
ldb 23
mul as bb
sys &test_au ; 0xffffd3c2 (the first 16 bits are 0xffff because they retain their previous value)
sys &test_r ; 0xffffffff (result is negative)

lda -11890
ldb 23
mul ab bs
sys &test_au ; 0xffffd1c2 (the first 24 bits are 0xffffd1 because they retain their previous value)
sys &test_r ; 0xffffffff
ld a ab
sys &test_au ; 0xc2 (now the upper bits were set to 0)
hlt
)EOF";
    run_and_compare_num(s, {0x44u, 0u, 0x0b44u, 0u, 0xffffd3c2u, 0xffffffffu, 0xffffd1c2u, 0xffffffffu, 0xc2u});
}

TEST_F(SmallRegisters, div) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
div ab bs
sys &test_au ; 0x0
sys &test_r ; 0x55

lda 0x1234
ldb 0x55
div as bb
sys &test_au ; 0x36
sys &test_r ; 0x46

lda -11890
ldb 23
div as bb
sys &test_au ; 0xfffffdfc
sys &test_r ; 0xffffffea

)EOF";
    run_and_compare_num(s, {0x0u, 0x55u, 0x36u, 0x46u, 0xfffffdfcu, 0xffffffea});
}

#pragma clang diagnostic pop
