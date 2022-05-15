%{
#include "tx8.tab.h"

#include <stdio.h>
#include <math.h>
#include <tx8/core/instruction.h>

extern TX_ASM_YYSTYPE tx_asm_yylval;
char* strdup(char* src);

int tx_asm_sprefix(const char* prefix, const char* str) {
    return strncmp(prefix, str, strlen(prefix)) == 0;
}
tx_int32 tx_asm_num(const char* str) {
    if (tx_asm_sprefix("0x", str)) return strtol(str + 2, (char**)NULL, 16);
    if (tx_asm_sprefix("0b", str)) return strtol(str + 2, (char**)NULL, 2);
    return atoi(str);
}
tx_int32 tx_asm_hex(const char* str) {
    return strtol(str, (char**)NULL, 16);
}

int fileno(FILE *stream);
%}

%option prefix="tx_asm_yy"
%option caseless
%option yylineno
%option noinput
%option nounput

OP0         nop|ret|hlt
OP1         jmp|cal|sys|lda|sta|ldb|stb|ldc|stc|ldd|std|zer|psh|pop|inc|dec|not|fin|fde|itf|fti|sin|cos|tan|sqt|abs|fab|rnd|rsd|utf|ftu
OP2         mov|mxv|add|sub|mul|div|mod|and|ora|nnd|xor|shr|shl|ror|rol|fad|fsu|fmu|fdi|fmo|umu|udi
OP3         jeq|jne|jgt|jge|jlt|jle|max|min|fmx|fmn|atn|umx|umn
IDENTIFIER  [a-zA-Z][a-zA-Z0-9_\-]*
INTEGER8    (0x[0-9a-f]{1,2}|0b[01]{1,8}|-?[0-9]+)(i8|u8)
INTEGER16   (0x[0-9a-f]{1,4}|0b[01]{1,16}|-?[0-9]+)(i16|u16)
INTEGER32   (0x[0-9a-f]{1,8}|0b[01]{1,32}|-?[0-9]+)(i|u|i32|u32)?
FLOAT       -?[0-9]+\.[0-9]+
ADDRESS     [0-9a-f]{1,6}
REGISTER    a|b|c|d|o|s|p|ai|bi|ci|di|oi|si|pi|as|bs|cs|ds|os|ss|ps|ab|bb|cb|db|ob|sb|pb
SPACE       [ \t]+
SEMI        [;]
EOL         ([ \t]*)(;.*)?(\n|\r\n)

%%
{SEMI}.*        ; // ignore comments

{SPACE}         { return tx_asm_SPACE; };
:{IDENTIFIER}   { tx_asm_yylval.s = strdup(yytext + 1); return tx_asm_LABEL; }
&{IDENTIFIER}   { tx_asm_yylval.s = strdup(tx_asm_yytext + 1); return tx_asm_ALIAS; }
@{REGISTER}     { tx_asm_yylval.i = tx_reg_id_from_name(tx_asm_yytext + 1); return tx_asm_REGISTER_ADDRESS; }
{REGISTER}      { tx_asm_yylval.i = tx_reg_id_from_name(tx_asm_yytext); return tx_asm_REGISTER; }
#{ADDRESS}      { tx_asm_yylval.i = tx_asm_hex(tx_asm_yytext + 1); return tx_asm_ABSOLUTE_ADDRESS; }
$-?{ADDRESS}    { tx_asm_yylval.i = tx_asm_hex(tx_asm_yytext + 1); return tx_asm_RELATIVE_ADDRESS; }
{FLOAT}         { tx_asm_yylval.f = atof(tx_asm_yytext); return tx_asm_FLOAT; }
{INTEGER8}      { tx_asm_yylval.i = tx_asm_num(tx_asm_yytext); return tx_asm_INTEGER8; }
{INTEGER16}     { tx_asm_yylval.i = tx_asm_num(tx_asm_yytext); return tx_asm_INTEGER16; }
{INTEGER32}     { tx_asm_yylval.i = tx_asm_num(tx_asm_yytext); return tx_asm_INTEGER32; }
{OP0}           { tx_asm_yylval.i = tx_opcode_from_name(tx_asm_yytext); return tx_asm_OPCODE0; }
{OP1}           { tx_asm_yylval.i = tx_opcode_from_name(tx_asm_yytext); return tx_asm_OPCODE1; }
{OP2}           { tx_asm_yylval.i = tx_opcode_from_name(tx_asm_yytext); return tx_asm_OPCODE2; }
{OP3}           { tx_asm_yylval.i = tx_opcode_from_name(tx_asm_yytext); return tx_asm_OPCODE3; }
{EOL}           { return tx_asm_EOL; }
.               { return tx_asm_yytext[0]; }
%%
