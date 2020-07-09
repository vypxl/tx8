/**
 * @file types.h
 * @brief Typedefs and unions used in the tx8 ecosystem for portability.
 * @details These types should be usd exclusively when writing tx8 sources instead of plain c types like int or char.
 */
#pragma once

typedef unsigned char  tx_uint8;
typedef signed char    tx_int8;
typedef unsigned short tx_uint16;
typedef signed short   tx_int16;
typedef unsigned int   tx_uint32;
typedef signed int     tx_int32;
typedef float          tx_float32;

typedef tx_uint32 tx_mem_addr;
typedef tx_uint8* tx_mem_ptr;

// Union for easy conversion between uint, int and float
typedef union tx_num32 {
    tx_uint32  u;
    tx_int32   i;
    tx_float32 f;
} tx_num32;
