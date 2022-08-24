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
	_tileName.setCharacterSize(16);
	_tileName.setFillColor(sf::Color::White);
	_tileName.setOutlineColor(sf::Color::Black);
	_tileName.setOutlineThickness(1.5f);
	_tileDef.setCharacterSize(12);
	_tileDef.setFillColor(sf::Color::White);
	_tileDef.setOutlineColor(sf::Color::Black);
	_tileDef.setOutlineThickness(1.5f);
	_tileHP.setCharacterSize(12);
	_tileHP.setFillColor(sf::Color::White);
	_tileHP.setOutlineColor(sf::Color::Black);
	_tileHP.setOutlineThickness(1.5f);
}

void awe::tile_pane::setTile(const awe::tile& tile) noexcept {
	_tile = std::make_shared<const awe::tile>(tile);
}

void awe::tile_pane::addUnit(const awe::unit& unit) noexcept {
	_units.push_back(awe::unit_pane());
	_units.back().setUnit(unit);
	_units.back().setSpritesheet(_icons);
	_units.back().setFont(_font);
}

void awe::tile_pane::clearUnits() noexcept {
	_units.clear();
}

void awe::tile_pane::setGeneralLocation(const awe::tile_pane::location& location)
	noexcept {
	_location = location;
}

void awe::tile_pane::setSpritesheet(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet) noexcept {
	_icons = sheet;
	_tileDefIcon.setSpritesheet(sheet);
	_tileDefIcon.setSprite("defstar");
	_tileHPIcon.setSpritesheet(sheet);
	_tileHPIcon.setSprite("hp");
	for (auto& u : _units) u.setSpritesheet(sheet);
}

void awe::tile_pane::setFont(const std::shared_ptr<const sf::Font>& font) noexcept
	{
	if (font) {
		_font = font;
		_tileName.setFont(*font);
		_tileDef.setFont(*font);
		_tileHP.setFont(*font);
		for (auto& u : _units) u.setFont(font);
	}
}

void awe::tile_pane::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) noexcept {
	if (dict) {
		_dict = dict;
		for (auto& u : _units) u.setLanguageDictionary(dict);
	}
}

bool awe::tile_pane::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	const float sectionWidth = 60.0f;
	sf::Vector2f size = sf::Vector2f(sectionWidth * (_units.size() + 1), 100.0f);
	// background
	_bg.setSize(size);
	if (_location == awe::tile_pane::location::Left) {
		_bg.setPosition(
			sf::Vector2f(0.0f, target.getSize().y / (float)scaling - size.y)
		);
	} else if (_location == awe::tile_pane::location::Right) {
		_bg.setPosition(sf::Vector2f(target.getSize().x / (float)scaling - size.x,
			target.getSize().y / (float)scaling - size.y));
	}
	std::vector<sf::Vector2f> points = _calculateCurvePoints();
	_rounded_bg.setPointCount(points.size());
	for (std::size_t p = 0; p < points.size(); p++)
		_rounded_bg.setPoint(p, points.at(p));
	float tileCentre = 0.0f;
	if (_location == awe::tile_pane::location::Left) {
		tileCentre = _bg.getPosition().x + sectionWidth / 2.0f;
	} else if (_location == awe::tile_pane::location::Right) {
		tileCentre = _bg.getPosition().x + size.x - sectionWidth / 2.0f;
	}
	// tile
	if (_tileIcon.getSpritesheet() != _tile->getSpritesheet()) {
		_tileIcon.setSpritesheet(_tile->getSpritesheet());
	}
	if (_tileIcon.getSprite() != _tile->getSprite()) {
		_tileIcon.setSprite(_tile->getSprite());
	}
	_tileIcon.animate(target, scaling);
	_tileIcon.setPosition(
		sf::Vector2f(tileCentre - _tileIcon.getSize().x / 2.0f,
			_bg.getPosition().y + 10.0f)
	);
	// tile name
	_tileName.setString(_tile->getTileType()->getType()->getShortName());
	_tileName.setPosition(
		sf::Vector2f(tileCentre - _tileName.getLocalBounds().width / 2.0f,
			_tileIcon.getPosition().y + _tileIcon.getSize().y)
	);
	// tile defence
	_tileDefIcon.animate(target, scaling);
	_tileDefIcon.setPosition(
		sf::Vector2f(tileCentre - sectionWidth / 2.0f + 10.0f,
			_bg.getPosition().y + _bg.getSize().y -
			_tileDefIcon.getSize().y - 10.0f)
	);
	_tileDef.setString(std::to_string(
		_tile->getTileType()->getType()->getDefence()
	));
	_tileDef.setPosition(
		sf::Vector2f(tileCentre + sectionWidth / 2.0f -
			_tileDef.getLocalBounds().width - 10.0f,
			_tileDefIcon.getPosition().y - 3.0f)
	);
	// tile HP
	if (_tile->getTileType()->getType()->getMaxHP() > 0) {
		if (!_tileHPIcon.getSpritesheet()) _tileHPIcon.setSpritesheet(_icons);
		_tileHPIcon.animate(target, scaling);
		_tileHPIcon.setPosition(
			sf::Vector2f(_tileDefIcon.getPosition().x,
				_tileDefIcon.getPosition().y - _tileHPIcon.getSize().y - 5.0f)
		);
		_tileHP.setString(std::to_string(_tile->getTileHP()));
		_tileHP.setPosition(
			sf::Vector2f(tileCentre + sectionWidth / 2.0f -
				_tileHP.getLocalBounds().width - 5.0f,
				_tileHPIcon.getPosition().y - 3.0f)
		);
	} else {
		_tileHPIcon.setSpritesheet(nullptr);
		_tileHP.setString("");
		_tileHPIcon.animate(target, scaling);
	}
	// units
	std::size_t i = 1;
	for (auto& u : _units) {
		float unitPaneX = _bg.getPosition().x;
		if (_location == awe::tile_pane::location::Left) {
			unitPaneX += sectionWidth * i;
		} else if (_location == awe::tile_pane::location::Right) {
			unitPaneX += sectionWidth * (_units.size() - i);
		}
		u.setRect(
			sf::FloatRect(unitPaneX, _bg.getPosition().y, sectionWidth, size.y)
		);
		u.animate(target, scaling);
		i++;
	}
	return true;
}

void awe::tile_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	target.draw(_rounded_bg, states);
	target.draw(_bg, states);
	target.draw(_tileIcon, states);
	target.draw(_tileName, states);
	target.draw(_tileDefIcon, states);
	target.draw(_tileHPIcon, states);
	target.draw(_tileDef, states);
	target.draw(_tileHP, states);
	for (auto& u : _units) target.draw(u, states);
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