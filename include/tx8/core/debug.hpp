/**
 * @file debug.h
 * @brief Everything related to the integrated debugger.
 * @details Unless noted otherwise, all functions use `tx::log_err` for printing.
 */
#pragma once

#include "tx8/core/instruction.h"
#include "tx8/core/log.hpp"

namespace tx::debug {
    /// Pretty print a parameter
    void print_parameter(const tx_Parameter& p);
    /// Print an instruction amongst its parameter values
    void print_instruction(const tx_Instruction& inst);

    /// Print an instruction to stdout amongst and its parameters in their uninterpreted raw form and their modes
    void print_raw_instruction(const tx_Instruction& inst);
} // namespace tx::debug
