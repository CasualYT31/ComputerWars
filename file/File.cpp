#include "File.hpp"

#include <iterator>

namespace cw {
std::string readEntireTextFile(const std::filesystem::path& file) {
    auto fileStream = makeExceptionFStream<std::ifstream>(file);
    return std::string(std::istreambuf_iterator<char>{ *fileStream }, {});
}

bool iterateDirectory(
    const std::filesystem::path& directory,
    const std::function<bool(const std::filesystem::directory_entry&)>& callback,
    const bool recursive,
    const bool invokeOnDirectories,
    const std::function<void(const std::filesystem::directory_entry&, const std::exception&)> exceptionCallback
) noexcept {
    try {
        if (!std::filesystem::exists(directory)) {
            throw std::runtime_error(fmt::format("The path {} does not exist", directory));
        }
        if (!std::filesystem::is_directory(directory)) { return callback(std::filesystem::directory_entry(directory)); }
        bool result = true;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            try {
                if (entry.is_directory()) {
                    if (invokeOnDirectories) {
                        try {
                            result &= callback(entry);
                        } catch (const std::exception& e) {
                            exceptionCallback(entry, e);
                            result = false;
                        }
                    }
                    if (recursive) {
                        result &=
                            iterateDirectory(entry.path(), callback, recursive, invokeOnDirectories, exceptionCallback);
                    }
                } else {
                    result &= callback(entry);
                }
            } catch (const std::exception& e) {
                exceptionCallback(entry, e);
                result = false;
            }
        }
        return result;
    } catch (const std::exception& e) {
        exceptionCallback(std::filesystem::directory_entry(directory), e);
        return false;
    }
}
} // namespace cw
