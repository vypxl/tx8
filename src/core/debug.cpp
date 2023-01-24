#include "tx8/core/debug.hpp"

#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"

void tx::debug::print_raw_instruction(const Instruction& inst) {
    tx::log_err(
        "{:x} ({}) {:x} {:x} (modes: {:x} {:x})\n",
        (uint32) inst.opcode,
        op_names[(size_t) inst.opcode],
        inst.params.p1.value.u,
        inst.params.p2.value.u,
        (uint32) inst.params.p1.mode,
        (uint32) inst.params.p2.mode
    );
}

void tx::debug::print_instruction(const Instruction& inst) {
    tx::log_err("{}", op_names[(size_t) inst.opcode]);
    if (param_count[(size_t) inst.opcode] > 0) {
        tx::log_err(" ");
        tx::debug::print_parameter(inst.params.p1);
    }
    if (param_count[(size_t) inst.opcode] > 1) {
        tx::log_err(" ");
        tx::debug::print_parameter(inst.params.p2);
    }
    tx::log_err("\n");
}

void tx::debug::print_parameter(const Parameter& p) {
    num32 v = p.value;
    switch (p.mode) {
        case ParamMode::Constant8: tx::log_err("<{:#x} | {}>", (uint8) v.u, (int8) v.i); break;
        case ParamMode::Constant16: tx::log_err("<{:#x} | {}>", (uint16) v.u, (int16) v.i); break;
        case ParamMode::Constant32: tx::log_err("<{:#x} | {} | {:.5f}>", v.u, v.i, v.f); break;
        case ParamMode::AbsoluteAddress: tx::log_err("#{:x}", v.u); break;
        case ParamMode::RelativeAddress:
            if (v.i < 0) tx::log_err("$-{:x}", -v.i);
            else tx::log_err("${:x}", v.i);
            break;
        case ParamMode::RegisterAddress: tx::log_err("@{}", reg_names[v.u]); break;
        case ParamMode::Register: tx::log_err("{}", reg_names[v.u]); break;
        case ParamMode::Label: tx::log_err("label(id: {:#x})", v.u); break;
        case ParamMode::Unused: break;
        default: tx::log_err("{{ unknown parameter mode {:#x}; value: {:#x} }}", (uint32) p.mode, v.u); break;
    }
}
