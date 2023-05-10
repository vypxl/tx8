/**
 * @file util.h
 * @brief Various utilities used throughout the tx8 ecosystem
 * @details Currently only contains MIN and MAX macros, and a string hash function
 */
#pragma once

#include "tx8/core/types.hpp"

#include <fmt/format.h>
#include <optional>
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
    /// Models the information stored in a tx8 rom header
    struct RomInfo {
        uint32      size;
        std::string name;
        std::string description;
    };

    /// Calculate a hash value for a string
    static inline uint32 str_hash(const std::string& str) {
        const char* s = str.c_str();
        uint32      h = (uint8) *s;
        if (h != 0)
            for (++s; *s; ++s) h = (h << 5) - h + (uint32) *s; // NOLINT
        return h;
    }

    /// Parse a tx8 rom header from a stream
    /// Returns `nullopt` if the header is invalid, e. g. the checksum is wrong or the magic bytes are invalid
    std::optional<RomInfo> parse_header(std::istream& stream);
    /// Builds the binary representation of a tx8 rom header from a `RomInfo` struct
    /// This automatically calculates the checksum
    /// Truncates names and descriptions that are longer than their maximum length (256 / 65536 bytes)
    std::vector<uint8> build_header(const RomInfo& info);
} // namespace tx

template <>
struct fmt::formatter<tx::RomInfo> : fmt::formatter<string_view> {
    template <typename FormatContext>
    auto format(const tx::RomInfo& info, FormatContext& ctx) {
        return formatter<string_view>::format(
            fmt::format("Rom(\n\tname: {}\n\tdescription: {}\n\tsize: {}\n)", info.name, info.description, info.size),
            ctx
        );
    }
};

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
