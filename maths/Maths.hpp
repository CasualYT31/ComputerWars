/**
 * \file Maths.hpp
 * \brief Contains maths helper functions.
 */

#pragma once

#include <type_traits>
#include <unordered_set>

namespace cw {
/**
 * Hashes a container of hashable, homogeneous values.
 * \tparam T The type of values stored within the container.
 * \tparam C The type of container to iterate through.
 * \param vals A container with \c begin() and \c end() implementations that let you iterate through a collection of values
 * of type \c T.
 * \returns The final seed.
 */
template <typename T, typename C> std::size_t combinationHasher(const C& vals) {
    std::hash<T> hasher;
    std::size_t seed = 0;
    for (auto itr = vals.begin(), end = vals.end(); itr != end; ++itr)
        seed ^= hasher(*itr) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}
} // namespace cw

namespace std {
/**
 * \brief Hash implementation for std::unordered_set.
 */
template <typename T> struct hash<std::unordered_set<T>> {
    /**
     * \brief Hashes an std::unordered_set.
     * \param s The set to hash.
     * \returns The hash of the given set.
     */
    std::size_t operator()(const std::unordered_set<T>& s) const {
        return cw::combinationHasher<T>(s);
    }
};
} // namespace std
