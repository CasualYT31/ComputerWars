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

awe::tile::tile(const view_callbacks& callbacks,
	const std::shared_ptr<const awe::banks>& banks,
	const std::shared_ptr<awe::animated_tile>& animatedTile,
	const std::function<void(const std::function<void(void)>&)>& spriteCallback,
	const engine::logger::data& data,
	const std::string& type,
	const awe::ArmyID owner,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) :
	_viewCallbacks(callbacks), _type(type), _owner(owner), _tileSprite(animatedTile),
	_updateSprite(spriteCallback), _loggerData(data), _banks(banks) {
	animatedTile->setSpritesheet(sheet);
	updateSpriteID();
}

void awe::tile::setTileType(const std::string& type) {
	_type = type;
	updateSpriteID();
}

engine::CScriptWrapper<awe::tile_type_view> awe::tile::getTileType() const {
	const auto tileType = awe::tile_type_view::Create(_loggerData, _banks, _type);
	tileType->overrides
		.commander(_owner == awe::NO_ARMY ? "" : _viewCallbacks.commander(_owner))
		.country(_owner == awe::NO_ARMY ? "" : _viewCallbacks.country(_owner))
		.structure(_structure);
	return tileType;
}

engine::CScriptWrapper<awe::terrain_view> awe::tile::getTerrain() const {
	const auto terrain = awe::terrain_view::Create(_loggerData, _banks, getTerrainScriptName());
	terrain->overrides
		.commander(_owner == awe::NO_ARMY ? "" : _viewCallbacks.commander(_owner))
		.country(_owner == awe::NO_ARMY ? "" : _viewCallbacks.country(_owner))
		.structure(_structure)
		.tileType(_type);
	return terrain;
}

void awe::tile::setTileOwner(const awe::ArmyID owner) {
	_owner = owner;
	updateSpriteID();
}

engine::CScriptWrapper<awe::structure_view> awe::tile::getStructureType() const {
	if (!isPartOfStructure()) return nullptr;
	const auto structure = awe::structure_view::Create(_loggerData, _banks, _structure);
	structure->overrides
		.commander(_owner == awe::NO_ARMY ? "" : _viewCallbacks.commander(_owner))
		.country(_owner == awe::NO_ARMY ? "" : _viewCallbacks.country(_owner));
	return structure;
}

void awe::tile::setVisibility(const bool visible) {
	// If just the visibility changes, update the sprite immediately instead of
	// queueing the update, as start of day animations may temporarily show the
	// owner of tiles otherwise.
	const auto changed = visible != _visible;
	_visible = visible;
	if (changed) _updateSpriteID(_tileSprite, getTileType(), getTerrain(), _visible);
}

void awe::tile::updateSpriteID() {
	const auto tileType = getTileType();
	if (!tileType->isScriptNameValid()) return;
	const auto terrain = getTerrain();
	_updateSprite(std::bind(&awe::tile::_updateSpriteID, this, _tileSprite,
		tileType, terrain, _visible));
}

void awe::tile::_updateSpriteID(
	const std::weak_ptr<awe::animated_tile>& _tileSprite,
	engine::CScriptWrapper<awe::tile_type_view> type,
	engine::CScriptWrapper<awe::terrain_view> terrain, const bool visible) {
	if (!type->isScriptNameValid() || _tileSprite.expired()) return;
	// Explicitly clear owner country override if the tile is hidden, and it
	// shouldn't show its owner when hidden.
	if (!visible && !terrain->showOwnerWhenHidden()) {
		type->overrides.country("");
	}
	const auto tileSprite = _tileSprite.lock();
	tileSprite->setSprite(type->tile());
}
