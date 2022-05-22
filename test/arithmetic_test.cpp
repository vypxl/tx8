#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(VMTest, inc) {
    std::string s = R"EOF(
lda 0
inc a
psh a
sys &test_uint ; 1

lda -1
inc a
psh a
sys &test_uint ; 0

lda 0x7fffffff
inc a
psh a
sys &test_int ; -2147483648

hlt
)EOF";
    run_and_compare_num(s, {1u, 0u, INT_MIN});
}

TEST_F(VMTest, dec) {
    std::string s = R"EOF(
lda 0
dec a
psh a
sys &test_int ; -1

lda -1
dec a
psh a
sys &test_int ; -2

lda 1
dec a
psh a
sys &test_int ; 0
hlt
)EOF";
    run_and_compare_num(s, {-1, -2, 0});
}

TEST_F(VMTest, add) {
    std::string s = R"EOF(
lda 2
add a 3
psh a
sys &test_uint ; 5

lda 0
add a 0
psh a
sys &test_uint ; 0

lda 0xffffffff
add a 1
psh a
sys &test_uint ; 0

lda 0xffffffff
add a 0xffffffff
psh a
sys &test_uint ; 0xfffffffe

lda 4
add a -2
psh a
sys &test_int ; 2

lda -1
add a 3
psh a
sys &test_int ; 2

lda -5
add a 3
psh a
sys &test_int ; -2

lda 5
add a -8
psh a
sys &test_int ; -3

lda -2
add a -7
psh a
sys &test_int ; -9

hlt
)EOF";
    run_and_compare_num(s, {5u, 0u, 0u, 0xfffffffeu, 2, 2, -2, -3, -9});
}

TEST_F(VMTest, sub) {
    std::string s = R"EOF(
lda 2
sub a 3
psh a
sys &test_uint ; 0xffffffff

lda 32
sub a 1
psh a
sys &test_uint ; 1f

lda 0xdeadbeef
sub a 0xbeef
psh a
sys &test_uint ; 0xdead0000

lda 0x12345
sub a 0x12345
psh a
sys &test_uint ; 0

lda 4
sub a -2
psh a
sys &test_int ; 6

lda -1
sub a 3
psh a
sys &test_int ; -4

lda -5
sub a 3
psh a
sys &test_int ; -8

lda -5
sub a -8
psh a
sys &test_int ; 3

lda 5
sub a 6
psh a
sys &test_int ; -1

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0x1fu, 0xdead0000u, 0u, 6, -4, -8, 3, -1});
}

TEST_F(VMTest, mul) {
    std::string s = R"EOF(
lda 2
mul a 3
psh a
sys &test_uint ; 6

lda 0
mul a 0
psh a
sys &test_uint ; 0

lda 1
mul a 0
psh a
sys &test_uint ; 0

lda 0
mul a 4
psh a
sys &test_uint ; 0

lda 0xffffffff
mul a 1
psh a
sys &test_uint ; 0xffffffff

lda 0xffffffff
mul a 0xffffffff
psh a
sys &test_uint ; 1

lda 77
mul a -1
psh a
sys &test_int ; -77

lda -77
mul a 5
psh a
sys &test_int ; -385

lda -77
mul a -5
psh a
sys &test_int ; 385

hlt
)EOF";
    run_and_compare_num(s, {6u, 0u, 0u, 0u, 0xffffffffu, 1u, -77, -385, 385});
}

TEST_F(VMTest, div_signed) {
    std::string s = R"EOF(
lda 2
div a 3
psh a
sys &test_int ; 0

lda 0
div a 5
psh a
sys &test_int ; 0

lda 6
div a 2
psh a
sys &test_int ; 3

lda 1337
div a 1
psh a
sys &test_int ; 1337

lda 7
div a -1
psh a
sys &test_int ; -7

lda -7
div a 2
psh a
sys &test_int ; -3

lda -7
div a -1
psh a
sys &test_int ; 7

lda 42
div a 0
psh a
sys &test_int ; error

hlt
)EOF";
    run_and_compare_num(s,
                        {0, 0, 3, 1337, -7, -3, 7},
                        "Exception: divide by zero\nCaused by instruction:\n#4000a0: 25 (div) 0 0 "
                        "0 (modes: 6 3 0)\n");
}

