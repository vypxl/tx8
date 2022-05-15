#include "tx8/asm/types.h"

#include <stdlib.h>
#include <tx8/core/instruction.h>
#include <tx8/core/types.h>
#include <tx8/core/log.h>

void tx_asm_print_parameter(tx_asm_Parameter* p) {
    switch (p->mode) {
        case tx_param_constant8: tx_log("0x%xu8", (tx_uint8)p->value.u); break;
        case tx_param_constant16: tx_log("0x%xu16", (tx_uint16)p->value.u); break;
        case tx_param_constant32: tx_log("0x%xu32", p->value.u); break;
        case tx_param_absolute_address: tx_log("#%x", p->value.u); break;
        case tx_param_relative_address:
            if (p->value.i < 0) tx_log("$-%x", -p->value.i);
            else
                tx_log("$%x", p->value.i);
            break;
        case tx_param_register_address: tx_log("@%s", tx_reg_names[p->value.u]); break;
        case tx_param_register: tx_log("%s", tx_reg_names[p->value.u]); break;
        default: tx_log("{0x%x}", p->value.u); break;
    }
}

static inline tx_uint32 tx_asm_param_size(tx_uint32 mode) {
    if (mode >= 0 && mode <= tx_param_register_address) return tx_param_sizes[mode];
    if (mode == tx_asm_param_label_id) return tx_param_sizes[tx_param_constant32];
    return 0xffffffff;
}

tx_uint8 tx_asm_parameter_generate_binary(tx_asm_Parameter* p, tx_uint8* buf) {
    switch (p->mode) {
        case tx_param_constant8:
        case tx_param_register:
        case tx_param_register_address: buf[0] = (tx_uint8)p->value.u; break;
        case tx_param_constant16: ((tx_uint16*)(buf))[0] = (tx_uint16)p->value.u; break;
        case tx_param_constant32:
        case tx_param_absolute_address:
        case tx_param_relative_address: ((tx_uint32*)(buf))[0] = (tx_uint32)p->value.u; break;
        default: break;
    }

    return tx_asm_param_size(p->mode);
}

tx_uint32 tx_asm_instruction_length(tx_asm_Instruction* inst) {
    return 1 + tx_param_mode_bytes[tx_param_count[inst->opcode]] + tx_asm_param_size(inst->p1.mode)
           + tx_asm_param_size(inst->p2.mode) + tx_asm_param_size(inst->p3.mode);
}

void tx_asm_print_instruction(tx_asm_Instruction* inst) {
    tx_log("%s", tx_op_names[inst->opcode]);
    if (tx_param_count[inst->opcode] > 0) {
        tx_log(" ");
        tx_asm_print_parameter(&(inst->p1));
    }
    if (tx_param_count[inst->opcode] > 1) {
        tx_log(" ");
        tx_asm_print_parameter(&(inst->p2));
    }
    if (tx_param_count[inst->opcode] > 2) {
        tx_log(" ");
        tx_asm_print_parameter(&(inst->p3));
    }
    tx_log("\n");
}

void tx_asm_instruction_generate_binary(tx_asm_Instruction* inst, tx_uint8* buf) {
    buf[0] = inst->opcode;
    buf[1] = ((inst->p1.mode) << 4) | inst->p2.mode;
    buf[2] = (inst->p3.mode << 4);

    tx_uint8* _buf = buf + 1 + tx_param_mode_bytes[tx_param_count[inst->opcode]];
    _buf += tx_asm_parameter_generate_binary(&(inst->p1), _buf);
    _buf += tx_asm_parameter_generate_binary(&(inst->p2), _buf);
    tx_asm_parameter_generate_binary(&(inst->p3), _buf);
}

void tx_asm_LL_destroy(tx_asm_LL* ll) {
    if (ll == NULL) return;

    tx_asm_LL* next = tx_asm_LL_next(ll);
    free(ll->item);
    free(ll);
    tx_asm_LL_destroy(next);
}

tx_asm_LL* tx_asm_LL_next(tx_asm_LL* ll) {
    if (ll == NULL) return NULL;
    return ll->next;
}

void tx_asm_LL_insert(tx_asm_LL** ll, void* item) {
    tx_asm_LL* new_ll = malloc(sizeof(tx_asm_LL));
    new_ll->item      = item;
    new_ll->next      = NULL;

    // empty -> make first item
    if (*ll == NULL) *ll = new_ll;
    // at end of list -> append
    else if ((*ll)->next == NULL)
        (*ll)->next = new_ll;
    // otherwise -> insert and shift all following elements one to the right
    else {
        new_ll->next = (*ll)->next;
        (*ll)->next  = new_ll;
    }
}

void* tx_asm_LL_get(tx_asm_LL* ll, tx_uint32 idx) {
    tx_asm_LL* next = ll;
    for (tx_uint32 i = 0; i < idx; ++i) {
        if (next == NULL) return NULL;
        next = tx_asm_LL_next(next);
    }
    return next->item;
}

void tx_asm_LL_append(tx_asm_LL** ll, void* item) {
    tx_asm_LL** next = ll;
    if (*next != NULL)
        while ((*next)->next != NULL)
            next = &((*next)->next);
    tx_asm_LL_insert(next, item);
}
