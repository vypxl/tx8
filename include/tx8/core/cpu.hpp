/**
 * @file cpu.h
 * @brief CPU and its methods.
 * @details Includes some constants for CPU memory initialization, all raw op functions and the
 * table of opcode function pointers.
 */
#pragma once

#include "tx8/core/debug.hpp"
#include "tx8/core/instruction.hpp"
#include "tx8/core/types.hpp"

#include <functional>
#include <map>
#include <string>

namespace tx {
    /// The size of the tx8 memory in bytes
    const uint32 MEM_SIZE = 0xffffffU;
    /// The default position of the stack in tx8 memory
    const uint32 STACK_BEGIN = 0xc02000U;
    /// The default starting point for code in tx8 memory
    const uint32 ENTRY_POINT = 0x400000U;
    /// The position in tx8 memory where the rom is inserted
    const uint32 ROM_START = 0x400000U;
    /// The maximum size of a tx8 rom
    const uint32 ROM_SIZE = 0x800000U;
    /// The number of tx8 cpu registers
    const uint32 REGISTER_COUNT = 8;
    /// The maximum value produced by cpu::rand
    const uint32 RANDOM_MAX = 0x7fff;
    /// The initial random seed
    const uint32 RAND_INITIAL_SEED = 0x12345678;

    class CPU;
    /// A tx8 cpu system function
    using Sysfunc = std::function<void(CPU& cpu)>;

    /// @brief Struct representing a tx8 CPU with memory, registers, system function table and a random seed.
    class CPU {
      public:
        /// Pointer to the cpu memory
        std::vector<uint8> mem;
        /// Union for easy access to the cpu registers though simple identifiers and array indexing
        union {
            struct {
                uint32 a, b, c, d, r, o, p, s;
            };
            std::array<uint32, REGISTER_COUNT> registers;
        };

        /// If debug information should be printed
        bool debug;

      private:
        /// System function table
        std::map<uint32, Sysfunc> sys_func_table;
        /// Random seed
        uint32 rseed;
        /// If the cpu is currently halted (finished execution)
        bool halted;
        /// If the cpu is currently idle and waiting for an interrupt
        bool stopped;

      public:
        /// Initialize all cpu members and copy the rom into the memory
        explicit CPU(Rom rom);
        /// Execute instructions until an error occurs or a hlt instruction is reached
        void run();
        /// Register the given function in the system function table
        void register_sysfunc(const std::string& name, Sysfunc func);

        /// Write a value to the specified memory location
        void mem_write(mem_addr location, uint32 value, ValueSize size = ValueSize::Word);
        /// Read a value from the specified memory location
        uint32 mem_read(mem_addr location, ValueSize size = ValueSize::Word);
        /// Write a value from the specified memory location relative to the O register
        void mem_write_rel(mem_addr location, uint32 value, ValueSize size = ValueSize::Word);
        /// Read a value from the specified memory location relative to the O register
        uint32 mem_read_rel(mem_addr location, ValueSize size = ValueSize::Word);
        /// Overwrite the value of the specified cpu register (respects small registers)
        void reg_write(Register which, uint32 value);
        /// Read the value of the specified cpu register (respects small registers)
        uint32 reg_read(Register which);
        /// Same as reg_read, but sign extends values smaller than 32 bits instead of zero extending
        uint32 reg_read_sign_extended(Register which);

        /// Update the program counter to the specified new location
        void jump(mem_addr location);
        /// Push an value onto the stack
        void push(uint32 value, ValueSize size = ValueSize::Word);
        /// Pop a value from the stack
        uint32 pop(ValueSize size = ValueSize::Word);
        /// Get the topmost value from the stack
        uint32 top(ValueSize size = ValueSize::Word);

        // Convenience function to set the R register
        inline void write_r(uint32 value) { reg_write(Register::R, value); }
        /// Convenience function to set a bit in the r register to a value
        inline void set_r_bit(uint8 bit, uint8 value) { r = (r & ~(1u << bit)) | (value << bit); }
        // Convenience function to read the R register
        inline uint32 read_r() { return reg_read(Register::R); }

      private:
        /// Get a random value using the random seed (range 0 - RANDOM_MAX)
        uint32 rand();

        /// Parse an instruction from the given memory address
        Instruction parse_instruction(mem_addr pc);
        /// Execute the given parsed instruction
        void exec_instruction(Instruction instruction);

