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

ld ab -1
lds as ab
lds a as
sys &test_ai ; -1

hlt
)EOF";
    run_and_compare_num(s, {1, -1, 1, -1, -1});
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

zero a
zero b
ld as -2
ld bb -3
sub as bb
sys &test_au ; 1
sys &test_r ; 0

zero a
ld bb -1
sub a bb
sys &test_ai ; 1

hlt
)EOF";
    run_and_compare_num(s, {0xfeu, 0b01u, 0x7eu, 0b10u, 0xfffeu, 0b01u, 0x7ffeu, 0b10u, 1u, 0u, 1});
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

hlt
)EOF";
    run_and_compare_num(s, {0x0u, 0x55u, 0x36u, 0x46u, 0xfffffdfcu, 0xffffffea});
}

TEST_F(SmallRegisters, mod) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
mod ab bs
sys &test_au ; 0x55

lda 0x1234
ldb 0x55
mod as bb
sys &test_au ; 0x46

lda -11890
ldb 23
mod as bb
sys &test_au ; 0xffffffea

hlt
)EOF";
    run_and_compare_num(s, {0x55u, 0x46u, 0xffffffeau});
}

TEST_F(SmallRegisters, min) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
min ab bs
sys &test_au ; 0x55
sys &test_r ; 0x1234

min as bb
sys &test_au ; 0x34
sys &test_r ; 0x55

lda -11890
ldb 23
min as bb
sys &test_ai ; -11890
sys &test_ri ; 23

min ab bs
lds a ab ; needed because otherwise a would be -11890 again as the upper bits are not overwritten
sys &test_ai ; -114
sys &test_ri ; 23

hlt
)EOF";
    run_and_compare_num(s, {0x55u, 0x1234u, 0x34u, 0x55u, -11890, 23, -114, 23});
}

TEST_F(SmallRegisters, max) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
max ab bs
sys &test_au ; 0x34 ; 0x12 prefix is truncated
sys &test_r ; 0x55

lda 0x55
ldb 0x1234
max as bb
sys &test_au ; 0x55
sys &test_r ; 0x34

lda -11890
ldb 23
max as bb
lds a as ; needed because otherwise a would be weird as the upper bits are not overwritten
sys &test_ai ; 23
sys &test_ri ; -11890

lda -11890
ldb 23
max ab bs
lds a ab
sys &test_ai ; 23
sys &test_ri ; -114

hlt
)EOF";
    run_and_compare_num(s, {0x34u, 0x55u, 0x55u, 0x34u, 23, -11890, 23, -114});
}

TEST_F(SmallRegisters, abs) {
    std::string s = R"EOF(
lda -3
abs as
lds a as
sys &test_ai ; 3
sys &test_ri ; -1

abs as
sys &test_ai ; 3
sys &test_ri ; 1

lda -3
abs ab
lds a ab
sys &test_ai ; 3
sys &test_ri ; -1

abs ab
sys &test_ai ; 3
sys &test_ri ; 1

lda 0
abs as
sys &test_ai ; 0
sys &test_ri ; 0

abs ab
sys &test_ai ; 0
sys &test_ri ; 0

hlt
)EOF";
    run_and_compare_num(s, {3, -1, 3, 1, 3, -1, 3, 1, 0, 0, 0, 0});
}

TEST_F(SmallRegisters, sign) {
    std::string s = R"EOF(
lda -3
sign as
lds a as
sys &test_ai ; -1

sign ab
lds a ab
sys &test_ai ; -1

lda 3
sign as
sys &test_ai ; 1

sign ab
sys &test_ai ; 1

lda 0
sign as
sys &test_ai ; 0

sign ab
sys &test_ai ; 0

hlt
)EOF";
    run_and_compare_num(s, {-1, -1, 1, 1, 0, 0});
}

TEST_F(SmallRegisters, and) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
and ab bs
sys &test_au ; 0x14

