#include "tx8/core/log.hpp"

#include <cstdarg>

int            __tx_log_stdout     = 0;
int            __tx_log_stderr     = 0;
FILE*          __tx_log_file       = nullptr;
FILE*          __tx_log_file_err   = nullptr;
std::string*   __tx_log_string     = nullptr;
std::string*   __tx_log_string_err = nullptr;
tx_logfunc_ptr __tx_log_func       = nullptr;
tx_logfunc_ptr __tx_log_func_err   = nullptr;

void tx_log_reset() {
    tx_log_destroy();
    __tx_log_stdout = 0;
    __tx_log_stderr = 0;

    __tx_log_file = nullptr;
    delete __tx_log_string;
    __tx_log_string = nullptr;
    __tx_log_func   = nullptr;

    __tx_log_file_err = nullptr;
    delete __tx_log_string_err;
    __tx_log_string_err = nullptr;
    __tx_log_func_err   = nullptr;
}

void tx_log_destroy() {
    tx_log_clear_str();
    tx_log_clear_str_err();
}

void tx_log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    tx_logv(format, args);
    va_end(args);
}

void tx_log_err(const char* format, ...) {
    va_list args;
    va_start(args, format);
    tx_logv_err(format, args);
    va_end(args);
}

void tx_logv(const char* format, va_list args) {
    va_list argptr;
    if (__tx_log_stdout) {
        va_copy(argptr, args);
        vfprintf(stdout, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_string != nullptr) {
        va_copy(argptr, args);
        std::string s;
        s.resize(vsnprintf(nullptr, 0, format, argptr) + 1);
        va_end(argptr);
        va_copy(argptr, args);
        vsnprintf(&s[0], s.size(), format, argptr);
        __tx_log_string->append(s.c_str());
        va_end(argptr);
    }
    if (__tx_log_file != nullptr) {
        va_copy(argptr, args);
        vfprintf(__tx_log_file, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_func != nullptr) {
        va_copy(argptr, args);
        __tx_log_func(format, argptr);
        va_end(argptr);
    }
}

void tx_logv_err(const char* format, va_list args) {
    va_list argptr;
    if (__tx_log_stderr) {
        va_copy(argptr, args);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_string_err != nullptr) {
        va_copy(argptr, args);
        std::string s;
        s.resize(vsnprintf(nullptr, 0, format, argptr) + 1);
        va_end(argptr);
        va_copy(argptr, args);
        vsnprintf(&s[0], s.size(), format, argptr);
        __tx_log_string_err->append(s.c_str());
        va_end(argptr);
    }
    if (__tx_log_file_err != nullptr) {
        va_copy(argptr, args);
        vfprintf(__tx_log_file_err, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_func_err != nullptr) {
        va_copy(argptr, args);
        __tx_log_func_err(format, argptr);
        va_end(argptr);
    }
}
