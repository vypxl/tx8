#include "tx8/core/log.hpp"
#include "tx8/core/util.hpp"

#include <gtest/gtest.h>
#include <iostream>

using namespace tx;

TEST(Util, Build_and_Parse_Header) {
    tx::log.init_stream(&std::cout);
    tx::log_err.init_stream(&std::cerr);

    RomInfo info;
    info.name        = "Test Rom";
    info.description = "This is a test rom";
    info.size        = 0x1000;

    auto header = build_header(info);

    std::stringstream stream;
    stream.write((char*) header.data(), header.size());

    auto parsed = parse_header(stream);

    tx::log.reset();
    tx::log_err.reset();

    ASSERT_TRUE(parsed.has_value());
    ASSERT_EQ(parsed.value().name, info.name);
    ASSERT_EQ(parsed.value().description, info.description);
    ASSERT_EQ(parsed.value().size, info.size);
}
