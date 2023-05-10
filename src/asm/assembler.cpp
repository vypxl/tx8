#include "tx8/asm/assembler.hpp"

#include "tx8/core/cpu.hpp"
#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"
#include "tx8/core/types.hpp"
#include "tx8/core/util.hpp"

#include <sstream>
#include <variant>

#define tx_asm_INVALID_LABEL_ADDRESS 0xffffffff

tx::Assembler::Assembler(std::istream& input) : lexer(input), parser(lexer) { }

tx::Assembler::Assembler(const std::string& input) : is(input), lexer(is), parser(lexer) { }

tx::Assembler::Assembler(const char* input) : Assembler(std::string(input)) { }

void tx::Assembler::run() {
    if (!ran) {
        parser.parse();
        ast = parser.get_ast();
        if (parser.has_error()) {
            tx::log_err("Parser encountered an error, did not assemble.\n");
            return;
        }
        for (auto& node : ast) {
            if (std::holds_alternative<tx::ast::Instruction>(node)) {
                auto&           inst = std::get<tx::ast::Instruction>(node);
                tx::Instruction instruction {.opcode = inst.opcode};

                if (std::holds_alternative<tx::ast::Label>(inst.p1)) {
                    auto& label           = std::get<tx::ast::Label>(inst.p1);
                    auto  pos             = handle_label(label.name);
                    instruction.params.p1 = tx::Parameter {.value = {pos}, .mode = tx::ParamMode::Label};
                } else if (std::holds_alternative<tx::ast::String>(inst.p1)) {
                    auto& str             = std::get<tx::ast::String>(inst.p1);
                    auto  pos             = handle_string(str.value);
                    instruction.params.p1 = tx::Parameter {.value = {pos}, .mode = tx::ParamMode::Label};
                } else if (std::holds_alternative<tx::Parameter>(inst.p1)) {
                    instruction.params.p1 = std::get<tx::Parameter>(inst.p1);
                } else {
                    report_error("Unreachable, parameter 1 variant holds weird type");
                }

                if (std::holds_alternative<tx::ast::Label>(inst.p2)) {
                    auto& label           = std::get<tx::ast::Label>(inst.p2);
                    auto  pos             = handle_label(label.name);
                    instruction.params.p2 = tx::Parameter {.value = {pos}, .mode = tx::ParamMode::Label};
                } else if (std::holds_alternative<tx::ast::String>(inst.p2)) {
                    auto& str             = std::get<tx::ast::String>(inst.p2);
                    auto  pos             = handle_string(str.value);
                    instruction.params.p2 = tx::Parameter {.value = {pos}, .mode = tx::ParamMode::Label};
                } else if (std::holds_alternative<tx::Parameter>(inst.p2)) {
                    instruction.params.p2 = std::get<tx::Parameter>(inst.p2);
                } else {
                    report_error("Unreachable, parameter 2 variant holds weird type");
                }

                add_instruction(instruction);
            } else if (std::holds_alternative<tx::ast::Label>(node)) {
                auto& label = std::get<tx::ast::Label>(node);
                handle_label(label.name);
                set_label_position(label.name);
            } else if (std::holds_alternative<tx::ast::String>(node)) {
                auto& str = std::get<tx::ast::String>(node);
                // +1 for null byte
                instructions.emplace_back(std::vector<tx::uint8>(str.value.begin(), str.value.end() + 1));
                position += str.value.size() + 1;
            } else {
                report_error("Unreachable, ast node holds weird type");
            }
        }

        // calculate data section offsets
        for (const auto& entry : data_section) {
            set_label_position(entry.label_name);
            position += entry.data.size();
        }

        convert_labels();
        if (tx::log_debug.is_enabled()) {
            print_instructions();
            print_labels();
        }
    }
    ran = true;
}

