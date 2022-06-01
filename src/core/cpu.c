#include "tx8/core/cpu.h"

#include "tx8/core/debug.h"
#include "tx8/core/instruction.h"
#include "tx8/core/log.h"
#include "tx8/core/types.h"
#include "tx8/core/util.h"

#include <khash.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma clang diagnostic ignored "-Wunused-parameter"

#define ERR_ROM_TOO_LARGE "Could not initialize tx_CPU: rom is too large"
#define ERR_INVALID_PC "Exception: Invalid program counter"
#define ERR_SYSFUNC_REREGISTER "Could not register sysfunction '%s' as another function is already registered under the same name"
#define ERR_SYSFUNC_REALLOC "Could not register sysfunction '%s'; An error occurred while reallocating the sysfunction hashtable."
#define ERR_SYSFUNC_NOT_FOUND "Exception: Could not find sysfunction '%s'"
#define ERR_INVALID_REG_ID "Exception: Invalid register id 0x%x"
#define ERR_INVALID_REG_SIZE "Exception: Invalid register size 0x%x"
#define ERR_INVALID_MEM_WRITE_COUNT "tx_cpu_mem_write_n: Cannot write %d bytes"
#define ERR_CANNOT_LOAD_WORD "Cannot load a word into a smaller register"
#define ERR_DIV_BY_ZERO "Exception: Division by zero"

void tx_init_cpu(tx_CPU* cpu, tx_mem_ptr rom, tx_uint32 rom_size) {
    if (rom_size > tx_ROM_SIZE) {
        tx_cpu_error(cpu, ERR_ROM_TOO_LARGE);
        return;
    }

    // initialize registers and memory
    cpu->halted = 0;
    cpu->debug  = 0;
    cpu->a      = 0;
    cpu->b      = 0;
    cpu->c      = 0;
    cpu->d      = 0;
    cpu->o      = 0;
    cpu->s      = tx_STACK_BEGIN;
    cpu->p      = tx_ENTRY_POINT;
    cpu->mem    = malloc(tx_MEM_SIZE);

    // initialize sys function table
    cpu->sys_func_table = kh_init(tx_sysfunc);

    // zero out all memory
    for (tx_uint32 i = 0; i < tx_MEM_SIZE; i++)
        cpu->mem[i] = 0;
    // load rom into memory
    memcpy(cpu->mem + tx_ROM_START, rom, rom_size);
}

void tx_destroy_cpu(tx_CPU* cpu) {
    free(cpu->mem);
    kh_destroy_tx_sysfunc(cpu->sys_func_table);
}

void tx_run_cpu(tx_CPU* cpu) {
    tx_Instruction current_instruction;

    while (!cpu->halted) {
        if (cpu->p > tx_MEM_SIZE - tx_INSTRUCTION_MAX_LENGTH - 1 || cpu->p < 0) {
            tx_cpu_error(cpu, ERR_INVALID_PC);
            break;
        }

        current_instruction = tx_parse_instruction(cpu, cpu->p);

        if (cpu->debug && current_instruction.opcode != tx_op_nop)
            tx_debug_print_instruction(cpu, &current_instruction);
        tx_cpu_exec_instruction(cpu, current_instruction);

        // do not increment p if instruction changes p
        // TODO better detection
        if (!(current_instruction.opcode >= tx_op_jmp && current_instruction.opcode <= tx_op_ret))
            cpu->p += current_instruction.len;
    }
}

void tx_cpu_error(tx_CPU* cpu, char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    tx_log_err(format, argptr);
    cpu->halted = true;
    va_end(argptr);
    tx_Instruction current_instruction = tx_parse_instruction(cpu, cpu->p);
    tx_log_err("\nCaused by instruction:\n");
    tx_debug_print_raw_instruction(cpu, &current_instruction);
}

tx_uint32 tx_cpu_rand(tx_CPU* cpu) {
    return (cpu->rseed = (cpu->rseed * 214013 + 2541011) & tx_RAND_MAX) >> 16; // NOLINT
}

