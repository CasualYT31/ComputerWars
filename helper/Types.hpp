/**
 * \file Types.hpp
 * \brief Contains type-related helper templates.
 */

#pragma once

#include <string_view>
#include <type_traits>

namespace cw {
/**
 * \brief Determines if an [in] parameter should be const or const&.
 * \details Thanks go to https://stackoverflow.com/a/18365506. I'll never be able to write template stuff on my own.
 * \tparam T The type to test.
 */
template <
    typename T,
    bool =
        std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_pointer_v<T> || std::is_same_v<T, std::string_view>>
struct Param {};

/**
 * \brief If an [in] parameter should be const, this structure will be chosen.
 * \tparam T The type to test.
 */
template <typename T> struct Param<T, true> {
    /**
     * \brief Only a few types are best passed without const&, so if the type reports true for the given type, pass as const.
     */
    using In = const T;
};

/**
 * \brief If an [in] parameter should be const&, this structure will be chosen.
 * \tparam T The type to test.
 */
template <typename T> struct Param<T, false> {
    /**
     * \brief Most types are best passed as const&.
     */
    using In = const T&;
};
} // namespace cw
