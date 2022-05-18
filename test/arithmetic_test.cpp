#include "VMTest.hpp"

TEST_F(VMTest, inc) {
    std::string s = R"EOF(
lda 0
inc a
psh a
sys &print_u32 ; 1

lda -1
inc a
psh a
sys &print_u32 ; 0

lda 0x7fffffff
inc a
psh a
sys &print_i32 ; -2147483648

hlt
)EOF";
    run_and_compare(s, "10-2147483648");
}

TEST_F(VMTest, dec) {
    std::string s = R"EOF(
lda 0
dec a
psh a
sys &print_i32 ; -1

lda -1
dec a
psh a
sys &print_i32 ; -2

lda 1
dec a
psh a
sys &print_i32 ; 0
hlt
)EOF";
    run_and_compare(s, "-1-20");
}

TEST_F(VMTest, add) {
    std::string s = R"EOF(
lda 2
add a 3
psh a
sys &print_u32 ; 5

lda 0
add a 0
psh a
sys &print_u32 ; 0

lda 0xffffffff
add a 1
psh a
sys &print_u32 ; 0

lda 0xffffffff
add a 0xffffffff
psh a
sys &print_u32 ; 0xfffffffe

lda 4
add a -2
psh a
sys &print_i32 ; 2

lda -1
add a 3
psh a
sys &print_i32 ; 2

lda -5
add a 3
psh a
sys &print_i32 ; -2

lda 5
add a -8
psh a
sys &print_i32 ; -3

lda -2
add a -7
psh a
sys &print_i32 ; -9

hlt
)EOF";
    run_and_compare(s, "500fffffffe22-2-3-9");
}

TEST_F(VMTest, sub) {
    std::string s = R"EOF(
lda 2
sub a 3
psh a
sys &print_u32 ; 0xffffffff

lda 32
sub a 1
psh a
sys &print_u32 ; 1f

lda 0xdeadbeef
sub a 0xbeef
psh a
sys &print_u32 ; 0xdead0000

lda 0x12345
sub a 0x12345
psh a
sys &print_u32 ; 0

lda 4
sub a -2
psh a
sys &print_i32 ; 6

lda -1
sub a 3
psh a
sys &print_i32 ; -4

lda -5
sub a 3
psh a
sys &print_i32 ; -8

lda -5
sub a -8
psh a
sys &print_i32 ; 3

lda 5
sub a 6
psh a
sys &print_i32 ; -1

hlt
)EOF";
    run_and_compare(s, "ffffffff1fdead000006-4-83-1");
}

TEST_F(VMTest, mul) {
    std::string s = R"EOF(
lda 2
mul a 3
psh a
sys &print_u32 ; 6

lda 0
mul a 0
psh a
sys &print_u32 ; 0

lda 1
mul a 0
psh a
sys &print_u32 ; 0

lda 0
mul a 4
psh a
sys &print_u32 ; 0

lda 0xffffffff
mul a 1
psh a
sys &print_u32 ; 0xffffffff

lda 0xffffffff
mul a 0xffffffff
psh a
sys &print_u32 ; 1

lda 77
mul a -1
psh a
sys &print_i32 ; -77

lda -77
mul a 5
psh a
sys &print_i32 ; -385

lda -77
mul a -5
psh a
sys &print_i32 ; 385

hlt
)EOF";
    run_and_compare(s, "6000ffffffff1-77-385385");
}

TEST_F(VMTest, div_signed) {
    std::string s = R"EOF(
lda 2
div a 3
psh a
sys &print_i32 ; 0

lda 0
div a 5
psh a
sys &print_i32 ; 0

lda 6
div a 2
psh a
sys &print_i32 ; 3

lda 1337
div a 1
psh a
sys &print_i32 ; 1337

lda 7
div a -1
psh a
sys &print_i32 ; -7

lda -7
div a 2
psh a
sys &print_i32 ; -3

lda -7
div a -1
psh a
sys &print_i32 ; 7

lda 42
div a 0
psh a
sys &print_i32 ; 0

hlt
)EOF";
    run_and_compare(s, "0031337-7-37", "Exception: divide by zero\nCaused by instruction:\n#4000a0: 25 (div) 0 0 0 (modes: 6 3 0)\n");
}

