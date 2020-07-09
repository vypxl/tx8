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

/// List of opcodes understood by a tx8 cpu
/// Contains constants for every opcode
typedef enum tx_Opcode {
    tx_op_nop = 0x00,
    tx_op_jmp = 0x01,
    tx_op_jeq = 0x02,
    tx_op_jne = 0x03,
    tx_op_jgt = 0x04,
    tx_op_jge = 0x05,
    tx_op_jlt = 0x06,
    tx_op_jle = 0x07,
    tx_op_cal = 0x08,
    tx_op_ret = 0x09,
    tx_op_sys = 0x0a,
    tx_op_hlt = 0x0b,

    tx_op_mov = 0x10,
    tx_op_mxv = 0x11,
    tx_op_lda = 0x12,
    tx_op_sta = 0x13,
    tx_op_ldb = 0x14,
    tx_op_stb = 0x15,
    tx_op_ldc = 0x16,
    tx_op_stc = 0x17,
    tx_op_ldd = 0x18,
    tx_op_std = 0x19,
    tx_op_zer = 0x1a,
    tx_op_psh = 0x1b,
    tx_op_pop = 0x1c,

    tx_op_inc     = 0x20,
    tx_op_dec     = 0x21,
    tx_op_add     = 0x22,
    tx_op_sub     = 0x23,
    tx_op_mul     = 0x24,
    tx_op_div     = 0x25,
    tx_op_mod     = 0x26,
    tx_op_and     = 0x27,
    tx_op_ora     = 0x28,
    tx_op_not     = 0x29,
    tx_op_nnd     = 0x2a,
    tx_op_xor     = 0x2b,
    tx_op_shr     = 0x2c,
    tx_op_shl     = 0x2d,
    tx_op_ror     = 0x2e,
    tx_op_rol     = 0x2f,
    tx_op_fin     = 0x30,
    tx_op_fde     = 0x31,
    tx_op_fad     = 0x32,
    tx_op_fsu     = 0x33,
    tx_op_fmu     = 0x34,
    tx_op_fdi     = 0x35,
    tx_op_fmo     = 0x36,
    tx_op_itf     = 0x37,
    tx_op_fti     = 0x38,
    tx_op_max     = 0x39,
    tx_op_min     = 0x3a,
    tx_op_fmx     = 0x3b,
    tx_op_fmn     = 0x3c,
    tx_op_sin     = 0x3d,
    tx_op_cos     = 0x3e,
    tx_op_tan     = 0x3f,
    tx_op_atn     = 0x40,
    tx_op_sqt     = 0x41,
    tx_op_abs     = 0x42,
    tx_op_fab     = 0x43,
    tx_op_rnd     = 0x44,
    tx_op_rsd     = 0x45,
    tx_op_umu     = 0x46,
    tx_op_udi     = 0x47,
    tx_op_umx     = 0x48,
    tx_op_umn     = 0x49,
    tx_op_utf     = 0x4a,
    tx_op_ftu     = 0x4b,
    tx_op_invalid = 0xff
} tx_Opcode;

// clang-format off
/// Mapping of tx8 opcodes to their respective human readable names
static const char tx_op_names[256][4] = {
    // 0x0
    "nop", "jmp", "jeq", "jne", "jgt", "jge", "jlt", "jle", "cal", "ret", "sys", "hlt", "INV", "INV", "INV", "INV",
    // 0x1
    "mov", "mxv", "lda", "sta", "ldb", "stb", "ldc", "stc", "ldd", "std", "zer", "psh", "pop", "INV", "INV", "INV",
    // 0x2
    "inc", "dec", "add", "sub", "mul", "div", "mod", "and", "ora", "not", "nnd", "xor", "shr", "shl", "ror", "rol",
    // 0x3
    "fin", "fde", "fad", "fsu", "fmu", "fdi", "fmo", "itf", "fti", "max", "min", "fmx", "fmn", "sin", "cos", "tan",
    // 0x4
    "atn", "sqt", "abs", "fab", "rnd", "rsd", "umu", "udi", "umx", "umn", "utf", "ftu", "INV", "INV", "INV", "INV",
    // 0x5
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0x6
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0x7
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0x8
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0x9
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xa
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xb
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xc
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xd
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xe
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    // 0xf
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
};
// clang-format on

/// Convert a human readable tx8 opcode name to its corresponding opcode
static inline tx_Opcode tx_opcode_from_name(const char* name) {
    for (tx_uint32 i = 0; i < tx_op_invalid; ++i)
        if (strcmp(tx_op_names[i], name) == 0) return i;

    return tx_op_invalid;
}

