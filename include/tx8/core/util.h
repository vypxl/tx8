/**
 * @file util.h
 * @brief Various utilities used throughout the tx8 ecosystem
 * @details Currently only contains MIN and MAX macros, and a string hash function
 */
#pragma once

#include <khash.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define tx_str_hash(str) kh_str_hash_func(str)