tx_Instruction tx_parse_instruction(tx_CPU* cpu, tx_mem_addr pc) {
    if (pc > tx_MEM_SIZE - tx_INSTRUCTION_MAX_LENGTH - 1 || pc < 0) {
        tx_cpu_error(cpu, ERR_INVALID_PC);
        tx_Instruction nop = {0};
        nop.opcode         = tx_op_nop;
        nop.len            = 1;
        return nop;
    }

    tx_mem_ptr p      = cpu->mem + pc;
    tx_uint8   pcount = tx_param_count[p[0]];

    tx_uint8 mode_p1 = pcount > 0 ? p[1] >> 4U : 0;
    tx_uint8 mode_p2 = pcount > 1 ? p[1] & tx_PARAM_MODE_2_MASK : 0;

    tx_mem_addr param_start = pc + 1 + tx_param_mode_bytes[pcount];

    tx_uint32 p1 = tx_cpu_mem_read32(cpu, param_start) & tx_param_masks[mode_p1];
    tx_uint32 p2 =
        tx_cpu_mem_read32(cpu, param_start + tx_param_sizes[mode_p1]) & tx_param_masks[mode_p2];

    // clang-format off
    tx_Instruction inst = {
        .opcode     = (tx_Opcode)p[0],
        .parameters = {
            .mode_p1 = mode_p1,
            .mode_p2 = mode_p2,
            .p1 = p1,
            .p2 = p2,
        },
        .len = 1 + tx_param_mode_bytes[pcount] + tx_param_sizes[mode_p1] + tx_param_sizes[mode_p2]
    };
    // clang-format on

    return inst;
}

void tx_cpu_exec_instruction(tx_CPU* cpu, tx_Instruction instruction) {
    tx_cpu_op_function[instruction.opcode](cpu, &(instruction.parameters));
}

void tx_cpu_register_sysfunc(tx_CPU* cpu, char* name, tx_sysfunc_ptr func, void* data) {
    tx_int32  ret;
    tx_uint32 pos = kh_put(tx_sysfunc, cpu->sys_func_table, tx_str_hash(name), &ret);

    tx_sysfunc f = {
        .func = func,
        .data = data
    };

    if (ret == 0)
        tx_cpu_error(cpu, ERR_SYSFUNC_REREGISTER, name);
    else if (ret == -1)
        tx_cpu_error(cpu, ERR_SYSFUNC_REALLOC, name);
    else
        kh_value(cpu->sys_func_table, pos) = f;
}

void tx_cpu_exec_sys_func(tx_CPU* cpu, tx_uint32 hashed_name) {
    tx_uint32 key = kh_get(tx_sysfunc, cpu->sys_func_table, hashed_name);
    if (key == kh_end(cpu->sys_func_table))
        tx_cpu_error(cpu, ERR_SYSFUNC_NOT_FOUND, hashed_name);
    else {
        tx_sysfunc f = kh_value(cpu->sys_func_table, key);
        f.func(cpu, f.data);
    }
}

// Returns the numerical value of a parameter
tx_uint32 tx_cpu_get_param_value(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode) {
    switch (mode) {
        case tx_param_unused: return 0; break;
        case tx_param_constant8:
        case tx_param_constant16:
        case tx_param_constant32: return param; break;
        case tx_param_absolute_address: return tx_cpu_mem_read32(cpu, param); break;
        case tx_param_relative_address: return tx_cpu_mem_read32_rel(cpu, param); break;
        case tx_param_register: return tx_cpu_reg_read(cpu, (tx_uint8)param); break;
        case tx_param_register_address:
            return tx_cpu_mem_read32(cpu, tx_cpu_reg_read(cpu, (tx_uint8)param));
            break;
        default: return 0; break;
    }
}

tx_mem_addr tx_cpu_get_param_address(tx_CPU* cpu, tx_uint32 param, tx_ParamMode mode) {
    switch (mode) {
        case tx_param_absolute_address: return param; break;
        case tx_param_relative_address: return cpu->o + *((tx_int32*)(&param)); break;
        case tx_param_register_address: return tx_cpu_reg_read(cpu, (tx_uint8)param); break;
        default:
            tx_cpu_error(cpu, "Parameter is not an address");
            return tx_MEM_SIZE;
            break;
    }
}

void tx_cpu_jump(tx_CPU* cpu, tx_mem_addr location) { cpu->p = location; }

void tx_cpu_push8(tx_CPU* cpu, tx_uint8 value) {
    cpu->s--;
    tx_cpu_mem_write8(cpu, cpu->s, value);
}
void tx_cpu_push16(tx_CPU* cpu, tx_uint16 value) {
    cpu->s -= 2;
    tx_cpu_mem_write16(cpu, cpu->s, value);
}
void tx_cpu_push32(tx_CPU* cpu, tx_uint32 value) {
    cpu->s -= 4;
    tx_cpu_mem_write32(cpu, cpu->s, value);
}

