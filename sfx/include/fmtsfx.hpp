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

/**
 * @file fmtsfx.hpp
 * Defines fmt formatters for custom types available in the \c sfx module.
 */

#pragma once

#include "fmtengine.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/Scrollbar.hpp"
#include "TGUI/Widgets/Grid.hpp"

/**
 * Fmt formatter for the \c tgui::String type.
 */
template <> struct fmt::formatter<tgui::String> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::String& s, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "{}", s.toStdString());
	}
};

/**
 * Fmt formatter for the \c tgui::AutoLayout type.
 */
template <> struct fmt::formatter<tgui::AutoLayout> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::AutoLayout a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::AutoLayout::Manual:
			return fmt::format_to(ctx.out(), "Manual ({})", aValue);
		case tgui::AutoLayout::Top:
			return fmt::format_to(ctx.out(), "Top ({})", aValue);
		case tgui::AutoLayout::Left:
			return fmt::format_to(ctx.out(), "Left ({})", aValue);
		case tgui::AutoLayout::Right:
			return fmt::format_to(ctx.out(), "Right ({})", aValue);
		case tgui::AutoLayout::Bottom:
			return fmt::format_to(ctx.out(), "Bottom ({})", aValue);
		case tgui::AutoLayout::Leftmost:
			return fmt::format_to(ctx.out(), "Leftmost ({})", aValue);
		case tgui::AutoLayout::Rightmost:
			return fmt::format_to(ctx.out(), "Rightmost ({})", aValue);
		case tgui::AutoLayout::Fill:
			return fmt::format_to(ctx.out(), "Fill ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};

/**
 * Fmt formatter for the \c tgui::Label::HorizontalAlignment type.
 */
template <> struct fmt::formatter<tgui::Label::HorizontalAlignment> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::Label::HorizontalAlignment a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::Label::HorizontalAlignment::Left:
			return fmt::format_to(ctx.out(), "Left ({})", aValue);
		case tgui::Label::HorizontalAlignment::Center:
			return fmt::format_to(ctx.out(), "Center ({})", aValue);
		case tgui::Label::HorizontalAlignment::Right:
			return fmt::format_to(ctx.out(), "Right ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};

/**
 * Fmt formatter for the \c tgui::Label::VerticalAlignment type.
 */
template <> struct fmt::formatter<tgui::Label::VerticalAlignment> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::Label::VerticalAlignment a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::Label::VerticalAlignment::Top:
			return fmt::format_to(ctx.out(), "Top ({})", aValue);
		case tgui::Label::VerticalAlignment::Center:
			return fmt::format_to(ctx.out(), "Center ({})", aValue);
		case tgui::Label::VerticalAlignment::Bottom:
			return fmt::format_to(ctx.out(), "Bottom ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};

/**
 * Fmt formatter for the \c tgui::EditBox::Alignment type.
 */
template <> struct fmt::formatter<tgui::EditBox::Alignment> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::EditBox::Alignment a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::EditBox::Alignment::Left:
			return fmt::format_to(ctx.out(), "Left ({})", aValue);
		case tgui::EditBox::Alignment::Center:
			return fmt::format_to(ctx.out(), "Center ({})", aValue);
		case tgui::EditBox::Alignment::Right:
			return fmt::format_to(ctx.out(), "Right ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};

/**
 * Fmt formatter for the \c tgui::Scrollbar::Policy type.
 */
template <> struct fmt::formatter<tgui::Scrollbar::Policy> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::Scrollbar::Policy a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::Scrollbar::Policy::Automatic:
			return fmt::format_to(ctx.out(), "Automatic ({})", aValue);
		case tgui::Scrollbar::Policy::Always:
			return fmt::format_to(ctx.out(), "Always ({})", aValue);
		case tgui::Scrollbar::Policy::Never:
			return fmt::format_to(ctx.out(), "Never ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};

/**
 * Fmt formatter for the \c tgui::Grid::Alignment type.
 */
template <> struct fmt::formatter<tgui::Grid::Alignment> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it != '}') throw format_error("invalid format");
		return it;
	}

	template <typename FormatContext>
	auto format(const tgui::Grid::Alignment a, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		const auto aValue = static_cast<int>(a);
		switch (a) {
		case tgui::Grid::Alignment::Center:
			return fmt::format_to(ctx.out(), "Center ({})", aValue);
		case tgui::Grid::Alignment::UpperLeft:
			return fmt::format_to(ctx.out(), "UpperLeft ({})", aValue);
		case tgui::Grid::Alignment::Up:
			return fmt::format_to(ctx.out(), "Up ({})", aValue);
		case tgui::Grid::Alignment::UpperRight:
			return fmt::format_to(ctx.out(), "UpperRight ({})", aValue);
		case tgui::Grid::Alignment::Right:
			return fmt::format_to(ctx.out(), "Right ({})", aValue);
		case tgui::Grid::Alignment::BottomRight:
			return fmt::format_to(ctx.out(), "BottomRight ({})", aValue);
		case tgui::Grid::Alignment::Bottom:
			return fmt::format_to(ctx.out(), "Bottom ({})", aValue);
		case tgui::Grid::Alignment::BottomLeft:
			return fmt::format_to(ctx.out(), "BottomLeft ({})", aValue);
		case tgui::Grid::Alignment::Left:
			return fmt::format_to(ctx.out(), "Left ({})", aValue);
		default:
			return fmt::format_to(ctx.out(), "UNKNOWN ({})", aValue);
		}
	}
};
