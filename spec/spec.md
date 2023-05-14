# TX8 - Spec

## Graphics

(Not yet implemented)

- 8bit color (256 color)
- 512x288 pixel display
- 16x16px tiles (32x18 tile display)

## Sound & Music

(Not yet implemented)

- 8 instruments
- 8 effects
- 32-note sound samples
- music engine: 4-channel sound sample playback

## Miscellaneous

- Assembly instruction set
- Many convenience functions builtin (not yet implemented)
- 9 button input (not yet implemented)
- 8 megabyte cartridge (0x800000 bytes) for code, textures, map / data, sound, music

## Memory

- 16 megabyte memory
- Zero-initialized
- 4 byte addresses
- reading or writing out of bounds (beyond 0xffffff) truncates the address to 24-bit: `0x12345678 => 0x345678`
- when reading at the edge of memory, 0 is read for all out of bounds bytes
- when writing at the edge of memory, all out of bounds bytes are ignored
- 4mb (#0x000000 - #0x3fffff) system reserved / registers (read/writable)
- 8mb (#0x400000 - #0xbfffff) loaded cartridge data (read/writable)
- 4mb (#0xc00000 - #0xffffff) work RAM (read/writable)

## Assembly Programming (TX8-Asm)

Programming TX8 is generally done in assembly. The virtual processor and assembly language used is described below.
For a formal syntax specification, see [tx8_asm_language.md](tx8_asm_language.md).
A tx8 assembly file has the file extension `.tx8`.

### Registers

All CPU registers are 32-bit. Smaller parts can be accessed using suffixes:
Given the register `X`, the full register is available via `X` or `Xi`,
the lower 2 bytes via `Xs` and the lowest byte via `Xb`.
The names are case-insensitive.
(You can memorize the suffixes by thinking of: `int`, `short` and `byte`)

When mixing registers or other destinations of different sizes, the first parameter determines the size of the
operation.
This means when writing to `ab`, the operation is executed in 8-bit mode, the second parameter is truncated to 8-bit.
On the other hand, when writing to `as`, or `ai` / `a`, the operation is executed in 16/32-bit mode and the second
parameter is extended or truncated to the size of the first parameter. This does not hold for arithmetic instructions.
Those are always executed in 32 bit mode, regardless of the sources and destinations. See [arithmetic](#arithmetic)
for more information.

Beware that when using smaller registers as destinations, only the lower bytes will be set, the upper bytes will
retain their previous value. You can use something like `ld a as` afterwards to set the upper bits to zero.

When reading from a smaller register in an operation that requires a 32-bit value, the value is zero-extended.
When writing a larger value to a smaller register, the result is truncated.

The available CPU registers are described here:

- `A`,`B`,`C`,`D`: general purpose registers
- `R`: 'rest' register (stores secondary results of previous operations)
- `O`: offset for relative addressing
- `P`: Program counter / Instruction Pointer
- `S`: Stack pointer

The program counter can have any value in the range 0..0xfffff0 to prevent parsing an instruction outside of memory,
because an instruction can be up to 10 bytes long.

The program counter starts at `0x400000`, the first byte of the mapped rom data.
The stack pointer starts at `0xc02000`, in work ram. It is advised to change this according to the programs needs.

For how the registers are represented as parameters in binary, see this table:

| Register | 32-bit | 16-bit | 8-bit |
| -------- | ------ | ------ | ----- |
| A        | 0x0    | 0x20   | 0x10  |
| B        | 0x1    | 0x21   | 0x11  |
| C        | 0x2    | 0x22   | 0x12  |
| D        | 0x3    | 0x23   | 0x13  |
| R        | 0x4    | 0x24   | 0x14  |
| O        | 0x5    | 0x25   | 0x15  |
| P        | 0x6    | 0x26   | 0x16  |
| S        | 0x7    | 0x27   | 0x17  |

### Stack

The Stack lives at #0xc02000 by default and grows downward, so it has a size of 8192 (0x2000) bytes.
However, you could set the stack pointer `S` to anything you want, if you need a bigger or smaller stack.
There is no over/underflow detection, so be careful!

### Instructions

Every instruction consists of a 1-byte opcode followed by 0-1 bytes parameter modes and 0-2 parameters.
An example opcode looks like this: `op bi #43`.
Every instruction takes up one line.
Comments can be added after a semi-colon (;).

### Parameter Modes

There are 5 ways to give parameters to instructions:

- Constant mode (decimal, hex, binary and float): `lda 42`, `lda 0x2a`, `lda 0b101010`, `lda 42.1337`
- Absolute address mode (`#` prefix): `lda #42`
- Relative address mode (`$` prefix, the given address is offset by the value in register `O`): `lda $-42`
- Register mode: `lda bi`
- Register address mode (`@` prefix, interprets the value in the register as an address): `lda @bs`

In binary, parameter modes are indicated by the 0-1 bytes after the opcode. Every 4 bits represent one
parameter mode.

| Code | Mode             | Amount of bytes the parameter takes |
| ---- | ---------------- | ----------------------------------- |
| 0x0  | Unused           | 0                                   |
| 0x1  | Constant 8bit    | 1                                   |
| 0x2  | Constant 16bit   | 2                                   |
| 0x3  | Constant 32bit   | 4                                   |
| 0x4  | Absolute address | 3                                   |
| 0x5  | Relative address | 3                                   |
| 0x6  | Register         | 1                                   |
| 0x7  | Register address | 1                                   |

An example binary instruction looks like this:

```plain
asm: cmp a #0xc04023
bin: op mm p1 p3 p3 p3 p3
     02 64 00 23 40 c0 00
```

Jump points should be used to jump to absolute addresses like this:

```asm
:sub_a            ; use a colon followed by a name to set a jump point `:name`
    add a 2
    jmp :continue ; use colon (:) `:name` to specify a jump point

:main
    lda 42
    jmp :sub_a

:continue hlt     ; You can put a label and an instruction on the same line
```

### Integer Endianness

tx8 uses little-endian.

### Strings

Raw string data (ascii) can be included in tx8 assembly.
There are two ways of doing this.

First, strings can be embedded as raw data in between instruction, by just wrapping the desired text in double quotes `"<text>"`.
Then, the ascii bytes will be inserted into the binary at the position they appear in the assembly. You might want to add a label before
the string to be able to reference it.

Second, strings can be used like other parameters. For example, the instruction `push "Hello world"`, would push the address of the
specified text in the rom to the stack. The string is placed in the data section located at the end of the rom data. This way, the instruction
flow stays the same, no need for jumps to skip the string data.

Strings are wrapped in double quotes. You can use the escape codes `\t`, `\r`, `\n` and `\\`. Invalid escape codes are ignored and treated like normal text.
Strings can wrap multiple lines.

Note that all strings are implicitly null-terminated.

Note that if you use the first way, you might need to jump past the string data or risk the cpu executing the string data as code.

```tx8
jmp :code
:hello_str "Hello world"

:code
push :hello_str
sys &println
```

### Panics / Crashes

When the cpu runs into an error, such as trying to write to a non writable location (e. g. a constant value), the cpu should stop executing
and crash with an error.

Reading an invalid opcode will **not** crash, invalid opcodes shall be treated like `nop`. The runtime can output a warning if desired.

### Instruction List

Parameters are described like this: Three characters, one for each parameter:

- `0` for unused
- `v` for values (constants, addresses, registers)
- `w` for writable (addresses, registers)

#### Flow Control

The comparison instructions `cmp`, `fcmp` and `ucmp` compare the first parameter to the second parameter
and write the result of the comparison into the `R` register. This is equivalent to calculating the difference
and storing its signum in `R` (as a signed integer).

```
fcmp 2.3 3.1

<=>

lda 2.3
fsub a 3.1
fsign a
fti a
sta r
```

The conditional jump instructions read from the `R` register. They essentially compare the value in `R` to 0
and jump if the comparison is true.

For unconditional jumps, use `jmp`.
For conditional jumps, first use the correct `cmp` instruction,
then use the `jeq`, `jne`, `jgt`, `jge`, `jlt` or `jle` instructions to jump based on the comparison result.

If you want to jump based on the result of a `test` bit test operation, use `jne` after `test` to jump if the
tested bit was 1, `jeq` to jump if the tested bit was 0.

Note, after executing any instruction changing the `P` register, such as `jmp` or `call`, the program counter
shall not be incremented. This means `ld p :function` also does **not** increment `P` after executing.

| Opcode | Asm  | Parameters | Operation                                        | Example       |
| ------ | ---- | ---------- | ------------------------------------------------ | ------------- |
| 0x00   | hlt  | `00`       | halt / stop execution                            | `hlt`         |
| 0x01   | nop  | `00`       | no operation                                     | `nop`         |
| 0x02   | jmp  | `v0`       | jump to address                                  | `jmp :label`  |
| 0x03   | jeq  | `v0`       | jump if equal                                    | `jeq :branch` |
| 0x04   | jne  | `v0`       | jump if not equal                                | `jne :branch` |
| 0x05   | jgt  | `v0`       | jump if greater than                             | `jgt :branch` |
| 0x06   | jge  | `v0`       | jump if greater than or equal to                 | `jge :branch` |
| 0x07   | jlt  | `v0`       | jump if less than                                | `jlt :branch` |
| 0x08   | jle  | `v0`       | jump if less than or equal to                    | `jle :branch` |
| 0x09   | cmp  | `vv`       | Compare signed                                   | `cmp a -5`    |
| 0x0a   | fcmp | `vv`       | Compare floating point                           | `fcmp a 0.5`  |
| 0x0b   | ucmp | `vv`       | Compare unsigned                                 | `ucmp a 0`    |
| 0x0c   | call | `v0`       | call function                                    | `call :fun`   |
| 0x0d   | ret  | `00`       | return from function                             | `ret`         |
| 0x0e   | sys  | `v0`       | call system function (see [syscalls](#syscalls)) | `sys &print`  |

##### Calling Convention

There is a calling convention. tx8 libraries and programs are encouraged to use it for
better compatibility. The programmer may still choose a different way of calling functions
if they so please.

The calling convention of tx8 is very similar
to [CDECL](https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#CDECL)
To properly call a function / subroutine, the caller must follow these steps:

- push all function parameters onto the stack in right-to-left order
- use a `call` instruction to call the function
- clean the stack (add the byte size of all parameters to the stack pointer)

The callee must follow these steps:

- use parameters from the stack (remember to skip the 4-byte return address)
- run code
- make sure to restore the stack pointer
- use a `ret` instruction to return to the caller (pops the topmost address off the stack and jumps to it)

The callee does not have to preserve any register values.

##### Syscalls

The `sys` instruction expects a 32 bit value corresponding to a syscall defined in the runtime. The runtime
should implement the standard library (see [Syscall standard library](#syscall-standard-library)), and should also allow registering custom system calls.

Each syscall has a name and an id. The id is calculated via the string hash function (see [String hash function](#string-hash-function)),
by feeding the name of the syscall to it. `sys &print` => id is `strhash("print")`.

#### Loading and Storing

The shortcuts for registers (`lda`, `stc`, ...) are for convenience, `ld` can be used for everything.
`ld` moves a different amount of bytes according to its parameters:

- `ld register something` moves as many bytes as the register has (A=4, As=2, Ab=1) or how many the other parameter
  might have, whichever is lower
- `ld address register` moves as many bytes as the register has (A=4, As=2, Ab=1)
- `ld address address` moves one byte
- `ld address constant` moves as many bytes as the constant has

Use `lw` to always move 4 bytes.

Push and pop behave like this:

- `push register` pushes as many bytes as the register has (A=4, As=2, Ab=1)
- `push address` pushes 4 bytes
- `push constant` pushes as many bytes as the constant has (A=4, As=2, Ab=1)
- `pop register` pops as many bytes as the register has (A=4, As=2, Ab=1)
- `pop address` pops 4 bytes

`push` decreases the stack pointer by the amout of bytes pushed, and
`pop` increases the stack pointer by the amount of bytes popped.

| Opcode | Asm  | Parameters | Operation                                                                            | Example        |
| ------ | ---- | ---------- | ------------------------------------------------------------------------------------ | -------------- |
| 0x10   | ld   | `wv`       | load value (parameter2) into parameter1 (p1 := p2) (zero extension on small values)  | `ld A 42`      |
| 0x11   | lds  | `wv`       | same as `ld`, but values will be sign extended                                       | `ld A 42`      |
| 0x12   | lw   | `wv`       | load a word (4 bytes) from parameter2 into parameter1 (values will be zero extended) | `lw a #c01234` |
| 0x13   | lws  | `wv`       | same as `lw`, but values will be sign extended                                       | `lws a -1`     |
| 0x14   | lda  | `v0`       | load value into register A                                                           | `lda 42`       |
| 0x15   | sta  | `w0`       | store value from register A into parameter1                                          | `sta $2`       |
| 0x16   | ldb  | `v0`       | load value into register B                                                           | `ldb 55`       |
| 0x17   | stb  | `w0`       | store value from register B into parameter1                                          | `stb a`        |
| 0x18   | ldc  | `v0`       | load value into register C                                                           | `ldc $32`      |
| 0x19   | stc  | `w0`       | store value from register C into parameter1                                          | `stc #c01234`  |
| 0x1a   | ldd  | `v0`       | load value into register D                                                           | `ldd @cb`      |
| 0x1b   | std  | `w0`       | store value from register D into parameter1                                          | `std $-35`     |
| 0x1c   | zero | `w0`       | zero out parameter1 (addresses 1 byte)                                               | `zero a`       |
| 0x1d   | push | `v0`       | push onto stack                                                                      | `push a`       |
| 0x1e   | pop  | `w0`       | pop from stack                                                                       | `pop a`        |

#### Arithmetic

All arithmetic operations are in-place on the first parameter, so an `add a 5` increments register A by 5.

By default, all arithmetic operations are 32-bit, smaller integer constants and smaller registers are zero-extended
or sign-extended, depending on the instruction (signend integer instructions sign extend, others zero-extend).
When a smaller register is the destination of an operation, the result is truncated to the size of the register.

Normal instructions operate on signed integers. If you have unsigned integers
or floats, you have to use the specialized instructions.

##### Behaviour of the `R` register

- The `cmp`, `fcmp` and `ucmp` instructions set the `R` register to the result of the comparison.
  See [flow control](#flow-control).
- The `inc`, `dec`, `add` and `sub` instructions set the `R` register's lowest bit if there was an unsigned overflow,
  and the second-lowest bit if there was a signed overflow.
- The `mul` and `umul` instructions sets the `R` register to the top 32 bit of the 64 bit result.
- The `div`, and `udiv` instructions sets the `R` register to the remainder of the division.
- The `max`, `min`, `fmax`, `fmin`, `umax` and `umin` instructions set the `R` register to the discarded value.
- The `abs` and `fabs` instructions sets the `R` register to the signum of the original value (in the respective data
  types).
- The `slr`, `sar` and `sll` instructions set the `R` register to the shifted-out bits.
- The `set`, `clr` `tgl` and `test` instructions set the `R` register to the original value of the bit they operated on.
- The `rand` operation places the original random integer into the `R` register.

Note that the R register assignment is done **after** the operation itself, this means if one specified the `R`
register as the destination of an operation, the normal value is discarded and the residual value is found in `R`.

Example:

```asm
ld r 1
max r 2
```

This would result in the value `1` being stored in `R` and the value `2` being discarded.

Note that the reason why there are `add` and `uadd` instructions is that the the signed `add` sign-extends smaller values,
and the unsigned `uadd` zero-extends them. This means whenever you use smaller registers or constantst, you should use
the correct instruction.
The same goes for `sub` / `usub`.

Note that the R register behaviour aims to be 'what the programmer expects'. Take the `sll` instruction. When shifting
the `Ab` register by 8 bits, the `R` register will contain these 8 bits, even if, when considering the 32-bit default mode
for arithmetic, it would contain just zeros.

##### Increment and decrement

| Opcode | Asm | Parameters | Operation | Example  |
| ------ | --- | ---------- | --------- | -------- |
| 0x20   | inc | `w0`       | increment | `inc a`  |
| 0x21   | dec | `w0`       | decrement | `dec $1` |

##### Signed Integer Operations

| Opcode | Asm  | Parameters | Operation         | Example    |
| ------ | ---- | ---------- | ----------------- | ---------- |
| 0x22   | add  | `wv`       | add               | `add a 5`  |
| 0x23   | sub  | `wv`       | subtract          | `sub a 8`  |
| 0x24   | mul  | `wv`       | multiply          | `mul a -2` |
| 0x25   | div  | `wv`       | divide            | `div a 5`  |
| 0x26   | mod  | `wv`       | remainder         | `mod a 7`  |
| 0x27   | max  | `wv`       | p1 := max(p2, p3) | `max a 3`  |
| 0x28   | min  | `wv`       | p1 := min(p2, p3) | `min a 3`  |
| 0x29   | abs  | `w0`       | absolute value    | `abs a`    |
| 0x2a   | sign | `w0`       | signum of p1      | `sign a`   |

##### Bitwise Operations

| Opcode | Asm  | Parameters | Operation                                    | Example            |
| ------ | ---- | ---------- | -------------------------------------------- | ------------------ |
| 0x30   | and  | `wv`       | and                                          | `and c 0b10011010` |
| 0x31   | or   | `wv`       | or                                           | `or c 0x7f`        |
| 0x32   | not  | `w0`       | not                                          | `not c`            |
| 0x33   | nand | `wv`       | nand                                         | `nand c d`         |
| 0x34   | xor  | `wv`       | xor                                          | `xor c d`          |
| 0x35   | slr  | `wv`       | shift logical right                          | `slr b 2`          |
| 0x36   | sar  | `wv`       | shift arithmetic right                       | `sar b 1`          |
| 0x37   | sll  | `wv`       | shift logical left                           | `sll b 1`          |
| 0x38   | ror  | `wv`       | rotate right                                 | `ror b 3`          |
| 0x39   | rol  | `wv`       | rotate left                                  | `rol b 7`          |
| 0x3a   | set  | `wv`       | set the p2'th bit of p1                      | `set a 5`          |
| 0x3b   | clr  | `wv`       | clear the p2'th bit of p1                    | `clr a 7`          |
| 0x3c   | tgl  | `wv`       | toggle the p2'th bit of p1                   | `tgl a 7`          |
| 0x3d   | test | `vv`       | test the p2'th bit of p1 (write it into `R`) | `test a 3`         |

When shifting, only the lower 5 (4 / 3 with 16 / 8 bit destinations) bits of the shift amount are
considered. Analogously, the bit position for `set`, `clr`, `tgl`, and `test` is also truncated.

##### Floating Point Operations

| Opcode | Asm   | Parameters | Operation                                | Example          |
| ------ | ----- | ---------- | ---------------------------------------- | ---------------- |
| 0x40   | finc  | `w0`       | floating point increment                 | `finc a`         |
| 0x41   | fdec  | `w0`       | floating point decrement                 | `fdec $1`        |
| 0x42   | fadd  | `wv`       | floating point add                       | `fadd a 5.0`     |
| 0x43   | fsub  | `wv`       | floating point subtract                  | `fsub a 8`       |
| 0x44   | fmul  | `wv`       | floating point multiply                  | `fmul a -2.7924` |
| 0x45   | fdiv  | `wv`       | floating point divide                    | `fdiv a 5.2`     |
| 0x46   | fmod  | `wv`       | floating point remainder                 | `fmod a 7`       |
| 0x47   | fmax  | `wv`       | floating point max                       | `fmax a 2.5`     |
| 0x48   | fmin  | `wv`       | floating point min                       | `fmin a 2.5`     |
| 0x49   | fabs  | `w0`       | floating point absolute value            | `fabs b`         |
| 0x4a   | fsign | `w0`       | floating point signum (-1.0 / 0.0 / 1.0) | `fsign b`        |
| 0x4b   | sin   | `w0`       | sine                                     | `sin a`          |
| 0x4c   | cos   | `w0`       | cosine                                   | `cos b`          |
| 0x4d   | tan   | `w0`       | tangent                                  | `tan b`          |
| 0x4e   | asin  | `w0`       | arc sine                                 | `asin a`         |
| 0x4f   | acos  | `w0`       | arc cosine                               | `acos b`         |
| 0x50   | atan  | `w0`       | arc tangent                              | `atan b`         |
| 0x51   | atan2 | `wv`       | p1 := atan2(p1, p2)                      | `atan2 a b`      |
| 0x52   | sqrt  | `w0`       | square root                              | `sqrt a`         |
| 0x53   | pow   | `wv`       | power (p1 := p1 ^ p2)                    | `pow a b`        |
| 0x54   | exp   | `w0`       | exponential (p1 := exp(p1))              | `exp a`          |
| 0x55   | log   | `w0`       | natural logarithm (p1 := ln(p1))         | `log a`          |
| 0x56   | log2  | `w0`       | base 2 logarithm                         | `log2 a`         |
| 0x57   | log10 | `w0`       | base 10 logarithm                        | `log10 a`        |

Beware that floating point operations do not behave as expected when using integer immediates.
They are **not** converted to floating point values, instead their underlying bits are reinterpreted
as a floating point value, leading to unexpected behaviour. Therefore, always use floating point
immediates like `1.5`.

While it is possible to use floating point operations with smaller registers like `cb`,
this usage results in undefined behaviour (there is no half/quarter precision float).

##### Unsigned Integer Operations

| Opcode | Asm  | Parameters | Operation          | Example       |
| ------ | ---- | ---------- | ------------------ | ------------- |
| 0x60   | uadd | `wv`       | unsigned add       | `add a 5`     |
| 0x61   | usub | `wv`       | unsigned subtract  | `sub a 8`     |
| 0x62   | umul | `wv`       | unsigned multiply  | `umul a b`    |
| 0x63   | udiv | `wv`       | unsigned divide    | `udiv a b`    |
| 0x64   | umod | `wv`       | unsigned remainder | `umod a b`    |
| 0x65   | umax | `wv`       | unsigned max       | `umax a 2`    |
| 0x66   | umin | `wv`       | unsigned min       | `umin a 0x42` |

##### Miscellaneous Operations

| Opcode | Asm   | Parameters | Operation                                                                              | Example    |
| ------ | ----- | ---------- | -------------------------------------------------------------------------------------- | ---------- |
| 0x70   | rand  | `w0`       | p1 := pseudo random float between 0 and 1                                              | `rand $0`  |
| 0x71   | rseed | `v0`       | set random seed                                                                        | `rseed 42` |
| 0x72   | itf   | `w0`       | convert integer to floating point                                                      | `itf a`    |
| 0x73   | fti   | `w0`       | convert floating point to integer                                                      | `fti a`    |
| 0x74   | utf   | `w0`       | convert unsigned integer to floating point                                             | `utf a`    |
| 0x75   | ftu   | `w0`       | convert floating point to unsigned integer                                             | `ftu a`    |
| 0x76   | ei    | `00`       | enable interrupts (for future use, currently nop)                                      | `ei`       |
| 0x77   | di    | `00`       | disable interrupts (for future use, currently nop)                                     | `di`       |
| 0x78   | stop  | `00`       | stop execution until an interrupt occurs (for future use, currently equivalent to hlt) | `stop`     |

When converting floating point values to int or uint, the conversion behaves like a c-style cast. This means the
fractional part is discarded, and if the magnitude of the float is too large for the receiving datatype, the result
is undefined. The result is also undefined when trying to convert a negative float to an unsigned int.

### The random number generator

The `rand` operation uses a pseudo random number generator, specifically a
[Linear congruential generator](https://en.wikipedia.org/wiki/Linear_congruential_generator) with a multiplier of
`214013`, an increment of `2541011` and a modulo of `2^32`. The random range is `0 - 0x7fff` and the initial seed
is `0x12345678`.

This means the `rand` operation always produces the same sequence of numbers if the seed is not changed.
Note that `rand` returns a random **float** between 0 and 1, not an integer. If you need the random integer,
it is found in the `R` register. To get a random integer without affecting any other registers, use `rand r`.

### Syscall standard library

The following syscalls shall be implemented by the runtime:

| Name      | Function                                                                                                                                     |
| --------- | -------------------------------------------------------------------------------------------------------------------------------------------- |
| print_u32 | Prints the topmost 32 bit value from the stack as an unsigned int                                                                            |
| print_i32 | Prints the topmost 32 bit value from the stack as a signed int                                                                               |
| print_f32 | Prints the topmost 32 bit value from the stack as a float                                                                                    |
| print     | Prints the zero-terminated string located at the memory address specified by the topmost value on the stack                                  |
| println   | Prints the zero-terminated string located at the memory address specified by the topmost value on the stack, followed by a newline character |

The table now includes the missing functions and their corresponding descriptions.

"Print" can mean just printing to stdout, but a more sophisticated runtime might define it otherwise.

### String hash function

tx8 defines the following string hash function. It is used by [syscalls](#syscalls), and may be used for other purposes in the future.

The function takes a string and returns a 32-bit hash value. See the C reference implementation:

```c
uint32_t str_hash(const char* s) {
    uint32 h = (uint8_t) *s;
    if (h != 0)
        for (++s; *s; ++s)
            h = (h << 5) - h + (uint32_t) *s;
    return h;
}
```

## Roms (Binary Files)

TX8 programs or games are distributed as binary files. These files must include a header at the top.
The file extension for tx8 roms is `.txr`.

| Bytes | Type                                 | Meaning / Content                                                                       |
| ----- | ------------------------------------ | --------------------------------------------------------------------------------------- |
| 0-3   | Magic bytes                          | `0x54 0x58 0x38 0x00` (or `TX8\0` in ascii)                                             |
| 4     | 8bit unsigned integer                | program name length in bytes (0 for no name)                                            |
| 5-6   | Little endian 16bit unsigned integer | description length in bytes (0 for no description)                                      |
| 7-10  | Little endian 32bit unsigned integer | Length of the actual binary data in bytes                                               |
| 11    | 8bit unsigned integer                | Checksum: XOR of all bytes in header (except this byte)                                 |
| 12-63 | Reserved                             | Reserved for future use (should be all-zero)                                            |
| 64+   | String                               | Program name (as many bytes as specified, not zero-terminated)                          |
| ...   | String                               | Description (as many bytes as specified, not zero-terminated)                           |
| Rest  | Binary                               | Actual binary data (instructions, assets, ...). This part is what is loaded into memory |
