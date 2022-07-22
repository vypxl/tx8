#include "tx8/asm/assembler.hpp"

#include "tx8/core/cpu.hpp"
#include "tx8/core/debug.hpp"
#include "tx8/core/log.hpp"

#include <sstream>
#include <tx8/core/instruction.h>
#include <tx8/core/types.h>

#define tx_asm_INVALID_LABEL_ADDRESS 0xffffffff

using namespace tx;

Assembler::Assembler(std::unique_ptr<std::istream> input) : is(std::move(input)), parser(lexer, *this) { }

Assembler::Assembler(const std::string& input) : Assembler(std::make_unique<std::istringstream>(input)) { }

Assembler::Assembler(const char* input) : Assembler(std::string(input)) { }

void Assembler::run() {
    if (!ran) {
        lexer.switch_streams(is.get(), nullptr);
        parser.parse();
        convert_labels();
    }
    ran = true;
}

bool Assembler::write_binary(std::ostream& output) {
    run();
    if (error) {
        tx::log_err("Assembler encountered an error, did not write binary file.\n");
        return false;
    }

    auto binary = generate_binary();

    if (binary.has_value()) {
        output.write((char*) binary.value().data(), static_cast<std::streamsize>(binary.value().size()));
        return true;
    }

    return false;
}

std::optional<std::vector<tx_uint8>> Assembler::generate_binary() {
    run();
    if (error) {
        tx::log_err("Assembler encountered an error, did not generate binary.\n");
        return std::nullopt;
    }

    std::vector<tx_uint8> binary;
    binary.reserve(position);

    for (auto& inst : instructions) write_instruction(inst, binary);

    return binary;
}

tx_uint32 Assembler::handle_label(const std::string& name) {
    // search for an existing label with the same name and return its id if found
    for (auto& label : labels) {
        if (strcmp(name.c_str(), label.name) == 0) return label.id;
    }

    // create a new label
    tx_Label label;
    label.name     = strdup(name.c_str());
    label.id       = ++last_label_id;
    label.position = tx_asm_INVALID_LABEL_ADDRESS;

    // insert the new label into the list
    labels.push_back(label);

    // return the new id
    return label.id;
}

// returns the id of the label whose position was set
tx_uint32 Assembler::set_label_position(const std::string& name) {
    // find label that matches the name
    for (auto& label : labels) {
        if (strcmp(name.c_str(), label.name) == 0) {
            // error if the matched label already has a position set
            if (label.position != tx_asm_INVALID_LABEL_ADDRESS)
                report_error("Cannot create two or more labels with the same name '{}'\n", name.c_str());
            else {
                // set position of found label
                // TODO fix position offset hack
                label.position = tx_ROM_START + position;
                return label.id;
            }
        }
    }

    // error if no match was found
    report_error("No label '{}' to set position to\n", name.c_str());
    return 0;
}

tx_uint32 Assembler::convert_label(tx_uint32 id) {
    for (auto& label : labels) {
        if (label.id == id) {
            if (label.position != tx_asm_INVALID_LABEL_ADDRESS) return label.position;

            // error if label does not have a position set
            report_error("Label '{}' has no corresponding location", label.name);
            return 0;
        }
    }

    report_error("No label with id {} found", id);
    return 0;
}

static inline void truncate_param(tx_Parameter& p) {
    if (p.mode != tx_param_constant32) return;
    if ((p.value.u & 0xffffff00) == 0) p.mode = tx_param_constant8;
    else if ((p.value.u & 0xffff0000) == 0) p.mode = tx_param_constant16;
}

void Assembler::add_instruction(tx_Instruction inst) {
    // Check if numbers could be made smaller
    truncate_param(inst.params.p1);
    truncate_param(inst.params.p2);

    calculate_instruction_length(inst);
    instructions.push_back(inst);
    position += inst.len;
}

void Assembler::convert_labels() {
    for (auto& inst : instructions) {
        if (inst.params.p1.mode == tx_param_label) {
            inst.params.p1.value.u = Assembler::convert_label(inst.params.p1.value.u);
            inst.params.p1.mode    = tx_param_constant32;
        }
        if (inst.params.p2.mode == tx_param_label) {
            inst.params.p2.value.u = Assembler::convert_label(inst.params.p2.value.u);
            inst.params.p2.mode    = tx_param_constant32;
        }
    }
}

void Assembler::print_instructions() {
    tx_uint32 pos = 0;
    for (auto& inst : instructions) {
        tx::log_err("[asm] [#{:04x}:{:02x}] ", pos, inst.len);
        pos += inst.len;
        tx::debug::print_instruction(inst);
    }
}

void Assembler::print_labels() {
    for (auto& label : labels) tx::log_err("[asm] :{} [{:x}] = #{:x}\n", label.name, label.id, label.position);
}
void Assembler::write_parameter(tx_Parameter& p, std::vector<tx_uint8>& binary) {
    auto*  v_ptr = (tx_uint8*) (&p.value.u);
    size_t c     = 0;
#define push_next binary.push_back(v_ptr[c++])
    switch (p.mode) {
        case tx_param_constant32: push_next;
        case tx_param_absolute_address:
        case tx_param_relative_address: push_next;
        case tx_param_constant16: push_next;
        case tx_param_constant8:
        case tx_param_register:
        case tx_param_register_address: push_next;
        default: break;
    }
#undef push_next
}

void Assembler::write_instruction(tx_Instruction& inst, std::vector<tx_uint8>& binary) {
    binary.push_back(static_cast<tx_uint8>(inst.opcode));
    if (inst.params.p1.mode != tx_param_unused)
        binary.push_back(static_cast<tx_uint8>(((inst.params.p1.mode) << 4u) | inst.params.p2.mode));

    write_parameter(inst.params.p1, binary);
    write_parameter(inst.params.p2, binary);
}

tx_uint32 Assembler::calculate_instruction_length(tx_Instruction& inst) {
    return inst.len = 1 + tx_param_mode_bytes[tx_param_count[inst.opcode]] + tx_param_sizes[inst.params.p1.mode]
                      + tx_param_sizes[inst.params.p2.mode];
}
