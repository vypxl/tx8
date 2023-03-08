#include "tx8/asm/assembler.hpp"
#include "tx8/core/cpu.hpp"
#include "tx8/core/stdlib.hpp"

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fstream>
#include <iostream>

void cmd_run(const std::string& fname, bool debug) {
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

void cmd_build(const std::string& srcName, const std::string& destName, bool debug) {
    std::ifstream src(srcName, std::ios::in);
    std::ofstream dest(destName, std::ios::out | std::ios::binary);
    tx::Assembler as(src);
    as.debug = debug;
    as.write_binary(dest);
    fmt::println("Wrote {} bytes tx8 binary to {}", as.get_binary_size(), destName);
}

int main() {
    CLI::App app {"tx8 CLI"};

    app.require_subcommand(1, 1);

    bool debug = false;
    app.add_flag("-v,--debug", debug, "Verbose debug output");

    auto* run = app.add_subcommand("run", "Run a tx8 file");

    std::string run_src;

    run->add_option("file", run_src, "The tx8 file to run. Can be a source file or a binary file")
        ->required()
        ->check(CLI::ExistingFile);

    run->callback([&]() { cmd_run(run_src, debug); });

    auto*       build = app.add_subcommand("build", "Build a tx8 rom from a source file");
    std::string build_src;
    std::string build_dest = "out.txr";

    build->add_option("source", build_src, "The tx8 source file to build")->required()->check(CLI::ExistingFile);

    build->add_option("destination", build_dest, "The destination file to write the binary to")
        ->default_str("out.txr")
        ->check(CLI::NonexistentPath);

    build->callback([&]() { cmd_build(build_src, build_dest, debug); });

    tx::log.init_stream(&std::cout);
    tx::log_err.init_stream(&std::cerr);

    CLI11_PARSE(app);
    return 0;
}
