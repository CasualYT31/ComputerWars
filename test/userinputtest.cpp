/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file userinputtest.h
 * Tests the \c sfx::user_input class.
 */

#include "sharedfunctions.h"
#include "userinput.h"

/**
 * This test fixture is used to initialise the testing object.
 */
class UserInputTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the test object before all tests.
	 */
	void SetUp() override {
		ui.load(getTestAssetPath("ui/ui.json"));
	}

	/**
	 * The \c sfx::user_input object to test on.
	 */
	sfx::user_input ui;
};

/**
 * Tests behaviour of a \c user_input object after loading a valid script.
 */
TEST_F(UserInputTest, LoadValidScript) {
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 25.0);
	EXPECT_EQ(ui.getConfiguration("select").keyboard.size(), 1);
}

/**
 * Tests behaviour of \c user_input object after loading an invalid script,
 * \em after loading a valid script.
 * The state of the object should be reset.
 */
TEST_F(UserInputTest, LoadInvalidScript) {
	ui.load(getTestAssetPath("ui/faultyui.json"));
	EXPECT_EQ(ui.getConfiguration("select").keyboard.size(), 0);
}