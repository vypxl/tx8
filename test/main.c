#include "../src/cpu.h"
#include "../src/instruction.h"
#include "../src/types.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_ROM_SIZE 8

static const tx_uint8 testrom[TEST_ROM_SIZE] = {tx_op_lda, tx_param_constant8 << 4U, 0x42, tx_op_hlt};

int main() {
    tx_CPU cpu;
    tx_init_cpu(&cpu, (tx_mem_ptr)testrom, TEST_ROM_SIZE);
    tx_run_cpu(&cpu);
    tx_destroy_cpu(&cpu);
}
