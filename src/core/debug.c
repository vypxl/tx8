#include "tx8/core/debug.h"

#include "tx8/core/instruction.h"

#include <stdio.h>

void tx_debug_print_instruction(tx_CPU* cpu, tx_Instruction* inst) {
    printf("#%x: %s %x %x %x\n",
           cpu->p,
           tx_op_names[inst->opcode],
           tx_cpu_get_param_value(cpu, inst->parameters.p1, inst->parameters.mode_p1),
           tx_cpu_get_param_value(cpu, inst->parameters.p2, inst->parameters.mode_p2),
           tx_cpu_get_param_value(cpu, inst->parameters.p3, inst->parameters.mode_p3));
}
