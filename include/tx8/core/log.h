/**
 * @file log.h
 * @brief Global logging functionality used by the tx8 ecosystem
 * @details The methods declared here allow the programmer to specify how tx8 logs things.
 * It is possible to log enable logging to stdout/stderr, a file, a string, or a custom
 * logging function. See the `tx8_log_init_` methods. Normal and Error logging are separated.
 * Normal output (such as a `print` syscall from a tx8 program) is logged normally. Errors and
 * debug output is logged via the functions with an `_err` suffix. If logging to strings is enabled,
 * the aggregated logs can be retrieved via `tx_log_get_str[_err]` and cleared via
 * `tx_log_clear_str[_err]`.
 */
#pragma once

#include "kstring.h"

#include <bits/types/FILE.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*tx_logfunc_ptr)(const char* format, va_list args);

/// Reset the logger to its initial state (also calls destroy)
void tx_log_reset();
/// Destroys all resources associated with the logger
void tx_log_destroy();

/// Issue a normal log
void tx_log(const char* format, ...);
/// Issue an error log
void tx_log_err(const char* format, ...);

/// Issue a normal log
void tx_logv(const char* format, va_list argptr);
/// Issue an error log
void tx_logv_err(const char* format, va_list argptr);

#ifdef __cplusplus
}
#endif

extern int            __tx_log_stdout;
extern int            __tx_log_stderr;
extern FILE*          __tx_log_file;
extern FILE*          __tx_log_file_err;
extern kstring_t*     __tx_log_string;
extern kstring_t*     __tx_log_string_err;
extern tx_logfunc_ptr __tx_log_func;
extern tx_logfunc_ptr __tx_log_func_err;

/// Discards the aggregated log string
inline void tx_log_clear_str() {
    if (__tx_log_string == NULL) return;
    char* s = ks_release(__tx_log_string);
    if (s != NULL) free(s);
    kputs("", __tx_log_string);
}

/// Discards the aggregated error log string
inline void tx_log_clear_str_err() {
    if (__tx_log_string_err == NULL) return;
    char* s = ks_release(__tx_log_string_err);
    if (s != NULL) free(s);
    kputs("", __tx_log_string_err);
}

/// Normal logs go to stdout
inline void tx_log_init_stdout() { __tx_log_stdout = 1; }

/// Error logs go to stderr
inline void tx_log_init_stderr() { __tx_log_stderr = 1; }

/// Normal logs go to a file
inline void tx_log_init_file(FILE* f) { __tx_log_file = f; }

/// Error logs go to a file
inline void tx_log_init_file_err(FILE* f) { __tx_log_file_err = f; }

/// Normal logs go to a string
inline void tx_log_init_str() {
    if (__tx_log_string == NULL) {
        __tx_log_string    = (kstring_t*) malloc(sizeof(kstring_t));
        __tx_log_string->l = 0;
        __tx_log_string->m = 0;
        __tx_log_string->s = NULL;
    }
    tx_log_clear_str();
}

/// Error logs go to a string
inline void tx_log_init_str_err() {
    if (__tx_log_string_err == NULL) {
        __tx_log_string_err    = (kstring_t*) malloc(sizeof(kstring_t));
        __tx_log_string_err->l = 0;
        __tx_log_string_err->m = 0;
        __tx_log_string_err->s = NULL;
    }
    tx_log_clear_str_err();
}

/// Normal logs call the given function
inline void tx_log_init_func(tx_logfunc_ptr fun) { __tx_log_func = fun; }

/// Error logs call the given function
inline void tx_log_init_func_err(tx_logfunc_ptr fun) { __tx_log_func_err = fun; }

/// Retrieve the saved error log string
inline char* tx_log_get_str() {
    if (__tx_log_string == NULL) return NULL;
    return __tx_log_string->s;
}

/// Retrieve the saved error log string
inline char* tx_log_get_str_err() {
    if (__tx_log_string_err == NULL) return NULL;
    return __tx_log_string_err->s;
}
