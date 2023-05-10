#include "tx8/asm/lexer.hpp"

#include "fmt/format.h"
#include "tx8/core/log.hpp"

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

optional<LexerToken> readInteger(const string& raw) { // NOLINT
    if (raw.empty()) return std::nullopt;
    std::string s = raw;

    bool neg = s[0] == '-';
    if (neg) s = s.substr(1);

    int base = 10; // NOLINT
    if (s[1] == 'x') {
        base = 16; // NOLINT
        s    = s.substr(2);
    } else if (s[1] == 'b') {
        base = 2;
        s    = s.substr(2);
    }

    size_t    pos;
    long long i;
    try {
        i = std::stoll(s, &pos, base);
    } catch (const std::invalid_argument& e) { return std::nullopt; } catch (const std::out_of_range& e) {
        return std::nullopt;
    }
    if (neg) i = -i;
    tx::uint32 value = i;

#define LIM(type) std::numeric_limits<type>
#define CASE(suffix, type, size) \
    if (rest == #suffix) { \
        if (i < LIM(tx::type)::min() || i > LIM(tx::type)::max()) return std::nullopt; \
        return Integer {value, tx::ValueSize::size}; \
    }

    if (pos != s.size()) {
        std::string rest = s.substr(pos);

        CASE(i8, int8, Byte)
        CASE(i16, int16, Short)
        CASE(i32, int32, Word)
        CASE(u8, uint8, Byte)
        CASE(u16, uint16, Short)
        CASE(u32, uint32, Word)

        return std::nullopt;
    }

    if (i < LIM(tx::int32)::min() || i > LIM(tx::uint32)::max()) return std::nullopt;
    return Integer {value, tx::ValueSize::Word};

#undef LIM
#undef CASE
}

optional<tx::uint32> readAddress(const string& s, bool allow_negative = false) {
    size_t    pos;
    long long i;
    try {
        i = std::stoll(s, &pos, 16); // NOLINT
    } catch (const std::invalid_argument&) { return std::nullopt; } catch (const std::out_of_range&) {
        return std::nullopt;
    }

    if (pos != s.size()) return std::nullopt;
    if (i > tx::MAX_MEMORY_ADDRESS || i < -((long long) tx::MAX_MEMORY_ADDRESS)) return std::nullopt;
    if (i < 0 && !allow_negative) return std::nullopt;
    return i;
}

optional<LexerToken> readFloat(const string& s) {
    if (s.empty()) return std::nullopt;
    size_t      pos;
    tx::float32 val;
    try {
        val = std::stof(s, &pos);
    } catch (const std::invalid_argument&) { return std::nullopt; } catch (const std::out_of_range&) {
        return std::nullopt;
    }
    if (pos != s.size()) return std::nullopt;
    return Float {val};
}

optional<LexerToken> readString(std::istream& is) {
    std::string result;
    char        c;

    // Read string until terminating ", while allowing escaped characters
    while (is.get(c) && c != '"') {
        if (c == '\\') {
            if (!is.get(c)) { return std::nullopt; }
            switch (c) {
                case '"':
                case '\\': result += c; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += '\\'; result += c;
            }
        } else {
            result += c;
        }
    }

    if (is.fail()) { return std::nullopt; }

    return StringT {result};
}

optional<LexerToken> Lexer::next_token() {
    readSpace();
    int c = is.peek();
    if (is.eof() || is.bad()) {
        tx::log_debug("[lexer] eof or read error\n");
        return std::nullopt;
    }

    optional<LexerToken> token = std::nullopt;

    if (c == '\n') {
        is.get();
        tx::log_debug("[lexer] got token: {}\n", EndOfLine {});
        return EndOfLine {};
    }


    string s;
    is >> s;
    // this handles the case where the comment starts immediately after a token, e. g. `lda 0; comment`
    // compare with `lda 0 ; comment` (note the space)
    if (s.contains(';') && !(s[0] == '"')) {
        s = s.substr(0, s.find(';'));
        is.putback(';');
    }

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
        case '"':
            s.erase(s.begin());
            for ([[maybe_unused]] char c : s) { is.unget(); }
            token = readString(is);
            break;
        default:
            // clang-format off
            token = readOpcode(s)
                .or_else([&]() { return readRegister(s); })
                .or_else([&]() { return readInteger(s); })
                .or_else([&]() { return readFloat(s); });
            // clang-format on
    }

    LexerToken tok = token.value_or(Invalid {s});
    tx::log_debug("[lexer] got token: {}\n", tok);
    return tok;
}
