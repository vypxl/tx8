#include "tx8/core/cpu.hpp"

#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"
#include "tx8/core/types.hpp"
#include "tx8/core/util.hpp"

#include <cmath>
#include <utility>

#pragma clang diagnostic ignored "-Wunused-parameter"

#define ERR_ROM_TOO_LARGE "Could not initialize CPU: rom is too large"
#define ERR_INVALID_PC    "Exception: Invalid program counter"
#define ERR_SYSFUNC_REREGISTER \
    "Could not register sysfunction '{}' as another function is already registered under the " \
    "same name"
#define ERR_SYSFUNC_REALLOC \
    "Could not register sysfunction '{}'; An error occurred while reallocating the sysfunction " \
    "hashtable."
#define ERR_SYSFUNC_NOT_FOUND "Exception: Could not find sysfunction '{}'"
#define ERR_INVALID_REG_ID    "Exception: Invalid register id {:#x}"
#define ERR_INVALID_REG_SIZE  "Exception: Invalid register size {:#x}"
#define ERR_CANNOT_LOAD_WORD  "Exception: Cannot load a word into a smaller register"
#define ERR_DIV_BY_ZERO       "Exception: Division by zero"

namespace tx {
    CPU::CPU(Rom rom) { // NOLINT
        if (rom.size() > ROM_SIZE) {
            error(ERR_ROM_TOO_LARGE);
            return;
        }

        // initialize registers and memory
        halted  = false;
        stopped = false;
        debug   = false;
        rseed   = RAND_INITIAL_SEED;
        a       = 0;
        b       = 0;
        c       = 0;
        d       = 0;
        o       = 0;
        r       = 0;
        s       = STACK_BEGIN;
        p       = ENTRY_POINT;
        mem     = std::vector<uint8>(MEM_SIZE);

        // load rom into memory
        std::copy(rom.begin(), rom.end(), mem.begin() + ROM_START);
    }

    void CPU::run() {
        if (debug) log_err("[cpu] Beginning execution...\n");

        tx::uint32 prev_p;
        while (!halted) {
            if (p > MEM_SIZE - INSTRUCTION_MAX_LENGTH - 1 || p < 0) {
                error(ERR_INVALID_PC);
                break;
            }

            // TODO: implement properly upon implementing interrupts
            if (stopped) {
                halted = true;
                if (debug) log_err("[cpu] Stopped.\n");
                break;
            }

            Instruction current_instruction = parse_instruction(p);

            if (debug && current_instruction.opcode != Opcode::Nop) {
                log_err("[cpu] [#{:x}] {}\n", p, current_instruction);
            }
            prev_p = p;
            exec_instruction(current_instruction);

            // do not increment p if instruction changes p
            if (p == prev_p) p += current_instruction.len;
        }
        if (debug) log_err("[cpu] Halted.\n");
    }

    uint32 CPU::rand() {
        return ((rseed = (rseed * 214013 + 2541011)) >> 16) & RANDOM_MAX; // NOLINT
    }

    Instruction CPU::parse_instruction(mem_addr pc) {
        if (pc > MEM_SIZE - INSTRUCTION_MAX_LENGTH - 1 || pc < 0) {
            error_raw(ERR_INVALID_PC);
            Instruction nop = {};
            nop.opcode      = Opcode::Nop;
            nop.len         = 1;
            return nop;
        }

        mem_ptr p      = mem.data() + pc;
        uint8   pcount = param_count[p[0]];

        uint8 mode_p1 = pcount > 0 ? p[1] >> 4U : 0;
        uint8 mode_p2 = pcount > 1 ? p[1] & PARAM_MODE_2_MASK : 0;

        mem_addr param_start = pc + 1 + param_mode_bytes[pcount];

        uint32 value_p1 = mem_read(param_start) & param_masks[mode_p1];
        uint32 value_p2 = mem_read(param_start + param_sizes[mode_p1]) & param_masks[mode_p2];

        // clang-format off
        Instruction inst = {
            .opcode     = (Opcode)p[0],
            .params = {
                .p1 = {
                    .value  = { .u = value_p1 },
                    .mode = (ParamMode) mode_p1,
                },
                .p2 = {
                    .value  = { .u = value_p2 },
                    .mode = (ParamMode) mode_p2,
                }
            },
            .len = (uint8) (1 + param_mode_bytes[pcount] + param_sizes[mode_p1] + param_sizes[mode_p2])
        };
        // clang-format on

        return inst;
    }

