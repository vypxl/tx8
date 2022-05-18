#include "tx8/core/log.h"

#include <tx8/core/cpu.h>

#define f(name) void tx_stdlib_##name(tx_CPU* cpu, void* data)

#pragma clang diagnostic ignored "-Wunused-parameter"

/// `print_u32(uint32 n)` - logs `n`
f(print_u32) {
    tx_uint32 val = tx_cpu_top32(cpu);
    tx_log("%x", val);
}

/// `print_i32(int32 n)` - logs `n` as a signed value
f(print_i32) {
    tx_num32 val;
    val.u = tx_cpu_top32(cpu);
    tx_log("%d", val.i);
}

f(print_f32) {
    tx_num32 val;
    val.u = tx_cpu_top32(cpu);
    tx_log("%f", val.f);
}

/// `print(char* s)` - logs the zero terminated string at `s`
f(print) {
    tx_uint32 addr = tx_cpu_top32(cpu);
    tx_log("%s", (char*) (cpu->mem + addr));
}

/// `println(char* s)` - Prints the zero terminated string at `s` with a trailing newline
f(println) {
    tx_uint32 addr = tx_cpu_top32(cpu);
    char* str = (char*) (cpu->mem + addr);
    tx_log("%s\n", str);
}

#pragma clang diagnostic warning "-Wunused-parameter"

#undef f

#define r(name) tx_cpu_register_sysfunc(cpu, #name, &tx_stdlib_##name, NULL)

void tx_cpu_use_stdlib(tx_CPU* cpu) {
    r(print_u32);
    r(print_i32);
    r(print_f32);
    r(print);
    r(println);
}

#undef r
