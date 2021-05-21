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

awe::tile::tile(sf::Vector2u location) noexcept : _location(location) {}

sf::Vector2u awe::tile::getLocation() const noexcept {
	return _location;
}

void awe::tile::setTileType(const std::shared_ptr<const awe::tile_type>& newTile) noexcept {
	_tileType = newTile;
	_updateSprite();
}

std::shared_ptr<const awe::tile_type> awe::tile::getTileType() const noexcept {
	return _tileType;
}

void awe::tile::setHP(const awe::HP newHP) noexcept {
	_hp = newHP;
	if (_tileType) {
		if (_hp < 0 || _hp >(int)_tileType->getType()->getMaxHP()) {
			_hp = _tileType->getType()->getMaxHP();
		}
	}
}

sf::Int32 awe::tile::getHP() const noexcept {
	return _hp;
}

void awe::tile::setOwner(const std::shared_ptr<awe::army>& newOwner) noexcept {
	_owner = newOwner;
	_updateSprite();
}

std::weak_ptr <awe::army> awe::tile::getOwner() const noexcept {
	return _owner;
}

void awe::tile::setUnit(const std::shared_ptr<awe::unit>& newUnit) noexcept {
	_unit = newUnit;
}

std::weak_ptr<awe::unit> awe::tile::getUnit() const noexcept {
	return _unit;
}

bool awe::tile::operator==(const awe::tile& rhs) const noexcept {
	return UUID == rhs.UUID;
}

bool awe::tile::operator!=(const awe::tile& rhs) const noexcept {
	return !(*this == rhs);
}

void awe::tile::setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
	if (ptr) {
		_sprite.setSpritesheet(ptr);
		_updateSprite();
	} else {
		_sprite.setSpritesheet(nullptr);
	}
	_tileSpritesheet = ptr;
}

std::shared_ptr<sfx::animated_spritesheet> awe::tile::getTileSpritesheet() const noexcept {
	return _tileSpritesheet;
}

unsigned int awe::tile::getTileSpriteID() const noexcept {
	return _sprite.getSprite();
}

bool awe::tile::animate(const sf::RenderTarget& target) noexcept {
	return _sprite.animate(target);
}

void awe::tile::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
}

void awe::tile::_updateSprite() noexcept {
	if (_tileType) {
		auto pOwner = _owner.lock();
		if (pOwner && pOwner->getCountry()) {
			_sprite.setSprite(_tileType->getOwnedTile(pOwner->getCountry()->UUID.getID()));
		} else {
			_sprite.setSprite(_tileType->getNeutralTile());
		}
	}
}