/**
 * @file instruction.h
 * @brief Constants, structs and conversion methods for various values used in the tx8 ecosystem.
 * @details Includes definitions for Opcodes, Registers, Parameters and Instructions
 */
#pragma once
#include "tx8/core/types.h"

#include <stdbool.h>
#include <string.h>

#pragma clang diagnostic ignored "-Wunused-function"

#ifdef __cplusplus
extern "C" {
#endif

/// List of opcodes understood by a tx8 cpu
/// Contains constants for every opcode
typedef enum tx_Opcode {
    tx_op_hlt  = 0x00,
    tx_op_nop  = 0x01,
    tx_op_jmp  = 0x02,
    tx_op_jeq  = 0x03,
    tx_op_jne  = 0x04,
    tx_op_jgt  = 0x05,
    tx_op_jge  = 0x06,
    tx_op_jlt  = 0x07,
    tx_op_jle  = 0x08,
    tx_op_cmp  = 0x09,
    tx_op_fcmp = 0x0a,
    tx_op_ucmp = 0x0b,
    tx_op_call = 0x0c,
    tx_op_ret  = 0x0d,
    tx_op_sys  = 0x0e,

    tx_op_ld   = 0x10,
    tx_op_lw   = 0x11,
    tx_op_lda  = 0x12,
    tx_op_sta  = 0x13,
    tx_op_ldb  = 0x14,
    tx_op_stb  = 0x15,
    tx_op_ldc  = 0x16,
    tx_op_stc  = 0x17,
    tx_op_ldd  = 0x18,
    tx_op_std  = 0x19,
    tx_op_zero = 0x1a,
    tx_op_push = 0x1b,
    tx_op_pop  = 0x1c,

    tx_op_inc  = 0x20,
    tx_op_dec  = 0x21,
    tx_op_add  = 0x22,
    tx_op_sub  = 0x23,
    tx_op_mul  = 0x24,
    tx_op_div  = 0x25,
    tx_op_mod  = 0x26,
    tx_op_max  = 0x27,
    tx_op_min  = 0x28,
    tx_op_abs  = 0x29,
    tx_op_sign = 0x2a,

    tx_op_and  = 0x30,
    tx_op_or   = 0x31,
    tx_op_not  = 0x32,
    tx_op_nand = 0x33,
    tx_op_xor  = 0x34,
    tx_op_slr  = 0x35,
    tx_op_sar  = 0x36,
    tx_op_sll  = 0x37,
    tx_op_ror  = 0x38,
    tx_op_rol  = 0x39,
    tx_op_set  = 0x3a,
    tx_op_clr  = 0x3b,
    tx_op_tgl  = 0x3c,
    tx_op_test = 0x3d,

    tx_op_finc  = 0x40,
    tx_op_fdec  = 0x41,
    tx_op_fadd  = 0x42,
    tx_op_fsub  = 0x43,
    tx_op_fmul  = 0x44,
    tx_op_fdiv  = 0x45,
    tx_op_fmod  = 0x46,
    tx_op_fmax  = 0x47,
    tx_op_fmin  = 0x48,
    tx_op_fabs  = 0x49,
    tx_op_fsign = 0x4a,
    tx_op_sin   = 0x4b,
    tx_op_cos   = 0x4c,
    tx_op_tan   = 0x4d,
    tx_op_asin  = 0x4e,
    tx_op_acos  = 0x4f,
    tx_op_atan  = 0x50,
    tx_op_atan2 = 0x51,
    tx_op_sqrt  = 0x52,
    tx_op_pow   = 0x53,
    tx_op_exp   = 0x54,
    tx_op_log   = 0x55,
    tx_op_log2  = 0x56,
    tx_op_log10 = 0x57,

    tx_op_umul = 0x60,
    tx_op_udiv = 0x61,
    tx_op_umod = 0x62,
    tx_op_umax = 0x63,
    tx_op_umin = 0x64,

    tx_op_rand  = 0x70,
    tx_op_rseed = 0x71,
    tx_op_itf   = 0x72,
    tx_op_fti   = 0x73,
    tx_op_utf   = 0x74,
    tx_op_ftu   = 0x75,
    tx_op_ei    = 0x76,
    tx_op_di    = 0x77,
    tx_op_stop  = 0x78,
    tx_op_invalid = 0xff
} tx_Opcode;

/// List of tx8 instruction parameter modes
typedef enum tx_ParamMode {
    tx_param_unused           = 0x0,
    tx_param_constant8        = 0x1,
    tx_param_constant16       = 0x2,
    tx_param_constant32       = 0x3,
    tx_param_absolute_address = 0x4,
    tx_param_relative_address = 0x5,
    tx_param_register         = 0x6,
    tx_param_register_address = 0x7,

    tx_param_label = 0xf, /// Only used by the assembler
} tx_ParamMode;

/// Struct representing an instruction parameters
typedef struct tx_Parameter {
    tx_ParamMode mode;
    tx_num32     value;
} tx_Parameter;

/// Struct holding the two parameters of an instruction for convenience
typedef struct tx_Parameters {
    tx_Parameter p1;
    tx_Parameter p2;
} tx_Parameters;

/// Struct representing a single instruction with opcode, parameters and a length in bytes (binary length)
typedef struct tx_Instruction {
    tx_Opcode     opcode;
    tx_Parameters params;
    tx_uint8      len;
} tx_Instruction;

/// Struct representing a label, consisting of a string name, an id and an absolute address
typedef struct tx_Label {
    char*     name;
    tx_uint32 id;
    tx_uint32 position;
} tx_Label;

// clang-format off
/// Mapping of tx8 opcodes to their respective human readable names
static const char tx_op_names[256][6] = {
    // 0x0
    "hlt",   "nop",   "jmp",   "jeq",   "jne",   "jgt",   "jge",   "jlt",   "jle",   "cmp",   "fcmp",  "ucmp",  "call",  "ret",   "sys",   "IN_0f",
    // 0x1
    "ld",    "lw",    "lda",   "sta",   "ldb",   "stb",   "ldc",   "stc",   "ldd",   "std",   "zero",  "push",  "pop",   "IN_1d", "IN_1e", "IN_1f",
    // 0x2
    "inc",   "dec",   "add",   "sub",   "mul",   "div",   "mod",   "max",   "min",   "abs",   "sign",  "IN_2b", "IN_2c", "IN_2d", "IN_2e", "IN_2f",
    // 0x3
    "and",   "or",    "not",   "nand",  "xor",   "slr",   "sar",   "sll",   "ror",   "rol",   "set",   "clr",   "tgl",  "test",   "IN_3e", "IN_3f",
    // 0x4
    "finc",  "fdec",  "fadd",  "fsub",  "fmul",  "fdiv",  "fmod",  "fmax",  "fmin",  "fabs",  "fsign", "sin",   "cos",   "tan",   "asin",   "acos",
    // 0x5
    "atan",  "atan2", "sqrt",  "pow",   "exp",   "log",   "log2",  "log10", "IN_58", "IN_59", "IN_5a", "IN_5b", "IN_5c", "IN_5d", "IN_5e", "IN_5f",
    // 0x6
    "umul",  "udiv",  "umod",  "umax",  "umin",  "IN_65", "IN_66", "IN_67", "IN_68", "IN_69", "IN_6a", "IN_6b", "IN_6c", "IN_6d", "IN_6e", "IN_6f",
    // 0x7
    "rand",  "rseed", "itf",   "fti",   "utf",   "ftu",   "ei",    "di",    "stop",  "IN_79", "IN_7a", "IN_7b", "IN_7c", "IN_7d", "IN_7e", "IN_7f",
    // 0x8
    "IN_80", "IN_81", "IN_82", "IN_83", "IN_84", "IN_85", "IN_86", "IN_87", "IN_88", "IN_89", "IN_8a", "IN_8b", "IN_8c", "IN_8d", "IN_8e", "IN_8f",
    // 0x9
    "IN_90", "IN_91", "IN_92", "IN_93", "IN_94", "IN_95", "IN_96", "IN_97", "IN_98", "IN_99", "IN_9a", "IN_9b", "IN_9c", "IN_9d", "IN_9e", "IN_9f",
    // 0xa
    "IN_a0", "IN_a1", "IN_a2", "IN_a3", "IN_a4", "IN_a5", "IN_a6", "IN_a7", "IN_a8", "IN_a9", "IN_aa", "IN_ab", "IN_ac", "IN_ad", "IN_ae", "IN_af",
    // 0xb
    "IN_b0", "IN_b1", "IN_b2", "IN_b3", "IN_b4", "IN_b5", "IN_b6", "IN_b7", "IN_b8", "IN_b9", "IN_ba", "IN_bb", "IN_bc", "IN_bd", "IN_be", "IN_bf",
    // 0xc
    "IN_c0", "IN_c1", "IN_c2", "IN_c3", "IN_c4", "IN_c5", "IN_c6", "IN_c7", "IN_c8", "IN_c9", "IN_ca", "IN_cb", "IN_cc", "IN_cd", "IN_ce", "IN_cf",
    // 0xd
    "IN_d0", "IN_d1", "IN_d2", "IN_d3", "IN_d4", "IN_d5", "IN_d6", "IN_d7", "IN_d8", "IN_d9", "IN_da", "IN_db", "IN_dc", "IN_dd", "IN_de", "IN_df",
    // 0xe
    "IN_e0", "IN_e1", "IN_e2", "IN_e3", "IN_e4", "IN_e5", "IN_e6", "IN_e7", "IN_e8", "IN_e9", "IN_ea", "IN_eb", "IN_ec", "IN_ed", "IN_ee", "IN_ef",
    // 0xf
    "IN_f0", "IN_f1", "IN_f2", "IN_f3", "IN_f4", "IN_f5", "IN_f6", "IN_f7", "IN_f8", "IN_f9", "IN_fa", "IN_fb", "IN_fc", "IN_fd", "IN_fe", "IN_ff",
};
// clang-format on

/// Convert a human readable tx8 opcode name to its corresponding opcode
static inline tx_Opcode tx_opcode_from_name(const char* name) {
    for (tx_uint32 i = 0; i < tx_op_invalid; ++i)
        if (strcmp(tx_op_names[i], name) == 0) return (tx_Opcode) i;

    return tx_op_invalid;
}

typedef enum tx_Register {
    tx_reg_a            = 0x0,
    tx_reg_b            = 0x1,
    tx_reg_c            = 0x2,
    tx_reg_d            = 0x3,
    tx_reg_r            = 0x4,
    tx_reg_o            = 0x5,
    tx_reg_p            = 0x6,
    tx_reg_s            = 0x7,
    tx_reg_as           = 0x20,
    tx_reg_bs           = 0x21,
    tx_reg_cs           = 0x22,
    tx_reg_ds           = 0x23,
    tx_reg_rs           = 0x24,
    tx_reg_os           = 0x25,
    tx_reg_ps           = 0x26,
    tx_reg_ss           = 0x27,
    tx_reg_ab           = 0x10,
    tx_reg_bb           = 0x11,
    tx_reg_cb           = 0x12,
    tx_reg_db           = 0x13,
    tx_reg_rb           = 0x14,
    tx_reg_ob           = 0x15,
    tx_reg_pb           = 0x16,
    tx_reg_sb           = 0x17,
    tx_register_invalid = 0xff
} tx_Register;

// Constants for getting different attributs of a register id
#define tx_REG_SIZE_4    0x00u
#define tx_REG_SIZE_1    0x10u
#define tx_REG_SIZE_2    0x20u
#define tx_REG_SIZE_MASK 0xf0u
#define tx_REG_ID_MASK   0x0fu

// clang-format off
/// Mapping of tx8 cpu registers to their respective human readable names
static const char tx_reg_names[256][3] = {
    "a",  "b",  "c",  "d",  "r",  "o",  "p",  "s",  "", "", "", "", "", "", "", "",
    "as", "bs", "cs", "ds", "rs", "os", "ps", "ss", "", "", "", "", "", "", "", "",
    "ab", "bb", "cb", "db", "rb", "ob", "pb", "sb", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "",
    "",    "",  "",   "",   "",   "",   "",   "", "", "", "", "", "", "", "", "IN"
};
// clang-format on

/// Convert a human readable tx8 cpu register name to its corresponding id
static inline tx_uint8 tx_reg_id_from_name(const char* name) {
    tx_uint8 id = 0;
    switch (name[0]) {
        case 'A':
        case 'a': id = tx_reg_a; break;
        case 'B':
        case 'b': id = tx_reg_b; break;
        case 'C':
        case 'c': id = tx_reg_c; break;
        case 'D':
        case 'd': id = tx_reg_d; break;
        case 'R':
        case 'r': id = tx_reg_r; break;
        case 'O':
        case 'o': id = tx_reg_o; break;
        case 'S':
        case 's': id = tx_reg_s; break;
        case 'P':
        case 'p': id = tx_reg_p; break;
        default: return tx_register_invalid; break;
    }

    switch (name[1]) {
        case 'S':
        case 's': id |= tx_REG_SIZE_2; break;
        case 'B':
        case 'b': id |= tx_REG_SIZE_1; break;
        default: break;
    }

    return id;
}

/// Masks to truncate a tx8 cpu register value according to the register size
static const tx_uint32 tx_register_mask[4] = {0xffffffff, 0xff, 0x0, 0xffffff};
/// Get the register size of a tx8 cpu register by its id
static inline tx_uint8 tx_register_size(tx_Register reg) {
    switch (reg & tx_REG_SIZE_MASK) {
        case tx_REG_SIZE_1: return 1; break;
        case tx_REG_SIZE_2: return 2; break;
        case tx_REG_SIZE_4: return 4; break;
        default: return 0; break;
    }
}

/// Get the size of a tx8 instruction parameter in bytes
static inline tx_uint8 tx_param_value_size(tx_Parameter param) {
    switch (param.mode) {
        case tx_param_constant8: return 1;
        case tx_param_constant16: return 2;
        case tx_param_constant32:
        case tx_param_absolute_address:
        case tx_param_relative_address:
        case tx_param_register_address: return 4;
        case tx_param_register: return tx_register_size((tx_Register) param.value.u);
        default: return 0;
    }
}

/// Check if a parameter represents a writable destination by its mode
static inline bool tx_param_iswritable(tx_ParamMode which) {
    return which == tx_param_register || which == tx_param_absolute_address
           || which == tx_param_relative_address || which == tx_param_register_address;
}

/// Check if a parameter represents some memory address by its mode
static inline bool tx_param_isaddress(tx_ParamMode which) {
    return which == tx_param_absolute_address || which == tx_param_relative_address
           || which == tx_param_register_address;
}

/// Check if a parameter represents a register by its mode
static inline bool tx_param_isregister(tx_ParamMode which) { return which == tx_param_register; };

/// Mapping of parameter modes to their byte size in tx8 binary
static const tx_uint8  tx_param_sizes[0x10] = {0, 1, 2, 4, 3, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 4};
/// Masks to truncate a parameter value to its meaningful bytes (indexed by parameter mode)
static const tx_uint32 tx_param_masks[0x8] = {
    0, 0xff, 0xffff, 0xffffffff, 0xffffff, 0xffffff, 0xff, 0xff};
/// Mask to get the mode of parameter 2 in the first parameter mode byte
#define tx_PARAM_MODE_2_MASK 0xfu

// clang-format off
/// Mapping of opcodes to their respective parameter counts
static const tx_uint8 tx_param_count[256] = {
    // 0x0
    0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 0, 1, 0,
    // 0x1
    2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    // 0x2
    1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0,
    // 0x3
    2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
    // 0x4
    1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
    // 0x5
    1, 2, 1, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x6
    2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x7
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xa
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xb
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xc
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xd
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xe
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xf
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
// clang-format on

/// Mapping of parameter counts to number of parameter mode bytes in a binary instruction
static const tx_uint8 tx_param_mode_bytes[3] = {0, 1, 1};

/// The maximum length of a binary instruction in bytes
#define tx_INSTRUCTION_MAX_LENGTH 0xa

#ifdef __cplusplus
}
#endif

#pragma clang diagnostic warning "-Wunused-function"
