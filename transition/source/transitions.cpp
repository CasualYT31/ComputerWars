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

#include "transitions.hpp"

transition::base::base(const bool isFadingIn, const sf::Time& duration) noexcept :
	_isFadingIn(isFadingIn), _duration(duration) {}

bool transition::base::isFadingIn() const noexcept {
	return _isFadingIn;
}

sf::Time transition::base::duration() const {
	return _duration;
}

transition::rectangle::rectangle(const bool isFadingIn, const sf::Color& colour,
	const sf::Time& duration) : base(isFadingIn, duration) {
	_toprect.setFillColor(colour);
	_bottomrect.setFillColor(colour);
	_toprect.setOutlineThickness(2.0f);
	_toprect.setOutlineColor(sf::Color::Red);
	_bottomrect.setOutlineThickness(2.0f);
	_bottomrect.setOutlineColor(sf::Color::Red);
}

bool transition::rectangle::animate(const sf::RenderTarget& target) {
	if (isFinished())
		return true;
	const auto targetSize = sf::Vector2f(target.getSize());

	if (firstTimeAnimated())
		_size = isFadingIn() ? targetSize : sf::Vector2f(0.0f, 0.0f);

	const float delta = calculateDelta();
	const sf::Vector2f offset(
		((targetSize.y / duration().asSeconds()) *
			(targetSize.x / targetSize.y)) * delta,
		((targetSize.x / duration().asSeconds()) *
			(targetSize.y / targetSize.x)) * delta
	);
	_size += isFadingIn() ? -offset : offset;
	_toprect.setSize(_size);
	_bottomrect.setSize(_size);
	_toprect.setPosition(-50.0f, -50.0f);
	_bottomrect.setPosition(targetSize.x - _size.x,
		targetSize.y - _size.y);

	if (isFadingIn() && (_size.x <= 0.0f && _size.y <= 0.0f)) {
		finish();
	} else if (!isFadingIn() && (_size.x >= targetSize.x &&
		_size.y >= targetSize.y)) {
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
