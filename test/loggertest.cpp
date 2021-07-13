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

/**@file loggertest.h
 * Tests the \c engine::logger class.
 */

#include "logger.h"
#include "gtest/gtest.h"

 /**
  * This function tests the \c logger class.
  */
TEST(LoggerTest, LoggerTest) {
	engine::logger log("logger_test");
	// test simple writes, errors, and warnings
	log.write("Hello World!");
	log.warning("We are currently testing!");
	log.error("Oh no!");
	// test variable writes, errors, and warnings
	int simple_int = 8;
	std::string text = "Inserted";
	double f_number = -79.5;
	bool boolean = true;
	log.write("Number = {}", simple_int);
	log.warning("{} text, {} = number", text, f_number);
	log.error("Error is {}!", boolean);
	// now search the log file to see if all of the previous writes were written as
	// expected
	std::string logFile = engine::sink::GetLog();
	EXPECT_NE(logFile.find("[info] Hello World!"), std::string::npos);
	EXPECT_NE(
		logFile.find("[warning] We are currently testing!"), std::string::npos);
	EXPECT_NE(logFile.find("[error] Oh no!"), std::string::npos);
	EXPECT_NE(logFile.find("[info] Number = 8"), std::string::npos);
	EXPECT_NE(logFile.find(
		"[warning] Inserted text, -79.5 = number"), std::string::npos);
	EXPECT_NE(logFile.find("[error] Error is true!"), std::string::npos);
}