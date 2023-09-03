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

/**@file hash.hpp
 * Declares custom specialisations of \c std::hash.
 */

#pragma once

#include "SFML/System/Vector2.hpp"

namespace std {
	/**
	 * Custom specialisation of \c std::hash for \c sf::Vector2.
	 * Much thanks to https://en.cppreference.com/w/cpp/utility/hash and
	 * https://stackoverflow.com/questions/9927208/requirements-for-elements-in-stdunordered-set.
	 * Also thanks to Elias Daler and Laurent
	 * (https://en.sfml-dev.org/forums/index.php?topic=24275.0).
	 */
	template<typename T> struct hash<sf::Vector2<T>> {
		/**
		 * Custom specialisation of \c std::hash for \c sf::Vector2.
		 */
		std::size_t operator()(sf::Vector2<T> const& s) const noexcept {
			// I don't actually think they need to be separate...
			// Only did it this way in order to preserve Elias' intended execution
			// (since the actual hashing was in a function in their example,
			// meaning there would be two separate hashers in execution)
			std::hash<T> hasherX, hasherY;
			std::size_t seed = 0;
			seed ^= hasherX(s.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hasherY(s.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}