typedef enum tx_Register {
    tx_reg_a            = 0x0,
    tx_reg_b            = 0x1,
    tx_reg_c            = 0x2,
    tx_reg_d            = 0x3,
    tx_reg_o            = 0x4,
    tx_reg_p            = 0x5,
    tx_reg_s            = 0x6,
    tx_reg_as           = 0x20,
    tx_reg_bs           = 0x21,
    tx_reg_cs           = 0x22,
    tx_reg_ds           = 0x23,
    tx_reg_os           = 0x24,
    tx_reg_ps           = 0x25,
    tx_reg_ss           = 0x26,
    tx_reg_ab           = 0x10,
    tx_reg_bb           = 0x11,
    tx_reg_cb           = 0x12,
    tx_reg_db           = 0x13,
    tx_reg_ob           = 0x14,
    tx_reg_pb           = 0x15,
    tx_reg_sb           = 0x16,
    tx_register_invalid = 0xff
} tx_Register;

// Constants for getting different attributs of a register id
#define tx_REG_SIZE_4    0x00U
#define tx_REG_SIZE_1    0x10U
#define tx_REG_SIZE_2    0x20U
#define tx_REG_SIZE_MASK 0xf0U
#define tx_REG_ID_MASK   0x0fU

// clang-format off
/// Mapping of tx8 cpu registers to their respective human readable names
static const char tx_reg_names[256][2] = {
    "a",  "b",  "c",  "d",  "o",  "p",  "s",  "", "", "", "", "", "", "", "", "",
    "as", "bs", "cs", "ds", "os", "ps", "ss", "", "", "", "", "", "", "", "", "",
    "ab", "bb", "cb", "db", "ob", "pb", "sb", "", "", "", "", "", "", "", "", "",
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
        case 'a': id = tx_reg_a; break;
        case 'b': id = tx_reg_b; break;
        case 'c': id = tx_reg_c; break;
        case 'd': id = tx_reg_d; break;
        case 'o': id = tx_reg_o; break;
        case 's': id = tx_reg_s; break;
        case 'p': id = tx_reg_p; break;
        default: return tx_register_invalid; break;
    }

    switch (name[1]) {
        case 's': id |= tx_REG_SIZE_2; break;
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

/// List of tx8 instruction parameter modes
typedef enum tx_ParamMode {
    tx_param_unused           = 0x0,
    tx_param_constant8        = 0x1,
    tx_param_constant16       = 0x2,
    tx_param_constant32       = 0x3,
    tx_param_absolute_address = 0x4,
    tx_param_relative_address = 0x5,
    tx_param_register         = 0x6,
    tx_param_register_address = 0x7
} tx_ParamMode;

/// Get the size of a tx8 instruction parameter in bytes
static inline tx_uint8 tx_param_value_size(tx_uint32 param, tx_ParamMode mode) {
    switch (mode) {
        case tx_param_constant8: return 1; break;
        case tx_param_constant16: return 2; break;
        case tx_param_constant32:
        case tx_param_absolute_address:
        case tx_param_relative_address:
        case tx_param_register_address: return 4;
        case tx_param_register: return tx_register_size(param);
        default: return 0; break;
    }
    return 0;
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
static const tx_uint8  tx_param_sizes[0x8] = {0, 1, 2, 4, 3, 3, 1, 1};
/// Masks to truncate a parameter value to its meaningful bytes (indexed by parameter mode)
static const tx_uint32 tx_param_masks[0x8] = {
    0, 0xff, 0xffff, 0xffffffff, 0xffffff, 0xffffff, 0xff, 0xff};
/// Mask to get the mode of parameter 2 in the first parameter mode byte
#define tx_PARAM_MODE_2_MASK 0xfU

// clang-format off
/// Mapping of opcodes to their respective parameter counts
static const tx_uint8 tx_param_count[256] = {
    // 0x0
    0, 1, 3, 3, 3, 3, 3, 3, 1, 0, 1, 0, 0, 0, 0, 0,
    // 0x1
    2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    // 0x2
    1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2,
    // 0x3
    1, 2, 2, 2, 2, 2, 2, 1, 1, 3, 3, 3, 3, 1, 1, 1,
    // 0x4
    3, 1, 1, 1, 1, 1, 2, 2, 3, 3, 1, 1, 0, 0, 0, 0,
    // 0x5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
static const tx_uint8 tx_param_mode_bytes[4] = {0, 1, 1, 2};

/// Struct representing the three possible parameters to an instruction
typedef struct tx_Parameters {
    tx_ParamMode mode_p1, mode_p2, mode_p3;
    tx_uint32    p1, p2, p3;
} tx_Parameters;

/// Struct representing a single instruction with opcode, parameters and a length in bytes (binary length)
typedef struct tx_Instruction {
    tx_Opcode     opcode;
    tx_Parameters parameters;
    tx_uint8      len;
} tx_Instruction;

/// The maximum length of a binary instruction in bytes
#define tx_INSTRUCTION_MAX_LENGTH 0xe

#pragma clang diagnostic warning "-Wunused-function"
