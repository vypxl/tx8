#pragma once

#include "tx8/core/instruction.hpp"

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

#define o(t) std::ostream& operator<<(std::ostream& os, const tx::lexer::token::t& token)

o(EndOfLine);
o(Register);
o(RegisterAddress);
o(AbsoluteAddress);
o(RelativeAddress);
o(Integer);
o(Opcode);
o(Float);
o(Label);
o(Alias);
o(Invalid);

#undef o

std::ostream& operator<<(std::ostream& os, const tx::Lexer::LexerToken& token);
