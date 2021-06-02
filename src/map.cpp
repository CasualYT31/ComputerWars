/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

#include "map.h"

awe::map::map(const std::string& name) noexcept : _logger(name) {}

awe::map::map(const std::string& file, const std::shared_ptr<awe::bank<awe::country>>& countries, const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
		const std::shared_ptr<awe::bank<awe::unit_type>>& units, const unsigned char version, const std::string& name) noexcept : _logger(name) {
	try {
		if (!countries) throw std::exception("no country bank provided!");
		_file.open(file, true);
		_filename = file;
		_CWM_Header(false, countries, tiles, units, version);
		_file.close();
	} catch (std::exception& e) {
		_logger.error("Map loading operation: couldn't load map file \"{}\": {}", file, e.what());
		_file.close();
	}
}

bool awe::map::save(std::string file, const unsigned char version) noexcept {
	if (file == "") file = _filename;
	try {
		_file.open(file, false);
		_filename = file;
		_CWM_Header(true, nullptr, nullptr, nullptr, version);
		_file.close();
	} catch (std::exception& e) {
		_logger.error("Map saving operation: couldn't save map file \"{}\": {}", file, e.what());
		_file.close();
		return false;
	}
	return true;
}

void awe::map::setMapName(const std::string& name) noexcept {
	_mapName = name;
}

std::string awe::map::getMapName() const noexcept {
	return _mapName;
}

void awe::map::setMapSize(const sf::Vector2u dim, const std::shared_ptr<const awe::tile_type>& tile) noexcept {
	// first, resize the tiles vectors accordingly
	bool mapHasShrunk = (getMapSize().x > dim.x || getMapSize().y > dim.y);
	_tiles.resize(dim.x);
	for (std::size_t x = 0; x < dim.x; x++) {
		_tiles[x].resize(dim.y, { tile, _sheet_tile });
	}
	if (mapHasShrunk) {
		// then, go through all owned tiles in each army and delete those that are now out of bounds
		for (auto army : _armys) {
			auto tiles = army.second.getTiles();
			for (auto tile : tiles) {
				if (_isOutOfBounds(tile)) army.second.removeTile(tile);
			}
		}
		// then, go through all units and delete those that are out of bounds
		std::vector<awe::UnitID> unitsToDelete;
		for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
			if (_isOutOfBounds(itr->second.getPosition())) unitsToDelete.push_back(itr->first);
		}
		for (auto itr = unitsToDelete.begin(), enditr = unitsToDelete.end(); itr != enditr; itr++) {
			deleteUnit(*itr);
		}
		// finally, if the currently selected tile is now out of bounds, adjust it
		if (_isOutOfBounds(_sel)) {
			if (dim.x == 0)
				_sel.x = 0; // will still be out of bounds: this should be checked for anyway in the drawing code
			else if (_sel.x >= dim.x)
				_sel.x = dim.x - 1;
			if (dim.y == 0)
				_sel.y = 0; // will still be out of bounds: this should be checked for anyway in the drawing code
			else if (_sel.y >= dim.y)
				_sel.y = dim.y - 1;
		}
	}
}

sf::Vector2u awe::map::getMapSize() const noexcept {
	sf::Vector2u ret((unsigned int)_tiles.size(), 0);
	if (ret.x) ret.y = (unsigned int)_tiles.at(0).size();
	return ret;
}

bool awe::map::createArmy(const std::shared_ptr<const awe::country>& country) noexcept {
	if (!country) {
		_logger.error("createArmy operation cancelled: attempted to create an army with no country!");
		return false;
	}
	if (_isArmyPresent(country->getID())) {
		_logger.error("createArmy operation cancelled: attempted to create an army with a country, \"{}\", that already existed on the map!", country->getName());
		return false;
	}
	_armys.insert(std::pair<awe::BankID, awe::army>(country->getID(), country));
	return true;
}

void awe::map::deleteArmy(const awe::ArmyID army) noexcept {
	if (!_isArmyPresent(army)) {
		_logger.error("deleteArmy operation cancelled: attempted to delete an army, {}, that didn't exist on the map!", army);
		return;
	}
	// firstly, delete all units belonging to the army
	auto units = _armys[army].getUnits();
	for (auto unit : units) {
		deleteUnit(unit);
	}
	// then, disown all tiles
	auto tiles = _armys[army].getTiles();
	for (auto tile : tiles) {
		_tiles[tile.x][tile.y].setTileOwner(awe::army::NO_ARMY);
	}
	// finally, delete the army from the army list
	_armys.erase(army);
}

