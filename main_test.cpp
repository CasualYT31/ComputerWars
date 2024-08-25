#include "log/Log.hpp"

#include <gtest/gtest.h>

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    cw::Log::Setup(std::nullopt, false, true, true);
    cw::Log::SetLevel(cw::Log::Level::trace);
    return RUN_ALL_TESTS();
}
