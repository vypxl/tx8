#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(Bitwise, and) {
    std::string s = R"EOF(
lda 0xffffffff
and a 0xffffffff
sys &test_au ; 0xffffffff

lda 0xffffffff
and a 0xfffffffe
sys &test_au ; 0xfffffffe

lda 0xffffffff
and a 0
sys &test_au ; 0

lda 0
and a 0xffffffff
sys &test_au ; 0

lda 1337
and a 7331
sys &test_ai ; 1057

lda 0xdeadbeef
and a 0xffff
sys &test_au ; 0xbeef

lda -5243
and a 5520
sys &test_ai ; 384

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0xfffffffeu, 0u, 0u, 1057, 0xbeefu, 384});
}

TEST_F(Bitwise, or) {
    std::string s = R"EOF(
lda 0xffffffff
or a 0xffffffff
sys &test_au ; 0xffffffff

lda 0xffffffff
or a 0xfffffffe
sys &test_au ; 0xffffffff

lda 0xffffffff
or a 0
sys &test_au ; 0xffffffff

lda 0
or a 0xffffffff
sys &test_au ; 0xffffffff

lda 1337
or a 7331
sys &test_ai ; 7611

lda 0xdeadbeef
or a 0xffff
sys &test_au ; 0xdeadffff

lda -5243
or a 5520
sys &test_ai ; -107

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 7611, 0xdeadffffu, -107});
}

TEST_F(Bitwise, not ) {
    std::string s = R"EOF(
lda 0xffffffff
not a
sys &test_au ; 0

lda -1
not a
sys &test_au ; 0

lda 0
not a
sys &test_au ; 0xffffffff

lda 0xdeadbeef
not a
sys &test_au ; 0x21524110

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0xffffffffu, 0x21524110u});
}

TEST_F(Bitwise, nand) {
    std::string s = R"EOF(
lda 0xffffffff
nand a 0xffffffff
sys &test_au ; 0

lda 0xffffffff
nand a 0xfffffffe
sys &test_au ; 1

lda 0xffffffff
nand a 0
sys &test_au ; 0xffffffff

lda 0
nand a 0xffffffff
sys &test_au ; 0xffffffff

lda 1337
nand a 7331
sys &test_ai ; -1058

lda 0xdeadbeef
nand a 0xffff
sys &test_au ; 0xffff4110

lda -5243
nand a 5520
sys &test_ai ; -385

hlt
)EOF";
    run_and_compare_num(s, {0u, 1u, 0xffffffffu, 0xffffffffu, -1058, 0xffff4110u, -385});
}

TEST_F(Bitwise, xor) {
    std::string s = R"EOF(
lda 0xffffffff
xor a 0xffffffff
sys &test_au ; 0

lda 0xffffffff
xor a 0xfffffffe
sys &test_au ; 1

lda 0xffffffff
xor a 0
sys &test_au ; 0xffffffff

lda 0
xor a 0xffffffff
sys &test_au ; 0xffffffff

lda 1337
xor a 7331
sys &test_ai ; 6554

lda 0xdeadbeef
xor a 0xffff
sys &test_au ; 0xdead4110

lda -5243
xor a 5520
sys &test_ai ; -491

hlt
)EOF";
    run_and_compare_num(s, {0u, 1u, 0xffffffffu, 0xffffffffu, 6554, 0xdead4110u, -491});
}

TEST_F(Bitwise, slr) {
    std::string s = R"EOF(
lda 0
slr a 1
sys &test_au ; 0
sys &test_r ; 0

lda 1
slr a 1
sys &test_au ; 0
sys &test_r ; 1

lda 0xff
slr a 1
sys &test_au ; 0x7f

lda 0xdeadbeef
slr a 17
sys &test_au ; 0x6f56
sys &test_r ; 0x1eef

lda 0xffffffff
slr a 32 ; (0 in the lowest 5 bits)
sys &test_au ; 0xffffffff
sys &test_r ; 0

lda 0xffffffff
slr a -4 ; (== 0xfffffffc => 28 in the lowest 5 bits)
sys &test_au ; 0xf
sys &test_r ; 0xfffffff

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0u, 1u, 0x7fu, 0x6f56u, 0x1beefu, 0xffffffffu, 0u, 0xfu, 0xfffffffu});
}

