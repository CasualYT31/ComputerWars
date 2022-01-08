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

#include "army_pane.h"

void awe::army_pane::setArmy(const awe::army& army) noexcept {
	_army = std::make_shared<const awe::army>(army);
}

void awe::army_pane::setGeneralLocation(const awe::army_pane::location& location)
	noexcept {
	_location = location;
}

bool awe::army_pane::animate(const sf::RenderTarget& target) noexcept {
	if (_army) {
		_bg.setFillColor(_army->getCountry()->getColour());
		_rounded_bg.setFillColor(_army->getCountry()->getColour());
	} else {
		_bg.setFillColor(sf::Color::White);
		_rounded_bg.setFillColor(sf::Color::White);
	}
	// ensure original transform has been cleared
	_position = sf::Transform();
	if (_location == awe::army_pane::location::Left) {
		_animateLeft();
	} else if (_location == awe::army_pane::location::Right) {
		_animateRight(target);
	}
	return true;
}

void awe::army_pane::_animateLeft() noexcept {
	sf::Vector2f size = sf::Vector2f(200.0f, 50.0f);
	sf::Vector2f origin = sf::Vector2f(0.0f, 0.0f);
	// step 1: pane background
	_bg.setPosition(origin);
	_bg.setSize(size);
	_rounded_bg.setPosition(
		sf::Vector2f(origin.x + size.x - size.y / 2.0f, origin.y)
	);
	_rounded_bg.setRadius(size.y / 2.0f);
	// step 2: CO face
	// step 3: funds
	// step 4: power meter
}

void awe::army_pane::_animateRight(const sf::RenderTarget& target) noexcept {
	sf::Vector2f size = sf::Vector2f(200.0f, 50.0f);
	sf::Vector2f origin = sf::Vector2f(size.x + size.y / 2.0f, 0.0f);
	_position.translate(sf::Vector2f(target.getSize().x - origin.x, 0.0f));
	// step 1: pane background
	_bg.setPosition(sf::Vector2f(origin.x - size.x, origin.y));
	_bg.setSize(size);
	_rounded_bg.setPosition(
		sf::Vector2f(origin.x - size.x - size.y / 2.0f, origin.y)
	);
	_rounded_bg.setRadius(size.y / 2.0f);
	// step 2: CO face
	// step 3: funds
	// step 4: power meter
}

void awe::army_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	// combine translation with given states
	states.transform.combine(_position);
	// draw
	target.draw(_rounded_bg, states);
	target.draw(_bg, states);
	target.draw(_co, states);
	target.draw(_funds, states);
}