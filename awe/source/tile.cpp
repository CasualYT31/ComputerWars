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

#include "tile.hpp"

awe::tile::tile(const std::shared_ptr<awe::animated_tile>& animatedTile,
	const std::function<void(const std::function<void(void)>&)>& spriteCallback,
	const std::shared_ptr<const awe::tile_type>& type,
	const awe::ArmyID owner,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) :
	_type(type), _owner(owner), _tileSprite(animatedTile),
	_updateSprite(spriteCallback) {
	animatedTile->setSpritesheet(sheet);
	updateSpriteID();
}

void awe::tile::setTileType(const std::shared_ptr<const awe::tile_type>& type) {
	_type = type;
	updateSpriteID();
}

std::shared_ptr<const awe::tile_type> awe::tile::getTileType() const {
	return _type;
}

void awe::tile::setTileOwner(const awe::ArmyID owner) {
	_owner = owner;
	updateSpriteID();
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

void awe::tile::setStructureType(
	const std::shared_ptr<const awe::structure>& structure) {
	_structure = structure;
}

std::shared_ptr<const awe::structure> awe::tile::getStructureType() const {
	return _structure;
}

void awe::tile::setStructureTile(const sf::Vector2i offset) {
	_offset = offset;
}

sf::Vector2i awe::tile::getStructureTile() const {
	return _offset;
}

void awe::tile::setStructureDestroyed(const bool isDestroyed) {
	_destroyed = isDestroyed;
}

bool awe::tile::getStructureDestroyed() const {
	return _destroyed;
}

void awe::tile::updateSpriteID() {
	if (!_type) return;
	_updateSprite(std::bind([](
		const std::weak_ptr<awe::animated_tile>& _tileSprite,
		const awe::ArmyID owner,
		const std::shared_ptr<const awe::tile_type>& type) {
			if (_tileSprite.expired()) return;
			const auto tileSprite = _tileSprite.lock();
			if (owner == awe::NO_ARMY) {
				tileSprite->setSprite(type->getNeutralTile());
			} else {
				tileSprite->setSprite(type->getOwnedTile(owner));
			}
		}, _tileSprite, _owner, _type)
	);
}