    void CPU::exec_instruction(Instruction instruction) {
        op_function[(size_t) instruction.opcode](this, instruction.params);
    }

    void CPU::register_sysfunc(const std::string& name, Sysfunc func) {
        auto h = tx::str_hash(name);

        if (sys_func_table.contains(h)) error(ERR_SYSFUNC_REREGISTER, name);
        else sys_func_table[h] = std::move(func);
    }

    void CPU::exec_sysfunc(uint32 hashed_name) {
        auto it = sys_func_table.find(hashed_name);
        if (it == sys_func_table.end()) error(ERR_SYSFUNC_NOT_FOUND, hashed_name);
        else {
            Sysfunc f = it->second;
            f(*this);
        }
    }

    // Returns the numerical value of a parameter
    uint32 CPU::get_param_value(Parameter param) {
        switch (param.mode) {
            case ParamMode::Unused: return 0;
            case ParamMode::Constant8:
            case ParamMode::Constant16:
            case ParamMode::Constant32: return param.value.u;
            case ParamMode::AbsoluteAddress: return mem_read(param.value.u);
            case ParamMode::RelativeAddress: return mem_read_rel(param.value.u);
            case ParamMode::Register: return reg_read((Register) param.value.u);
            case ParamMode::RegisterAddress: return mem_read(reg_read((Register) param.value.u));
            default: return 0;
        }
    }

    // Returns the numerical value of a parameter, but sign extends values smaller than 32 pit instead of zero-extending
    uint32 CPU::get_param_value_sign_extended(Parameter param) {
        switch (param.mode) {
            case ParamMode::Unused: return 0;
            case ParamMode::Constant8: return (int32) (int8) param.value.u;
            case ParamMode::Constant16: return (int32) (int16) param.value.u;
            case ParamMode::Constant32: return param.value.u;
            case ParamMode::AbsoluteAddress: return mem_read(param.value.u);
            case ParamMode::RelativeAddress: return mem_read_rel(param.value.u);
            case ParamMode::Register: return reg_read_sign_extended((Register) param.value.u);
            case ParamMode::RegisterAddress: return mem_read(reg_read((Register) param.value.u));
            default: return 0;
        }
    }

    mem_addr CPU::get_param_address(Parameter param) {
        switch (param.mode) {
            case ParamMode::AbsoluteAddress: return param.value.u;
            case ParamMode::RelativeAddress: return o + param.value.u;
            case ParamMode::RegisterAddress: return reg_read((Register) param.value.u);
            default: error("Parameter is not an address"); return MEM_SIZE;
        }
    }

    void CPU::jump(mem_addr location) { p = location; }

    void CPU::push(uint32 value, ValueSize size) {
        s -= (uint32) size;
        mem_write(s, value, size);
    }

    uint32 CPU::pop(ValueSize size) {
        s += (uint32) size;
        return mem_read(s - (uint32) size, size);
    }

    uint32 CPU::top(ValueSize size) { return mem_read(s, size); }

    uint8* CPU::mem_get_ptr(mem_addr location) { return (location < MEM_SIZE) ? mem.data() + location : nullptr; }

    void CPU::mem_write(mem_addr location, uint32 value, ValueSize size) {
        uint8* p = mem.data() + (location & MEM_SIZE);

        auto bytes_to_write = (uint32) size;
        if (MEM_SIZE - location < bytes_to_write) bytes_to_write = MEM_SIZE - location;
        memcpy(p, &value, bytes_to_write);
    }

