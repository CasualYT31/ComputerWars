/**
 * \file File.hpp
 * \brief Contains file stream helper functions.
 */

#pragma once

#include <fstream>
#include <memory>

namespace cw {
/**
 * \brief Constructs a file stream that throws exceptions if either the bad or fail bit are set.
 * \tparam T The type of file stream to construct.
 * \tparam Params The types of parameters to pass to the constructor of T.
 * \param constructorParams The parameters to pass to the constructor of T.
 * \returns Pointer to the file stream object.
 * \throws std::ios_base::failure if the file stream couldn't be opened.
 */
template <typename T, typename... Params> std::unique_ptr<T> makeExceptionFStream(Params... constructorParams) {
    std::unique_ptr<T> file = std::make_unique<T>(constructorParams...);
    if (!file->good()) { throw std::ios_base::failure("File couldn't be opened"); }
    file->exceptions(std::ios_base::failbit | std::ios_base::badbit);
    return file;
}
} // namespace cw
