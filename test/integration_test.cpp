#include "VMTest.hpp"

TEST_F(Integration, HelloWorld) {
    std::string s = R"EOF(
push "Hello world!"
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
