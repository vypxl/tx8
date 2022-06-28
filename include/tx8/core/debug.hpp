/**
 * @file debug.h
 * @brief Everything related to the integrated debugger.
 * @details Unless noted otherwise, all functions use `tx::log_err` for printing.
 */
#pragma once

#include "tx8/core/instruction.h"
#include "tx8/core/log.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/// Pretty print a parameter
void tx_debug_print_parameter(tx_Parameter* p);
/// Print an instruction amongst its parameter values
void tx_debug_print_instruction(tx_Instruction* inst);

/// Print an instruction to stdout amongst and its parameters in their uninterpreted raw form and their modes
void tx_debug_print_raw_instruction(tx_Instruction* inst);

/// Prints the current program counter without at newline
/// To be used before other debug functions
#define tx_debug_print_pc(cpu) tx::log_err("[#{:x}] ", cpu->p);

#ifdef __cplusplus
}
#endif
