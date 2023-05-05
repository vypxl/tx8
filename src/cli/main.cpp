#include "tx8/asm/assembler.hpp"
#include "tx8/core/cpu.hpp"
#include "tx8/core/stdlib.hpp"
#include "tx8/core/util.hpp"

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fstream>
#include <iostream>

static tx::Log log_cli;

void cmd_run(const std::string& fname) {
    std::ifstream file(fname, std::ios::in);
    auto          rominfo = tx::parse_header(file);

    tx::Rom rom;

    if (rominfo.has_value()) {
        log_cli("Running {}\n", rominfo.value());
        rom.resize(rominfo.value().size);
        file.read((char*) rom.data(), (long) rom.size());
    } else {
        log_cli("Running source file {}\n", fname);
        file.seekg(0);

        tx::Assembler as(file);
        auto          rom_ = as.generate_binary();
        if (!rom_.has_value()) {
            fmt::println("Assembler encountered an error: \n{}", tx::log_err.get_str());
            exit(1);
        }
        rom = std::move(rom_.value());
    }

    file.close();

    tx::CPU cpu(rom, true);

    tx::stdlib::use_stdlib(cpu);

    cpu.run();
}

void cmd_build(const std::string& srcName, const std::string& destName) {
    std::ifstream src(srcName, std::ios::in);
    std::ofstream dest(destName, std::ios::out | std::ios::binary);
    tx::Assembler as(src);
    as.run();

    tx::RomInfo info {as.get_binary_size(), "Game", "Description"};
    auto        header = tx::build_header(info);

    dest.write((char*) header.data(), (long) header.size());
    as.write_binary(dest);
    log_cli("Wrote {} bytes tx8 binary to {}\n", header.size() + info.size, destName);
}

int main() {
    CLI::App app {"tx8 CLI"};

    app.require_subcommand(1, 1);

    bool debug = false;
    bool quiet = false;
    app.add_flag("-v,--debug", debug, "Verbose debug output");
    app.add_flag("-q,--quiet", quiet, "Quiet mode (no additional output from the cli)");

    app.parse_complete_callback([&]() {
        if (debug) tx::log_debug.init_stream(&std::cerr);
        if (!quiet) log_cli.init_stream(&std::cout);
    });

    auto* run = app.add_subcommand("run", "Run a tx8 file");

    std::string run_src;

    run->add_option("file", run_src, "The tx8 file to run. Can be a source file or a binary file")
        ->required()
        ->check(CLI::ExistingFile);

    run->callback([&]() { cmd_run(run_src); });

    auto*       build = app.add_subcommand("build", "Build a tx8 rom from a source file");
    std::string build_src;
    std::string build_dest = "out.txr";

    build->add_option("source", build_src, "The tx8 source file to build")->required()->check(CLI::ExistingFile);

    build->add_option("destination", build_dest, "The destination file to write the binary to")
        ->default_str("out.txr")
        ->check(CLI::NonexistentPath);

    build->callback([&]() { cmd_build(build_src, build_dest); });

    tx::log.init_stream(&std::cout);
    tx::log_err.init_stream(&std::cerr);

    CLI11_PARSE(app);
    return 0;
}
