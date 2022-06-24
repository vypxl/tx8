/**
 * @file types.h
 * @brief Typedefs and unions used in the tx8 ecosystem for portability.
 * @details These types should be usd exclusively when writing tx8 sources instead of plain c types like int or char.
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#define tx_MAKE_ARRAY_TYPE(type, name) \
    typedef struct tx_Array_##name { \
        type*  data; \
        size_t size; \
        size_t front; \
    } tx_Array_##name; \
\
    static inline void tx_array_##name##_init(tx_Array_##name* a, tx_uint32 size) { \
        a->data  = (type*) malloc(size * sizeof(type)); \
        a->front = 0; \
        a->size  = size; \
    } \
\
    static inline void tx_array_##name##_insert(tx_Array_##name* a, type element) { \
        if (a->front == a->size) { \
            a->size *= 2; \
            a->data = (type*) realloc(a->data, a->size * sizeof(type)); \
        } \
        a->data[a->front++] = element; \
    } \
\
    static inline void tx_array_##name##_destroy(tx_Array_##name* a) { \
        free(a->data); \
        a->data  = NULL; \
        a->front = a->size = 0; \
    }

#ifdef __cplusplus
}
#endif
