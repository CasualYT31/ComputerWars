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

#include "map.hpp"
#include "fmtawe.hpp"

bool awe::map::setTileType(const sf::Vector2u& pos,
	const std::shared_ptr<const awe::tile_type>& type) {
	if (!type) _logger.warning("setTileType warning: assigning the tile at "
		"position {} an empty type!", pos);
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileType operation cancelled: attempted to assign type "
			"\"{}\" to tile at position {}, which is out of bounds with the map's "
			"size of {}!", ((type) ? (type->getType()->getName()) : ("[NULL]")),
			pos, getMapSize());
		return false;
	}
	// Carry on with operation, even if the type given matches with the type the
	// tile already has.
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::TILE_TYPE));
	// Firstly, check if this tile forms part of a structure. If it does, we should
	// delete the structure. This involves the following:
	// 1. Converting each tile's type in the structure to its configured deleted
	//    type, unless one wasn't given.
	// 2. Disowning all tiles in the structure.
	// 3. Removing each tile's configured structure type.
	if (isTileAStructureTile(pos)) {
		const auto structure = getTileStructure(pos);
		const auto offset = getTileStructureOffset(pos);
		// Create list of tiles to update.
		std::vector<std::pair<sf::Vector2u, std::size_t>> tiles;
		tiles.resize(structure->getDependentTileCount() + 1);
		const auto rootTile = sf::Vector2u(pos.x - offset.x, pos.y - offset.y);
		tiles[0] = { rootTile, 0 };
		for (std::size_t i = 0, e = structure->getDependentTileCount(); i < e;
			++i) {
			const sf::Vector2i depOffset = structure->getDependentTileOffset(i);
			tiles[i + 1] = {
				{ rootTile.x + depOffset.x, rootTile.y + depOffset.y }, i
			};
		}
		for (const auto& tile : tiles) {
			// Remove the tile's configured structure now to prevent calling
			// setTileType() infinitely.
			setTileStructureData(tile.first, "", { 0, 0 }, false);
			const auto tileType = tile.first == rootTile ?
				structure->getRootDeletedTileType() :
				structure->getDependentDeletedTileType(tile.second);
			const auto hasTileType = tile.first == rootTile ?
				structure->hasRootDeletedTileType() :
				structure->hasDependentDeletedTileType(tile.second);
			if (hasTileType) {
				// Setting the tile's type will also disown it.
				setTileType(tile.first, tileType);
			} else {
				// We'll have to disown it manually.
				setTileOwner(tile.first, awe::NO_ARMY);
			}
		}
		// Now, allow the caller to set the originally given tile's type.
	}
	const auto unitOnTile = getUnitOnTile(pos);
	// If the tile has a unit on it, update its army's visible tile cache.
	if (unitOnTile != awe::NO_UNIT) {
		auto visibleTiles = getAvailableTiles(pos, 1, getUnitVision(unitOnTile));
		visibleTiles.insert(pos);
		_armies.at(_units.at(unitOnTile).data.getArmy()).removeVisibleTiles(
			visibleTiles);
	}
	_updateCapturingUnit(unitOnTile);
	_tiles[pos.x][pos.y].data.setTileType(type);
	// Set the tile's HP to the max.
	_tiles[pos.x][pos.y].data.setTileHP(
		static_cast<awe::HP>(type->getType()->getMaxHP()));
	// Remove ownership of the tile from the army who owns it, if any army does.
	setTileOwner(pos, awe::NO_ARMY);
	// If the tile has a unit on it, update its army's visible tile cache.
	if (unitOnTile != awe::NO_UNIT) {
		auto visibleTiles = getAvailableTiles(pos, 1, getUnitVision(unitOnTile));
		visibleTiles.insert(pos);
		_armies.at(_units.at(unitOnTile).data.getArmy()).addVisibleTiles(
			visibleTiles);
	}
	return true;
}

bool awe::map::setTileType(const sf::Vector2u& pos, const std::string& type) {
	return setTileType(pos, _tileTypes->operator[](type));
}

