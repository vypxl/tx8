#include "VMTest.hpp"

TEST_F(VMTest, HelloWorld) {
    std::string s = R"EOF(
mov o 0xc03000

mov $0 0x48
mov $1 0x65
mov $2 0x6c
mov $3 0x6c
mov $4 0x6f
mov $5 0x20
mov $6 0x77
mov $7 0x6f
mov $8 0x72
mov $9 0x6c
mov $a 0x64
mov $b 0x21
mov $c 0

psh o
sys &println

hlt
)EOF";
    run_and_compare(s, "Hello world!\n");
}
