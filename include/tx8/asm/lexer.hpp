#pragma once

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer tx_lex_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL tx::parser::Parser::symbol_type tx::parser::Lexer::get_next_token()

#include "tx8_parser.hpp"

namespace tx::parser {
    class Lexer : public yyFlexLexer {
      public:
        Lexer()          = default;
        virtual ~Lexer() = default;
        virtual tx::parser::Parser::symbol_type get_next_token();
    };
} // namespace tx::parser