std::unordered_set<sf::Vector2u> awe::map::querySetTileTypeChangedTiles(
	const sf::Vector2u& pos) const {
	std::unordered_set<sf::Vector2u> changing = { pos };
	if (isTileAStructureTile(pos)) {
		const auto structure = getTileStructure(pos);
		const auto offset = getTileStructureOffset(pos);
		// Should really get rid of the duplicate code in setTileType().
		const auto rootTile = sf::Vector2u(pos.x - offset.x, pos.y - offset.y);
		changing.insert(rootTile);
		for (std::size_t i = 0, e = structure->getDependentTileCount(); i < e;
			++i) {
			const sf::Vector2i depOffset = structure->getDependentTileOffset(i);
			changing.insert(
				{ rootTile.x + depOffset.x, rootTile.y + depOffset.y });
		}
	}
	return changing;
}

CScriptArray* awe::map::querySetTileTypeChangedTilesAsArray(
	const sf::Vector2u& pos) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		querySetTileTypeChangedTiles(pos));
}

std::shared_ptr<const awe::tile_type> awe::map::getTileType(
	const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileType operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return nullptr;
	}
	return _tiles[pos.x][pos.y].data.getTileType();
}

const awe::tile_type* awe::map::getTileTypeObject(const sf::Vector2u& pos) const {
	auto ret = getTileType(pos);
	if (ret) {
		return ret.get();
	} else {
		throw std::out_of_range("This tile does not exist!");
	}
}

void awe::map::setTileHP(const sf::Vector2u& pos, const awe::HP hp) {
	if (!_isOutOfBounds(pos)) {
		if (hp == getTileHP(pos)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::TILE_HP));
		_tiles[pos.x][pos.y].data.setTileHP(hp);
	} else {
		_logger.error("setTileHP operation cancelled: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
	}
}

awe::HP awe::map::getTileHP(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileHP operation failed: tile at position {} is out of "
			"bounds with the map's size of {}!", pos, getMapSize());
		return 0;
	}
	return _tiles[pos.x][pos.y].data.getTileHP();
}

void awe::map::setTileOwner(const sf::Vector2u& pos, awe::ArmyID army) {
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileOwner operation cancelled: army with ID {} couldn't "
			"be assigned to tile at position {}, as it is out of bounds with the "
			"map's size of {}!", army, pos, getMapSize());
		return;
	}
	if (army == getTileOwner(pos)) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::TILE_OWNER));
	_updateCapturingUnit(getUnitOnTile(pos));
	auto& tile = _tiles[pos.x][pos.y];
	// First, remove the tile from the army who currently owns it.
	if (_isArmyPresent(tile.data.getTileOwner()))
		_armies.at(tile.data.getTileOwner()).removeTile(pos);
	// Now assign it to the real owner, if any.
	if (_isArmyPresent(army)) _armies.at(army).addTile(pos);
	// Update the actual tile now.
	tile.data.setTileOwner(army);
}

awe::ArmyID awe::map::getTileOwner(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileOwner operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return awe::NO_ARMY;
	}
	return _tiles[pos.x][pos.y].data.getTileOwner();
}

awe::UnitID awe::map::getUnitOnTile(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getUnitOnTile operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return awe::NO_UNIT;
	}
	auto u = _tiles[pos.x][pos.y].data.getUnit();
	if (u != awe::NO_UNIT && _units.at(u).data.isOnMap()) return u;
	return awe::NO_UNIT;
}

