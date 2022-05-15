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
