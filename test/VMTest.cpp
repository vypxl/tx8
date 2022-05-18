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
    nums.clear();
}

void VMTest::append_num(void* vm, tx_uint32 value, tx_NumType type) {
    VMTest*            real_vm = (VMTest*)vm;
    tx_num32_with_type val;
    val.num.u = value;
    val.type  = type;
    real_vm->nums.push_back(val);
}

void VMTest::append_uint(tx_CPU* cpu, void* vm) {
    VMTest::append_num(vm, tx_cpu_top32(cpu), tx_NumType::TX_NUM_UINT32);
}

void VMTest::append_int(tx_CPU* cpu, void* vm) {
    VMTest::append_num(vm, tx_cpu_top32(cpu), tx_NumType::TX_NUM_INT32);
}

void VMTest::append_float(tx_CPU* cpu, void* vm) {
    VMTest::append_num(vm, tx_cpu_top32(cpu), tx_NumType::TX_NUM_FLOAT32);
}

void VMTest::run_and_compare_str(std::string code, const std::string out, const std::string err) {
    if (!run_code(code)) return;

    EXPECT_STREQ(tx_log_get_str(), out.c_str());
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());
}

void VMTest::run_and_compare_num(std::string             code,
                               std::vector<std::variant<tx_uint32, tx_int32, tx_float32>> expecteds,
                               std::string             err) {
    if (!run_code(code)) return;
    ASSERT_STREQ(tx_log_get_str_err(), err.c_str());

    if (nums.size() != expecteds.size()) {
        ADD_FAILURE() << "Counts of logged (" << nums.size() << ") and expected ("
                      << expecteds.size() << ") values do not match.";
        return;
    }

    for (size_t i = 0; i < nums.size(); i++)
        if (nums[i].type == tx_NumType::TX_NUM_UINT32) {
            tx_uint32 result = nums[i].num.u;
            tx_uint32 expected = std::get<tx_uint32>(expecteds[i]);
            EXPECT_EQ(result, expected) << "At index " << i;
        } else if (nums[i].type == tx_NumType::TX_NUM_INT32) {
            tx_int32 result = nums[i].num.i;
            tx_int32 expected = std::get<tx_int32>(expecteds[i]);
            EXPECT_EQ(result, expected) << "At index " << i;
        } else if (nums[i].type == tx_NumType::TX_NUM_FLOAT32) {
            tx_float32 result = nums[i].num.f;
            tx_float32 expected = std::get<tx_float32>(expecteds[i]);
            EXPECT_FLOAT_EQ(result, expected) << "At index " << i;
        }
}

void VMTest::run_binary() {
    tx_uint32 rom_size;
    tx_uint8* rom = tx_asm_assembler_generate_binary(&as, &rom_size);

    tx_init_cpu(&cpu, rom, rom_size);
    tx_cpu_use_stdlib(&cpu);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_uint", VMTest::append_uint, this);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_int", VMTest::append_int, this);
    tx_cpu_register_sysfunc(&cpu, (char*)"test_float", VMTest::append_float, this);
    cpu.debug = (bool)tx_asm_yydebug;
    free(rom);

    tx_run_cpu(&cpu);
}

bool VMTest::run_code(const std::string s) {
    int asm_err = tx_asm_run_assembler_buffer(&as, (char*)s.c_str(), (int)s.length());
    if (asm_err != 0) {
        ADD_FAILURE() << "Assembler encountered an error:" << std::endl << tx_log_get_str_err();
        return false;
    }

    if ((bool)tx_asm_yydebug) {
        printf("labels:\n");
        tx_asm_assembler_print_labels(&as);

        printf("\ninstructions:\n");
        tx_asm_assembler_print_instructions(&as);
    }

    run_binary();

    return true;
}