TEST_F(VMTest, mod_signed) {
    std::string s = R"EOF(
lda 2
mod a 3
psh a
sys &test_int ; 2

lda 0
mod a 5
psh a
sys &test_int ; 0

lda 42
mod a 5
psh a
sys &test_int ; 2

lda 7
mod a -1
psh a
sys &test_int ; 0

lda -7
mod a 2
psh a
sys &test_int ; -1

lda -7
mod a -2
psh a
sys &test_int ; -1

lda 7
mod a 2
psh a
sys &test_int ; 1

lda 1337
mod a 0
psh a
sys &test_int ; error

hlt
)EOF";
    run_and_compare_num(s,
                        {2, 0, 2, 0, -1, -1, 1},
                        "Exception: divide by zero\nCaused by instruction:\n#4000a0: 26 (mod) 0 0 "
                        "0 (modes: 6 3 0)\n");
}

TEST_F(VMTest, and) {
    std::string s = R"EOF(
lda 0xffffffff
and a 0xffffffff
psh a
sys &test_uint ; 0xffffffff

lda 0xffffffff
and a 0xfffffffe
psh a
sys &test_uint ; 0xfffffffe

lda 0xffffffff
and a 0
psh a
sys &test_uint ; 0

lda 0
and a 0xffffffff
psh a
sys &test_uint ; 0

lda 1337
and a 7331
psh a
sys &test_int ; 1057

lda 0xdeadbeef
and a 0xffff
psh a
sys &test_uint ; 0xbeef

lda -5243
and a 5520
psh a
sys &test_int ; 384

hlt
)EOF";
    run_and_compare_num(s, {0xffffffffu, 0xfffffffeu, 0u, 0u, 1057, 0xbeefu, 384});
}

TEST_F(VMTest, or) {
    std::string s = R"EOF(
lda 0xffffffff
ora a 0xffffffff
psh a
sys &test_uint ; 0xffffffff

lda 0xffffffff
ora a 0xfffffffe
psh a
sys &test_uint ; 0xffffffff

lda 0xffffffff
ora a 0
psh a
sys &test_uint ; 0xffffffff

lda 0
ora a 0xffffffff
psh a
sys &test_uint ; 0xffffffff

lda 1337
ora a 7331
psh a
sys &test_int ; 7611

lda 0xdeadbeef
ora a 0xffff
psh a
sys &test_uint ; 0xdeadffff

lda -5243
ora a 5520
psh a
sys &test_int ; -107

hlt
)EOF";
    run_and_compare_num(
        s, {0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 7611, 0xdeadffffu, -107});
}

TEST_F(VMTest, not ) {
    std::string s = R"EOF(
lda 0xffffffff
not a
psh a
sys &test_uint ; 0

lda -1
not a
psh a
sys &test_uint ; 0

lda 0
not a
psh a
sys &test_uint ; 0xffffffff

lda 0xdeadbeef
not a
psh a
sys &test_uint ; 0x21524110

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0xffffffffu, 0x21524110u});
}

TEST_F(VMTest, nand) {
    std::string s = R"EOF(
lda 0xffffffff
nnd a 0xffffffff
psh a
sys &test_uint ; 0

lda 0xffffffff
nnd a 0xfffffffe
psh a
sys &test_uint ; 1

lda 0xffffffff
nnd a 0
psh a
sys &test_uint ; 0xffffffff

lda 0
nnd a 0xffffffff
psh a
sys &test_uint ; 0xffffffff

lda 1337
nnd a 7331
psh a
sys &test_int ; -1058

lda 0xdeadbeef
nnd a 0xffff
psh a
sys &test_uint ; 0xffff4110

lda -5243
nnd a 5520
psh a
sys &test_int ; -385

hlt
)EOF";
    run_and_compare_num(s, {0u, 1u, 0xffffffffu, 0xffffffffu, -1058, 0xffff4110u, -385});
}

