#include "tx8/asm/parser.hpp"

tx::AST tx::Parser::get_ast() { return ast; }

void tx::Parser::parse() { lexer.next_token(); }

std::ostream& operator<<(std::ostream& os, const tx::ParserNode& node) {
    std::visit([&os](auto&& arg) { os << arg; }, node);
    return os;
}

std::ostream& operator<<(std::ostream& os, const tx::AST& ast) {
    for (const auto& node : ast) { os << node << std::endl; }
    return os;
}
