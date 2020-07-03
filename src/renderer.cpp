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
}

bool sfx::renderer::_load(safe::json& j) noexcept {
	j.apply(width, { "width" }, &width, true);
	j.apply(height, { "height" }, &height, true);
	j.apply(caption, { "caption" }, &caption, true);
	j.apply(style.close, { "close" }, &style.close, true);
	j.apply(style.def, { "def" }, &style.def, true);
	j.apply(style.fullscreen, { "fullscreen" }, &style.fullscreen, true);
	j.apply(style.none, { "none" }, &style.none, true);
	j.apply(style.resize, { "resize" }, &style.resize, true);
	j.apply(style.titlebar, { "titlebar" }, &style.titlebar, true);
	return true;
}

bool sfx::renderer::_save(nlohmann::json& j) noexcept {
	j["width"] = width;
	j["height"] = height;
	j["caption"] = caption;
	j["close"] = style.close;
	j["def"] = style.def;
	j["fullscreen"] = style.fullscreen;
	j["none"] = style.none;
	j["resize"] = style.resize;
	j["titlebar"] = style.titlebar;
	return true;
}

float sfx::animated_drawable::calculateDelta() noexcept {
	return _deltaTimer.restart().asSeconds();
}

// TRANSITIONS

sfx::trans::rectangle::rectangle(const bool isFadingIn, const sf::Time& duration, const sf::Color& colour) noexcept :
	_isFadingIn(isFadingIn), _duration(duration) {
	_toprect.setFillColor(colour);
	_bottomrect.setFillColor(colour);
}

bool sfx::trans::rectangle::animate(sf::RenderTarget& target) noexcept {
	// initialise animation
	if (_isFirstCallToAnimate) {
		if (_isFadingIn) {
			_size.x = (float)target.getSize().x;
			_size.y = (float)target.getSize().y;
		}
		else {
			_size = sf::Vector2f(0.0, 0.0);
		}
		_isFirstCallToAnimate = false;
	}
	// animate
	float delta = calculateDelta();
	if (_isFadingIn) {
		_size.x -= (((float)target.getSize().y / _duration.asSeconds()) * ((float)target.getSize().x / (float)target.getSize().y)) * delta;
		_size.y -= (((float)target.getSize().x / _duration.asSeconds()) * ((float)target.getSize().y / (float)target.getSize().x)) * delta;
	}
	else {
		_size.x += (((float)target.getSize().y / _duration.asSeconds()) * ((float)target.getSize().x / (float)target.getSize().y)) * delta;
		_size.y += (((float)target.getSize().x / _duration.asSeconds()) * ((float)target.getSize().y / (float)target.getSize().x)) * delta;
	}
	_toprect.setSize(_size);
	_bottomrect.setSize(_size);
	_toprect.setPosition(0, 0);
	_bottomrect.setPosition(target.getSize().x - _size.x, target.getSize().y - _size.y);
	// check to see if transition has completed
	if (_isFadingIn && (_size.x <= 0.0f && _size.y <= 0.0f)) {
		return true;
	}
	else if (!_isFadingIn && (_size.x >= target.getSize().x && _size.y >= target.getSize().y)) {
		return true;
	}
	else {
		return false;
	}
}

void sfx::trans::rectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_toprect, states);
	target.draw(_bottomrect, states);
}