TEST_F(VMTest, xor) {
    std::string s = R"EOF(
lda 0xffffffff
xor a 0xffffffff
psh a
sys &test_uint ; 0

lda 0xffffffff
xor a 0xfffffffe
psh a
sys &test_uint ; 1

lda 0xffffffff
xor a 0
psh a
sys &test_uint ; 0xffffffff

lda 0
xor a 0xffffffff
psh a
sys &test_uint ; 0xffffffff

lda 1337
xor a 7331
psh a
sys &test_int ; 6554

lda 0xdeadbeef
xor a 0xffff
psh a
sys &test_uint ; 0xdead4110

lda -5243
xor a 5520
psh a
sys &test_int ; -491

hlt
)EOF";
    run_and_compare_num(s, {0u, 1u, 0xffffffffu, 0xffffffffu, 6554, 0xdead4110u, -491});
}

TEST_F(VMTest, shr) {
    std::string s = R"EOF(
lda 0
shr a 1
psh a
sys &test_uint ; 0

lda 1
shr a 1
psh a
sys &test_uint ; 0

lda 0xff
shr a 1
psh a
sys &test_uint ; 0x7f

lda 0xdeadbeef
shr a 17
psh a
sys &test_uint ; 0x6f56

lda 0xffffffff
shr a 32
psh a
sys &test_uint ; 0

lda 0xffffffff
shr a -4
psh a
sys &test_uint ; 0

hlt
)EOF";
    run_and_compare_num(s, {0u, 0u, 0x7fu, 0x6f56u, 0u, 0u});
}

TEST_F(VMTest, shl) {
    std::string s = R"EOF(
lda 0
shl a 1
psh a
sys &test_uint ; 0

lda 1
shl a 1
psh a
sys &test_uint ; 2

lda 0xff
shl a 1
psh a
sys &test_uint ; 0x1fe

lda 0xdeadbeef
shl a 17
psh a
sys &test_uint ; 0x7dde0000

lda 0xffffffff
shl a 32
psh a
sys &test_uint ; 0

lda 0xffffffff
shl a -4
psh a
sys &test_uint ; 0

hlt
)EOF";
    run_and_compare_num(s, {0u, 2u, 0x1feu, 0x7dde0000u, 0u, 0u});
}

TEST_F(VMTest, ror) {
    std::string s = R"EOF(
lda 0
ror a 1
psh a
sys &test_uint ; 0

lda 0x12345678
ror a 16
psh a
sys &test_uint ; 0x56781234

lda -6194
lda 0xffffe7ce
ror a 19
psh a
sys &test_uint ; fcf9dfff

lda 1
ror a 31
psh a
sys &test_uint ; 2

lda 0xdeadbeef
ror a 64
psh a
sys &test_uint ; 0xdeadbeef

hlt
)EOF";
    run_and_compare_num(s, {0u, 0x56781234u, 0xfcf9dfffu, 0x2u, 0xdeadbeefu});
}

TEST_F(VMTest, rol) {
    std::string s = R"EOF(
lda 0
rol a 1
psh a
sys &test_uint ; 0

lda 0x12345678
rol a 16
psh a
sys &test_uint ; 0x56781234

lda -6194
rol a 19
psh a
sys &test_uint ; 0x3e77ffff

lda 1
rol a 31
psh a
sys &test_uint ; 0x8000000

lda 0xdeadbeef
rol a 64
psh a
sys &test_uint ; 0xdeadbeef

hlt
)EOF";
    run_and_compare_num(s, {0u, 0x56781234u, 0x3e77ffffu, 0x80000000u, 0xdeadbeefu});
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
    run_and_compare_num(s, {1.0f, 0.0f, 1235.567f});
}

TEST_F(VMTest, fde) {
    std::string s = R"EOF(
lda 0.0
fde a
psh a
sys &test_float ; -1.0

lda 1.0
fde a
psh a
sys &test_float ; 0.0

lda 1234.567
fde a
psh a
sys &test_float ; 1233.567
hlt
)EOF";
    run_and_compare_num(s, {-1.0f, 0.0f, 1233.567f});
}

