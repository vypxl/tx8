# TX8 Assembly language

This document contains the specification of the tx8 assembly language.
For more information, see [spec.md](spec.md).

## Syntax

Syntax rules are mixed between [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form) and regular expressions for brevity.

```plain
(* These rules use regular expressions *)

op0         = hlt|nop|ret|ei|di|stop
op1         = jmp|jeq|jne|jgt|jge|jlt|jle|call|sys|lda|sta|ldb|stb|ldc|stc|ldd|std|zero|push|pop|inc|dec|abs|sign|not|finc|fdec|fabs|fsign|sin|cos|tan|asin|acos|atan|sqrt|exp|log|log2|log10|rand|rseed|itf|fti|utf|ftu
op2         = cmp|fcmp|ucmp|ld|lds|lw|lws|add|sub|mul|div|mod|max|min|and|or|nand|xor|slr|sar|sll|ror|rol|set|clr|tgl|test|fadd|fsub|fmul|fdiv|fmod|fmax|fmin|atan2|pow|uadd|usub|umul|udiv|umod|umax|umin
identifier  = [a-zA-Z][a-zA-Z0-9_\-]*
integer8    = (0x[0-9a-f]{1,2}|0b[01]{1,8}|-?[0-9]+)(i8|u8)
integer16   = (0x[0-9a-f]{1,4}|0b[01]{1,16}|-?[0-9]+)(i16|u16)
integer32   = (0x[0-9a-f]{1,8}|0b[01]{1,32}|-?[0-9]+)(i|u|i32|u32)?
float       = -?[0-9]+\.[0-9]+
address     = [0-9a-f]{1,6}
register    = a|b|c|d|r|o|s|p|ai|bi|ci|di|ri|oi|si|pi|as|bs|cs|ds|rs|os|ss|ps|ab|bb|cb|db|rb|ob|sb|pb|A|B|C|D|R|O|S|P|AI|BI|CI|DI|RI|OI|SI|PI|AS|BS|CS|DS|RS|OS|SS|PS|AB|BB|CB|DB|RB|OB|SB|PB|Ai|Bi|Ci|Di|Ri|Oi|Si|Pi|As|Bs|Cs|Ds|Rs|Os|Ss|Ps|Ab|Bb|Cb|Db|Rb|Ob|Sb|Pb
space       = ([ \t]+)|([ \t]*)(;.*)
eol         = \n|\r\n

(* These rules use EBNF *)

absolute_address = '#', address ;
relative_address = '$', ['-'], address (* optional minus for negative offset *) ;
register_address = '@', register ;
label            = ':', identifier ;
alias            = '&', identifier ;

parameter   = integer8 | integer16 | integer32 | float | absolute_address | relative_address | register | register_address | label | alias ;
instruction = op0 | op1, space, parameter | op2, space, parameter, space, parameter ;
statement   = label | {label, space}, instruction ;
program     = {[space], statement, [space], eol} ;
```
