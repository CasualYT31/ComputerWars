/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file sharedfunctions.hpp
 * Contains helper functions and macros used by testing code.
 */

#pragma once

#include "nlohmann/json.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <functional>

/**
 * This macro is a short way to determine if a piece of text is in the log file.
 * @param n The string to search for in the log file.
 */
#define EXPECT_IN_LOG(n) \
	EXPECT_NE(engine::sink::GetLog().find(n), std::string::npos);

/**
 * This macro is a short way to determine if a piece of text is not in the log
 * file.
 * @param n The string to search for in the log file.
 */
#define EXPECT_NOT_IN_LOG(n) \
	EXPECT_EQ(engine::sink::GetLog().find(n), std::string::npos);

/**
 * Helper function which gets the current year.
 * @warning Yeah, tests can fail if the log file is created on the last second of
 *          the year... But I'm not faffing around to take that into account.
 * @return  The current year in a string format expected to be found in the log.
 */
std::string getYear() noexcept;

/**
 * Function used to find the path to a test assets folder.
 * This function determines the root path, then the client can append more folders
 * as required via the parameter. The root path will always end with a '/'.
 * @param  subdirs The subdirectories to append to the root path.
 * @return The path to the test asset folder the client requires.
 */
std::string getTestAssetPath(const std::string& subdirs) noexcept;

/**
 * Determines if the current test is called a given name.
 * @param  list The list of test names to test.
 * @return \c TRUE if the current test's name matched at least one of the names
 *         given, \c FALSE if it did not match any of the names given.
 */
bool isTest(const std::vector<const char*>& list) noexcept;

/**
 * Determines if the current test name contains a given string.
 * @param  str The string to search for in the current test name.
 * @return \c TRUE if the current test's name contained the given string, \c FALSE
 *         otherwise.
 */
bool testNameContains(const char* str) noexcept;

/**
 * This function is used to setup a JSON script before testing.
 * @param f Function which accepts a reference to a JSON object and writes to it
 *          before saving as necessary.
 * @param p The path of the JSON script <em>relative to</em> the root test asset
 *          folder.
 */
void setupJSONScript(const std::function<void(nlohmann::json&)>& f,
	const std::string& p);

/**
 * Specifically sets up the <tt>renderer/renderer.json</tt> test JSON script.
 */
void setupRendererJSONScript();

/**
 * Waits for three seconds.
 * @param msg Optional string message to write to standard output.
 */
void longWait(std::string msg = "") noexcept;

/**
 * Waits for one second.
 * @param msg Optional string message to write to standard output.
 */
void shortWait(std::string msg = "") noexcept;
