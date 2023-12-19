/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

#include "map.hpp"
#include "fmtawe.hpp"

bool awe::map::setSelectedUnit(const awe::UnitID unit) {
	if (unit == awe::NO_UNIT) {
		_selectedUnitRenderData.top().selectedUnit = awe::NO_UNIT;
		_selectedUnitRenderData.top().clearState();
		return true;
	}
	if (_isUnitPresent(unit)) {
		// if (_units.at(unit).isOnMap()) {
		_selectedUnitRenderData.top().selectedUnit = unit;
		return true;
		// } else {
		//	_logger.error("setSelectedUnit operation failed: cannot select unit "
		//		"with ID {} as it is not on the map.", unit);
		// }
	} else {
		_logger.error("setSelectedUnit operation failed: unit with ID {} doesn't "
			"exist!", unit);
	}
	return false;
}

bool awe::map::pushSelectedUnit(const awe::UnitID unit) {
	_selectedUnitRenderData.emplace(*_scripts);
	const auto ret = setSelectedUnit(unit);
	if (!ret) {
		_selectedUnitRenderData.pop();
		_logger.error("pushSelectedUnit operation failed: see above.");
	}
	return ret;
}

void awe::map::popSelectedUnit() {
	if (_selectedUnitRenderData.size() > 1) {
		_selectedUnitRenderData.pop();
		// At some point, the previously selected unit might have been deleted, and
		// if this is the case, we need to deselect it.
		if (!_isUnitPresent(_selectedUnitRenderData.top().selectedUnit) &&
			_selectedUnitRenderData.top().selectedUnit != awe::NO_UNIT) {
			_logger.warning("popSelectUnit operation: newly selected unit with ID "
				"{} is now no longer present: the selected unit render data state "
				"will now be cleared!",
				_selectedUnitRenderData.top().selectedUnit);
			setSelectedUnit(awe::NO_UNIT);
		}
	} else {
		_logger.error("popSelectUnit operation failed: the size of the stack was "
			"{}, which is too low!", _selectedUnitRenderData.size());
	}
}

awe::UnitID awe::map::getSelectedUnit() const {
	return _selectedUnitRenderData.top().selectedUnit;
}

void awe::map::addAvailableTile(const sf::Vector2u& tile) {
	if (_isOutOfBounds(tile)) {
		_logger.error("addAvailableTile operation failed: tile {} is out of "
			"bounds!", tile);
	} else {
		if (_selectedUnitRenderData.top().availableTiles.find(tile) ==
			_selectedUnitRenderData.top().availableTiles.end()) {
			_selectedUnitRenderData.top().availableTiles.insert(tile);
		}
	}
}

bool awe::map::isAvailableTile(const sf::Vector2u& tile) const {
	if (_isOutOfBounds(tile)) {
		_logger.error("isAvailableTile operation failed: tile {} is out of "
			"bounds!", tile);
		return false;
	} else {
		return _selectedUnitRenderData.top().availableTiles.find(tile) !=
			_selectedUnitRenderData.top().availableTiles.end();
	}
}

void awe::map::clearAvailableTiles() {
	_selectedUnitRenderData.top().availableTiles.clear();
}

void awe::map::setAvailableTileShader(const awe::available_tile_shader shader) {
	_selectedUnitRenderData.top().availableTileShader = shader;
}

awe::available_tile_shader awe::map::getAvailableTileShader() const {
	return _selectedUnitRenderData.top().availableTileShader;
}

CScriptArray* awe::map::getClosedList() {
	return _selectedUnitRenderData.top().closedList;
}

void awe::map::disableSelectedUnitRenderingEffects(const bool val) {
	_selectedUnitRenderData.top().disableRenderingEffects = val;
}

void awe::map::disableShaderForAvailableUnits(const bool val) {
	_selectedUnitRenderData.top().disableShaderForAvailableUnits = val;
}