void awe::map::setArmyFunds(const awe::ArmyID army, const awe::Funds funds) noexcept {
	if (_isArmyPresent(army)) {
		_armys[army].setFunds(funds);
	} else {
		_logger.error("setArmyFunds operation cancelled: attempted to set {} funds to an army, {}, that didn't exist!", funds, army);
	}
}

awe::Funds awe::map::getArmyFunds(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armys.at(army).getFunds();
	_logger.error("getArmyFunds operation failed: army with ID {} didn't exist at the time of calling!", army);
	return 0;
}

std::unordered_set<sf::Vector2u> awe::map::getTilesOfArmy(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armys.at(army).getTiles();
	_logger.error("getTilesOfArmy operation failed: army with ID {} didn't exist at the time of calling!", army);
	return std::unordered_set<sf::Vector2u>();
}

std::unordered_set<awe::UnitID> awe::map::getUnitsOfArmy(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armys.at(army).getUnits();
	_logger.error("getUnitsOfArmy operation failed: army with ID {} didn't exist at the time of calling!", army);
	return std::unordered_set<awe::UnitID>();
}

awe::UnitID awe::map::createUnit(const std::shared_ptr<const awe::unit_type>& type, const awe::ArmyID army) noexcept {
	if (!type) _logger.warning("createUnit warning: creating a unit for army {} without a type!", army);
	if (!_isArmyPresent(army)) {
		_logger.error("createUnit operation failed: attempted to create \"{}\" for army with ID {} that didn't exist!", ((type) ? (type->getName()) : ("[NULL]")), army);
		return 0;
	}
	awe::UnitID id;
	try {
		id = _findUnitID();
	} catch (std::bad_alloc&) {
		_logger.error("createUnit fatal error: could not generate a unique ID for a new unit. There are too many units allocated!");
		return 0;
	}
	_units.insert({ id, awe::unit(type, army, _sheet_unit) });
	return id;
}

void awe::map::deleteUnit(const awe::UnitID id) noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("deleteUnit operation cancelled: attempted to delete unit with ID {} that didn't exist!", id);
		return;
	}
	// firstly, remove the unit from the tile, if it was on a tile
	// we don't need to check if the unit "is actually on the map or not"
	// since the tile will always hold the index to the unit in either case:
	// which is why we need the "actually" check to begin with
	if (!_isOutOfBounds(_units[id].getPosition())) _tiles[_units[id].getPosition().x][_units[id].getPosition().y].setUnit(0);
	// secondly, remove the unit from the army's list
	if (_isArmyPresent(_units[id].getArmy()))
		_armys[_units[id].getArmy()].removeUnit(id);
	else
		_logger.warning("deleteUnit warning: unit with ID {} didn't have a valid owning army ID, which was {}", id, _units[id].getArmy());
	// thirdly, delete all units that are loaded onto this one
	auto loaded = _units[id].loadedUnits();
	for (awe::UnitID unit : loaded) {
		deleteUnit(unit);
	}
	// finally, delete the unit from the main list
	_units.erase(id);
}

void awe::map::setUnitPosition(const awe::UnitID id, const sf::Vector2u pos) noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("setUnitPosition operation cancelled: unit with ID {} doesn't exist!", id);
		return;
	}
	if (_isOutOfBounds(pos)) {
		_logger.error("setUnitPosition operation cancelled: attempted to move unit with ID {} to position ({},{}), which is out of bounds with the map's size ({},{})!",
			id, pos.x, pos.y, getMapSize().x, getMapSize().y);
		return;
	}
	if (getUnitOnTile(pos)) {
		_logger.error("setUnitPosition operation cancelled: attempted to move unit with ID {} to position ({},{}), which is currently occupied by unit with ID {}!",
			id, pos.x, pos.y, getUnitOnTile(pos));
		return;
	}
	// make new tile occupied
	_tiles[pos.x][pos.y].setUnit(id);
	// make old tile vacant
	// don't make tile vacant if a loaded unit also occupies the same tile internally, just to be safe
	if (_units[id].isOnMap()) _tiles[_units[id].getPosition().x][_units[id].getPosition().y].setUnit(0);
	// assign new location to unit
	_units[id].setPosition(pos);
}

