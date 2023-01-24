#include "tx8/core/log.hpp"

void tx::Log::reset() {
    stream = nullptr;
    file   = nullptr;
    delete str;
    str  = nullptr;
    func = nullptr;
}

tx::Log::Log() { reset(); }

tx::Log::~Log() {
    delete str;
    str = nullptr;
}

tx::Log tx::log;
tx::Log tx::log_err;
