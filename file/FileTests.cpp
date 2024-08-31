#include "File.hpp"

#include <gmock/gmock.h>
#include <streambuf>
#include <unordered_set>

TEST(FileTests, MakeExceptionFstreamOpenFailureTest) {
    EXPECT_THROW(cw::makeExceptionFStream<std::ifstream>("non-existent-file"), std::ios_base::failure);
}

TEST(FileTests, MakeExceptionFstreamReadFailAndEofBitTest) {
    const auto file = cw::makeExceptionFStream<std::ifstream>("FileTests_InputFile.txt");
    EXPECT_TRUE(file);
    std::string buffer;
    EXPECT_NO_THROW(std::getline(*file, buffer));
    EXPECT_TRUE(file->good());
    EXPECT_EQ(buffer, "-1000000 Hello, World.");
    EXPECT_THROW(std::getline(*file, buffer), std::ios_base::failure);
    EXPECT_TRUE(file->fail());
    EXPECT_TRUE(file->eof());
}

TEST(FileTests, MakeExceptionFstreamReadFailBitTest) {
    const auto file = cw::makeExceptionFStream<std::ifstream>("FileTests_InputFile.txt");
    EXPECT_TRUE(file);
    short value = 0;
    EXPECT_THROW(*file >> value, std::ios_base::failure);
    EXPECT_TRUE(file->fail());
    EXPECT_FALSE(file->eof());
}

TEST(FileTests, MakeExceptionFstreamWriteBadBitTest) {
    const auto file = cw::makeExceptionFStream<std::ofstream>("tmp/FileTests_OutputFile.txt");
    EXPECT_TRUE(file);
    std::streambuf* const buffer = nullptr;
    EXPECT_THROW(*file << buffer, std::ios_base::failure);
    EXPECT_TRUE(file->bad());
}

TEST(FileTests, ReadEntireTextFileSuccess) {
    const auto contents = cw::readEntireTextFile("FileTests_InputFile.txt");
    EXPECT_EQ(contents, "-1000000 Hello, World.\n");
}

TEST(FileTests, ReadEntireTextFileFailure) {
    EXPECT_THROW(cw::readEntireTextFile("non-existent"), std::ios_base::failure);
}

class IterateDirectoryTests : public testing::Test {
protected:
    const std::function<bool(const std::filesystem::directory_entry&)> defaultCallback =
        [](const std::filesystem::directory_entry&) { return true; };

    const std::function<void(const std::filesystem::directory_entry&, const std::exception&)> defaultExceptionCallback =
        [](const std::filesystem::directory_entry&, const std::exception&) {};

    inline bool iterateDirectory(
        const std::filesystem::path& directory,
        const bool recursive,
        const bool invokeOnDirectories,
        const std::function<bool(const std::filesystem::directory_entry&)>& callback,
        const std::function<void(const std::filesystem::directory_entry&, const std::exception&)> exceptionCallback
    ) {
        return cw::iterateDirectory(
            directory,
            [&counter = _callbackCount, &callback](const std::filesystem::directory_entry& entry) {
                ++counter;
                return callback(entry);
            },
            recursive,
            invokeOnDirectories,
            [&counter = _exceptionCount,
             &exceptionCallback](const std::filesystem::directory_entry& entry, const std::exception& e) {
                ++counter;
                exceptionCallback(entry, e);
            }
        );
    }

    inline std::size_t callbackCount() const {
        return _callbackCount;
    }

    inline std::size_t exceptionCount() const {
        return _exceptionCount;
    }

private:
    std::size_t _callbackCount = 0;
    std::size_t _exceptionCount = 0;
};

TEST_F(IterateDirectoryTests, NonExistentFailure) {
    EXPECT_FALSE(iterateDirectory(
        "non-existent",
        false,
        false,
        defaultCallback,
        [](const std::filesystem::directory_entry& entry, const std::exception& e) {
            EXPECT_EQ(entry, std::filesystem::directory_entry("non-existent"));
            EXPECT_TRUE(std::string(e.what()).find("does not exist") != std::string::npos);
        }
    ));
    EXPECT_EQ(callbackCount(), 0);
    EXPECT_EQ(exceptionCount(), 1);
}

TEST_F(IterateDirectoryTests, SingleFile) {
    EXPECT_TRUE(iterateDirectory(
        "IterateDirectoryTests/IterateDirectoryTests_File.txt",
        false,
        false,
        [](const std::filesystem::directory_entry& entry) {
            EXPECT_EQ(entry, std::filesystem::directory_entry("IterateDirectoryTests/IterateDirectoryTests_File.txt"));
            return true;
        },
        defaultExceptionCallback
    ));
    EXPECT_EQ(callbackCount(), 1);
    EXPECT_EQ(exceptionCount(), 0);
}

TEST_F(IterateDirectoryTests, DirectoryNonRecursive) {
    std::unordered_set<std::filesystem::path> expectedFiles = { "IterateDirectoryTests_File.txt",
                                                                "IterateDirectoryTests_File2.json" };
    const auto expectedCount = expectedFiles.size();
    EXPECT_TRUE(iterateDirectory(
        "IterateDirectoryTests",
        false,
        false,
        [&expectedFiles](const std::filesystem::directory_entry& entry) {
            EXPECT_EQ(expectedFiles.erase(entry.path().filename()), 1);
            return true;
        },
        defaultExceptionCallback
    ));
    EXPECT_TRUE(expectedFiles.empty());
    EXPECT_EQ(callbackCount(), expectedCount);
    EXPECT_EQ(exceptionCount(), 0);
}

