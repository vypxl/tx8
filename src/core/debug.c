#include "tx8/core/debug.h"

#include "tx8/core/instruction.h"
#include "tx8/core/log.h"

#include <stdio.h>

void tx_debug_print_instruction(tx_CPU* cpu, tx_Instruction* inst) {
    tx_log_err("#%x: %s %x %x\n",
           cpu->p,
           tx_op_names[inst->opcode],
           tx_cpu_get_param_value(cpu, inst->params.p1),
           tx_cpu_get_param_value(cpu, inst->params.p2));
}

// TODO use methods that currently live in tx_asm
void tx_debug_print_raw_instruction(tx_CPU* cpu, tx_Instruction* inst) {
    tx_log_err("#%x: %x (%s) %x %x (modes: %x %x)\n",
            cpu->p,
            inst->opcode, tx_op_names[inst->opcode],
            inst->params.p1.value.u, inst->params.p2.value.u,
            inst->params.p1.mode, inst->params.p2.mode);
}
