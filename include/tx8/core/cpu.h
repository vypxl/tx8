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

#define tx_MEM_SIZE       0xffffffU
#define tx_STACK_BEGIN    0xc02000U
#define tx_ENTRY_POINT    0x400000U
#define tx_ROM_START      0x400000U
#define tx_ROM_SIZE       0x800000U
#define tx_REGISTER_COUNT 7
#define tx_RAND_MAX_32    ((1U << 31U) - 1)
#define tx_RAND_MAX       ((1U << 15U) - 1)

typedef struct tx_CPU tx_CPU;
typedef void (*tx_sysfunc_ptr)(tx_CPU* cpu);
KHASH_INIT(tx_sysfunc, tx_uint32, tx_sysfunc_ptr, 1, kh_int_hash_func, kh_int_hash_equal) // NOLINT

struct tx_CPU {
    tx_mem_ptr mem;
    union {
        struct {
            tx_uint32 a, b, c, d, o, p, s;
        };
        tx_uint32 registers[tx_REGISTER_COUNT];
    };
    khash_t(tx_sysfunc)* sys_func_table;
    tx_uint32 rseed;
    bool      halted;
};

void      tx_init_cpu(tx_CPU* cpu, tx_mem_ptr rom, tx_uint32 rom_size);
void      tx_destroy_cpu(tx_CPU* cpu);
void      tx_run_cpu(tx_CPU* cpu);
void      tx_cpu_error(tx_CPU* cpu, char* format, ...);
tx_uint32 tx_cpu_rand(tx_CPU* cpu);

tx_Instruction tx_parse_instruction(tx_CPU* cpu, tx_mem_addr pc);
void           tx_cpu_exec_instruction(tx_CPU* cpu, tx_Instruction instruction);

void tx_cpu_register_sysfunc(tx_CPU* cpu, char* name, tx_sysfunc_ptr func);
void tx_cpu_exec_sysfunc(tx_CPU* cpu, tx_uint32 hashed_name);

tx_uint32   tx_cpu_get_param_value(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode);
tx_mem_addr tx_cpu_get_param_address(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode);

void      tx_cpu_jump(tx_CPU* cpu, tx_uint32 location);
void      tx_cpu_push8(tx_CPU* cpu, tx_uint8 value);
void      tx_cpu_push16(tx_CPU* cpu, tx_uint16 value);
void      tx_cpu_push32(tx_CPU* cpu, tx_uint32 value);
tx_uint8  tx_cpu_pop8(tx_CPU* cpu);
tx_uint16 tx_cpu_pop16(tx_CPU* cpu);
tx_uint32 tx_cpu_pop32(tx_CPU* cpu);
tx_uint8* tx_cpu_mem_get_ptr(tx_CPU* cpu, tx_mem_addr location);
tx_uint8* tx_cpu_mem_get_ptr_rel(tx_CPU* cpu, tx_mem_addr location);
void      tx_cpu_mem_write8(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value);
void      tx_cpu_mem_write16(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value);
void      tx_cpu_mem_write32(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value);
tx_uint8  tx_cpu_mem_read8(tx_CPU* cpu, tx_mem_addr location);
tx_uint16 tx_cpu_mem_read16(tx_CPU* cpu, tx_mem_addr location);
tx_uint32 tx_cpu_mem_read32(tx_CPU* cpu, tx_mem_addr location);
void      tx_cpu_mem_write8_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value);
void      tx_cpu_mem_write16_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value);
void      tx_cpu_mem_write32_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value);
tx_uint8  tx_cpu_mem_read8_rel(tx_CPU* cpu, tx_mem_addr location);
tx_uint16 tx_cpu_mem_read16_rel(tx_CPU* cpu, tx_mem_addr location);
tx_uint32 tx_cpu_mem_read32_rel(tx_CPU* cpu, tx_mem_addr location);
void      tx_cpu_reg_write(tx_CPU* cpu, tx_Register which, tx_uint32 value);
tx_uint32 tx_cpu_reg_read(tx_CPU* cpu, tx_Register which);

