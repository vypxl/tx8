#pragma once

#include "tx8/core/instruction.hpp"

#include <fmt/format.h>
#include <istream>
#include <memory>
#include <optional>

namespace tx {
    namespace lexer::token {
        struct EndOfLine { };
        struct Register {
            tx::Register which;
        };
        struct RegisterAddress {
            tx::Register which;
        };
        struct AbsoluteAddress {
            tx::uint32 address;
        };
        struct RelativeAddress {
            tx::uint32 address;
        };
        struct Integer {
            tx::uint32    value;
            tx::ValueSize size;
        };
        struct Opcode {
            tx::Opcode opcode;
        };
        struct Float {
            tx::float32 value;
        };
        struct Label {
            std::string name;
        };
        struct Alias {
            std::string name;
        };
        struct Invalid {
            std::string value;
        };
    } // namespace lexer::token

    class Lexer {
      public:
        using LexerToken = std::variant<
            tx::lexer::token::EndOfLine,
            tx::lexer::token::Register,
            tx::lexer::token::RegisterAddress,
            tx::lexer::token::AbsoluteAddress,
            tx::lexer::token::RelativeAddress,
            tx::lexer::token::Integer,
            tx::lexer::token::Opcode,
            tx::lexer::token::Float,
            tx::lexer::token::Label,
            tx::lexer::token::Alias,
            tx::lexer::token::Invalid>;

        explicit Lexer(std::istream& input) : is(input) {};

        std::optional<LexerToken> next_token();

      private:
        std::istream& is;

        void readSpace();
    };
} // namespace tx

#define FORMATTER_BEGIN(t, name) \
    template <> \
    struct fmt::formatter<t> : fmt::formatter<std::string_view> { \
        template <typename FormatContext> \
        auto format([[maybe_unused]] const t& /* NOLINT */ name, FormatContext& ctx) {
#define FORMATTER_END \
    } \
    } \
    ;

#define o0(t, name) \
    FORMATTER_BEGIN(t, token) \
        return formatter<std::string_view>::format(fmt::format("{}", (name)), ctx); \
    FORMATTER_END
#define o1(t, name, x) \
    FORMATTER_BEGIN(t, token) \
        return formatter<std::string_view>::format(fmt::format("{} ( {} )", (name), token.x), ctx); \
    FORMATTER_END
#define o1t(t, name, x, table) \
    FORMATTER_BEGIN(t, token) \
        return formatter<std::string_view>::format( \
            fmt::format("{} ( {} )", (name), (table)[(tx::uint32) token.x]), \
            ctx \
        ); \
    FORMATTER_END

o0(tx::lexer::token::EndOfLine, "EOL");
o1t(tx::lexer::token::Register, "Register", which, tx::reg_names);
o1t(tx::lexer::token::RegisterAddress, "RegisterAddress", which, tx::reg_names);
o1(tx::lexer::token::AbsoluteAddress, "AbsoluteAddress", address);
o1(tx::lexer::token::RelativeAddress, "RelativeAddress", address);
o1(tx::lexer::token::Integer, "Integer", value);
o1t(tx::lexer::token::Opcode, "Opcode", opcode, tx::op_names);
o1(tx::lexer::token::Float, "Float", value);
o1(tx::lexer::token::Label, "Label", name);
o1(tx::lexer::token::Alias, "Alias", name);
o1(tx::lexer::token::Invalid, "Invalid", value);

FORMATTER_BEGIN(tx::Lexer::LexerToken, token)
    std::string s;
    std::visit([&s](auto&& arg) { s = fmt::format("{}", arg); }, token);
    return formatter<std::string_view>::format(s, ctx);
FORMATTER_END

#undef o0
#undef o1
#undef o1t
#undef formatter_begin
