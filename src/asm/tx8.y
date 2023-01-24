%skeleton "lalr1.cc" /* C++ */
%require "3.0"

%define api.namespace { tx::parser }
%define api.parser.class { Parser }
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include <tx8/core/instruction.hpp>
    #include <tx8/core/util.hpp>
    #include <sstream>

    namespace tx {
        class Assembler;
        namespace parser {
            class Lexer;
        }
    }
}

%code top {
#include <tx8/asm/assembler.hpp>
#include <tx8/asm/lexer.hpp>

#include <stdio.h>

extern int tx_lex_yylineno;
// extern tx::Assembler* tx_asm_yyasm;

extern int tx_asm_yyerror(const char* str);
#define yylex(x, y) lexer.get_next_token()
}

%lex-param { tx::parser::Lexer& lexer }
%lex-param { tx::Assembler& as }
%parse-param { tx::parser::Lexer& lexer }
%parse-param { tx::Assembler& as }

%locations
%define parse.trace
%define parse.error verbose

%start program

%token END 0 "end of file"
%token EOL "end of line"
%token SPACE "space"
%token <tx::uint32> REGISTER "register" REGISTER_ADDRESS "register address" ABSOLUTE_ADDRESS "absolute address" RELATIVE_ADDRESS "relative address" INTEGER8 "int8" INTEGER16 "int16" INTEGER32 "int32" OPCODE0 "opcode 0" OPCODE1 "opcode 1" OPCODE2 "opcode 2"
%token <tx::float32> FLOAT "float constant"
%token <std::string> LABEL "label" ALIAS "sysfunc name" INVALID "invalid token"

%type <Parameter> parameter
%type <Instruction> instruction

%%
program:     %empty
           | program SPACE
           | program EOL
           | program statement
statement:   LABEL SPACE instruction EOL             { as.handle_label($1); as.set_label_position($1); as.add_instruction($3); }
           | LABEL EOL                               { as.handle_label($1); as.set_label_position($1); }
           | instruction EOL                         { as.add_instruction($1); }
           | error EOL                               { }

instruction: OPCODE2 SPACE parameter SPACE parameter { Instruction i = { .opcode = (Opcode) $1, .params = { .p1 = $3, .p2 = $5 } }; $$ = i; }
           | OPCODE1 SPACE parameter                 { Instruction i = { .opcode = (Opcode) $1, .params = { .p1 = $3 } }; $$ = i; }
           | OPCODE0                                 { Instruction i = { .opcode = (Opcode) $1 }; $$ = i; }

parameter:   ALIAS                                   { Parameter p = { .value = { .u = tx::str_hash($1) }, .mode = tx::ParamMode::Constant32 }; $$ = p; }
           | INTEGER8                                { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::Constant8 }; $$ = p; }
           | INTEGER16                               { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::Constant16 }; $$ = p; }
           | INTEGER32                               { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::Constant32 }; $$ = p; }
           | FLOAT                                   { Parameter p = { .value = { .f = $1 }, .mode = tx::ParamMode::Constant32 }; $$ = p; }
           | ABSOLUTE_ADDRESS                        { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::AbsoluteAddress }; $$ = p; }
           | RELATIVE_ADDRESS                        { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::RelativeAddress }; $$ = p; }
           | REGISTER                                { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::Register }; $$ = p; }
           | REGISTER_ADDRESS                        { Parameter p = { .value = { .u = $1 }, .mode = tx::ParamMode::RegisterAddress }; $$ = p; }
           | LABEL                                   { Parameter p = { .value = { .u = as.handle_label($1) }, .mode = tx::ParamMode::Label }; $$ = p; }
%%

void tx::parser::Parser::error(const tx::parser::location& loc, const std::string& str) {
    std::stringstream ss;
    ss << loc;
    as.report_error("Line {}: {}\n", ss.str(), str);
}
