/**
 * \file File.hpp
 * \brief Contains file stream helper functions.
 */

#pragma once

#include "log/Log.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

namespace cw {
/**
 * \brief Constructs a file stream that throws exceptions if either the bad or fail bit are set.
 * \tparam T The type of file stream to construct.
 * \tparam Params The types of parameters to pass to the constructor of T.
 * \param constructorParams The parameters to pass to the constructor of T.
 * \returns Pointer to the file stream object. Guaranteed to be not nullptr.
 * \throws std::ios_base::failure if the file stream couldn't be opened.
 */
template <typename T, typename... Params> std::unique_ptr<T> makeExceptionFStream(Params... constructorParams) {
    std::unique_ptr<T> file = std::make_unique<T>(constructorParams...);
    if (!file || !file->good()) { throw std::ios_base::failure("File couldn't be opened"); }
    file->exceptions(std::ios_base::failbit | std::ios_base::badbit);
    return file;
}

/**
 * \brief Reads the entire contents of a text file.
 * \param file The path to the file.
 * \returns The contents of the text file.
 * \throws std::ios_base::failure if the file could not be read.
 */
std::string readEntireTextFile(const std::filesystem::path& file);

/**
 * \brief Iterates over a directory, invoking the given callback for each file [and directory] entry found.
 * \param directory The directory to search through.
 * \param callback The callback to invoke for each entry. It should return true if the callback succeeded, false otherwise.
 * \param recursive True if the directory should be searched recursively, false if not.
 * \param invokeOnDirectories True if the callback should be invoked on directories, too. If recursive is true, the callback
 * will be invoked before the recursive search is made.
 * \param exceptionCallback The callback to invoke when interacting with a directory entry throws an exception. Is also
 * invoked when the callback throws an exception.
 * \returns True if every callback invocation returned true, false if even one invocation returned false and/or even one
 * entry caused an exception. True is also returned if no entries are found. If the given entry was a file, the callback will
 * be invoked once, and the file will be provided as an entry.
 */
bool iterateDirectory(
    const std::filesystem::path& directory,
    const std::function<bool(const std::filesystem::directory_entry&)>& callback,
    const bool recursive = true,
    const bool invokeOnDirectories = false,
    const std::function<void(const std::filesystem::directory_entry&, const std::exception&)> exceptionCallback =
        [](const std::filesystem::directory_entry& entry, const std::exception& e) {
            LOG(err, "Encountered an error when interacting with the directory entry {}: {}", entry, e);
        }
) noexcept;
} // namespace cw