void awe::map::setTileStructureData(const sf::Vector2u& pos,
	const std::shared_ptr<const awe::structure>& structure,
	const sf::Vector2i& offset, const bool destroyed) {
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileStructureData operation failed: tile at position {} "
			"is out-of-bounds with the map's size of {}!", pos, getMapSize());
		return;
	}
	if (_isOutOfBounds({ pos.x - offset.x, pos.y - offset.y })) {
		_logger.error("setTileStructureData operation failed: tile at position {} "
			"was given an offset of {}, but that computes to an out-of-bounds "
			"root tile, with the map's size at {}!", pos, offset, getMapSize());
		return;
	}

	// Log warnings if the input data is not expected.
	if (structure) {
		const std::string tileType = getTileType(pos)->getScriptName();
		if (offset.x == 0 && offset.y == 0) {
			if (tileType != structure->getRootTileTypeScriptName() &&
				tileType != structure->getRootDestroyedTileTypeScriptName()) {
				_logger.warning("setTileStructureData: tile at position {} is "
					"being configured as the root of structure \"{}\", but its "
					"type, \"{}\", is not either of the structure's configured "
					"normal (\"{}\") or destroyed (\"{}\") types.", pos,
					structure->getScriptName(), tileType,
					structure->getRootTileTypeScriptName(),
					structure->getRootDestroyedTileTypeScriptName());
			}
		} else {
			const auto count = structure->getDependentTileCount();
			std::size_t i = 0;
			for (; i < count; ++i) {
				if (structure->getDependentTileOffset(i) == offset) {
					if (tileType != structure->getDependentTileTypeScriptName(i) &&
						tileType !=
						structure->getDependentDestroyedTileTypeScriptName(i)) {
						_logger.warning("setTileStructureData: tile at position "
							"{} is being configured as the dependent of structure "
							"\"{}\" with offset {}, but its type, \"{}\", is not "
							"either of the structure's configured normal (\"{}\") "
							"or destroyed (\"{}\") types for this offset.", pos,
							structure->getScriptName(), offset, tileType,
							structure->getDependentTileTypeScriptName(i),
							structure->getDependentDestroyedTileTypeScriptName(i));
					}
					break;
				}
			}
			if (i == count) {
				_logger.warning("setTileStructureData: tile at position {} is "
					"being configured as the dependent of structure \"{}\" with "
					"offset {}, but this structure does not have this configured "
					"offset!", pos, structure->getScriptName(), offset);
			}
		}
	}

	if (structure == getTileStructure(pos) &&
		offset == getTileStructureOffset(pos) &&
		destroyed == isTileDestroyed(pos)) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::TILE_STRUCTURE_DATA));
	auto& tile = _tiles[pos.x][pos.y];
	tile.data.setStructureType(structure);
	tile.data.setStructureTile(offset);
	tile.data.setStructureDestroyed(destroyed);
}

void awe::map::setTileStructureData(const sf::Vector2u& pos,
	const std::string& structure, const sf::Vector2i& offset,
	const bool destroyed) {
	if (structure.empty()) setTileStructureData(pos,
		static_cast<std::shared_ptr<const awe::structure>>(nullptr), offset,
		destroyed);
	else setTileStructureData(pos, (*_structures)[structure], offset, destroyed);
}

std::shared_ptr<const awe::structure> awe::map::getTileStructure(
	const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileStructure operation failed: tile at position {} is "
			"out of bounds with the map's size of {}!", pos, getMapSize());
		return nullptr;
	}
	return _tiles[pos.x][pos.y].data.getStructureType();
}

const awe::structure* awe::map::getTileStructureObject(
	const sf::Vector2u& pos) const {
	auto ret = getTileStructure(pos);
	if (ret) return ret.get();
	else throw std::out_of_range("This tile does not exist!");
}

bool awe::map::isTileAStructureTile(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("isTileAStructureTile operation failed: tile at position {} "
			"is out of bounds with the map's size of {}!", pos, getMapSize());
		return false;
	}
	return _tiles[pos.x][pos.y].data.getStructureType().operator bool();
}

sf::Vector2i awe::map::getTileStructureOffset(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileStructureOffset operation failed: tile at position "
			"{} is out of bounds with the map's size of {}!", pos, getMapSize());
		return { 0, 0 };
	}
	return _tiles[pos.x][pos.y].data.getStructureTile();
}

bool awe::map::isTileDestroyed(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("isTileDestroyed operation failed: tile at position {} is "
			"out of bounds with the map's size of {}!", pos, getMapSize());
		return false;
	}
	return _tiles[pos.x][pos.y].data.getStructureDestroyed();
}

