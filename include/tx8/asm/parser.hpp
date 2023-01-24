#pragma once

#include "tx8/asm/lexer.hpp"
#include "tx8/core/instruction.hpp"

#include <vector>

namespace tx {
    using ParserNode = std::variant<tx::Instruction, tx::Label>;
    using AST        = std::vector<tx::ParserNode>;

    class Parser {
      public:
        explicit Parser(tx::Lexer& lexer) : lexer(lexer) {};
        void    parse();
        tx::AST get_ast();

      private:
        tx::Lexer& lexer;
        tx::AST    ast;
    };
} // namespace tx
