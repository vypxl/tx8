/**
 * @file stdlib.h
 * @brief Everything related to the tx8 standard library which can be used by tx8 programs
 */
#pragma once

#include <tx8/core/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Use this method to register the all standard library functions
void tx_cpu_use_stdlib(tx_CPU* cpu);

#ifdef __cplusplus
}
#endif
