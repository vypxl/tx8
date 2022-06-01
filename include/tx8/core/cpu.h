/**
 * @file cpu.h
 * @brief tx_CPU and its methods.
 * @details Includes some constants for tx_CPU memory initialization, all raw op functions and the
 * table of opcode function pointers.
 */
#pragma once

#include "tx8/core/instruction.h"
#include "tx8/core/types.h"

#include <khash.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// The size of the tx8 memory in bytes
#define tx_MEM_SIZE 0xffffffU
/// The default position of the stack in tx8 memory
#define tx_STACK_BEGIN 0xc02000U
/// The default starting point for code in tx8 memory
#define tx_ENTRY_POINT 0x400000U
/// The position in tx8 memory where the rom is inserted
#define tx_ROM_START 0x400000U
/// The maximum size of a tx8 rom
#define tx_ROM_SIZE 0x800000U
/// The number of tx8 cpu registers
#define tx_REGISTER_COUNT 8
/// The maximum value produced by tx_cpu_rand
#define tx_RAND_MAX ((1U << 31U) - 1)

typedef struct tx_CPU tx_CPU;
/// A function pointer type alias for tx8 system functions callable by `sys` instructions
typedef void (*tx_sysfunc_ptr)(tx_CPU* cpu, void* data);

/// A tx8 cpu system function
typedef struct tx_sysfunc {
    tx_sysfunc_ptr func;
    void* data;
} tx_sysfunc;

/// \private
KHASH_INIT(tx_sysfunc, tx_uint32, tx_sysfunc, 1, kh_int_hash_func, kh_int_hash_equal);

/// @brief Struct representing a tx8 CPU with memory, registers, system function table and a random seed.
struct tx_CPU {
    /// Pointer to the cpu memory
    tx_mem_ptr mem;
    /// Union for easy access to the cpu registers though simple identifiers and array indexing
    union {
        struct {
            tx_uint32 a, b, c, d, r, o, p, s;
        };
        tx_uint32 registers[tx_REGISTER_COUNT];
    };
    /// System function table
    khash_t(tx_sysfunc) * sys_func_table;
    /// Random seed
    tx_uint32 rseed;
    /// If the cpu is currently halted (stopped execution)
    bool halted;
    /// If debug information should be printed
    bool debug;
};

/// Initialize all cpu members and copy the rom into the memory
void tx_init_cpu(tx_CPU* cpu, tx_mem_ptr rom, tx_uint32 rom_size);
/// Free all resources allocated by the cpu
void tx_destroy_cpu(tx_CPU* cpu);
/// Execute instructions until an error occurs or a hlt instruction is reached
void tx_run_cpu(tx_CPU* cpu);
/// Print an error message and halt the cpu (sets `halted` to true)
void tx_cpu_error(tx_CPU* cpu, char* format, ...);
/// Get a random 32bit value using the random seed
tx_uint32 tx_cpu_rand(tx_CPU* cpu);

/// Parse an instruction from the given memory address
tx_Instruction tx_parse_instruction(tx_CPU* cpu, tx_mem_addr pc);
/// Execute the given parsed instruction
void tx_cpu_exec_instruction(tx_CPU* cpu, tx_Instruction instruction);

/// Register the given function in the system function table
void tx_cpu_register_sysfunc(tx_CPU* cpu, char* name, tx_sysfunc_ptr func, void* data);
/// Execute the system function specified by its id (the hash of the string name)
void tx_cpu_exec_sysfunc(tx_CPU* cpu, tx_uint32 hashed_name);

/// Get the raw numerical value of a parameter using its mode
tx_uint32 tx_cpu_get_param_value(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode);
/// Get the absolute address specified by a parameter using its mode (fails if the parameter does not represent an address)
tx_mem_addr tx_cpu_get_param_address(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode);