TEST_F(VMTest, mod_signed) {
    std::string s = R"EOF(
lda 2
mod a 3
psh a
sys &print_i32 ; 2

lda 0
mod a 5
psh a
sys &print_i32 ; 0

lda 42
mod a 5
psh a
sys &print_i32 ; 2

lda 7
mod a -1
psh a
sys &print_i32 ; 0

lda -7
mod a 2
psh a
sys &print_i32 ; -1

lda -7
mod a -2
psh a
sys &print_i32 ; -1

lda 7
mod a 2
psh a
sys &print_i32 ; 1

lda 1337
mod a 0
psh a
sys &print_i32 ; 0

hlt
)EOF";
    run_and_compare(s, "2020-1-11", "Exception: divide by zero\nCaused by instruction:\n#4000a0: 26 (mod) 0 0 0 (modes: 6 3 0)\n");
}

TEST_F(VMTest, and) {
    std::string s = R"EOF(
lda 0xffffffff
and a 0xffffffff
psh a
sys &print_u32 ; 0xffffffff

lda 0xffffffff
and a 0xfffffffe
psh a
sys &print_u32 ; 0xfffffffe

lda 0xffffffff
and a 0
psh a
sys &print_u32 ; 0

lda 0
and a 0xffffffff
psh a
sys &print_u32 ; 0

lda 1337
and a 7331
psh a
sys &print_i32 ; 1057

lda 0xdeadbeef
and a 0xffff
psh a
sys &print_u32 ; 0xbeef

lda -5243
and a 5520
psh a
sys &print_i32 ; 384

hlt
)EOF";
    run_and_compare(s, "fffffffffffffffe001057beef384");
}

TEST_F(VMTest, or) {
    std::string s = R"EOF(
lda 0xffffffff
ora a 0xffffffff
psh a
sys &print_u32 ; 0xffffffff

lda 0xffffffff
ora a 0xfffffffe
psh a
sys &print_u32 ; 0xffffffff

lda 0xffffffff
ora a 0
psh a
sys &print_u32 ; 0xffffffff

lda 0
ora a 0xffffffff
psh a
sys &print_u32 ; 0xffffffff

lda 1337
ora a 7331
psh a
sys &print_i32 ; 7611

lda 0xdeadbeef
ora a 0xffff
psh a
sys &print_u32 ; 0xdeadffff

lda -5243
ora a 5520
psh a
sys &print_i32 ; -107

hlt
)EOF";
    run_and_compare(s, "ffffffffffffffffffffffffffffffff7611deadffff-107");
}

TEST_F(VMTest, not) {
    std::string s = R"EOF(
lda 0xffffffff
not a
psh a
sys &print_u32 ; 0

lda -1
not a
psh a
sys &print_u32 ; 0

lda 0
not a
psh a
sys &print_u32 ; 0xffffffff

lda 0xdeadbeef
not a
psh a
sys &print_u32 ; 0x21524110

hlt
)EOF";
    run_and_compare(s, "00ffffffff21524110");
}

TEST_F(VMTest, nand) {
    std::string s = R"EOF(
lda 0xffffffff
nnd a 0xffffffff
psh a
sys &print_u32 ; 0

lda 0xffffffff
nnd a 0xfffffffe
psh a
sys &print_u32 ; 1

lda 0xffffffff
nnd a 0
psh a
sys &print_u32 ; 0xffffffff

lda 0
nnd a 0xffffffff
psh a
sys &print_u32 ; 0xffffffff

lda 1337
nnd a 7331
psh a
sys &print_i32 ; -1058

lda 0xdeadbeef
nnd a 0xffff
psh a
sys &print_u32 ; 0xdead4110

lda -5243
nnd a 5520
psh a
sys &print_i32 ; -385

hlt
)EOF";
    run_and_compare(s, "01ffffffffffffffff-1058ffff4110-385");
}