// write 1, 2 or 4 bytes read from value to the specified memory location
void tx_cpu_mem_write_n(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value, tx_uint8 bytes);

void tx_cpu_op_nop(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jmp(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jeq(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jne(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jgt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jge(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jlt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_jle(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_cal(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ret(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sys(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_hlt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mov(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mxv(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_lda(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sta(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldb(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_stb(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_stc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ldd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_std(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_zer(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_psh(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_pop(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_inc(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_dec(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_add(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sub(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mul(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_div(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_mod(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_and(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ora(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_not(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_nnd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_xor(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_shr(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_shl(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ror(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_rol(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fin(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fde(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fad(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fsu(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmu(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fdi(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmo(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_itf(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fti(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_max(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_min(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmx(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fmn(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sin(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_cos(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_tan(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_atn(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_sqt(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_abs(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_fab(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_rnd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_rsd(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umu(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_udi(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umx(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_umn(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_utf(tx_CPU* cpu, tx_Parameters* params);
void tx_cpu_op_ftu(tx_CPU* cpu, tx_Parameters* params);

void tx_cpu_inv_op(tx_CPU* cpu, tx_Parameters* params);

typedef void (*tx_OpFunction)(tx_CPU* cpu, tx_Parameters* params);
// clang-format off
static const tx_OpFunction tx_cpu_op_function[256] = {
    // 0x0
    &tx_cpu_op_nop, &tx_cpu_op_jmp, &tx_cpu_op_jeq, &tx_cpu_op_jne, &tx_cpu_op_jgt, &tx_cpu_op_jge, &tx_cpu_op_jlt, &tx_cpu_op_jle, &tx_cpu_op_cal, &tx_cpu_op_ret, &tx_cpu_op_sys, &tx_cpu_op_hlt, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op,
    // 0x1
    &tx_cpu_op_mov, &tx_cpu_op_mxv, &tx_cpu_op_lda, &tx_cpu_op_sta, &tx_cpu_op_ldb, &tx_cpu_op_stb, &tx_cpu_op_ldc, &tx_cpu_op_stc, &tx_cpu_op_ldd, &tx_cpu_op_std, &tx_cpu_op_zer, &tx_cpu_op_psh, &tx_cpu_op_pop, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0x2
    &tx_cpu_op_inc, &tx_cpu_op_dec, &tx_cpu_op_add, &tx_cpu_op_sub, &tx_cpu_op_mul, &tx_cpu_op_div, &tx_cpu_op_mod, &tx_cpu_op_and, &tx_cpu_op_ora, &tx_cpu_op_not, &tx_cpu_op_nnd, &tx_cpu_op_xor, &tx_cpu_op_shr, &tx_cpu_op_shl, &tx_cpu_op_ror, &tx_cpu_op_rol, 
    // 0x3
    &tx_cpu_op_fin, &tx_cpu_op_fde, &tx_cpu_op_fad, &tx_cpu_op_fsu, &tx_cpu_op_fmu, &tx_cpu_op_fdi, &tx_cpu_op_fmo, &tx_cpu_op_itf, &tx_cpu_op_fti, &tx_cpu_op_max, &tx_cpu_op_min, &tx_cpu_op_fmx, &tx_cpu_op_fmn, &tx_cpu_op_sin, &tx_cpu_op_cos, &tx_cpu_op_tan, 
    // 0x4
    &tx_cpu_op_atn, &tx_cpu_op_sqt, &tx_cpu_op_abs, &tx_cpu_op_fab, &tx_cpu_op_rnd, &tx_cpu_op_rsd, &tx_cpu_op_umu, &tx_cpu_op_udi, &tx_cpu_op_umx, &tx_cpu_op_umn, &tx_cpu_op_utf, &tx_cpu_op_ftu, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op,
    // 0x5
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0x6
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0x7
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0x8
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0x9
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xa
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xb
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xc
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xd
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xe
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
    // 0xf
    &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, &tx_cpu_inv_op, 
};
// clang-format on
