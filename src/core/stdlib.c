#include <stdio.h>
#include <tx8/core/cpu.h>

#define f(name) void tx_stdlib_##name(tx_CPU* cpu)

f(print) {
    tx_uint32 val = tx_cpu_pop32(cpu);
    tx_cpu_push32(cpu, val);
    printf("[PRINT] %x", val);
}

#undef f

#define r(name) tx_cpu_register_sysfunc(cpu, #name, &tx_stdlib_##name)

void tx_cpu_use_stdlib(tx_CPU* cpu) { r(print); }

#undef r
