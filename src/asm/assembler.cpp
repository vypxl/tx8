#include "tx8/asm/assembler.hpp"

#include "tx8/asm/types.hpp"
#include "tx8/core/cpu.hpp"
#include "tx8/core/debug.hpp"
#include "tx8/core/log.hpp"

#include <cstdarg>
#include <sstream>
#include <tx8/core/instruction.h>
#include <tx8/core/types.h>

//extern "C" int   tx_lex_yylineno;
//extern "C" int   tx_asm_yyparse();
//extern "C" FILE* tx_asm_yyin;
//extern "C" void* tx_asm_yy_scan_bytes(const char* yybytes, int _yybytes_len);
//extern void* tx::Lexer::yy_scan_bytes(const char* yybytes, int _yybytes_len);
//extern int tx::

#define tx_asm_INVALID_LABEL_ADDRESS 0xffffffff

tx_asm_Assembler* tx_asm_yyasm;

void tx_asm_init_assembler(tx_asm_Assembler* as) {
    as->position      = 0;
    as->last_label_id = 1;
    as->error         = 0;
}

tx_asm_Assembler::tx_asm_Assembler() : parser(lexer, *this) { }

//int tx_asm_run_assembler_file(tx_asm_Assembler* as, FILE* input) {
//    tx_asm_yyin  = input;
//    tx_asm_yyasm = as;
//    tx_asm_yyparse();
//    tx_asm_assembler_convert_labels(as);
//
//    return as->error;
//}

int tx_asm_run_assembler_buffer(tx_asm_Assembler* as, char* buf, int size) {
    std::stringstream s;
    s.write(buf, size);
    //    tx_asm_yy_scan_bytes(buf, size);
    as->lexer.switch_streams(&s, nullptr);
    tx_asm_yyasm = as;
    //    tx_asm_yyparse();
    as->parser.parse();
    tx_asm_assembler_convert_labels(as);

    return as->error;
}

bool tx_asm_assembler_write_binary_file(tx_asm_Assembler* as, FILE* output) {
    if (as->error != 0) {
        tx::log_err("Assembler encountered an error, did not write binary file.\n");
        return false;
    }
    tx_uint8 buf[tx_INSTRUCTION_MAX_LENGTH];
    for (auto& inst : as->instructions) {
        tx_asm_instruction_generate_binary(&inst, buf);
        fwrite(buf, inst.len, 1, output);
    }

    return true;
}

bool tx_asm_assembler_generate_binary(tx_asm_Assembler* as, tx_uint8* rom_dest) {
    if (as->error != 0) {
        tx::log_err("Assembler encountered an error, did not generate binary.\n");
        return false;
    }

    tx_uint32 pos = 0;

    for (auto& inst : as->instructions) {
        tx_asm_instruction_generate_binary(&inst, rom_dest + pos);
        pos += inst.len;
    }

    return true;
}

void tx_asm_destroy_assembler(tx_asm_Assembler* as) {
    as->labels.clear();
    as->instructions.clear();
}

tx_uint32 tx_asm_assembler_handle_label(tx_asm_Assembler* as, const std::string& name) {
    // search for an existing label with the same name and return its id if found
    for (auto& label : as->labels) {
        if (strcmp(name.c_str(), label.name) == 0) return label.id;
    }

    // create a new label
    tx_Label label;
    label.name     = strdup(name.c_str());
    label.id       = ++(as->last_label_id);
    label.position = tx_asm_INVALID_LABEL_ADDRESS;

    // insert the new label into the list
    as->labels.push_back(label);

    // return the new id
    return label.id;
}

// returns the id of the label whose position was set
tx_uint32 tx_asm_assembler_set_label_position(tx_asm_Assembler* as, const std::string& name) {
    // find label that matches the name
    for (auto& label : as->labels) {
        if (strcmp(name.c_str(), label.name) == 0) {
            // error if the matched label already has a position set
            if (label.position != tx_asm_INVALID_LABEL_ADDRESS)
                tx_asm_error(as, "Cannot create two or more labels with the same name '{}'\n", name.c_str());
            else {
                // set position of found label
                // TODO fix position offset hack
                label.position = tx_ROM_START + as->position;
                return label.id;
            }
        }
    }

    // error if no match was found
    tx_asm_error(as, "No label '{}' to set position to\n", name.c_str());
    return 0;
}

tx_uint32 tx_asm_assembler_convert_label(tx_asm_Assembler* as, tx_uint32 id) {
    for (auto& label : as->labels) {
        if (label.id == id) {
            if (label.position != tx_asm_INVALID_LABEL_ADDRESS) return label.position;

            // error if label does not have a position set
            tx_asm_error(as, "Label '{}' has no corresponding location", label.name);
            return 0;
        }
    }

    tx_asm_error(as, "No label with id {} found", id);
    return 0;
}

void tx_asm_assembler_add_instruction(tx_asm_Assembler* as, tx_Instruction inst) {
    tx_asm_instruction_calculate_length(&inst);
    as->instructions.push_back(inst);
    as->position += inst.len;
}

void tx_asm_assembler_convert_labels(tx_asm_Assembler* as) {
    for (auto& inst : as->instructions) {
        if (inst.params.p1.mode == tx_param_label) {
            inst.params.p1.value.u = tx_asm_assembler_convert_label(as, inst.params.p1.value.u);
            inst.params.p1.mode    = tx_param_constant32;
        }
        if (inst.params.p2.mode == tx_param_label) {
            inst.params.p2.value.u = tx_asm_assembler_convert_label(as, inst.params.p2.value.u);
            inst.params.p2.mode    = tx_param_constant32;
        }
    }
}

void tx_asm_assembler_print_instructions(tx_asm_Assembler* as) {
    tx_uint32 pos = 0;
    for (auto& inst : as->instructions) {
        tx::log_err("[asm] [#{:04x}:{:02x}] ", pos, inst.len);
        pos += inst.len;
        tx_debug_print_instruction(&inst);
    }
}

void tx_asm_assembler_print_labels(tx_asm_Assembler* as) {
    for (auto& label : as->labels) { tx::log_err("[asm] :{} [{:x}] = #{:x}\n", label.name, label.id, label.position); }
}
