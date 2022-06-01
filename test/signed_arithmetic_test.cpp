#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(VMTest, inc) {
    std::string s = R"EOF(
lda 0
inc a
push a
sys &test_uint ; 1

lda -1
inc a
push a
sys &test_uint ; 0

lda 0x7fffffff
inc a
push a
sys &test_int ; -2147483648

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, INT_MIN});
}

TEST_F(VMTest, dec) {
    std::string s = R"EOF(
lda 0
dec a
push a
sys &test_int ; -1

lda -1
dec a
push a
sys &test_int ; -2

lda 1
dec a
push a
sys &test_int ; 0
hlt
)EOF";
    run_and_compare_num(s, {-1, -2, 0});
}

TEST_F(VMTest, add) {
    std::string s = R"EOF(
lda 2
add a 3
push a
sys &test_uint ; 5

lda 0
add a 0
push a
sys &test_uint ; 0

lda 0xffffffff
add a 1
push a
sys &test_uint ; 0

lda 0xffffffff
add a 0xffffffff
push a
sys &test_uint ; 0xfffffffe

lda 4
add a -2
push a
sys &test_int ; 2

lda -1
add a 3
push a
sys &test_int ; 2

lda -5
add a 3
push a
sys &test_int ; -2

lda 5
add a -8
push a
sys &test_int ; -3

lda -2
add a -7
push a
sys &test_int ; -9

hlt
)EOF";
    run_and_compare_num(s, {5u, 0u, 0u, 0xfffffffeu, 2, 2, -2, -3, -9});
}

TEST_F(VMTest, sub) {
    std::string s = R"EOF(
lda 2
sub a 3
push a
sys &test_uint ; 0xffffffff

lda 32
sub a 1
push a
sys &test_uint ; 1f

lda 0xdeadbeef
sub a 0xbeef
push a
sys &test_uint ; 0xdead0000

lda 0x12345
sub a 0x12345
push a
sys &test_uint ; 0

lda 4
sub a -2
push a
sys &test_int ; 6

lda -1
sub a 3
push a
sys &test_int ; -4

lda -5
sub a 3
push a
sys &test_int ; -8

lda -5
sub a -8
push a
sys &test_int ; 3

lda 5
sub a 6
push a
sys &test_int ; -1

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0x1fu, 0xdead0000u, 0u, 6, -4, -8, 3, -1});
}

TEST_F(VMTest, mul) {
    std::string s = R"EOF(
lda 2
mul a 3
push a
sys &test_uint ; 6

lda 0
mul a 0
push a
sys &test_uint ; 0

lda 1
mul a 0
push a
sys &test_uint ; 0

lda 0
mul a 4
push a
sys &test_uint ; 0

lda 0xffffffff
mul a 1
push a
sys &test_uint ; 0xffffffff

lda 0xffffffff
mul a 0xffffffff
push a
sys &test_uint ; 1

lda 77
mul a -1
push a
sys &test_int ; -77

lda -77
mul a 5
push a
sys &test_int ; -385

lda -77
mul a -5
push a
sys &test_int ; 385

hlt
)EOF";
    run_and_compare_num(s, {6u, 0u, 0u, 0u, 0xffffffffu, 1u, -77, -385, 385});
}

TEST_F(VMTest, div) {
    std::string s = R"EOF(
lda 2
div a 3
push a
sys &test_int ; 0

lda 0
div a 5
push a
sys &test_int ; 0

lda 6
div a 2
push a
sys &test_int ; 3

lda 1337
div a 1
push a
sys &test_int ; 1337

lda 7
div a -1
push a
sys &test_int ; -7

lda -7
div a 2
push a
sys &test_int ; -3

lda -7
div a -1
push a
sys &test_int ; 7

lda 42
div a 0
push a
sys &test_int ; error

hlt
)EOF";
    run_and_compare_num(s,
                        {0, 0, 3, 1337, -7, -3, 7},
                        "Exception: Division by zero\nCaused by instruction:\n#4000a0: 25 (div) 0 "
                        "0 (modes: 6 3)\n");
}

TEST_F(VMTest, mod) {
    std::string s = R"EOF(
lda 2
mod a 3
push a
sys &test_int ; 2

lda 0
mod a 5
push a
sys &test_int ; 0

lda 42
mod a 5
push a
sys &test_int ; 2

lda 7
mod a -1
push a
sys &test_int ; 0

lda -7
mod a 2
push a
sys &test_int ; -1

lda -7
mod a -2
push a
sys &test_int ; -1

lda 7
mod a 2
push a
sys &test_int ; 1

lda 1337
mod a 0
push a
sys &test_int ; error

hlt
)EOF";
    run_and_compare_num(s,
                        {2, 0, 2, 0, -1, -1, 1},
                        "Exception: Division by zero\nCaused by instruction:\n#4000a0: 26 (mod) 0 "
                        "0 (modes: 6 3)\n");
}

#pragma clang diagnostic pop
