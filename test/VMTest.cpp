#include "VMTest.hpp"

extern int tx_asm_yydebug;

VMTest::VMTest() {
    tx_log_init_str();
    tx_log_init_str_err();
}

VMTest::~VMTest() { tx_log_reset(); }

void VMTest::SetUp() { tx_asm_init_assembler(&as); }

void VMTest::TearDown() {
    tx_asm_destroy_assembler(&as);
    tx_destroy_cpu(&cpu);
    tx_log_clear_str();
    tx_log_clear_str_err();
    floats.clear();
}

void VMTest::run_and_compare(std::string code, const std::string out, const std::string err) {
    if (!run_code(code)) return;

    EXPECT_STREQ(tx_log_get_str(), out.c_str());
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());
}

void VMTest::run_and_compare_f(std::string code, std::vector<tx_float32> expected, std::string err) {
    if (!run_code(code)) return;
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());

    if (floats.size() != expected.size()) {
        ADD_FAILURE() << "Counts of logged (" << floats.size() << ") and expected ("
                      << expected.size() << ") floats do not match.";
        return;
    }

    auto result = floats;
    for (size_t i = 0; i < floats.size(); i++)
        EXPECT_FLOAT_EQ(result[i], expected[i]) << "At index " << i;
}

void VMTest::append_float(tx_CPU* cpu, void* vm) {
    VMTest*  real_vm = (VMTest*)vm;
    tx_num32 val;
    val.u = tx_cpu_top32(cpu);
    real_vm->floats.push_back(val.f);
}

void VMTest::run_binary() {
    tx_uint32 rom_size;
    tx_uint8* rom = tx_asm_assembler_generate_binary(&as, &rom_size);

    tx_init_cpu(&cpu, rom, rom_size);
    tx_cpu_use_stdlib(&cpu);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_float", VMTest::append_float, this);
    cpu.debug = (bool) tx_asm_yydebug;
    free(rom);

    tx_run_cpu(&cpu);
}

bool VMTest::run_code(const std::string s) {
    int asm_err = tx_asm_run_assembler_buffer(&as, (char*)s.c_str(), (int)s.length());
    if (asm_err != 0) {
        ADD_FAILURE() << "Assembler encountered an error:" << std::endl << tx_log_get_str_err();
        return false;
    }

    if ((bool) tx_asm_yydebug) {
        printf("labels:\n");
        tx_asm_assembler_print_labels(&as);

        printf("\ninstructions:\n");
        tx_asm_assembler_print_instructions(&as);
    }

    run_binary();

    return true;
}
