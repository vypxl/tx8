#include "tx8/core/stdlib.hpp"

#include "tx8/core/cpu.hpp"
#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"

#define f(name) void name(CPU& cpu)

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace tx::stdlib {
    /// `print_u32(uint32 n)` - logs `n`
    f(print_u32) {
        uint32 val = cpu.top();
        tx::log("{}", val);
    }

    /// `print_i32(int32 n)` - logs `n` as a signed value
    f(print_i32) {
        num32 val;
        val.u = cpu.top();
        tx::log("{}", val.i);
    }

    f(print_f32) {
        num32 val;
        val.u = cpu.top();
        tx::log("{}", val.f);
    }

    /// `print(char* s)` - logs the zero terminated string at `s`
    f(print) {
        uint32 addr = cpu.top();
        tx::log("{}", (char*) (cpu.mem.data() + addr));
    }

    /// `println(char* s)` - Prints the zero terminated string at `s` with a trailing newline
    f(println) {
        uint32 addr = cpu.top();
        char*  str  = (char*) (cpu.mem.data() + addr);
        tx::log("{}\n", str);
    }

    /// `put(char c)` - Prints the character `c`
    f(put) {
        char c = (char) cpu.top();
        tx::log("{}", c);
    }

    /// `get()` - Reads a character from stdin and pushes it to the stack
    f(get) {
        char c = (char) getc(stdin);
        cpu.push(c, tx::ValueSize::Byte);
    }

#pragma clang diagnostic warning "-Wunused-parameter"


#define r(name) cpu.register_sysfunc(#name, &(name))

    void use_stdlib(CPU& cpu) {
        r(print_u32);
        r(print_i32);
        r(print_f32);
        r(print);
        r(println);
        r(put);
        r(get);
    }

} // namespace tx::stdlib

#undef r
#undef f
