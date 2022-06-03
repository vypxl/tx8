#include "VMTest.hpp"

#include <cmath>

extern int tx_asm_yydebug;

VMTest::VMTest() {
    tx_log_init_str();
    tx_log_init_str_err();

    if (tx_asm_yydebug) {
        tx_log_init_stdout();
        tx_log_init_stderr();
    }
}

VMTest::~VMTest() { tx_log_reset(); }

void VMTest::SetUp() { tx_asm_init_assembler(&as); }

void VMTest::TearDown() {
    tx_asm_destroy_assembler(&as);
    tx_destroy_cpu(&cpu);
    tx_log_clear_str();
    tx_log_clear_str_err();
    nums.clear();
}

void VMTest::append_num(void* vm, tx_num32_variant value) {
    auto* real_vm = (VMTest*)vm;
    real_vm->nums.push_back(value);
}

void VMTest::append_uint(tx_CPU* cpu, void* vm) {
    VMTest::append_num(vm, tx_cpu_top32(cpu));
}

void VMTest::append_int(tx_CPU* cpu, void* vm) {
    tx_num32 v = { .u = tx_cpu_top32(cpu) };
    VMTest::append_num(vm, v.i);
}

void VMTest::append_float(tx_CPU* cpu, void* vm) {
    tx_num32 v = { .u = tx_cpu_top32(cpu) };
    VMTest::append_num(vm, v.f);
}

void VMTest::run_and_compare_str(const std::string& code, const std::string& out, const std::string& err) {
    if (!run_code(code)) return;

    EXPECT_STREQ(tx_log_get_str(), out.c_str());
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());
}

void VMTest::run_and_compare_num(const std::string& code, const std::vector<tx_num32_variant>& expecteds, const std::string& err) {
    if (!run_code(code)) return;
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());

    if (nums.size() != expecteds.size()) {
        ADD_FAILURE() << "Counts of logged (" << nums.size() << ") and expected ("
                      << expecteds.size() << ") values do not match.";
        return;
    }

    for (size_t i = 0; i < nums.size(); i++) {
        auto a = nums[i];
        auto b = expecteds[i];
        if (std::holds_alternative<tx_uint32>(a) || std::holds_alternative<tx_int32>(a)) {
            EXPECT_EQ(a, b);
        } else /* tx_float32 */ {
            tx_float32 x = std::get<tx_float32>(a);
            tx_float32 y = std::get<tx_float32>(b);
            if (std::isnan(x) && std::isnan(y)) SUCCEED();
            else EXPECT_FLOAT_EQ(x, y) << "At index " << i;
        }
    }
}

void VMTest::run_binary() {
    tx_uint32 rom_size = tx_asm_assembler_get_binary_size(&as);
    auto rom = std::vector<tx_uint8>(rom_size);
    tx_asm_assembler_generate_binary(&as, rom.data());

    tx_init_cpu(&cpu, rom.data(), rom_size);
    tx_cpu_use_stdlib(&cpu);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_uint", VMTest::append_uint, this);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_int", VMTest::append_int, this);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_float", VMTest::append_float, this);
    cpu.debug = (bool)tx_asm_yydebug;

    tx_run_cpu(&cpu);
}

bool VMTest::run_code(const std::string& s) {
    int asm_err = tx_asm_run_assembler_buffer(&as, (char*)s.c_str(), (int)s.length());
    if (asm_err != 0) {
        ADD_FAILURE() << "Assembler encountered an error:" << std::endl << tx_log_get_str_err();
        return false;
    }

    if ((bool)tx_asm_yydebug) {
        tx_log_err("labels:\n");
        tx_asm_assembler_print_labels(&as);

        tx_log_err("\ninstructions:\n");
        tx_asm_assembler_print_instructions(&as);
    }

    run_binary();

    return true;
}
