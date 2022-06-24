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

/// Write the binary representation of the given parameter to the buffer (size of the buffer should be at least 4 bytes)
tx_uint8 tx_asm_parameter_generate_binary(tx_Parameter* p, tx_uint8* buf);

/// Recalculate the `len` field of the instruction (binary representation of the instruction in bytes) and return it
tx_uint32 tx_asm_instruction_calculate_length(tx_Instruction* inst);
/// Write the binary representation of the given instruction to the buffer (size of the buffer should be at least tx_INSTRUCTION_MAX_LENGTH)
void tx_asm_instruction_generate_binary(tx_Instruction* inst, tx_uint8* buf);

#ifdef __cplusplus
}
#endif
