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
    run_and_compare_num(
        s, {0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 7611, 0xdeadffffu, -107});
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

lda 1
slr a 1
sys &test_au ; 0

lda 0xff
slr a 1
sys &test_au ; 0x7f

lda 0xdeadbeef
slr a 17
sys &test_au ; 0x6f56

lda 0xffffffff
slr a 32
sys &test_au ; 0

lda 0xffffffff
slr a -4
sys &test_au ; 0

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0x7fu, 0x6f56u, 0u, 0u});
}

TEST_F(Bitwise, sll) {
    std::string s = R"EOF(
lda 0
sll a 1
sys &test_au ; 0

lda 1
sll a 1
sys &test_au ; 2

lda 0xff
sll a 1
sys &test_au ; 0x1fe

lda 0xdeadbeef
sll a 17
sys &test_au ; 0x7dde0000

lda 0xffffffff
sll a 32
sys &test_au ; 0

lda 0xffffffff
sll a -4
sys &test_au ; 0

hlt
)EOF";
    run_and_compare_num(s, {0u, 2u, 0x1feu, 0x7dde0000u, 0u, 0u});
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

#pragma clang diagnostic pop
