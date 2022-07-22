#include "tx8/core/debug.hpp"

#include "tx8/core/instruction.h"
#include "tx8/core/log.hpp"

void tx_debug_print_raw_instruction(tx_Instruction* inst) {
    tx::log_err(
        "{:x} ({}) {:x} {:x} (modes: {:x} {:x})\n",
        (tx_uint32) inst->opcode,
        tx_op_names[inst->opcode],
        inst->params.p1.value.u,
        inst->params.p2.value.u,
        (tx_uint32) inst->params.p1.mode,
        (tx_uint32) inst->params.p2.mode
    );
}

void tx_debug_print_instruction(tx_Instruction* inst) {
    tx::log_err("{}", tx_op_names[inst->opcode]);
    if (tx_param_count[inst->opcode] > 0) {
        tx::log_err(" ");
        tx_debug_print_parameter(&(inst->params.p1));
    }
    if (tx_param_count[inst->opcode] > 1) {
        tx::log_err(" ");
        tx_debug_print_parameter(&(inst->params.p2));
    }
    tx::log_err("\n");
}

void tx_debug_print_parameter(tx_Parameter* p) {
    tx_num32 v = p->value;
    switch (p->mode) {
        case tx_param_constant8: tx::log_err("<{:#x} | {}>", (tx_uint8) v.u, (tx_int8) v.i); break;
        case tx_param_constant16: tx::log_err("<{:#x} | {}>", (tx_uint16) v.u, (tx_int16) v.i); break;
        case tx_param_constant32: tx::log_err("<{:#x} | {} | {:.5f}>", v.u, v.i, v.f); break;
        case tx_param_absolute_address: tx::log_err("#{:x}", v.u); break;
        case tx_param_relative_address:
            if (v.i < 0) tx::log_err("$-{:x}", -v.i);
            else tx::log_err("${:x}", v.i);
            break;
        case tx_param_register_address: tx::log_err("@{}", tx_reg_names[v.u]); break;
        case tx_param_register: tx::log_err("{}", tx_reg_names[v.u]); break;
        case tx_param_label: tx::log_err("label(id: {:#x})", v.u); break;
        case tx_param_unused: break;
        default: tx::log_err("{{ unknown parameter mode {:#x}; value: {:#x} }}", (tx_uint32) p->mode, v.u); break;
    }
}