lda 0x55
ldb 0x1234
and as bb
sys &test_au ; 0x14

lda 0xfebc
ldb 0x1234
and ab bs
sys &test_au ; 0xfe34

lda 0xfebc
ldb 0x1234
and as bb
sys &test_au ; 0x34

hlt
)EOF";
    run_and_compare_num(s, {0x14u, 0x14u, 0xfe34u, 0x34u});
}

TEST_F(SmallRegisters, or) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
or ab bs
sys &test_au ; 0x75

lda 0x55
ldb 0x1234
or as bb
sys &test_au ; 0x75

lda 0xfebc
ldb 0x1234
or ab bs
sys &test_au ; 0xfebc

lda 0xfebc
ldb 0x1234
or as bb
sys &test_au ; 0xfebc

hlt
)EOF";
    run_and_compare_num(s, {0x75u, 0x75u, 0xfebcu, 0xfebcu});
}

TEST_F(SmallRegisters, not ) {
    std::string s = R"EOF(
lda 0x123456
not as
sys &test_au ; 0x12cba9

not ab
sys &test_au ; 0x12cb56

hlt
)EOF";
    run_and_compare_num(s, {0x12cba9u, 0x12cb56u});
}

TEST_F(SmallRegisters, nand) {
    std::string s = R"EOF(
lda 0x123456
ldb 0xfebc
nand ab bs
sys &test_au ; 0x1234eb

lda 0x123456
nand as bb
sys &test_au ; 0x12ffeb

hlt
)EOF";
    run_and_compare_num(s, {0x1234ebu, 0x12ffebu});
}

TEST_F(SmallRegisters, xor) {
    std::string s = R"EOF(
lda 0x123456
ldb 0xfebc
xor ab bs
sys &test_au ; 0x1234ea

lda 0x123456
xor as bb
sys &test_au ; 0x12caea

hlt
)EOF";
    run_and_compare_num(s, {0x1234eau, 0x1234eau});
}

TEST_F(SmallRegisters, slr) {
    std::string s = R"EOF(
lda 0x123456
ldb 0x1234de04
slr as bs
sys &test_au ; 0x120345
sys &test_r ; 0x6

slr ab bb
sys &test_au ; 0x120304
sys &test_r ; 0x5

hlt
)EOF";
    run_and_compare_num(s, {0x120345u, 0x6u, 0x120304u, 0x5u});
}

TEST_F(SmallRegisters, sll) {
    std::string s = R"EOF(
lda 0x123456
ldb 0x1234de04
sll as bs
sys &test_au ; 0x124560
sys &test_r ; 0x3

sll ab bb
sys &test_au ; 0x124500
sys &test_r ; 0x6

hlt
)EOF";
    run_and_compare_num(s, {0x124560u, 0x3u, 0x124500u, 0x6u});
}

TEST_F(SmallRegisters, sar) {
    std::string s = R"EOF(
lda 0x123456
ldb 0x1234de04
sar as bs
sys &test_au ; 0x120345
sys &test_r ; 0x6

sar ab bb
sys &test_au ; 0x120304
sys &test_r ; 0x5

lda 0x128856
ldb 0x1234de04
sar as bs
sys &test_au ; 0x12f885
sys &test_r ; 0x6

sar ab bb
sys &test_au ; 0x12f8f8
sys &test_r ; 0x5

hlt
)EOF";
    run_and_compare_num(s, {0x120345u, 0x6u, 0x120304u, 0x5u, 0x12f885u, 0x6u, 0x12f8f8u, 0x5u});
}

TEST_F(SmallRegisters, ror) {
    std::string s = R"EOF(
lda 0x123456
ldb 0x1234de04
ror as bs
sys &test_au ; 0x126345

ror ab bb
sys &test_au ; 0x126354

hlt
)EOF";
    run_and_compare_num(s, {0x126345u, 0x126354u});
}

