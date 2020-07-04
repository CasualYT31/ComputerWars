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

#include "renderer.h"

void sfx::renderer::openWindow(const sf::ContextSettings& settings) noexcept {
	create(sf::VideoMode(width, height), caption, 
		(style.close ? sf::Style::Close : 0) |
		(style.def ? sf::Style::Default : 0) |
		(style.fullscreen ? sf::Style::Fullscreen : 0) |
		(style.none ? sf::Style::None : 0) |
		(style.resize ? sf::Style::Resize : 0) |
		(style.titlebar ? sf::Style::Titlebar : 0),
		settings);
	setPosition(sf::Vector2i(x, y));
	setFramerateLimit(framerate);
	setVerticalSyncEnabled(style.vsync);
	setMouseCursorVisible(style.mouseVisible);
	setMouseCursorGrabbed(style.mouseGrabbed);
}

bool sfx::renderer::_load(safe::json& j) noexcept {
	j.apply(width, { "width" }, &width, true);
	j.apply(height, { "height" }, &height, true);
	j.apply(x, { "x" }, &x, true);
	j.apply(y, { "y" }, &y, true);
	j.apply(framerate, { "framerate" }, &framerate, true);
	j.apply(caption, { "caption" }, &caption, true);
	j.apply(style.close, { "close" }, &style.close, true);
	j.apply(style.def, { "def" }, &style.def, true);
	j.apply(style.fullscreen, { "fullscreen" }, &style.fullscreen, true);
	j.apply(style.none, { "none" }, &style.none, true);
	j.apply(style.resize, { "resize" }, &style.resize, true);
	j.apply(style.titlebar, { "titlebar" }, &style.titlebar, true);
	j.apply(style.vsync, { "vsync" }, &style.vsync, true);
	j.apply(style.mouseVisible, { "cursor" }, &style.mouseVisible, true);
	j.apply(style.mouseGrabbed, { "grabbedmouse" }, &style.mouseGrabbed, true);
	return true;
}

bool sfx::renderer::_save(nlohmann::json& j) noexcept {
	width = getSize().x;
	height = getSize().y;
	x = getPosition().x;
	y = getPosition().y;
	j["width"] = width;
	j["height"] = height;
	j["x"] = x;
	j["y"] = y;
	j["framerate"] = framerate;
	j["caption"] = caption;
	j["close"] = style.close;
	j["def"] = style.def;
	j["fullscreen"] = style.fullscreen;
	j["none"] = style.none;
	j["resize"] = style.resize;
	j["titlebar"] = style.titlebar;
	j["vsync"] = style.vsync;
	j["cursor"] = style.mouseVisible;
	j["grabbedmouse"] = style.mouseGrabbed;
	return true;
}

float sfx::animated_drawable::calculateDelta() noexcept {
	return _deltaTimer.restart().asSeconds();
}