sf::Vector2u awe::map::getUnitPosition(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitPosition operation failed: unit with ID {} doesn't exist!", id);
		return sf::Vector2u(0, 0);
	}
	return _units.at(id).getPosition();
}

bool awe::map::isUnitOnMap(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("isUnitOnMap operation failed: unit with ID {} doesn't exist!", id);
		return false;
	}
	return _units.at(id).isOnMap();
}

void awe::map::setUnitHP(const awe::UnitID id, const awe::HP hp) noexcept {
	if (_isUnitPresent(id)) {
		_units[id].setHP(hp);
	} else {
		_logger.error("setUnitHP operation cancelled: attempted to assign HP {} to unit with ID {}, which doesn't exist!", hp, id);
	}
}

awe::HP awe::map::getUnitHP(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getHP();
	_logger.error("getUnitHP operation failed: unit with ID {} doesn't exist!", id);
	return 0;
}

void awe::map::setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) noexcept {
	if (_isUnitPresent(id)) {
		_units[id].setFuel(fuel);
	} else {
		_logger.error("setUnitFuel operation cancelled: attempted to assign fuel {} to unit with ID {}, which doesn't exist!", fuel, id);
	}
}

awe::Fuel awe::map::getUnitFuel(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getFuel();
	_logger.error("getUnitFuel operation failed: unit with ID {} doesn't exist!", id);
	return 0;
}

void awe::map::setUnitAmmo(const awe::UnitID id, const awe::Ammo ammo) noexcept {
	if (_isUnitPresent(id)) {
		_units[id].setAmmo(ammo);
	} else {
		_logger.error("setUnitAmmo operation cancelled: attempted to assign ammo {} to unit with ID {}, which doesn't exist!", ammo, id);
	}
}

awe::Ammo awe::map::getUnitAmmo(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getAmmo();
	_logger.error("getUnitAmmo operation failed: unit with ID {} doesn't exist!", id);
	return 0;
}

void awe::map::loadUnit(const awe::UnitID load, const awe::UnitID onto) noexcept {
	if (!_isUnitPresent(onto)) {
		_logger.error("loadUnit operation cancelled: attempted to load a unit onto unit with ID {}, the latter of which does not exist!", onto);
		return;
	}
	if (!_isUnitPresent(load)) {
		_logger.error("loadUnit operation cancelled: attempted to load unit with ID {} onto unit with ID {}, the former of which does not exist!", load);
		return;
	}
	if (_units[load].loadedOnto()) {
		_logger.warning("loadUnit warning: unit with ID {} was already loaded onto unit with ID {}", load, onto);
		return;
	}
	_units[onto].loadUnit(load);
	_units[load].loadOnto(onto);
}

void awe::map::unloadUnit(const awe::UnitID unload, const awe::UnitID from, const sf::Vector2u onto) noexcept {
	if (!_isUnitPresent(from)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload a unit from unit with ID {}, the latter of which does not exist!", from);
		return;
	}
	if (!_isUnitPresent(unload)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload unit with ID {} from unit with ID {}, the former of which does not exist!", unload);
		return;
	}
	if (auto u = getUnitOnTile(onto)) {
		if (u) {
			_logger.error("unloadUnit operation cancelled: attempted to unload unit with ID {} from unit with ID {}, to position ({},{}), which has a unit with ID {} already occupying it!",
				unload, from, onto.x, onto.y, u);
		} else {
			_logger.error("unloadUnit operation cancelled: attempted to unload unit with ID {} from unit with ID {}, to position ({},{}), which is out of bounds with the map's size of ({},{})!",
				unload, from, onto.x, onto.y, getMapSize().x, getMapSize().y);
		}
		return;
	}
	if (_units[from].unloadUnit(unload)) { // unload successful, continue with operation
		_units[unload].loadOnto(0);
		setUnitPosition(unload, onto);
	} else {
		_logger.error("unloadUnit operation failed: unit with ID {} was not loaded onto unit with ID {}", unload, from);
	}
}

awe::ArmyID awe::map::getArmyOfUnit(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getArmy();
	_logger.error("getArmyOfUnit operation failed: unit with ID {} doesn't exist!", id);
	return awe::army::NO_ARMY;
}

