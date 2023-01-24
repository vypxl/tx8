#pragma once

#include "tx8/core/instruction.hpp"

#include <istream>
#include <memory>

namespace tx {
    namespace lexer::token {
        struct EndOfFile { };
        struct EndOfLine { };
        struct Space { };
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
        struct Integer8 {
            tx::uint8 value;
        };
        struct Integer16 {
            tx::uint16 value;
        };
        struct Integer32 {
            tx::uint32 value;
        };
        struct Opcode0 {
            tx::Opcode opcode;
        };
        struct Opcode1 {
            tx::Opcode opcode;
        };
        struct Opcode2 {
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
            tx::lexer::token::EndOfFile,
            tx::lexer::token::EndOfLine,
            tx::lexer::token::Space,
            tx::lexer::token::Register,
            tx::lexer::token::RegisterAddress,
            tx::lexer::token::AbsoluteAddress,
            tx::lexer::token::RelativeAddress,
            tx::lexer::token::Integer8,
            tx::lexer::token::Integer16,
            tx::lexer::token::Integer32,
            tx::lexer::token::Opcode0,
            tx::lexer::token::Opcode1,
            tx::lexer::token::Opcode2,
            tx::lexer::token::Float,
            tx::lexer::token::Label,
            tx::lexer::token::Alias,
            tx::lexer::token::Invalid>;
        explicit Lexer(std::istream& input) : is(input) {};

        LexerToken next_token();

      private:
        std::istream& is;
    };
} // namespace tx

#define o(t) std::ostream& operator<<(std::ostream& os, const tx::lexer::token::t& token)

o(EndOfFile);
o(EndOfLine);
o(Space);
o(Register);
o(RegisterAddress);
o(AbsoluteAddress);
o(RelativeAddress);
o(Integer8);
o(Integer16);
o(Integer32);
o(Opcode0);
o(Opcode1);
o(Opcode2);
o(Float);
o(Label);
o(Alias);
o(Invalid);

#undef o

std::ostream& operator<<(std::ostream& os, const tx::Lexer::LexerToken& token);
