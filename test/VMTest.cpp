#include "VMTest.hpp"

#include <cmath>
#include <tx8/core/stdlib.hpp>

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
    tx::log.clear_str();
    tx::log_err.clear_str();
    nums.clear();
}

void VMTest::append_num(tx::num32_variant value) { nums.push_back(value); }

void VMTest::run_and_compare_str(const std::string& code, const std::string& out, const std::string& err) {
    if (!run_code(code)) return;

    EXPECT_EQ(tx::log.get_str(), out);
    ASSERT_EQ(tx::log_err.get_str(), err);
}

void VMTest::run_and_compare_num(
    const std::string&                    code,
    const std::vector<tx::num32_variant>& expecteds,
    const std::string&                    err
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
        if (std::holds_alternative<tx::uint32>(b) || std::holds_alternative<tx::int32>(b)) {
            if (a != b) {
                if (std::holds_alternative<tx::uint32>(b))
                    ADD_FAILURE() << "Value 0x" << std::hex << std::get<tx::uint32>(a) << " at index " << std::dec << i
                                  << " does not match expected value 0x" << std::hex << std::get<tx::uint32>(b) << ".";
                else
                    ADD_FAILURE() << "Value " << std::get<tx::int32>(a) << " at index " << std::dec << i
                                  << " does not match expected value " << std::get<tx::int32>(b) << ".";
            } else {
                SUCCEED();
            }
        } else /* tx::float32 */ {
            tx::float32 actual   = std::get<tx::float32>(a);
            tx::float32 expected = std::get<tx::float32>(b);
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

    tx::CPU cpu(rom);
    cpu.debug = (bool) tx_asm_yydebug;

    tx::stdlib::use_stdlib(cpu);
    use_testing_stdlib(cpu);

    cpu.run();

    return true;
}

void VMTest::use_testing_stdlib(tx::CPU& cpu) {
    cpu.register_sysfunc("test_uint", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.top()};
        append_num(v.u);
    });

    cpu.register_sysfunc("test_int", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.top()};
        append_num(v.i);
    });

    cpu.register_sysfunc("test_float", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.top()};
        append_num(v.f);
    });

    cpu.register_sysfunc("test_au", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.a};
        append_num(v.u);
    });

    cpu.register_sysfunc("test_ai", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.a};
        append_num(v.i);
    });

    cpu.register_sysfunc("test_af", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.a};
        append_num(v.f);
    });

    cpu.register_sysfunc("test_r", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.r};
        append_num(v.u);
    });

    cpu.register_sysfunc("test_ri", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.r};
        append_num(v.i);
    });


    cpu.register_sysfunc("test_rf", [this](tx::CPU& cpu) {
        tx::num32 v = {.u = cpu.r};
        append_num(v.f);
    });
}
