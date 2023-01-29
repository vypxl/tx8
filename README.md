[![Build Status](https://github.com/vypxl/tx8/actions/workflows/test.yml/badge.svg)](https://github.com/vypxl/tx8/actions/workflows/test.yml)
[![Coverage Status](https://coveralls.io/repos/github/vypxl/tx8/badge.svg?branch=main)](https://coveralls.io/github/vypxl/tx8?branch=main)
[![License](https://img.shields.io/badge/license-%20MIT-blue.svg)](https://github.com/vypxl/tx8/blob/main/LICENSE)

# TX8

**WIP** The assembler as well as the bytecode interpreter should work as of now. I will create unit tests for both and
add a few stdlib functions next. The api and binary format is not yet finalized, expect changes all over the place.
As I am writing the tests, I will be modifying / augmenting the spec to correctly handle all features.

## About

TX8 is a fantasy console running custom bytecode from virtual cartridges, featuring
sound, music and graphics capabilities. More information in the [specification](https://github.com/vypxl/tx8/blob/main/spec/spec.md).

# In this repository

## TX8 Core

The tx8-core C library source code.
tx8-core includes header files which contain types and constants used throughout the TX8
ecosystem and an implementation of a bytecode interpreter.
You can also find the TX8 specification in this repository under `spec/`.

## TX8 Assembler

The assembler that can translate tx8-Assembly into bytecode files.

# Development

To start developing on TX8, you need `cmake >= 3.25`, `ninja` and `clang >= 15` or `gcc >= 12`.\
`Apple Clang` is **not** supported!\
Set up the development environment via `CC=<C compiler> CXX=<C++ compiler> cmake --preset debug"`.\
You can then build the test executable via `cmake --build --preset debug`\
and execute it via `ctest --preset debug` or directly via `build/debug/tx8-test`.

If you are using `clangd`, you should link `compile_commands.json`\
via `ln -s build/debug/compile_commands.json compile_commands.json` for `clangd` to work correctly.

To build the documentation, run `doxygen` in the project root directory.

TX8 uses Google Test for unit testing.