TEST_F(Bitwise, sar) {
    std::string s = R"EOF(
lda 0
sar a 1
sys &test_au ; 0
sys &test_r ; 0

lda 1
sar a 1
sys &test_au ; 0
sys &test_r ; 1

lda 0xff
sar a 1
sys &test_au ; 0x7f

lda 0xdeadbeef
sar a 17
sys &test_au ; 0xffffef56
sys &test_r ; 0x1eef

lda 0xffffffff
sar a 32 ; (0 in the lowest 5 bits)
sys &test_au ; 0xffffffff
sys &test_r ; 0

lda 0xffffffff
sar a -4 ; (== 0xfffffffc => 28 in the lowest 5 bits)
sys &test_au ; 0xffffffff
sys &test_r ; 0fffffff

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0u, 1u, 0x7fu, 0xffffef56u, 0x1beefu, 0xffffffffu, 0u, 0xffffffffu, 0xfffffffu});
}

TEST_F(Bitwise, sll) {
    std::string s = R"EOF(
lda 0
sll a 1
sys &test_au ; 0
sys &test_r ; 0

lda 1
sll a 1
sys &test_au ; 2
sys &test_r ; 0

lda 0xff
sll a 1
sys &test_au ; 0x1fe

lda 0xdeadbeef
sll a 17
sys &test_au ; 0x7dde0000
sys &test_r ; 0x1bd5b

lda 0xffffffff
sll a 32
sys &test_au ; 0xffffffff
sys &test_r ; 0

lda 0xffffffff
sll a -4 ; (28 in the lowest 5 bits)
sys &test_au ; 0xf0000000
sys &test_r ; 0xfffffff

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 2u, 0u, 0x1feu, 0x7dde0000u, 0x1bd5bu, 0xffffffffu, 0u, 0xf0000000u, 0xfffffffu});
}

TEST_F(Bitwise, ror) {
    std::string s = R"EOF(
lda 0
ror a 1
sys &test_au ; 0

lda 0x12345678
ror a 16
sys &test_au ; 0x56781234

lda -6194
lda 0xffffe7ce
ror a 19
sys &test_au ; fcf9dfff

lda 1
ror a 31
sys &test_au ; 2

lda 0xdeadbeef
ror a 64
sys &test_au ; 0xdeadbeef

hlt
)EOF";
    run_and_compare_num(s, {0u, 0x56781234u, 0xfcf9dfffu, 0x2u, 0xdeadbeefu});
}

TEST_F(Bitwise, rol) {
    std::string s = R"EOF(
lda 0
rol a 1
sys &test_au ; 0

lda 0x12345678
rol a 16
sys &test_au ; 0x56781234

lda -6194
rol a 19
sys &test_au ; 0x3e77ffff

lda 1
rol a 31
sys &test_au ; 0x8000000

lda 0xdeadbeef
rol a 64
sys &test_au ; 0xdeadbeef

hlt
)EOF";
    run_and_compare_num(s, {0u, 0x56781234u, 0x3e77ffffu, 0x80000000u, 0xdeadbeefu});
}

TEST_F(Bitwise, set) {
    std::string s = R"EOF(
lda 0
set a 1
sys &test_au ; 2
sys &test_r ; 0

lda 0
set a 31
set a 31
sys &test_au ; 0x80000000
sys &test_r ; 1

lda 5
set a 3
sys &test_au ; 0xd

lda 0
set a 32
sys &test_au ; 1

hlt
)EOF";
    run_and_compare_num(s, {2u, 0u, 0x80000000u, 1u, 0xdu, 1u});
}

TEST_F(Bitwise, clr) {
    std::string s = R"EOF(
lda 1
clr a 1
sys &test_au ; 1
sys &test_r ; 0

lda 0xffffffff
clr a 31
sys &test_au ; 0x7fffffff
sys &test_r ; 1

lda 0xd
clr a 3
sys &test_au ; 5

lda 1
clr a 32
sys &test_au ; 0

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, 0x7fffffffu, 1u, 5u, 0u});
}

TEST_F(Bitwise, tgl) {
    std::string s = R"EOF(
lda 1
tgl a 1
sys &test_au ; 3
sys &test_r ; 0

lda 0xffffffff
tgl a 31
sys &test_au ; 0x7fffffff
sys &test_r ; 1

lda 0xd
tgl a 3
sys &test_au ; 5

lda 2
tgl a 32
sys &test_au ; 3

hlt
)EOF";
    run_and_compare_num(s, {3u, 0u, 0x7fffffffu, 1u, 5u, 3u});
}

TEST_F(Bitwise, test) {
    std::string s = R"EOF(
lda 0xf00dbabe
test a 0
sys &test_r ; 0
test a 31
sys &test_r ; 1
test a 32
sys &test_r ; 0
test a 6
sys &test_r ; 0

hlt
)EOF";
    run_and_compare_num(s, {0u, 1u, 0u, 0u});
}

#pragma clang diagnostic pop