TEST_F(IterateDirectoryTests, DirectoryRecursive) {
    std::unordered_set<std::filesystem::path> expectedFiles = { "IterateDirectoryTests_File.txt",
                                                                "IterateDirectoryTests_File2.json",
                                                                "IterateDirectoryTests_File3.txt",
                                                                "IterateDirectoryTests_File4.txt" };
    const auto expectedCount = expectedFiles.size();
    EXPECT_TRUE(iterateDirectory(
        "IterateDirectoryTests",
        true,
        false,
        [&expectedFiles](const std::filesystem::directory_entry& entry) {
            EXPECT_EQ(expectedFiles.erase(entry.path().filename()), 1);
            return true;
        },
        defaultExceptionCallback
    ));
    EXPECT_TRUE(expectedFiles.empty());
    EXPECT_EQ(callbackCount(), expectedCount);
    EXPECT_EQ(exceptionCount(), 0);
}

TEST_F(IterateDirectoryTests, DirectoryNonRecursiveIncludingFolders) {
    std::unordered_set<std::filesystem::path> expectedFiles = { "IterateDirectoryTests_File.txt",
                                                                "IterateDirectoryTests_File2.json",
                                                                "InnerDirectory" };
    const auto expectedCount = expectedFiles.size();
    EXPECT_TRUE(iterateDirectory(
        "IterateDirectoryTests",
        false,
        true,
        [&expectedFiles](const std::filesystem::directory_entry& entry) {
            EXPECT_EQ(expectedFiles.erase(entry.path().filename()), 1);
            return true;
        },
        defaultExceptionCallback
    ));
    EXPECT_TRUE(expectedFiles.empty());
    EXPECT_EQ(callbackCount(), expectedCount);
    EXPECT_EQ(exceptionCount(), 0);
}

TEST_F(IterateDirectoryTests, DirectoryRecursiveIncludingFolders) {
    std::unordered_set<std::filesystem::path> expectedFiles = { "IterateDirectoryTests_File.txt",
                                                                "IterateDirectoryTests_File2.json",
                                                                "IterateDirectoryTests_File3.txt",
                                                                "IterateDirectoryTests_File4.txt",
                                                                "InnerDirectory",
                                                                "InnerDirectory2" };
    const auto expectedCount = expectedFiles.size();
    bool seenInnerDir = false, seenInnerDir2 = false;
    EXPECT_TRUE(iterateDirectory(
        "IterateDirectoryTests",
        true,
        true,
        [&expectedFiles, &seenInnerDir, &seenInnerDir2](const std::filesystem::directory_entry& entry) {
            if (entry.path().filename() == "IterateDirectoryTests_File3.txt") {
                EXPECT_TRUE(seenInnerDir);
            } else if (entry.path().filename() == "IterateDirectoryTests_File4.txt") {
                EXPECT_TRUE(seenInnerDir && seenInnerDir2);
            }
            EXPECT_EQ(expectedFiles.erase(entry.path().filename()), 1);
            if (entry.path().filename() == "InnerDirectory") { seenInnerDir = true; }
            if (entry.path().filename() == "InnerDirectory2") { seenInnerDir2 = true; }
            return true;
        },
        defaultExceptionCallback
    ));
    EXPECT_TRUE(expectedFiles.empty());
    EXPECT_EQ(callbackCount(), expectedCount);
    EXPECT_EQ(exceptionCount(), 0);
}

TEST_F(IterateDirectoryTests, DirectoryRecursiveIncludingFoldersCatchExceptions) {
    std::unordered_set<std::filesystem::path> expectedFiles = { "IterateDirectoryTests_File.txt",
                                                                "IterateDirectoryTests_File2.json",
                                                                "IterateDirectoryTests_File3.txt",
                                                                "IterateDirectoryTests_File4.txt",
                                                                "InnerDirectory",
                                                                "InnerDirectory2" };
    const auto expectedCount = expectedFiles.size();
    bool seenInnerDir = false, seenInnerDir2 = false;
    EXPECT_FALSE(iterateDirectory(
        "IterateDirectoryTests",
        true,
        true,
        [&expectedFiles, &seenInnerDir, &seenInnerDir2](const std::filesystem::directory_entry& entry) {
            if (entry.path().filename() == "IterateDirectoryTests_File3.txt") {
                EXPECT_TRUE(seenInnerDir);
            } else if (entry.path().filename() == "IterateDirectoryTests_File4.txt") {
                EXPECT_TRUE(seenInnerDir && seenInnerDir2);
            }
            EXPECT_EQ(expectedFiles.erase(entry.path().filename()), 1);
            if (entry.path().filename() == "InnerDirectory") { seenInnerDir = true; }
            if (entry.path().filename() == "InnerDirectory2") { seenInnerDir2 = true; }
            throw std::runtime_error("User error");
            return true;
        },
        [](const std::filesystem::directory_entry&, const std::exception& e) {
            EXPECT_EQ(std::string(e.what()), "User error");
        }
    ));
    EXPECT_TRUE(expectedFiles.empty());
    EXPECT_EQ(callbackCount(), expectedCount);
    EXPECT_EQ(exceptionCount(), expectedCount);
}