TEST_F(SmallRegisters, rol) {
    std::string s = R"EOF(
lda 0x123456
ldb 0x1234de04
rol as bs
sys &test_au ; 0x124563

rol ab bb
sys &test_au ; 0x124536

hlt
)EOF";
    run_and_compare_num(s, {0x124563u, 0x124536u});
}

TEST_F(SmallRegisters, set) {
    std::string s = R"EOF(
lda 0
ldb 4
set as bs
sys &test_au ; 0x10
sys &test_r ; 0x0

ldb 0
set ab bb
sys &test_au ; 0x11
sys &test_r ; 0x0

set ab 23 ; truncated => 7
sys &test_au ; 0x91
sys &test_r ; 0x0

set ab 0
sys &test_au ; 0x91
sys &test_r ; 0x1

hlt
)EOF";
    run_and_compare_num(s, {0x10u, 0x0u, 0x11u, 0x0u, 0x91u, 0x0u, 0x91u, 0x1u});
}

TEST_F(SmallRegisters, clr) {
    std::string s = R"EOF(
lda 0x91
clr ab 23
sys &test_au ; 0x11
sys &test_r ; 0x1

clr as 4
sys &test_au ; 0x1
sys &test_r ; 0x1

clr ab 0
sys &test_au ; 0x0
sys &test_r ; 0x1

clr as 0
sys &test_au ; 0x0
sys &test_r ; 0x0

hlt
)EOF";
    run_and_compare_num(s, {0x11u, 0x1u, 0x1u, 0x1u, 0x0u, 0x1u, 0x0u, 0x0u});
}

TEST_F(SmallRegisters, tgl) {
    std::string s = R"EOF(
lda 0
ldb 4
tgl as bs
sys &test_au ; 0x10
sys &test_r ; 0x0

ldb 0
tgl ab bb
sys &test_au ; 0x11
sys &test_r ; 0x0

tgl ab 23 ; truncated => 7
sys &test_au ; 0x91
sys &test_r ; 0x0

lda 0x91
tgl ab 23
sys &test_au ; 0x11
sys &test_r ; 0x1

tgl as 4
sys &test_au ; 0x1
sys &test_r ; 0x1

tgl ab 0
sys &test_au ; 0x0
sys &test_r ; 0x1

hlt
)EOF";
    run_and_compare_num(s, {0x10u, 0x0u, 0x11u, 0x0u, 0x91u, 0x0u, 0x11u, 0x1u, 0x1u, 0x1u, 0x0u, 0x1u});
}

TEST_F(SmallRegisters, test) {
    std::string s = R"EOF(
lda 0x123457
test ab 8
sys &test_r ; 0x1

test as 16
sys &test_r ; 0x1

hlt
)EOF";
    run_and_compare_num(s, {0x1u, 0x1u});
}

TEST_F(SmallRegisters, uadd) {
    std::string s = R"EOF(
lda 0xfe
uadd ab 3
sys &test_au ; 0x1
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7e
uadd ab 3
sys &test_au ; 0x81
sys &test_r ; 0b10 (signed overflow)

lda 0xfffe
uadd as 3
sys &test_au ; 0x1
sys &test_r ; 0b01 (unsigned overflow)

lda 0x7ffe
uadd as 3
sys &test_au ; 0x8001
sys &test_r ; 0b10 (signed overflow)

lda 2
ldb -3
uadd a bb
sys &test_ai ; 0xff
sys &test_r ; 0

hlt
)EOF";
    run_and_compare_num(s, {0x1u, 0b01u, 0x81u, 0b10u, 0x1u, 0b01u, 0x8001u, 0b10u, 0xff, 0u});
}

