%{
#include "tx8_parser.hpp"

#include <tx8/asm/lexer.hpp>

#include <stdio.h>
#include <math.h>
#include <tx8/core/instruction.hpp>

int tx_lex_sprefix(const char* prefix, const char* str) {
    return strncmp(prefix, str, strlen(prefix)) == 0;
}
tx::int32 tx_lex_num(const char* str) {
    if (tx_lex_sprefix("0x", str)) return strtol(str + 2, (char**)NULL, 16);
    if (tx_lex_sprefix("0b", str)) return strtol(str + 2, (char**)NULL, 2);
    return atoi(str);
}
tx::int32 tx_lex_hex(const char* str) {
    return strtol(str, (char**)NULL, 16);
}

using namespace tx::parser;

#define yyterminate() Parser::make_END(location());
%}

%option c++
%option yyclass="Lexer"
%option prefix="tx_lex_"
%option caseless
%option yylineno
%option noinput
%option nounput
%option noyywrap

OP0         hlt|nop|ret|ei|di|stop
OP1         jmp|jeq|jne|jgt|jge|jlt|jle|call|sys|lda|sta|ldb|stb|ldc|stc|ldd|std|zero|push|pop|inc|dec|abs|sign|not|finc|fdec|fabs|fsign|sin|cos|tan|asin|acos|atan|sqrt|exp|log|log2|log10|rand|rseed|itf|fti|utf|ftu
OP2         cmp|fcmp|ucmp|ld|lds|lw|lws|add|sub|mul|div|mod|max|min|and|or|nand|xor|slr|sar|sll|ror|rol|set|clr|tgl|test|fadd|fsub|fmul|fdiv|fmod|fmax|fmin|atan2|pow|uadd|usub|umul|udiv|umod|umax|umin
IDENTIFIER  [a-zA-Z][a-zA-Z0-9_\-]*
INTEGER8    (0x[0-9a-f]{1,2}|0b[01]{1,8}|-?[0-9]+)(i8|u8)
INTEGER16   (0x[0-9a-f]{1,4}|0b[01]{1,16}|-?[0-9]+)(i16|u16)
INTEGER32   (0x[0-9a-f]{1,8}|0b[01]{1,32}|-?[0-9]+)(i|u|i32|u32)?
FLOAT       -?[0-9]+\.[0-9]+
ADDRESS     [0-9a-f]{1,6}
REGISTER    a|b|c|d|r|o|s|p|ai|bi|ci|di|ri|oi|si|pi|as|bs|cs|ds|rs|os|ss|ps|ab|bb|cb|db|rb|ob|sb|pb|A|B|C|D|R|O|S|P|AI|BI|CI|DI|RI|OI|SI|PI|AS|BS|CS|DS|RS|OS|SS|PS|AB|BB|CB|DB|RB|OB|SB|PB|Ai|Bi|Ci|Di|Ri|Oi|Si|Pi|As|Bs|Cs|Ds|Rs|Os|Ss|Ps|Ab|Bb|Cb|Db|Rb|Ob|Sb|Pb
SPACE       ([ \t]+)|([ \t]*)(;.*)
SEMI        [;]
EOL         \n|\r\n

%%
{SEMI}.*        ; // ignore comments

{SPACE}         { return Parser::make_SPACE(location()); };
:{IDENTIFIER}   { return Parser::make_LABEL(yytext + 1, location()); }
&{IDENTIFIER}   { return Parser::make_ALIAS(yytext + 1, location()); }
@{REGISTER}     { return Parser::make_REGISTER_ADDRESS((tx::uint32) reg_id_from_name(yytext + 1), location()); }
{REGISTER}      { return Parser::make_REGISTER((tx::uint32) reg_id_from_name(yytext), location()); }
#{ADDRESS}      { return Parser::make_ABSOLUTE_ADDRESS(tx_lex_hex(yytext + 1), location()); }
$-?{ADDRESS}    { return Parser::make_RELATIVE_ADDRESS(tx_lex_hex(yytext + 1), location()); }
{FLOAT}         { return Parser::make_FLOAT(atof(yytext), location()); }
{INTEGER8}      { return Parser::make_INTEGER8(tx_lex_num(yytext), location()); }
{INTEGER16}     { return Parser::make_INTEGER16(tx_lex_num(yytext), location()); }
{INTEGER32}     { return Parser::make_INTEGER32(tx_lex_num(yytext), location()); }
{OP0}           { return Parser::make_OPCODE0((tx::uint32) opcode_from_name(yytext), location()); }
{OP1}           { return Parser::make_OPCODE1((tx::uint32) opcode_from_name(yytext), location()); }
{OP2}           { return Parser::make_OPCODE2((tx::uint32) opcode_from_name(yytext), location()); }
{SPACE}?{EOL}   { return Parser::make_EOL(location()); }
.               { return Parser::make_INVALID(yytext, location()); }
<<EOF>>         { return yyterminate(); }
%%
