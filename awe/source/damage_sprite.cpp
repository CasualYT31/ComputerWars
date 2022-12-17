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

#include "damage_sprite.hpp"

awe::damage_tooltip::damage_tooltip() noexcept {
	_damage.setOutlineColor(sf::Color::Black);
	_damage.setFillColor(sf::Color::White);
	_damage.setOutlineThickness(2.0f);
}

void awe::damage_tooltip::setDamage(const unsigned int dmg) noexcept {
	_damage.setString(std::to_string(dmg) + "%");
}

void awe::damage_tooltip::setPosition(const sf::Vector2f& pos) noexcept {
	_sprite.setPosition(pos + sf::Vector2f(28.0f, 27.0f));
	_damage.setPosition(pos + sf::Vector2f(28.0f, 37.0f));
}

bool awe::damage_tooltip::animate(const sf::RenderTarget& target,
	const double scaling) noexcept {
	return _sprite.animate(target, scaling);
}

void awe::damage_tooltip::draw(sf::RenderTarget& target, sf::RenderStates states)
	const {
	if (_visible) {
		target.draw(_sprite, states);
		target.draw(_damage, states);
	}
}
