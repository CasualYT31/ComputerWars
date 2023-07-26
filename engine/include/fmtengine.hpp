/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**@file fmtengine.hpp
 * Defines fmt formatters for custom types available in the \c engine module.
 * <a href="https://fmt.dev/latest/api.html#format-api" target="_blank">Code taken
 * from the fmt documentation.</a>
 */

#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"
#include "angelscript.h"
#include "logger.hpp"

/**
 * Fmt formatter for the \c sf::Vector2 types.
 */
template <typename T> struct fmt::formatter<sf::Vector2<T>> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const sf::Vector2<T>& p, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({}, {})", p.x, p.y);
	}
};

/**
 * Fmt formatter for the \c sf::Color type.
 */
template <> struct fmt::formatter<sf::Color> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const sf::Color& p, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "[{}, {}, {}, {}]", p.r, p.g, p.b, p.a);
	}
};

/**
 * Fmt formatter for the \c asSMessageInfo type.
 */
template <> struct fmt::formatter<asSMessageInfo> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const asSMessageInfo& m, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "(@{}:{},{}): {}", m.section, m.row,
			m.col, m.message);
	}
};