bool awe::map::isTileVisible(const sf::Vector2u& pos,
	const awe::ArmyID army) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("isTileVisible operation failed: tile at position {} is "
			"out of bounds with the map's size of {}!", pos, getMapSize());
		return false;
	}
	if (army == awe::NO_ARMY) return isFoWEnabled();
	if (!_isArmyPresent(army)) {
		_logger.error("isTileVisible operation failed: army with ID {} doesn't "
			"exist!", army);
		return false;
	}
	// A tile is visible if...
	// 1. Fog of War is disabled.
	if (!isFoWEnabled()) return true;
	// 2. It is of a terrain type that is always visible.
	const auto terrain = _tiles[pos.x][pos.y].data.getTileType()->getType();
	if (terrain->getFoWVisibility() == awe::terrain::fow_visibility::Visible)
		return true;
	// 3. It is owned by a teammate.
	const auto team = getArmyTeam(army);
	const auto tileOwner = getTileOwner(pos);
	if (tileOwner != awe::NO_ARMY && team == getArmyTeam(tileOwner)) return true;
	// 4. It is within an army's cache of visible tiles...
	for (const auto& army : _armies) {
		if (team != army.second.getTeam()) continue;
		if (army.second.isTileVisible(pos)) {
			// ...unless it is of a terrain type that is configured to be hidden,
			// and none of its adjacent tiles (or the tile itself) is occupied by a
			// teammate...
			if (terrain->getFoWVisibility() !=
				awe::terrain::fow_visibility::Hidden) return true;
			// (This may be too slow if you want to increase this range at all. In
			// which case you will have to update the army visible tile cache to
			// incorporate this logic instead of leaving it here.)
			auto adjacentTiles = getAvailableTiles(pos, 1, 1);
			adjacentTiles.insert(pos);
			for (const auto& tile : adjacentTiles) {
				const auto unitOnTile = getUnitOnTile(tile);
				if (unitOnTile == awe::NO_UNIT) continue;
				if (getTeamOfUnit(unitOnTile) == team) return true;
			}
			// ...in which case it is invisible.
			break;
		}
	}
	// Otherwise, the tile is invisible to the army.
	return false;
}

std::unordered_set<sf::Vector2u> awe::map::getAvailableTiles(
	const sf::Vector2u& tile, unsigned int startFrom,
	const unsigned int endAt) const {
	// Checking.
	const sf::Vector2u mapSize = getMapSize();
	if (_isOutOfBounds(tile)) {
		_logger.error("getAvailableTiles operation failed: tile at position {} is "
			"out of bounds with the map's size of {}!", tile, mapSize);
		return {};
	}
	if (startFrom == 0) startFrom = 1;
	if (startFrom > endAt) return {};

	std::unordered_set<sf::Vector2u> tiles;
	// Get highest tile in range and add it to the list for consideration.
	unsigned int widthOfLine = 1;
	sf::Vector2<int64_t> highest = { (int64_t)tile.x,
		(int64_t)tile.y - (int64_t)endAt };
	if (highest.y < 0) {
		widthOfLine += (unsigned int)(2 * abs(highest.y + 1) + 1);
		highest.y = 0;
	}
	// Now go down the map and add tiles as appropriate, until we either hit the
	// bottom of the map or the end of the given range.
	while (true) {
		int leftMostX = (int)tile.x - (int)(widthOfLine / 2),
			rightMostX = (int)tile.x + (int)(widthOfLine / 2);
		for (int64_t x = (leftMostX < 0 ? 0 : leftMostX);
			x <= rightMostX && x < mapSize.x; ++x) {
			sf::Vector2u newTile = { (unsigned int)x, (unsigned int)highest.y };
			if (distance(newTile, tile) >= startFrom) tiles.insert(newTile);
		}
		++highest.y;
		if (highest.y > tile.y + endAt || highest.y >= mapSize.y) break;
		if (highest.y <= tile.y)
			widthOfLine += 2;
		else
			widthOfLine -= 2;
	}
	return tiles;
}

CScriptArray* awe::map::getAvailableTilesAsArray(
	const sf::Vector2u& tile, const unsigned int startFrom,
	const unsigned int endAt) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		getAvailableTiles(tile, startFrom, endAt));
}

