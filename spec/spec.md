# TX8 - Spec

## Graphics

- 8bit color (256 color)
- 512x288 pixel display
- 16x16 tiles (32x18 tile display)

## Sound & Music

- 8 instruments
- 8 effects
- 32-note sound samples
- music engine: 4-channel sound sample playback

## Miscellaneous

- Assembly instruction set
- Many convenience functions builtin
- 9 button input
- 8 megabyte cartridge (0x800000 bytes) for code, textures, map / data, sound, music

## Memory

- 16 megabyte memory
- 4 byte adresses
- reading or writing out of bounds (beyond 0xffffff) truncates the address to 24-bit: `0x12345678 => 0x345678`
- 4mb (#0x000000 - #0x3fffff) system reserved / registers (read/writable)
- 8mb (#0x400000 - #0xbfffff) loaded cartrige data (read/writable)
- 4mb (#0xc00000 - #0xffffff) work RAM (read/writable)

## Assembly Programming (TX8-Asm)

Programming TX8 is generally done in assembly. The virtual processor and assembly language used is described below.

### Registers

All CPU registers are 32-bit. Smaller parts can be accessed using suffixes:
Given the register `X`, the full register is available via `X` or `Xi`,
the lower 2 bytes via `Xs` and the lowest byte via `Xb`.
The names are case insensitive.
(You can memorize the suffixes by thinking of: `int`, `short` and `byte`)
The available CPU registers are described here:

- `A`,`B`,`C`,`D`: general purpose registers
- `O`: offset for relative addressing
- `S`: Stack pointer
- `P`: Program counter / Instruction Pointer

The program counter can have any value in the range 0..0xfffff0 to prevent parsing instruction outside of memory, because an instruction can be up to 15 bytes long.

### Stack

The Stack lives at #0xc02000 by default and grows downward, so it has a size of 8192 (0x2000) bytes.
However, you could set the stack pointer `S` to anything you want, if you need a bigger or smaller stack.
There is no over/underflow detection, so be careful!

### Instructions

Every instruction consists of a 1-byte opcode followed by 0-2 bytes parameter modes and 0-3 parameters.
An example opcode looks like this: `op 42 bi #43`.
Every instruction takes up one line.
Comments can be added after a semi-colon (;).

### Parameter Modes

There are 5 ways to give parameters to instructions:

- Constant mode (decimal, hex, binary and float): `lda 42`, `lda 0x2a`, `lda 0b101010`, `lda 42.1337`
- Absolute address mode (`#` prefix): `lda #42`
- Relative address mode (`$` prefix, dependent on register `O`): `lda $-42`
- Register mode: `lda bi`
- Register address mode: `lda @bs`

In binary, parameter modes are indicated by the 0-2 bytes after the opcode. Every 4 bits represent one
parameter mode, the last 4 bits are unused (only 3-param ops).

| Code | Mode             |
| ---- | ---------------- |
| 0x0  | Unused           |
| 0x1  | Constant 8bit    |
| 0x2  | Constant 16bit   |
| 0x3  | Constant 32bit   |
| 0x4  | Absolute address |
| 0x5  | Relative address |
| 0x6  | Register         |
| 0x7  | Register address |

An example binary instruction looks like this:

```plain
asm: max a #0xc04023 35
bin: op mm m_ p1 p2 p2 p2 p3
     39 64 10 00 23 40 c0 23
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

### Instruction List

Parameters are described like this: Three characters, one for each parameter:

- `0` for unused
- `v` for values (constants, addresses, registers)
- `w` for writable (addresses, registers)
- `c` for constants

#### Flow Control

Comparing jump instruction all operate like this: `if parameter1 <comparison> parameter2 then jump to parameter3`.

| Opcode | Asm | Parameters | Operation                                | Example             |
| ------ | --- | ---------- | ---------------------------------------- | ------------------- |
| 0x00   | nop | `000`      | no operation                             | `nop`               |
| 0x01   | jmp | `v00`      | jump to address                          | `jmp :label`        |
| 0x02   | jeq | `vvv`      | jump if equal                            | `jeq 42 42 :branch` |
| 0x03   | jne | `vvv`      | jump if not equal                        | `jne 5 7 :branch`   |
| 0x04   | jgt | `vvv`      | jump if greater than                     | `jgt 7 5 :branch`   |
| 0x05   | jge | `vvv`      | jump if greater than or equal to         | `jge 8 6 :branch`   |
| 0x06   | jlt | `vvv`      | jump if less than                        | `jlt 2 4 :branch`   |
| 0x07   | jle | `vvv`      | jump if less than or equal to            | `jle 4 4 :branch`   |
| 0x08   | cal | `v00`      | call function                            | `cal :fun`          |
| 0x09   | ret | `000`      | return from function                     | `ret`               |
| 0x0a   | sys | `c00`      | call system function (more further down) | `sys &PRINT`        |
| 0x0b   | hlt | `000`      | halt / stop execution                    | `hlt`               |

##### Calling Convention

The calling convention of TX8 is very similar to [CDECL](https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#CDECL)
To properly call a function / subroutine, the caller must follow these steps:

- push all function parameters onto the stack in right-to-left order
- use a `cal` instruction to call the function
- clean the stack (add the byte size of all parameters to the stack pointer)

The callee must follow these steps:

- use parameters from the stack (remember to skip the 4-byte return address)
- run code
- make sure to restore the stack pointer
- use a `ret` instruction to return to the caller (pops the topmost address off the stack and jumps to it)

The callee does not have to preserve any register values.

#### Loading and Storing

The shortcuts for registers are for convenience, `mov` can be used for everything.
`mov` moves a different amount of bytes according to its parameters:

- `mov register something` moves as many bytes as the register has (A=4, As=2, Ab=1) or how many the other parameter might have, whichever is lower
- `mov address register` moves as many bytes as the register has (A=4, As=2, Ab=1)
- `mov address address` moves one byte
- `mov address constant` moves as many bytes as the constant has

Use `mxv` to always move 4 bytes.

Push and pop behave like this:

- `psh register` pushes as many bytes as the register has (A=4, As=2, Ab=1)
- `psh address` pushes 4 bytes
- `psh constant` pushes as many bytes as the constant has (A=4, As=2, Ab=1)
- `pop register` pops as many bytes as the register has (A=4, As=2, Ab=1)
- `pop address` pops 4 bytes

| Opcode | Asm | Parameters | Operation                                          | Example         |
| ------ | --- | ---------- | -------------------------------------------------- | --------------- |
| 0x10   | mov | `wv0`      | move value (parameter2) into parameter1 (p1 := p2) | `mov A 42`      |
| 0x11   | mxv | `wv0`      | move 4 bytes from parameter2 into parameter1       | `mxv a #c01234` |
| 0x12   | lda | `v00`      | load value into register A                         | `lda 42`        |
| 0x13   | sta | `w00`      | store value from register A into parameter1        | `sta $2`        |
| 0x14   | ldb | `v00`      | load value into register B                         | `ldb 55`        |
| 0x15   | stb | `w00`      | store value from register B into parameter1        | `stb a`         |
| 0x16   | ldc | `v00`      | load value into register C                         | `ldc $32`       |
| 0x17   | stc | `w00`      | store value from register C into parameter1        | `stc #c01234`   |
| 0x18   | ldd | `v00`      | load value into register D                         | `ldd @cb`       |
| 0x19   | std | `w00`      | store value from register D into parameter1        | `std $-35`      |
| 0x1a   | zer | `w00`      | zero out parameter1 (addresses 1 byte)             | `zer a`         |
| 0x1b   | psh | `v00`      | push onto stack                                    | `psh a`         |
| 0x1c   | pop | `w00`      | pop from stack                                     | `pop a`         |

#### Arithmetic

All arithmetic operations are in-place on the first parameter, so an `add a 5` increments register A by 5. When operating on registers, the number of
bytes the operation influences is determined by that. When operating on addresses, 4 bytes will be influenced.
Normal instructions operate on signed integers, if you have unsigned integers
or floats, you have to use the respective special instructions.

| Opcode | Asm | Parameters | Operation                                  | Example            |
| ------ | --- | ---------- | ------------------------------------------ | ------------------ |
| 0x20   | inc | `w00`      | increment                                  | `inc a`            |
| 0x21   | dec | `w00`      | decrement                                  | `dec $1`           |
| 0x22   | add | `wv0`      | add                                        | `add a 5`          |
| 0x23   | sub | `wv0`      | substract                                  | `sub a 8`          |
| 0x24   | mul | `wv0`      | multiply                                   | `mul a -2`         |
| 0x25   | div | `wv0`      | divide                                     | `div a 5`          |
| 0x26   | mod | `wv0`      | remainder                                  | `mod a 7`          |
| 0x27   | and | `wv0`      | bitwise and                                | `and c 0b10011010` |
| 0x28   | ora | `wv0`      | or                                         | `or c 0x7f`        |
| 0x29   | not | `w00`      | not                                        | `not c`            |
| 0x2a   | nnd | `wv0`      | nand                                       | `nand c d`         |
| 0x2b   | xor | `wv0`      | xor                                        | `xor c d`          |
| 0x2c   | shr | `wv0`      | bitshift right                             | `shr b 2`          |
| 0x2d   | shl | `wv0`      | bitshift left                              | `shl b 1`          |
| 0x2e   | ror | `wv0`      | bitrotate right                            | `ror b 3`          |
| 0x2f   | rol | `wv0`      | bitrotate left                             | `rol b 7`          |
| 0x30   | fin | `w00`      | floating point increment                   | `fin a`            |
| 0x31   | fde | `w00`      | floating point decrement                   | `fde $1`           |
| 0x32   | fad | `wv0`      | floating point add                         | `fad a 5.0`        |
| 0x33   | fsu | `wv0`      | floating point substract                   | `fsu a 8`          |
| 0x34   | fmu | `wv0`      | floating point multiply                    | `fmu a -2.7924`    |
| 0x35   | fdi | `wv0`      | floating point divide                      | `fdi a 5.2`        |
| 0x36   | fmo | `wv0`      | floating point remainder                   | `fmo a 7`          |
| 0x37   | itf | `w00`      | convert integer to floating point          | `itf a`            |
| 0x38   | fti | `w00`      | convert floating point to integer          | `fti a`            |
| 0x39   | max | `wvv`      | p1 := max(p2, p3)                          | `max a b 3`        |
| 0x3a   | min | `wvv`      | p1 := min(p2, p3)                          | `min a b 3`        |
| 0x3b   | fmx | `wvv`      | floating point max                         | `fmx a b 2.5`      |
| 0x3c   | fmn | `wvv`      | floating point min                         | `fmn a b 2.5`      |
| 0x3d   | sin | `w00`      | sine                                       | `sin a`            |
| 0x3e   | cos | `w00`      | cosine                                     | `cos b`            |
| 0x3f   | tan | `w00`      | tangent                                    | `tan b`            |
| 0x40   | atn | `wvv`      | p1 := atan2(p2, p3)                        | `atn a b c`        |
| 0x41   | sqt | `w00`      | square root                                | `sqt a`            |
| 0x42   | abs | `w00`      | absolute value                             | `abs a`            |
| 0x43   | fab | `w00`      | floating point absolute value              | `fab b`            |
| 0x44   | rnd | `w00`      | p1 := pseudo random float between 0 and 1  | `rnd $0`           |
| 0x45   | rsd | `v00`      | set random seed                            | `rsd 42`           |
| 0x46   | umu | `wv0`      | unsigned multiply                          | `umu a b`          |
| 0x47   | udi | `wv0`      | unsigned divide                            | `udi a b`          |
| 0x48   | umx | `wvv`      | unsigned max                               | `umx a b 2`        |
| 0x49   | umn | `wvv`      | unsigned min                               | `umn a b 0x42`     |
| 0x4a   | utf | `w00`      | convert unsinged integer to floating point | `utf a`            |
| 0x4b   | ftu | `w00`      | convert floating point to unsigned integer | `ftu a`            |

## Binary Files

TX8 programs or games are distributed as binary files. These files must include a header at the top.

| Bytes | Type                                 | Meaning / Content                                                                         |
| ----- | ------------------------------------ | ----------------------------------------------------------------------------------------- |
| 0-3   | Magic bytes                          | `0x54 0x58 0x38` (or `TX8` in ascii)                                                      |
| 4     | Little endian 8bit unsigned integer  | program name length in bytes (0 for no name)                                              |
| 5-6   | Little endian 16bit unsigned integer | description length in bytes (0 for no description)                                        |
| 7-10  | Little endian 32bit unsigned integer | Length of the actual binary data in bytes                                                 |
| 11-64 | Reserved                             | Reserved for future use (should be all-zero)                                              |
| 65+   | String                               | Program name (as many bytes as specified, not zero-terminated)                            |
| ...   | String                               | Description (as many bytes as specified, not zero-terminated)                             |
| Rest  | Binary                               | Actual binary data (instructions, assets, ...). This part is what gets loaded into memory |
