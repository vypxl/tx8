/**
 * @file stdlib.h
 * @brief Everything related to the tx8 standard library which can be used by tx8 programs
 */
#pragma once

#include <tx8/core/cpu.hpp>

namespace tx::stdlib {
    /// Use this method to register the all standard library functions
    void use_stdlib(CPU& cpu);
} // namespace tx::stdlib