std::unordered_set<sf::Vector2u> awe::map::getTilesInCone(sf::Vector2u tile,
	awe::direction dir, const unsigned int startFrom,
	const unsigned int endAt) const {
	const sf::Vector2u mapSize = getMapSize();
	if (_isOutOfBounds(tile)) {
		_logger.error("getTilesInCone operation failed: tile at position {} is "
			"out-of-bounds with the map's size of {}!", tile, mapSize);
		return {};
	}
	if (startFrom > endAt) return {};
	std::unordered_set<sf::Vector2u> tiles;
	int iterateX = -1, iterateY = -1;
	switch (dir) {
	case awe::direction::Down:
		iterateY = 1;
		break;
	case awe::direction::Right:
		iterateX = 1;
		break;
	}
	tile.x += startFrom * iterateX;
	tile.y += startFrom * iterateY;
	for (unsigned int coneStrip = startFrom; coneStrip <= endAt; ++coneStrip) {
		const auto oldSize = tiles.size();
		for (unsigned int rowOrCol = 0; rowOrCol <= 2 * coneStrip; ++rowOrCol) {
			sf::Vector2u tileToAdd;
			if (dir == awe::direction::Up || dir == awe::direction::Down)
				tileToAdd = { tile.x + rowOrCol, tile.y };
			else
				tileToAdd = { tile.x, tile.y + rowOrCol };
			if (!_isOutOfBounds(tileToAdd)) tiles.insert(tileToAdd);
		}
		if (oldSize == tiles.size()) break;
		tile.x += iterateX;
		tile.y += iterateY;
	}
	return tiles;
}

CScriptArray* awe::map::getTilesInConeAsArray(const sf::Vector2u& tile,
	const direction dir, const unsigned int startFrom,
	const unsigned int endAt) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		getTilesInCone(tile, dir, startFrom, endAt));
}

std::unordered_set<sf::Vector2u> awe::map::getTilesInCrosshair(
	const sf::Vector2u& tile) const {
	const sf::Vector2u mapSize = getMapSize();
	if (_isOutOfBounds(tile)) {
		_logger.error("getTilesInCrosshair operation failed: tile at position {} "
			"is out-of-bounds with the map's size of {}!", tile, mapSize);
		return {};
	}
	std::unordered_set<sf::Vector2u> tiles = { tile };
	for (unsigned int x = 0; x < mapSize.x; ++x) tiles.insert({ x, tile.y });
	for (unsigned int y = 0; y < mapSize.y; ++y) tiles.insert({ tile.x, y });
	return tiles;
}

CScriptArray* awe::map::getTilesInCrosshairAsArray(
	const sf::Vector2u& tile) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		getTilesInCrosshair(tile));
}

std::unordered_set<sf::Vector2u> awe::map::getTilesInLine(sf::Vector2u tile,
	const direction dir, unsigned int distance) const {
	const sf::Vector2u mapSize = getMapSize();
	if (_isOutOfBounds(tile)) {
		_logger.error("getTilesInLine operation failed: tile at position {} is "
			"out-of-bounds with the map's size of {}!", tile, mapSize);
		return {};
	}
	if (distance == 0) {
		switch (dir) {
		case awe::direction::Up:
			distance = tile.y;
			break;
		case awe::direction::Down:
			distance = mapSize.y - tile.y - 1;
			break;
		case awe::direction::Left:
			distance = tile.x;
			break;
		case awe::direction::Right:
			distance = mapSize.x - tile.x - 1;
			break;
		}
	}
	std::unordered_set<sf::Vector2u> tiles = { tile };
	for (unsigned int tileNumber = 0; tileNumber < distance; ++tileNumber) {
		switch (dir) {
		case awe::direction::Up:
			--tile.y;
			break;
		case awe::direction::Down:
			++tile.y;
			break;
		case awe::direction::Left:
			--tile.x;
			break;
		case awe::direction::Right:
			++tile.x;
			break;
		}
		if (_isOutOfBounds(tile)) {
			_logger.warning("getTilesInLine operation warning: a distance of {} "
				"was specified, from tile at position {}, which would've resulted "
				"in out-of-bounds tiles being returned (with map size of {}). "
				"Breaking from operation early.", distance, tile, mapSize);
			break;
		}
		tiles.insert(tile);
	}
	return tiles;
}

CScriptArray* awe::map::getTilesInLineAsArray(const sf::Vector2u& tile,
	const direction dir, const unsigned int distance) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		getTilesInLine(tile, dir, distance));
}

