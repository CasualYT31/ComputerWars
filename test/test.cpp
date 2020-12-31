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

#include "test.h"

test::test_case::test_case(const std::string& name) noexcept : _logger(name) {}

void test::test_case::runTest(const std::string& name, const std::function<void(void)>& test) noexcept {
	if (!_started) {
		_started = true;
		_timer.restart();
	}
	_count++;
	_currentTestName = name;
	try {
		test();
		// successful test
	} catch (std::bad_function_call& e) { // no test
		_faultyCount++;
		_logger.error("FAULTY TEST ~~~ {} ~~~ {}", _currentTestName, e.what());
	} catch (test::failed_assert& e) { // assertion failed
		// see _failedTest()
		e.what();
	}
}

void test::test_case::endTesting() noexcept {
	_logger.write("~~~ Ran {} test{} in {}s ~~~ {} (faults={} errors={})", _count, (_count == 1 ? "" : "s"),
		_timer.getElapsedTime().asSeconds(), (_faultyCount + _failedCount == 0 ? "OK" : "FAILED"), _faultyCount, _failedCount);
	_started = false;
	_count = 0;
	_faultyCount = 0;
	_failedCount = 0;
}