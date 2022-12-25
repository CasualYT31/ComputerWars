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

/**@file joysticktest.hpp
 * Tests the \c sfx::joystick structure.
 */

#include "sharedfunctions.hpp"
#include "userinput.hpp"

/**
 * Tests \c sfx::joystick::operator==.
 */
TEST(JoystickTest, EqualityCheck) {
	sfx::joystick rhs, lhs;
	rhs.axis = sf::Joystick::Axis::X;
	rhs.direction = sfx::axis_direction::Negative;
	lhs = rhs;
	EXPECT_TRUE(rhs == lhs);
	lhs.axis = sf::Joystick::Axis::Y;
	EXPECT_FALSE(rhs == lhs);
	lhs.axis = rhs.axis;
	rhs.direction = sfx::axis_direction::Positive;
	EXPECT_FALSE(rhs == lhs);
}

/**
 * Tests \c sfx::joystick::operator!=.
 */
TEST(JoystickTest, InequalityCheck) {
	sfx::joystick rhs, lhs;
	rhs.axis = sf::Joystick::Axis::X;
	rhs.direction = sfx::axis_direction::Negative;
	lhs = rhs;
	lhs.axis = sf::Joystick::Axis::U;
	EXPECT_TRUE(rhs != lhs);
	lhs.axis = rhs.axis;
	lhs.direction = sfx::axis_direction::Positive;
	EXPECT_TRUE(rhs != lhs);
	lhs = rhs;
	EXPECT_FALSE(rhs != lhs);
}