bool awe::map::setTileType(const sf::Vector2u pos, const std::shared_ptr<const awe::tile_type>& type) noexcept {
	if (!type) _logger.warning("setTileType warning: assigning the tile at position ({},{}) an empty type!", pos.x, pos.y);
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileType operation cancelled: attempted to assign type \"{}\" to tile at position ({},{}), which is out of bounds with the map's size of ({},{})!",
			((type) ? (type->getType()->getName()) : ("[NULL]")), pos.x, pos.y, getMapSize().x, getMapSize().y);
		return false;
	}
	_tiles[pos.x][pos.y].setTileType(type);
	// remove ownership of the tile from the army who owns it, if any army does
	setTileOwner(pos, awe::army::NO_ARMY);
	return true;
}

std::shared_ptr<const awe::tile_type> awe::map::getTileType(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileType operation failed: tile at position ({},{}) is out of bounds with the map's size of ({},{})!", pos.x, pos.y, getMapSize().x, getMapSize().y);
		return nullptr;
	}
	return _tiles[pos.x][pos.y].getTileType();
}

void awe::map::setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept {
	if (!_isOutOfBounds(pos)) {
		_tiles[pos.x][pos.y].setTileHP(hp);
	} else {
		_logger.error("setTileHP operation cancelled: tile at position ({},{}) is out of bounds with the map's size of ({},{})!", pos.x, pos.y, getMapSize().x, getMapSize().y);
	}
}

awe::HP awe::map::getTileHP(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileHP operation failed: tile at position ({},{}) is out of bounds with the map's size of ({},{})!", pos.x, pos.y, getMapSize().x, getMapSize().y);
		return 0;
	}
	return _tiles[pos.x][pos.y].getTileHP();
}

void awe::map::setTileOwner(const sf::Vector2u pos, awe::ArmyID army) noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileOwner operation cancelled: army with ID {} couldn't be assigned to tile at position ({},{}), as it is out of bounds with the map's size of ({},{})!",
			army, pos.x, pos.y, getMapSize().x, getMapSize().y);
		return;
	}
	auto& tile = _tiles[pos.x][pos.y];
	// first, remove the tile from the army who currently owns it
	if (_isArmyPresent(tile.getTileOwner())) _armys[tile.getTileOwner()].removeTile(pos);
	// now assign it to the real owner, if any
	if (_isArmyPresent(army)) _armys[army].addTile(pos);
	// update the actual tile now
	tile.setTileOwner(army);
}

awe::ArmyID awe::map::getTileOwner(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileOwner operation failed: tile at position ({},{}) is out of bounds with the map's size of ({},{})!", pos.x, pos.y, getMapSize().x, getMapSize().y);
		return awe::army::NO_ARMY;
	}
	return _tiles[pos.x][pos.y].getTileOwner();
}

awe::UnitID awe::map::getUnitOnTile(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getUnitOnTile operation failed: tile at position ({},{}) is out of bounds with the map's size of ({},{})!", pos.x, pos.y, getMapSize().x, getMapSize().y);
		return 0;
	}
	auto u = _tiles[pos.x][pos.y].getUnit();
	if (u != 0 && _units.at(u).isOnMap()) return u;
	return 0;
}

void awe::map::selectTile(const sf::Vector2u pos) noexcept {
	if (!_isOutOfBounds(pos)) _sel = pos;
}

void awe::map::selectArmy(const awe::ArmyID army) noexcept {
	if (_isArmyPresent(army))
		_currentArmy = army;
	else
		_logger.error("selectArmy operation cancelled: army with ID {} does not exist!", army);
}

void awe::map::setVisiblePortionOfMap(const sf::Rect<sf::Uint32>& rect) noexcept {
	if (rect.width == 0 || rect.height == 0) {
		_logger.error("setVisiblePortionOfMap operation cancelled: attempted to set a portion of {} width and {} height: 0 in either of these dimensions is not accepted!",
			rect.width, rect.height);
	} else if (_isOutOfBounds(sf::Vector2u(rect.left + rect.width - 1, rect.top + rect.height - 1))) {
		// we should only need to check the most bottom right corner
		_logger.error("setVisiblePortionOfMap operation cancelled: attempted to set portion to ({} by {}) starting at ({},{}), which is out of bounds with the map's size of ({},{})!",
			rect.width, rect.height, rect.left, rect.top, getMapSize().x, getMapSize().y);
	} else {
		_visiblePortion = rect;
	}
}

void awe::map::setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_tile = sheet;
	// go through all of the tiles and set the new spritesheet to each one
	for (auto& column : _tiles) {
		for (auto& tile : column) {
			tile.setSpritesheet(sheet);
		}
	}
}

