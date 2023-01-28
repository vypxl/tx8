#include "tx8/core/debug.hpp"

#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"

#include <sstream>

void tx::debug::print_instruction(const Instruction& inst) {
    std::stringstream s;
    s << inst;
    tx::log_err("{}\n", s.str());
}

void tx::debug::print_parameter(const Parameter& p) {
    std::stringstream s;
    s << p;
    tx::log_err("{}", s.str());
}
