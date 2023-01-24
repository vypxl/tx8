#include "tx8/asm/lexer.hpp"

#include "fmt/format.h"

#include <iostream>
#include <limits>

using std::optional;
using std::string;
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

optional<tx::Register> readReg(const string& s) {
    tx::Register r = tx::reg_id_from_name(s);
    if (r != tx::Register::Invalid) return r;
    return std::nullopt;
}

optional<LexerToken> readRegister(const string& s) {
    auto r = readReg(s);
    if (r.has_value()) return Register {*r};
    return std::nullopt;
}

optional<LexerToken> readOpcode(const string& s) {
    tx::Opcode o = tx::opcode_from_name(s);
    if (o != tx::Opcode::Invalid) return Opcode {o};
    return std::nullopt;
}

optional<std::string> readIdentifier(const string& s) {
    if (s.empty()) return std::nullopt;
    if (std::isalpha(s[0]) == 0) return std::nullopt;
    for (const auto& c : s) {
        if ((std::isalnum(c) == 0) && c != '_' && c != '-') return std::nullopt;
    }
    return s;
}

// TODO: Implement
optional<LexerToken> readInteger(const string& s) {
    if (s.empty()) return std::nullopt;
    return std::nullopt;
}

optional<tx::uint32> readAddress(const string& s, bool allow_negative = false) {
    size_t    pos;
    long long i;
    try {
        i = std::stoll(s, &pos, 16); // NOLINT
    } catch (const std::invalid_argument&) { return std::nullopt; } catch (const std::out_of_range&) {
        return std::nullopt;
    }

    if (s.c_str()[pos] != 0) return std::nullopt;
    if (i > tx::MAX_MEMORY_ADDRESS || i < -((long long) tx::MAX_MEMORY_ADDRESS)) return std::nullopt;
    if (i < 0 && !allow_negative) return std::nullopt;
    return i;
}

// TODO: Implement
optional<LexerToken> readFloat(const string& s) {
    if (s.empty()) return std::nullopt;
    return std::nullopt;
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
    string               s;
    is >> s;

    switch (c) {
        case ':':
            s.erase(s.begin());
            token = readIdentifier(s).transform([](const auto& s) { return LabelT {s}; });
            break;
        case '&':
            s.erase(s.begin());
            token = readIdentifier(s).transform([](const auto& s) { return Alias {s}; });
            break;
        case '@':
            s.erase(s.begin());
            token = readReg(s).transform([](const auto& r) { return RegisterAddress {r}; });
            break;
        case '#':
            s.erase(s.begin());
            token = readAddress(s).transform([](const auto& r) { return AbsoluteAddress {r}; });
            break;
        case '$':
            s.erase(s.begin());
            token = readAddress(s, true).transform([](const auto& r) { return RelativeAddress {r}; });
            break;
        default:
            // clang-format off
            token = readOpcode(s)
                .or_else([&]() { return readRegister(s); })
                .or_else([&]() { return readInteger(s); })
                .or_else([&]() { return readFloat(s); });
            // clang-format on
    }

    return token.value_or(Invalid {s});
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
o1(Integer, "Integer", value);
o1t(Opcode, "Opcode", opcode, tx::op_names);
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