void awe::map::regenerateClosedListSprites() {
	// Starting from the beginning; calculate the arrow sprites to draw for
	// each tile.
	CScriptArray* list = _selectedUnitRenderData.top().closedList;
	for (asUINT i = 0, length = list->GetSize(); i < length; ++i) {
		awe::closed_list_node* const current =
			(awe::closed_list_node* const)list->At(i);
		current->sprite.setSpritesheet(_sheet_icon);
		if (i == 0) {
			current->sprite.setSpritesheet(nullptr);
		} else if (i == length - 1) {
			awe::closed_list_node* const prev =
				(awe::closed_list_node* const)list->At(i - 1);
			if (prev->tile.x < current->tile.x) {
				current->sprite.setSprite("unitArrowRight");
			} else if (prev->tile.x > current->tile.x) {
				current->sprite.setSprite("unitArrowLeft");
			} else if (prev->tile.y < current->tile.y) {
				current->sprite.setSprite("unitArrowDown");
			} else if (prev->tile.y > current->tile.y) {
				current->sprite.setSprite("unitArrowUp");
			}
		} else {
			awe::closed_list_node* const prev =
				(awe::closed_list_node* const)list->At(i - 1);
			awe::closed_list_node* const next =
				(awe::closed_list_node* const)list->At(i + 1);
			if ((prev->tile.x < current->tile.x &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowHori");
			} else if ((prev->tile.y < current->tile.y &&
				current->tile.y < next->tile.y) ||
				(prev->tile.y > current->tile.y &&
					current->tile.y > next->tile.y)) {
				current->sprite.setSprite("unitArrowVert");
			} else if ((prev->tile.y < current->tile.y &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.y > next->tile.y)) {
				current->sprite.setSprite("unitArrowNE");
			} else if ((prev->tile.y > current->tile.y &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.y < next->tile.y)) {
				current->sprite.setSprite("unitArrowSE");
			} else if ((prev->tile.x < current->tile.x &&
				current->tile.y > next->tile.y) ||
				(prev->tile.y < current->tile.y &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowNW");
			} else if ((prev->tile.x < current->tile.x &&
				current->tile.y < next->tile.y) ||
				(prev->tile.y > current->tile.y &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowSW");
			}
		}
	}
}

void awe::map::addPreviewUnit(const awe::UnitID unit, const sf::Vector2u& pos) {
	if (!_isUnitPresent(unit)) {
		_logger.error("addPreviewUnit operation failed: unit with ID {} does not "
			"exist.", unit);
		return;
	}
	if (_isOutOfBounds(pos)) {
		_logger.error("addPreviewUnit operation failed: tile {} is out of bounds.",
			pos);
		return;
	}
	_unitLocationOverrides[unit] = pos;
}

void awe::map::removePreviewUnit(const awe::UnitID unit) {
	if (_unitLocationOverrides.find(unit) == _unitLocationOverrides.end()) {
		_logger.error("removePreviewUnit operation failed: unit with ID {} did "
			"not have a position override at the time of calling.", unit);
	} else {
		_unitLocationOverrides.erase(unit);
	}
}

void awe::map::removeAllPreviewUnits() {
	_unitLocationOverrides.clear();
}

std::size_t awe::map::getUnitPreviewsCount() const {
	return _unitLocationOverrides.size();
}

void awe::map::setTarget(
	const std::shared_ptr<sf::RenderTarget>& target) noexcept {
	_target = target;
}

void awe::map::alwaysShowHiddenUnits(const bool alwaysShow) noexcept {
	_alwaysShowHiddenUnits = alwaysShow;
}

void awe::map::setSelectedTile(const sf::Vector2u& pos) {
	if (!_isOutOfBounds(pos)) _sel = pos;
}

void awe::map::moveSelectedTileUp() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y - 1));
}

void awe::map::moveSelectedTileDown() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y + 1));
}

void awe::map::moveSelectedTileLeft() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x - 1, getSelectedTile().y));
}

void awe::map::moveSelectedTileRight() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x + 1, getSelectedTile().y));
}

