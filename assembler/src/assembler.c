#include "assembler.h"

#include "asm_types.h"
#include "instruction.h"
#include "types.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char*        strdup(char* src);
extern int   tx_asm_yyerror(char* msg, ...);
extern int   tx_asm_yyparse();
extern FILE* tx_asm_yyin;
extern int   tx_asm_yylineno;

tx_asm_Assembler* tx_asm_yyasm;

void tx_asm_init_assembler(tx_asm_Assembler* asm) {
    asm->labels        = NULL;
    asm->instructions  = NULL;
    asm->position      = 0;
    asm->last_label_id = 0;
}

void tx_asm_run_assembler(tx_asm_Assembler* asm, FILE* input) {
    tx_asm_yyin  = input;
    tx_asm_yyasm = asm;
    tx_asm_yyparse();
    tx_asm_assembler_convert_labels(asm);
}

void tx_asm_assembler_write_binary(tx_asm_Assembler* asm, FILE* output) {
    tx_uint8 buf[tx_INSTRUCTION_MAX_LENGTH];
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Instruction*, inst, asm->instructions)
        tx_asm_instruction_write_binary(inst, buf);
        fwrite(buf, tx_asm_instruction_length(inst), 1, output);
    tx_asm_LL_FOREACH_END
}

void tx_asm_destroy_assembler(tx_asm_Assembler* asm) {
    tx_asm_LL_destroy(asm->labels);
    tx_asm_LL_destroy(asm->instructions);
}

void tx_asm_error(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    fprintf(stderr, "Line %d: ", tx_asm_yylineno);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}

// return the id of the existing or newly created label
tx_uint32 tx_asm_assembler_new_label(tx_asm_Assembler* asm, char* name) {
    // search for an existing label with the same name and return its id if found
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Label*, label, asm->labels)
        if (strcmp(name, label->name) == 0) return label->id;
    tx_asm_LL_FOREACH_END

    // create a new label
    tx_asm_Label* label = malloc(sizeof(tx_asm_Label));
    label->name         = strdup(name);
    label->id           = ++(asm->last_label_id);
    label->position     = 0xffffffff;

    // insert the new label into the list
    tx_asm_LL_append(&(asm->labels), label);

    // return the new id
    return label->id;
}

// returns the id of the label whose position was set
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* asm, char* name) {
    // find label that matches the name
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Label*, label, asm->labels)
        if (strcmp(name, label->name) == 0) {
            // error if the matched label already has a position set
            if (label->position != 0xffffffff)
                tx_asm_error("Cannot create two or more labels with the same name '%s'\n", name);
            else {
                // set position of found label
                label->position = asm->position;
                return label->id;
            }
        }
    tx_asm_LL_FOREACH_END

    // error if no match was found
    tx_asm_error("No label '%s' to set position to\n", name);
    return 0;
}

tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* asm, tx_uint32 id) {
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Label*, label, asm->labels)
        if (label->id == id)
            if (label->position != 0xffffffff) {
                return label->position;
            } else {
                tx_asm_error("Label '%s' has no corresponding location", label->name);
                return 0;
            }
    tx_asm_LL_FOREACH_END

    tx_asm_error("No label with id %d found", id);
    return 0;
}

void tx_asm_assembler_add_instruction(tx_asm_Assembler* asm, tx_asm_Instruction inst) {
    tx_asm_Instruction* _inst = malloc(sizeof(tx_asm_Instruction));
    memcpy(_inst, &inst, sizeof(tx_asm_Instruction));
    tx_asm_LL_append(&(asm->instructions), _inst);
    asm->position += tx_asm_instruction_length(_inst);
}

void tx_asm_assembler_convert_labels(tx_asm_Assembler* asm) {
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Instruction*, inst, asm->instructions)
        if (inst->p1.mode == tx_asm_param_label_id) {
            inst->p1.value.u = tx_asm_assembler_convert_label(asm, inst->p1.value.u);
            inst->p1.mode    = tx_param_constant32;
        }
        if (inst->p2.mode == tx_asm_param_label_id) {
            inst->p2.value.u = tx_asm_assembler_convert_label(asm, inst->p2.value.u);
            inst->p2.mode    = tx_param_constant32;
        }
        if (inst->p3.mode == tx_asm_param_label_id) {
            inst->p3.value.u = tx_asm_assembler_convert_label(asm, inst->p3.value.u);
            inst->p3.mode    = tx_param_constant32;
        }
    tx_asm_LL_FOREACH_END
}

void tx_asm_assembler_print_instructions(tx_asm_Assembler* asm) {
    tx_uint32 pos = 0;
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Instruction*, inst, asm->instructions)
        printf("[#%04x:%02x]", pos, tx_asm_instruction_length(inst));
        pos += tx_asm_instruction_length(inst);
        tx_asm_print_instruction(inst);
    tx_asm_LL_FOREACH_END
}

void tx_asm_assembler_print_labels(tx_asm_Assembler* asm) {
    tx_asm_LL_FOREACH_BEGIN(tx_asm_Label*, label, asm->labels)
        printf(":%s [%x] = #%x\n", label->name, label->id, label->position);
    tx_asm_LL_FOREACH_END
}
