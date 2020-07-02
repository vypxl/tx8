#include "tx8/asm/assembler.h"
#include "tx8/asm/types.h"

#include <stdio.h>
#include <stdlib.h>

int tx_asm_yydebug = 0;

int main(int argc, char** argv) {
    ++argv;
    --argc;
    FILE* input = NULL;
    if (argc > 0) input = fopen(argv[0], "r");
    else
        input = stdin;

    tx_asm_Assembler asm;
    tx_asm_init_assembler(&asm);
    tx_asm_run_assembler(&asm, input);

    if (tx_asm_yydebug) {
        printf("\nlabels:\n");
        tx_asm_assembler_print_labels(&asm);

        printf("\ninstructions:\n");
        tx_asm_assembler_print_instructions(&asm);
    }

    tx_asm_assembler_write_binary(&asm, stdout);

    tx_asm_destroy_assembler(&asm);

    return 0;
}