    uint32 CPU::mem_read(mem_addr location, ValueSize size) {
        uint32 value = 0;
        uint8* p     = mem.data() + (location & MEM_SIZE);

        auto bytes_to_read = (uint32) size;
        if (MEM_SIZE - location < bytes_to_read) bytes_to_read = MEM_SIZE - location;
        memcpy(&value, p, bytes_to_read);

        return value;
    }

    void   CPU::mem_write_rel(mem_addr location, uint32 value, ValueSize size) { mem_write(o + location, value, size); }
    uint32 CPU::mem_read_rel(mem_addr location, ValueSize size) { return mem_read(o + location, size); }

    void CPU::reg_write(Register which, uint32 value) {
        uint32 id = ((uint32) which) & REG_ID_MASK;
        if (id > REGISTER_COUNT) error(ERR_INVALID_REG_ID, id);
        else switch (((uint32) which) & REG_SIZE_MASK) {
                case REG_SIZE_1: *((uint8*) (registers.data() + id)) = (uint8) value; break;
                case REG_SIZE_2: *((uint16*) (registers.data() + id)) = (uint16) value; break;
                case REG_SIZE_4: *((uint32*) (registers.data() + id)) = (uint32) value; break;
                default: error(ERR_INVALID_REG_SIZE, id); break;
            }
    }

    uint32 CPU::reg_read(Register which) {
        uint32 id   = ((uint32) which) & REG_ID_MASK;
        uint32 size = ((uint32) which) & REG_SIZE_MASK;
        if (id > REGISTER_COUNT) error(ERR_INVALID_REG_ID, id);
        // because REG_SIZE_4 is 0x00, the only valid register sizes are 0x00, 0x10 and 0x20
        else if (size > REG_SIZE_2) error(ERR_INVALID_REG_SIZE, id);
        else return registers[id] & register_mask[size >> 4U];
        return 0;
    }

    uint32 CPU::reg_read_sign_extended(Register which) {
        uint32 val = reg_read(which);
        switch ((uint32) which & REG_SIZE_MASK) {
            case REG_SIZE_1: return (int32) (int8) val;
            case REG_SIZE_2: return (int32) (int16) val;
            default: return val;
        }
    }

// local conveniences macros
#define PARAMV(which)  get_param_value(params.p##which)
#define PARAMVI(which) get_param_value_sign_extended(params.p##which)
#define PARAMA(which)  get_param_address(params.p##which)
#define CHECK_WRITABLE(name) \
    if (!param_is_writable(params.p1.mode)) { \
        error("Destination of " #name " is not writable"); \
        return; \
    }

    void CPU::op_hlt(const Parameters& params) { halted = true; }

    void CPU::op_nop(const Parameters& params) { }

    void CPU::op_jmp(const Parameters& params) { jump(PARAMV(1)); }

#define COMP_JUMP(comparison, name) \
    void CPU::op_##name(const Parameters& params) { \
        /* NOLINT */ if (read_r() comparison 0) \
            jump(PARAMV(1)); \
    }

    COMP_JUMP(==, jeq)
    COMP_JUMP(!=, jne)
    COMP_JUMP(>, jgt)
    COMP_JUMP(>=, jge)
    COMP_JUMP(<, jlt)
    COMP_JUMP(<=, jle)

#undef COMP_JUMP

    void CPU::op_cmp(const Parameters& params) {
        num32 a   = {.u = PARAMVI(1)};
        num32 b   = {.u = PARAMVI(2)};
        int32 res = CMP(a.i, b.i);
        write_r(res);
    }

#define COMPARISON(dtype, name) \
    void CPU::op_##name(const Parameters& params) { \
        num32 a   = {.u = PARAMV(1)}; \
        num32 b   = {.u = PARAMV(2)}; \
        int32 res = CMP(a.dtype, b.dtype); \
        write_r(res); \
    }

    COMPARISON(f, fcmp)
    COMPARISON(u, ucmp)

#undef COMPARISON

    void CPU::op_call(const Parameters& params) {
        push(p);
        jump(PARAMV(1));
    }

    void CPU::op_ret(const Parameters& params) { jump(pop()); }

    void CPU::op_sys(const Parameters& params) { exec_sysfunc(PARAMV(1)); }

