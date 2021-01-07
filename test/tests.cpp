/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

#include "tests.h"
#include "language.h"
#include <filesystem>
#include <iostream>

//*********************
//*TESTING ENTRY POINT*
//*********************
int test::test() {
	// setup the test cases
	std::string path = "./test/results/";
	std::vector<test::test_case*> testcases;
	testcases.push_back(new test::test_logger(path));
	testcases.push_back(new test::test_language(path));

	// run the test cases
	for (auto itr = testcases.begin(), enditr = testcases.end(); itr != enditr; itr++) {
		(*itr)->runTests();
		delete *itr;
	}
	return 0;
}

//****************
//*LOGGER.H TESTS*
//****************
test::test_logger::test_logger(const std::string& path) noexcept : test_case(path + "logger_test_case.log") {}

void test::test_logger::runTests() noexcept {
	RUN_TEST(test::test_logger::sink);
	RUN_TEST(test::test_logger::logger);
	endTesting();
}

void test::test_logger::sink() {
	// first Get should actually create the file, second should not
	auto firstLog = global::sink::Get("Tests", "Dev", "./test/results/", false);
	auto secondLog = global::sink::Get("Test Again", "Developer", "./test/", false);
	ASSERT_EQUAL(firstLog, secondLog);
	bool firstLogFileExists = std::filesystem::exists("./test/results/Log.log");
	bool secondLogFileExists = std::filesystem::exists("./test/Log.log");
	ASSERT_TRUE(firstLogFileExists);
	ASSERT_FALSE(secondLogFileExists);
	// now test the properties
	ASSERT_EQUAL(global::sink::ApplicationName(), "Tests");
	ASSERT_EQUAL(global::sink::DeveloperName(), "Dev");
	ASSERT_EQUAL(global::sink::GetYear(), "2021"); // obviously test is dependent on year of execution...
	// has the file been written as expected so far?
	// also implicitly tests that GetLog() is working as expected
	std::string file = global::sink::GetLog();
	std::string firstLine = file.substr(0, file.find('\n'));
	ASSERT_EQUAL(firstLine, "Tests © 2021 Dev");
}

void test::test_logger::logger() {
	global::logger log("logger_test");
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
	// now search the log file to see if all of the previous writes were written as expected
	std::string logFile = global::sink::GetLog();
	ASSERT_TRUE(logFile.find("[info] Hello World!"));
	ASSERT_TRUE(logFile.find("[warning] We are currently testing!"));
	ASSERT_TRUE(logFile.find("[error] Oh no!"));
	ASSERT_TRUE(logFile.find("[info] Number = 8"));
	ASSERT_TRUE(logFile.find("[warning] Inserted text, -79.5 = number"));
	ASSERT_TRUE(logFile.find("[error] Error is true!"));
}

//******************
//*LANGUAGE.H TESTS*
//******************
test::test_language::test_language(const std::string& path) noexcept : test_case(path + "language_test_case.log") {}

void test::test_language::runTests() noexcept {
	RUN_TEST(test::test_language::expand_string);
	RUN_TEST(test::test_language::language_dictionary);
	endTesting();
}

void test::test_language::expand_string() {
	// expand_string is a pretty easy class to test,
	// but there are a lot of cases to cover:
	// 1. test with default var char
	// a. test no variables, no var chars
	// b. test no variables, 1 var char
	// c. test no variables, 2 var chars
	// d. test 2 variables, no var chars
	// e. test 2 variables, 1 var char
	// f. test 2 variables, 2 var chars
	// g. test 2 variables, 3 var chars
	// h. test 3 variables, 2 var chars next to each other
	// i. test 3 variables, 3 sets of 3 var chars next to each other
	// 2. test get and set var char methods
	// 3. repeat tests a-i with a new var char
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '#');
	expand_string_("#");
	i18n::expand_string::setVarChar('$');
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '$');
	expand_string_("$");
	// ENSURE TO REVERT BACK TO THE OLD VAR CHAR TO
	// ENSURE THAT FUTURE TESTS THAT MAY RELY ON IT WORK
	i18n::expand_string::setVarChar('#');
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '#');
}

void test::test_language::expand_string_(const std::string& var) {
	// see expand_string() ~ this method performs tests a-i
	ASSERT_EQUAL(i18n::expand_string::insert("Hello World!"), "Hello World!");
	ASSERT_EQUAL(i18n::expand_string::insert("Hello" + var + "World!"), "Hello" + var + "World!");
	ASSERT_EQUAL(i18n::expand_string::insert("Hello" + var + "World!" + var), "Hello" + var + "World!" + var);
	ASSERT_EQUAL(i18n::expand_string::insert("var1= var2=", 18, "Test"), "var1= var2=");
	ASSERT_EQUAL(i18n::expand_string::insert("var1=" + var + " var2=", 18, "Test"), "var1=18 var2=");
	ASSERT_EQUAL(i18n::expand_string::insert("var1=" + var + " var2=" + var, -18, "Test"), "var1=-18 var2=Test");
	ASSERT_EQUAL(i18n::expand_string::insert(var + "var1=" + var + " var2=" + var, 0.5, "Testing"), "0.5var1=Testing var2=" + var);
	ASSERT_EQUAL(i18n::expand_string::insert(var + var, true, false, 9.792), var);
	ASSERT_EQUAL(i18n::expand_string::insert(var + var + var + " " + var + var + var + " " + var + var + var, 34, "LLL", 9.792),
		                                     var + "34 " + var + "LLL " + var + "9.792");
}

void test::test_language::language_dictionary() {
	// pass
}