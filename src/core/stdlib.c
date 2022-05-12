#include "tx8/core/log.h"

#include <tx8/core/cpu.h>

#define f(name) void tx_stdlib_##name(tx_CPU* cpu)

/// `print_u32(uint32 n)` - logs `n`
f(print_u32) {
    tx_uint32 val = tx_cpu_top32(cpu);
    tx_log("%x", val);
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

#undef f

#define r(name) tx_cpu_register_sysfunc(cpu, #name, &tx_stdlib_##name)

void tx_cpu_use_stdlib(tx_CPU* cpu) {
    r(print_u32);
    r(print);
    r(println);
}

#undef r