#define LD(name, type) \
    void CPU::op_##name(const Parameters& params) { \
        CHECK_WRITABLE(#name) \
\
        uint32 val = type(2); \
\
        /* register <- value */ \
        if (param_is_register(params.p1.mode)) reg_write((Register) params.p1.value.u, val); \
        /* address <- address */ \
        else if (param_is_address(params.p2.mode)) mem_write(PARAMA(1), val, ValueSize::Byte); \
        /* address <- register */ \
        else if (param_is_register(params.p2.mode)) \
            mem_write(PARAMA(1), val, register_size((Register) params.p2.value.u)); \
        /* address <- constant */ \
        else mem_write(PARAMA(1), val, param_value_size(params.p2)); \
    }

    LD(ld, PARAMV)
    LD(lds, PARAMVI)

#undef LD

#define LW(name, type) \
    void CPU::op_##name(const Parameters& params) { \
        CHECK_WRITABLE(#name) \
\
        uint32 val = type(2); \
\
        if (param_is_register(params.p1.mode)) { \
            if (register_size((Register) params.p1.value.u) != ValueSize::Word) error(ERR_CANNOT_LOAD_WORD); \
            else reg_write((Register) params.p1.value.u, val); \
        } else { \
            mem_write(PARAMA(1), val); \
        } \
    }

    LW(lw, PARAMV)
    LW(lws, PARAMVI)

#undef LW

// Macro for defining ld and st ops for registers a through d
#define DEFINE_LDX_STX(name, which) \
    void CPU::op_ld##name(const Parameters& params) { reg_write(Register::which, PARAMV(1)); } \
    void CPU::op_st##name(const Parameters& params) { \
        CHECK_WRITABLE("st" #which) \
        mem_write(PARAMA(1), reg_read(Register::which)); \
    }
    DEFINE_LDX_STX(a, A)
    DEFINE_LDX_STX(b, B)
    DEFINE_LDX_STX(c, C)
    DEFINE_LDX_STX(d, D)

#undef DEFINE_LDX_STX

    void CPU::op_zero(const Parameters& params) {
        CHECK_WRITABLE("zero")

        if (param_is_register(params.p1.mode)) reg_write((Register) params.p1.value.u, 0);
        else mem_write(PARAMA(1), 0);
    }

    void CPU::op_push(const Parameters& params) {
        uint32 val = PARAMV(1);
        switch (param_value_size(params.p1)) {
            case ValueSize::Byte: push(val, ValueSize::Byte); break;
            case ValueSize::Short: push(val, ValueSize::Short); break;
            case ValueSize::Word: push(val); break;
            default: break;
        }
    }

    void CPU::op_pop(const Parameters& params) {
        CHECK_WRITABLE("pop")

        if (param_is_register(params.p1.mode)) {
            auto reg = (Register) params.p1.value.u;
            reg_write(reg, pop(register_size(reg)));
        } else mem_write(params.p1.value.u, pop());
    }

// Macros for defining arithmetic operations
#define AR_OP_0_BEGIN(name) \
    CHECK_WRITABLE(name) \
    num32 result;
#define AR_OP_1_BEGIN(name) \
    CHECK_WRITABLE(name) \
    num32 a = {.u = PARAMVI(1)}; \
    num32 result;
#define AR_OP_2_BEGIN(name) \
    CHECK_WRITABLE(name) \
    num32 a = {.u = PARAMVI(1)}; \
    num32 b = {.u = PARAMVI(2)}; \
    num32 result;
#define AR_UOP_1_BEGIN(name) \
    CHECK_WRITABLE(name) \
    num32 a = {.u = PARAMV(1)}; \
    num32 result;
#define AR_UOP_2_BEGIN(name) \
    CHECK_WRITABLE(name) \
    num32 a = {.u = PARAMV(1)}; \
    num32 b = {.u = PARAMV(2)}; \
    num32 result;

