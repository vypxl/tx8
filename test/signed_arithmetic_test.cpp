#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(Signed, inc) {
    std::string s = R"EOF(
lda 0
inc a
sys &test_au ; 1
sys &test_r ; 0 (no overflow)

lda -1
inc a
sys &test_au ; 0

lda 2147483647
inc a
sys &test_ai ; -2147483648
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, 0u, INT32_MIN, 0b10u});
}

TEST_F(Signed, dec) {
    std::string s = R"EOF(
lda 0
dec a
sys &test_ai ; -1

lda -1
dec a
sys &test_ai ; -2
sys &test_r ; 0 (no overflow)

lda -2147483648
dec a
sys &test_ai ; 2147483647
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {-1, -2, 0u, INT32_MAX, 0b10u});
}

TEST_F(Signed, add) {
    std::string s = R"EOF(
lda 2
add a 3
sys &test_au ; 5

lda 0
add a 0
sys &test_au ; 0
sys &test_r ; 0 (no overflow)

lda 0xffffffff
add a 1
sys &test_au ; 0

lda 0xffffffff
add a 0xffffffff
sys &test_au ; 0xfffffffe

lda 4
add a -2
sys &test_ai ; 2

lda -1
add a 3
sys &test_ai ; 2

lda -5
add a 3
sys &test_ai ; -2

lda 5
add a -8
sys &test_ai ; -3

lda -2
add a -7
sys &test_ai ; -9

lda 0x7fffffff
add a 5
sys &test_r; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {5u, 0u, 0u, 0u, 0xfffffffeu, 2, 2, -2, -3, -9, 0b10u});
}

TEST_F(Signed, sub) {
    std::string s = R"EOF(
lda 2
sub a 3
sys &test_ai ; -1

lda 32
sub a 1
sys &test_au ; 1f
sys &test_r ; 0 (no overflow)

lda 0xdeadbeef
sub a 0x0beefu32
sys &test_au ; 0xdead0000

lda 0x12345
sub a 0x12345
sys &test_au ; 0

lda 4
sub a -2
sys &test_ai ; 6

lda -1
sub a 3
sys &test_ai ; -4

lda -5
sub a 3
sys &test_ai ; -8

lda -5
sub a -8
sys &test_ai ; 3

lda 5
sub a 6
sys &test_ai ; -1

lda 0x80000000
sub a 5
sys &test_au ; 0x7ffffffb
sys &test_r ; 0b10 (signed overflow)

hlt
)EOF";
    run_and_compare_num(s, {-1, 0x1fu, 0b0u, 0xdead0000u, 0u, 6, -4, -8, 3, -1, 0x7ffffffbu, 0b10u});
}

TEST_F(Signed, mul) {
    std::string s = R"EOF(
lda 2
mul a 3
sys &test_au ; 6

lda 0
mul a 0
sys &test_au ; 0

lda 1
mul a 0
sys &test_au ; 0

lda 0
mul a 4
sys &test_au ; 0

lda 0xffffffff
mul a 1
sys &test_au ; 0xffffffff
sys &test_r ; 0xffffffff

lda 0xffffffff
mul a 0xffffffff
sys &test_au ; 1
sys &test_r ; 0

lda 77
mul a -1
sys &test_ai ; -77

lda -77
mul a 5
sys &test_ai ; -385

lda -77
mul a -5
sys &test_ai ; 385

lda 0x5aadbeef
mul a 16
sys &test_au ; 0xaadbeef0
sys &test_r ; 0x5

hlt
)EOF";
    run_and_compare_num(s, {6u, 0u, 0u, 0u, 0xffffffffu, 0xffffffffu, 1u, 0u, -77, -385, 385, 0xaadbeef0u, 0x5u});
}

TEST_F(Signed, div) {
    std::string s = R"EOF(
lda 2
div a 3
sys &test_ai ; 0
sys &test_r ; 2

lda 0
div a 5
sys &test_ai ; 0

lda 6
div a 2
sys &test_ai ; 3
sys &test_r ; 0

lda 1337
div a 1
sys &test_ai ; 1337

lda 7
div a -1
sys &test_ai ; -7

lda -7
div a 2
sys &test_ai ; -3
sys &test_r ; 0xffffffff (-1)

lda -7
div a -1
sys &test_ai ; 7

lda 42
div a 0
sys &test_ai ; error

hlt
)EOF";
    run_and_compare_num(
        s,
        {0, 2u, 0, 3, 0u, 1337, -7, -3, 0xffffffffu, 7},
        "Exception: Division by zero\nCaused by instruction:\n"
        "[#40009d] div a <0x0 | 0 | 0.00000>\n"
    );
}

TEST_F(Signed, mod) {
    std::string s = R"EOF(
lda 2
mod a 3
sys &test_ai ; 2

lda 0
mod a 5
sys &test_ai ; 0

lda 42
mod a 5
sys &test_ai ; 2

lda 7
mod a -1
sys &test_ai ; 0

lda -7
mod a 2
sys &test_ai ; -1

lda -7
mod a -2
sys &test_ai ; -1

lda 7
mod a 2
sys &test_ai ; 1

lda 1337
mod a 0
sys &test_ai ; error

hlt
)EOF";
    run_and_compare_num(
        s,
        {2, 0, 2, 0, -1, -1, 1},
        "Exception: Division by zero\nCaused by instruction:\n"
        "[#40008b] mod a <0x0 | 0 | 0.00000>\n"
    );
}

TEST_F(Signed, max) {
    std::string s = R"EOF(
lda 2
max a 3
sys &test_ai ; 3
sys &test_r ; 2

lda -5
max a 5
sys &test_ai ; 5

lda 0
max a -9999
sys &test_ai ; 0
sys &test_r ; 0xffffd8f1 (-9999)

lda 0xffffffff
max a -1
sys &test_ai ; -1

hlt
)EOF";
    run_and_compare_num(s, {3, 2u, 5, 0, 0xffffd8f1u, -1});
}

TEST_F(Signed, min) {
    std::string s = R"EOF(

lda 2
min a 3
sys &test_ai ; 2
sys &test_r ; 3

lda -5
min a 5
sys &test_ai ; -5

lda 0
min a -9999
sys &test_ai ; -9999
sys &test_r ; 0

lda 0xffffffff
min a -1
sys &test_ai ; -1

hlt
)EOF";
    run_and_compare_num(s, {2, 3u, -5, -9999, 0u, -1});
}

TEST_F(Signed, abs) {
    std::string s = R"EOF(
lda -2
abs a
sys &test_ai ; 2
sys &test_r ; 0xffffffff (-1)

lda 0
abs a
sys &test_ai ; 0
sys &test_r ; 0

lda 42
abs a
sys &test_ai ; 42
sys &test_r ; 1

lda 0xffffffff
abs a
sys &test_ai ; 1

; -(INT_MIN) == INT_MIN / abs(INT_MIN) == INT_MIN
; See `man abs`
lda -2147483648
abs a
sys &test_ai ; -2147483648

hlt
)EOF";
    run_and_compare_num(s, {2, 0xffffffffu, 0, 0u, 42, 1u, 1, (INT32_MIN)});
}

TEST_F(Signed, sign) {
    std::string s = R"EOF(
lda -2
sign a
sys &test_ai ; -1

lda 0
sign a
sys &test_ai ; 0

lda 42
sign a
sys &test_ai ; 1

hlt
)EOF";
    run_and_compare_num(s, {-1, 0, 1});
}

#pragma clang diagnostic pop
