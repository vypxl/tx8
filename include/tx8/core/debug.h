/**
 * @file debug.h
 * @brief Everything related to the integrated debugger.
 * @details Currently only contains print_instruction method
 */
#pragma once

#include "tx8/core/instruction.h"

#include <bits/types/FILE.h>
#include <tx8/core/cpu.h>

/// Print an instruction to stdout amongst its position in memory and its parameter values
void tx_debug_print_instruction(tx_CPU* cpu, tx_Instruction* inst);
/// Print an instruction to the specified file amongst its position in memory and its parameter values
void tx_debug_fprint_instruction(tx_CPU* cpu, tx_Instruction* inst, FILE* f);

/// Print an instruction to stdout amongst its position in memory and its parameters
void tx_debug_print_raw_instruction(tx_CPU* cpu, tx_Instruction* inst);
/// Print an instruction to the specified file amongst its position in memory and its parameters
void tx_debug_fprint_raw_instruction(tx_CPU* cpu, tx_Instruction* inst, FILE* f);
