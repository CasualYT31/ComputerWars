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

void awe::map::setMapName(std::string name) {
	if (name.find('~') != std::string::npos) {
		_logger.warning("setMapName operation: illegal character \"~\" in new "
			"name \"{}\", removing all instances of character \"~\".", name);
		name = std::regex_replace(name, std::regex("~"), "");
	}
	if (name == getMapName()) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::MAP_NAME));
	_mapName = name;
}

std::string awe::map::getMapName() const {
	return _mapName;
}

void awe::map::setMapSize(const sf::Vector2u& dim,
	const std::shared_ptr<const awe::tile_type>& tile, const awe::ArmyID owner) {
	if (dim == getMapSize()) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::MAP_SIZE));
	removeRectangleSelection();
	// First, resize the tiles vectors accordingly.
	bool mapHasShrunk = (getMapSize().x > dim.x || getMapSize().y > dim.y);
	_tiles.resize(dim.x);
	for (std::size_t x = 0; x < dim.x; ++x) {
		_tiles[x].reserve(dim.y);
		for (std::size_t y = 0; y < dim.y; ++y)
			_tiles[x].emplace_back(
				engine::logger::data{ _logger.getData().sink, "tile" },
				[&](const std::function<void(void)>& func)
					{ _animationQueue.push(func); }, tile, owner,
				(*_sheets)[getEnvironmentSpritesheet()]);
	}
	_mapSizeCache = dim;
	if (mapHasShrunk) {
		// Then, go through all owned tiles in each army and delete those that are
		// now out of bounds.
		for (auto& army : _armies) {
			auto tiles = army.second.getTiles();
			for (auto& tile : tiles) {
				if (_isOutOfBounds(tile)) army.second.removeTile(tile);
			}
		}
		// Then, go through all units and delete those that are out of bounds.
		std::vector<awe::UnitID> unitsToDelete;
		for (auto& itr : _units) {
			if (_isOutOfBounds(itr.second.data.getPosition()))
				unitsToDelete.push_back(itr.first);
		}
		// Check if they are still present, as some of those IDs may be for units
		// that are loaded, which will be deleted as their holder unit is deleted.
		for (auto& itr : unitsToDelete) if (_isUnitPresent(itr)) deleteUnit(itr);
		// Finally, if the currently selected tile is now out of bounds, adjust it.
		if (_isOutOfBounds(_sel)) {
			if (dim.x == 0)
				// Will still be out of bounds: this should be checked for anyway
				// in the drawing code.
				_sel.x = 0;
			else if (_sel.x >= dim.x)
				_sel.x = dim.x - 1;
			if (dim.y == 0)
				// Will still be out of bounds: this should be checked for anyway
				// in the drawing code.
				_sel.y = 0;
			else if (_sel.y >= dim.y)
				_sel.y = dim.y - 1;
		}
	}
}

void awe::map::setMapSize(const sf::Vector2u& dim, const std::string& tile,
	const awe::ArmyID owner) {
	setMapSize(dim, _tileTypes->operator[](tile), owner);
}

bool awe::map::fillMap(const std::shared_ptr<const awe::tile_type>& type,
	const awe::ArmyID owner) {
	if (!type) {
		_logger.error("fillMap operation failed: an empty tile type was given!");
		return false;
	}
	return rectangleFillTiles(sf::Vector2u(0, 0),
		getMapSize() - sf::Vector2u(1, 1), type, owner);
}

bool awe::map::fillMap(const std::string& type, const awe::ArmyID owner) {
	return fillMap(_tileTypes->operator[](type), owner);
}

bool awe::map::rectangleFillTiles(const sf::Vector2u& start,
	const sf::Vector2u& end, const std::shared_ptr<const awe::tile_type>& type,
	const awe::ArmyID owner) {
	if (!type) {
		_logger.error("rectangleFillTiles operation failed: an empty tile type "
			"was given!");
		return false;
	}
	if (_isOutOfBounds(start)) {
		_logger.error("rectangleFillTiles operation failed: the start tile {} is "
			"out of bounds.", start);
		return false;
	}
	if (_isOutOfBounds(end)) {
		_logger.error("rectangleFillTiles operation failed: the end tile {} is "
			"out of bounds.", end);
		return false;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::RECT_FILL_TILES));
	bool ret = true;
	const unsigned int startX = std::min(start.x, end.x),
		startY = std::min(start.y, end.y);
	for (unsigned int x = startX, width = static_cast<unsigned int>(::abs(
		static_cast<sf::Int64>(start.x) - static_cast<sf::Int64>(end.x)) + 1);
		x < startX + width; ++x) {
		for (unsigned int y = startY, height = static_cast<unsigned int>(::abs(
			static_cast<sf::Int64>(start.y) - static_cast<sf::Int64>(end.y)) + 1);
			y < startY + height; ++y) {
			if (!setTileType({ x, y }, type)) ret = false;
			setTileOwner({ x, y }, owner);
		}
	}
	return ret;
}

