/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file maths.hpp
 * Defines maths functions.
 */

#pragma once

#include "SFML/System/Vector2.hpp"
#include <cmath>

namespace engine {
	/**
	 * Normalises the given vector.
	 * @param  v The vector to normalise.
	 * @return The normalised vector.
	 */
	inline sf::Vector2f normalise(const sf::Vector2f& v) noexcept {
		return v / ::sqrtf(v.x * v.x + v.y * v.y);
	}

	/**
	 * Floating point equality comparison.
	 * @remark This function is an amended version of the function with the same
	 *         signature from scriptmath.cpp, making it originally part of
	 *         AngelScript.
	 * @param  a       LHS.
	 * @param  b       RHS.
	 * @param  epsilon Tolerance.
	 */
	bool closeTo(const float a, const float b,
		const float epsilon = 0.00001f) noexcept;

	/**
	 * Version of \c closeTo() for use with floating-point vectors.
	 */
	inline bool closeTo(const sf::Vector2f& a, const sf::Vector2f& b,
		const float epsilon = 0.00001f) noexcept {
		return closeTo(a.x, b.x, epsilon) && closeTo(a.y, b.y, epsilon);
	}
}
