#include "log/Log.hpp"

#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

int main(int argc, char* argv[]) {
    testing::InitGoogleMock(&argc, argv);
    std::filesystem::current_path("test-files");
    std::filesystem::create_directory("tmp");
    cw::Log::Setup(std::nullopt, false, true, true);
    cw::Log::SetLevel(cw::Log::Level::trace);
    return RUN_ALL_TESTS();
}
