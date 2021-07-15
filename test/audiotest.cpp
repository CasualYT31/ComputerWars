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

/**@file audiotest.h
 * Tests the \c sfx::audio class.
 */

#include "sharedfunctions.h"
#include "audio.h"

/**
 * This test fixture is used to initialise an \c audio object for testing.
 */
class AudioTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the object before certain tests.
	 */
	void SetUp() override {
		/* if (isTest({"LoadExistentFile"})) {
			// prepare fonts.json and otherfonts.json only once
			setupJSONScript([](nlohmann::json& j) {
				// retrieve the correct path to the font file at runtime
				j["dialouge"] = getTestAssetPath("fonts/AdvanceWars2.ttf");
			}, "fonts/fonts.json");
			setupJSONScript([](nlohmann::json& j) {
				// retrieve the correct path to the font file at runtime
				j["text"] = getTestAssetPath("fonts/AdvanceWars2.ttf");
			}, "fonts/otherfonts.json");
		}
		if (!isTest({ "EmptyFonts", "LoadNonExistentFile" })) {
			// load fonts.json
			fontstest.load(getTestAssetPath("fonts/fonts.json"));
		} */
	}

	/**
	 * The \c audio object to test on.
	 */
	sfx::audio audio;
};

/**
 * Tests the behaviour of <tt>audio::load()</tt>ing a valid script.
 */
TEST_F(AudioTest, LoadValidScript) {

}