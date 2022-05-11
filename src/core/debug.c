#include "tx8/core/debug.h"

#include "tx8/core/instruction.h"

#include <stdio.h>

void tx_debug_fprint_instruction(tx_CPU* cpu, tx_Instruction* inst, FILE* f) {
    fprintf(f, "#%x: %s %x %x %x\n",
           cpu->p,
           tx_op_names[inst->opcode],
           tx_cpu_get_param_value(cpu, inst->parameters.p1, inst->parameters.mode_p1),
           tx_cpu_get_param_value(cpu, inst->parameters.p2, inst->parameters.mode_p2),
           tx_cpu_get_param_value(cpu, inst->parameters.p3, inst->parameters.mode_p3));
}

void tx_debug_print_instruction(tx_CPU* cpu, tx_Instruction* inst) {
    tx_debug_fprint_instruction(cpu, inst, stdout);
}

// TODO use methods that currently live in tx_asm
void tx_debug_fprint_raw_instruction(tx_CPU* cpu, tx_Instruction* inst, FILE* f) {
    fprintf(f, "#%x: %x (%s) %x %x %x (modes: %x %x %x)\n",
            cpu->p,
            inst->opcode, tx_op_names[inst->opcode],
            inst->parameters.p1, inst->parameters.p2, inst->parameters.p3,
            inst->parameters.mode_p1, inst->parameters.mode_p2, inst->parameters.mode_p3);
}

void tx_debug_print_raw_instruction(tx_CPU* cpu, tx_Instruction* inst) {
    tx_debug_fprint_raw_instruction(cpu, inst, stdout);
}