bool tx::Assembler::write_binary(std::ostream& output) {
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

std::optional<tx::Rom> tx::Assembler::generate_binary() {
    run();
    if (error) {
        tx::log_err("Assembler encountered an error, did not generate binary.\n");
        return std::nullopt;
    }

    Rom binary;
    binary.reserve(position);

    for (auto& inst : instructions) {
        std::visit(
            overloaded {
                [&](tx::Instruction inst) { write_instruction(inst, binary); },
                [&](std::vector<tx::uint8> raw) { binary.insert(binary.end(), raw.begin(), raw.end()); }},
            inst
        );
    }
    for (const auto& entry : data_section) binary.insert(binary.end(), entry.data.begin(), entry.data.end());

    return binary;
}

tx::uint32 tx::Assembler::handle_label(const std::string& name) {
    // search for an existing label with the same name and return its id if found
    for (auto& label : labels) {
        if (name == label.name) return label.id;
    }

    // create a new label
    Label label;
    label.name     = name;
    label.id       = ++last_label_id;
    label.position = tx_asm_INVALID_LABEL_ADDRESS;

    // insert the new label into the list
    labels.push_back(label);

    // return the new id
    return label.id;
}

tx::uint32 tx::Assembler::handle_string(const std::string& str) {
    std::string label_name = fmt::format("__tx_data_str_{}", last_string_id++);
    tx::uint32  label_id   = handle_label(label_name);
    data_section.push_back(DataSectionEntry {label_name, std::vector<tx::uint8>(str.begin(), str.end() + 1)});

    return label_id;
}

// returns the id of the label whose position was set
tx::uint32 tx::Assembler::set_label_position(const std::string& name) {
    // find label that matches the name
    for (auto& label : labels) {
        if (name == label.name) {
            // error if the matched label already has a position set
            if (label.position != tx_asm_INVALID_LABEL_ADDRESS)
                report_error("Cannot create two or more labels with the same name '{}'\n", name.c_str());
            else {
                // set position of found label
                // TODO fix position offset hack
                label.position = ROM_START + position;
                return label.id;
            }
        }
    }

    // error if no match was found
    report_error("No label '{}' to set position to\n", name.c_str());
    return 0;
}

tx::uint32 tx::Assembler::convert_label(uint32 id) {
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

void tx::Assembler::add_instruction(Instruction inst) {
    calculate_instruction_length(inst);
    instructions.emplace_back(inst);
    position += inst.len;
}

void tx::Assembler::convert_labels() {
    for (auto& instruction : instructions) {
        if (!std::holds_alternative<tx::Instruction>(instruction)) continue;
        auto& inst = std::get<tx::Instruction>(instruction);
        if (inst.params.p1.mode == ParamMode::Label) {
            inst.params.p1.value.u = Assembler::convert_label(inst.params.p1.value.u);
            inst.params.p1.mode    = ParamMode::Constant32;
        }
        if (inst.params.p2.mode == ParamMode::Label) {
            inst.params.p2.value.u = Assembler::convert_label(inst.params.p2.value.u);
            inst.params.p2.mode    = ParamMode::Constant32;
        }
    }
}

void tx::Assembler::print_instructions() {
    tx::log_debug("[asm] Instructions:\n");
    uint32 pos = 0;
    for (auto& inst : instructions) {
        std::visit(
            overloaded {
                [&](const tx::Instruction& i) {
                    tx::log_debug("[asm] [#{:04x}:{:02x}] {}\n", pos, i.len, i);
                    pos += i.len;
                },
                [&](const std::vector<tx::uint8>& data) {
                    tx::log_debug("[asm] [#{:04x}] Data of length {}\n", pos, data.size());
                    pos += data.size();
                }},
            inst
        );
    }
}

void tx::Assembler::print_labels() {
    tx::log_debug("[asm] Labels:\n");
    for (auto& label : labels) tx::log_debug("[asm] :{} [{:x}] = #{:x}\n", label.name, label.id, label.position);
}

void tx::Assembler::write_parameter(Parameter& p, Rom& binary) {
    auto*  v_ptr = (uint8*) (&p.value.u);
    size_t c     = 0;
#define push_next binary.push_back(v_ptr[c++])
    switch (p.mode) {
        case ParamMode::Constant32: push_next; // NOLINT
        case ParamMode::AbsoluteAddress:
        case ParamMode::RelativeAddress: push_next;
        case ParamMode::Constant16: push_next;
        case ParamMode::Constant8:
        case ParamMode::Register:
        case ParamMode::RegisterAddress: push_next;
        default: break;
    }
#undef push_next
}

void tx::Assembler::write_instruction(Instruction& inst, Rom& binary) {
    binary.push_back(static_cast<uint8>(inst.opcode));
    if (inst.params.p1.mode != ParamMode::Unused)
        binary.push_back(static_cast<uint8>((((uint32) inst.params.p1.mode) << 4u) | ((uint32) inst.params.p2.mode)));

    write_parameter(inst.params.p1, binary);
    write_parameter(inst.params.p2, binary);
}

tx::uint32 tx::Assembler::calculate_instruction_length(Instruction& inst) {
    return inst.len = 1 + param_mode_bytes[param_count[(size_t) inst.opcode]]
                      + param_sizes[(size_t) inst.params.p1.mode] + param_sizes[(size_t) inst.params.p2.mode];
}