/// Update the program counter to the specified new location
void tx_cpu_jump(tx_CPU* cpu, tx_uint32 location);
/// Push an 8bit value onto the stack
void tx_cpu_push8(tx_CPU* cpu, tx_uint8 value);
/// Push a 16bit value onto the stack
void tx_cpu_push16(tx_CPU* cpu, tx_uint16 value);
/// Push a 32bit value onto the stack
void tx_cpu_push32(tx_CPU* cpu, tx_uint32 value);
/// Pop an 8bit value from the stack
tx_uint8 tx_cpu_pop8(tx_CPU* cpu);
/// Pop a 16bit value from the stack
tx_uint16 tx_cpu_pop16(tx_CPU* cpu);
/// Pop a 32bit value from the stack
tx_uint32 tx_cpu_pop32(tx_CPU* cpu);
/// Get the topmost 8bit value from the stack
tx_uint8 tx_cpu_top8(tx_CPU* cpu);
/// Get the topmost 816it value from the stack
tx_uint16 tx_cpu_top16(tx_CPU* cpu);
/// Get the topmost 32bit value from the stack
tx_uint32 tx_cpu_top32(tx_CPU* cpu);
/// Get a pointer to the specified location in tx8 cpu memory
tx_mem_ptr tx_cpu_mem_get_ptr(tx_CPU* cpu, tx_mem_addr location);
/// Get a pointer to the specified location relative to the O register in tx8 cpu memory
tx_mem_ptr tx_cpu_mem_get_ptr_rel(tx_CPU* cpu, tx_mem_addr location);
/// Write an 8bit value to the specified memory location
void tx_cpu_mem_write8(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value);
/// Write a 16bit value to the specified memory location
void tx_cpu_mem_write16(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value);
/// Write a 32bit value to the specified memory location
void tx_cpu_mem_write32(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value);
/// Read an 8bit value from the specified memory location
tx_uint8 tx_cpu_mem_read8(tx_CPU* cpu, tx_mem_addr location);
/// Read a 16bit value from the specified memory location
tx_uint16 tx_cpu_mem_read16(tx_CPU* cpu, tx_mem_addr location);
/// Read a 32bit value from the specified memory location
tx_uint32 tx_cpu_mem_read32(tx_CPU* cpu, tx_mem_addr location);
/// Write an 8bit value from the specified memory location relative to the O register
void tx_cpu_mem_write8_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value);
/// Write a 16bit value from the specified memory location relative to the O register
void tx_cpu_mem_write16_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value);
/// Write a 32bit value from the specified memory location relative to the O register
void tx_cpu_mem_write32_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value);
/// Read an 8bit value from the specified memory location relative to the O register
tx_uint8 tx_cpu_mem_read8_rel(tx_CPU* cpu, tx_mem_addr location);
/// Read a 16bit value from the specified memory location relative to the O register
tx_uint16 tx_cpu_mem_read16_rel(tx_CPU* cpu, tx_mem_addr location);
/// Read a 32bit value from the specified memory location relative to the O register
tx_uint32 tx_cpu_mem_read32_rel(tx_CPU* cpu, tx_mem_addr location);
/// Overwrite the value of the specified cpu register (respects small registers)
void tx_cpu_reg_write(tx_CPU* cpu, tx_Register which, tx_uint32 value);
/// Read the value of the specified cpu register (respects small registers)
tx_uint32 tx_cpu_reg_read(tx_CPU* cpu, tx_Register which);

/// Write 1, 2 or 4 bytes of `value` to the specified memory location
void tx_cpu_mem_write_n(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value, tx_uint8 bytes);

// Convenience function to set the R register
static inline void tx_cpu_write_r(tx_CPU* cpu, tx_uint32 value) {
    tx_cpu_reg_write(cpu, tx_reg_r, value);
}

// Convenience function to read the R register
static inline tx_uint32 tx_cpu_read_r(tx_CPU* cpu) {
    return tx_cpu_reg_read(cpu, tx_reg_r);
}

// All opcode handler functions