#define AR_OP_END \
    if (param_is_register(params.p1.mode)) reg_write((Register) params.p1.value.u, result.u); \
    else mem_write(PARAMA(1), result.u);

#define AR_SIMPLE_OP_1(name, op) \
    AR_OP_1_BEGIN(name) \
        result.i = op a.i; \
    AR_OP_END
#define AR_SIMPLE_OP_2(name, op) \
    AR_OP_2_BEGIN(name) \
        result.i = a.i op b.i; \
    AR_OP_END

#define AR_SIMPLE_UOP_1(name, op) \
    AR_UOP_1_BEGIN(name) \
        result.u = op a.u; \
    AR_OP_END
#define AR_SIMPLE_UOP_2(name, op) \
    AR_UOP_2_BEGIN(name) \
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
    AR_UOP_2_BEGIN(name) \
        result.u = fun(a.u, b.u); \
    AR_OP_END

#define R(x) write_r(x)
#define RF(x) \
    num32 __rf = {.f = (x)}; \
    write_r(__rf.u)
#define R_SIZES(one, half, full) \
    if (param_is_register(params.p1.mode)) { \
        uint32 reg = params.p1.value.u; \
        switch (reg & REG_SIZE_MASK) { \
            case REG_SIZE_1: R((one)); break; \
            case REG_SIZE_2: R((half)); break; \
            case REG_SIZE_4: R((full)); break; \
            default: break; \
        } \
    } else R((full))

#define AR_OVF_OP(name, type) \
    type(name) uint32 rval; \
    ValueSize         size = ValueSize::Word; \
    if (param_is_register(params.p1.mode)) size = register_size((Register) params.p1.value.u); \
    switch (size) { \
        case ValueSize::Byte: \
            rval = __builtin_##name##_overflow((uint8) a.u, (uint8) b.u, (uint8*) (&result.u)); \
            rval |= __builtin_##name##_overflow((int8) a.i, (int8) b.i, (int8*) (&result.i)) << 1; \
            break; \
        case ValueSize::Short: \
            rval = __builtin_##name##_overflow((uint16) a.u, (uint16) b.u, (uint16*) (&result.u)); \
            rval |= __builtin_##name##_overflow((int16) a.i, (int16) b.i, (int16*) (&result.i)) << 1; \
            break; \
        case ValueSize::Word: \
            rval = __builtin_##name##_overflow(a.u, b.u, &result.u); \
            rval |= __builtin_##name##_overflow(a.i, b.i, &result.i) << 1; \
            break; \
    } \
    AR_OP_END \
    R(rval);

