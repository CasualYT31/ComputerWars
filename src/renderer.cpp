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

void sfx::delta_timer::calculate() noexcept {
	_delta = _deltaTimer.restart().asSeconds();
}

float sfx::delta_timer::get() const noexcept {
	return _delta;
}

const sfx::renderer_properties sfx::renderer::default_properties = sfx::renderer_properties();

sfx::renderer::renderer(const std::string& name) noexcept : _logger(name) {}

void sfx::renderer::setProperties(const sfx::renderer_properties& newval) noexcept {
	_window = newval;
}

sfx::renderer_properties sfx::renderer::getProperties() const noexcept {
	return _window;
}

void sfx::renderer::openWindow() noexcept {
	if (_window.fullscreen) {
		create((_window.size < sf::VideoMode::getFullscreenModes().size() ?
			sf::VideoMode::getFullscreenModes()[_window.size] :
			sf::VideoMode::getFullscreenModes()[sf::VideoMode::getFullscreenModes().size() - 1]),
			_window.caption, sf::Style::Fullscreen);
	} else {
		create(sf::VideoMode(_window.width, _window.height), _window.caption, sf::Style::Titlebar | sf::Style::Close);
	}
}

void sfx::renderer::drawToScale(const sf::Texture& tex, float x, float y, const bool scale) {
	drawToScale(sf::Sprite(tex), x, y, scale);
}

void sfx::renderer::drawToScale(const sf::Drawable& drawing, float x, float y, const bool scale) {
	sf::Transform positioning;
	if (scale) positioning.scale(_window.scale, _window.scale);
	positioning.translate(x + getCamera().x, y + getCamera().y);
	sf::RenderStates state(positioning);
	this->draw(drawing, state);
}

sf::Vector2f sfx::renderer::getCamera() const noexcept {
	return _camera;
}

sf::Vector2f sfx::renderer::setCamera(const sf::Vector2f& newCamera) noexcept {
	auto old = _camera;
	_camera = newCamera * _window.scale;
	return old;
}

sf::Vector2f sfx::renderer::setCamera(const float x, const float y) noexcept {
	return setCamera(sf::Vector2f(x, y));
}

sf::Vector2f sfx::renderer::moveCamera(const sf::Vector2f& offset) noexcept {
	auto old = _camera;
	_camera += offset * _window.scale;
	return _camera;
}

sf::Vector2f sfx::renderer::moveCamera(const float x, const float y) noexcept {
	auto old = _camera;
	_camera.x += x * _window.scale;
	_camera.y += y * _window.scale;
	return _camera;
}

bool sfx::renderer::_load(safe::json& j) noexcept {
	j.apply(_window.width, { "width" }, &default_properties.width, true);
	j.apply(_window.height, { "height" }, &default_properties.height, true);
	j.apply(_window.size, { "size" }, &default_properties.size, true);
	j.apply(_window.caption, { "caption" }, &default_properties.caption, true);
	j.apply(_window.fullscreen, { "fullscreen" }, &default_properties.fullscreen, true);
	j.apply(_window.scale, { "scale" }, &default_properties.scale, true);
	if (_window.size >= sf::VideoMode::getFullscreenModes().size()) {
		_logger.write("The fullscreen size index given in the script \"{}\" is too large ({} >= {}). It has been reset to {}.",
			getScriptPath(), _window.size, sf::VideoMode::getFullscreenModes().size(), (sf::VideoMode::getFullscreenModes().size() == 0 ? 0 : sf::VideoMode::getFullscreenModes().size() - 1));
		if (sf::VideoMode::getFullscreenModes().size() == 0) {
			_window.size = 0;
		} else {
			_window.size = (unsigned int)sf::VideoMode::getFullscreenModes().size() - 1;
		}
	}
	return true;
}

bool sfx::renderer::_save(nlohmann::json& j) noexcept {
	j["width"] = _window.width;
	j["height"] = _window.height;
	j["size"] = _window.size;
	j["caption"] = _window.caption;
	j["fullscreen"] = _window.fullscreen;
	j["scale"] = _window.scale;
	return true;
}

//TRANSITIONS
sfx::trans::rectangle::rectangle(sf::RenderWindow* window, const bool fadingIn, const sf::Time& duration, const sf::Color& colour) noexcept :
	_window(window), _fadingIn(fadingIn), _duration(duration), _colour(colour) {
	_toprect.setFillColor(_colour);
	_bottomrect.setFillColor(_colour);
	if (_fadingIn) {
		_size.x = (float) _window->getSize().x;
		_size.y = (float) _window->getSize().y;
	} else {
		_size = sf::Vector2f(0.0,0.0);
	}
}

bool sfx::trans::rectangle::transition() {
	_delta.calculate();
	static sf::Clock test;
	if (_fadingIn) {
		_size.x -= (((float) _window->getSize().y / _duration.asSeconds()) * ((float) _window->getSize().x / (float) _window->getSize().y)) * _delta.get();
		_size.y -= (((float) _window->getSize().x / _duration.asSeconds()) * ((float) _window->getSize().y / (float) _window->getSize().x)) * _delta.get();
	} else {
		_size.x += (((float) _window->getSize().y / _duration.asSeconds()) * ((float) _window->getSize().x / (float) _window->getSize().y)) * _delta.get();
		_size.y += (((float) _window->getSize().x / _duration.asSeconds()) * ((float) _window->getSize().y / (float) _window->getSize().x)) * _delta.get();
	}
	_toprect.setSize(_size);
	_bottomrect.setSize(_size);
	_toprect.setPosition(0, 0);
	_bottomrect.setPosition(_window->getSize().x - _size.x, _window->getSize().y - _size.y);
	_window->draw(_toprect);
	_window->draw(_bottomrect);
	if (_fadingIn && (_size.x <= 0.0f && _size.y <= 0.0f)) {
		return true;
	} else if (!_fadingIn && (_size.x >= _window->getSize().x && _size.y >= _window->getSize().y)) {
		return true;
	} else {
		return false;
	}
}