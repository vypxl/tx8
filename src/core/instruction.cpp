#include "tx8/core/instruction.hpp"

#include "tx8/core/types.hpp"

#include <fmt/format.h>
#include <ostream>

std::ostream& operator<<(std::ostream& os, const tx::Parameter& p) {
    using namespace tx;

    num32 v = p.value;
    switch (p.mode) {
        case ParamMode::Constant8: os << fmt::format("<{:#x} | {}>", (uint8) v.u, (int8) v.i); break;
        case ParamMode::Constant16: os << fmt::format("<{:#x} | {}>", (uint16) v.u, (int16) v.i); break;
        case ParamMode::Constant32: os << fmt::format("<{:#x} | {} | {:.5f}>", v.u, v.i, v.f); break;
        case ParamMode::AbsoluteAddress: os << fmt::format("#{:x}", v.u); break;
        case ParamMode::RelativeAddress:
            if (v.i < 0) os << fmt::format("$-{:x}", -v.i);
            else os << fmt::format("${:x}", v.i);
            break;
        case ParamMode::RegisterAddress: os << fmt::format("@{}", reg_names[v.u]); break;
        case ParamMode::Register: os << fmt::format("{}", reg_names[v.u]); break;
        case ParamMode::Label: os << fmt::format("label(id: {:#x})", v.u); break;
        case ParamMode::Unused: break;
        default: os << fmt::format("{{ unknown parameter mode {:#x}; value: {:#x} }}", (uint32) p.mode, v.u); break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const tx::Instruction& inst) {
    os << tx::op_names[(size_t) inst.opcode];
    if (tx::param_count[(size_t) inst.opcode] > 0) os << " " << inst.params.p1;
    if (tx::param_count[(size_t) inst.opcode] > 1) os << " " << inst.params.p2;
    return os;
}

std::ostream& operator<<(std::ostream& os, const tx::Label& l) {
    os << fmt::format("label(id: {:#x})", l.id);
    return os;
}