std::unordered_set<sf::Vector2u> awe::map::getTilesInArea(sf::Vector2u tile1,
	sf::Vector2u tile2) const {
	const sf::Vector2u mapSize = getMapSize();
	if (mapSize.x == 0 || mapSize.y == 0) return {};
	if (tile1.x >= mapSize.x) tile1.x = mapSize.x - 1;
	if (tile1.y >= mapSize.y) tile1.y = mapSize.y - 1;
	if (tile2.x >= mapSize.x) tile2.x = mapSize.x - 1;
	if (tile2.y >= mapSize.y) tile2.y = mapSize.y - 1;
	std::unordered_set<sf::Vector2u> tiles;
	const unsigned int startX = std::min(tile1.x, tile2.x),
		endX = std::max(tile1.x, tile2.x), startY = std::min(tile1.y, tile2.y),
		endY = std::max(tile1.y, tile2.y);
	for (unsigned int x = startX; x <= endX; ++x)
		for (unsigned int y = startY; y <= endY; ++y)
			tiles.insert({ x, y });
	return tiles;
}

CScriptArray* awe::map::getTilesInAreaAsArray(const sf::Vector2u& tile1,
	const sf::Vector2u& tile2) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("Vector2",
		getTilesInArea(tile1, tile2));
}

std::vector<awe::closed_list_node> awe::map::findPath(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const unsigned int* const movePoints, const awe::Fuel* const fuel,
	const awe::TeamID* const team, const awe::ArmyID* const army,
	const bool hasInfiniteFuel, const bool ignoreUnitChecks,
	const std::unordered_set<awe::UnitID>& ignoredUnits) const {
	// openSet could be a min-heap or priority queue for added efficiency.
	std::unordered_set<sf::Vector2u> openSet = { origin };
	std::unordered_map<sf::Vector2u, sf::Vector2u> cameFrom;
	std::unordered_map<sf::Vector2u, int> gScore = { {origin, 0} };
	std::unordered_map<sf::Vector2u, int> fScore = { {origin, 0} };

	while (!openSet.empty()) {
		bool firstElement = true;
		sf::Vector2u currentTile;
		for (auto& node : openSet) {
			if (firstElement) {
				firstElement = false;
				currentTile = node;
			} else if (fScore.at(node) < fScore.at(currentTile)) {
				currentTile = node;
			}
		}

		if (currentTile == dest) {
			// Path found.
			std::vector<awe::closed_list_node> ret =
			{ {currentTile, gScore[currentTile]} };
			while (cameFrom.find(currentTile) != cameFrom.end()) {
				currentTile = cameFrom[currentTile];
				ret.insert(ret.begin(), { currentTile, gScore[currentTile] });
			}
			return ret;
		}

		openSet.erase(currentTile);
		auto adjacentTiles = getAvailableTiles(currentTile, 1, 1);
		for (auto& adjacentTile : adjacentTiles) {
			// Get the movement cost for this terrain.
			const auto moveCost = getTileType(adjacentTile)->getType()->
				getMoveCost(moveType.getScriptName());

			// If this unit cannot traverse the terrain, do not add it to any set.
			if (moveCost < 0) continue;

			int tentativeGScore = gScore[currentTile] + moveCost;

			// If:
			// 1. The unit does not have enough fuel (if it has finite fuel).
			// 2. The unit has ran out of movement points.
			// 3. The tile has a unit belonging to an opposing team that isn't
			//    ignored or invisible/hidden.
			// then it cannot traverse the tile, so don't add it to the open set.
			const auto unitOnAdjacentTile = getUnitOnTile(adjacentTile);
			const auto fuelCheck =
				hasInfiniteFuel || fuel == nullptr || tentativeGScore <= *fuel;
			const auto mpCheck = movePoints == nullptr ||
				static_cast<unsigned int>(tentativeGScore) <= *movePoints;
			const auto unitCheck = ignoreUnitChecks ||
				!_isUnitPresent(unitOnAdjacentTile) ||
				(ignoredUnits.find(unitOnAdjacentTile) != ignoredUnits.end()) ||
				(army == nullptr || !isUnitVisible(unitOnAdjacentTile, *army)) ||
				(team != nullptr && getTeamOfUnit(unitOnAdjacentTile) == *team);
			if (fuelCheck && mpCheck && unitCheck) {
				if (gScore.find(adjacentTile) == gScore.end() ||
					tentativeGScore < gScore[adjacentTile]) {
					cameFrom[adjacentTile] = currentTile;
					gScore[adjacentTile] = tentativeGScore;
					fScore[adjacentTile] =
						tentativeGScore + awe::distance(adjacentTile, dest);
					if (openSet.find(adjacentTile) == openSet.end()) {
						openSet.insert(adjacentTile);
					}
				}
			}
		}
	}

	return {};
}

