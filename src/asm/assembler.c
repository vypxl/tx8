#include "tx8/asm/assembler.h"

#include "tx8/asm/types.h"
#include "tx8/core/cpu.h"
#include "tx8/core/debug.h"
#include "tx8/core/log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <tx8/core/instruction.h>
#include <tx8/core/types.h>

char*        strdup(char* src);
extern int   tx_asm_yyerror(char* msg, ...);
extern int   tx_asm_yyparse();
extern FILE* tx_asm_yyin;
extern int   tx_asm_yylineno;
extern void* tx_asm_yy_scan_bytes (const char * yybytes, int _yybytes_len);

#define tx_asm_INVALID_LABEL_ADDRESS 0xffffffff

tx_asm_Assembler* tx_asm_yyasm;

void tx_asm_init_assembler(tx_asm_Assembler* as) {
    tx_array_label_init(&(as->labels), 0xffff);
    tx_array_instruction_init(&(as->instructions), 0xffff);
    as->position      = 0;
    as->last_label_id = 1;
    as->error         = 0;
}

int tx_asm_run_assembler_file(tx_asm_Assembler* as, FILE* input) {
    tx_asm_yyin  = input;
    tx_asm_yyasm = as;
    tx_asm_yyparse();
    tx_asm_assembler_convert_labels(as);

    return as->error;
}

int tx_asm_run_assembler_buffer(tx_asm_Assembler* as, char* buf, int size) {
    tx_asm_yy_scan_bytes(buf, size);
    tx_asm_yyasm = as;
    tx_asm_yyparse();
    tx_asm_assembler_convert_labels(as);

    return as->error;
}

bool tx_asm_assembler_write_binary_file(tx_asm_Assembler* as, FILE* output) {
    if (as->error != 0) {
        tx_log_err("Assembler encountered an error, did not write binary file.\n");
        return false;
    }
    tx_uint8 buf[tx_INSTRUCTION_MAX_LENGTH];
    for (unsigned i = 0; i < as->instructions.front; i++) {
        tx_Instruction* inst = as->instructions.data + i;
        tx_asm_instruction_generate_binary(inst, buf);
        fwrite(buf, inst->len, 1, output);
    }

    return true;
}

bool tx_asm_assembler_generate_binary(tx_asm_Assembler* as, tx_uint8* rom_dest) {
    if (as->error != 0) {
        tx_log_err("Assembler encountered an error, did not generate binary.\n");
        return false;
    }

    tx_uint32 pos = 0;

    for (unsigned i = 0; i < as->instructions.front; i++) {
        tx_Instruction* inst = as->instructions.data + i;
        tx_asm_instruction_generate_binary(inst, rom_dest + pos);
        pos += inst->len;
    }

    return true;
}

void tx_asm_destroy_assembler(tx_asm_Assembler* as) {
    tx_array_label_destroy(&as->labels);
    tx_array_instruction_destroy(&as->instructions);
}

void tx_asm_error(tx_asm_Assembler* as, char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    tx_logv_err(format, argptr);
    as->error = 1;
    va_end(argptr);
}

tx_uint32 tx_asm_assembler_handle_label(tx_asm_Assembler* as, char* name) {
    // search for an existing label with the same name and return its id if found
    for (unsigned i = 0; i < as->labels.front; i++) {
        tx_Label* label = as->labels.data + i;
        if (strcmp(name, label->name) == 0) return label->id;
    }

    // create a new label
    tx_Label label;
    label.name     = strdup(name);
    label.id       = ++(as->last_label_id);
    label.position = tx_asm_INVALID_LABEL_ADDRESS;

    // insert the new label into the list
    tx_array_label_insert(&(as->labels), label);

    // return the new id
    return label.id;
}

// returns the id of the label whose position was set
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* as, char* name) {
    // find label that matches the name
    for (unsigned i = 0; i < as->labels.front; i++) {
        tx_Label* label = as->labels.data + i;
        if (strcmp(name, label->name) == 0) {
            // error if the matched label already has a position set
            if (label->position != tx_asm_INVALID_LABEL_ADDRESS)
                tx_asm_error(
                    as, "Cannot create two or more labels with the same name '%s'\n", name);
            else {
                // set position of found label
                // TODO fix position offset hack
                label->position = tx_ROM_START + as->position;
                return label->id;
            }
        }
    }

    // error if no match was found
    tx_asm_error(as, "No label '%s' to set position to\n", name);
    return 0;
}

tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* as, tx_uint32 id) {
    for (unsigned i = 0; i < as->labels.front; i++) {
        tx_Label* label = as->labels.data + i;
        if (label->id == id) {
            if (label->position != tx_asm_INVALID_LABEL_ADDRESS) return label->position;

            // error if label does not have a position set
            tx_asm_error(as, "Label '%s' has no corresponding location", label->name);
            return 0;
        }
    }

    tx_asm_error(as, "No label with id %d found", id);
    return 0;
}

void tx_asm_assembler_add_instruction(tx_asm_Assembler* as, tx_Instruction inst) {
    tx_asm_instruction_calculate_length(&inst);
    tx_array_instruction_insert(&(as->instructions), inst);
    as->position += inst.len;
}

void tx_asm_assembler_convert_labels(tx_asm_Assembler* as) {
    for (unsigned i = 0; i < as->instructions.front; i++) {
        tx_Instruction* inst = as->instructions.data + i;
        if (inst->params.p1.mode == tx_param_label) {
            inst->params.p1.value.u = tx_asm_assembler_convert_label(as, inst->params.p1.value.u);
            inst->params.p1.mode    = tx_param_constant32;
        }
        if (inst->params.p2.mode == tx_param_label) {
            inst->params.p2.value.u = tx_asm_assembler_convert_label(as, inst->params.p2.value.u);
            inst->params.p2.mode    = tx_param_constant32;
        }
    }
}

void tx_asm_assembler_print_instructions(tx_asm_Assembler* as) {
    tx_uint32 pos = 0;
    for (unsigned i = 0; i < as->instructions.front; i++) {
        tx_Instruction* inst = as->instructions.data + i;
        tx_log_err("[asm] [#%04x:%02x] ", pos, inst->len);
        pos += inst->len;
        tx_debug_print_instruction(inst);
    }
}

void tx_asm_assembler_print_labels(tx_asm_Assembler* as) {
    for (unsigned i = 0; i < as->labels.front; i++) {
        tx_Label* label = as->labels.data + i;
        tx_log_err("[asm] :%s [%x] = #%x\n", label->name, label->id, label->position);
    }
}