        /// Execute the system function specified by its id (the hash of the string name)
        void exec_sysfunc(uint32 hashed_name);

        /// Get the raw numerical value of a parameter using its mode
        uint32 get_param_value(Parameter param);
        /// Same as `get_param_value`, but sign extends values smaller than a word instead of zero extending
        uint32 get_param_value_sign_extended(Parameter param);
        /// Get the absolute address specified by a parameter using its mode (fails if the parameter does not represent an address)
        mem_addr get_param_address(Parameter param);

        /// Get a pointer to the specified location in tx8 cpu memory
        [[deprecated("Don't use mem_get_ptr")]] uint8* mem_get_ptr(mem_addr location);
        /// Check if a location is within valid memory range
        static inline bool mem_in_range(mem_addr location) { return location < MEM_SIZE; }

        /// Print an error message and halt the cpu (sets `halted` to true)
        template <typename... Args>
        void error_raw(const std::string& format, Args... args) {
            log_err(format, args...);
            halted = true;
        }
        /// Same as `error_raw`, but prints the instruction the cpu is currently executing
        /// Beware that this function calls `parse_instruction`, so don't call this when encountering instruction parsing errors
        template <typename... Args>
        void error(const std::string& format, Args... args) {
            error_raw(format, args...);

            Instruction current_instruction = parse_instruction(p);
            log_err("\nCaused by instruction:\n");
            log_err("[#{:x}] ", p);
            debug::print_instruction(current_instruction);
        }

        // All opcode handler functions

        void op_hlt(const Parameters& params);
        void op_nop(const Parameters& params);
        void op_jmp(const Parameters& params);
        void op_jeq(const Parameters& params);
        void op_jne(const Parameters& params);
        void op_jgt(const Parameters& params);
        void op_jge(const Parameters& params);
        void op_jlt(const Parameters& params);
        void op_jle(const Parameters& params);
        void op_cmp(const Parameters& params);
        void op_fcmp(const Parameters& params);
        void op_ucmp(const Parameters& params);
        void op_call(const Parameters& params);
        void op_ret(const Parameters& params);
        void op_sys(const Parameters& params);

        void op_ld(const Parameters& params);
        void op_lds(const Parameters& params);
        void op_lw(const Parameters& params);
        void op_lws(const Parameters& params);
        void op_lda(const Parameters& params);
        void op_sta(const Parameters& params);
        void op_ldb(const Parameters& params);
        void op_stb(const Parameters& params);
        void op_ldc(const Parameters& params);
        void op_stc(const Parameters& params);
        void op_ldd(const Parameters& params);
        void op_std(const Parameters& params);
        void op_zero(const Parameters& params);
        void op_push(const Parameters& params);
        void op_pop(const Parameters& params);

        void op_inc(const Parameters& params);
        void op_dec(const Parameters& params);
        void op_add(const Parameters& params);
        void op_sub(const Parameters& params);
        void op_mul(const Parameters& params);
        void op_div(const Parameters& params);
        void op_mod(const Parameters& params);
        void op_max(const Parameters& params);
        void op_min(const Parameters& params);
        void op_abs(const Parameters& params);
        void op_sign(const Parameters& params);

        void op_and(const Parameters& params);
        void op_or(const Parameters& params);
        void op_not(const Parameters& params);
        void op_nand(const Parameters& params);
        void op_xor(const Parameters& params);
        void op_slr(const Parameters& params);
        void op_sar(const Parameters& params);
        void op_sll(const Parameters& params);
        void op_ror(const Parameters& params);
        void op_rol(const Parameters& params);
        void op_set(const Parameters& params);
        void op_clr(const Parameters& params);
        void op_tgl(const Parameters& params);
        void op_test(const Parameters& params);

        void op_finc(const Parameters& params);
        void op_fdec(const Parameters& params);
        void op_fadd(const Parameters& params);
        void op_fsub(const Parameters& params);
        void op_fmul(const Parameters& params);
        void op_fdiv(const Parameters& params);
        void op_fmod(const Parameters& params);
        void op_fmax(const Parameters& params);
        void op_fmin(const Parameters& params);
        void op_fabs(const Parameters& params);
        void op_fsign(const Parameters& params);
        void op_sin(const Parameters& params);
        void op_cos(const Parameters& params);
        void op_tan(const Parameters& params);
        void op_asin(const Parameters& params);
        void op_acos(const Parameters& params);
        void op_atan(const Parameters& params);
        void op_atan2(const Parameters& params);
        void op_sqrt(const Parameters& params);
        void op_pow(const Parameters& params);
        void op_exp(const Parameters& params);
        void op_log(const Parameters& params);
        void op_log2(const Parameters& params);
        void op_log10(const Parameters& params);