#define AR_OVF_MUL(name, type, vtype, m) \
    m(name) type##64_t a_64 = a.vtype; \
    type##64_t b_64         = b.vtype; \
    type##64_t r_64         = a_64 * b_64; \
    result.vtype            = r_64; \
    AR_OP_END \
    R(r_64 >> 32u);

    // Actual arithmetic operations

    void CPU::op_inc(const Parameters& params) {
        AR_UOP_1_BEGIN(inc)
            result.u = a.u;
            result.u++;
            if (param_is_register(params.p1.mode)) result.u &= register_mask[(params.p1.value.u & REG_SIZE_MASK) >> 4u];
        AR_OP_END
        R_SIZES(
            ((result.u == (uint8) INT8_MIN) << 1u) | (result.u == 0),
            ((result.u == (uint16) INT16_MIN) << 1u) | (result.u == 0),
            ((result.u == (uint32) INT32_MIN) << 1u) | (result.u == 0)
        );
    }
    void CPU::op_dec(const Parameters& params) {
        AR_UOP_1_BEGIN(dec)
            result.u = a.u;
            result.u--;
            if (param_is_register(params.p1.mode)) result.u &= register_mask[(params.p1.value.u & REG_SIZE_MASK) >> 4u];
        AR_OP_END
        R_SIZES(
            ((result.u == (uint8) INT8_MAX) << 1u) | (result.u == UINT8_MAX),
            ((result.u == (uint16) INT16_MAX) << 1u) | (result.u == UINT16_MAX),
            ((result.u == (uint32) INT32_MAX) << 1u) | (result.u == UINT32_MAX)
        );
    }
    void CPU::op_add(const Parameters& params) { AR_OVF_OP(add, AR_OP_2_BEGIN) }
    void CPU::op_sub(const Parameters& params) { AR_OVF_OP(sub, AR_OP_2_BEGIN) }
    void CPU::op_mul(const Parameters& params) { AR_OVF_MUL(mul, int, i, AR_OP_2_BEGIN) }
    void CPU::op_div(const Parameters& params) {
        AR_OP_2_BEGIN("div")
            if (b.i == 0) {
                error(ERR_DIV_BY_ZERO);
                return;
            }
            result.i = a.i / b.i;
        AR_OP_END
        R(a.i % b.i);
    }
    void CPU::op_mod(const Parameters& params) {
        AR_OP_2_BEGIN("mod")
            if (b.i == 0) {
                error(ERR_DIV_BY_ZERO);
                return;
            }
            result.i = a.i % b.i;
        AR_OP_END
    }
    void CPU::op_max(const Parameters& params) {
        AR_FUN_OP_2("max", MAX);
        R(MIN(a.i, b.i));
    }
    void CPU::op_min(const Parameters& params) {
        AR_FUN_OP_2("min", MIN);
        R(MAX(a.i, b.i));
    }
    void CPU::op_abs(const Parameters& params) {
        AR_FUN_OP_1("abs", abs);
        R(SGN(a.i));
    }
    void CPU::op_sign(const Parameters& params) { AR_FUN_OP_1("sign", SGN) }

    void CPU::op_and(const Parameters& params) { AR_SIMPLE_UOP_2("and", &) }
    void CPU::op_or(const Parameters& params) { AR_SIMPLE_UOP_2("or", |) }
    void CPU::op_not(const Parameters& params) { AR_SIMPLE_UOP_1("not", ~) }
    void CPU::op_nand(const Parameters& params) {
        AR_UOP_2_BEGIN("nand")
            result.u = ~(a.u & b.u);
        AR_OP_END
    }
    void CPU::op_xor(const Parameters& params) { AR_SIMPLE_UOP_2("xor", ^) }

#define BITMASK_5 0b11111u
#define BIT_TRUNC \
    uint32 mask = BITMASK_5; \
    if (param_is_register(params.p1.mode)) { \
        uint32 reg = params.p1.value.u; \
        switch (reg & REG_SIZE_MASK) { \
            case REG_SIZE_1: mask = 0b111u; break; \
            case REG_SIZE_2: mask = 0b1111u; break; \
            case REG_SIZE_4: mask = BITMASK_5; break; \
            default: break; \
        } \
    } \
    b.u &= mask;

    void CPU::op_slr(const Parameters& params) {
        AR_UOP_2_BEGIN("slr")
            BIT_TRUNC;
            result.u = a.u >> b.u;
        AR_OP_END
        R(b.u == 0 ? 0 : (a.u << (32 - b.u)) >> (32 - b.u));
    }
    void CPU::op_sar(const Parameters& params) {
        AR_OP_2_BEGIN("sar")
            BIT_TRUNC;
            result.i = a.i >> b.u;
        AR_OP_END
        R(b.u == 0 ? 0 : (a.u << (32 - b.u)) >> (32 - b.u));
    }
    void CPU::op_sll(const Parameters& params) {
        AR_UOP_2_BEGIN("sll")
            BIT_TRUNC;
            result.u = a.u << b.u;
        AR_OP_END
        R(b.u == 0 ? 0 : a.u >> ((mask + 1) - b.u));
    }
    void CPU::op_ror(const Parameters& params) {
        AR_UOP_2_BEGIN("ror")
            BIT_TRUNC;
            result.u = (a.u >> b.u) | (a.u << ((-b.u) & mask));
        AR_OP_END
    }
    void CPU::op_rol(const Parameters& params) {
        AR_UOP_2_BEGIN("ror")
            BIT_TRUNC;
            result.u = (a.u << b.u) | (a.u >> ((-b.u) & mask));
        AR_OP_END
    }
    void CPU::op_set(const Parameters& params) {
        AR_UOP_2_BEGIN("set")
            BIT_TRUNC;
            result.u = a.u | (1u << b.u);
        AR_OP_END
        R((a.u >> b.u) & 1u);
    }
    void CPU::op_clr(const Parameters& params) {
        AR_UOP_2_BEGIN("clr")
            BIT_TRUNC;
            result.u = a.u & ~(1u << b.u);
        AR_OP_END
        R((a.u >> b.u) & 1u);
    }
    void CPU::op_tgl(const Parameters& params) {
        AR_UOP_2_BEGIN("tgl")
            BIT_TRUNC;
            result.u = a.u ^ (1u << b.u);
        AR_OP_END
        R((a.u >> b.u) & 1u);
    }
    void CPU::op_test(const Parameters& params) {
        num32 a = {.u = PARAMV(1)};
        num32 b = {.u = PARAMV(2)};
        BIT_TRUNC;
        R((a.u >> b.u) & 1u);
    }

