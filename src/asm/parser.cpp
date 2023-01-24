#include "tx8/asm/parser.hpp"

tx::AST tx::Parser::get_ast() { return ast; }

void tx::Parser::parse() { lexer.next_token(); }
