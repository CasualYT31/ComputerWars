/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file converttest.hpp
 * Tests the \c sfx::convert class.
 */

#include "sharedfunctions.hpp"
#include "userinput.hpp"

/**
 * Tests \c sfx::convert::tokeycode().
 */
TEST(ConvertTest, ToKeyCode) {
	// test conversion with normal data
	EXPECT_EQ(sfx::convert::tokeycode(35), sf::Keyboard::Key::Num9);
	EXPECT_NE(sfx::convert::tokeycode(0), sf::Keyboard::Key::Num9);
	// test conversion with extreme data
	EXPECT_EQ(sfx::convert::tokeycode(100), sf::Keyboard::Key::Pause);
	EXPECT_EQ(sfx::convert::tokeycode(sf::Keyboard::Key::KeyCount),
		sf::Keyboard::Key::Pause);
	// test conversion with invalid data, with and without logger output
	EXPECT_EQ(sfx::convert::tokeycode(UINT_MAX), sf::Keyboard::Key::Pause);
	engine::logger logger("keycodetesting");
	EXPECT_EQ(sfx::convert::tokeycode(500, &logger), sf::Keyboard::Key::Pause);
	EXPECT_IN_LOG("keycodetesting");
}

/**
 * Tests \c sfx::convert::tomousebtn().
 */
TEST(ConvertTest, ToMouseBtn) {
	// test conversion with normal data
	EXPECT_EQ(sfx::convert::tomousebtn(2), sf::Mouse::Button::Middle);
	EXPECT_NE(sfx::convert::tomousebtn(0), sf::Mouse::Button::Middle);
	// test conversion with extreme data
	EXPECT_EQ(sfx::convert::tomousebtn(4), sf::Mouse::Button::XButton2);
	EXPECT_EQ(sfx::convert::tomousebtn(sf::Mouse::Button::ButtonCount),
		sf::Mouse::Button::XButton2);
	// test conversion with invalid data, with and without logger output
	EXPECT_EQ(sfx::convert::tomousebtn(UINT_MAX), sf::Mouse::Button::XButton2);
	engine::logger logger("mousebtntesting");
	EXPECT_EQ(sfx::convert::tomousebtn(500, &logger), sf::Mouse::Button::XButton2);
	EXPECT_IN_LOG("mousebtntesting");
}

/**
 * Tests \c sfx::convert::toaxis().
 */
TEST(ConvertTest, ToAxis) {
	// test conversion with normal data
	EXPECT_EQ(sfx::convert::toaxis(5), sf::Joystick::Axis::V);
	EXPECT_NE(sfx::convert::toaxis(0), sf::Joystick::Axis::V);
	// test conversion with extreme data
	EXPECT_EQ(sfx::convert::toaxis(7), sf::Joystick::Axis::PovY);
	EXPECT_EQ(sfx::convert::toaxis(sf::Joystick::AxisCount),
		sf::Joystick::Axis::PovY);
	// test conversion with invalid data, with and without logger output
	EXPECT_EQ(sfx::convert::toaxis(UINT_MAX), sf::Joystick::Axis::PovY);
	engine::logger logger("axistesting");
	EXPECT_EQ(sfx::convert::toaxis(500, &logger), sf::Joystick::Axis::PovY);
	EXPECT_IN_LOG("axistesting");
}

/**
 * Tests \c sfx::convert::tosignaltype().
 */
TEST(ConvertTest, ToSignalType) {
	// test conversion with normal data
	EXPECT_EQ(sfx::convert::tosignaltype(1), sfx::control_signal::ButtonForm);
	EXPECT_NE(sfx::convert::tosignaltype(0), sfx::control_signal::ButtonForm);
	// test conversion with extreme data
	EXPECT_EQ(sfx::convert::tosignaltype(2), sfx::control_signal::DelayedForm);
	EXPECT_EQ(sfx::convert::tosignaltype(sfx::control_signal::SignalTypeCount),
		sfx::control_signal::ButtonForm);
	// test conversion with invalid data, with and without logger output
	EXPECT_EQ(sfx::convert::tosignaltype(UINT_MAX),
		sfx::control_signal::ButtonForm);
	engine::logger logger("signaltypetesting");
	EXPECT_EQ(sfx::convert::tosignaltype(500, &logger),
		sfx::control_signal::ButtonForm);
	EXPECT_IN_LOG("signaltypetesting");
}

/**
 * Tests \c sfx::convert::toaxisdir().
 */
TEST(ConvertTest, ToAxisDir) {
	EXPECT_EQ(sfx::convert::toaxisdir(-1), sfx::axis_direction::Negative);
	EXPECT_EQ(sfx::convert::toaxisdir(0), sfx::axis_direction::Positive);
	EXPECT_EQ(sfx::convert::toaxisdir(1), sfx::axis_direction::Positive);
	EXPECT_EQ(sfx::convert::toaxisdir(INT_MIN), sfx::axis_direction::Negative);
	EXPECT_EQ(sfx::convert::toaxisdir(INT_MAX), sfx::axis_direction::Positive);
}