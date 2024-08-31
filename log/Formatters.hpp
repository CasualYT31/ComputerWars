/**
 * \file Formatters.hpp
 * \brief Contains fmt formatters for common datatypes.
 */

#pragma once

#include "spdlog/fmt/std.h"

#include <filesystem>
#include <set>

namespace fmt {
/**
 * \brief FMT formatter for std::set.
 * \tparam T Type of object being stored within the set. Must also be FMT-serialisable.
 */
template <typename T> struct formatter<std::set<T>> {
    /**
     * \brief Parses this object's format string.
     * \details This type of object uses the default "{}" format string.
     * \param ctx The format parse context that contains the format string.
     * \returns The beginning of the format parse context.
     */
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    /**
     * \brief Formats the object.
     * \tparam FormatContext Type of the object that receives the formatted object.
     * \param o The object to format.
     * \param ctx The object receiving the formatted object.
     * \returns Internal output stream within the format context object.
     */
    template <typename FormatContext> auto format(const std::set<T>& o, FormatContext& ctx) const -> decltype(ctx.out()) {
        if (o.empty()) { return fmt::format_to(ctx.out(), "{{}}"); }
        fmt::format_to(ctx.out(), "{{ ");
        bool first = true;
        for (const auto& e : o) {
            fmt::format_to(ctx.out(), "{}{}", (first ? "" : ", "), e);
            first = false;
        }
        return fmt::format_to(ctx.out(), " }}");
    }
};

/**
 * \brief FMT formatter for std::filesystem::path.
 */
template <> struct formatter<std::filesystem::path> {
    /**
     * \brief Parses this object's format string.
     * \details This type of object uses the default "{}" format string.
     * \param ctx The format parse context that contains the format string.
     * \returns The beginning of the format parse context.
     */
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    /**
     * \brief Formats the object.
     * \tparam FormatContext Type of the object that receives the formatted object.
     * \param o The object to format.
     * \param ctx The object receiving the formatted object.
     * \returns Internal output stream within the format context object.
     */
    template <typename FormatContext>
    auto format(const std::filesystem::path& o, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", o.generic_string());
    }
};

/**
 * \brief FMT formatter for std::filesystem::directory_entry.
 */
template <> struct formatter<std::filesystem::directory_entry> {
    /**
     * \brief Parses this object's format string.
     * \details This type of object uses the default "{}" format string.
     * \param ctx The format parse context that contains the format string.
     * \returns The beginning of the format parse context.
     */
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    /**
     * \brief Formats the object.
     * \tparam FormatContext Type of the object that receives the formatted object.
     * \param o The object to format.
     * \param ctx The object receiving the formatted object.
     * \returns Internal output stream within the format context object.
     */
    template <typename FormatContext>
    auto format(const std::filesystem::directory_entry& o, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", o.path());
    }
};
} // namespace fmt
