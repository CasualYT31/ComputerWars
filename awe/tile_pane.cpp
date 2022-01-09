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

#include "tile_pane.h"

awe::tile_pane::tile_pane() noexcept {
	_bg.setFillColor(sf::Color(250,250,250,128));
	_rounded_bg.setFillColor(_bg.getFillColor());
}

void awe::tile_pane::setTile(const awe::tile& tile) noexcept {
	_tile = std::make_shared<const awe::tile>(tile);
}

void awe::tile_pane::addUnit(const awe::unit& unit) noexcept {
	_units.push_back(std::make_shared<const awe::unit>(unit));
}

void awe::tile_pane::clearUnits() noexcept {
	_units.clear();
}

void awe::tile_pane::setGeneralLocation(const awe::tile_pane::location& location)
	noexcept {
	_location = location;
}

bool awe::tile_pane::animate(const sf::RenderTarget& target) noexcept {
	sf::Vector2f size = sf::Vector2f(60.0f, 100.0f);
	_bg.setSize(size);
	if (_location == awe::tile_pane::location::Left) {
		_bg.setPosition(sf::Vector2f(0.0f, target.getSize().y - size.y));
	} else if (_location == awe::tile_pane::location::Right) {
		_bg.setPosition(sf::Vector2f(target.getSize().x - size.x,
			target.getSize().y - size.y));
	}
	std::vector<sf::Vector2f> points = _calculateCurvePoints();
	_rounded_bg.setPointCount(points.size());
	for (std::size_t p = 0; p < points.size(); p++)
		_rounded_bg.setPoint(p, points.at(p));
	return true;
}

void awe::tile_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	target.draw(_rounded_bg, states);
	target.draw(_bg, states);
}

std::vector<sf::Vector2f> awe::tile_pane::_calculateCurvePoints() const noexcept {
	std::vector<sf::Vector2f> ret;
	// https://math.stackexchange.com/questions/1643836/how-do-i-find-the-equation-for-a-semicircle-with-a-radius-of-2-on-the-x-axis
	for (float y = -(_bg.getSize().y / 2.0f); y <= _bg.getSize().y / 2.0f; y++) {
		const float radius = _bg.getSize().y / 2.0f;
		float x = std::sqrtf(radius * radius - y * y) * 0.25f;
		if (_location == awe::tile_pane::location::Right) x = -x;
		sf::Vector2f p = sf::Vector2f(x, y);
		if (_location == awe::tile_pane::location::Left) {
			p += sf::Vector2f(_bg.getSize().x,
				_bg.getPosition().y + _bg.getSize().y / 2.0f);
		} else if (_location == awe::tile_pane::location::Right) {
			p += sf::Vector2f(_bg.getPosition().x,
				_bg.getPosition().y + _bg.getSize().y / 2.0f);
		}
		ret.push_back(p);
	}
	return ret;
}