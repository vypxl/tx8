#include <stdio.h>
#include <stdlib.h>
#include <tx8/asm/assembler.h>
#include <tx8/asm/types.h>
#include <tx8/core/cpu.h>
#include <tx8/core/instruction.h>
#include <tx8/core/stdlib.h>
#include <tx8/core/types.h>

int tx_asm_yydebug = 1;

// int main(int argc, char** argv) {
int main() {
    /*
    ++argv;
    --argc;
    FILE* input = NULL;
    if (argc > 0) input = fopen(argv[0], "r");
    else
        input = stdin;
    */

    FILE* input = fopen("test/test.tx8", "r"); // TODO remove this

    tx_asm_Assembler asm;
    tx_asm_init_assembler(&asm);
    tx_asm_run_assembler(&asm, input);

    if (tx_asm_yydebug) {
        printf("labels:\n");
        tx_asm_assembler_print_labels(&asm);

        printf("\ninstructions:\n");
        tx_asm_assembler_print_instructions(&asm);
    }

    tx_asm_assembler_write_binary_file(&asm, stdout);
    tx_uint32 rom_size = 0;
    tx_uint8* rom      = tx_asm_assembler_generate_binary(&asm, &rom_size);
    tx_asm_destroy_assembler(&asm);

    tx_CPU cpu;
    tx_init_cpu(&cpu, rom, rom_size);
    tx_cpu_use_stdlib(&cpu);
    free(rom);

    printf("\n==== Starting execution ====\n");
    tx_run_cpu(&cpu);
    tx_destroy_cpu(&cpu);
    return 0;
}
