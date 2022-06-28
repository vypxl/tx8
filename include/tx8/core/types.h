/**
 * @file types.h
 * @brief Typedefs and unions used in the tx8 ecosystem for portability.
 * @details These types should be usd exclusively when writing tx8 sources instead of plain c types like int or char.
 */
#pragma once

#include <stdint.h>

typedef uint8_t  tx_uint8;
typedef int8_t   tx_int8;
typedef uint16_t tx_uint16;
typedef int16_t  tx_int16;
typedef uint32_t tx_uint32;
typedef int32_t  tx_int32;
typedef float    tx_float32;

typedef tx_uint32 tx_mem_addr;
typedef tx_uint8* tx_mem_ptr;

/// Union for easy conversion between uint, int and float
typedef union tx_num32 {
    /// The unsigned 32bit integer value
    tx_uint32 u;
    /// The signed 32bit integer value
    tx_int32 i;
    /// The 32bit floating point value
    tx_float32 f;
} tx_num32;
