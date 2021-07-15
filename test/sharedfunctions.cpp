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

#include "sharedfunctions.h"

std::string getYear() noexcept {
	std::time_t t = std::time(nullptr);
	std::tm* const pTInfo = std::localtime(&t);
	return std::to_string(1900 + pTInfo->tm_year);
}

std::string getTestAssetPath(const std::string& subdirs) noexcept {
	return COMPUTER_WARS_ROOT_TEST_ASSET_FOLDER + subdirs;
}

bool isTest(const std::vector<const char*>& list) noexcept {
	const char* current =
		testing::UnitTest::GetInstance()->current_test_info()->name();
	for (auto& name : list) {
		if (!strcmp(current, name)) return true;
	}
	return false;
}

void setupJSONScript(const std::function<void(nlohmann::json&)>& f,
	const std::string& p) {
	nlohmann::json j;
	f(j);
	// save the json script
	// if the test script can't be written, then the rest of the test won't likely
	// work, so use assert here
	ASSERT_NO_THROW({
		std::ofstream jscript(getTestAssetPath(p), std::ios_base::trunc);
		jscript << j;
		jscript.close();
	});
}