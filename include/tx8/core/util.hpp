/**
 * @file util.h
 * @brief Various utilities used throughout the tx8 ecosystem
 * @details Currently only contains MIN and MAX macros, and a string hash function
 */
#pragma once

#include "tx8/core/types.hpp"

#include <string>

/// Get the minimum of a or b (beware double evaluation)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
/// Get the maximum of a or b (beware double evaluation)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
// Get the sign of a
#define SGN(a) ((0 < (a)) - ((a) < 0))
/// Get the signum of the comparison between a and b (-1 if lt, 0 if eq, 1 if gt)
#define CMP(a, b) (((b) < (a)) - ((a) < (b)))

namespace tx {
    /// Calculate a hash value for a string
    static inline uint32 str_hash(const std::string& str) {
        const char* s = str.c_str();
        uint32      h = (uint8) *s;
        if (h != 0)
            for (++s; *s; ++s) h = (h << 5) - h + (uint32) *s; // NOLINT
        return h;
    }
} // namespace tx
