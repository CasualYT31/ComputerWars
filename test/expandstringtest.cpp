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

/**@file expandstringtest.hpp
 * Tests the \c engine::expand_string class.
 */

#include "language.hpp"
#include "gtest/gtest.hpp"

/**
 * Run a series of tests on \c engine::expand_string with a given var char.
 * @param var The var char to test with.
 */
void expand_string(const std::string& var) {
	// a. test no variables, no var chars
	// b. test no variables, 1 var char
	// c. test no variables, 2 var chars
	// d. test 2 variables, no var chars
	// e. test 2 variables, 1 var char
	// f. test 2 variables, 2 var chars
	// g. test 2 variables, 3 var chars
	// h. test 3 variables, 2 var chars next to each other
	// i. test 3 variables, 3 sets of 3 var chars next to each other
	EXPECT_EQ(engine::expand_string::insert("Hello World!"), "Hello World!");
	EXPECT_EQ(engine::expand_string::insert(
		"Hello" + var + "World!"), "Hello" + var + "World!");
	EXPECT_EQ(engine::expand_string::insert(
		"Hello" + var + "World!" + var), "Hello" + var + "World!" + var);
	EXPECT_EQ(engine::expand_string::insert(
		"var1= var2=", 18, "Test"), "var1= var2=");
	EXPECT_EQ(engine::expand_string::insert(
		"var1=" + var + " var2=", 18, "Test"), "var1=18 var2=");
	EXPECT_EQ(engine::expand_string::insert(
		"var1=" + var + " var2=" + var, -18, "Test"), "var1=-18 var2=Test");
	EXPECT_EQ(engine::expand_string::insert(
		var + "var1=" + var + " var2=" + var, 0.5, "Testing"),
		"0.5var1=Testing var2=" + var);
	EXPECT_EQ(engine::expand_string::insert(var + var, true, false, 9.792), var);
	EXPECT_EQ(engine::expand_string::insert(
		var + var + var + " " + var + var + var + " " + var + var + var, 34, "LLL",
		9.792), var + "34 " + var + "LLL " + var + "9.792");
}

/**
 * Tests \c engine::expand_string with the default var char, which should be '#'.
 */
TEST(ExpandStringTest, WithDefaultVarChar) {
	// expand_string() won't work if this doesn't, so use ASSERT
	ASSERT_EQ(engine::expand_string::getVarChar(), '#');
	expand_string("#");
}

/**
 * Tests \c engine::expand_string::setVarChar().
 */
TEST(ExpandStringTest, SetVarChar) {
	engine::expand_string::setVarChar('$');
}

/**
 * Tests \c engine::expand_string with a custom var char.
 * By the end of the test, the default var char is reassigned. This is because
 * future tests may rely on the default var char to remain assigned.
 */
TEST(ExpandStringTest, WithCustomVarChar) {
	// expand_string() won't work if this doesn't, so use ASSERT
	// we can safely assume that the default var char is in place
	ASSERT_EQ(engine::expand_string::getVarChar(), '$');
	expand_string("$");
	engine::expand_string::setVarChar('#');
	ASSERT_EQ(engine::expand_string::getVarChar(), '#');
}