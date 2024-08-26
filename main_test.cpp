#include "log/Log.hpp"

#include <filesystem>
#include <gtest/gtest.h>

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    std::filesystem::create_directory("test-files/tmp");
    cw::Log::Setup(std::nullopt, false, true, true);
    cw::Log::SetLevel(cw::Log::Level::trace);
    return RUN_ALL_TESTS();
}
