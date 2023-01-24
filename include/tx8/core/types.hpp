/**
 * @file types.h
 * @brief Typedefs and unions used in the tx8 ecosystem for portability.
 * @details These types should be usd exclusively when writing tx8 sources instead of plain c types like int or char.
 */
#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace tx {
    using uint8   = uint8_t;
    using int8    = int8_t;
    using uint16  = uint16_t;
    using int16   = int16_t;
    using uint32  = uint32_t;
    using int32   = int32_t;
    using float32 = float;

    using mem_addr = uint32;
    using mem_ptr  = uint8*;

    using num32 = union num32 {
        uint32  u;
        int32   i;
        float32 f;
    };
    using num32_variant = std::variant<tx::uint32, tx::int32, tx::float32>;

    using Rom = std::vector<uint8>;
} // namespace tx