void awe::map::setSelectedTileByPixel(const sf::Vector2i& pixel) {
	if (!_target) return;
	const auto coord = _target->mapPixelToCoords(pixel, _view);
	auto sel = getSelectedTile(), size = getMapSize();

	// Since each tile MUST be a set height, we can easily calculate the row.
	if (coord.y < 0.0f) {
		sel.y = 0;
	} else if (coord.y >= size.y * awe::tile::MIN_HEIGHT) {
		sel.y = size.y - 1;
	} else {
		sel.y = static_cast<sf::Uint32>(static_cast<float>(coord.y) /
			awe::tile::MIN_HEIGHT);
	}

	// However, since tiles can technically be any width (though they really
	// shouldn't be), we need to iterate through each column.
	sel.x = 0;
	if (coord.x >= 0.0f) {
		for (float xCounter = 0.0f; sel.x < size.x; ++sel.x) {
			auto tileWidth = _tiles[sel.x][sel.y].getPixelSize().x;
			if (tileWidth < static_cast<float>(awe::tile::MIN_WIDTH))
				tileWidth = static_cast<float>(awe::tile::MIN_WIDTH);
			if (coord.x < (xCounter += tileWidth)) break;
		}
		if (sel.x >= size.x) sel.x = size.x - 1;
	}

	setSelectedTile(sel);
}

void awe::map::setAdditionallySelectedTile(const sf::Vector2u& pos) {
	_additionalSel = pos;
}

void awe::map::clearAdditionallySelectedTile() {
	_additionalSel.reset();
	_additionallySelectedTileCursorUL.setCurrentFrame(0);
	_additionallySelectedTileCursorUR.setCurrentFrame(0);
	_additionallySelectedTileCursorLL.setCurrentFrame(0);
	_additionallySelectedTileCursorLR.setCurrentFrame(0);
}

void awe::map::setULAdditionalCursorSprite(const std::string& sprite) {
	_additionallySelectedTileCursorUL.setSprite(sprite);
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setULAdditionalCursorSprite was just given sprite with "
			"ID \"{}\", which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setURAdditionalCursorSprite(const std::string& sprite) {
	_additionallySelectedTileCursorUR.setSprite(sprite);
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setURAdditionalCursorSprite was just given sprite with "
			"ID \"{}\", which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLLAdditionalCursorSprite(const std::string& sprite) {
	_additionallySelectedTileCursorLL.setSprite(sprite);
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLLAdditionalCursorSprite was just given sprite with "
			"ID \"{}\", which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLRAdditionalCursorSprite(const std::string& sprite) {
	_additionallySelectedTileCursorLR.setSprite(sprite);
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLRAdditionalCursorSprite was just given sprite with "
			"ID \"{}\", which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setSelectedArmy(const awe::ArmyID army) {
	if (army == awe::NO_ARMY || _isArmyPresent(army)) {
		if (army == getSelectedArmy()) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::SELECT_ARMY));
		_currentArmy = army;
	} else {
		_logger.error("setSelectedArmy operation cancelled: army with ID {} does "
			"not exist!", army);
	}
}

awe::ArmyID awe::map::getSelectedArmy() const {
	return _currentArmy;
}

awe::ArmyID awe::map::getNextArmy() const {
	if (_currentArmy == awe::NO_ARMY || _armies.size() == 0) return awe::NO_ARMY;
	auto itr = ++_armies.find(_currentArmy);
	if (itr == _armies.end()) itr = _armies.begin();
	return itr->first;
}

awe::ArmyID awe::map::getFirstArmy() const {
	if (_armies.size() == 0) return awe::NO_ARMY;
	else return _armies.cbegin()->first;
}

void awe::map::setMapScalingFactor(const float factor) {
	if (factor <= 0.0f) {
		_logger.error("setMapScalingFactor operation failed: attempted to assign "
			"a map scaling factor {} that was at or below 0.0.", factor);
	} else {
		_scaling = factor;
	}
}

bool awe::map::isCursorOnLeftSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).x < _target->getSize().x / 2.0f;
}

bool awe::map::isCursorOnTopSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).y < _target->getSize().y / 2.0f;
}

awe::quadrant awe::map::getCursorQuadrant() const {
	const bool isTop = isCursorOnTopSide();
	if (isCursorOnLeftSide()) {
		if (isTop) {
			return awe::quadrant::UpperLeft;
		} else {
			return awe::quadrant::LowerLeft;
		}
	} else {
		if (isTop) {
			return awe::quadrant::UpperRight;
		} else {
			return awe::quadrant::LowerRight;
		}
	}
}

void awe::map::setRectangleSelectionStart(const sf::Vector2u& tile) {
	if (!_isOutOfBounds(tile)) _startOfRectSel = tile;
}

