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

#include "tile.hpp"

awe::tile::tile(const std::shared_ptr<const awe::tile_type>& type,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept :
	_sprite(sheet, "") {
	setTileType(type);
}

void awe::tile::setTileType(const std::shared_ptr<const awe::tile_type>& type)
	noexcept {
	_type = type;
	_updateSpriteID();
}

std::shared_ptr<const awe::tile_type> awe::tile::getTileType() const noexcept {
	return _type;
}

void awe::tile::setTileOwner(const awe::ArmyID owner) noexcept {
	_owner = owner;
	_updateSpriteID();
}

awe::ArmyID awe::tile::getTileOwner() const noexcept {
	return _owner;
}

void awe::tile::setTileHP(const awe::HP hp) noexcept {
	if (hp <= 0) {
		_hp = 0;
	} else {
		_hp = hp;
	}
}

awe::HP awe::tile::getTileHP() const noexcept {
	return _hp;
}

void awe::tile::setUnit(const awe::UnitID id) noexcept {
	_unit = id;
}

awe::UnitID awe::tile::getUnit() const noexcept {
	return _unit;
}

void awe::tile::setSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sprite.setSpritesheet(sheet);
}

std::shared_ptr<const sfx::animated_spritesheet> awe::tile::getSpritesheet() const
	noexcept {
	return _sprite.getSpritesheet();
}

std::string awe::tile::getSprite() const noexcept {
	return _sprite.getSprite();
}

void awe::tile::setPixelPosition(float x, float y) noexcept {
	_sprite.setPosition(sf::Vector2f(x, y));
}

sf::Vector2f awe::tile::getPixelPosition() const noexcept {
	return _sprite.getPosition();
}

sf::Vector2f awe::tile::getPixelSize() const noexcept {
	return _sprite.getSize();
}

bool awe::tile::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	return _sprite.animate(target, scaling);
}

void awe::tile::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
}

void awe::tile::_updateSpriteID() noexcept {
	// If this tile has no type, leave the sprite ID alone.
	if (_type) {
		if (_owner == awe::army::NO_ARMY) {
			_sprite.setSprite(_type->getNeutralTile());
		} else {
			_sprite.setSprite(_type->getOwnedTile(_owner));
		}
	}
}