TEST_F(VMTest, fad) {
    std::string s = R"EOF(
lda 15.62
fad a 0.0
psh a
sys &test_float ; 15.62

lda 0.0
fad a 0.0
psh a
sys &test_float ; 0.0

lda -12.34
fad a -12.34
psh a
sys &test_float ; -24.68

lda 55.55
fad a -11.11
psh a
sys &test_float ; 44.44

lda -11.11
fad a 55.55
psh a
sys &test_float ; 44.44

hlt
)EOF";
    run_and_compare_num(s, {15.62f, 0.0f, -24.68f, 44.44f, 44.44f});
}

TEST_F(VMTest, fsu) {
    std::string s = R"EOF(
lda 15.62
fsu a 0.0
psh a
sys &test_float ; 15.62

lda 0.0
fsu a 0.0
psh a
sys &test_float ; 0.0

lda -12.34
fsu a -12.34
psh a
sys &test_float ; 0.0

lda 55.55
fsu a -11.11
psh a
sys &test_float ; 66.66

lda -11.11
fsu a 55.55
psh a
sys &test_float ; -66.66

hlt
)EOF";
    run_and_compare_num(s, {{15.62f, 0.0f, 0.0f, 66.66f, -66.66f}});
}

TEST_F(VMTest, fmu) {
    std::string s = R"EOF(
lda 1.0
fmu a 5.5
psh a
sys &test_float ; 5.5

lda 0.0
fmu a 1.5
psh a
sys &test_float ; 0.0

lda 2.5
fmu a -1.5
psh a
sys &test_float ; -3.75

lda -6.0
fmu a 2.0
psh a
sys &test_float ; -12.0

lda -2.5
fmu a -6.0
psh a
sys &test_float ; 15.0

lda -1.0
fmu a -1.0
psh a
sys &test_float ; 1.0

hlt
)EOF";
    run_and_compare_num(s, {{5.5f, 0.0f, -3.75f, -12.0f, 15.0f, 1.0f}});
}

TEST_F(VMTest, fdi) {
    std::string s = R"EOF(
lda 1.0
fdi a 5.0
psh a
sys &test_float ; 0.2

lda 5.0
fdi a -0.2
psh a
sys &test_float ; -25.0

lda -1.0
fdi a 10.0
psh a
sys &test_float ; -0.1

lda -5.0
fdi a -10.0
psh a
sys &test_float ; 0.5

lda 0.0
fdi a 1.0
psh a
sys &test_float ; 0.0

lda 13.37
fdi a 0.0
psh a
sys &test_float ; inf

lda 0.0
fdi a 0.0
psh a
sys &test_float ; nan

hlt
)EOF";
    run_and_compare_num(s,
                        {0.2f,
                         -25.0f,
                         -0.1f,
                         0.5f,
                         0.0f,
                         std::numeric_limits<float>::infinity(),
                         std::numeric_limits<float>::quiet_NaN()});
}

TEST_F(VMTest, fmo) {
    std::string s = R"EOF(
lda 1.0
fmo a 5.0
psh a
sys &test_float ; 1.0

lda 13.37
fmo a 10.0
psh a
sys &test_float ; 3.37

lda 5.4
fmo a -2.2
psh a
sys &test_float ; 1.0

lda -6.0
fmo a 1.6
psh a
sys &test_float ; -1.2

lda 1.0
fmo a 0.0
psh a
sys &test_float ; nan

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 3.37f, 1.0f, -1.2f, std::numeric_limits<float>::quiet_NaN()});
}

TEST_F(VMTest, itf) {
    std::string s = R"EOF(
lda 0
itf a
psh a
sys &test_float ; 0.0

lda 1337
itf a
psh a
sys &test_float ; 1337.0

lda -5
itf a
psh a
sys &test_float ; -5.0

hlt
)EOF";
    run_and_compare_num(s, {0.0f, 1337.0f, -5.0f});
}

TEST_F(VMTest, fti) {
    std::string s = R"EOF(
lda 0.0
fti a
psh a
sys &test_int ; 0

lda 133.7
fti a
psh a
sys &test_int ; 133

lda -4.2
fti a
psh a
sys &test_int ; -4

hlt
)EOF";
    run_and_compare_num(s, {0, 133, -4});
}

#pragma clang diagnostic pop