tx_uint8 tx_cpu_pop8(tx_CPU* cpu) {
    cpu->s++;
    return tx_cpu_mem_read8(cpu, cpu->s - 1);
}
tx_uint16 tx_cpu_pop16(tx_CPU* cpu) {
    cpu->s += 2;
    return tx_cpu_mem_read16(cpu, cpu->s - 2);
}
tx_uint32 tx_cpu_pop32(tx_CPU* cpu) {
    cpu->s += 4;
    return tx_cpu_mem_read32(cpu, cpu->s - 4);
}

tx_uint8 tx_cpu_top8(tx_CPU* cpu) {
    return tx_cpu_mem_read8(cpu, cpu->s);
}
tx_uint16 tx_cpu_top16(tx_CPU* cpu) {
    return tx_cpu_mem_read16(cpu, cpu->s);
}
tx_uint32 tx_cpu_top32(tx_CPU* cpu) {
    return tx_cpu_mem_read32(cpu, cpu->s);
}

tx_uint8* tx_cpu_mem_get_ptr(tx_CPU* cpu, tx_mem_addr location) {
    return (location < tx_MEM_SIZE) ? cpu->mem + location : NULL;
}
tx_uint8* tx_cpu_mem_get_ptr_rel(tx_CPU* cpu, tx_mem_addr location) {
    return (location < tx_MEM_SIZE) ? cpu->mem + cpu->o + location : NULL;
}

void tx_cpu_mem_write8(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value) {
    *tx_cpu_mem_get_ptr(cpu, location) = value;
}
void tx_cpu_mem_write16(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value) {
    *((tx_uint16*)tx_cpu_mem_get_ptr(cpu, location)) = value;
}
void tx_cpu_mem_write32(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value) {
    *((tx_uint32*)tx_cpu_mem_get_ptr(cpu, location)) = value;
}
tx_uint8 tx_cpu_mem_read8(tx_CPU* cpu, tx_mem_addr location) {
    return *tx_cpu_mem_get_ptr(cpu, location);
}
tx_uint16 tx_cpu_mem_read16(tx_CPU* cpu, tx_mem_addr location) {
    return *((tx_uint16*)tx_cpu_mem_get_ptr(cpu, location));
}
tx_uint32 tx_cpu_mem_read32(tx_CPU* cpu, tx_mem_addr location) {
    return *((tx_uint32*)tx_cpu_mem_get_ptr(cpu, location));
}

void tx_cpu_mem_write8_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint8 value) {
    *tx_cpu_mem_get_ptr_rel(cpu, location) = value;
}
void tx_cpu_mem_write16_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint16 value) {
    *((tx_uint16*)tx_cpu_mem_get_ptr_rel(cpu, location)) = value;
}
void tx_cpu_mem_write32_rel(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value) {
    *((tx_uint32*)tx_cpu_mem_get_ptr_rel(cpu, location)) = value;
}
tx_uint8 tx_cpu_mem_read8_rel(tx_CPU* cpu, tx_mem_addr location) {
    return *tx_cpu_mem_get_ptr_rel(cpu, location);
}
tx_uint16 tx_cpu_mem_read16_rel(tx_CPU* cpu, tx_mem_addr location) {
    return *((tx_uint16*)tx_cpu_mem_get_ptr_rel(cpu, location));
}
tx_uint32 tx_cpu_mem_read32_rel(tx_CPU* cpu, tx_mem_addr location) {
    return *((tx_uint32*)tx_cpu_mem_get_ptr_rel(cpu, location));
}

void tx_cpu_reg_write(tx_CPU* cpu, tx_Register which, tx_uint32 value) {
    if ((which & tx_REG_ID_MASK) > tx_REGISTER_COUNT) tx_cpu_error(cpu, ERR_INVALID_REG_ID, which & tx_REG_ID_MASK);
    else switch (which & tx_REG_SIZE_MASK) {
        case tx_REG_SIZE_1:
            *((tx_uint8*)(cpu->registers + (which & tx_REG_ID_MASK))) = (tx_uint8)value;
            break;
        case tx_REG_SIZE_2:
            *((tx_uint16*)(cpu->registers + (which & tx_REG_ID_MASK))) = (tx_uint16)value;
            break;
        case tx_REG_SIZE_4:
            *((tx_uint32*)(cpu->registers + (which & tx_REG_ID_MASK))) = (tx_uint32)value;
            break;
        default: tx_cpu_error(cpu, ERR_INVALID_REG_SIZE, which & tx_REG_SIZE_MASK); break;
    }
}

