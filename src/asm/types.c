#include "tx8/asm/types.h"

#include <tx8/core/instruction.h>
#include <tx8/core/types.h>

tx_uint8 tx_asm_parameter_generate_binary(tx_Parameter* p, tx_uint8* buf) {
    switch (p->mode) {
        case tx_param_constant8:
        case tx_param_register:
        case tx_param_register_address: buf[0] = (tx_uint8)p->value.u; break;
        case tx_param_constant16: memcpy(buf, &(p->value.u), sizeof(tx_uint16)); break;
        case tx_param_constant32:
        case tx_param_absolute_address:
        case tx_param_relative_address: memcpy(buf, &(p->value.u), sizeof(tx_uint32)); break;
        default: break;
    }

    return tx_param_sizes[p->mode];
}

tx_uint32 tx_asm_instruction_calculate_length(tx_Instruction* inst) {
    return inst->len = 1 + tx_param_mode_bytes[tx_param_count[inst->opcode]] + tx_param_sizes[inst->params.p1.mode]
           + tx_param_sizes[inst->params.p2.mode];
}

void tx_asm_instruction_generate_binary(tx_Instruction* inst, tx_uint8* buf) {
    buf[0] = inst->opcode;
    if (inst->params.p1.mode != 0) buf[1] = ((inst->params.p1.mode) << 4) | inst->params.p2.mode;

    tx_uint8* _buf = buf + 1 + tx_param_mode_bytes[tx_param_count[inst->opcode]];
    _buf += tx_asm_parameter_generate_binary(&(inst->params.p1), _buf);
    tx_asm_parameter_generate_binary(&(inst->params.p2), _buf);
}
