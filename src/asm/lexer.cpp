#include "tx8/asm/lexer.hpp"

#include "fmt/format.h"

#include <iostream>
#include <limits>

using std::optional;
using tx::Lexer;
using namespace tx::lexer::token;
using LexerToken = tx::Lexer::LexerToken;
using LabelT     = tx::lexer::token::Label;
using RegisterT  = tx::lexer::token::Register;

void Lexer::readSpace() {
    char c;
    while (is.get(c), !is.fail() && (c == ' ' || c == '\t')) { }

    if (c == ';') is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (is.peek() != std::char_traits<char>::eof()) is.unget();
}

LexerToken Lexer::readInvalid() {
    std::string s;
    is >> s;
    return Invalid {s};
}

optional<LexerToken> Lexer::next_token() {
    readSpace();
    int c = is.peek();
    if (is.eof() || is.bad()) return std::nullopt;
    if (c == '\n') {
        is.get();
        return EndOfLine {};
    }

    optional<LexerToken> token = std::nullopt;
    std::string          s;
    is >> s;

    switch (c) {
        case ':':
            s.erase(s.begin());
            token = readIdentifier().transform([](const auto& s) { return LabelT {s}; });
            break;
        case '&':
            s.erase(s.begin());
            token = readIdentifier().transform([](const auto& s) { return Alias {s}; });
            break;
        case '@':
            s.erase(s.begin());
            token = readRegister().transform([](const auto& r) { return RegisterAddress {r}; });
            break;
        case '#':
            s.erase(s.begin());
            token = readAddress().transform([](const auto& r) { return AbsoluteAddress {r}; });
            break;
        case '$':
            s.erase(s.begin());
            token = readAddress().transform([](const auto& r) { return RelativeAddress {r}; });
            break;
        default:
            token = readOpcode0()
                        .or_else([&]() { return readOpcode1(); })
                        .or_else([&]() { return readOpcode2(); })
                        .or_else([&]() {
                            return readRegister().transform([](const auto& r) { return LexerToken {RegisterT {r}}; });
                        })
                        .or_else([&]() { return readInteger8(); })
                        .or_else([&]() { return readInteger16(); })
                        .or_else([&]() { return readInteger32(); })
                        .or_else([&]() { return readFloat(); });
    }

    return token.value_or(readInvalid());
}

#define o0(t, name) \
    std::ostream& operator<<(std::ostream& os, [[maybe_unused]] const t& _token) { \
        os << (name); \
        return os; \
    }
#define o1(t, name, x) \
    std::ostream& operator<<(std::ostream& os, const t& token) { \
        os << (name) << " ( " << token.x << " )"; \
        return os; \
    }
#define o1t(t, name, x, table) \
    std::ostream& operator<<(std::ostream& os, const t& token) { \
        os << (name) << "( " << (table)[(tx::uint32) token.x] << " )"; \
        return os; \
    }

o0(EndOfLine, "EOL");
o1t(Register, "Register", which, tx::reg_names);
o1t(RegisterAddress, "RegisterAddress", which, tx::reg_names);
o1(AbsoluteAddress, "AbsoluteAddress", address);
o1(RelativeAddress, "RelativeAddress", address);
o1(Integer8, "Integer8", value);
o1(Integer16, "Integer16", value);
o1(Integer32, "Integer32", value);
o1t(Opcode0, "Opcode0", opcode, tx::op_names);
o1t(Opcode1, "Opcode1", opcode, tx::op_names);
o1t(Opcode2, "Opcode2", opcode, tx::op_names);
o1(Float, "Float", value);
o1(Label, "Label", name);
o1(Alias, "Alias", name);
o1(Invalid, "Invalid", value);

#undef o0
#undef o1
#undef o1t

std::ostream& operator<<(std::ostream& os, const LexerToken& token) {
    std::visit([&os](auto&& arg) { os << arg; }, token);
    return os;
}
