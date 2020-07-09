# TX8 Core

**WIP** I just finished implementing the bytecode interpreter, I did not test it yet, nor did I
implement any of the other features. The `sys` opcode is not implemented too.

This repository contains the tx8-core C library source code.
tx8-core includes header files which contain types and constants used thoughout the TX8
ecosystem and an implementation of a bytecode interpreter.
You can also find the TX8 specification in this repository under `spec/`.

## TX8

TX8 is a fantasy console running custom bytecode from virtual cartridges, featuring
sound, music and graphics capabilities. More information in the specification.

## Used Libraries

TX8 uses the khash library from [klib](https://github.com/attractivechaos/klib), which is licensed under the MIT License.
You can find the license text in the [header file](https://github.com/vypxl/tx8/blob/master/deps/khash.h).
