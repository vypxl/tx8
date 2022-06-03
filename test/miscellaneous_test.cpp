#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(Miscellaneous, itf) {
    std::string s = R"EOF(
lda 0
itf a
push a
sys &test_float ; 0.0

lda 1337
itf a
push a
sys &test_float ; 1337.0

lda -5
itf a
push a
sys &test_float ; -5.0

hlt
)EOF";
    run_and_compare_num(s, {0.0f, 1337.0f, -5.0f});
}

TEST_F(Miscellaneous, fti) {
    std::string s = R"EOF(
lda 0.0
fti a
push a
sys &test_int ; 0

lda 133.7
fti a
push a
sys &test_int ; 133

lda -4.2
fti a
push a
sys &test_int ; -4

hlt
)EOF";
    run_and_compare_num(s, {0, 133, -4});
}

#pragma clang diagnostic pop
