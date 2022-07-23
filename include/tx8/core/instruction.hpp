/**
 * @file instruction.h
 * @brief Constants, structs and conversion methods for various values used in the tx8 ecosystem.
 * @details Includes definitions for Opcodes, Registers, Parameters and Instructions
 */
#pragma once
#include "tx8/core/types.hpp"

#include <array>
#include <string>

#pragma clang diagnostic ignored "-Wunused-function"

namespace tx {
    /// Enum to specify the amount of bytes a value should have
    /// @details Used in various memory-related functions
    enum class ValueSize { Byte = 1, Short = 2, Word = 4 };

    /// List of opcodes understood by a tx8 cpu
    /// Contains constants for every opcode
    enum class Opcode {
        Hlt  = 0x00,
        Nop  = 0x01,
        Jmp  = 0x02,
        Jeq  = 0x03,
        Jne  = 0x04,
        Jgt  = 0x05,
        Jge  = 0x06,
        Jlt  = 0x07,
        Jle  = 0x08,
        Cmp  = 0x09,
        Fcmp = 0x0a,
        Ucmp = 0x0b,
        Call = 0x0c,
        Ret  = 0x0d,
        Sys  = 0x0e,

        Ld   = 0x10,
        Lw   = 0x11,
        Lda  = 0x12,
        Sta  = 0x13,
        Ldb  = 0x14,
        Stb  = 0x15,
        Ldc  = 0x16,
        Stc  = 0x17,
        Ldd  = 0x18,
        Std  = 0x19,
        Zero = 0x1a,
        Push = 0x1b,
        Pop  = 0x1c,

        Inc  = 0x20,
        Dec  = 0x21,
        Add  = 0x22,
        Sub  = 0x23,
        Mul  = 0x24,
        Div  = 0x25,
        Mod  = 0x26,
        Max  = 0x27,
        Min  = 0x28,
        Abs  = 0x29,
        Sign = 0x2a,

        And  = 0x30,
        Or   = 0x31,
        Not  = 0x32,
        Nand = 0x33,
        Xor  = 0x34,
        Slr  = 0x35,
        Sar  = 0x36,
        Sll  = 0x37,
        Ror  = 0x38,
        Rol  = 0x39,
        Set  = 0x3a,
        Clr  = 0x3b,
        Tgl  = 0x3c,
        Test = 0x3d,

        Finc  = 0x40,
        Fdec  = 0x41,
        Fadd  = 0x42,
        Fsub  = 0x43,
        Fmul  = 0x44,
        Fdiv  = 0x45,
        Fmod  = 0x46,
        Fmax  = 0x47,
        Fmin  = 0x48,
        Fabs  = 0x49,
        Fsign = 0x4a,
        Sin   = 0x4b,
        Cos   = 0x4c,
        Tan   = 0x4d,
        Asin  = 0x4e,
        Acos  = 0x4f,
        Atan  = 0x50,
        Atan2 = 0x51,
        Sqrt  = 0x52,
        Pow   = 0x53,
        Exp   = 0x54,
        Log   = 0x55,
        Log2  = 0x56,
        Log10 = 0x57,

        Umul = 0x60,
        Udiv = 0x61,
        Umod = 0x62,
        Umax = 0x63,
        Umin = 0x64,

        Rand  = 0x70,
        Rseed = 0x71,
        Itf   = 0x72,
        Fti   = 0x73,
        Utf   = 0x74,
        Ftu   = 0x75,
        Ei    = 0x76,
        Di    = 0x77,
        Stop  = 0x78,

        Invalid = 0xff
    };

    /// Returns true if the instruction corresponding to the given opcode changes the program counter
    static inline bool op_changes_p(Opcode op) {
        return (op >= Opcode::Jmp && op <= Opcode::Jle) || op == Opcode::Call || op == Opcode::Ret;
    }

    /// List of tx8 instruction parameter modes
    enum class ParamMode {
        Unused          = 0x0,
        Constant8       = 0x1,
        Constant16      = 0x2,
        Constant32      = 0x3,
        AbsoluteAddress = 0x4,
        RelativeAddress = 0x5,
        Register        = 0x6,
        RegisterAddress = 0x7,

        Label = 0xf, /// Only used by the assembler
    };

    /// Struct representing an instruction parameters
    struct Parameter {
        num32     value;
        ParamMode mode;
    };

    /// Struct holding the two parameters of an instruction for convenience
    struct Parameters {
        Parameter p1;
        Parameter p2;
    };

    /// Struct representing a single instruction with opcode, parameters and a length in bytes (binary length)
    struct Instruction {
        Opcode     opcode;
        Parameters params;
        uint8      len;
    };

    /// Struct representing a label, consisting of a string name, an id and an absolute address
    struct Label {
        std::string name;
        uint32      id;
        uint32      position;
    };

