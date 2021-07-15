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

#include "sharedfunctions.h"
#include "logger.h"

/**
 * This function tests \c engine::logger::write(), with no variable insertion.
 */
TEST(LoggerTest, Write) {
	engine::logger log("logger_test");
	log.write("Hello World!");
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[info] Hello World!");
}

/**
 * This function tests \c engine::logger::warning(), with no variable insertion.
 */
TEST(LoggerTest, Warning) {
	engine::logger log("logger_test");
	log.warning("We are currently testing!");
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[warning] We are currently testing!");
}

/**
 * This function tests \c engine::logger::error(), with no variable insertion.
 */
TEST(LoggerTest, Error) {
	engine::logger log("logger_test");
	log.error("Oh no!");
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[error] Oh no!");
}

/**
 * This function tests \c engine::logger::write(), with variable insertion.
 */
TEST(LoggerTest, WriteVariables) {
	engine::logger log("logger_test");
	int simple_int = 8;
	log.write("Number = {}", simple_int);
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[info] Number = 8");
}

/**
 * This function tests \c engine::logger::warning(), with variable insertion.
 */
TEST(LoggerTest, WarningVariables) {
	engine::logger log("logger_test");
	std::string text = "Inserted";
	double f_number = -79.5;
	log.warning("{} text, {} = number", text, f_number);
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[warning] Inserted text, -79.5 = number");
}

/**
 * This function tests \c engine::logger::error(), with variable insertion.
 */
TEST(LoggerTest, ErrorVariables) {
	engine::logger log("logger_test");
	bool boolean = true;
	log.error("Error is {}!", boolean);
	std::string logFile = engine::sink::GetLog();
	EXPECT_IN_LOG("[error] Error is true!");
}

/**
 * This function tests \c engine::logger::countCreated().
 */
TEST(LoggerTest, CountCreated) {
	EXPECT_EQ(engine::logger::countCreated(), 6);
}