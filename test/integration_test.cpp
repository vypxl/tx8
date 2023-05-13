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

TEST_F(Integration, cmp) {
    std::string s = R"EOF(
push 1337
zero a

cmp 1 1
jeq :j0
sys &test_ai

:j0
cmp 1 0
jne :j1
sys &test_ai

:j1
cmp -1 1
jlt :j2
sys &test_ai

:j2
cmp 1 -1
jgt :j3
sys &test_ai

:j3
cmp 1 2
jle :j4
sys &test_ai

:j4
cmp 2 2
jle :j5
sys &test_ai

:j5
cmp 2 1
jge :j6
sys &test_ai

:j6
cmp 1 1
jge :j7
sys &test_ai

:j7

hlt
)EOF";
    run_and_compare_num(s, {});
}

TEST_F(Integration, ucmp) {
    std::string s = R"EOF(
push 1337
zero a

ucmp 1 1
jeq :j0
sys &test_ai

:j0
ucmp 1 0
jne :j1
sys &test_ai

:j1
ucmp 0 1
jlt :j2
sys &test_ai

:j2
ucmp 1 0
jgt :j3
sys &test_ai

:j3
ucmp 2 2
jle :j4
sys &test_ai

:j4
ucmp 1 2
jle :j5
sys &test_ai

:j5
ucmp 2 1
jge :j6
sys &test_ai

:j6
ucmp 1 1
jge :j7
sys &test_ai

:j7

hlt
)EOF";
    run_and_compare_num(s, {});
}


TEST_F(Integration, fcmp) {
    std::string s = R"EOF(
push 1337
zero a

fcmp 1.0 1.0
jeq :j0
sys &test_ai

:j0
fcmp 1.0 0.0
jne :j1
sys &test_ai

:j1
fcmp 0.0 1.0
jlt :j2
sys &test_ai

:j2
fcmp 1.0 0.0
jgt :j3
sys &test_ai

:j3
fcmp 2.0 2.0
jle :j4
sys &test_ai

:j4
fcmp 1.0 2.0
jle :j5
sys &test_ai

:j5
fcmp 2.0 1.0
jge :j6
sys &test_ai

:j6
fcmp 1.0 1.0
jge :j7
sys &test_ai

:j7

hlt
)EOF";
    run_and_compare_num(s, {});
}
