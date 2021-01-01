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
 * @file tests.h
 * Defines all the tests.
 */

#pragma once

#include "test.h"

// for documentation on the test namespace, please see test.h
namespace test {
	/**
	 * The entry point into the solution's tests.
	 * The test results are output to a variety of log files in the path <tt>./test/results/</tt>.\n
	 * When you write a new <tt>test_case</tt>-derived class, you need to add it to the list of test cases in this function.
	 * You can do this my pushing back a <tt>new</tt>d instance of your class to the \c testcases vector.
	 * @return The value ideally returned by \c main().
	 */
	int test();

	/**
	 * Tests logger.h classes.
	 */
	class test_logger : public test::test_case {
	public:
		/**
		 * Assigns the name "logger_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_logger(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the creation of a log file via the \c sink class.
		 */
		void sink_Get();
	};

	/**
	 * Tests language.h classes.
	 */
	class test_language : public test::test_case {
	public:
		/**
		 * Assigns the name "language_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_language(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c expand_string class.
		 */
		void expand_string();

		/**
		 * Runs tests related to the \c language_dictionary class.
		 */
		void language_dictionary();
	};
}