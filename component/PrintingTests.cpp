#include "Printing.hpp"

#include "gtest/gtest.h"
#include <filesystem>

TEST(PrintingTest, Add) {
    EXPECT_EQ(5, cw::add(2, 3));
    EXPECT_NE(-1, cw::add(1, 1));
}

TEST(AssetsFolder, SymLinkTest) { EXPECT_TRUE(std::filesystem::exists("assets")); }
