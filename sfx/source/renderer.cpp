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

#include "renderer.hpp"
#include "fmtsfx.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif

float sfx::delta_timer::calculateDelta(const sf::Time& timeout) {
	// If the delta timer has timed out, restart the delta timer twice to
	// achieved the desired effect.
	if (_deltaTimer.getElapsedTime() >= timeout) _deltaTimer.restart();
	return _deltaTimer.restart().asSeconds();
}

float sfx::delta_timer::accumulatedDelta(const sf::Time& timeout) {
	_delta += calculateDelta(timeout);
	return _delta;
}

float sfx::delta_timer::accumulatedDelta(float& delta, const sf::Time& timeout) {
	delta = calculateDelta(timeout);
	_delta += delta;
	return _delta;
}

void sfx::delta_timer::resetDeltaAccumulation(const float to) noexcept {
	_delta = to;
}

float sfx::animated_drawable::calculateDelta(const sf::Time& timeout) {
	_firsttime = false;
	return delta_timer::calculateDelta(timeout);
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

void sfx::animated_drawable_with_independent_view::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	const sf::View oldView = target.getView();
	target.setView(sf::View(sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(target.getSize().x),
		static_cast<float>(target.getSize().y))));
	drawWithIndependentView(target, states);
	target.setView(oldView);
}

bool sfx::maximiseWindow(const sf::WindowHandle window, const bool maximise,
	engine::logger* const logger) noexcept {
#ifdef _WIN32
	// https://learn.microsoft.com/en-us/windows/win32/menurc/wm-syscommand.
	LRESULT result = 0;
	if (maximise) {
		// Likely not needed for our use case, but still useful to employ for a
		// generic function nonetheless: https://stackoverflow.com/a/4687861.
		result = SendMessage(window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	} else {
		result = SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	if (result != 0 && logger) {
		logger->error("maximiseWindow() call failed on Windows: LRESULT = {}, "
			"maximise parameter = {}.", result, maximise);
	}
	return result == 0;
#else
	if (logger) {
		logger->error("maximiseWindow() called on unsupported platform, window "
			"will not be changed.");
	}
	return false;
#endif
}

bool sfx::isWindowMaximised(const sf::WindowHandle window,
	engine::logger* const logger) noexcept {
#ifdef _WIN32
	WINDOWPLACEMENT windowPlacement;
	windowPlacement.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement(window, &windowPlacement)) {
		return windowPlacement.showCmd == SW_MAXIMIZE;
	} else {
		if (logger) {
			DWORD lastError = GetLastError();
			logger->error("isWindowMaximised() call failed on Windows: "
				"GetLastError() = {}. 1400 means invalid window handle.",
				lastError);
		}
		return false;
	}
#else
	if (logger) {
		logger->error("isWindowMaximised() called on unsupported platform, FALSE "
			"will be returned.");
	}
	return false;
#endif
}

const sf::Vector2u sfx::renderer::MIN_SIZE{ 426, 240 };

sfx::renderer::renderer(const engine::logger::data& data) :
	json_script({ data.sink, "json_script" }), _logger(data) {}

void sfx::renderer::setSize(const sf::Vector2u& size) {
	sf::RenderWindow::setSize(_updateSize(size));
}

void sfx::renderer::openWindow() {
	sf::VideoMode mode(_settings.width, _settings.height);
	if (_settings.style.fullscreen && !mode.isValid()) {
		_logger.critical("The renderer's configurations contained an invalid "
			"width ({}) and height ({}) for fullscreen mode. The program has "
			"reverted to windowed mode.", mode.width, mode.height);
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
	// A funny effect with SFML happens when moving the window in full screen mode
	// you can see it with multiple monitors: it's like a part of the window pops
	// up on the other screen...
	if (!_settings.style.fullscreen) {
		if (!_settings.style.maximised ||
			!maximiseWindow(getSystemHandle(), true, &_logger)) {
			setPosition(sf::Vector2i(_settings.x, _settings.y));
		}
	}
	setFramerateLimit(_settings.framerate);
	setVerticalSyncEnabled(_settings.style.vsync);
	setMouseCursorVisible(_settings.style.mouseVisible);
	setMouseCursorGrabbed(_settings.style.mouseGrabbed);
	// Apply icon.
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

void sfx::renderer::setSettings(const sfx::renderer_settings& newSettings) {
	_settings = newSettings;
	openWindow();
}

void sfx::renderer::handleEvents(
	const std::function<void(const sf::Event&)>& onEvent) {
	sf::Event event;
	while (pollEvent(event)) {
		if (event.type == sf::Event::Resized) sf::RenderWindow::setSize(
			_updateSize({ event.size.width, event.size.height }, false));
		onEvent(event);
	}
}

bool sfx::renderer::animate(sfx::animated_drawable& drawable) const {
	return drawable.animate(*this);
}

bool sfx::renderer::_load(engine::json& j) {
	sfx::renderer_settings settings;
	j.apply(settings.width, { "width" }, true);
	j.apply(settings.height, { "height" }, true);
	const auto newSize = _updateSize({ settings.width, settings.height });
	settings.width = newSize.x;
	settings.height = newSize.y;
	j.apply(settings.x, { "x" }, true);
	j.apply(settings.y, { "y" }, true);
	j.apply(settings.framerate, { "framerate" }, true);
	j.apply(settings.caption, { "caption" }, true);
	j.apply(settings.iconPath, { "icon" }, true);
	j.apply(settings.style.close, { "close" }, true);
	j.apply(settings.style.def, { "def" }, true);
	j.apply(settings.style.fullscreen, { "fullscreen" }, true);
	j.apply(settings.style.none, { "none" }, true);
	j.apply(settings.style.resize, { "resize" }, true);
	j.apply(settings.style.titlebar, { "titlebar" }, true);
	j.apply(settings.style.vsync, { "vsync" }, true);
	j.apply(settings.style.mouseVisible, { "cursor" }, true);
	j.apply(settings.style.mouseGrabbed, { "grabbedmouse" }, true);
	j.apply(settings.style.maximised, { "maximised" }, true);
	_settings = std::move(settings);
	return true;
}

bool sfx::renderer::_save(nlohmann::ordered_json& j) {
	_settings.style.maximised = isWindowMaximised(getSystemHandle(), &_logger);
	if (!_settings.style.maximised) {
		// With the current code, this won't necessarily remember the exactly size
		// and location of a window right before the maximise/fullscreen, (you'd
		// have to explicitly save before maximising, etc.), but it's good enough.
		_settings.width = getSize().x;
		_settings.height = getSize().y;
		if (!_settings.style.fullscreen) {
			_settings.x = getPosition().x;
			_settings.y = getPosition().y;
		}
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
	j["maximised"] = _settings.style.maximised;
	return true;
}

sf::Vector2u sfx::renderer::_updateSize(const sf::Vector2u& size, const bool log) {
	sf::Vector2u newSize(size);
	if (newSize.x < MIN_SIZE.x) newSize.x = MIN_SIZE.x;
	if (newSize.y < MIN_SIZE.y) newSize.y = MIN_SIZE.y;
	if (log && size != newSize) _logger.warning("Attempted to set the render "
		"window's size to {}, which is smaller than the minimum allowed size of "
		"{}. Setting the window's size to {}.", size, MIN_SIZE, newSize);
	return newSize;
}