void awe::map::setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_unit = sheet;
	// go through all of the units and set the new spritesheet to each one
	for (auto& unit : _units) {
		unit.second.setSpritesheet(sheet);
	}
}

bool awe::map::animate(const sf::RenderTarget& target) noexcept {
	// step 1. the tiles
	float tiley = 0.0;
	for (sf::Uint32 y = 0, height = getMapSize().y; y < height; y++) {
		float tilex = 0.0;
		for (sf::Uint32 x = 0, width = getMapSize().x; x < width; x++) {
			auto& tile = _tiles[x][y];
			tile.animate(target);
			// position them and their unit if they are in the visible portion
			if (x >= _visiblePortion.left && x < _visiblePortion.left + _visiblePortion.width &&
				y >= _visiblePortion.top && y < _visiblePortion.top + _visiblePortion.height) {
				sf::Uint32 tileWidth = 0, tileHeight = 0;
				auto type = tile.getTileType();
				if (type) {
					if (tile.getTileOwner() == awe::army::NO_ARMY) {
						tileWidth = (sf::Uint32)_sheet_tile->accessSprite(type->getNeutralTile()).width;
						tileHeight = (sf::Uint32)_sheet_tile->accessSprite(type->getNeutralTile()).height;
					} else {
						tileWidth = (sf::Uint32)_sheet_tile->accessSprite(type->getOwnedTile(tile.getTileOwner())).width;
						tileHeight = (sf::Uint32)_sheet_tile->accessSprite(type->getOwnedTile(tile.getTileOwner())).height;
					}
				}
				if (tileWidth < tile.MIN_WIDTH) tileWidth = tile.MIN_WIDTH;
				if (tileHeight < tile.MIN_HEIGHT) tileHeight = tile.MIN_HEIGHT;
				tile.setPixelPosition(tilex, tiley - (float)(tileHeight - tile.MIN_HEIGHT));
				_units[tile.getUnit()].setPixelPosition(tilex, tiley);
				tilex += (float)tileWidth;
			}
		}
		tiley += (float)awe::tile::MIN_HEIGHT;
	}
	// step 2. the units
	// note that unit positioning was carried out in step 1
	for (auto& unit : _units) {
		unit.second.animate(target);
	}
	return false;
}

void awe::map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// step 1. the tiles
	for (sf::Uint32 y = _visiblePortion.top; y < _visiblePortion.top + _visiblePortion.height; y++) {
		for (sf::Uint32 x = _visiblePortion.left; x < _visiblePortion.left + _visiblePortion.width; x++) {
			target.draw(_tiles[x][y], states);
		}
	}
	// step 2. the units
	// loop through all visible tiles only, and retrieve their units, instead of looping through all units
	// unfortunately they have to be looped through separately to prevent tiles taller than the minimum
	// height from drawing over units
	for (sf::Uint32 y = _visiblePortion.top; y < _visiblePortion.top + _visiblePortion.height; y++) {
		for (sf::Uint32 x = _visiblePortion.left; x < _visiblePortion.left + _visiblePortion.width; x++) {
			target.draw(_units.at(_tiles[x][y].getUnit()), states);
		}
	}
	// step 3. the cursor
	// step 4. army pane
	// step 5. tile + unit pane
}

bool awe::map::_isOutOfBounds(const sf::Vector2u pos) const noexcept {
	return pos.x >= getMapSize().x || pos.y >= getMapSize().y;
}

bool awe::map::_isArmyPresent(const awe::ArmyID id) const noexcept {
	return _armys.find(id) != _armys.end();
}

bool awe::map::_isUnitPresent(const awe::UnitID id) const noexcept {
	return _units.find(id) != _units.end();
}

awe::UnitID awe::map::_findUnitID() {
	if (_units.size() == 0) return _lastUnitID;
	// minus 1 to account for the reserved value, 0
	if (_units.size() == UINT32_MAX - 1) throw std::bad_alloc();
	auto temp = _lastUnitID + 1;
	while (_isUnitPresent(temp)) {
		// manually handle overflow just to be safe
		if (temp == UINT32_MAX)
			temp = 1;
		else
			temp++;
	}
	_lastUnitID = temp;
	return temp;
}