TEST_F(SmallRegisters, usub) {
    std::string s = R"EOF(
lda 0x1
usub ab 3
sys &test_au ; 0xfe
sys &test_r ; 0b01 (unsigned overflow)

lda 0x81
usub ab 3
sys &test_au ; 0x7e
sys &test_r ; 0b10 (signed overflow)

lda 0x1
usub as 3
sys &test_au ; 0xfffe
sys &test_r ; 0b01 (unsigned overflow)

lda 0x8001
usub as 3
sys &test_au ; 0x7ffe
sys &test_r ; 0b10 (signed overflow)

lda 0
ldb 0
ld ab -2
ld bb -3
usub ab bb
sys &test_au ; 1
sys &test_r ; 0

lda 0
ld bb -1
usub a bb
sys &test_ai ; -255

hlt
)EOF";
    run_and_compare_num(s, {0xfeu, 0b01u, 0x7eu, 0b10u, 0xfffeu, 0b01u, 0x7ffeu, 0b10u, 0x1u, 0u, -255});
}

TEST_F(SmallRegisters, umul) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
umul ab bs
sys &test_au ; 0x44
sys &test_r ; 0

lda 0x1234
ldb 0x55
umul as bb
sys &test_au ; 0x0b44
sys &test_r ; 0

lda -11890
ldb 23
umul as bb
sys &test_au ; 0xffffd3c2 (the first 16 bits are 0xffff because they retain their previous value)
sys &test_r ; 0

lda -11890
ldb 23
umul ab bs
sys &test_au ; 0xffffd1c2 (the first 24 bits are 0xffffd1 because they retain their previous value)
sys &test_r ; 0
ld a ab
sys &test_au ; 0xc2 (now the upper bits were set to 0)

hlt
)EOF";
    run_and_compare_num(s, {0x44u, 0u, 0x0b44u, 0u, 0xffffd3c2u, 0u, 0xffffd1c2u, 0u, 0xc2u});
}

TEST_F(SmallRegisters, udiv) {
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

zero a
ld as -11890
ldb 23
udiv as bb
sys &test_au ; 0x91c
sys &test_r ; 0xa

hlt
)EOF";
    run_and_compare_num(s, {0x0u, 0x55u, 0x36u, 0x46u, 0x91cu, 0xau});
}

TEST_F(SmallRegisters, umod) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
umod ab bs
sys &test_au ; 0x55

lda 0x1234
ldb 0x55
umod as bb
sys &test_au ; 0x46

lda -11890
ldb 23
umod as bb
sys &test_au ; 0xffff000a

hlt
)EOF";
    run_and_compare_num(s, {0x55u, 0x46u, 0xffff000au});
}

TEST_F(SmallRegisters, umin) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
umin ab bs
sys &test_au ; 0x55
sys &test_r ; 0x1234

umin as bb
sys &test_au ; 0x34
sys &test_r ; 0x55

lda -11890
ldb 23
umin as bb
lds a as
lds r rs
sys &test_ai ; 23
sys &test_ri ; -11890

lda -11890
ldb 23
umin ab bs
lds a ab ; needed because otherwise a would be -11890 again as the upper bits are not overwritten
lds r rb
sys &test_ai ; 23
sys &test_ri ; -114

hlt
)EOF";
    run_and_compare_num(s, {0x55u, 0x1234u, 0x34u, 0x55u, 23, -11890, 23, -114});
}

TEST_F(SmallRegisters, umax) {
    std::string s = R"EOF(
lda 0x55
ldb 0x1234
umax ab bs
sys &test_au ; 0x34 ; 0x12 prefix is truncated
sys &test_r ; 0x55

lda 0x55
ldb 0x1234
umax as bb
sys &test_au ; 0x55
sys &test_r ; 0x34

lda -11890
ldb 23
umax as bb
lds a as ; needed because otherwise a would be weird as the upper bits are not overwritten
sys &test_ai ; -11890
sys &test_ri ; 23

lda -11890
ldb 23
umax ab bs
lds a ab
sys &test_ai ; -114
sys &test_ri ; 23

hlt
)EOF";
    run_and_compare_num(s, {0x34u, 0x55u, 0x55u, 0x34u, -11890, 23, -114, 23});
}

#pragma clang diagnostic pop
