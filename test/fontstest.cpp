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

/**@file fontstest.hpp
 * Tests the \c sfx::fonts class.
 */

#include "sharedfunctions.hpp"
#include "fonts.hpp"

/**
 * This test fixture is used to initialise a \c fonts object for testing.
 */
class FontsTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the object before certain tests.
	 */
	void SetUp() override {
		if (isTest({ "LoadExistentFile" })) {
			// prepare fonts.json and otherfonts.json only once
			setupJSONScript([](nlohmann::json& j) {
				// retrieve the correct path to the font file at runtime
				j["dialogue"] = getTestAssetPath("fonts/AdvanceWars2.ttf");
			}, "fonts/fonts.json");
			setupJSONScript([](nlohmann::json& j) {
				// retrieve the correct path to the font file at runtime
				j["text"] = getTestAssetPath("fonts/AdvanceWars2.ttf");
			}, "fonts/otherfonts.json");
		}
		if (!isTest({ "EmptyFonts", "LoadNonExistentFile" })) {
			// load fonts.json
			fontstest.load(getTestAssetPath("fonts/fonts.json"));
		}
	}

	/**
	 * The \c fonts object to test on.
	 */
	sfx::fonts fontstest;
};

/**
 * Test behaviour of an \c sfx::fonts object when it is empty.
 * Also tests that the object is writing to the log file as expected.
 */
TEST_F(FontsTest, EmptyFonts) {
	EXPECT_EQ(fontstest["test"], nullptr);
	EXPECT_IN_LOG("fonts");
}

/**
 * Tests \c sfx::fonts::load() with a non-existent file.
 */
TEST_F(FontsTest, LoadNonExistentFile) {
	fontstest.load("badfile.json");
	EXPECT_TRUE(fontstest.whatFailed() & engine::json_state::FAILED_SCRIPT_LOAD);
}

/**
 * Test \c sfx::fonts::load() with an existent file.
 */
TEST_F(FontsTest, LoadExistentFile) {
	// test fixture should have already loaded the file
	// assert it is not null beforehand in case loading the font failed
	ASSERT_NE(fontstest["dialogue"], nullptr);
	EXPECT_EQ(fontstest["dialogue"]->getInfo().family, "Advance Wars 2 GBA");
}

/**
 * Tests that attempting to \c load() a non-existent file doesn't overwrite an
 * \c sfx::fonts object's state.
 */
TEST_F(FontsTest, LoadNonExistentFileDoesNotOverwrite) {
	fontstest.load("anotherbadfile.json");
	EXPECT_TRUE(fontstest.whatFailed() & engine::json_state::FAILED_SCRIPT_LOAD);
	fontstest.resetState();
	ASSERT_NE(fontstest["dialogue"], nullptr);
	EXPECT_EQ(fontstest["dialogue"]->getInfo().family, "Advance Wars 2 GBA");
}

/**
 * Tests that <tt>load()</tt>ing a faulty file overwrites an \c sfx::fonts object's
 * state.
 */
TEST_F(FontsTest, LoadFaultyFileOverwrites) {
	fontstest.load(getTestAssetPath("fonts/faultyfonts.json"));
	fontstest.resetState();
	EXPECT_EQ(fontstest["dialogue"], nullptr);
}

/**
 * Tests that <tt>load()</tt>ing a healthy file overwrites an \c sfx::fonts
 * object's existing state.
 */
TEST_F(FontsTest, LoadHealthyFileOverwrites) {
	EXPECT_NE(fontstest["dialogue"], nullptr);
	fontstest.load(getTestAssetPath("fonts/otherfonts.json"));
	EXPECT_EQ(fontstest["dialogue"], nullptr);
	ASSERT_NE(fontstest["text"], nullptr);
	EXPECT_EQ(fontstest["text"]->getInfo().family, "Advance Wars 2 GBA");
}

/**
 * Tests \c sfx::fonts::load() and \c sfx::fonts::save().
 */
TEST_F(FontsTest, JSONLoadSave) {
	fontstest.save(getTestAssetPath("fonts/fonts_save.json"));
	sfx::fonts savetest;
	savetest.load(getTestAssetPath("fonts/fonts_save.json"));
	ASSERT_NE(savetest["dialogue"], nullptr);
	EXPECT_EQ(savetest["dialogue"]->getInfo().family, "Advance Wars 2 GBA");
}