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

#include "tests.h"
#include "language.h"
#include "fonts.h"
#include "audio.h"
#include "renderer.h"
#include <filesystem>
#include <iostream>

//*********************
//*TESTING ENTRY POINT*
//*********************
int test::test() {
	// setup the test cases
	std::string path = "./test/results/";
	std::vector<test::test_case*> testcases;
	testcases.push_back(new test::test_logger(path));
	testcases.push_back(new test::test_language(path));
	testcases.push_back(new test::test_safejson(path));
	testcases.push_back(new test::test_uuid(path));
	testcases.push_back(new test::test_fonts(path));
	testcases.push_back(new test::test_audio(path));
	testcases.push_back(new test::test_renderer(path));

	// run the test cases
	for (auto itr = testcases.begin(), enditr = testcases.end(); itr != enditr; itr++) {
		(*itr)->runTests();
		delete *itr;
	}
	return 0;
}

//****************
//*LOGGER.H TESTS*
//****************
test::test_logger::test_logger(const std::string& path) noexcept : test_case(path + "logger_test_case.log") {}

void test::test_logger::runTests() noexcept {
	RUN_TEST(test::test_logger::sink);
	RUN_TEST(test::test_logger::logger);
	endTesting();
}

void test::test_logger::sink() {
	// first Get should actually create the file, second should not
	auto firstLog = global::sink::Get("Tests", "Dev", "./test/results/", false);
	auto secondLog = global::sink::Get("Test Again", "Developer", "./test/", false);
	ASSERT_EQUAL(firstLog, secondLog);
	bool firstLogFileExists = std::filesystem::exists("./test/results/Log.log");
	bool secondLogFileExists = std::filesystem::exists("./test/Log.log");
	ASSERT_TRUE(firstLogFileExists);
	ASSERT_FALSE(secondLogFileExists);
	// now test the properties
	ASSERT_EQUAL(global::sink::ApplicationName(), "Tests");
	ASSERT_EQUAL(global::sink::DeveloperName(), "Dev");
	ASSERT_EQUAL(global::sink::GetYear(), "2021"); // obviously test is dependent on year of execution...
	// has the file been written as expected so far?
	// also implicitly tests that GetLog() is working as expected
	std::string file = global::sink::GetLog();
	std::string firstLine = file.substr(0, file.find('\n'));
	ASSERT_EQUAL(firstLine, "Tests © 2021 Dev");
}

void test::test_logger::logger() {
	global::logger log("logger_test");
	// test simple writes, errors, and warnings
	log.write("Hello World!");
	log.warning("We are currently testing!");
	log.error("Oh no!");
	// test variable writes, errors, and warnings
	int simple_int = 8;
	std::string text = "Inserted";
	double f_number = -79.5;
	bool boolean = true;
	log.write("Number = {}", simple_int);
	log.warning("{} text, {} = number", text, f_number);
	log.error("Error is {}!", boolean);
	// now search the log file to see if all of the previous writes were written as expected
	std::string logFile = global::sink::GetLog();
	ASSERT_NOT_EQUAL(logFile.find("[info] Hello World!"), std::string::npos);
	ASSERT_NOT_EQUAL(logFile.find("[warning] We are currently testing!"), std::string::npos);
	ASSERT_NOT_EQUAL(logFile.find("[error] Oh no!"), std::string::npos);
	ASSERT_NOT_EQUAL(logFile.find("[info] Number = 8"), std::string::npos);
	ASSERT_NOT_EQUAL(logFile.find("[warning] Inserted text, -79.5 = number"), std::string::npos);
	ASSERT_NOT_EQUAL(logFile.find("[error] Error is true!"), std::string::npos);
}

//******************
//*SAFEJSON.H TESTS*
//******************
test::test_safejson::test_safejson(const std::string& path) noexcept : test_case(path + "safejson_test_case.log") {}

void test::test_safejson::runTests() noexcept {
	RUN_TEST(test::test_safejson::json);
	endTesting();
}