tx_uint32 tx_cpu_reg_read(tx_CPU* cpu, tx_Register which) {
    if ((which & tx_REG_ID_MASK) > tx_REGISTER_COUNT) tx_cpu_error(cpu, ERR_INVALID_REG_ID, which & tx_REG_ID_MASK);
    // because tx_REG_SIZE_4 is 0x00, the only valid register sizes are 0x00, 0x10 and 0x20
    else if ((which & tx_REG_SIZE_MASK) > tx_REG_SIZE_2)
        tx_cpu_error(cpu, ERR_INVALID_REG_SIZE, which & tx_REG_SIZE_MASK);
    else
        return cpu->registers[which & tx_REG_ID_MASK]
               & tx_register_mask[(which & tx_REG_SIZE_MASK) >> 4U];
    return 0;
}

void tx_cpu_mem_write_n(tx_CPU* cpu, tx_mem_addr location, tx_uint32 value, tx_uint8 bytes) {
    switch (bytes) {
        case 1: tx_cpu_mem_write8(cpu, location, (tx_uint8)value); break;
        case 2: tx_cpu_mem_write16(cpu, location, (tx_uint16)value); break;
        case 4: tx_cpu_mem_write32(cpu, location, value); break;
        default: tx_cpu_error(cpu, ERR_INVALID_MEM_WRITE_COUNT, bytes); break;
    }
}

// local conveniences macros
#define PARAMV(which) tx_cpu_get_param_value(cpu, params->p##which, params->mode_p##which)
#define PARAMA(which) tx_cpu_get_param_address(cpu, params->p##which, params->mode_p##which)
#define CHECK_WRITABLE(name) \
    if (!tx_param_iswritable(params->mode_p1)) { \
        tx_cpu_error(cpu, "Destination of " #name " is not writable"); \
        return; \
    }

void tx_cpu_op_hlt(tx_CPU* cpu, tx_Parameters* params) { cpu->halted = 1; }

void tx_cpu_op_nop(tx_CPU* cpu, tx_Parameters* params) { }

void tx_cpu_op_jmp(tx_CPU* cpu, tx_Parameters* params) { tx_cpu_jump(cpu, PARAMV(1)); }

#define COMP_JUMP(comparison, name) \
    void tx_cpu_op_##name(tx_CPU* cpu, tx_Parameters* params) { \
        if (tx_cpu_read_r(cpu) comparison 0) tx_cpu_jump(cpu, PARAMV(1)); \
    }

COMP_JUMP(==, jeq)
COMP_JUMP(!=, jne)
COMP_JUMP(>, jgt)
COMP_JUMP(>=, jge)
COMP_JUMP(<, jlt)
COMP_JUMP(<=, jle)

#undef COMP_JUMP

#define COMPARISON(dtype, name) \
    void tx_cpu_op_##name(tx_CPU* cpu, tx_Parameters* params) { \
        tx_num32 a = { .u = PARAMV(1) }; \
        tx_num32 b = { .u = PARAMV(2) }; \
        tx_int32 res = CMP(a.dtype, b.dtype); \
        tx_cpu_write_r(cpu, res); \
    }

COMPARISON(i, cmp)
COMPARISON(f, fcmp)
COMPARISON(u, ucmp)

#undef COMPARISON

void tx_cpu_op_call(tx_CPU* cpu, tx_Parameters* params) {
    tx_cpu_push32(cpu, cpu->p);
    tx_cpu_jump(cpu, PARAMV(1));
}

void tx_cpu_op_ret(tx_CPU* cpu, tx_Parameters* params) { tx_cpu_jump(cpu, tx_cpu_pop32(cpu)); }

void tx_cpu_op_sys(tx_CPU* cpu, tx_Parameters* params) { tx_cpu_exec_sys_func(cpu, PARAMV(1)); }