CScriptArray* awe::map::findPathAsArray(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const unsigned int movePoints, const awe::Fuel fuel,
	const awe::TeamID team, const awe::ArmyID army, const bool hasInfiniteFuel,
	const bool ignoreUnitChecks, const CScriptArray* const ignoredUnits) const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto vec = findPath(origin, dest, moveType, &movePoints, &fuel, &team,
		&army, hasInfiniteFuel, ignoreUnitChecks,
		engine::ConvertCScriptArray<std::unordered_set<awe::UnitID>, awe::UnitID>(
			ignoredUnits));
	CScriptArray* ret = _scripts->createArray("ClosedListNode");
	for (auto& node : vec) {
		ret->InsertLast(&awe::closed_list_node());
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->tile = node.tile;
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->g = node.g;
	}
	return ret;
}

CScriptArray* awe::map::findPathAsArrayUnloadUnit(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const awe::ArmyID army, const CScriptArray* const ignoredUnits) const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto vec = findPath(origin, dest, moveType, nullptr, nullptr, nullptr,
		&army, true, false,
		engine::ConvertCScriptArray<std::unordered_set<awe::UnitID>, awe::UnitID>(
			ignoredUnits));
	CScriptArray* ret = _scripts->createArray("ClosedListNode");
	for (auto& node : vec) {
		ret->InsertLast(&awe::closed_list_node());
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->tile = node.tile;
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->g = node.g;
	}
	return ret;
}

int awe::map::scanPath(CScriptArray* path, const awe::UnitID unit,
	std::size_t ignores) const {
	if (path && _isUnitPresent(unit)) {
		asUINT len = path->GetSize();
		const auto armyID = getArmyOfUnit(unit);
		for (asUINT i = 0; i < len; ++i) {
			const auto unitID =
				getUnitOnTile(((awe::closed_list_node*)path->At(i))->tile);
			if (_isUnitPresent(unitID)) {
				if (!isUnitVisible(unitID, armyID)) {
					if (ignores == 0) {
						if (path) path->Release();
						return static_cast<int>(i);
					} else {
						--ignores;
					}
				}
			}
		}
	} else {
		_logger.error("scanPath operation failed: unit with ID {} exist{}",
			((_isUnitPresent(unit)) ? ("does") : ("does not")),
			((path) ? (", path is not NULL.") : (", path is NULL.")));
	}
	if (path) path->Release();
	return -1;
}

void awe::map::convertTiles(const std::vector<sf::Vector2u>& tiles,
	const std::shared_ptr<const awe::tile_type>& fromTileType,
	const std::shared_ptr<const awe::tile_type>& toTileType,
	const awe::ArmyID transferOwnership) {
	if (!fromTileType) {
		_logger.error("convertTiles operation failed: a tile type filter was not "
			"given!");
		return;
	}
	if (!toTileType) {
		_logger.error("convertTiles operation failed: the tile type to convert to "
			"was not given!");
		return;
	}
	if (transferOwnership != awe::NO_ARMY && !_isArmyPresent(transferOwnership)) {
		_logger.error("convertTiles operation failed: cannot transfer ownership "
			"of converted tiles to non-existent army {}.", transferOwnership);
		return;
	}
	for (const auto& tile : tiles) {
		if (_isOutOfBounds(tile)) {
			_logger.error("convertTiles operation: cannot convert tile {} to the "
				"tile type \"{}\", with new owner {}: tile is out-of-bounds!",
				tile, toTileType->getScriptName(), transferOwnership);
			continue;
		}
		if (getTileType(tile)->getScriptName() == fromTileType->getScriptName()) {
			setTileType(tile, toTileType);
			setTileOwner(tile, transferOwnership);
		}
	}
}

void awe::map::convertTiles(const CScriptArray* const tiles,
	const std::string& fromTileType, const std::string& toTileType,
	const awe::ArmyID transferOwnership) {
	convertTiles(engine::ConvertCScriptArray<std::vector<sf::Vector2u>,
		sf::Vector2u>(tiles), (*_tileTypes)[fromTileType],
		(*_tileTypes)[toTileType], transferOwnership);
}

