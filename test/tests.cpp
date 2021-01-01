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

int test::test() {
	// setup the test cases
	std::vector<test::test_case*> testcases;
	testcases.push_back(new test::test_language("./test/results/"));

	// run the test cases
	for (auto itr = testcases.begin(), enditr = testcases.end(); itr != enditr; itr++) {
		(*itr)->runTests();
		delete *itr;
	}
	return 0;
}

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