void test::test_safejson::json() {
	// test empty json object
	safe::json j(std::string("name:test_json"));
	ASSERT_FALSE(j.keysExist({ "test", "test" }));
	ASSERT_FALSE(j.keysExist({}));
	// test apply() NO_KEYS_GIVEN
	int holder = 0;
	j.apply(holder, {});
	ASSERT_TRUE(j.whatFailed() & safe::json_state::NO_KEYS_GIVEN);
	j.resetState();
	// test apply() KEYS_DID_NOT_EXIST
	j.apply(holder, { "test" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::KEYS_DID_NOT_EXIST);
	j.resetState();
	// test assignment operator
	j = R"(
	{
		"pi": 3.141,
		"happy": true,
		"name": "John",
		"nothing": null,
		"answer": {
			"everything": 42
		},
		"list": [1, 0, 2],
		"object": {
			"currency": "USD",
			"value": -42
		}
	}
	)"_json;
	ASSERT_FALSE(j.keysExist({ "test" }));
	ASSERT_TRUE(j.keysExist({ "object", "value" }));
	// test apply() MISMATCHING_TYPE
	j.apply(holder, { "happy" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::MISMATCHING_TYPE);
	j.resetState();
	j.apply(holder, { "pi" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::MISMATCHING_TYPE);
	j.resetState();
	// test apply
	j.apply(holder, { "answer", "everything" });
	ASSERT_TRUE(j.inGoodState());
	ASSERT_EQUAL(holder, 42);
	ASSERT_NAME_IN_LOG("name:test_json");
	// test applyArray() MISMATCHING_SIZE
	std::array<int, 2> holderArrayError;
	j.applyArray(holderArrayError, { "list" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::MISMATCHING_SIZE);
	j.resetState();
	std::array<int, 4> holderArrayTooBig;
	j.applyArray(holderArrayTooBig, { "list" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::MISMATCHING_SIZE);
	j.resetState();
	// test applyArray() MISMATCHING_ELEMENT_TYPE
	std::array<std::string, 3> holderArrayBadType;
	j.applyArray(holderArrayBadType, { "list" });
	ASSERT_TRUE(j.whatFailed() & safe::json_state::MISMATCHING_ELEMENT_TYPE);
	j.resetState();
	// test applyArray()
	std::array<int, 3> holderArray;
	j.applyArray(holderArray, { "list" });
	ASSERT_TRUE(j.inGoodState());
	ASSERT_EQUAL(holderArray[2], 2);
	// test applyColour()
	j = R"({
		"colour": [255, 180, 255, 255]
	})"_json;
	sf::Color recipient;
	j.applyColour(recipient, { "colour" });
	ASSERT_TRUE(j.inGoodState());
	ASSERT_EQUAL(recipient.r, 255);
	ASSERT_EQUAL(recipient.g, 180);
}

//******************
//*LANGUAGE.H TESTS*
//******************
test::test_language::test_language(const std::string& path) noexcept : test_case(path + "language_test_case.log") {}

void test::test_language::runTests() noexcept {
	RUN_TEST(test::test_language::expand_string);
	RUN_TEST(test::test_language::language_dictionary);
	RUN_TEST(test::test_language::language_dictionary_json);
	endTesting();
}

void test::test_language::expand_string() {
	// expand_string is a pretty easy class to test,
	// but there are a lot of cases to cover:
	// 1. test with default var char
	// a. test no variables, no var chars
	// b. test no variables, 1 var char
	// c. test no variables, 2 var chars
	// d. test 2 variables, no var chars
	// e. test 2 variables, 1 var char
	// f. test 2 variables, 2 var chars
	// g. test 2 variables, 3 var chars
	// h. test 3 variables, 2 var chars next to each other
	// i. test 3 variables, 3 sets of 3 var chars next to each other
	// 2. test get and set var char methods
	// 3. repeat tests a-i with a new var char
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '#');
	expand_string_("#");
	i18n::expand_string::setVarChar('$');
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '$');
	expand_string_("$");
	// ENSURE TO REVERT BACK TO THE OLD VAR CHAR TO
	// ENSURE THAT FUTURE TESTS THAT MAY RELY ON IT WORK
	i18n::expand_string::setVarChar('#');
	ASSERT_EQUAL(i18n::expand_string::getVarChar(), '#');
}

