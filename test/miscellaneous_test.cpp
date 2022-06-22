#pragma clang diagnostic push
#pragma ide diagnostic   ignored "readability-magic-numbers"
#include "VMTest.hpp"

TEST_F(Miscellaneous, rand_and_rseed) {
    std::string s = R"EOF(
rand a
sys &test_af ; 0.40556046
sys &test_r  ; 0x33e9

rand a
sys &test_af ; 0.70946378
sys &test_r  ; 0x5acf

rseed 5
rand a
sys &test_af ; 0.001678518
sys &test_r  ; 0x37

rseed 5
rand a
sys &test_af ; 0.001678518
sys &test_r  ; 0x37

hlt
)EOF";
    run_and_compare_num(s, {0.40556046f, 0x33e9u, 0.70946378f, 0x5acfu, 0.001678518f, 0x37u, 0.001678518f, 0x37u});
}

TEST_F(Miscellaneous, itf) {
    std::string s = R"EOF(
lda 0
itf a
sys &test_af ; 0.0

lda 1337
itf a
sys &test_af ; 1337.0

lda -5
itf a
sys &test_af ; -5.0

lda 0x80000000
itf a
sys &test_af ; -2147483648.0

hlt
)EOF";
    run_and_compare_num(s, {0.0f, 1337.0f, -5.0f, -2147483648.0f});
}

TEST_F(Miscellaneous, fti) {
    std::string s = R"EOF(
lda 0.0
fti a
sys &test_ai ; 0

lda 133.7
fti a
sys &test_ai ; 133

lda -4.2
fti a
sys &test_ai ; -4

hlt
)EOF";
    run_and_compare_num(s, {0, 133, -4});
}

TEST_F(Miscellaneous, utf) {
    std::string s = R"EOF(
lda 0
utf a
sys &test_af ; 0.0

lda 1337
utf a
sys &test_af ; 1337.0

lda 0x80000000
utf a
sys &test_af ; 2147483648.0

hlt
)EOF";
    run_and_compare_num(s, {0.0f, 1337.0f, 2147483648.0f});
}

TEST_F(Miscellaneous, ftu) {
    std::string s = R"EOF(
lda 0.0
ftu a
sys &test_au ; 0

lda 42.1337
ftu a
sys &test_au ; 42

lda -4.2
ftu a
sys &test_au ; 0xfffffffc

hlt
)EOF";
    run_and_compare_num(s, {0u, 42u, 0xfffffffcu});
}

#pragma clang diagnostic pop
