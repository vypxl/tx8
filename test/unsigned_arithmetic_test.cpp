#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
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

TEST_F(Unsigned, umul) {
    std::string s = R"EOF(
lda 0xffffffff
umul a 1
sys &test_au ; 0xffffffff
sys &test_r ; 0

lda 0xffffffff
umul a 0xffffffff
sys &test_au ; 0x1
sys &test_r ; 0xfffffffe

lda 0xdeadbeef
umul a 0xf00dbabe
sys &test_au ; 0x43d05b62
sys &test_r ; 0xd0ced442

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0u, 0x1u, 0xfffffffeu, 0x43d05b62u, 0xd0ced442u});
}

TEST_F(Unsigned, udiv) {
    std::string s = R"EOF(
lda 0xdeadbeef
udiv a 1
sys &test_au ; 0xdeadbeef
sys &test_r ; 0

lda 0x1000
udiv a 0x8
sys &test_au ; 0x200

lda 555
udiv a 2
sys &test_au ; 277
sys &test_r ; 1

lda 5
udiv a 0

hlt
)EOF";
    run_and_compare_num(
        s,
        {0xdeadbeefu, 0u, 0x200u, 277u, 1u},
        "Exception: Division by zero\nCaused by instruction:\n[#40004b] udiv a <0x0 | 0 | 0.00000>\n"
    );
}

TEST_F(Unsigned, umod) {
    std::string s = R"EOF(
lda 0xdeadbeef
umod a 0xbeef
sys &test_au ; 0x227f

lda 15
umod a 5
sys &test_au ; 0x0

lda 2
umod a 0

hlt
)EOF";
    run_and_compare_num(
        s,
        {0x227fu, 0u},
        "Exception: Division by zero\nCaused by instruction:\n[#40002c] umod a <0x0 | 0 | 0.00000>\n"
    );
}

TEST_F(Unsigned, umax) {
    std::string s = R"EOF(
lda 0xffffffff
umax a 0xffffffff
sys &test_au ; 0xffffffff
sys &test_r ; 0xffffffff

lda 1
umax a 0
sys &test_au ; 0x1
sys &test_r ; 0x0

lda 0x42
umax a 0x1337
sys &test_au ; 0x1337
sys &test_r ; 0x42

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0xffffffff, 0x1u, 0x0u, 0x1337u, 0x42u});
}

TEST_F(Unsigned, umin) {
    std::string s = R"EOF(
lda 0xffffffff
umin a 0xffffffff
sys &test_au ; 0xffffffff
sys &test_r ; 0xffffffff

lda 1
umin a 0
sys &test_au ; 0x0
sys &test_r ; 0x1

lda 0x42
umin a 0x1337
sys &test_au ; 0x42
sys &test_r ; 0x1337

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0xffffffff, 0x0u, 0x1u, 0x42u, 0x1337u});
}

#pragma clang diagnostic pop
