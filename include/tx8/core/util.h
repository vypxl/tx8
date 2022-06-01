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
#define tx_str_hash(str) kh_str_hash_func(str)

#ifdef __cplusplus
}
#endif
