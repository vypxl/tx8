#include <gtest/gtest.h>

namespace tx::testing {
    bool enable_debug = false;
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
