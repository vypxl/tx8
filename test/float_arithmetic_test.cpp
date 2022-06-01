#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(VMTest, finc) {
    std::string s = R"EOF(
lda 0.0
finc a
push a
sys &test_float ; 1.0

lda -1.0
finc a
push a
sys &test_float ; 0.0

lda 1234.567
finc a
push a
sys &test_float ; 1235.567

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 0.0f, 1235.567f});
}

TEST_F(VMTest, fdec) {
    std::string s = R"EOF(
lda 0.0
fdec a
push a
sys &test_float ; -1.0

lda 1.0
fdec a
push a
sys &test_float ; 0.0

lda 1234.567
fdec a
push a
sys &test_float ; 1233.567
hlt
)EOF";
    run_and_compare_num(s, {-1.0f, 0.0f, 1233.567f});
}

TEST_F(VMTest, fadd) {
    std::string s = R"EOF(
lda 15.62
fadd a 0.0
push a
sys &test_float ; 15.62

lda 0.0
fadd a 0.0
push a
sys &test_float ; 0.0

lda -12.34
fadd a -12.34
push a
sys &test_float ; -24.68

lda 55.55
fadd a -11.11
push a
sys &test_float ; 44.44

lda -11.11
fadd a 55.55
push a
sys &test_float ; 44.44

hlt
)EOF";
    run_and_compare_num(s, {15.62f, 0.0f, -24.68f, 44.44f, 44.44f});
}

TEST_F(VMTest, fsub) {
    std::string s = R"EOF(
lda 15.62
fsub a 0.0
push a
sys &test_float ; 15.62

lda 0.0
fsub a 0.0
push a
sys &test_float ; 0.0

lda -12.34
fsub a -12.34
push a
sys &test_float ; 0.0

lda 55.55
fsub a -11.11
push a
sys &test_float ; 66.66

lda -11.11
fsub a 55.55
push a
sys &test_float ; -66.66

hlt
)EOF";
    run_and_compare_num(s, {{15.62f, 0.0f, 0.0f, 66.66f, -66.66f}});
}

TEST_F(VMTest, fmul) {
    std::string s = R"EOF(
lda 1.0
fmul a 5.5
push a
sys &test_float ; 5.5

lda 0.0
fmul a 1.5
push a
sys &test_float ; 0.0

lda 2.5
fmul a -1.5
push a
sys &test_float ; -3.75

lda -6.0
fmul a 2.0
push a
sys &test_float ; -12.0

lda -2.5
fmul a -6.0
push a
sys &test_float ; 15.0

lda -1.0
fmul a -1.0
push a
sys &test_float ; 1.0

hlt
)EOF";
    run_and_compare_num(s, {{5.5f, 0.0f, -3.75f, -12.0f, 15.0f, 1.0f}});
}

TEST_F(VMTest, fdiv) {
    std::string s = R"EOF(
lda 1.0
fdiv a 5.0
push a
sys &test_float ; 0.2

lda 5.0
fdiv a -0.2
push a
sys &test_float ; -25.0

lda -1.0
fdiv a 10.0
push a
sys &test_float ; -0.1

lda -5.0
fdiv a -10.0
push a
sys &test_float ; 0.5

lda 0.0
fdiv a 1.0
push a
sys &test_float ; 0.0

lda 13.37
fdiv a 0.0
push a
sys &test_float ; inf

lda 0.0
fdiv a 0.0
push a
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

TEST_F(VMTest, fmod) {
    std::string s = R"EOF(
lda 1.0
fmod a 5.0
push a
sys &test_float ; 1.0

lda 13.37
fmod a 10.0
push a
sys &test_float ; 3.37

lda 5.4
fmod a -2.2
push a
sys &test_float ; 1.0

lda -6.0
fmod a 1.6
push a
sys &test_float ; -1.2

lda 1.0
fmod a 0.0
push a
sys &test_float ; nan

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 3.37f, 1.0f, -1.2f, std::numeric_limits<float>::quiet_NaN()});
}

#pragma clang diagnostic pop
