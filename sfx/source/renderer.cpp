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

#include "renderer.hpp"

sfx::renderer::renderer(const std::string& name) noexcept : _logger(name) {}

void sfx::renderer::openWindow() noexcept {
	sf::VideoMode mode(_settings.width, _settings.height);
	if (_settings.style.fullscreen && !mode.isValid()) {
		_logger.warning("Invalid video mode for fullscreen ({}x{})! Reverting to "
			"windowed mode...", mode.width, mode.height);
		boxer::show("The renderer's configurations contained an invalid width and "
			"height for fullscreen mode. The program has reverted to windowed "
			"mode.", "Error!");
		_settings.style.fullscreen = false;
	}
	create(mode, _settings.caption,
		(_settings.style.close ? sf::Style::Close : 0) |
		(_settings.style.def ? sf::Style::Default : 0) |
		(_settings.style.fullscreen ? sf::Style::Fullscreen : 0) |
		(_settings.style.none ? sf::Style::None : 0) |
		(_settings.style.resize ? sf::Style::Resize : 0) |
		(_settings.style.titlebar ? sf::Style::Titlebar : 0),
		_settings.contextSettings);
	// a funny effect with SFML happens when moving the window in full screen mode
	// you can see it with multiple monitors: it's like a part of the window pops
	// up on the other screen...
	if (!_settings.style.fullscreen)
		setPosition(sf::Vector2i(_settings.x, _settings.y));
	setFramerateLimit(_settings.framerate);
	setVerticalSyncEnabled(_settings.style.vsync);
	setMouseCursorVisible(_settings.style.mouseVisible);
	setMouseCursorGrabbed(_settings.style.mouseGrabbed);
	// apply icon
	if (_settings.iconPath != "") {
		sf::Image icon;
		if (icon.loadFromFile(_settings.iconPath)) {
			setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		} else {
			_logger.warning("Unable to apply icon with path \"{}\" - unable to "
				"load file.", _settings.iconPath);
		}
	}
}

const sfx::renderer_settings& sfx::renderer::getSettings() const noexcept {
	return _settings;
}

void sfx::renderer::setSettings(const sfx::renderer_settings& newSettings)
	noexcept {
	_settings = newSettings;
	openWindow();
}

bool sfx::renderer::animate(sfx::animated_drawable& drawable, const double scaling)
	const noexcept {
	return drawable.animate(*this, scaling);
}

bool sfx::renderer::_load(engine::json& j) noexcept {
	j.apply(_settings.width, { "width" }, true);
	j.apply(_settings.height, { "height" }, true);
	j.apply(_settings.x, { "x" }, true);
	j.apply(_settings.y, { "y" }, true);
	j.apply(_settings.framerate, { "framerate" }, true);
	j.apply(_settings.caption, { "caption" }, true);
	j.apply(_settings.iconPath, { "icon" }, true);
	j.apply(_settings.style.close, { "close" }, true);
	j.apply(_settings.style.def, { "def" }, true);
	j.apply(_settings.style.fullscreen, { "fullscreen" }, true);
	j.apply(_settings.style.none, { "none" }, true);
	j.apply(_settings.style.resize, { "resize" }, true);
	j.apply(_settings.style.titlebar, { "titlebar" }, true);
	j.apply(_settings.style.vsync, { "vsync" }, true);
	j.apply(_settings.style.mouseVisible, { "cursor" }, true);
	j.apply(_settings.style.mouseGrabbed, { "grabbedmouse" }, true);
	return true;
}

bool sfx::renderer::_save(nlohmann::ordered_json& j) noexcept {
	_settings.width = getSize().x;
	_settings.height = getSize().y;
	if (!_settings.style.fullscreen) {
		_settings.x = getPosition().x;
		_settings.y = getPosition().y;
	}
	j["width"] = _settings.width;
	j["height"] = _settings.height;
	j["x"] = _settings.x;
	j["y"] = _settings.y;
	j["framerate"] = _settings.framerate;
	j["caption"] = _settings.caption;
	j["icon"] = _settings.iconPath;
	j["close"] = _settings.style.close;
	j["def"] = _settings.style.def;
	j["fullscreen"] = _settings.style.fullscreen;
	j["none"] = _settings.style.none;
	j["resize"] = _settings.style.resize;
	j["titlebar"] = _settings.style.titlebar;
	j["vsync"] = _settings.style.vsync;
	j["cursor"] = _settings.style.mouseVisible;
	j["grabbedmouse"] = _settings.style.mouseGrabbed;
	return true;
}

float sfx::animated_drawable::calculateDelta(const sf::Time& timeout) noexcept {
	_firsttime = false;
	// If the delta timer has timed out, restart the delta timer twice to
	// achieved the desired effect.
	if (_deltaTimer.getElapsedTime() >= timeout) _deltaTimer.restart();
	return _deltaTimer.restart().asSeconds();
}

float sfx::animated_drawable::accumulatedDelta(const sf::Time& timeout) noexcept {
	_delta += calculateDelta(timeout);
	return _delta;
}

void sfx::animated_drawable::resetDeltaAccumulation(const float to) noexcept {
	_delta = to;
}

void sfx::animated_drawable::finish() noexcept {
	_finished = true;
}

bool sfx::animated_drawable::isFinished() const noexcept {
	return _finished;
}

bool sfx::animated_drawable::firstTimeAnimated() const noexcept {
	return _firsttime;
}

void sfx::animated_drawable::resetAnimation() noexcept {
	_finished = false;
	_firsttime = true;
}