void tx_cpu_op_hlt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_nop(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jmp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jeq(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jne(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jgt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jge(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jlt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jle(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_cmp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fcmp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ucmp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_call(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ret(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sys(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_ld(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_lw(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_lda(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sta(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldb(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_stb(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_stc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_std(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_zero(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_push(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_pop(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_inc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_dec(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_add(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sub(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mul(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_div(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mod(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_max(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_min(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_abs(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sign(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_and(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_or(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_not(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_nand(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_xor(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_slr(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sar(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sll(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ror(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_rol(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_set(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_clr(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_tgl(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_test(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_finc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fdec(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fadd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fsub(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmul(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fdiv(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmod(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmax(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmin(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fabs(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fsign(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sin(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_cos(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_tan(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_asin(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_acos(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_atan(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_atan2(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sqrt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_pow(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_exp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_log(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_log2(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_log10(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_umul(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_udiv(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umod(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umax(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umin(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_op_rand(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_rseed(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_itf(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fti(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_utf(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ftu(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ei(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_di(tx_CPU* cpu, tx_Parameters* params);


/// Opcode handler function for invalid opcodes
void tx_cpu_op_inv(tx_CPU* cpu, tx_Parameters* params);

/// A function pointer type alias for opcode handler functions
typedef void (*tx_OpFunction)(tx_CPU* cpu, tx_Parameters* params);
// clang-format off
/// Mapping of opcodes to their handler functions
static const tx_OpFunction tx_cpu_op_function[256] = {
    // 0x0
    &tx_cpu_op_hlt, &tx_cpu_op_nop, &tx_cpu_op_jmp, &tx_cpu_op_jeq, &tx_cpu_op_jne, &tx_cpu_op_jgt, &tx_cpu_op_jge, &tx_cpu_op_jlt, &tx_cpu_op_jle, &tx_cpu_op_cmp, &tx_cpu_op_fcmp, &tx_cpu_op_ucmp, &tx_cpu_op_call, &tx_cpu_op_ret, &tx_cpu_op_sys, &tx_cpu_op_inv,
    // 0x1
    &tx_cpu_op_ld, &tx_cpu_op_lw, &tx_cpu_op_lda, &tx_cpu_op_sta, &tx_cpu_op_ldb, &tx_cpu_op_stb, &tx_cpu_op_ldc, &tx_cpu_op_stc, &tx_cpu_op_ldd, &tx_cpu_op_std, &tx_cpu_op_zero, &tx_cpu_op_push, &tx_cpu_op_pop, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x2
    &tx_cpu_op_inc, &tx_cpu_op_dec, &tx_cpu_op_add, &tx_cpu_op_sub, &tx_cpu_op_mul, &tx_cpu_op_div, &tx_cpu_op_mod, &tx_cpu_op_max, &tx_cpu_op_min, &tx_cpu_op_abs, &tx_cpu_op_sign, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x3
    &tx_cpu_op_and, &tx_cpu_op_or, &tx_cpu_op_not, &tx_cpu_op_nand, &tx_cpu_op_xor, &tx_cpu_op_slr, &tx_cpu_op_sar, &tx_cpu_op_sll, &tx_cpu_op_ror, &tx_cpu_op_rol, &tx_cpu_op_set, &tx_cpu_op_clr, &tx_cpu_op_tgl, &tx_cpu_op_test, &tx_cpu_op_inv,  &tx_cpu_op_inv,
    // 0x4
    &tx_cpu_op_finc, &tx_cpu_op_fdec, &tx_cpu_op_fadd, &tx_cpu_op_fsub, &tx_cpu_op_fmul, &tx_cpu_op_fdiv, &tx_cpu_op_fmod, &tx_cpu_op_fmax, &tx_cpu_op_fmin, &tx_cpu_op_fabs, &tx_cpu_op_fsign, &tx_cpu_op_sin, &tx_cpu_op_cos, &tx_cpu_op_tan, &tx_cpu_op_asin, &tx_cpu_op_acos,
    // 0x5
    &tx_cpu_op_atan, &tx_cpu_op_atan2, &tx_cpu_op_sqrt, &tx_cpu_op_pow, &tx_cpu_op_exp, &tx_cpu_op_log, &tx_cpu_op_log2, &tx_cpu_op_log2, &tx_cpu_op_log10,  &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x6
    &tx_cpu_op_umul, &tx_cpu_op_udiv, &tx_cpu_op_umod, &tx_cpu_op_umax, &tx_cpu_op_umin, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x7
    &tx_cpu_op_rand, &tx_cpu_op_rseed, &tx_cpu_op_itf, &tx_cpu_op_fti, &tx_cpu_op_utf, &tx_cpu_op_ftu, &tx_cpu_op_ei, &tx_cpu_op_di, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x8
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0x9
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xa
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xb
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xc
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xd
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xe
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
    // 0xf
    &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv, &tx_cpu_op_inv,
};
// clang-format on

#ifdef __cplusplus
}
#endif
