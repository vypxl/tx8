#pragma once

#include "tx8/asm/lexer.hpp"
#include "tx8/core/instruction.hpp"

#include <fmt/format.h>
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

#define FORMATTER_BEGIN(t, name) \
    template <> \
    struct fmt::formatter<t> : fmt::formatter<std::string_view> { \
        template <typename FormatContext> \
        auto format([[maybe_unused]] const t& /* NOLINT */ name, FormatContext& ctx) {
#define FORMATTER_END \
    } \
    } \
    ;

FORMATTER_BEGIN(tx::ast::Label, label)
    return formatter<string_view>::format(fmt::format("Label '{}'", label.name), ctx);
FORMATTER_END

FORMATTER_BEGIN(tx::ast::Parameter, token)
    std::string s;
    std::visit([&s](auto&& arg) { s = fmt::format("{}", arg); }, token);
    return formatter<std::string_view>::format(s, ctx);
FORMATTER_END

FORMATTER_BEGIN(tx::ast::Instruction, inst)
    return formatter<string_view>::format(fmt::format("Instruction {} {} {}", inst.opcode, inst.p1, inst.p2), ctx);
FORMATTER_END

FORMATTER_BEGIN(tx::ast::Invalid, invalid)
    return formatter<string_view>::format("Invalid", ctx);
FORMATTER_END

FORMATTER_BEGIN(tx::ParserNode, node)
    std::string s;
    std::visit([&s](auto&& arg) { s = fmt::format("{}", arg); }, node);
    return formatter<std::string_view>::format(s, ctx);
FORMATTER_END
