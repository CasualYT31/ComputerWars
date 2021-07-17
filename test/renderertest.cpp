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

/**@file renderertest.h
 * Tests the \c sfx::renderer class.
 */

#include "sharedfunctions.h"
#include "renderer.h"

/**
 * This test fixture is used to initialise a \c renderer object for testing.
 */
class RendererTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the object before all tests.
	 * Also prepares that script before the first test commences.
	 */
	void SetUp() override {
		if (isTest({ "LoadValidScript" })) setupRendererJSONScript();
		// always load the script at the beginning of every test
		window.load(getTestAssetPath("renderer/renderer.json"));
	}

	/**
	 * The \c renderer object to test on.
	 */
	sfx::renderer window;
};

/**
 * Tests loading a valid JSON script to configure a \c renderer object.
 */
TEST_F(RendererTest, LoadValidScript) {
	EXPECT_EQ(window.getSettings().caption, "Computer Wars");
}

/**
 * Tests loading an invalid JSON script to configure a \c renderer object.
 * Properties containing valid values should overwrite, and those that don't
 * contain valid values shouldn't.
 */
TEST_F(RendererTest, LoadInvalidScript) {
	EXPECT_FALSE(window.getSettings().style.vsync);
	window.load(getTestAssetPath("renderer/faultyrenderer.json"));
	EXPECT_EQ(window.getSettings().caption, "Computer Wars");
	EXPECT_TRUE(window.getSettings().style.vsync);
}

#ifdef COMPUTER_WARS_FULL_RENDERER_TESTING

/**
 * Tests \c sfx::renderer::openWindow() and \c sfx::renderer::setSettings().
 */
TEST_F(RendererTest, OpenAndChangeWindow) {
	// openWindow
	window.openWindow();
	EXPECT_EQ(window.getSize().x, 1408);
	EXPECT_EQ(window.getSize().y, 795);
	longWait("Now opened window...");
	// change some properties on the fly
	EXPECT_EQ(window.getPosition().x, 235);
	sfx::renderer_settings newSettings = window.getSettings();
	newSettings.x = 50;
	window.setSettings(newSettings);
	EXPECT_EQ(window.getPosition().x, 50);
	longWait("Now moved window...");
}

/**
 * Tests \c sfx::renderer::save().
 */
TEST_F(RendererTest, SaveScript) {
	sfx::renderer_settings newSettings = window.getSettings();
	newSettings.x = 50;
	window.setSettings(newSettings);
	window.save();
	newSettings.x = 4;
	window.setSettings(newSettings);
	window.load();
	EXPECT_EQ(window.getSettings().x, 50);
}

#endif