void test::test_language::expand_string_(const std::string& var) {
	// see expand_string() ~ this method performs tests a-i
	ASSERT_EQUAL(i18n::expand_string::insert("Hello World!"), "Hello World!");
	ASSERT_EQUAL(i18n::expand_string::insert("Hello" + var + "World!"), "Hello" + var + "World!");
	ASSERT_EQUAL(i18n::expand_string::insert("Hello" + var + "World!" + var), "Hello" + var + "World!" + var);
	ASSERT_EQUAL(i18n::expand_string::insert("var1= var2=", 18, "Test"), "var1= var2=");
	ASSERT_EQUAL(i18n::expand_string::insert("var1=" + var + " var2=", 18, "Test"), "var1=18 var2=");
	ASSERT_EQUAL(i18n::expand_string::insert("var1=" + var + " var2=" + var, -18, "Test"), "var1=-18 var2=Test");
	ASSERT_EQUAL(i18n::expand_string::insert(var + "var1=" + var + " var2=" + var, 0.5, "Testing"), "0.5var1=Testing var2=" + var);
	ASSERT_EQUAL(i18n::expand_string::insert(var + var, true, false, 9.792), var);
	ASSERT_EQUAL(i18n::expand_string::insert(var + var + var + " " + var + var + var + " " + var + var + var, 34, "LLL", 9.792),
		                                     var + "34 " + var + "LLL " + var + "9.792");
}

void test::test_language::language_dictionary() {
	i18n::language_dictionary dict("name:test_dictionary");
	// test behaviour when dictionary is empty
	ASSERT_FALSE(dict.removeLanguage("test"));
	ASSERT_FALSE(dict.removeLanguage(""));
	ASSERT_FALSE(dict.setLanguage("testing"));
	ASSERT_EQUAL(dict.getLanguage(), "");
	ASSERT_EQUAL(dict("Native String"), "Native String");
	ASSERT_TRUE(dict.setLanguage(""));
	ASSERT_EQUAL(dict("Native String"), "Native String");
	// now test addLanguage and load individual languages
	ASSERT_TRUE(dict.addLanguage("test", "bad_path.json"));
	ASSERT_FALSE(dict.addLanguage("", "test/assets/lang/ENG_GB.json"));
	ASSERT_FALSE(dict.setLanguage("test"));
	ASSERT_TRUE(dict.addLanguage("test", "test/assets/lang/ENG_GB.json"));
	ASSERT_NOT_EQUAL(dict("day", 10), "Day 10");
	ASSERT_TRUE(dict.setLanguage("test"));
	ASSERT_EQUAL(dict("day", 10), "Day 10");
	ASSERT_EQUAL(dict("greeting", 10), "Hello, World!");
	ASSERT_EQUAL(dict.getLanguage(), "test");
	ASSERT_FALSE(dict.addLanguage("test", "test/assets/lang/GER_DE.json"));
	ASSERT_TRUE(dict.addLanguage("other", "test/assets/lang/GER_DE.json"));
	ASSERT_TRUE(dict.setLanguage("other"));
	ASSERT_EQUAL(dict("cancel"), "Stornieren");
	ASSERT_EQUAL(dict.getLanguage(), "other");
	ASSERT_TRUE(dict.setLanguage("test"));
	ASSERT_EQUAL(dict("cancel"), "Cancel");
	ASSERT_NAME_IN_LOG("name:test_dictionary");
}

void test::test_language::language_dictionary_json() {
	// do some json_script generic tests
	// test non-existent file
	i18n::language_dictionary jsonscripttest;
	jsonscripttest.load("file");
	ASSERT_TRUE(jsonscripttest.whatFailed() & safe::json_state::FAILED_SCRIPT_LOAD);
	// instantiate a fresh language_dictionary object and test the json_script methods
	// common approach for json_script class:
	// ensure load() works and that it completely replaces the state of the object as required
	// ensure that save() writes a JSON script as necessary in the correct format,
	// this can easily be tested by using the verified load() method.
	i18n::language_dictionary dict_js("test_dict_json_script");
	dict_js.load("test/assets/lang/lang.json");
	ASSERT_EQUAL(dict_js.getLanguage(), "ENG_GB");
	ASSERT_EQUAL(dict_js("language"), "English");
	ASSERT_TRUE(dict_js.setLanguage("GER_DE"));
	ASSERT_EQUAL(dict_js("language"), "Deutsch");
	ASSERT_TRUE(dict_js.setLanguage(""));
	ASSERT_EQUAL(dict_js("language"), "language");
	dict_js.save();
	ASSERT_TRUE(dict_js.setLanguage("ENG_GB"));
	dict_js.load();
	ASSERT_EQUAL(dict_js.getLanguage(), "");
	ASSERT_TRUE(dict_js.setLanguage("ENG_GB"));
	dict_js.save();
}

