#include "VMTest.hpp"

TEST_F(VMTest, add_unsigned) {
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

hlt
)EOF";
    run_and_compare(s, "500fffffffe");
}

TEST_F(VMTest, add_signed) {
    std::string s = R"EOF(
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
    run_and_compare(s, "22-2-3-9");
}
