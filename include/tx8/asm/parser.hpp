#pragma once

#include "tx8/asm/lexer.hpp"
#include "tx8/core/instruction.hpp"

#include <optional>
#include <variant>
#include <vector>

namespace tx {
    namespace ast {
        struct Label {
            std::string name;
        };
        using Parameter = std::variant<tx::Parameter, Label>;
        struct Instruction {
            tx::Opcode opcode;
            Parameter  p1;
            Parameter  p2;
        };
        struct Invalid { };
    } // namespace ast
    using ParserNode = std::variant<tx::ast::Instruction, tx::ast::Label, tx::ast::Invalid>;
    using AST        = std::vector<tx::ParserNode>;

    class Parser {
      public:
        explicit Parser(tx::Lexer& lexer) : lexer(lexer) {};
        void    parse();
        tx::AST get_ast();
        bool    has_error();

      private:
        tx::Lexer& lexer;
        tx::AST    ast;
        bool       error = false;

        std::optional<std::variant<tx::Parameter, tx::ast::Label>> read_parameter();
        std::optional<tx::ast::Instruction>                        read_instruction(tx::Opcode opcode);
    };
} // namespace tx

std::ostream& operator<<(std::ostream& os, const tx::ast::Label& label);
std::ostream& operator<<(std::ostream& os, const tx::ast::Instruction& inst);
std::ostream& operator<<(std::ostream& os, const tx::ast::Parameter& param);
std::ostream& operator<<(std::ostream& os, const tx::ast::Invalid& invalid);
std::ostream& operator<<(std::ostream& os, const tx::ParserNode& node);
std::ostream& operator<<(std::ostream& os, const tx::AST& ast);
