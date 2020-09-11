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

#include "terrain.h"

awe::tile::tile(sf::Vector2u location, const std::shared_ptr<const awe::tile_type>& tile, const sf::Int32 hp, std::shared_ptr<awe::army> owner) noexcept {
	setLocation(location);
	setTile(tile);
	setHP(hp);
	setOwner(owner);
}

void awe::tile::setLocation(sf::Vector2u location) noexcept {
	_location = location;
}

sf::Vector2u awe::tile::getLocation() const noexcept {
	return _location;
}

void awe::tile::setTile(const std::shared_ptr<const awe::tile_type>& newTile) noexcept {
	_tileType = newTile;
}

std::shared_ptr<const awe::tile_type> awe::tile::getTile() const noexcept {
	return _tileType;
}

sf::Int32 awe::tile::setHP(const sf::Int32 newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	if (_tileType) {
		if (_hp < 0 || _hp >(int)_tileType->getType()->getMaxHP()) {
			_hp = _tileType->getType()->getMaxHP();
		}
	}
	return old;
}

sf::Int32 awe::tile::getHP() const noexcept {
	return _hp;
}

void awe::tile::setOwner(std::shared_ptr<awe::army> newOwner) noexcept {
	_owner = newOwner;
}

std::weak_ptr <awe::army> awe::tile::getOwner() const noexcept {
	return _owner;
}

void awe::tile::setUnit(std::shared_ptr<awe::unit> newUnit) noexcept {
	_unit = newUnit;
}

std::weak_ptr<awe::unit> awe::tile::getUnit() const noexcept {
	return _unit;
}

bool awe::tile::isOwned() const noexcept {
	return !_owner.expired();
}

bool awe::tile::isOccupied() const noexcept {
	return !_unit.expired();
}

bool awe::tile::operator==(const awe::tile& rhs) const noexcept {
	return UUID == rhs.UUID;
}

bool awe::tile::operator!=(const awe::tile& rhs) const noexcept {
	return !(*this == rhs);
}