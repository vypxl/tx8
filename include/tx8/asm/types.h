/**
 * @file types.h
 * @brief Intermediate types used throughout the tx8-asm library
 * @details Includes structs for Parameter, Instruction, Statement and Label constructs, and a linked list type.
 *          The types defined here differ from those defined in the core library! This is because the assembler
 *          has different needs for the representation of some parts.
 */
#pragma once

#include <tx8/core/instruction.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Struct representing a parameter to an instruction, consisting of value and mode
typedef struct tx_asm_Parameter {
    tx_num32 value;
    tx_uint8 mode;
} tx_asm_Parameter;

/// Struct representing an instruction with opcode and parameters
typedef struct tx_asm_Instruction {
    tx_Opcode        opcode;
    tx_asm_Parameter p1, p2;
} tx_asm_Instruction;

/// Struct representing a label, consisting of a string name, an id and an absolute position in the resulting binary
typedef struct tx_asm_Label {
    char*     name;
    tx_uint32 id;
    tx_uint32 position;
} tx_asm_Label;

/// A simple representation of a linked list
typedef struct tx_asm_LL {
    void*             item;
    struct tx_asm_LL* next;
} tx_asm_LL;

/// A custom parameter mode to be used as a marker for label ids to be converted to their absolute positions
static const tx_uint8 tx_asm_param_label_id = 0xf1;

/// Pretty print a parameter
void tx_asm_print_parameter(tx_asm_Parameter* p);
/// Write the binary representation of the given parameter to the buffer (size of the buffer should be at least 4 bytes)
tx_uint8 tx_asm_parameter_generate_binary(tx_asm_Parameter* p, tx_uint8* buf);

/// Get the length of the binary representation of the instruction in bytes
tx_uint32 tx_asm_instruction_length(tx_asm_Instruction* inst);
/// Pretty print an instruction
void tx_asm_print_instruction(tx_asm_Instruction* inst);
/// Write the binary representation of the given instruction to the buffer (size of the buffer should be at least tx_INSTRUCTION_MAX_LENGTH)
void tx_asm_instruction_generate_binary(tx_asm_Instruction* inst, tx_uint8* buf);

/// Free all resources allocated by the linked list. Also frees all items!
void tx_asm_LL_destroy(tx_asm_LL* ll);
/// Get the next item in a linked list or NULL if the list is at its end or NULL itself
tx_asm_LL* tx_asm_LL_next(tx_asm_LL* ll);
/// Insert the new malloc'd item at the current position in the linked list
void tx_asm_LL_insert(tx_asm_LL** ll, void* item);
/// Get the item at the position idx
void* tx_asm_LL_get(tx_asm_LL* ll, tx_uint32 idx);
/// Insert the new malloc'd item at the end of the linked list
void tx_asm_LL_append(tx_asm_LL** ll, void* item);

/// Begin a for each like iteration over a linked list. `name` specifies the identifier of the current item.
#define tx_asm_LL_FOREACH_BEGIN(type, name, ll) \
    tx_asm_LL* __next = ll; \
    if ((__next) != NULL) do { \
            type name = (type)(__next->item);
/// End a for each iteration over a linked list
#define tx_asm_LL_FOREACH_END \
    } \
    while ((__next = tx_asm_LL_next(__next))) \
        ;

#ifdef __cplusplus
}
#endif
