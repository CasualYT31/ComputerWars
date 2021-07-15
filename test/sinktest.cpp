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

/**@file sinktest.h
 * Tests the \c engine::sink class.
 * Unfortunately, it is not very easy to perform date, time, and hardware
 * specification tests, especially tests that work across any machine. The simplest
 * thing to do is manually check these items yourself.
 */

#include "sharedfunctions.h"
#include "logger.h"
#include <filesystem>

/**
 * This function tests \c engine::sink::Get().
 * The first call to \c Get() should actually create the file, the second call
 * should not.
 */
TEST(SinkTest, GetSink) {
	// first ensure that the test folder has been removed to ensure the test's
	// consistency
	std::error_code ignored;
	std::filesystem::remove("Log.log", ignored);
	auto firstLog = engine::sink::Get("Tests", "Dev", ".", false, false);
	auto secondLog =
		engine::sink::Get("Test Again", "Developer", "test", false, false);
	// if sink creation failed altogether, then all future tests will fail in this
	// suite, so make this an assertion
	ASSERT_TRUE(firstLog);
	EXPECT_EQ(firstLog, secondLog);
	bool firstLogFileExists = std::filesystem::exists("./Log.log");
	bool secondLogFileExists = std::filesystem::exists("test/Log.log");
	EXPECT_TRUE(firstLogFileExists);
	EXPECT_FALSE(secondLogFileExists);
}

/**
 * This function tests \c engine::sink::ApplicationName().
 */
TEST(SinkTest, ApplicationName) {
	EXPECT_EQ(engine::sink::ApplicationName(), "Tests");
}

/**
 * This function tests \c engine::sink::DeveloperName().
 */
TEST(SinkTest, DeveloperName) {
	EXPECT_EQ(engine::sink::DeveloperName(), "Dev");
}

/**
 * This function tests \c engine::sink::GetYear().
 */
TEST(SinkTest, GetYear) {
	EXPECT_EQ(engine::sink::GetYear(), getYear());
}

/**
 * This function tests \c engine::sink::GetLog().
 * This function also tests that the log has been written as expected thus far.
 */
TEST(SinkTest, GetLog) {
	std::string file = engine::sink::GetLog();
	std::string firstLine = file.substr(0, file.find('\n'));
	EXPECT_EQ(firstLine, "Tests © " + getYear() + " Dev");
}