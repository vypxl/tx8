#include "VMTest.hpp"

#include <cmath>

extern int tx_asm_yydebug;

VMTest::VMTest() {
    tx::log.init_str();
    tx::log_err.init_str();

    if (tx_asm_yydebug) {
        tx::log.init_stream(&std::cout);
        tx::log_err.init_stream(&std::cerr);
    }
}

VMTest::~VMTest() {
    tx::log.reset();
    tx::log_err.reset();
}

void VMTest::SetUp() { }

void VMTest::TearDown() {
    tx_destroy_cpu(&cpu);
    tx::log.clear_str();
    tx::log_err.clear_str();
    nums.clear();
}

void VMTest::append_num(void* vm, tx_num32_variant value) {
    auto* real_vm = (VMTest*) vm;
    real_vm->nums.push_back(value);
}

void VMTest::test_uint(tx_CPU* cpu, void* vm) { VMTest::append_num(vm, tx_cpu_top32(cpu)); }

void VMTest::test_int(tx_CPU* cpu, void* vm) {
    tx_num32 v = {.u = tx_cpu_top32(cpu)};
    VMTest::append_num(vm, v.i);
}

void VMTest::test_float(tx_CPU* cpu, void* vm) {
    tx_num32 v = {.u = tx_cpu_top32(cpu)};
    VMTest::append_num(vm, v.f);
}

void VMTest::test_au(tx_CPU* cpu, void* vm) { VMTest::append_num(vm, cpu->a); }

void VMTest::test_ai(tx_CPU* cpu, void* vm) {
    tx_num32 v = {.u = cpu->a};
    VMTest::append_num(vm, v.i);
}

void VMTest::test_af(tx_CPU* cpu, void* vm) {
    tx_num32 v = {.u = cpu->a};
    VMTest::append_num(vm, v.f);
}

void VMTest::test_r(tx_CPU* cpu, void* vm) { VMTest::append_num(vm, cpu->r); }

void VMTest::test_rf(tx_CPU* cpu, void* vm) {
    tx_num32 v = {.u = cpu->r};
    VMTest::append_num(vm, v.f);
}

void VMTest::run_and_compare_str(const std::string& code, const std::string& out, const std::string& err) {
    if (!run_code(code)) return;

    EXPECT_EQ(tx::log.get_str(), out);
    ASSERT_EQ(tx::log_err.get_str(), err);
}

void VMTest::run_and_compare_num(
    const std::string&                   code,
    const std::vector<tx_num32_variant>& expecteds,
    const std::string&                   err
) {
    if (!run_code(code)) return;
    ASSERT_EQ(tx::log_err.get_str(), err);

    if (nums.size() != expecteds.size()) {
        ADD_FAILURE() << "Counts of logged (" << nums.size() << ") and expected (" << expecteds.size()
                      << ") values do not match.";
        return;
    }

    for (size_t i = 0; i < nums.size(); i++) {
        auto a = nums[i];
        auto b = expecteds[i];
        if (std::holds_alternative<tx_uint32>(b) || std::holds_alternative<tx_int32>(b)) {
            if (a != b) {
                if (std::holds_alternative<tx_uint32>(b))
                    ADD_FAILURE() << "Value 0x" << std::hex << std::get<tx_uint32>(a) << " at index " << std::dec << i
                                  << " does not match expected value 0x" << std::hex << std::get<tx_uint32>(b) << ".";
                else
                    ADD_FAILURE() << "Value " << std::get<tx_int32>(a) << " at index " << std::dec << i
                                  << " does not match expected value " << std::get<tx_int32>(b) << ".";
            } else {
                SUCCEED();
            }
        } else /* tx_float32 */ {
            tx_float32 actual   = std::get<tx_float32>(a);
            tx_float32 expected = std::get<tx_float32>(b);
            if (std::isnan(actual) && std::isnan(expected)) SUCCEED();
            else EXPECT_FLOAT_EQ(actual, expected) << "At index " << i;
        }
    }
}

bool VMTest::run_code(const std::string& s) {
    tx::Assembler as(s);
    auto          rom_ = as.generate_binary();
    if (!rom_.has_value()) {
        ADD_FAILURE() << "Assembler encountered an error:" << std::endl << tx::log_err.get_str();
        return false;
    }

    auto rom = rom_.value();

    if ((bool) tx_asm_yydebug) {
        tx::log_err("labels:\n");
        as.print_labels();

        tx::log_err("\ninstructions:\n");
        as.print_instructions();
    }

    tx_init_cpu(&cpu, rom.data(), rom.size());
    tx_cpu_use_stdlib(&cpu);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_uint", VMTest::test_uint, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_int", VMTest::test_int, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_float", VMTest::test_float, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_au", VMTest::test_au, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_ai", VMTest::test_ai, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_af", VMTest::test_af, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_r", VMTest::test_r, this);
    tx_cpu_register_sysfunc(&cpu, (char*) "test_rf", VMTest::test_rf, this);
    cpu.debug = (bool) tx_asm_yydebug;

    tx_run_cpu(&cpu);

    return true;
}