        void op_uadd(const Parameters& params);
        void op_usub(const Parameters& params);
        void op_umul(const Parameters& params);
        void op_udiv(const Parameters& params);
        void op_umod(const Parameters& params);
        void op_umax(const Parameters& params);
        void op_umin(const Parameters& params);

        void op_rand(const Parameters& params);
        void op_rseed(const Parameters& params);
        void op_itf(const Parameters& params);
        void op_fti(const Parameters& params);
        void op_utf(const Parameters& params);
        void op_ftu(const Parameters& params);
        void op_ei(const Parameters& params);
        void op_di(const Parameters& params);
        void op_stop(const Parameters& params);

        /// Opcode handler function for invalid opcodes
        void op_inv(const Parameters& params);

        // clang-format off
        /// Mapping of opcodes to their handler functions
        const std::array<std::function<void (CPU*, const Parameters& params)>, 256> op_function = {
            // 0x0
            &CPU::op_hlt, &CPU::op_nop, &CPU::op_jmp, &CPU::op_jeq, &CPU::op_jne, &CPU::op_jgt, &CPU::op_jge, &CPU::op_jlt, &CPU::op_jle, &CPU::op_cmp, &CPU::op_fcmp, &CPU::op_ucmp, &CPU::op_call, &CPU::op_ret, &CPU::op_sys, &CPU::op_inv,
            // 0x1
            &CPU::op_ld, &CPU::op_lds, &CPU::op_lw, &CPU::op_lws, &CPU::op_lda, &CPU::op_sta, &CPU::op_ldb, &CPU::op_stb, &CPU::op_ldc, &CPU::op_stc, &CPU::op_ldd, &CPU::op_std, &CPU::op_zero, &CPU::op_push, &CPU::op_pop, &CPU::op_inv,
            // 0x2
            &CPU::op_inc, &CPU::op_dec, &CPU::op_add, &CPU::op_sub, &CPU::op_mul, &CPU::op_div, &CPU::op_mod, &CPU::op_max, &CPU::op_min, &CPU::op_abs, &CPU::op_sign, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0x3
            &CPU::op_and, &CPU::op_or, &CPU::op_not, &CPU::op_nand, &CPU::op_xor, &CPU::op_slr, &CPU::op_sar, &CPU::op_sll, &CPU::op_ror, &CPU::op_rol, &CPU::op_set, &CPU::op_clr, &CPU::op_tgl, &CPU::op_test, &CPU::op_inv,  &CPU::op_inv,
            // 0x4
            &CPU::op_finc, &CPU::op_fdec, &CPU::op_fadd, &CPU::op_fsub, &CPU::op_fmul, &CPU::op_fdiv, &CPU::op_fmod, &CPU::op_fmax, &CPU::op_fmin, &CPU::op_fabs, &CPU::op_fsign, &CPU::op_sin, &CPU::op_cos, &CPU::op_tan, &CPU::op_asin, &CPU::op_acos,
            // 0x5
            &CPU::op_atan, &CPU::op_atan2, &CPU::op_sqrt, &CPU::op_pow, &CPU::op_exp, &CPU::op_log, &CPU::op_log2, &CPU::op_log10, &CPU::op_inv,  &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0x6
            &CPU::op_uadd, &CPU::op_usub, &CPU::op_umul, &CPU::op_udiv, &CPU::op_umod, &CPU::op_umax, &CPU::op_umin, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0x7
            &CPU::op_rand, &CPU::op_rseed, &CPU::op_itf, &CPU::op_fti, &CPU::op_utf, &CPU::op_ftu, &CPU::op_ei, &CPU::op_di, &CPU::op_stop, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0x8
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0x9
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xa
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xb
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xc
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xd
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xe
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
            // 0xf
            &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv, &CPU::op_inv,
        };
        // clang-format on
    };
} // namespace tx
