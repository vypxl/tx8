/**
 * @file assembler.h
 * @brief tx_asm_Assembler and its methods
 * @details Includes methods for parsing assembly and generating debug text or tx8 binary output
 */

#pragma once

#include "lexer.hpp"
#include "tx8/asm/types.hpp"
#include "tx8_parser.hpp"

#include <cstdio>
#include <tx8/core/types.h>
#include <vector>

/// @brief Struct to store the state of an assembler.
/// @details It stores all found labels and instructions.
typedef struct tx_asm_Assembler {
    std::vector<tx_Label>       labels;
    std::vector<tx_Instruction> instructions;
    tx_uint32                   position;
    tx_uint32                   last_label_id;
    tx_uint8                    error;

    tx::parser::Lexer  lexer;
    tx::parser::Parser parser;

    tx_asm_Assembler();
} tx_asm_Assembler;

/// Initialize the assembler
void tx_asm_init_assembler(tx_asm_Assembler* as);
/// @brief Run the assembly process on the specified input file.
/// @details Run this (or tx_asm_run_assembler_buffer) before reading any output like the resulting binary.
int tx_asm_run_assembler_file(tx_asm_Assembler* as, FILE* input);
/// @brief Run the assembly process on the specified buffer.
/// @details Run this (or tx_asm_run_assembler_file) before reading any output like the resulting binary.
int tx_asm_run_assembler_buffer(tx_asm_Assembler* as, char* buf, int size);
/// Write the generated binary to a tx8 binary file specified by `output`
/// @returns true if the binary was successfully written, false otherwise
bool tx_asm_assembler_write_binary_file(tx_asm_Assembler* as, FILE* output);
/// Get the size of the binary the assembler would currently generate
static inline tx_uint32 tx_asm_assembler_get_binary_size(tx_asm_Assembler* as) { return as->position; }
/// Write the generated binary to a the buffer `rom_dest` (it must be at least `tx_asm_assembler_get_binary_size(as)` bytes long)
/// @returns true if the binary generation was successful, false otherwise
bool tx_asm_assembler_generate_binary(tx_asm_Assembler* as, tx_uint8* rom_dest);
/// Free all resources allocated by the assembler
void tx_asm_destroy_assembler(tx_asm_Assembler* as);
/// Print an error message with the current line number from lex
void tx_asm_error(tx_asm_Assembler* as, const char* format, ...);

/// Register a new label and return its id or the id of the already registered label with the same name
tx_uint32 tx_asm_assembler_handle_label(tx_asm_Assembler* as, const std::string& name);
/// Set the position of a registered label if it has not been set already. Returns the id of the label.
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* as, const std::string& name);
/// Get the position associated with the label which has the specified id.
tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* as, tx_uint32 id);
/// Convert all label IDs found in parameters of instructions in the instruction list to their corresponding absolute positions
void tx_asm_assembler_convert_labels(tx_asm_Assembler* as);

/// Add a parsed instruction to the instruction list of the assembler
void tx_asm_assembler_add_instruction(tx_asm_Assembler* as, tx_Instruction inst);

/// Pretty print the instruction list
void tx_asm_assembler_print_instructions(tx_asm_Assembler* as);
/// Pretty print all found labels and their positions
void tx_asm_assembler_print_labels(tx_asm_Assembler* as);
