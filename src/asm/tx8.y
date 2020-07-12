%code requires {
#include "tx8/asm/types.h"

#include <tx8/core/instruction.h>
#include <tx8/core/types.h>
#include <tx8/core/util.h>
}

%{
#include "tx8/asm/assembler.h"

#include <stdio.h>

extern int tx_asm_yylineno;
extern tx_asm_Assembler* tx_asm_yyasm;
%}

%define api.prefix {tx_asm_yy}
%define parse.error verbose
%locations

%start program
%union {
    tx_uint32 i;
    tx_float32 f;
    char* s;
    tx_asm_Parameter param;
    tx_asm_Instruction inst;
}

%token <i> tx_asm_EOL tx_asm_SPACE tx_asm_REGISTER tx_asm_REGISTER_ADDRESS tx_asm_ABSOLUTE_ADDRESS tx_asm_RELATIVE_ADDRESS tx_asm_INTEGER8 tx_asm_INTEGER16 tx_asm_INTEGER32 tx_asm_OPCODE0 tx_asm_OPCODE1 tx_asm_OPCODE2 tx_asm_OPCODE3
%token <f> tx_asm_FLOAT
%token <s> tx_asm_LABEL tx_asm_ALIAS tx_asm_INVALID

%type <param> parameter
%type <inst> instruction

%%
program:     %empty 
           | program tx_asm_SPACE
           | program tx_asm_EOL
           | program statement
statement:   tx_asm_LABEL tx_asm_SPACE instruction        { tx_asm_assembler_handle_label(tx_asm_yyasm, $1); tx_asm_assembler_set_label_position(tx_asm_yyasm, $1); free($1); tx_asm_assembler_add_instruction(tx_asm_yyasm, $3); }
           | tx_asm_LABEL tx_asm_EOL                      { tx_asm_assembler_handle_label(tx_asm_yyasm, $1); tx_asm_assembler_set_label_position(tx_asm_yyasm, $1); free($1); }
           | instruction tx_asm_EOL                       { tx_asm_assembler_add_instruction(tx_asm_yyasm, $1); }
instruction: tx_asm_OPCODE3 tx_asm_SPACE parameter tx_asm_SPACE parameter tx_asm_SPACE parameter { tx_asm_Instruction i = { .opcode = $1, .p1 = $3, .p2 = $5, .p3 = $7 }; $$ = i; }
           | tx_asm_OPCODE2 tx_asm_SPACE parameter tx_asm_SPACE parameter                        { tx_asm_Instruction i = { .opcode = $1, .p1 = $3, .p2 = $5 }; $$ = i; }
           | tx_asm_OPCODE1 tx_asm_SPACE parameter                                               { tx_asm_Instruction i = { .opcode = $1, .p1 = $3 }; $$ = i; }
           | tx_asm_OPCODE0                                                                      { tx_asm_Instruction i = { .opcode = $1 }; $$ = i; }
parameter:   tx_asm_ALIAS                                 { tx_asm_Parameter p = { .value.u = tx_str_hash($1), .mode = tx_param_constant32 }; $$ = p; free($1); }
           | tx_asm_INTEGER8                              { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_constant8 }; $$ = p; }
           | tx_asm_INTEGER16                             { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_constant16 }; $$ = p; }
           | tx_asm_INTEGER32                             { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_constant32 }; $$ = p; }
           | tx_asm_FLOAT                                 { tx_asm_Parameter p = { .value.f = $1, .mode = tx_param_constant32 }; $$ = p; }
           | tx_asm_ABSOLUTE_ADDRESS                      { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_absolute_address }; $$ = p; }
           | tx_asm_RELATIVE_ADDRESS                      { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_relative_address }; $$ = p; }
           | tx_asm_REGISTER                              { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_register }; $$ = p; }
           | tx_asm_REGISTER_ADDRESS                      { tx_asm_Parameter p = { .value.u = $1, .mode = tx_param_register_address }; $$ = p; }
           | tx_asm_LABEL                                 { tx_asm_Parameter p = { .value.u = tx_asm_assembler_handle_label(tx_asm_yyasm, $1), .mode = tx_asm_param_label_id }; $$ = p; }
%%

int tx_asm_yyerror(const char* str) {
    fprintf(stderr, "Line %d: %s\n ", tx_asm_yylineno, str);
    return 1;
}

int tx_asm_yywrap() {
    return 1;
}