void tx_cpu_op_ld(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE("ld")

    tx_uint32 val = PARAMV(2);

    // register <- value
    if (tx_param_isregister(params->mode_p1)) tx_cpu_reg_write(cpu, params->p1, val);
    // address <- address
    else if (tx_param_isaddress(params->mode_p2))
        tx_cpu_mem_write8(cpu, PARAMA(1), (tx_uint8)val);
    // address <- register
    else if (tx_param_isregister(params->mode_p2))
        tx_cpu_mem_write_n(cpu, PARAMA(1), val, tx_register_size(params->p2));
    // address <- constant
    else
        tx_cpu_mem_write_n(cpu, PARAMA(1), val, tx_param_value_size(params->p2, params->mode_p2));
}

void tx_cpu_op_lw(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE("lw")

    tx_uint32 val = PARAMV(2);

    if (tx_param_isregister(params->mode_p1)) {
        if (tx_register_size(params->p1) != 4) tx_cpu_error(cpu, ERR_CANNOT_LOAD_WORD);
        else
            tx_cpu_reg_write(cpu, params->p1, val);
    } else {
        tx_cpu_mem_write32(cpu, PARAMA(1), val);
    }
}

// Macro for defining ld and st ops for registers a thru d
#define DEFINE_LDX_STX(which) \
    void tx_cpu_op_ld##which(tx_CPU* cpu, tx_Parameters* params) { \
        tx_cpu_reg_write(cpu, tx_reg_##which, PARAMV(1)); \
    } \
    void tx_cpu_op_st##which(tx_CPU* cpu, tx_Parameters* params) { \
        CHECK_WRITABLE("st" #which) \
        tx_cpu_mem_write32(cpu, PARAMA(1), tx_cpu_reg_read(cpu, tx_reg_##which)); \
    }
DEFINE_LDX_STX(a)
DEFINE_LDX_STX(b)
DEFINE_LDX_STX(c)
DEFINE_LDX_STX(d)
#undef DEFINE_LDX_STX

void tx_cpu_op_zero(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE("zero")

    if (tx_param_isregister(params->mode_p1)) tx_cpu_reg_write(cpu, params->p1, 0);
    else
        tx_cpu_mem_write32(cpu, PARAMA(1), 0);
}

void tx_cpu_op_push(tx_CPU* cpu, tx_Parameters* params) {
    tx_uint32 val = PARAMV(1);
    switch (tx_param_value_size(params->p1, params->mode_p1)) {
        case 1: tx_cpu_push8(cpu, (tx_uint8)val); break;
        case 2: tx_cpu_push16(cpu, (tx_uint16)val); break;
        case 4: tx_cpu_push32(cpu, val); break;
        default: break;
    }
}

void tx_cpu_op_pop(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE("pop")

    if (tx_param_isregister(params->mode_p1)) {
        switch (tx_register_size(params->p1)) {
            case 0: tx_cpu_reg_write(cpu, params->p1, tx_cpu_pop32(cpu)); break;
            case 1: tx_cpu_reg_write(cpu, params->p1, tx_cpu_pop16(cpu)); break;
            case 2: tx_cpu_reg_write(cpu, params->p1, tx_cpu_pop8(cpu)); break;
            default: break;
        }
    } else
        tx_cpu_mem_write32(cpu, params->p1, tx_cpu_pop32(cpu));
}

// Macros for defining arithmetic operations
#define AR_OP_1_BEGIN(name) \
    CHECK_WRITABLE(name) \
    tx_num32 a = { .u = PARAMV(1) }; \
    tx_num32 result;
#define AR_OP_2_BEGIN(name) \
    CHECK_WRITABLE(name) \
    tx_num32 a = { .u = PARAMV(1) }; \
    tx_num32 b = { .u = PARAMV(2) }; \
    tx_num32 result;

#define AR_OP_END \
    if (tx_param_isregister(params->mode_p1)) tx_cpu_reg_write(cpu, params->p1, result.u); \
    else \
        tx_cpu_mem_write32(cpu, PARAMA(1), result.u);

#define AR_SIMPLE_OP_1(name, op) \
    AR_OP_1_BEGIN(name) \
        result.i = op a.i; \
    AR_OP_END
#define AR_SIMPLE_OP_2(name, op) \
    AR_OP_2_BEGIN(name) \
        result.i = a.i op b.i; \
    AR_OP_END

#define AR_SIMPLE_UOP_1(name, op) \
    AR_OP_1_BEGIN(name) \
        result.u = op a.u; \
    AR_OP_END
#define AR_SIMPLE_UOP_2(name, op) \
    AR_OP_2_BEGIN(name) \
        result.u = a.u op b.u; \
    AR_OP_END

#define AR_SIMPLE_FOP_1(name, op) \
    AR_OP_1_BEGIN(name) \
        result.f = op a.f; \
    AR_OP_END
#define AR_SIMPLE_FOP_2(name, op) \
    AR_OP_2_BEGIN(name) \
        result.f = a.f op b.f; \
    AR_OP_END

#define AR_FUN_OP_1(name, fun) \
    AR_OP_1_BEGIN(name) \
        result.i = fun(a.i); \
    AR_OP_END
#define AR_FUN_OP_2(name, fun) \
    AR_OP_2_BEGIN(name) \
        result.i = fun(a.i, b.i); \
    AR_OP_END

#define AR_FUN_FOP_1(name, fun) \
    AR_OP_1_BEGIN(name) \
        result.f = fun(a.f); \
    AR_OP_END
#define AR_FUN_FOP_2(name, fun) \
    AR_OP_2_BEGIN(name) \
        result.f = fun(a.f, b.f); \
    AR_OP_END

#define AR_FUN_UOP_2(name, fun) \
    AR_OP_2_BEGIN(name) \
        result.u = fun(a.u, b.u); \
    AR_OP_END

// Actual arithmetic operations

void tx_cpu_op_inc(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_OP_1("inc", 1 +) }
void tx_cpu_op_dec(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_OP_1("dec", -1 +) }
void tx_cpu_op_add(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_OP_2("add", +) }
void tx_cpu_op_sub(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_OP_2("sub", -) }
void tx_cpu_op_mul(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_OP_2("mul", *) }
void tx_cpu_op_div(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("div")
        if (b.i == 0) {
            tx_cpu_error(cpu, ERR_DIV_BY_ZERO);
            return;
        }
        result.i = a.i / b.i;
    AR_OP_END
}
void tx_cpu_op_mod(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("mod")
        if (b.i == 0) {
            tx_cpu_error(cpu, ERR_DIV_BY_ZERO);
            return;
        }
        result.i = a.i % b.i;
    AR_OP_END
}
void tx_cpu_op_max(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_OP_2("max", MAX) }
void tx_cpu_op_min(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_OP_2("min", MIN) }
void tx_cpu_op_abs(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_OP_1("abs", abs) }
void tx_cpu_op_sign(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_OP_1("sign", SGN) }

void tx_cpu_op_and(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_UOP_2("and", &) }
void tx_cpu_op_or(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_UOP_2("or", |) }
void tx_cpu_op_not(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_UOP_1("not", ~) }
void tx_cpu_op_nand(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("nand")
        result.u = ~(a.u & b.u);
    AR_OP_END
}
void tx_cpu_op_xor(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_UOP_2("xor", ^) }
void tx_cpu_op_slr(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("shr")
        if (b.u >= 32) result.u = 0;
        else result.u = a.u >> b.u;
    AR_OP_END
}
void tx_cpu_op_sar(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("sar")
        if (b.u >= 32) result.u = a.i < 0 ? -1 : 0;
        else result.i = a.i >> b.u;
    AR_OP_END
}
void tx_cpu_op_sll(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("shl")
        if (b.u >= 32) result.u = 0;
        else result.u = a.u << b.u;
    AR_OP_END
}
void tx_cpu_op_ror(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("ror")
        const tx_uint32 mask = 31;
        b.u &= mask;
        result.u = (a.u >> b.u) | (a.u << ((-b.u) & mask));
    AR_OP_END
}
void tx_cpu_op_rol(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("ror")
        const tx_uint32 mask = 31;
        b.u &= mask;
        result.u = (a.u << b.u) | (a.u >> ((-b.u) & mask));
    AR_OP_END
}
void tx_cpu_op_set(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("set")
        if (b.u > 31) return;
        result.u = a.u | (1u << b.u);
    AR_OP_END
}
void tx_cpu_op_clr(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("clr")
        if (b.u > 31) return;
        result.u = a.u & ~(1u << b.u);
    AR_OP_END
}
void tx_cpu_op_tgl(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("tgl")
        if (b.u > 31) return;
        result.u = a.u ^ (1u << b.u);
    AR_OP_END
}
void tx_cpu_op_test(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE(name) \
    tx_num32 a = { .u = PARAMV(1) };
    tx_num32 b = { .u = PARAMV(2) };
    if (b.u > 31) return;
    tx_cpu_write_r(cpu, (a.u >> b.u) & 1u);
}

void tx_cpu_op_finc(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_1("finc", 1 +) }
void tx_cpu_op_fdec(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_1("fdec", -1 +) }
void tx_cpu_op_fadd(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_2("fadd", +) }
void tx_cpu_op_fsub(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_2("fsub", -) }
void tx_cpu_op_fmul(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_2("fmul", *) }
void tx_cpu_op_fdiv(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_FOP_2("fdiv", /) }
void tx_cpu_op_fmod(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_2("fmod", fmodf) }
void tx_cpu_op_fmax(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_2("fmax", MAX) }
void tx_cpu_op_fmin(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_2("fmin", MIN) }
void tx_cpu_op_fabs(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("fabs", fabsf) }
void tx_cpu_op_fsign(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("fsign", SGN) }
void tx_cpu_op_sin(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("sin", sinf) }
void tx_cpu_op_cos(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("cos", cosf) }
void tx_cpu_op_tan(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("tan", tanf) }
void tx_cpu_op_asin(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("asin", asinf) }
void tx_cpu_op_acos(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("acos", acosf) }
void tx_cpu_op_atan(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("atan", atanf) }
void tx_cpu_op_atan2(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_2("atan2", atan2f) }
void tx_cpu_op_sqrt(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("sqrt", sqrtf) }
void tx_cpu_op_pow(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_2("pow", powf) }
void tx_cpu_op_exp(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("exp", expf) }
void tx_cpu_op_log(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("log", logf) }
void tx_cpu_op_log2(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("log2", log2f) }
void tx_cpu_op_log10(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_FOP_1("log10", log10f) }

void tx_cpu_op_umul(tx_CPU* cpu, tx_Parameters* params) { AR_SIMPLE_UOP_2("umul", *) }
void tx_cpu_op_udiv(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("udiv")
        if (b.u == 0) {
            tx_cpu_error(cpu, ERR_DIV_BY_ZERO);
            return;
        }
        result.u = a.u / b.u;
    AR_OP_END
}
void tx_cpu_op_umod(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_2_BEGIN("umod")
        if (b.u == 0) {
            tx_cpu_error(cpu, ERR_DIV_BY_ZERO);
            return;
        }
        result.u = a.u % b.u;
    AR_OP_END
}
void tx_cpu_op_umax(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_UOP_2("umax", MAX) }
void tx_cpu_op_umin(tx_CPU* cpu, tx_Parameters* params) { AR_FUN_UOP_2("umin", MIN) }

void tx_cpu_op_rand(tx_CPU* cpu, tx_Parameters* params) {
    CHECK_WRITABLE("rand")
        tx_num32 result;
        result.f = ((tx_float32)tx_cpu_rand(cpu)) / ((tx_float32)tx_RAND_MAX);
    AR_OP_END
}
void tx_cpu_op_rseed(tx_CPU* cpu, tx_Parameters* params) { cpu->rseed = PARAMV(1); }
void tx_cpu_op_itf(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_1_BEGIN("itf")
    result.f = (tx_float32)a.i;
    AR_OP_END
}
void tx_cpu_op_fti(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_1_BEGIN("fti")
    result.i = (tx_int32)a.f;
    AR_OP_END
}
void tx_cpu_op_utf(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_1_BEGIN("utf")
        result.f = (tx_float32)a.u;
    AR_OP_END
}
void tx_cpu_op_ftu(tx_CPU* cpu, tx_Parameters* params) {
    AR_OP_1_BEGIN("ftu")
        result.u = (tx_uint32)a.f;
    AR_OP_END
}
void tx_cpu_op_ei(tx_CPU* cpu, tx_Parameters* params) { }
void tx_cpu_op_di(tx_CPU* cpu, tx_Parameters* params) { }

// Invalid operation
void tx_cpu_op_inv(tx_CPU* cpu, tx_Parameters* params) {
    tx_log_err("Invalid opcode at %x: %x", cpu->p, cpu->mem[cpu->p]);
}

// TODO toggle bit
