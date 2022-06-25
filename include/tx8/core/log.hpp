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

#include <bits/types/FILE.h>
#include <cstdio>
#include <fmt/format.h>
#include <string>

using tx_logfunc_ptr = void (*)(const std::string& s);

namespace tx {
    class Log {
      public:
        /// Reset the logger to its initial state
        void reset();

        Log();
        ~Log();

        /// Issue log message
        template <typename S, typename... Args>
        void operator()(const S& format, Args&&... args) {
            fmt::format_args fargs = fmt::make_format_args(std::forward<Args>(args)...);
            std::string      s     = fmt::vformat(format, fargs);

            if (stream != nullptr) *stream << s;
            if (str != nullptr) str->append(s.c_str());

            if (file != nullptr) fprintf(file, "%s", s.c_str());
            if (func != nullptr) func(s);
        }

        /// Discards the aggregated log string
        inline void clear_str() {
            if (str != nullptr) str->clear();
        }

        /// Normal logs go to stream
        inline void init_stream(std::ostream* strm) { stream = strm; }

        /// Normal logs go to a file
        inline void init_file(FILE* f) { file = f; }

        /// Normal logs go to a string
        inline void init_str() {
            if (str == nullptr) str = new std::string();
            else str->clear();
        }

        /// Normal logs call the given function
        inline void init_func(tx_logfunc_ptr fun) { func = fun; }

        /// Retrieve the saved error log string
        inline std::string get_str() { return *str; }

      private:
        std::ostream*  stream = nullptr;
        FILE*          file   = nullptr;
        std::string*   str    = nullptr;
        tx_logfunc_ptr func   = nullptr;
    };

    extern Log log;
    extern Log log_err;
} // namespace tx