#undef BITMASK_5
#undef BIT_TRUNC

    void CPU::op_finc(const Parameters& params) { AR_SIMPLE_FOP_1("finc", 1 +) }
    void CPU::op_fdec(const Parameters& params) { AR_SIMPLE_FOP_1("fdec", -1 +) }
    void CPU::op_fadd(const Parameters& params) { AR_SIMPLE_FOP_2("fadd", +) }
    void CPU::op_fsub(const Parameters& params) { AR_SIMPLE_FOP_2("fsub", -) }
    void CPU::op_fmul(const Parameters& params) { AR_SIMPLE_FOP_2("fmul", *) }
    void CPU::op_fdiv(const Parameters& params) { AR_SIMPLE_FOP_2("fdiv", /) }
    void CPU::op_fmod(const Parameters& params) { AR_FUN_FOP_2("fmod", fmodf) }
    void CPU::op_fmax(const Parameters& params) {
        AR_FUN_FOP_2("fmax", MAX);
        RF(MIN(a.f, b.f));
    }
    void CPU::op_fmin(const Parameters& params) {
        AR_FUN_FOP_2("fmin", MIN);
        RF(MAX(a.f, b.f));
    }
    void CPU::op_fabs(const Parameters& params) {
        AR_FUN_FOP_1("fabs", fabsf);
        RF((float32) SGN(a.f));
    }
    void CPU::op_fsign(const Parameters& params) { AR_FUN_FOP_1("fsign", SGN) }
    void CPU::op_sin(const Parameters& params) { AR_FUN_FOP_1("sin", sinf) }
    void CPU::op_cos(const Parameters& params) { AR_FUN_FOP_1("cos", cosf) }
    void CPU::op_tan(const Parameters& params) { AR_FUN_FOP_1("tan", tanf) }
    void CPU::op_asin(const Parameters& params) { AR_FUN_FOP_1("asin", asinf) }
    void CPU::op_acos(const Parameters& params) { AR_FUN_FOP_1("acos", acosf) }
    void CPU::op_atan(const Parameters& params) { AR_FUN_FOP_1("atan", atanf) }
    void CPU::op_atan2(const Parameters& params) { AR_FUN_FOP_2("atan2", atan2f) }
    void CPU::op_sqrt(const Parameters& params) { AR_FUN_FOP_1("sqrt", sqrtf) }
    void CPU::op_pow(const Parameters& params) { AR_FUN_FOP_2("pow", powf) }
    void CPU::op_exp(const Parameters& params) { AR_FUN_FOP_1("exp", expf) }
    void CPU::op_log(const Parameters& params) { AR_FUN_FOP_1("log", logf) }
    void CPU::op_log2(const Parameters& params) { AR_FUN_FOP_1("log2", log2f) }
    void CPU::op_log10(const Parameters& params) { AR_FUN_FOP_1("log10", log10f) }

    void CPU::op_uadd(const Parameters& params) { AR_OVF_OP(add, AR_UOP_2_BEGIN) }
    void CPU::op_usub(const Parameters& params) { AR_OVF_OP(sub, AR_UOP_2_BEGIN) }
    void CPU::op_umul(const Parameters& params) { AR_OVF_MUL(umul, uint, u, AR_UOP_2_BEGIN); }
    void CPU::op_udiv(const Parameters& params) {
        AR_UOP_2_BEGIN("udiv")
            if (b.u == 0) {
                error(ERR_DIV_BY_ZERO);
                return;
            }
            result.u = a.u / b.u;
        AR_OP_END
        R(a.u % b.u);
    }
    void CPU::op_umod(const Parameters& params) {
        AR_UOP_2_BEGIN("umod")
            if (b.u == 0) {
                error(ERR_DIV_BY_ZERO);
                return;
            }
            result.u = a.u % b.u;
        AR_OP_END
    }
    void CPU::op_umax(const Parameters& params) {
        AR_FUN_UOP_2("umax", MAX);
        R(MIN(a.u, b.u));
    }
    void CPU::op_umin(const Parameters& params) {
        AR_FUN_UOP_2("umin", MIN);
        R(MAX(a.u, b.u));
    }

    void CPU::op_rand(const Parameters& params) {
        AR_OP_0_BEGIN("rand")
            uint32 rand_val = rand();
            result.f        = ((float32) rand_val) / ((float32) RANDOM_MAX);
        AR_OP_END
        R(rand_val);
    }
    void CPU::op_rseed(const Parameters& params) { rseed = PARAMV(1); }
    void CPU::op_itf(const Parameters& params) {
        AR_OP_1_BEGIN("itf")
            result.f = (float32) a.i;
        AR_OP_END
    }
    void CPU::op_fti(const Parameters& params) {
        AR_OP_1_BEGIN("fti")
            result.i = (int32) a.f;
        AR_OP_END
    }
    void CPU::op_utf(const Parameters& params) {
        AR_OP_1_BEGIN("utf")
            result.f = (float32) a.u;
        AR_OP_END
    }
    void CPU::op_ftu(const Parameters& params) {
        AR_OP_1_BEGIN("ftu")
            result.u = (uint32) a.f;
        AR_OP_END
    }
    void CPU::op_ei(const Parameters& params) { }
    void CPU::op_di(const Parameters& params) { }
    void CPU::op_stop(const Parameters& params) { stopped = true; }

    // Invalid operation
    void CPU::op_inv(const Parameters& params) { tx::log_err("Invalid opcode at #{:x}: {:x}", p, mem[p]); }

} // namespace tx