void awe::map::_CWM_Header(const bool isSave, const std::shared_ptr<awe::bank<awe::country>>& countries, const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
		const std::shared_ptr<awe::bank<awe::unit_type>>& units, unsigned char version) {
	sf::Uint32 finalVersion = FIRST_FILE_VERSION + version;
	if (isSave) {
		_file.writeNumber(finalVersion);
	} else {
		finalVersion = _file.readNumber<sf::Uint32>();
		version = finalVersion - FIRST_FILE_VERSION;
	}
	switch (version) {
	case 0:
		_CWM_0(isSave, countries, tiles, units);
		break;
	default:
		_logger.error("CWM version {} is unrecognised!", version);
		throw std::exception("read above");
	}
}

void awe::map::_CWM_0(const bool isSave, const std::shared_ptr<awe::bank<awe::country>>& countries, const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
		const std::shared_ptr<awe::bank<awe::unit_type>>& units) {
	if (isSave) {
		_file.writeString(getMapName());
		_file.writeNumber((sf::Uint32)getMapSize().x);
		_file.writeNumber((sf::Uint32)getMapSize().y);
		_file.writeNumber((sf::Uint32)_armys.size());
		for (auto& army : _armys) {
			_file.writeNumber(army.second.getCountry()->getID());
			_file.writeNumber(army.second.getFunds());
		}
		for (sf::Uint64 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint64 x = 0; x < getMapSize().x; x++) {
				auto& tile = _tiles[x][y];
				_file.writeNumber(tile.getTileType()->getID());
				_file.writeNumber(tile.getTileHP());
				_file.writeNumber(tile.getTileOwner());
				if (tile.getUnit()) {
					_CWM_0_Unit(isSave, countries, tiles, units, tile.getUnit());
				} else {
					_file.writeNumber(awe::army::NO_ARMY);
				}
			}
		}
	} else {
		setMapName(_file.readString());
		sf::Uint32 width = _file.readNumber<sf::Uint32>(), height = _file.readNumber<sf::Uint32>();
		setMapSize(sf::Vector2u(width, height));
		sf::Uint32 armyCount = _file.readNumber<sf::Uint32>();
		for (sf::Uint64 army = 0; army < armyCount; army++) {
			auto pCountry = (*countries)[_file.readNumber<awe::BankID>()];
			if (createArmy(pCountry)) {
				awe::Funds funds = _file.readNumber<awe::Funds>();
				setArmyFunds(pCountry->getID(), funds);
			} else {
				throw std::exception("read above");
			}
		}
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto pos = sf::Vector2u(x, y);
				if (setTileType(pos, (*tiles)[_file.readNumber<awe::BankID>()])) {
					awe::HP hp = _file.readNumber<awe::HP>();
					setTileHP(pos, hp);
					awe::ArmyID army = _file.readNumber<awe::ArmyID>();
					setTileOwner(pos, army);
					_CWM_0_Unit(isSave, countries, tiles, units, 0);
				} else {
					throw std::exception("read above");
				}
			}
		}
	}
}

bool awe::map::_CWM_0_Unit(const bool isSave, const std::shared_ptr<awe::bank<awe::country>>& countries, const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
		const std::shared_ptr<awe::bank<awe::unit_type>>& units, awe::UnitID id) {
	if (isSave) {
		auto& unit = _units[id];
		_file.writeNumber(unit.getArmy());
		_file.writeNumber(unit.getType()->getID());
		_file.writeNumber(unit.getHP());
		_file.writeNumber(unit.getFuel());
		_file.writeNumber(unit.getAmmo());
		auto loaded = unit.loadedUnits();
		if (loaded.size()) {
			for (auto loadedUnitID : loaded) {
				_CWM_0_Unit(isSave, countries, tiles, units, loadedUnitID);
			}
		} else {
			_file.writeNumber(awe::army::NO_ARMY);
		}
		return true;
	} else {
		auto ownerArmy = _file.readNumber<awe::ArmyID>();
		if (ownerArmy != awe::army::NO_ARMY) {
			auto unitID = createUnit((*units)[_file.readNumber<awe::BankID>()], ownerArmy);
			if (unitID) {
				auto hp = _file.readNumber<awe::HP>();
				setUnitHP(unitID, hp);
				auto fuel = _file.readNumber<awe::Fuel>();
				setUnitFuel(unitID, fuel);
				auto ammo = _file.readNumber<awe::Ammo>();
				setUnitAmmo(unitID, ammo);
				while (_CWM_0_Unit(isSave, countries, tiles, units, id));
			} else {
				throw std::exception("read above");
			}
			return true;
		} else {
			return false;
		}
	}
}