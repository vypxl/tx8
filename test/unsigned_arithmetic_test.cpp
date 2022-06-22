#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include "VMTest.hpp"


TEST_F(Unsigned, inc) {
    std::string s = R"EOF(
lda 0xffffffff
inc a
sys &test_au ; 0
sys &test_r ; 0b01 (unsigned overflow)

hlt
)EOF";
    run_and_compare_num(s, {0u, 0b01u});
}

TEST_F(Unsigned, dec) {
    std::string s = R"EOF(
lda 0
dec a
sys &test_au ; 0xffffffff
sys &test_r ; 0b01 (unsigned overflow)

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0b01u});
}

TEST_F(Unsigned, add) {
    std::string s = R"EOF(
lda 0xffffffff
add a 1
sys &test_au ; 0
sys &test_r ; 0b01 (unsigned overflow)

lda 0xffffffff
add a 0xffffffff
sys &test_au ; 0xfffffffe
sys &test_r ; 0b01 (unsigned overflow)

lda 4
add a -2
sys &test_ai ; 2
sys &test_r ; 0b01 (unsigned overflow)

hlt
)EOF";
    run_and_compare_num(s, {0u, 0b01u, 0xfffffffeu, 0b01u, 2, 0b1u});
}

TEST_F(Unsigned, sub) {
    std::string s = R"EOF(
lda 2
sub a 3
sys &test_au ; 0xffffffff
sys &test_r ; 0b01 (unsigned overflow)

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0b1u});
}

#pragma clang diagnostic pop
