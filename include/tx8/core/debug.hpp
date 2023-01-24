/**
 * @file debug.h
 * @brief Everything related to the integrated debugger.
 * @details Unless noted otherwise, all functions use `tx::log_err` for printing.
 */
#pragma once

#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"

namespace tx::debug {
    /// Pretty print a parameter
    void print_parameter(const Parameter& p);
    /// Print an instruction amongst its parameter values
    void print_instruction(const Instruction& inst);
} // namespace tx::debug
