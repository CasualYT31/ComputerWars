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
#include <random>
#include <memory>

#define PI     3.14159265358979323846f
#define TO_RAD (PI / 180.0f)
#define TO_DEG (180.0f / PI)

namespace engine {
	/**
	 * Calculates the magnitude of a vector.
	 * @param  v The vector to measure.
	 * @return The length of the vector.
	 */
	inline float magnitude(const sf::Vector2f& v) noexcept {
		// Is slightly faster than passing { 0, 0 } to distance() I guess.
		return ::sqrtf(v.x * v.x + v.y * v.y);
	}

	/**
	 * Normalises the given vector.
	 * @param  v The vector to normalise.
	 * @return The normalised vector.
	 */
	inline sf::Vector2f normalise(const sf::Vector2f& v) noexcept {
		return v / magnitude(v);
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

	/**
	 * Calculates the euclidean distance between two points.
	 * @param  a The first point.
	 * @param  b The second point.
	 * @return The distance.
	 */
	inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) noexcept {
		return ::sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	}

	/**
	 * Calculates the angle of a point, in degrees, relative to the X axis.
	 * @param  p      The point.
	 * @param  offset The offset to apply to the angle 
	 * @return The angle of the given point, in degrees.
	 */
	inline float angleOfPoint(const sf::Vector2f& p) noexcept {
		const auto angle = ::atan2f(p.y, p.x) * TO_DEG;
		return angle < 0.0f ? angle + 360.0f : angle;
	}

	/**
	 * Invokes the sine function with degrees.
	 * @param  a Angle, in degrees.
	 * @return Result.
	 */
	inline float sin(const float a) noexcept {
		return ::sinf(a * TO_RAD);
	}

	/**
	 * Invokes the cosine function with degrees.
	 * @param  a Angle, in degrees.
	 * @return Result.
	 */
	inline float cos(const float a) noexcept {
		return ::cosf(a * TO_RAD);
	}

	/**
	 * Creates and seeds a random number generator.
	 * @return Pointer to the RNG.
	 */
	std::unique_ptr<std::mt19937> RNGFactory();
}