    // clang-format off
    /// Mapping of tx8 opcodes to their respective human readable names
    const std::array<std::string, 256> op_names = {
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
    static inline Opcode opcode_from_name(const std::string& name) {
        for (uint32 i = 0; i < (uint32) Opcode::Invalid; ++i)
            if (op_names[i] == name) return (Opcode) i;

        return Opcode::Invalid;
    }

    enum class Register {
        A       = 0x0,
        B       = 0x1,
        C       = 0x2,
        D       = 0x3,
        R       = 0x4,
        O       = 0x5,
        P       = 0x6,
        S       = 0x7,
        As      = 0x20,
        Bs      = 0x21,
        Cs      = 0x22,
        Ds      = 0x23,
        Rs      = 0x24,
        Os      = 0x25,
        Ps      = 0x26,
        Ss      = 0x27,
        Ab      = 0x10,
        Bb      = 0x11,
        Cb      = 0x12,
        Db      = 0x13,
        Rb      = 0x14,
        Ob      = 0x15,
        Pb      = 0x16,
        Sb      = 0x17,
        Invalid = 0xff
    };

    // Constants for getting different attributes of a register id
    const uint32 REG_SIZE_4    = 0x00u;
    const uint32 REG_SIZE_1    = 0x10u;
    const uint32 REG_SIZE_2    = 0x20u;
    const uint32 REG_SIZE_MASK = 0xf0u;
    const uint32 REG_ID_MASK   = 0x0fu;

    // clang-format off
    /// Mapping of tx8 cpu registers to their respective human readable names
    const std::array<std::string, 256> reg_names = {
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

    /// Convert a human readable tx8 cpu register name to its corresponding enum value
    static inline Register reg_id_from_name(const std::string& name) {
        Register id = Register::Invalid;
        switch (name[0]) {
            case 'A':
            case 'a': id = Register::A; break;
            case 'B':
            case 'b': id = Register::B; break;
            case 'C':
            case 'c': id = Register::C; break;
            case 'D':
            case 'd': id = Register::D; break;
            case 'R':
            case 'r': id = Register::R; break;
            case 'O':
            case 'o': id = Register::O; break;
            case 'S':
            case 's': id = Register::S; break;
            case 'P':
            case 'p': id = Register::P; break;
            default: return Register::Invalid;
        }

        switch (name[1]) {
            case 'S':
            case 's': id = (Register) (((uint32) id) | REG_SIZE_2); break;
            case 'B':
            case 'b': id = (Register) (((uint32) id) | REG_SIZE_1); break;
            default: break;
        }

        return id;
    }

    /// Masks to truncate a tx8 cpu register value according to the register size
    const std::array<uint32, 3> register_mask = {0xffffffff, 0xff, 0xffff};
    /// Get the register size of a tx8 cpu register by its id
    static inline ValueSize register_size(Register reg) {
        switch (((uint32) reg) & REG_SIZE_MASK) {
            case REG_SIZE_1: return ValueSize::Byte;
            case REG_SIZE_2: return ValueSize::Short;
            case REG_SIZE_4: return ValueSize::Word;
            default: return ValueSize::Word;
        }
    }

    /// Get the size of a tx8 instruction parameter in bytes
    static inline ValueSize param_value_size(Parameter param) {
        switch (param.mode) {
            case ParamMode::Constant8: return ValueSize::Byte;
            case ParamMode::Constant16: return ValueSize::Short;
            case ParamMode::Constant32:
            case ParamMode::AbsoluteAddress:
            case ParamMode::RelativeAddress:
            case ParamMode::RegisterAddress: return ValueSize::Word;
            case ParamMode::Register: return register_size((Register) param.value.u);
            default: return ValueSize::Word;
        }
    }

    /// Check if a parameter represents a writable destination by its mode
    static inline bool param_is_writable(ParamMode which) {
        return which == ParamMode::Register || which == ParamMode::AbsoluteAddress
               || which == ParamMode::RelativeAddress || which == ParamMode::RegisterAddress;
    }

    /// Check if a parameter represents some memory address by its mode
    static inline bool param_is_address(ParamMode which) {
        return which == ParamMode::AbsoluteAddress || which == ParamMode::RelativeAddress
               || which == ParamMode::RegisterAddress;
    }

    /// Check if a parameter represents a register by its mode
    static inline bool param_is_register(ParamMode which) { return which == ParamMode::Register; };

    /// Mapping of parameter modes to their byte size in tx8 binary
    const std::array<uint8, 16> param_sizes = {0, 1, 2, 4, 3, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 4};
    /// Masks to truncate a parameter value to its meaningful bytes (indexed by parameter mode)
    const std::array<uint32, 8> param_masks = {0, 0xff, 0xffff, 0xffffffff, 0xffffff, 0xffffff, 0xff, 0xff};
    /// Mask to get the mode of parameter 2 in the first parameter mode byte
    const uint32 PARAM_MODE_2_MASK = 0xfu;

    // clang-format off
    /// Mapping of opcodes to their respective parameter counts
    const std::array<uint8, 256> param_count = {
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
        2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        // 0x7
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    const std::array<uint8, 3> param_mode_bytes = {0, 1, 1};

    /// The maximum length of a binary instruction in bytes
    const uint32 INSTRUCTION_MAX_LENGTH = 0xa;
} // namespace tx

#pragma clang diagnostic warning "-Wunused-function"
