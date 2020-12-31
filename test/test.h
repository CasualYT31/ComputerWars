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

/**
 * @file test.h
 * Declares the base class of all test classes.
 */

#pragma once

#include "logger.h"
#include "sfml/System/Clock.hpp"

/**
 * Macro that allows implementations of test::test_case::runTests() to run a test and
 * automatically assign the name of the test as the test function's name.
 * @param instance      The name of the class which contains the method to run.
 * @param test_function The name of the method which contains the test to run.
 */
#define RUN_TEST(instance, test_function) runTest(#test_function, new std::function<void(test::test_case*)>(&instance::test_function))

/**
 * The \c test namespace contains test-related classes.
 */
namespace test {
	/**
	 * Exception thrown when a \c test_case assert method fails.
	 */
	class failed_assert : public std::exception {
	public:
		/**
		 * The exception text.
		 * @return "Assertion failed!"
		 */
		virtual const char* what() const throw() {
			return "Assertion failed!";
		}
	};

	/**
	 * Class which represents a set of unit tests.
	 * Each derived class is a test case, with its unit tests executed within its implemented \c runTests().
	 * Tests can be divided up within the subclass in whatever way the programmer sees fit, so long as they are executed within \c runTests(),
	 * and remain within the subclass (as if they are outside of the subclass, the assert methods cannot be called).\n
	 * Note also that tests cannot be specified with \c noexcept, as when an assertion fails, \c failed_assert is thrown,
	 * and must be left alone for the \c test_case class to handle.
	 */
	class test_case {
	public:
		/**
		 * The method which will include this test case's tests.
		 * @warning Care must be taken when writing the unit tests to \b not call this method within them,
		 *          or else the stack will overflow (unless you implement a terminating condition).
		 */
		virtual void runTests() noexcept = 0;
	protected:
		/**
		 * Called to initialise a new unit test.
		 * If an assert statement fails within \c test, it will stop immediately and the failure will be logged.
		 * If \c test isn't a valid, callable object, the test will be classified as faulty and will be logged.
		 * @param name The name of the test.
		 * @param test The test to execute. \c failed_assert exceptions will be automatically handled.
		 */
		void runTest(const std::string& name, const std::function<void(test::test_case*)>& test) noexcept;

		/**
		 * Called when all unit tests have been carried out.
		 * Logs the total runtime of the test case, and the number of both faulty and failed tests.
		 * Also resets \c _started to \c false so that the test case can be executed again.
		 */
		void endTesting() noexcept;

		/**
		 * Asserts that two values are equivalent according to their comparison operator results (==).
		 * @tparam T The type of parameter \c a.
		 * @tparam U The type of parameter \c b.
		 * @param  a LHS value.
		 * @param  b RHS value.
		 * @throws failed_assert if \c a and \b are unequal.
		 */
		template<typename T, typename U>
		void assertEqual(T a, U b) const;

		/**
		 * Asserts that two values aren't equivalent according to their comparison operator results (!=).
		 * @tparam T The type of parameter \c a.
		 * @tparam U The type of parameter \c b.
		 * @param  a LHS value.
		 * @param  b RHS value.
		 * @throws failed_assert if \c a and \b are equal.
		 */
		template<typename T, typename U>
		void assertNotEqual(T a, U b) const;

		/**
		 * Asserts that a value evaluates to true according to its boolean operator.
		 * @tparam T The type of parameter \c a.
		 * @param  a The value.
		 * @throws failed_assert if \c a evaluates to false.
		 */
		template<typename T>
		void assertTrue(T a) const;

		/**
		 * Asserts that a value evaluates to false according to its boolean operator.
		 * @tparam T The type of parameter \c a.
		 * @param  a The value.
		 * @throws failed_assert if \c a evaluates to true.
		 */
		template<typename T>
		void assertFalse(T a) const;

		/**
		 * Asserts that a given value (not key) is stored within a given \c unordered_map.
		 * @tparam T The type of values stored in the map.
		 * @tparam U The type of keys stored in the map.
		 * @param  a The value to test for.
		 * @param  b The map to search.
		 * @throws failed_assert if \c a is not within \b.
		 */
		template<typename T, typename U>
		void assertInMap(T a, std::unordered_map<U, T> b) const;

		/**
		 * Asserts that a given value (not key) isn't stored within a given \c unordered_map.
		 * @tparam T The type of values stored in the map.
		 * @tparam U The type of keys stored in the map.
		 * @param  a The value to test for.
		 * @param  b The map to search.
		 * @throws failed_assert if \c a is within \b.
		 */
		template<typename T, typename U>
		void assertNotInMap(T a, std::unordered_map<U, T> b) const;
	private:
		/**
		 * Throws a \c failed_assert exception and logs it as a failed test.
		 * @tparam T   Type of parameter \c a.
		 * @tparam U   Type of parameter \c b.
		 * @param  msg The message to literally append to the log message.
		 *             This means you can add {}s where you want to insert parameters \c a and \c b.
		 * @param  a   The first value.
		 * @param  b   The second value.
		 * @throws failed_assert
		 */
		template<typename T, typename U>
		void _failedTest(const std::string& msg, T a, U b) const;

		/**
		 * Logger object used to print test output.
		 */
		mutable global::logger _logger;

		/**
		 * Flag used to determine if the test case has started execution or not.
		 */
		bool _started = false;

		/**
		 * Keeps track of the duration of the test case during execution.
		 */
		sf::Clock _timer;

		/**
		 * Keeps track of the number of the tests.
		 */
		std::size_t _count = 0;

		/**
		 * Keeps track of the number of faulty tests.
		 */
		std::size_t _faultyCount = 0;

		/**
		 * Keeps track of the number of failed tests.
		 */
		std::size_t _failedCount = 0;

		/**
		 * Keeps track of the name of the current test.
		 */
		std::string _currentTestName = "";
	};
}

template<typename T, typename U>
void test::test_case::_failedTest(const std::string& msg, T a, U b) const {
	_failedCount++;
	_logger.error("FAILED TEST ~~~ {} ~~~ " + msg, _currentTestName, a, b);
	throw new failed_assert();
}

template<typename T, typename U>
void test::test_case::assertEqual(T a, U b) const {
	if (!(a == b)) _failedTest("{} is not equal to {}", a, b);
}

template<typename T, typename U>
void test::test_case::assertNotEqual(T a, U b) const {
	if (!(a != b)) _failedTest("{} is equal to {}", a, b);
}

template<typename T>
void test::test_case::assertTrue(T a) const {
	if (!a) _failedTest("{} is {}", a, "true");
}

template<typename T>
void test::test_case::assertFalse(T a) const {
	if (a) _failedTest("{} is {}", a, "false");
}

template<typename T, typename U>
void test::test_case::assertInMap(T a, std::unordered_map<U, T> b) const {
	for (auto itr = b.begin(), enditr = b.end(); itr != enditr; itr++) {
		if (itr->second == a) return;
	}
	_failedTest("{} is not in {}", a, "the map");
}

template<typename T, typename U>
void test::test_case::assertNotInMap(T a, std::unordered_map<U, T> b) const {
	for (auto itr = b.begin(), enditr = b.end(); itr != enditr; itr++) {
		if (itr->second == a) _failedTest("{} is stored at the {} key in the map", a, itr->first);
	}
}