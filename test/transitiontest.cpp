/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file transitiontest.hpp
 * Tests the \c transition classes.
 */

#include "sharedfunctions.hpp"
#include "transitions.hpp"

#ifdef COMPUTER_WARS_FULL_TRANSITION_TESTING

/**
 * Tests \c transition::rectangle.
 */
TEST(TransitionTest, Rectangle) {
	setupRendererJSONScript();
	sfx::renderer window;
	window.load(getTestAssetPath("renderer/renderer.json"));
	transition::rectangle in(false, sf::seconds(4));
	window.openWindow();
	for (;;) {
		window.clear(sf::Color::White);
		if (window.animate(in)) break;
		window.draw(in);
		window.display();
	}
	// transition should use up same amount of time regardless of framerate
	window.setFramerateLimit(5);
	transition::rectangle out(true, sf::seconds(4));
	for (;;) {
		window.clear(sf::Color::White);
		if (window.animate(out)) break;
		window.draw(out);
		window.display();
	}
}

#endif