bool awe::map::canStructureFit(const sf::Vector2u& fromTile,
	const std::shared_ptr<const awe::structure>& structure) const {
	if (_isOutOfBounds(fromTile)) return false;
	for (std::size_t i = 0, e = structure->getDependentTileCount(); i < e; ++i) {
		// Cba dealing with integer overflow.
		const auto& offset = structure->getDependentTileOffset(i);
		if (_isOutOfBounds({ fromTile.x + offset.x, fromTile.y + offset.y }))
			return false;
	}
	return true;
}

bool awe::map::canStructureFit(const sf::Vector2u& fromTile,
	const std::string& structure) const {
	return canStructureFit(fromTile, (*_structures)[structure]);
}

void awe::map::destroyStructure(sf::Vector2u tile) {
	if (_isOutOfBounds(tile)) {
		_logger.error("destroyStructure operation failed: tile at position {} is "
			"out-of-bounds with the map's size of {}!", tile, getMapSize());
		return;
	}
	const auto& structure = _tiles[tile.x][tile.y].data.getStructureType();
	if (!structure) {
		_logger.error("destroyStructure operation failed: tile at position {} is "
			"not attached to any structure!", tile);
		return;
	}
	// If the given tile was a dependent tile, find the root tile first.
	auto offset = _tiles[tile.x][tile.y].data.getStructureTile();
	if (offset != sf::Vector2i{ 0, 0 }) {
		tile.x -= offset.x;
		tile.y -= offset.y;
	}
	// Destroy each tile and set their owner to NO_ARMY. Reassign the structure
	// data as setTileType() will delete the structure, which will disown all tiles
	// for us.
	// Carry on with operation even if the structure is already destroyed.
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::DESTROY_STRUCTURE));
	setTileType(tile, structure->getRootDestroyedTileType());
	setTileStructureData(tile, structure, { 0, 0 }, true);
	for (std::size_t i = 0, c = structure->getDependentTileCount(); i < c; ++i) {
		offset = structure->getDependentTileOffset(i);
		const auto depTile = sf::Vector2u{ tile.x + offset.x, tile.y + offset.y };
		setTileType(depTile, structure->getDependentDestroyedTileType(i));
		setTileStructureData(depTile, structure, offset, true);
	}
}

void awe::map::deleteStructure(sf::Vector2u tile) {
	if (_isOutOfBounds(tile)) {
		_logger.error("deleteStructure operation failed: tile at position {} is "
			"out-of-bounds with the map's size of {}!", tile, getMapSize());
		return;
	}
	const auto& structure = _tiles[tile.x][tile.y].data.getStructureType();
	if (!structure) {
		_logger.error("deleteStructure operation failed: tile at position {} is "
			"not attached to any structure!", tile);
		return;
	}
	// If the given tile was a dependent tile, find the root tile first.
	auto offset = _tiles[tile.x][tile.y].data.getStructureTile();
	if (offset != sf::Vector2i{ 0, 0 }) {
		tile.x -= offset.x;
		tile.y -= offset.y;
	}
	// Delete each tile. setTileType() will carry out disowning each tile and
	// deleting the structure data for us.
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::DELETE_STRUCTURE));
	setTileType(tile, structure->getRootDeletedTileType());
	for (std::size_t i = 0, c = structure->getDependentTileCount(); i < c; ++i) {
		offset = structure->getDependentTileOffset(i);
		const auto depTile = sf::Vector2u{ tile.x + offset.x, tile.y + offset.y };
		setTileType(depTile, structure->getDependentDeletedTileType(i));
	}
}

std::shared_ptr<const awe::structure> awe::map::getTileTypeStructure(
	const std::shared_ptr<const awe::tile_type>& type) const {
	for (const auto& structure : *_structures) {
		if (!structure.second->isPaintable() &&
			structure.second->getRootTileType() == type) return structure.second;
	}
	return nullptr;
}

std::string awe::map::getTileTypeStructure(const std::string& type) const {
	const auto structure = getTileTypeStructure((*_tileTypes)[type]);
	if (structure) return structure->getScriptName();
	else return "";
}
