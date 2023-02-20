#include "tx8/asm/assembler.hpp"
#include "tx8/core/cpu.hpp"
#include "tx8/core/stdlib.hpp"

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fstream>
#include <iostream>

void cmd_run(const std::string& fname, bool debug) {
    fmt::println("Running file: {} (debug: {})", fname, debug);

    tx::log.init_stream(&std::cout);
    tx::log_err.init_stream(&std::cerr);

    std::ifstream file(fname, std::ios::in);
    tx::Assembler as(file);
    as.debug  = debug;
    auto rom_ = as.generate_binary();

    file.close();

    if (!rom_.has_value()) {
        fmt::println("Assembler encountered an error: \n{}", tx::log_err.get_str());
        exit(1);
    }

    auto rom = rom_.value();

    tx::CPU cpu(rom);
    cpu.debug = debug;

    tx::stdlib::use_stdlib(cpu);

    cpu.run();
}

int main() {
    CLI::App app {"tx8 CLI"};

    app.require_subcommand(1, 1);

    bool debug = false;
    app.add_flag("-v,--debug", debug, "Verbose debug output");

    auto* run = app.add_subcommand("run", "Run a tx8 file");

    std::string app_file;

    run->add_option("file", app_file, "The tx8 file to run. Can be a source file or a binary file")
        ->required()
        ->check(CLI::ExistingFile);

    run->callback([&]() { cmd_run(app_file, debug); });

    CLI11_PARSE(app);
    return 0;
}
