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
#include "logger.h"
#include <filesystem>

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
	int val = 8;
	ASSERT_NOT_EQUAL(val, 8);
	ASSERT_FALSE(false);
	ASSERT_TRUE(true);
}

void test::test_language::language_dictionary() {
	ASSERT_EQUAL(8, 8);
	std::unordered_map<std::string, int> map;
	map["test"] = 9;
	map["another"] = -9;
	ASSERT_NOT_IN_MAP(-9, map);
}