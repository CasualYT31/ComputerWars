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

#include "../include/labelunit.hpp"

awe::label_unit::label_unit(const awe::unit& unit,
	const std::shared_ptr<awe::animated_unit>& unitSprite,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::string& sprite, const bool leftSide, const float duration) :
	_unitSprite(unitSprite), _label(sheet, sprite), _leftSide(leftSide),
	_duration(duration) {}

bool awe::label_unit::animate(const sf::RenderTarget& target) {
	if (firstTimeAnimated()) {
		_finalPosition = _unitSprite->getPixelPosition();
		_finalPosition.y += _unitSprite->getPixelSize().y / 2.0f;
		if (!_leftSide) _finalPosition.x += _unitSprite->getPixelSize().x;
		_label.setPosition(_finalPosition);
	}
	const auto delta = accumulatedDelta();
	_label.animate(target);
	// Continually set the position and origin of the label.
	if (delta < MOVE_IN_DURATION) {
		const float offset = INITIAL_OFFSET * (1 - (delta / MOVE_IN_DURATION));
		_label.setPosition(sf::Vector2f(
			_finalPosition.x + (_leftSide ? -offset : offset),
			_finalPosition.y
		));
	} else _label.setPosition(_finalPosition);
	const auto labelSize = _label.getSize();
	if (_leftSide) {
		_label.setOrigin(sf::Vector2f(labelSize.x, labelSize.y / 2.0f));
	} else {
		_label.setOrigin(sf::Vector2f(0.0f, labelSize.y / 2.0f));
	}
	return delta >= _duration;
}

void awe::label_unit::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	target.draw(_label, states);
}
