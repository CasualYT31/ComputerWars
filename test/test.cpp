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

test::failed_assert::failed_assert(const std::string& msg) noexcept : runtime_error(msg) {}

test::test_case::test_case(const std::string& name) noexcept : _output(name) {}

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
		_output << "FAULTY TEST ~~~ " << _currentTestName << " ~~~ " << e.what() << '\n';
	} catch (test::failed_assert& e) { // assertion failed
		_failedCount++;
		_output << "FAILED TEST ~~~ " << _currentTestName << " ~~~ " << e.what() << '\n';
	}
}

void test::test_case::endTesting() noexcept {
	_output << "----------------------------\n";
	_output << "Ran " << _count << " test" << (_count == 1 ? "" : "s") << " in " << _timer.getElapsedTime().asSeconds() << "s\n";
	_output << (_faultyCount + _failedCount == 0 ? "OK" : "FAILED") << " ~~~ (faults=" << _faultyCount << " errors=" << _failedCount << ")";
	_started = false;
	_count = 0;
	_faultyCount = 0;
	_failedCount = 0;
}

void test::test_case::_failedTest(const std::string& msg) const {
	throw test::failed_assert(msg);
}