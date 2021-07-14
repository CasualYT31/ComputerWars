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

/**@file sharedfunctions.h
 * Contains helper functions and macros used by testing code.
 */

#pragma once

#include "logger.h"
#include "gtest/gtest.h"

/**
 * This macro is a short way to determine if a piece of text is in the log file.
 * @param n The string to search for in the log file.
 */
#define EXPECT_IN_LOG(n) \
	EXPECT_NE(engine::sink::GetLog().find(n), std::string::npos);

/**
 * Helper function which gets the current year.
 * @warning Yeah, tests can fail if the log file is created on the last second of
 *          the year... But I'm not faffing around to take that into account.
 * @return  The current year in a string format expected to be found in the log.
 */
std::string getYear() noexcept;