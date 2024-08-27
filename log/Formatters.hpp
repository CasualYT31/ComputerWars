/**
 * \file Formatters.hpp
 * \brief Contains fmt formatters for common datatypes.
 */

#pragma once

#include "spdlog/fmt/std.h"

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
        if (o.empty()) { return format_to(ctx.out(), "{{}}"); }
        format_to(ctx.out(), "{{ ");
        bool first = true;
        for (const auto& e : o) {
            format_to(ctx.out(), "{}{}", (first ? "" : ", "), e);
            first = false;
        }
        return format_to(ctx.out(), " }}");
    }
};
} // namespace fmt
