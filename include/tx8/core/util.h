/**
 * @file util.h
 * @brief Various utilities used throughout the tx8 ecosystem
 * @details Currently only contains MIN and MAX macros, and a string hash function
 */
#pragma once

#include <khash.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Get the minimum of a or b (beware double evaluation)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
/// Get the maximum of a or b (beware double evaluation)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
// Get the sign of a
#define SGN(a) ((0 < (a)) - ((a) < 0))
/// Get the signum of the comparison between a and b (-1 if lt, 0 if eq, 1 if gt)
#define CMP(a, b) (((b) < (a)) - ((a) < (b)))

/// Calculate a hash value for a string
static inline tx_uint32 tx_str_hash(const char* s) {
    tx_uint32 h = (tx_uint32) *s;
    if (h)
        for (++s; *s; ++s) h = (h << 5) - h + (tx_uint32) *s;
    return h;
}

#ifdef __cplusplus
}
#endif
