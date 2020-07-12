/**
 * @file assembler.h
 * @brief tx_asm_Assembler and its methods
 * @details Includes methods for parsing assembly and generating debug text or tx8 binary output
 */
#pragma once

#include "tx8/asm/types.h"

#include <stdio.h>
#include <tx8/core/types.h>

/// @brief Struct to store the state of an assembler.
/// @details It stores all found labels and instructions.
typedef struct tx_asm_Assembler {
    tx_asm_LL* labels;
    tx_asm_LL* instructions;
    tx_uint32  position;
    tx_uint32  last_label_id;
} tx_asm_Assembler;

/// Initialize the assembler
void tx_asm_init_assembler(tx_asm_Assembler* asm);
/// @brief Run the assembly process on the specified input.
/// @details Run this before reading any output like the resulting binary.
void tx_asm_run_assembler(tx_asm_Assembler* asm, FILE* input);
/// Write the generated binary to a tx8 binary file specified by `output`
void tx_asm_assembler_write_binary_file(tx_asm_Assembler* asm, FILE* output);
/// Get a pointer to the generated tx8 binary in malloc'd memory
tx_uint8* tx_asm_assembler_generate_binary(tx_asm_Assembler* asm, tx_uint32* out_size);
/// Free all resources allocated by the assembler
void tx_asm_destroy_assembler(tx_asm_Assembler* asm);
/// Print an error message with the current linenumber from lex
void tx_asm_error(const char* format, ...);

/// Register a new label and return its id or the id of the already registered label with the same name
tx_uint32 tx_asm_assembler_handle_label(tx_asm_Assembler* asm, char* name);
/// Set the position of a registered label if it has not been set already. Returns the id of the label.
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* asm, char* name);
/// Get the position associated with the label which has the specified id.
tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* asm, tx_uint32 id);
/// Convert all label IDs found in parameters of instructions in the instruction list to their corresponding absolute positions
void tx_asm_assembler_convert_labels(tx_asm_Assembler* asm);

/// Add a parsed instruction to the instruction list of the assembler
void tx_asm_assembler_add_instruction(tx_asm_Assembler* asm, tx_asm_Instruction inst);

/// Pretty print the instruction list
void tx_asm_assembler_print_instructions(tx_asm_Assembler* asm);
/// Pretty print all found labels and their positions
void tx_asm_assembler_print_labels(tx_asm_Assembler* asm);
