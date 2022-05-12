/**
 * @file debug.h
 * @brief Everything related to the integrated debugger.
 * @details Unless noted otherwise, all functions use `tx_log_err` for printing.
 */
#pragma once

#include "tx8/core/instruction.h"

#include <bits/types/FILE.h>
#include <tx8/core/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Print an instruction amongst its position in memory and its parameter values
void tx_debug_print_instruction(tx_CPU* cpu, tx_Instruction* inst);

/// Print an instruction to stdout amongst its position in memory and its parameters
void tx_debug_print_raw_instruction(tx_CPU* cpu, tx_Instruction* inst);

#ifdef __cplusplus
}
#endif
