/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file audiotest.hpp
 * Tests the \c sfx::audio class.
 */

#include "sharedfunctions.hpp"
#include "audio.hpp"

/**
 * This test fixture is used to initialise an \c audio object for testing.
 */
class AudioTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the object before certain tests.
	 * Also prepares that script before the first test commences.
	 */
	void SetUp() override {
		if (isTest({ "LoadValidScript" })) {
			// prepare audio.json only once
			setupJSONScript([](nlohmann::json& j) {
				// retrieve the correct paths to the audio files at runtime
				j["volume"] = 100.0;
				j["load"]["type"] = "sound";
				j["load"]["path"] = getTestAssetPath("audio/load.wav");
				j["load"]["offset"] = 0.0;
				j["unload"]["type"] = "sound";
				j["unload"]["path"] = getTestAssetPath("audio/unload.wav");
				j["unload"]["offset"] = 0.0;
				j["noco"]["type"] = "music";
				j["noco"]["path"] = getTestAssetPath("audio/NoCO.ogg");
				j["noco"]["offset"] = 0.0;
				j["noco"]["loopto"] = 5400;
				j["noco"]["loopwhen"] = 69335;
				j["jake"]["type"] = "music";
				j["jake"]["path"] = getTestAssetPath("audio/Jake.ogg");
				j["jake"]["offset"] = 1.0;
				j["jake"]["loopto"] = 0;
				j["jake"]["loopwhen"] = 90706;
			}, "audio/audio.json");
		}
		// always load the script at the beginning of every test
		audio.load(getTestAssetPath("audio/audio.json"));
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
	audio.play("jake");
	audio.pause();
	EXPECT_EQ(audio.getCurrentMusic(), "jake");
}

/**
 * Tests the behaviour of <tt>audio::load()</tt>ing an invalid script.
 * It should reset the state of the \c audio object.
 */
TEST_F(AudioTest, LoadInvalidScript) {
	audio.load(getTestAssetPath("audio/faultyaudio.json"));
	audio.resetState();
	EXPECT_IN_LOG("audio");
	audio.play("jake");
	EXPECT_EQ(audio.getCurrentMusic(), "");
}

/**
 * Test \c sfx::audio::getVolume().
 */
TEST_F(AudioTest, GetVolume) {
	EXPECT_FLOAT_EQ(audio.getVolume(), 100.0);
}

#ifdef COMPUTER_WARS_FULL_AUDIO_TESTING

/**
 * Tests the \c sfx::audio object as whole.
 * This function tests \c setVolume(), \c play(), \c pause(), \c stop(), and
 * \c fadeout().
 */
TEST_F(AudioTest, NormalOperation) {
	// test setVolume() and play()
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.setVolume(-50.0);
	EXPECT_FLOAT_EQ(audio.getVolume(), 0.0);
	longWait("Set volume to... 0.0.");
	audio.setVolume(500.0);
	EXPECT_FLOAT_EQ(audio.getVolume(), 100.0);
	longWait("Set volume to... 100.0.");
	audio.setVolume(50.0);
	longWait("Set volume to... 50.0.");
	// test pause()
	audio.pause();
	EXPECT_EQ(audio.getCurrentMusic(), "noco");
	longWait("Now paused...");
	audio.play();
	longWait("Now playing...");
	// test stop()
	audio.stop();
	EXPECT_EQ(audio.getCurrentMusic(), "");
	longWait("Now stopped...");
	audio.play("noco");
	longWait("Now playing... noco.");
	// test fadeout()
	std::cout << "Now fading out for... 3 seconds." << std::endl;
	while (!audio.fadeout(sf::seconds(3.0)));
	shortWait("");
	audio.play("noco");
	longWait("Now playing... noco.");
}

/**
 * Tests the behaviour of playing multiple pieces of music and sound
 * simultaneously.
 */
TEST_F(AudioTest, SimultaneousMusicSound) {
	// test music playing behaviour
	audio.play("noco");
	longWait("Now playing... noco.");
	audio.play("jake");
	longWait("Now playing... jake.");
	audio.pause("jake");
	longWait("Now pausing...");
	audio.play("noco");
	longWait("Now playing... noco.");
	// should start from the beginning again despite being paused previously
	audio.play("jake");
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
}

#endif

/**
 * Tests \c sfx::audio::save().
 */
TEST_F(AudioTest, Save) {
	audio.setVolume(50.0);
	audio.save();
	audio.setVolume(100.0);
	audio.load();
	EXPECT_FLOAT_EQ(audio.getVolume(), 50.0);
}