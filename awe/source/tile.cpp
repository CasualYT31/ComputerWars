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

void awe::tile::setTileOwner(const awe::ArmyID owner) {
	_owner = owner;
	updateSpriteID();
}

void awe::tile::setVisibility(const bool visible) {
	// If just the visibility changes, update the sprite immediately instead of
	// queueing the update, as start of day animations may temporarily show the
	// owner of tiles otherwise.
	const auto changed = visible != _visible;
	_visible = visible;
	if (changed) _updateSpriteID(_tileSprite, _owner, _type, _visible);
}

void awe::tile::updateSpriteID() {
	if (!_type) return;
	_updateSprite(std::bind(&awe::tile::_updateSpriteID, this, _tileSprite, _owner,
		_type, _visible));
}

void awe::tile::_updateSpriteID(
	const std::weak_ptr<awe::animated_tile>& _tileSprite, const awe::ArmyID owner,
	const std::shared_ptr<const awe::tile_type>& type, const bool visible) {
	if (!type || _tileSprite.expired()) return;
	const auto tileSprite = _tileSprite.lock();
	if (owner == awe::NO_ARMY ||
		(!visible && !type->getType()->showOwnerWhenHidden())) {
		tileSprite->setSprite(type->getNeutralTile());
	} else {
		tileSprite->setSprite(type->getOwnedTile(owner));
	}
}
