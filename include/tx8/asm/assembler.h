#pragma once

#include "tx8/asm/types.h"

#include <stdio.h>
#include <tx8/core/types.h>

typedef struct tx_asm_Assembler {
    tx_asm_LL* labels;
    tx_asm_LL* instructions;
    tx_uint32  position;
    tx_uint32  last_label_id;
} tx_asm_Assembler;

static const tx_uint8 tx_asm_param_label_id = 0xf1;

void tx_asm_init_assembler(tx_asm_Assembler* asm);
void tx_asm_run_assembler(tx_asm_Assembler* asm, FILE* input);
void tx_asm_assembler_write_binary(tx_asm_Assembler* asm, FILE* output);
void tx_asm_destroy_assembler(tx_asm_Assembler* asm);

void tx_asm_error(const char* format, ...);

// return the id of the existing or newly created label
tx_uint32 tx_asm_assembler_new_label(tx_asm_Assembler* asm, char* name);

// returns the id of the label whose position was set
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* asm, char* name);

tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* asm, tx_uint32 id);

void tx_asm_assembler_add_instruction(tx_asm_Assembler* asm, tx_asm_Instruction inst);

void tx_asm_assembler_convert_labels(tx_asm_Assembler* asm);

void tx_asm_assembler_print_param(tx_asm_Assembler* asm, tx_asm_Parameter* p);

void tx_asm_assembler_print_instruction(tx_asm_Assembler* asm, tx_asm_Instruction* inst);

void tx_asm_assembler_print_instructions(tx_asm_Assembler* asm);

void tx_asm_assembler_print_labels(tx_asm_Assembler* asm);
