#include "tx8/core/log.h"

#include <stdarg.h>

int __tx_log_stdout = 0;
int __tx_log_stderr = 0;
FILE* __tx_log_file = NULL;
FILE* __tx_log_file_err = NULL;
kstring_t* __tx_log_string = NULL;
kstring_t* __tx_log_string_err = NULL;
tx_logfunc_ptr __tx_log_func = NULL;
tx_logfunc_ptr __tx_log_func_err = NULL;

void tx_log_reset() {
    tx_log_destroy();
    __tx_log_stdout = 0;
    __tx_log_stderr = 0;

    __tx_log_file   = NULL;
    __tx_log_string = NULL;
    __tx_log_func   = NULL;

    __tx_log_file_err   = NULL;
    __tx_log_string_err = NULL;
    __tx_log_func_err   = NULL;
}

void tx_log_destroy() {
    if (__tx_log_string != NULL) {
        free(__tx_log_string->s);
        free(__tx_log_string);
    }
    if (__tx_log_string_err != NULL){
        free(__tx_log_string_err->s);
        free(__tx_log_string_err);
    }
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
    if (__tx_log_string != NULL) {
        va_copy(argptr, args);
        kvsprintf(__tx_log_string, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_file != NULL) {
        va_copy(argptr, args);
        vfprintf(__tx_log_file, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_func != NULL) {
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
    if (__tx_log_string_err != NULL) {
        va_copy(argptr, args);
        kvsprintf(__tx_log_string_err, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_file_err != NULL) {
        va_copy(argptr, args);
        vfprintf(__tx_log_file_err, format, argptr);
        va_end(argptr);
    }
    if (__tx_log_func_err != NULL) {
        va_copy(argptr, args);
        __tx_log_func_err(format, argptr);
        va_end(argptr);
    }
}
