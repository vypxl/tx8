#include "tx8/asm/parser.hpp"

#include "tx8/core/log.hpp"
#include "tx8/core/util.hpp"

using std::holds_alternative;
using LexerToken = tx::Lexer::LexerToken;
using namespace tx::lexer::token;
using LabelT         = tx::lexer::token::Label;
using OpcodeT        = tx::lexer::token::Opcode;
using RegisterT      = tx::lexer::token::Register;
using LabelAst       = tx::ast::Label;
using InstructionAst = tx::ast::Instruction;
using ParamAst       = tx::ast::Parameter;

tx::AST tx::Parser::get_ast() { return ast; }

bool tx::Parser::has_error() { return error; }

std::optional<std::variant<tx::Parameter, LabelAst>> tx::Parser::read_parameter() {
    std::optional<LexerToken> token;

    token = lexer.next_token();
    if (token.has_value()) {
        if (holds_alternative<RegisterT>(*token)) {
            return tx::Parameter {
                .value = {(tx::uint32) std::get<RegisterT>(*token).which},
                .mode  = tx::ParamMode::Register};
        }
        if (holds_alternative<RegisterAddress>(*token)) {
            return tx::Parameter {
                .value = {(tx::uint32) std::get<RegisterAddress>(*token).which},
                .mode  = tx::ParamMode::RegisterAddress};
        }
        if (holds_alternative<AbsoluteAddress>(*token)) {
            return tx::Parameter {
                .value = {(tx::uint32) std::get<AbsoluteAddress>(*token).address},
                .mode  = tx::ParamMode::AbsoluteAddress};
        }
        if (holds_alternative<RelativeAddress>(*token)) {
            return tx::Parameter {
                .value = {(tx::uint32) std::get<RelativeAddress>(*token).address},
                .mode  = tx::ParamMode::RelativeAddress};
        }
        if (holds_alternative<Integer>(*token)) {
            auto          i = std::get<Integer>(*token);
            tx::Parameter p {.value = {(tx::uint32) i.value}, .mode = tx::ParamMode::Constant32};
            if (i.size == tx::ValueSize::Byte) {
                p.mode = tx::ParamMode::Constant8;
            } else if (i.size == tx::ValueSize::Short) {
                p.mode = tx::ParamMode::Constant16;
            }
            return p;
        }
        if (holds_alternative<Float>(*token)) {
            tx::num32 n = {.f = std::get<Float>(*token).value};
            return tx::Parameter {.value = {n.u}, .mode = tx::ParamMode::Constant32};
        }
        if (holds_alternative<Alias>(*token)) {
            return tx::Parameter {
                .value = {tx::str_hash(std::get<Alias>(*token).name)},
                .mode  = tx::ParamMode::Constant32};
        }
        if (holds_alternative<LabelT>(*token)) { return LabelAst {std::get<LabelT>(*token).name}; }

        tx::log_err("Expected parameter, not {}\n", *token);
        error = true;
    } else {
        tx::log_err("Expected parameter, got EOF");
        error = true;
    }

    return std::nullopt;
}

std::optional<tx::ast::Instruction> tx::Parser::read_instruction(tx::Opcode opcode) {
    std::optional<LexerToken> token;

    InstructionAst instruction {
        .opcode = opcode,
        .p1     = tx::Parameter {.value = {0}, .mode = tx::ParamMode::Unused},
        .p2     = tx::Parameter {.value = {0}, .mode = tx::ParamMode::Unused}};
    std::optional<ParamAst> p;

    switch (tx::param_count[(tx::uint32) opcode]) {
        case 1:
            p = read_parameter();
            if (p.has_value()) {
                instruction.p1 = p.value();
            } else {
                return std::nullopt;
            }
            break;
        case 2:
            p = read_parameter();
            if (p.has_value()) {
                instruction.p1 = p.value();
            } else {
                return std::nullopt;
            }
            p = read_parameter();
            if (p.has_value()) {
                instruction.p2 = p.value();
            } else {
                return std::nullopt;
            }
            break;
    }
    token = lexer.next_token();
    if (token.has_value() && !holds_alternative<EndOfLine>(*token)) {
        tx::log_err("Expected EOL, got {}\n", *token);
        error = true;
        return std::nullopt;
    }

    return instruction;
}

void tx::Parser::parse() {
    std::optional<LexerToken> token;

    while (true) {
        token = lexer.next_token();
        if (token.has_value()) {
            if (holds_alternative<OpcodeT>(*token)) {
                auto instruction = read_instruction(std::get<OpcodeT>(*token).opcode);
                if (instruction.has_value()) {
                    ast.push_back(instruction.value());
                } else {
                    tx::log_err("Expected instruction\n");
                    error = true;
                }
            } else if (holds_alternative<LabelT>(*token)) {
                LabelAst label {
                    std::get<LabelT>(*token).name,
                };
                ast.push_back(label);
            } else if (holds_alternative<EndOfLine>(*token)) {
            } else {
                tx::log_err("Expected instruction or label, got {}\n", *token);
                error = true;
            }
        } else {
            break;
        }
    }

    if (tx::log_debug.is_enabled()) {
        for (const auto& node : ast) { tx::log_debug("[parser] {}\n", node); }
    }
}
