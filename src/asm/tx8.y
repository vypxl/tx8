%skeleton "lalr1.cc" /* C++ */
%require "3.0"

%define api.namespace { tx::parser }
%define api.parser.class { Parser }
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include <tx8/core/types.h>
    #include <tx8/core/instruction.h>
    #include <tx8/core/util.h>
    #include <sstream>

    namespace tx::parser {
        class Lexer;
    }
    struct tx_asm_Assembler;
    typedef struct tx_asm_Assembler tx_asm_Assembler;
}

%code top {
#include <tx8/asm/assembler.hpp>
#include <tx8/asm/lexer.hpp>

#include <stdio.h>

extern int tx_lex_yylineno;
extern struct tx_asm_Assembler* tx_asm_yyasm;

extern int tx_asm_yyerror(const char* str);
#define yylex(x, y) lexer.get_next_token()
}

%lex-param { tx::parser::Lexer& lexer }
%lex-param { tx_asm_Assembler& as }
%parse-param { tx::parser::Lexer& lexer }
%parse-param { tx_asm_Assembler& as }

%locations
%define parse.trace
%define parse.error verbose

%start program

%token END 0 "end of file"
%token EOL "end of line"
%token SPACE "space"
%token <tx_uint32> REGISTER REGISTER_ADDRESS ABSOLUTE_ADDRESS RELATIVE_ADDRESS INTEGER8 INTEGER16 INTEGER32 OPCODE0 OPCODE1 OPCODE2
%token <tx_float32> FLOAT
%token <std::string> LABEL ALIAS INVALID

%type <tx_Parameter> parameter
%type <tx_Instruction> instruction

%%
program:     %empty
           | program SPACE
           | program EOL
           | program statement
statement:   LABEL SPACE instruction EOL             { tx_asm_assembler_handle_label(&as, $1); tx_asm_assembler_set_label_position(&as, $1); tx_asm_assembler_add_instruction(&as, $3); }
           | LABEL EOL                               { tx_asm_assembler_handle_label(&as, $1); tx_asm_assembler_set_label_position(&as, $1); }
           | instruction EOL                         { tx_asm_assembler_add_instruction(&as, $1); }
           | error EOL                               { }

instruction: OPCODE2 SPACE parameter SPACE parameter { tx_Instruction i = { .opcode = (tx_Opcode) $1, .params = { .p1 = $3, .p2 = $5 } }; $$ = i; }
           | OPCODE1 SPACE parameter                 { tx_Instruction i = { .opcode = (tx_Opcode) $1, .params = { .p1 = $3 } }; $$ = i; }
           | OPCODE0                                 { tx_Instruction i = { .opcode = (tx_Opcode) $1 }; $$ = i; }

parameter:   ALIAS                                   { tx_Parameter p = { .value = { .u = tx_str_hash($1.c_str()) }, .mode = tx_param_constant32 }; $$ = p; }
           | INTEGER8                                { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_constant8 }; $$ = p; }
           | INTEGER16                               { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_constant16 }; $$ = p; }
           | INTEGER32                               { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_constant32 }; $$ = p; }
           | FLOAT                                   { tx_Parameter p = { .value = { .f = $1 }, .mode = tx_param_constant32 }; $$ = p; }
           | ABSOLUTE_ADDRESS                        { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_absolute_address }; $$ = p; }
           | RELATIVE_ADDRESS                        { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_relative_address }; $$ = p; }
           | REGISTER                                { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_register }; $$ = p; }
           | REGISTER_ADDRESS                        { tx_Parameter p = { .value = { .u = $1 }, .mode = tx_param_register_address }; $$ = p; }
           | LABEL                                   { tx_Parameter p = { .value = { .u = tx_asm_assembler_handle_label(tx_asm_yyasm, $1) }, .mode = tx_param_label }; $$ = p; }
%%

void tx::parser::Parser::error(const tx::parser::location& loc, const std::string& str) {
    std::stringstream ss;
    ss << loc;
    tx_asm_error(tx_asm_yyasm, "Line %d: %s\n", ss.str(), str.c_str());
}
