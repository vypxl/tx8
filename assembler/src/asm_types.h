#pragma once

#include "instruction.h"

typedef struct tx_asm_Parameter {
    tx_num32 value;
    tx_uint8 mode;
} tx_asm_Parameter;

typedef struct tx_asm_Instruction {
    tx_Opcode        opcode;
    tx_asm_Parameter p1, p2, p3;
} tx_asm_Instruction;

typedef struct tx_asm_Statement {
    char*               label;
    tx_asm_Instruction* inst;
} tx_asm_Statement;

typedef struct tx_asm_Label {
    char*     name;
    tx_uint32 id;
    tx_uint32 position;
} tx_asm_Label;

typedef struct tx_asm_LL {
    void*             item;
    struct tx_asm_LL* next;
} tx_asm_LL;

void     tx_asm_print_parameter(tx_asm_Parameter* p);
tx_uint8 tx_asm_parameter_write_binary(tx_asm_Parameter* p, tx_uint8* buf);

tx_uint32 tx_asm_instruction_length(tx_asm_Instruction* inst);
void      tx_asm_print_instruction(tx_asm_Instruction* inst);
void      tx_asm_instruction_write_binary(tx_asm_Instruction* inst, tx_uint8* buf);


void       tx_asm_LL_destroy(tx_asm_LL* ll);
tx_asm_LL* tx_asm_LL_next(tx_asm_LL* ll);
void       tx_asm_LL_insert(tx_asm_LL** ll, void* item);
void*      tx_asm_LL_get(tx_asm_LL* ll, tx_uint32 idx);
void       tx_asm_LL_append(tx_asm_LL** ll, void* item);

#define tx_asm_LL_FOREACH_BEGIN(type, name, ll) \
    tx_asm_LL* __next = ll; \
    if ((__next) != NULL) do { \
            type name = (type)(__next->item);
#define tx_asm_LL_FOREACH_END \
    } \
    while ((__next = tx_asm_LL_next(__next))) \
        ;