//**************
//*UUID.H TESTS*
//**************
test::test_uuid::test_uuid(const std::string& path) noexcept : test_case(path + "uuid_test_case.log"), ID(UUID_INIT) {}

void test::test_uuid::runTests() noexcept {
	RUN_TEST(test::test_uuid::uuid);
	endTesting();
}

void test::test_uuid::uuid() {
	// test init in constructor and getID()
	ASSERT_EQUAL(ID.getID(), UUID_INIT);
	// test operator== and operator!=
	test::test_uuid& reference = *this;
	ASSERT_TRUE(ID == reference.ID);
	ASSERT_FALSE(ID != reference.ID);
	// wrapping, and operators again
	engine::uuid<test::test_uuid> anotherID;
	ASSERT_EQUAL(anotherID.getID(), 0);
	ASSERT_FALSE(ID == anotherID);
	ASSERT_TRUE(ID != anotherID);
	engine::uuid<test::test_uuid> yetAnotherID;
	ASSERT_EQUAL(yetAnotherID.getID(), 1);
}

//***************
//*FONTS.H TESTS*
//***************
test::test_fonts::test_fonts(const std::string& path) noexcept : test_case(path + "fonts_test_case.log") {}

void test::test_fonts::runTests() noexcept {
	RUN_TEST(test::test_fonts::fonts);
	endTesting();
}

void test::test_fonts::fonts() {
	sfx::fonts fontstest;
	// test behaviour when fonts object is empty
	ASSERT_EQUAL(fontstest["test"], nullptr);
	// test logging
	ASSERT_NAME_IN_LOG("fonts");
	// now test load() ~ non-existent file
	fontstest.load("badfile.json");
	ASSERT_TRUE(fontstest.whatFailed() & safe::json_state::FAILED_SCRIPT_LOAD);
	fontstest.resetState();
	// test load() ~ existent file
	fontstest.load("test/assets/fonts/fonts.json");
	ASSERT_EQUAL(fontstest["dialogue"]->getInfo().family, "Advance Wars 2 GBA");
	// test load() ~ non-existent file ~ ensure that state isn't overwritten (it shouldn't in this case)
	fontstest.load("anotherbadfile.json");
	ASSERT_TRUE(fontstest.whatFailed() & safe::json_state::FAILED_SCRIPT_LOAD);
	fontstest.resetState();
	ASSERT_EQUAL(fontstest["dialogue"]->getInfo().family, "Advance Wars 2 GBA");
	// test load() ~ faulty file ~ ensure that state is overwritten (as per documentation)
	fontstest.load("test/assets/fonts/faultyfonts.json");
	fontstest.resetState();
	ASSERT_EQUAL(fontstest["dialogue"], nullptr);
	// test load() ~ existent file ~ ensure that state is overwritten
	fontstest.load("test/assets/fonts/fonts.json");
	ASSERT_NOT_EQUAL(fontstest["dialogue"], nullptr);
	fontstest.load("test/assets/fonts/otherfonts.json");
	ASSERT_EQUAL(fontstest["dialogue"], nullptr);
	ASSERT_EQUAL(fontstest["text"]->getInfo().family, "Advance Wars 2 GBA");
	// test save() ~ load() that script and test accordingly
	fontstest.save("test/assets/fonts/fonts_save.json");
	sfx::fonts savetest;
	savetest.load("test/assets/fonts/fonts_save.json");
	ASSERT_EQUAL(savetest["text"]->getInfo().family, "Advance Wars 2 GBA");
}

//***************
//*AUDIO.H TESTS*
//***************
test::test_audio::test_audio(const std::string& path) noexcept : test_case(path + "audio_test_case.log") {}

void test::test_audio::runTests() noexcept {
	RUN_TEST(test::test_audio::audio);
	endTesting();
}

