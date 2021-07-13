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
 */

#include "logger.h"
#include "gtest/gtest.h"
#include <filesystem>

/**
 * This function tests the \c sink class.
 */
TEST(SinkTest, SinkTest) {
	// first ensure that the test folder has been removed
	std::filesystem::remove_all(std::filesystem::current_path() / "test");
	// carry out tests
	// The first call to \c Get() should actually create the file
	// the second call should not
	auto firstLog = engine::sink::Get("Tests", "Dev", "./test/", false);
	auto secondLog =
		engine::sink::Get("Test Again", "Developer", "./test/results/", false);
	EXPECT_EQ(firstLog, secondLog);
	bool firstLogFileExists = std::filesystem::exists("./test/Log.log");
	bool secondLogFileExists = std::filesystem::exists("./test/results/Log.log");
	EXPECT_TRUE(firstLogFileExists);
	EXPECT_FALSE(secondLogFileExists);
	// now test the properties
	EXPECT_EQ(engine::sink::ApplicationName(), "Tests");
	EXPECT_EQ(engine::sink::DeveloperName(), "Dev");
	// obviously test is dependent on year of execution...
	EXPECT_EQ(engine::sink::GetYear(), "2021");
	// has the file been written as expected so far?
	// also implicitly tests that GetLog() is working as expected
	std::string file = engine::sink::GetLog();
	std::string firstLine = file.substr(0, file.find('\n'));
	EXPECT_EQ(firstLine, "Tests � 2021 Dev");
}