#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include "VMTest.hpp"
#include <cmath>

TEST_F(Float, finc) {
    std::string s = R"EOF(
lda 0.0
finc a
sys &test_af ; 1.0

lda -1.0
finc a
sys &test_af ; 0.0

lda 1234.567
finc a
sys &test_af ; 1235.567

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 0.0f, 1235.567f});
}

TEST_F(Float, fdec) {
    std::string s = R"EOF(
lda 0.0
fdec a
sys &test_af ; -1.0

lda 1.0
fdec a
sys &test_af ; 0.0

lda 1234.567
fdec a
sys &test_af ; 1233.567
hlt
)EOF";
    run_and_compare_num(s, {-1.0f, 0.0f, 1233.567f});
}

TEST_F(Float, fadd) {
    std::string s = R"EOF(
lda 15.62
fadd a 0.0
sys &test_af ; 15.62

lda 0.0
fadd a 0.0
sys &test_af ; 0.0

lda -12.34
fadd a -12.34
sys &test_af ; -24.68

lda 55.55
fadd a -11.11
sys &test_af ; 44.44

lda -11.11
fadd a 55.55
sys &test_af ; 44.44

hlt
)EOF";
    run_and_compare_num(s, {15.62f, 0.0f, -24.68f, 44.44f, 44.44f});
}

TEST_F(Float, fsub) {
    std::string s = R"EOF(
lda 15.62
fsub a 0.0
sys &test_af ; 15.62

lda 0.0
fsub a 0.0
sys &test_af ; 0.0

lda -12.34
fsub a -12.34
sys &test_af ; 0.0

lda 55.55
fsub a -11.11
sys &test_af ; 66.66

lda -11.11
fsub a 55.55
sys &test_af ; -66.66

hlt
)EOF";
    run_and_compare_num(s, {{15.62f, 0.0f, 0.0f, 66.66f, -66.66f}});
}

TEST_F(Float, fmul) {
    std::string s = R"EOF(
lda 1.0
fmul a 5.5
sys &test_af ; 5.5

lda 0.0
fmul a 1.5
sys &test_af ; 0.0

lda 2.5
fmul a -1.5
sys &test_af ; -3.75

lda -6.0
fmul a 2.0
sys &test_af ; -12.0

lda -2.5
fmul a -6.0
sys &test_af ; 15.0

lda -1.0
fmul a -1.0
sys &test_af ; 1.0

hlt
)EOF";
    run_and_compare_num(s, {{5.5f, 0.0f, -3.75f, -12.0f, 15.0f, 1.0f}});
}

TEST_F(Float, fdiv) {
    std::string s = R"EOF(
lda 1.0
fdiv a 5.0
sys &test_af ; 0.2

lda 5.0
fdiv a -0.2
sys &test_af ; -25.0

lda -1.0
fdiv a 10.0
sys &test_af ; -0.1

lda -5.0
fdiv a -10.0
sys &test_af ; 0.5

lda 0.0
fdiv a 1.0
sys &test_af ; 0.0

lda 13.37
fdiv a 0.0
sys &test_af ; inf

lda 0.0
fdiv a 0.0
sys &test_af ; nan

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

TEST_F(Float, fmod) {
    std::string s = R"EOF(
lda 1.0
fmod a 5.0
sys &test_af ; 1.0

lda 13.37
fmod a 10.0
sys &test_af ; 3.37

lda 5.4
fmod a -2.2
sys &test_af ; 1.0

lda -6.0
fmod a 1.6
sys &test_af ; -1.2

lda 1.0
fmod a 0.0
sys &test_af ; nan

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 3.37f, 1.0f, -1.2f, std::numeric_limits<float>::quiet_NaN()});
}

TEST_F(Float, fmax) {
    std::string s = R"EOF(
lda 1.0
fmax a 5.0
sys &test_af ; 5.0
sys &test_rf ; 1.0

lda 13.37
fmax a -5.0
sys &test_af ; 13.37
sys &test_rf ; -5.0

lda 0.0
fmax a 0.0
sys &test_af ; 0.0
sys &test_rf ; 0.0

hlt
)EOF";
    run_and_compare_num(s, {5.0f, 1.0f, 13.37f, -5.0f, 0.0f, 0.0f});
}

