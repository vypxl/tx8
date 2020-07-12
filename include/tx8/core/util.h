/**
 * @file util.h
 * @brief Various utilities used throughout the tx8 ecosystem
 * @details Currently only contains MIN and MAX macros, and a string hash function
 */
#pragma once

#include <khash.h>

/// Get the minimum of a or b (beware double evaluation)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
/// Get the maximum of a or b (beware double evaluation)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/// Calculate a hash value for a string
#define tx_str_hash(str) kh_str_hash_func(str)
