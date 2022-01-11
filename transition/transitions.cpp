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

#include "transitions.h"

transition::base::base(const bool isFadingIn, const sf::Time& duration) noexcept :
	_isFadingIn(isFadingIn), _duration(duration) {}

bool transition::base::isFadingIn() const noexcept {
	return _isFadingIn;
}

sf::Time transition::base::duration() const noexcept {
	return _duration;
}

transition::rectangle::rectangle(const bool isFadingIn, const sf::Time& duration,
	const sf::Color& colour) noexcept : base(isFadingIn, duration) {
	_toprect.setFillColor(colour);
	_bottomrect.setFillColor(colour);
}

bool transition::rectangle::animate(const sf::RenderTarget& target,
	const double scaling) noexcept {
	if (isFinished()) return true;
	// initialise animation
	if (firstTimeAnimated()) {
		if (isFadingIn()) {
			_size.x = (float)target.getSize().x;
			_size.y = (float)target.getSize().y;
		} else {
			_size = sf::Vector2f(0.0, 0.0);
		}
	}
	// animate
	float delta = calculateDelta();
	if (isFadingIn()) {
		_size.x -= (((float)target.getSize().y / duration().asSeconds()) *
			((float)target.getSize().x / (float)target.getSize().y)) * delta;
		_size.y -= (((float)target.getSize().x / duration().asSeconds()) *
			((float)target.getSize().y / (float)target.getSize().x)) * delta;
	}
	else {
		_size.x += (((float)target.getSize().y / duration().asSeconds()) *
			((float)target.getSize().x / (float)target.getSize().y)) * delta;
		_size.y += (((float)target.getSize().x / duration().asSeconds()) *
			((float)target.getSize().y / (float)target.getSize().x)) * delta;
	}
	_toprect.setSize(_size);
	_bottomrect.setSize(_size);
	_toprect.setPosition(0, 0);
	_bottomrect.setPosition(target.getSize().x - _size.x,
		target.getSize().y - _size.y);
	// check to see if transition has completed
	if (isFadingIn() && (_size.x <= 0.0f && _size.y <= 0.0f)) {
		finish();
	} else if (!isFadingIn() && (_size.x >= target.getSize().x &&
		_size.y >= target.getSize().y)) {
		finish();
	}
	if (isFinished()) {
		_size = sf::Vector2f(0.0, 0.0);
		_toprect.setSize(_size);
		_bottomrect.setSize(_size);
	}
	return isFinished();
}

void transition::rectangle::draw(sf::RenderTarget& target, sf::RenderStates states)
	const {
	target.draw(_toprect, states);
	target.draw(_bottomrect, states);
}