TEST_F(Float, fmin) {
    std::string s = R"EOF(
lda 1.0
fmin a 5.0
sys &test_af ; 1.0
sys &test_rf ; 5.0

lda 13.37
fmin a -5.0
sys &test_af ; -5.0
sys &test_rf ; 13.37

lda 0.0
fmin a 0.0
sys &test_af ; 0.0
sys &test_rf ; 0.0

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 5.0f, -5.0f, 13.37f, 0.0f, 0.0f});
}

TEST_F(Float, fabs) {
    std::string s = R"EOF(
lda 1.0
fabs a
sys &test_af ; 1.0
sys &test_rf ; 1.0

lda -4.0
fabs a
sys &test_af ; 4.0
sys &test_rf ; -1.0

lda 0.0
fabs a
sys &test_af ; 0.0
sys &test_rf ; 0.0

hlt
)EOF";
    run_and_compare_num(s, {1.0f, 1.0f, 4.0f, -1.0f, 0.0f, 0.0f});
}

TEST_F(Float, fsign) {
    std::string s = R"EOF(
lda 1.0
fsign a
sys &test_af ; 1.0

lda -4.0
fsign a
sys &test_af ; -1.0

lda 0.0
fsign a
sys &test_af ; 0.0

hlt
)EOF";
    run_and_compare_num(s, {1.0f, -1.0f, 0.0f});
}

TEST_F(Float, sin) {
    std::string s = R"EOF(
lda 1.0
sin a
sys &test_af ; sin(1.0)

lda -1.0
sin a
sys &test_af ; sin(-1.0)

lda 0.0
sin a
sys &test_af ; sin(0.0)

lda 3.14159
sin a
sys &test_af ; sin(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) sin(1.0), (float) sin(-1.0), (float) sin(0.0), (float) sin(3.14159f)});
}

TEST_F(Float, cos) {
    std::string s = R"EOF(
lda 1.0
cos a
sys &test_af ; cos(1.0)

lda -1.0
cos a
sys &test_af ; cos(-1.0)

lda 0.0
cos a
sys &test_af ; cos(0.0)

lda 3.14159
cos a
sys &test_af ; cos(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) cos(1.0), (float) cos(-1.0), (float) cos(0.0), (float) cos(3.14159f)});
}

TEST_F(Float, tan) {
    std::string s = R"EOF(
lda 1.0
tan a
sys &test_af ; tan(1.0)

lda -1.0
tan a
sys &test_af ; tan(-1.0)

lda 0.0
tan a
sys &test_af ; tan(0.0)

lda 3.14159
tan a
sys &test_af ; tan(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) tan(1.0), (float) tan(-1.0), (float) tan(0.0), (float) tan(3.14159f)});
}

TEST_F(Float, asin) {
    std::string s = R"EOF(
lda 1.0
asin a
sys &test_af ; asin(1.0)

lda -1.0
asin a
sys &test_af ; asin(-1.0)

lda 0.0
asin a
sys &test_af ; asin(0.0)

hlt
)EOF";
    run_and_compare_num(s, {(float) asin(1.0), (float) asin(-1.0), (float) asin(0.0)});
}

TEST_F(Float, acos) {
    std::string s = R"EOF(
lda 1.0
acos a
sys &test_af ; acos(1.0)

lda -1.0
acos a
sys &test_af ; acos(-1.0)

lda 0.0
acos a
sys &test_af ; acos(0.0)

hlt
)EOF";
    run_and_compare_num(s, {(float) acos(1.0), (float) acos(-1.0), (float) acos(0.0)});
}

