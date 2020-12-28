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

/**
 * The \c test namespace contains test-related classes.
 */
namespace test {
	/**
	 * Class which represents a set of unit tests.
	 */
	class test_case {
	protected:
		template<typename T, typename U>
		void assertEqual(T a, U b) const;

		template<typename T, typename U>
		void assertNotEqual(T a, U b) const;

		template<typename T>
		void assertTrue(T a) const;

		template<typename T>
		void assertFalse(T a) const;

		template<typename T, typename U>
		void assertIn(T a, U b) const;

		template<typename T, typename U>
		void assertNotIn(T a, U b) const;
	private:
		/**
		 * Logger object used to print test output.
		 */
		mutable global::logger _logger;
	};
}