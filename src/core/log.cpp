#include "tx8/core/log.hpp"

using namespace tx;

void Log::reset() {
    stream = nullptr;
    file   = nullptr;
    delete str;
    str  = nullptr;
    func = nullptr;
}

Log::Log() { reset(); }

Log::~Log() {
    delete str;
    str = nullptr;
}

tx::Log tx::log;
tx::Log tx::log_err;
