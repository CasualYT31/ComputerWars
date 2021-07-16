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

/**@file texturetest.h
 * Tests the \c sfx::animated_spritesheet class.
 */

#include "sharedfunctions.h"
#include "texture.h"

/**
 * This test fixture is used to initialise testing objects.
 */
class TextureTest : public ::testing::Test {
protected:
	/**
	 * Prepares and loads configuration scripts for all objects before all tests.
	 */
	void SetUp() override {
		if (isTest({ "LoadValidScript" })) {
			setupRendererJSONScript();
			// setup the standard texture test script
			setupJSONScript([](nlohmann::json& j) {
				j["path"] = getTestAssetPath("sprites/sheet.png");
				j["frames"] = 1;
				j["framerate"] = 0.0;
				j["sprites"] = R"([
					[0,0,100,100],
					[100,0,100,100],
					[0,100,100,100],
					[100,100,100,100]
				])"_json;
			}, "sprites/sheet.json");
			// setup the multi-frame texture test script
			setupJSONScript([](nlohmann::json& j) {
				j["path"] = getTestAssetPath("sprites/sheet.png");
				j["frames"] = 6;
				j["framerate"] = 0.0;
				j["sprites"] = R"([
					[0,0,200,200]
				])"_json;
			}, "sprites/multi.json");
			// setup the animated texture test script
			setupJSONScript([](nlohmann::json& j) {
				j["path"] = getTestAssetPath("sprites/sheet.png");
				j["frames"] = 6;
				j["framerate"] = 1.0;
				j["sprites"] = R"([
					[0,0,200,200]
				])"_json;
			}, "sprites/ani.json");
		}
		// always load the texture script at the beginning of every test
		if (isTest({ "AnimatedSprite" })) {
			sheet->load(getTestAssetPath("sprites/ani.json"));
		} else if (isTest({ "ManualFrameSelection" })) {
			sheet->load(getTestAssetPath("sprites/multi.json"));
		} else {
			sheet->load(getTestAssetPath("sprites/sheet.json"));
		}
		// load the renderer script at the beginning of most tests
		if (!isTest({ "LoadValidScript", "LoadInvalidScript" })) {
			window.load(getTestAssetPath("renderer/renderer.json"));
		}
		// setup animated_sprite
		sprite.setSpritesheet(sheet);
		sprite.setSprite(0);
	}

	/**
	 * The \c animated_spritehseet object to test.
	 */
	std::shared_ptr<sfx::animated_spritesheet> sheet =
		std::make_shared<sfx::animated_spritesheet>();

	/**
	 * The \c renderer object to test sprites with.
	 */
	sfx::renderer window;

	/**
	 * The animated sprite object to test with.
	 */
	sfx::animated_sprite sprite;

	/**
	 * A timer object.
	 */
	sf::Clock timer;
};

/**
 * Tests the behaviour of \c sfx::animated_spritesheet::load().
 */
TEST_F(TextureTest, LoadValidScript) {
	EXPECT_NO_THROW(sheet->accessTexture(0));
	EXPECT_THROW(sheet->accessTexture(1), std::out_of_range);
}

/**
 * Tests the behaviour of loading an invalid script.
 * The state of the \c animated_spritesheet object should be retained in case the
 * \c path key was invalid.
 */
TEST_F(TextureTest, LoadInvalidScript) {
	sheet->load(getTestAssetPath("sprites/faultysheet.json"));
	EXPECT_NO_THROW(sheet->accessTexture(0));
}

#ifdef COMPUTER_WARS_FULL_TEXTURE_TESTING

/**
 * Tests the behaviour of ordinary sprites.
 */
TEST_F(TextureTest, OrdinarySprites) {
	window.openWindow();
	timer.restart();
	for (;;) {
		window.clear();
		window.animate(sprite);
		window.draw(sprite);
		window.display();
		if (timer.getElapsedTime().asSeconds() >= 1.0) {
			if (sprite.getSprite() == 3) break;
			sprite.setSprite(sprite.getSprite() + 1);
			timer.restart();
		}
	}
}

/**
 * Tests the behaviour of an animated sprite.
 */
TEST_F(TextureTest, AnimatedSprite) {
	window.openWindow();
	timer.restart();
	while (timer.getElapsedTime().asSeconds() < 7.0) {
		window.clear();
		window.animate(sprite);
		window.draw(sprite, sf::RenderStates().transform.translate(50.0, 50.0));
		window.display();
	}
}

/**
 * Tests the behaviour of manually selecting a sprite's frame.
 */
TEST_F(TextureTest, ManualFrameSelection) {
	bool flip = false;
	window.openWindow();
	timer.restart();
	for (;;) {
		window.clear();
		window.animate(sprite);
		window.draw(sprite, sf::RenderStates().transform.scale(1.25, 1.25));
		window.display();
		if (timer.getElapsedTime().asSeconds() >= 1.0) {
			if (sprite.getCurrentFrame() == 5) flip = true;
			if (flip) {
				if (--sprite == 5) break;
			} else {
				sprite++;
			}
			timer.restart();
		}
	}
}

#endif