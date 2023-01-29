#include "VMTest.hpp"

TEST_F(Integration, HelloWorld) {
    std::string s = R"EOF(
ld o 0xc03000

ld $0 0x48
ld $1 0x65
ld $2 0x6c
ld $3 0x6c
ld $4 0x6f
ld $5 0x20
ld $6 0x77
ld $7 0x6f
ld $8 0x72
ld $9 0x6c
ld $a 0x64
ld $b 0x21
ld $c 0

push o
sys &println

hlt
)EOF";
    run_and_compare_str(s, "Hello world!\n");
}

TEST_F(Integration, Jumping) {
    std::string s = R"EOF(
zero a
inc a
jmp :L1
add a 55
:L1 dec a
cmp a 0
jne :end
add a 1337
:end
sys &test_ai ; 1337
hlt
)EOF";
    run_and_compare_num(s, {1337}); // NOLINT
}