void awe::map::setRectangleSelectionEnd(const sf::Vector2u& tile) {
	if (!_isOutOfBounds(tile)) _endOfRectSel = tile;
}

void awe::map::removeRectangleSelection() {
	_startOfRectSel.reset();
	_endOfRectSel.reset();
}

sf::Vector2u awe::map::getRectangleSelectionStart() const {
	return _startOfRectSel ? *_startOfRectSel : sf::Vector2u(0, 0);
}

sf::Vector2u awe::map::getRectangleSelectionEnd() const {
	return _endOfRectSel ? *_endOfRectSel : sf::Vector2u(0, 0);
}

void awe::map::setULCursorSprite(const std::string& sprite) {
	_ulCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setULCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setURCursorSprite(const std::string& sprite) {
	_urCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setURCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLLCursorSprite(const std::string& sprite) {
	_llCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLLCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLRCursorSprite(const std::string& sprite) {
	_lrCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLRCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

sf::IntRect awe::map::getCursorBoundingBox() const {
	const auto pos = _cursor.getPosition(), size = _cursor.getSize();
	const auto ul = _target->mapCoordsToPixel(pos, _view);
	const auto guiScaling = _gui ? static_cast<int>(_gui->getScalingFactor()) : 1;
	return { ul / guiScaling,
		(_target->mapCoordsToPixel(pos + size, _view) - ul) / guiScaling };
}

sf::IntRect awe::map::getMapBoundingBox() const {
	// Map is always drawn at { 0, 0 } before the view is applied.
	auto mapSize = sf::Vector2f(getMapSize());
	mapSize.x *= static_cast<float>(awe::tile::MIN_WIDTH);
	mapSize.y *= static_cast<float>(awe::tile::MIN_HEIGHT);
	const auto ul = _target->mapCoordsToPixel({ 0.0f, 0.0f }, _view);
	return { ul, _target->mapCoordsToPixel(mapSize, _view) - ul };
}

bool awe::map::animate(const sf::RenderTarget& target) {
	// Create map of tiles -> units from _unitLocationOverrides.
	std::unordered_map<sf::Vector2u, awe::UnitID> unitLocationOverrides;
	for (const auto& pair : _unitLocationOverrides)
		unitLocationOverrides[pair.second] = pair.first;

	// Step 1. the tiles.
	// Also update the position of the cursor here!
	const auto mapSize = getMapSize(), selectedTile = getSelectedTile();
	float tiley = 0.0f;
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		float tilex = 0.0f;
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			auto& tile = _tiles[x][y];
			tile.animate(target);

			sf::Vector2u tilePos = sf::Vector2u(x, y);
			sf::Uint32 tileWidth = 0, tileHeight = 0;
			auto type = tile.getTileType();
			if (type) {
				tileWidth = (sf::Uint32)tile.getPixelSize().x;
				tileHeight = (sf::Uint32)tile.getPixelSize().y;
				/*if (tile.getTileOwner() == awe::NO_ARMY) {
					tileWidth = (sf::Uint32)_sheet_tile->getFrameRect(
						type->getNeutralTile()
					).width;
					tileHeight = (sf::Uint32)_sheet_tile->accessSprite(
						type->getNeutralTile()
					).height;
				} else {
					tileWidth = (sf::Uint32)_sheet_tile->accessSprite(
						type->getOwnedTile(tile.getTileOwner())
					).width;
					tileHeight = (sf::Uint32)_sheet_tile->accessSprite(
						type->getOwnedTile(tile.getTileOwner())
					).height;
				}*/
			}
			if (tileWidth < tile.MIN_WIDTH) tileWidth = tile.MIN_WIDTH;
			if (tileHeight < tile.MIN_HEIGHT) tileHeight = tile.MIN_HEIGHT;
			const auto fTileWidth = static_cast<float>(tileWidth);
			tile.setPixelPosition(tilex, tiley -
				static_cast<float>((tileHeight - tile.MIN_HEIGHT)));

			// Update the tile's unit's pixel position accordingly, if it
			// doesn't have an override.
			const auto tilesUnit = tile.getUnit();
			if (tilesUnit && _unitLocationOverrides.find(tilesUnit) ==
				_unitLocationOverrides.end()) {
				_units.at(tilesUnit).setPixelPosition(tilex, tiley);
			}

			// Check if this tile has a unit's location overridded onto it.
			if (unitLocationOverrides.find(tilePos) !=
				unitLocationOverrides.end()) {
				_units.at(unitLocationOverrides[tilePos]).setPixelPosition(
					tilex, tiley
				);
			}

			// Update cursor position.
			const auto cursorPosition = sf::Vector2f(tilex, tiley);
			if (selectedTile == tilePos) _cursor.setPosition(cursorPosition);

			// Update additional cursor position.
			if (_additionalSel && *_additionalSel == tilePos) {
				_additionallySelectedTileCursorUL.setPosition(cursorPosition);
				_additionallySelectedTileCursorUR.setPosition(cursorPosition +
					sf::Vector2f(fTileWidth -
						_additionallySelectedTileCursorUR.getSize().x, 0.f));
				_additionallySelectedTileCursorLL.setPosition(cursorPosition +
					sf::Vector2f(0.f, static_cast<float>(awe::tile::MIN_HEIGHT) -
						_additionallySelectedTileCursorLL.getSize().y));
				_additionallySelectedTileCursorLR.setPosition(cursorPosition +
					sf::Vector2f(fTileWidth,
						static_cast<float>(awe::tile::MIN_HEIGHT)) -
					_additionallySelectedTileCursorLR.getSize());
			}

			tilex += fTileWidth;
		}
		tiley += static_cast<float>(awe::tile::MIN_HEIGHT);
	}

	// Step 2. the selected unit closed list tile icons.
	if (_selectedUnitRenderData.top().selectedUnit != awe::NO_UNIT) {
		for (asUINT i = 0, size = _selectedUnitRenderData.top().closedList->
			GetSize(); i < size; ++i) {
			awe::closed_list_node* pathNode = (awe::closed_list_node*)
				_selectedUnitRenderData.top().closedList->At(i);
			pathNode->sprite.animate(target);
			auto pos =
				_tiles[pathNode->tile.x][pathNode->tile.y].getPixelPosition();
			const auto h =
				_tiles[pathNode->tile.x][pathNode->tile.y].getPixelSize().y;
			if (h > awe::tile::MIN_HEIGHT) pos.y += h - awe::tile::MIN_HEIGHT;
			pathNode->sprite.setPosition(pos);
		}
	}

	// Step 3. the units.
	// Note that unit positioning was carried out in step 1.
	for (auto& unit : _units) unit.second.animate(target);

	// Step 4. the additional cursor.
	if (_additionalSel) {
		_additionallySelectedTileCursorUL.animate(target);
		_additionallySelectedTileCursorUR.animate(target);
		_additionallySelectedTileCursorLL.animate(target);
		_additionallySelectedTileCursorLR.animate(target);
	}

	// Step 5. the cursor.
	const auto quadrant = getCursorQuadrant();
	switch (quadrant) {
	case awe::quadrant::LowerLeft:
		_cursor.setSprite(_llCursorSprite);
		break;
	case awe::quadrant::LowerRight:
		_cursor.setSprite(_lrCursorSprite);
		break;
	case awe::quadrant::UpperRight:
		_cursor.setSprite(_urCursorSprite);
		break;
	default: // Let awe::quadrant::UpperLeft be the default.
		_cursor.setSprite(_ulCursorSprite);
	}
	_cursor.animate(target);

	// Step 6. the rectangle selection graphic. Doesn't take crazy tile widths into
	// account.
	if (_startOfRectSel && _endOfRectSel) {
		const sf::Vector2f rectSelStart(
			static_cast<float>(awe::tile::MIN_WIDTH) * _startOfRectSel->x +
			static_cast<float>(awe::tile::MIN_WIDTH) / 2.0f,
			static_cast<float>(awe::tile::MIN_HEIGHT) * _startOfRectSel->y +
			static_cast<float>(awe::tile::MIN_HEIGHT) / 2.0f
		);
		const sf::Vector2f rectSelEnd(
			static_cast<float>(awe::tile::MIN_WIDTH) * _endOfRectSel->x +
			static_cast<float>(awe::tile::MIN_WIDTH) / 2.0f,
			static_cast<float>(awe::tile::MIN_HEIGHT) * _endOfRectSel->y +
			static_cast<float>(awe::tile::MIN_HEIGHT) / 2.0f
		);
		_rectangle.setPosition(sf::Vector2f{
			std::min(rectSelStart.x, rectSelEnd.x),
			std::min(rectSelStart.y, rectSelEnd.y)
			});
		_rectangle.setSize(sf::Vector2f{
			::abs(rectSelStart.x - rectSelEnd.x),
			::abs(rectSelStart.y - rectSelEnd.y)
			});
		_rectangle.setOutlineThickness(_scaling);
	}

	// Step 7. update the view to match the target's size, and apply the scaling.
	// Additionally, update the view offset.
	auto mapPixelSize = mapSize; // Ignore fancy tile heights and widths.
	mapPixelSize.x *= awe::tile::MIN_WIDTH;
	mapPixelSize.y *= awe::tile::MIN_HEIGHT;
	const auto rect = sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(target.getSize().x) / _scaling,
		static_cast<float>(target.getSize().y) / _scaling);
	const auto cursorRect = sf::FloatRect(
		sf::Vector2f(_target->mapCoordsToPixel(_cursor.getPosition(), _view)),
		_cursor.getSize() * _scaling);
	_view.reset(rect);
	_view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	static const auto moveOffsetAxis = [](const float viewSize,
		const float mapPixelSize, std::optional<float>& viewOffset,
		const float cursorPos, const float cursorSize, const float padding,
		const float screenSize) {
			if (viewSize > mapPixelSize) {
				// Map appears smaller than the screen along this axis, so centre it on
				// that axis. Also, reset the view offset to ensure that when the map
				// appears larger in the future, the view will centre itself on the
				// cursor along this axis.
				viewOffset.reset();
				return -(viewSize / 2.0f - mapPixelSize / 2.0f);
			} else {
				if (viewOffset) {
					// If the cursor is too close to either edge of the screen, adjust
					// the offset to bring the cursor a little further into the centre
					// of the screen.
					if (cursorPos <= padding * 2.0f) {
						*viewOffset -= padding;
					} else if (cursorPos + cursorSize >= screenSize - padding * 2.0f) {
						*viewOffset += padding;
					}
				} else {
					// Map appears larger immediately after appearing smaller. Default
					// offset to centre on the cursor.
					viewOffset = (cursorPos + cursorSize / 2.0f) - viewSize / 2.0f;
				}
				// Make sure the map fully fills the screen along this axis.
				if (*viewOffset < 0.0f) {
					viewOffset = 0.0f;
				} else if (*viewOffset + viewSize >= mapPixelSize) {
					viewOffset = mapPixelSize - viewSize;
				}
				return *viewOffset;
			}
	};
	_view.move(
		moveOffsetAxis(rect.width, static_cast<float>(mapPixelSize.x),
			_viewOffsetX, cursorRect.left, cursorRect.width,
			static_cast<float>(awe::tile::MIN_WIDTH),
			static_cast<float>(target.getSize().x)),
		moveOffsetAxis(rect.height, static_cast<float>(mapPixelSize.y),
			_viewOffsetY, cursorRect.top, cursorRect.height,
			static_cast<float>(awe::tile::MIN_HEIGHT),
			static_cast<float>(target.getSize().y))
	);

	// End.
	return false;
}

void awe::map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Step 1. temporarily apply our view.
	const auto oldView = target.getView();
	target.setView(_view);

	// Step 2. the tiles.
	auto mapSize = getMapSize();
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			if ((_selectedUnitRenderData.top().selectedUnit != awe::NO_UNIT ||
				_selectedUnitRenderData.top().availableTiles.size() > 0) &&
				!_selectedUnitRenderData.top().disableRenderingEffects) {
				sf::Vector2u currentTile(x, y);
				sf::RenderStates tileStates = states;
				if (_selectedUnitRenderData.top().availableTiles.find(currentTile)
					!= _selectedUnitRenderData.top().availableTiles.end()) {
					// Apply configured shading.
					switch (_selectedUnitRenderData.top().availableTileShader) {
					case awe::available_tile_shader::Yellow:
						tileStates.shader = &_availableTileShader;
						break;
					case awe::available_tile_shader::Red:
						tileStates.shader = &_attackableTileShader;
						break;
					}
				} else { // Not an available tile. Grey out.
					tileStates.shader = &_unavailableTileShader;
				}
				target.draw(_tiles[x][y], tileStates);
			} else {
				target.draw(_tiles[x][y], states);
			}
		}
	}

	// Step 3. the selected unit closed list tiles.
	if (_selectedUnitRenderData.top().selectedUnit != awe::NO_UNIT &&
		!_selectedUnitRenderData.top().disableRenderingEffects) {
		for (asUINT i = 0, size = _selectedUnitRenderData.top().closedList->
			GetSize(); i < size; ++i) {
			target.draw(((awe::closed_list_node*)
				_selectedUnitRenderData.top().closedList->At(i))->sprite,
				states);
		}
	}

	// Step 4. the units.
	// Unfortunately units have to be looped through separately to prevent tiles
	// taller than the minimum height from drawing over units. If a unit has a
	// location override, then render it, even if it isn't on the map according to
	// `isUnitOnMap()`. Units with location overrides are also drawn after all
	// other units to ensure they are as visible as possible.
	std::list<std::pair<const awe::unit&, sf::RenderStates>>
		unitsWithLocationOverrides;
	const auto currentArmy = getSelectedArmy();
	for (const auto& unitsPair : _units) {
		const awe::UnitID unitID = unitsPair.first;
		const auto hasLocationOverride =
			_unitLocationOverrides.find(unitID) != _unitLocationOverrides.end();

		if (_isUnitPresent(unitID) && ((isUnitOnMap(unitID) &&
			(_alwaysShowHiddenUnits || isUnitVisible(unitID, currentArmy))) ||
			hasLocationOverride)) {
			sf::RenderStates unitStates = states;
			unitStates.shader = &_unavailableTileShader;
			if (_selectedUnitRenderData.top().selectedUnit != awe::NO_UNIT &&
				!_selectedUnitRenderData.top().disableRenderingEffects &&
				unitID != _selectedUnitRenderData.top().selectedUnit &&
				(!_selectedUnitRenderData.top().disableShaderForAvailableUnits ||
					_selectedUnitRenderData.top().availableTiles.find(
						getUnitPosition(unitID)) ==
					_selectedUnitRenderData.top().availableTiles.end())) {
				if (hasLocationOverride) {
					unitsWithLocationOverrides.emplace_back(_units.at(unitID),
						unitStates);
				} else target.draw(_units.at(unitID), unitStates);
			} else {
				if (isUnitWaiting(unitID)) {
					if (hasLocationOverride) {
						unitsWithLocationOverrides.emplace_back(_units.at(unitID),
							unitStates);
					} else target.draw(_units.at(unitID), unitStates);
				} else {
					if (hasLocationOverride) {
						unitsWithLocationOverrides.emplace_back(_units.at(unitID),
							states);
					} else target.draw(_units.at(unitID), states);
				}
			}
		}
	}
	for (const auto& overridden : unitsWithLocationOverrides)
		target.draw(overridden.first, overridden.second);

	// Step 5. the additional cursor.
	if (_additionalSel && !_additionallySelectedTileCursorUL.getSprite().empty() &&
		!_additionallySelectedTileCursorUR.getSprite().empty() &&
		!_additionallySelectedTileCursorLL.getSprite().empty() &&
		!_additionallySelectedTileCursorLR.getSprite().empty()) {
		target.draw(_additionallySelectedTileCursorUL, states);
		target.draw(_additionallySelectedTileCursorUR, states);
		target.draw(_additionallySelectedTileCursorLL, states);
		target.draw(_additionallySelectedTileCursorLR, states);
	}

	// Step 6. the cursor. Always rendered over the additional cursor.
	if (!_cursor.getSprite().empty()) target.draw(_cursor, states);

	// Step 7. the rectangle selection graphic.
	if (_startOfRectSel && _endOfRectSel) target.draw(_rectangle, states);

	// Step 8. restore old view.
	target.setView(oldView);
}