bool awe::map::rectangleFillTiles(const sf::Vector2u& start,
	const sf::Vector2u& end, const std::string& type, const awe::ArmyID owner) {
	return rectangleFillTiles(start, end, _tileTypes->operator[](type), owner);
}

bool awe::map::rectangleFillUnits(const sf::Vector2u& start,
	const sf::Vector2u& end, const std::shared_ptr<const awe::unit_type>& type,
	const awe::ArmyID army) {
	if (!type) {
		_logger.error("rectangleFillUnits operation failed: an empty tile type "
			"was given!");
		return false;
	}
	if (army == awe::NO_ARMY) {
		_logger.error("rectangleFillUnits operation failed: the given army was "
			"{}, which is invalid!", army);
		return false;
	}
	if (_isOutOfBounds(start)) {
		_logger.error("rectangleFillUnits operation failed: the start tile {} is "
			"out of bounds.", start);
		return false;
	}
	if (_isOutOfBounds(end)) {
		_logger.error("rectangleFillUnits operation failed: the end tile {} is "
			"out of bounds.", end);
		return false;
	}
	if (_countries->getScriptNames().size() <= static_cast<std::size_t>(army)) {
		_logger.error("rectangleFillUnits operation failed: the army ID {} is "
			"invalid.", army);
		return false;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::RECT_FILL_UNITS));
	if (!_isArmyPresent(army)) createArmy(_countries->getScriptNames()[army]);
	bool ret = true;
	const unsigned int startX = std::min(start.x, end.x),
		startY = std::min(start.y, end.y);
	for (unsigned int x = startX, width = static_cast<unsigned int>(::abs(
		static_cast<sf::Int64>(start.x) - static_cast<sf::Int64>(end.x)) + 1);
		x < startX + width; ++x) {
		for (unsigned int y = startY, height = static_cast<unsigned int>(::abs(
			static_cast<sf::Int64>(start.y) - static_cast<sf::Int64>(end.y)) + 1);
			y < startY + height; ++y) {
			auto unit = getUnitOnTile({ x, y });
			if (unit != awe::NO_UNIT) deleteUnit(unit);
			unit = createUnit(type, army);
			if (unit == awe::NO_UNIT) {
				ret = false;
				continue;
			}
			waitUnit(unit, false);
			replenishUnit(unit, true);
			setUnitPosition(unit, { x, y });
		}
	}
	return ret;
}

bool awe::map::rectangleFillUnits(const sf::Vector2u& start,
	const sf::Vector2u& end, const std::string& type, const awe::ArmyID army) {
	return rectangleFillUnits(start, end, _unitTypes->operator[](type), army);
}

std::size_t awe::map::rectangleDeleteUnits(const sf::Vector2u& start,
	const sf::Vector2u& end) {
	if (_isOutOfBounds(start)) {
		_logger.error("rectangleDeleteUnits operation failed: the start tile {} "
			"is out of bounds.", start);
		return 0;
	}
	if (_isOutOfBounds(end)) {
		_logger.error("rectangleDeleteUnits operation failed: the end tile {} is "
			"out of bounds.", end);
		return 0;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::RECT_DEL_UNITS));
	const unsigned int startX = std::min(start.x, end.x),
		startY = std::min(start.y, end.y);
	std::size_t counter = 0;
	for (unsigned int x = startX, width = static_cast<unsigned int>(::abs(
		static_cast<sf::Int64>(start.x) - static_cast<sf::Int64>(end.x)) + 1);
		x < startX + width; ++x) {
		for (unsigned int y = startY, height = static_cast<unsigned int>(::abs(
			static_cast<sf::Int64>(start.y) - static_cast<sf::Int64>(end.y)) + 1);
			y < startY + height; ++y) {
			auto unit = getUnitOnTile({ x, y });
			if (unit != awe::NO_UNIT) {
				deleteUnit(unit);
				++counter;
			}
		}
	}
	return counter;
}

void awe::map::setDay(const awe::Day day) {
	if (day == getDay()) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::DAY));
	_day = day;
}

awe::Day awe::map::getDay() const noexcept {
	return _day;
}

void awe::map::enableFoW(const bool enabled) {
	if (isFoWEnabled() == enabled) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::FOG_OF_WAR));
	_fow = enabled;
}