TEST_F(Float, atan) {
    std::string s = R"EOF(
lda 1.0
atan a
sys &test_af ; atan(1.0)

lda -1.0
atan a
sys &test_af ; atan(-1.0)

lda 0.0
atan a
sys &test_af ; atan(0.0)

hlt
)EOF";
    run_and_compare_num(s, {(float) atan(1.0), (float) atan(-1.0), (float) atan(0.0)});
}

TEST_F(Float, atan2) {
    std::string s = R"EOF(
lda 2.0
atan2 a 2.0
sys &test_af ; atan2(2.0, 2.0)

lda -1.5
atan2 a 2.0
sys &test_af ; atan2(-1.5, 2.0)

lda 2.0
atan2 a -1.5
sys &test_af ; atan2(2.0, -1.5)

lda -7.0
atan2 a -1.5
sys &test_af ; atan2(-7.0, -1.5)

hlt
)EOF";
    run_and_compare_num(s, {(float) atan2(2.0, 2.0), (float) atan2(-1.5, 2.0), (float) atan2(2.0, -1.5), (float) atan2(-7.0, -1.5)});
}

TEST_F(Float, sqrt) {
    std::string s = R"EOF(
lda 1.0
sqrt a
sys &test_af ; sqrt(1.0)

lda -1.0
sqrt a
sys &test_af ; sqrt(-1.0)

lda 0.0
sqrt a
sys &test_af ; sqrt(0.0)

lda 3.14159
sqrt a
sys &test_af ; sqrt(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) sqrt(1.0), (float) sqrt(-1.0), (float) sqrt(0.0), (float) sqrt(3.14159f)});
}

TEST_F(Float, pow) {
    std::string s = R"EOF(
lda 1.0
pow a 5678134.5
sys &test_af ; pow(1.0, 5678134.5)

lda -2.6
pow a -5.1
sys &test_af ; pow(-2.6, -5.1)

lda 7.4
pow a -0.5
sys &test_af ; pow(7.4, -0.5)

hlt
)EOF";
    run_and_compare_num(s, {(float) pow(1.0, 5678134.5), (float) pow(-2.6, -5.1), (float) pow(7.4, -0.5)});
}

TEST_F(Float, exp) {
    std::string s = R"EOF(
lda 1.0
exp a
sys &test_af ; exp(1.0)

lda -1.0
exp a
sys &test_af ; exp(-1.0)

lda 0.0
exp a
sys &test_af ; exp(0.0)

lda 3.14159
exp a
sys &test_af ; exp(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) exp(1.0), (float) exp(-1.0), (float) exp(0.0), (float) exp(3.14159f)});
}

TEST_F(Float, log) {
    std::string s = R"EOF(
lda 1.0
log a
sys &test_af ; log(1.0)

lda -1.6
log a
sys &test_af ; log(-1.6)

lda 0.0
log a
sys &test_af ; log(0.0)

lda 3.14159
log a
sys &test_af ; log(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) log(1.0), (float) log(-1.6), (float) log(0.0), (float) log(3.14159f)});
}

TEST_F(Float, log2) {
    std::string s = R"EOF(
lda 1.0
log2 a
sys &test_af ; log2(1.0)

lda -1.6
log2 a
sys &test_af ; log2(-1.6)

lda 0.0
log2 a
sys &test_af ; log2(0.0)

lda 3.14159
log2 a
sys &test_af ; log2(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) log2(1.0), (float) log2(-1.6), (float) log2(0.0), (float) log2(3.14159f)});
}

TEST_F(Float, log10) {
    std::string s = R"EOF(
lda 1.0
log10 a
sys &test_af ; log10(1.0)

lda -1.6
log10 a
sys &test_af ; log10(-1.6)

lda 0.0
log10 a
sys &test_af ; log10(0.0)

lda 3.14159
push a
sys &print_f32
log10 a
push a
sys &print_f32
sys &test_af ; log10(3.14159)

hlt
)EOF";
    run_and_compare_num(s, {(float) log10f(1.0), (float) log10f(-1.6), (float) log10f(0.0), log10f(3.14159f)});
}

#pragma clang diagnostic pop