TEST_F(VMTest, xor) {
    std::string s = R"EOF(
lda 0xffffffff
xor a 0xffffffff
psh a
sys &print_u32 ; 0

lda 0xffffffff
xor a 0xfffffffe
psh a
sys &print_u32 ; 1

lda 0xffffffff
xor a 0
psh a
sys &print_u32 ; 0xffffffff

lda 0
xor a 0xffffffff
psh a
sys &print_u32 ; 0xffffffff

lda 1337
xor a 7331
psh a
sys &print_i32 ; 6554

lda 0xdeadbeef
xor a 0xffff
psh a
sys &print_u32 ; 0xdead4110

lda -5243
xor a 5520
psh a
sys &print_i32 ; -491

hlt
)EOF";
    run_and_compare(s, "01ffffffffffffffff6554dead4110-491");
}

TEST_F(VMTest, shr) {
    std::string s = R"EOF(
lda 0
shr a 1
psh a
sys &print_u32 ; 0

lda 1
shr a 1
psh a
sys &print_u32 ; 0

lda 0xff
shr a 1
psh a
sys &print_u32 ; 0x7f

lda 0xdeadbeef
shr a 17
psh a
sys &print_u32 ; 0x6f56

lda 0xffffffff
shr a 32
psh a
sys &print_u32 ; 0

lda 0xffffffff
shr a -4
psh a
sys &print_u32 ; 0

hlt
)EOF";
    run_and_compare(s, "007f6f5600");
}

TEST_F(VMTest, shl) {
    std::string s = R"EOF(
lda 0
shl a 1
psh a
sys &print_u32 ; 0

lda 1
shl a 1
psh a
sys &print_u32 ; 2

lda 0xff
shl a 1
psh a
sys &print_u32 ; 0x1fe

lda 0xdeadbeef
shl a 17
psh a
sys &print_u32 ; 0x7dde0000

lda 0xffffffff
shl a 32
psh a
sys &print_u32 ; 0

lda 0xffffffff
shl a -4
psh a
sys &print_u32 ; 0

hlt
)EOF";
    run_and_compare(s, "021fe7dde000000");
}

TEST_F(VMTest, ror) {
    std::string s = R"EOF(
lda 0
ror a 1
psh a
sys &print_u32 ; 0

lda 0x12345678
ror a 16
psh a
sys &print_u32 ; 0x56781234

lda -6194
lda 0xffffe7ce
ror a 19
psh a
sys &print_u32 ; fcf9dfff

lda 1
ror a 31
psh a
sys &print_u32 ; 2

lda 0xdeadbeef
ror a 64
psh a
sys &print_u32 ; 0xdeadbeef

hlt
)EOF";
    run_and_compare(s, "056781234fcf9dfff2deadbeef");
}

TEST_F(VMTest, rol) {
    std::string s = R"EOF(
lda 0
rol a 1
psh a
sys &print_u32 ; 0

lda 0x12345678
rol a 16
psh a
sys &print_u32 ; 0x56781234

lda -6194
rol a 19
psh a
sys &print_u32 ; 0x3e77ffff

lda 1
rol a 31
psh a
sys &print_u32 ; 0x8000000

lda 0xdeadbeef
rol a 64
psh a
sys &print_u32 ; 0xdeadbeef

hlt
)EOF";
    run_and_compare(s, "0567812343e77ffff80000000deadbeef");
}

TEST_F(VMTest, fin) {
    std::string s = R"EOF(
lda 0.0
fin a
psh a
sys &test_float ; 1.0

lda -1.0
fin a
psh a
sys &test_float ; 0.0

lda 1234.567
fin a
psh a
sys &test_float ; 1235.567

hlt
)EOF";
    run_and_compare_f(s, {1.0, 0.0, 1235.567});
}
