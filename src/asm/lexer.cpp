#include "tx8/asm/lexer.hpp"

tx::Lexer::LexerToken tx::Lexer::next_token() { return tx::lexer::token::EndOfFile {}; }

#define o0(t, name) \
    std::ostream& operator<<(std::ostream& os, [[maybe_unused]] const tx::lexer::token::t& _token) { \
        os << (name); \
        return os; \
    }
#define o1(t, name, x) \
    std::ostream& operator<<(std::ostream& os, const tx::lexer::token::t& token) { \
        os << "( " << token.x << " )"; \
        return os; \
    }
#define o1t(t, name, x, table) \
    std::ostream& operator<<(std::ostream& os, const tx::lexer::token::t& token) { \
        os << "( " << (table)[(tx::uint32) token.x] << " )"; \
        return os; \
    }

o0(EndOfFile, "EOF");
o0(EndOfLine, "EOL");
o0(Space, "Space");
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

std::ostream& operator<<(std::ostream& os, const tx::Lexer::LexerToken& token) {
    std::visit([&os](auto&& arg) { os << arg; }, token);
    return os;
}