#undef AR_OVF_OP
#undef AR_OVF_MUL

#undef R
#undef R_SIZES

#undef PARAMV
#undef PARAMA
#undef CHECK_WRITABLE

#undef AR_OP_1_BEGIN
#undef AR_OP_2_BEGIN
#undef AR_OP_END
#undef AR_SIMPLE_OP_1
#undef AR_SIMPLE_OP_2
#undef AR_SIMPLE_UOP_1
#undef AR_SIMPLE_UOP_2
#undef AR_SIMPLE_FOP_1
#undef AR_SIMPLE_FOP_2
#undef AR_FUN_OP_1
#undef AR_FUN_OP_2
#undef AR_FUN_FOP_1
#undef AR_FUN_FOP_2
#undef AR_FUN_UOP_2

#undef ERR_ROM_TOO_LARGE
#undef ERR_INVALID_PC
#undef ERR_SYSFUNC_REREGISTER
#undef ERR_SYSFUNC_REALLOC
#undef ERR_SYSFUNC_NOT_FOUND
#undef ERR_INVALID_REG_ID
#undef ERR_INVALID_REG_SIZE
#undef ERR_INVALID_MEM_WRITE_COUNT
#undef ERR_CANNOT_LOAD_WORD
#undef ERR_DIV_BY_ZERO