void test::test_audio::audio() {
	sfx::audio audio;
	// test valid load() script
	audio.load("test/assets/audio/audio.json");
	audio.play("jake");
	audio.pause();
	ASSERT_EQUAL(audio.getCurrentMusic(), "jake");
	// test faulty load() script ~ state should be reset
	audio.load("test/assets/audio/faultyaudio.json");
	audio.resetState();
	ASSERT_NAME_IN_LOG("audio");
	audio.play("jake");
	ASSERT_EQUAL(audio.getCurrentMusic(), "");
	audio.load("test/assets/audio/audio.json");
	// test getVolume()
	ASSERT_TRUE(audio.getVolume() < 101.0 && audio.getVolume() > 99.0);
	// test setVolume() and play()
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.setVolume(-50.0);
	ASSERT_TRUE(audio.getVolume() > -1.0 && audio.getVolume() < 1.0);
	longWait("Set volume to... 0.0.");
	audio.setVolume(500.0);
	ASSERT_TRUE(audio.getVolume() < 101.0 && audio.getVolume() > 99.0);
	longWait("Set volume to... 100.0.");
	audio.setVolume(50.0);
	longWait("Set volume to... 50.0.");
	// test pause()
	audio.pause();
	ASSERT_EQUAL(audio.getCurrentMusic(), "noco");
	longWait("Now paused...");
	audio.play();
	longWait("Now playing...");
	// test stop()
	audio.stop();
	ASSERT_EQUAL(audio.getCurrentMusic(), "");
	longWait("Now stopped...");
	audio.play("noco");
	longWait("Now playing... noco.");
	// test fadeout() and granularity stuff
	std::cout << "Now fading out for... 3 seconds. With granularity... " << audio.getGranularity() << std::endl;
	while (!audio.fadeout(sf::seconds(3.0)));
	shortWait("");
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.setGranularity(50.0);
	std::cout << "Now fading out for... 3 seconds. With granularity... " << audio.getGranularity() << std::endl;
	while (!audio.fadeout(sf::seconds(3.0)));
	shortWait("");
	// test music playing behaviour
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.play("jake");
	longWait("Now playing... jake.");
	audio.pause("jake");
	longWait("Now pausing...");
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.play("jake"); // should start from the beginning again despite being paused previously
	longWait("Now playing... jake.");
	// test sound playing behaviour
	longWait("Now testing sounds...");
	audio.play("load");
	shortWait("");
	audio.play("unload");
	shortWait("");
	audio.play("load");
	audio.play("unload");
	audio.stop();
	shortWait("Audio playback testing complete!");
	// test save()
	audio.save();
	audio.setVolume(100.0);
	audio.load();
	ASSERT_TRUE(audio.getVolume() > 49.0 && audio.getVolume() < 51.0);
	audio.setVolume(100.0); // tests will expect 100.0 volume at start
	audio.save();
}

void test::test_audio::longWait(const std::string& msg) noexcept {
	std::cout << msg << " Waiting... 3 seconds." << std::endl;
	return;
	sf::Clock timer;
	while (timer.getElapsedTime().asSeconds() < 3.0);
}

void test::test_audio::shortWait(const std::string& msg) noexcept {
	std::cout << msg << " Waiting... 1 second." << std::endl;
	return;
	sf::Clock timer;
	while (timer.getElapsedTime().asSeconds() < 3.0);
}

//******************
//*RENDERER.H TESTS*
//******************
test::test_renderer::test_renderer(const std::string& path) noexcept : test_case(path + "renderer_test_case.log") {}

void test::test_renderer::runTests() noexcept {
	RUN_TEST(test::test_renderer::renderer);
	endTesting();
}

void test::test_renderer::renderer() {
	sfx::renderer window;
	// test loading valid script
	window.load("test/assets/renderer/renderer.json");
	ASSERT_EQUAL(window.getSettings().caption, "Computer Wars");
	// test loading faulty script ~ some properties should overwrite and others shouldn't
	window.load("test/assets/renderer/faultyrenderer.json");
	ASSERT_EQUAL(window.getSettings().caption, "Computer Wars");
	window.load("test/assets/renderer/renderer.json");
	// openWindow
	window.openWindow();
	ASSERT_EQUAL(window.getSize().x, 1408);
	ASSERT_EQUAL(window.getSize().y, 795);
	// change some properties on the fly
	sfx::renderer_settings newSettings = window.getSettings();
	newSettings.x = 50;
	window.setSettings(newSettings);
	ASSERT_EQUAL(window.getPosition().x, 50);
	// test saving
	window.save();
	newSettings.x = 4;
	window.setSettings(newSettings);
	window.load();
	ASSERT_EQUAL(window.getSettings().x, 50);
	newSettings.x = 235;
	window.setSettings(newSettings);
	window.save();
}