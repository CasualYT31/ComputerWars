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

/**@file languagetest.h
 * Tests the \c engine::language_dictionary class.
 */

#include "sharedfunctions.h"
#include "language.h"
#include <fstream>

/**
 * This test fixture is used to initialise \c language_dictionary objects with a
 * name for the log file before testing.
 * It also prepares any scripts required for testing.
 */
class LanguageTest : public ::testing::Test {
protected:
	/**
	 * Prepares the lang/lang.json test script before the JSONLoadSave test.
	 */
	void SetUp() override {
		if (isTest({ "JSONLoadSave" })) {
			nlohmann::json j = R"({
				"lang": "ENG_GB",
				"GER_DE": "",
				"ENG_GB": ""
			})"_json;
			// retrieve the correct paths to each script at runtime
			j["GER_DE"] = getTestAssetPath("lang/GER_DE.json");
			j["ENG_GB"] = getTestAssetPath("lang/ENG_GB.json");
			// save the json script
			// if the test script can't be written, then the rest of the test won't
			// work, so use assert here
			ASSERT_NO_THROW({
				std::ofstream jscript(getTestAssetPath("lang/lang.json"),
					std::ios_base::trunc);
				jscript << j;
				jscript.close();
			});
		}
	}

	/**
	 * The \c language_dictionary object to test on.
	 */
	engine::language_dictionary dict =
		engine::language_dictionary(std::string("name:test_dictionary"));
};

/**
 * Test behaviour of a \c language_dictionary when it is empty.
 */
TEST_F(LanguageTest, EmptyDictionary) {
	EXPECT_FALSE(dict.removeLanguage("test"));
	EXPECT_FALSE(dict.removeLanguage(""));
	EXPECT_FALSE(dict.setLanguage("testing"));
	EXPECT_EQ(dict.getLanguage(), "");
	EXPECT_EQ(dict("Native String"), "Native String");
	EXPECT_TRUE(dict.setLanguage(""));
	EXPECT_EQ(dict("Native String"), "Native String");
}

/**
 * Test behaviour of a \c language_dictionary when individual languages are loaded.
 */
TEST_F(LanguageTest, LoadDictionaries) {
	EXPECT_TRUE(dict.addLanguage("test", "bad_path.json"));
	EXPECT_FALSE(dict.addLanguage("", getTestAssetPath("lang/ENG_GB.json")));
	EXPECT_FALSE(dict.setLanguage("test"));
	EXPECT_TRUE(dict.addLanguage("test", getTestAssetPath("lang/ENG_GB.json")));
	EXPECT_NE(dict("day", 10), "Day 10");
	EXPECT_TRUE(dict.setLanguage("test"));
	EXPECT_EQ(dict("day", 10), "Day 10");
	EXPECT_EQ(dict("greeting", 10), "Hello, World!");
	EXPECT_EQ(dict.getLanguage(), "test");
	EXPECT_FALSE(dict.addLanguage("test", getTestAssetPath("lang/GER_DE.json")));
	EXPECT_TRUE(dict.addLanguage("other", getTestAssetPath("lang/GER_DE.json")));
	EXPECT_TRUE(dict.setLanguage("other"));
	EXPECT_EQ(dict("cancel"), "Stornieren");
	EXPECT_EQ(dict.getLanguage(), "other");
	EXPECT_TRUE(dict.setLanguage("test"));
	EXPECT_EQ(dict("cancel"), "Cancel");
	EXPECT_IN_LOG("name:test_dictionary");
}

/**
 * Performs a generic \c engine::json_script test, where a non-existent file is
 * attempted to be loaded.
 */
TEST_F(LanguageTest, JSONScriptNonExist) {
	engine::language_dictionary jsonscripttest;
	jsonscripttest.load("file");
	EXPECT_TRUE(
		jsonscripttest.whatFailed() & engine::json_state::FAILED_SCRIPT_LOAD);
}

/**
 * Test behaviour of \c engine::language_dictionary::load() and
 * \c engine::language_dictionary::save().
 */
TEST_F(LanguageTest, JSONLoadSave) {
	// instantiate a fresh language_dictionary object and test the json_script
	// methods
	// common approach for json_script class:
	// ensure load() works and that it completely replaces the state of the object
	// as required
	// ensure that save() writes a JSON script as necessary in the correct format,
	// this can easily be tested by using the verified load() method.
	dict.load(getTestAssetPath("lang/lang.json"));
	EXPECT_EQ(dict.getLanguage(), "ENG_GB");
	EXPECT_EQ(dict("language"), "English");
	EXPECT_TRUE(dict.setLanguage("GER_DE"));
	EXPECT_EQ(dict("language"), "Deutsch");
	EXPECT_TRUE(dict.setLanguage(""));
	EXPECT_EQ(dict("language"), "language");
	dict.save();
	EXPECT_TRUE(dict.setLanguage("ENG_GB"));
	dict.load();
	EXPECT_EQ(dict.getLanguage(), "");
	EXPECT_TRUE(dict.setLanguage("ENG_GB"));
	dict.save();
}