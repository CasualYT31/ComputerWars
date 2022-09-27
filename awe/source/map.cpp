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

#include "map.hpp"
#include "fmtformatter.hpp"

void awe::selected_unit_render_data::clearState() noexcept {
	selectedUnit = 0;
	availableTiles.clear();
	availableTileShader = awe::selected_unit_render_data::shader::None;
	closedList.clear();
	renderUnitAtDestination = false;
}

awe::map::map(const std::string& name) noexcept : _logger(name) {
	_initShaders();
}

awe::map::map(const std::shared_ptr<awe::bank<awe::country>>& countries,
	const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
	const std::shared_ptr<awe::bank<awe::unit_type>>& units,
	const std::shared_ptr<awe::bank<awe::commander>>& commanders,
	const std::string& name) noexcept :
	_logger(name) {
	_countries = countries;
	_tileTypes = tiles;
	_unitTypes = units;
	_commanders = commanders;
	_initShaders();
}

bool awe::map::load(std::string file, const unsigned char version) noexcept {
	if (file == "") file = _filename;
	// Clear state.
	_sel = sf::Vector2u(0, 0);
	_currentArmy = awe::army::NO_ARMY;
	_updateTilePane = false;
	_lastUnitID = 1;
	_armies.clear();
	_units.clear();
	_tiles.clear();
	_mapName = "";
	_mapOffset = sf::Vector2f(0.0f, 0.0f);
	_day = 0;
	// Load new state.
	try {
		_file.open(file, true);
		_filename = file;
		_CWM_Header(false, version);
		_file.close();
	} catch (std::exception& e) {
		_logger.error("Map loading operation: couldn't load map file \"{}\": {}",
			file, e.what());
		_file.close();
		return false;
	}
	return true;
}

bool awe::map::save(std::string file, const unsigned char version) noexcept {
	if (file == "") file = _filename;
	try {
		_file.open(file, false);
		_filename = file;
		_CWM_Header(true, version);
		_file.close();
	} catch (std::exception& e) {
		_logger.error("Map saving operation: couldn't save map file \"{}\": {}",
			file, e.what());
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

void awe::map::setMapSize(const sf::Vector2u dim,
	const std::shared_ptr<const awe::tile_type>& tile) noexcept {
	// First, resize the tiles vectors accordingly.
	bool mapHasShrunk = (getMapSize().x > dim.x || getMapSize().y > dim.y);
	_tiles.resize(dim.x);
	for (std::size_t x = 0; x < dim.x; x++) {
		_tiles[x].resize(dim.y, { tile, _sheet_tile });
	}
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
			if (_isOutOfBounds(itr.second.getPosition()))
				unitsToDelete.push_back(itr.first);
		}
		// I decided to separate out identification and deletion because I wasn't
		// (and still aren't) sure if deleting elements will invalidate iterators.
		for (auto& itr : unitsToDelete) {
			deleteUnit(itr);
		}
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

sf::Vector2u awe::map::getMapSize() const noexcept {
	sf::Vector2u ret((unsigned int)_tiles.size(), 0);
	if (ret.x) ret.y = (unsigned int)_tiles.at(0).size();
	return ret;
}

void awe::map::setDay(const awe::Day day) noexcept {
	_day = day;
}

awe::Day awe::map::getDay() const noexcept {
	return _day;
}

bool awe::map::createArmy(const std::shared_ptr<const awe::country>& country)
	noexcept {
	if (!country) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with no country!");
		return false;
	}
	if (_isArmyPresent(country->getID())) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with a country, \"{}\", that already existed on the map!",
			country->getName());
		return false;
	}
	// Create the army.
	_armies.insert(std::pair<awe::BankID, awe::army>(country->getID(), country));
	static const awe::TeamID maxIDCounter = ~((awe::TeamID)0);
	// This will miss out the maximum value for a team ID, but I don't care.
	if (_teamIDCounter == maxIDCounter) _teamIDCounter = 0;
	_armies.at(country->getID()).setTeam(_teamIDCounter++);
	return true;
}

void awe::map::deleteArmy(const awe::ArmyID army) noexcept {
	if (!_isArmyPresent(army)) {
		_logger.error("deleteArmy operation cancelled: attempted to delete an "
			"army, {}, that didn't exist on the map!", army);
		return;
	}
	// Firstly, delete all units belonging to the army.
	auto units = _armies.at(army).getUnits();
	for (auto unit : units) {
		deleteUnit(unit);
	}
	// Then, disown all tiles.
	auto tiles = _armies.at(army).getTiles();
	for (auto& tile : tiles) {
		_tiles[tile.x][tile.y].setTileOwner(awe::army::NO_ARMY);
	}
	// Finally, delete the army from the army list.
	_armies.erase(army);
}

std::size_t awe::map::getArmyCount() const noexcept {
	return _armies.size();
}

std::set<awe::ArmyID> awe::map::getArmyIDs() const noexcept {
	std::set<awe::ArmyID> ret;
	for (auto& a : _armies) ret.insert(a.first);
	return ret;
}

void awe::map::setArmyTeam(const awe::ArmyID army, const awe::TeamID team)
	noexcept {
	if (_isArmyPresent(army)) {
		_armies.at(army).setTeam(team);
	} else {
		_logger.error("setArmyTeam operation cancelled: attempted to set an army "
			"{}'s team to {}, but that army didn't exist!", army, team);
	}
}

awe::TeamID awe::map::getArmyTeam(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getTeam();
	_logger.error("getArmyTeam operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return 0;
}

void awe::map::setArmyFunds(const awe::ArmyID army, const awe::Funds funds)
	noexcept {
	if (_isArmyPresent(army)) {
		_armies.at(army).setFunds(funds);
	} else {
		_logger.error("setArmyFunds operation cancelled: attempted to set {} "
			"funds to an army, {}, that didn't exist!", funds, army);
	}
}

awe::Funds awe::map::getArmyFunds(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getFunds();
	_logger.error("getArmyFunds operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return -1;
}

std::shared_ptr<const awe::country>
	awe::map::getArmyCountry(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getCountry();
	_logger.error("getArmyCountry operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return nullptr;
}

void awe::map::setArmyCOs(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& current,
	const std::shared_ptr<const awe::commander>& tag) noexcept {
	if (_isArmyPresent(army)) {
		if (!current && !tag) {
			_logger.error("setCOs operation failed: army with ID {} was given no "
				"COs!", army);
		} else {
			if (!current && tag) {
				_logger.warning("setCOs operation: army with ID {} was given a "
					"tag CO but not current CO! The army will instead be assigned "
					"a current CO and it will not be assigned a tag CO.", army);
			}
			_armies.at(army).setCOs(current, tag);
		}
	} else {
		_logger.error("setCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
	}
}

void awe::map::setArmyCurrentCO(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& current) noexcept {
	setArmyCOs(army, current, getArmyTagCO(army));
}

void awe::map::setArmyTagCO(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& tag) noexcept {
	setArmyCOs(army, getArmyCurrentCO(army), tag);
}

void awe::map::tagArmyCOs(const awe::ArmyID army) noexcept {
	if (!_isArmyPresent(army)) {
		_logger.error("tagCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
	} else {
		if (_armies.at(army).getTagCO()) {
			_armies.at(army).tagCOs();
		} else {
			_logger.error("tagCOs operation failed: army with ID {} didn't have a "
				"secondary CO at the time of calling!", army);
		}
	}
}

std::shared_ptr<const awe::commander> awe::map::getArmyCurrentCO(
	const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getCurrentCO();
	_logger.error("getCurrentCO operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return nullptr;
}

std::shared_ptr<const awe::commander> awe::map::getArmyTagCO(
	const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCO();
	_logger.error("getTagCO operation failed: army with ID {} didn't exist at the "
		"time of calling!", army);
	return nullptr;
}

bool awe::map::tagCOIsPresent(const awe::ArmyID army) const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCO().operator bool();
	_logger.error("tagCOIsPresent operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return false;
}

std::unordered_set<sf::Vector2u> awe::map::getTilesOfArmy(const awe::ArmyID army)
	const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getTiles();
	_logger.error("getTilesOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<sf::Vector2u>();
}

std::unordered_set<awe::UnitID> awe::map::getUnitsOfArmy(const awe::ArmyID army)
	const noexcept {
	if (_isArmyPresent(army)) return _armies.at(army).getUnits();
	_logger.error("getUnitsOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<awe::UnitID>();
}

std::map<unsigned int, std::unordered_set<awe::UnitID>>
	awe::map::getUnitsOfArmyByPriority(const awe::ArmyID army) const noexcept {
	if (!_isArmyPresent(army)) {
		_logger.error("getUnitsOfArmyByPriority operation failed: army with ID {} "
			"didn't exist at the time of calling!", army);
		return {};
	}
	std::map<unsigned int, std::unordered_set<awe::UnitID>> ret;
	auto units = getUnitsOfArmy(army);
	for (auto& unit : units)
		ret[_units.at(unit).getType()->getTurnStartPriority()].insert(unit);
	return ret;
}

awe::UnitID awe::map::createUnit(const std::shared_ptr<const awe::unit_type>& type,
	const awe::ArmyID army) noexcept {
	if (!type) _logger.warning("createUnit warning: creating a unit for army {} "
		"without a type!", army);
	if (!_isArmyPresent(army)) {
		_logger.error("createUnit operation failed: attempted to create \"{}\" "
			"for army with ID {} that didn't exist!",
			((type) ? (type->getName()) : ("[NULL]")), army);
		return 0;
	}
	awe::UnitID id;
	try {
		id = _findUnitID();
	} catch (std::bad_alloc&) {
		_logger.error("createUnit fatal error: could not generate a unique ID for "
			"a new unit. There are too many units allocated!");
		return 0;
	}
	_units.insert({ id, awe::unit(type, army, _sheet_unit, _sheet_icon) });
	_armies.at(army).addUnit(id);
	return id;
}

void awe::map::deleteUnit(const awe::UnitID id) noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("deleteUnit operation cancelled: attempted to delete unit "
			"with ID {} that didn't exist!", id);
		return;
	}
	// Firstly, remove the unit from the tile, if it was on a tile.
	// We don't need to check if the unit "is actually on the map or not," since
	// the tile will always hold the index to the unit in either case: which is why
	// we need the "actually" check to begin with
	if (!_isOutOfBounds(_units.at(id).getPosition()))
		_tiles[_units.at(id).getPosition().x]
		      [_units.at(id).getPosition().y].setUnit(0);
	// Secondly, remove the unit from the army's list.
	if (_isArmyPresent(_units.at(id).getArmy())) {
		_armies.at(_units.at(id).getArmy()).removeUnit(id);
	} else {
		_logger.warning("deleteUnit warning: unit with ID {} didn't have a valid "
			"owning army ID, which was {}", id, _units.at(id).getArmy());
	}
	// Thirdly, delete all units that are loaded onto this one.
	auto loaded = _units.at(id).loadedUnits();
	for (awe::UnitID unit : loaded) {
		deleteUnit(unit);
	}
	// Finally, delete the unit from the main list.
	_units.erase(id);
}

std::shared_ptr<const awe::unit_type> awe::map::getUnitType(const awe::UnitID id)
	const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getType();
	_logger.error("getUnitType operation failed: unit with ID {} doesn't exist!",
		id);
	return nullptr;
}

void awe::map::setUnitPosition(const awe::UnitID id, const sf::Vector2u pos)
	noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("setUnitPosition operation cancelled: unit with ID {} "
			"doesn't exist!", id);
		return;
	}
	if (_isOutOfBounds(pos)) {
		_logger.error("setUnitPosition operation cancelled: attempted to move "
			"unit with ID {} to position ({},{}), which is out of bounds with the "
			"map's size ({},{})!",
			id, pos.x, pos.y, getMapSize().x, getMapSize().y);
		return;
	}
	if (getUnitOnTile(pos)) {
		_logger.error("setUnitPosition operation cancelled: attempted to move "
			"unit with ID {} to position ({},{}), which is currently occupied by "
			"unit with ID {}!",
			id, pos.x, pos.y, getUnitOnTile(pos));
		return;
	}
	// Make new tile occupied.
	_tiles[pos.x][pos.y].setUnit(id);
	// Make old tile vacant. Don't make tile vacant if a loaded unit also occupies
	// the same tile internally, just to be safe.
	if (_units.at(id).isOnMap())
		_tiles[_units.at(id).getPosition().x]
		      [_units.at(id).getPosition().y].setUnit(0);
	// Assign new location to unit.
	_units.at(id).setPosition(pos);
}

sf::Vector2u awe::map::getUnitPosition(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitPosition operation failed: unit with ID {} doesn't "
			"exist!", id);
		return sf::Vector2u(0, 0);
	}
	return _units.at(id).getPosition();
}

bool awe::map::isUnitOnMap(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) {
		_logger.error("isUnitOnMap operation failed: unit with ID {} doesn't "
			"exist!", id);
		return false;
	}
	return _units.at(id).isOnMap();
}

void awe::map::setUnitHP(const awe::UnitID id, const awe::HP hp) noexcept {
	if (_isUnitPresent(id)) {
		_units.at(id).setHP(hp);
	} else {
		_logger.error("setUnitHP operation cancelled: attempted to assign HP {} "
			"to unit with ID {}, which doesn't exist!", hp, id);
	}
}

awe::HP awe::map::getUnitHP(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getHP();
	_logger.error("getUnitHP operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

awe::HP awe::map::getUnitDisplayedHP(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getDisplayedHP();
	_logger.error("getUnitDisplayedHP operation failed: unit with ID {} doesn't "
		"exist!", id);
	return 0;
}

void awe::map::setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) noexcept {
	if (_isUnitPresent(id)) {
		_units.at(id).setFuel(fuel);
	} else {
		_logger.error("setUnitFuel operation cancelled: attempted to assign fuel "
			"{} to unit with ID {}, which doesn't exist!", fuel, id);
	}
}

awe::Fuel awe::map::getUnitFuel(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getFuel();
	_logger.error("getUnitFuel operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

void awe::map::setUnitAmmo(const awe::UnitID id, const awe::Ammo ammo) noexcept {
	if (_isUnitPresent(id)) {
		_units.at(id).setAmmo(ammo);
	} else {
		_logger.error("setUnitAmmo operation cancelled: attempted to assign ammo "
			"{} to unit with ID {}, which doesn't exist!", ammo, id);
	}
}

awe::Ammo awe::map::getUnitAmmo(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getAmmo();
	_logger.error("getUnitAmmo operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

void awe::map::waitUnit(const awe::UnitID id, const bool waiting) noexcept {
	if (_isUnitPresent(id)) {
		_units.at(id).wait(waiting);
	} else {
		_logger.error("waitUnit operation cancelled: attempted to assign waiting "
			"state {} to unit with ID {}, which doesn't exist!", waiting, id);
	}
}

bool awe::map::isUnitWaiting(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).isWaiting();
	_logger.error("isUnitWaiting operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

void awe::map::loadUnit(const awe::UnitID load, const awe::UnitID onto) noexcept {
	if (!_isUnitPresent(onto)) {
		_logger.error("loadUnit operation cancelled: attempted to load a unit "
			"onto unit with ID {}, the latter of which does not exist!", onto);
		return;
	}
	if (!_isUnitPresent(load)) {
		_logger.error("loadUnit operation cancelled: attempted to load unit with "
			"ID {} onto unit with ID {}, the former of which does not exist!",
			load);
		return;
	}
	if (_units.at(load).loadedOnto()) {
		_logger.warning("loadUnit warning: unit with ID {} was already loaded "
			"onto unit with ID {}", load, onto);
		return;
	}
	_units.at(onto).loadUnit(load);
	_units.at(load).loadOnto(onto);
}

void awe::map::unloadUnit(const awe::UnitID unload, const awe::UnitID from,
	const sf::Vector2u onto) noexcept {
	if (!_isUnitPresent(from)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload a unit "
			"from unit with ID {}, the latter of which does not exist!", from);
		return;
	}
	if (!_isUnitPresent(unload)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload unit "
			"with ID {} from unit with ID {}, the former of which does not exist!",
			unload);
		return;
	}
	if (auto u = getUnitOnTile(onto)) {
		if (u) {
			_logger.error("unloadUnit operation cancelled: attempted to unload "
				"unit with ID {} from unit with ID {}, to position ({},{}), which "
				"has a unit with ID {} already occupying it!",
				unload, from, onto.x, onto.y, u);
		} else {
			_logger.error("unloadUnit operation cancelled: attempted to unload "
				"unit with ID {} from unit with ID {}, to position ({},{}), which "
				"is out of bounds with the map's size of ({},{})!",
				unload, from, onto.x, onto.y, getMapSize().x, getMapSize().y);
		}
		return;
	}
	if (_units.at(from).unloadUnit(unload)) {
		// Unload successful, continue with operation.
		_units.at(unload).loadOnto(0);
		setUnitPosition(unload, onto);
	} else {
		_logger.error("unloadUnit operation failed: unit with ID {} was not "
			"loaded onto unit with ID {}", unload, from);
	}
}

awe::ArmyID awe::map::getArmyOfUnit(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getArmy();
	_logger.error("getArmyOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return awe::army::NO_ARMY;
}

awe::TeamID awe::map::getTeamOfUnit(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _armies.at(_units.at(id).getArmy()).getTeam();
	_logger.error("getTeamOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

std::unordered_set<awe::UnitID> awe::map::getLoadedUnits(const awe::UnitID id)
	const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).loadedUnits();
	_logger.error("getLoadedUnits operation failed: unit with ID {} doesn't "
		"exist!", id);
	return {};
}

bool awe::map::setTileType(const sf::Vector2u pos,
	const std::shared_ptr<const awe::tile_type>& type) noexcept {
	if (!type) _logger.warning("setTileType warning: assigning the tile at "
		"position ({},{}) an empty type!", pos.x, pos.y);
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileType operation cancelled: attempted to assign type "
			"\"{}\" to tile at position ({},{}), which is out of bounds with the "
			"map's size of ({},{})!",
			((type) ? (type->getType()->getName()) : ("[NULL]")),
			pos.x, pos.y, getMapSize().x, getMapSize().y);
		return false;
	}
	_tiles[pos.x][pos.y].setTileType(type);
	// Remove ownership of the tile from the army who owns it, if any army does.
	setTileOwner(pos, awe::army::NO_ARMY);
	return true;
}

std::shared_ptr<const awe::tile_type> awe::map::getTileType(const sf::Vector2u pos)
	const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileType operation failed: tile at position ({},{}) is "
			"out of bounds with the map's size of ({},{})!",
			pos.x, pos.y, getMapSize().x, getMapSize().y);
		return nullptr;
	}
	return _tiles[pos.x][pos.y].getTileType();
}

void awe::map::setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept {
	if (!_isOutOfBounds(pos)) {
		_tiles[pos.x][pos.y].setTileHP(hp);
	} else {
		_logger.error("setTileHP operation cancelled: tile at position ({},{}) is "
			"out of bounds with the map's size of ({},{})!",
			pos.x, pos.y, getMapSize().x, getMapSize().y);
	}
}

awe::HP awe::map::getTileHP(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileHP operation failed: tile at position ({},{}) is "
			"out of bounds with the map's size of ({},{})!",
			pos.x, pos.y, getMapSize().x, getMapSize().y);
		return 0;
	}
	return _tiles[pos.x][pos.y].getTileHP();
}

void awe::map::setTileOwner(const sf::Vector2u pos, awe::ArmyID army) noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileOwner operation cancelled: army with ID {} couldn't "
			"be assigned to tile at position ({},{}), as it is out of bounds with "
			"the map's size of ({},{})!",
			army, pos.x, pos.y, getMapSize().x, getMapSize().y);
		return;
	}
	auto& tile = _tiles[pos.x][pos.y];
	// First, remove the tile from the army who currently owns it.
	if (_isArmyPresent(tile.getTileOwner()))
		_armies.at(tile.getTileOwner()).removeTile(pos);
	// Now assign it to the real owner, if any.
	if (_isArmyPresent(army)) _armies.at(army).addTile(pos);
	// Update the actual tile now.
	tile.setTileOwner(army);
}

awe::ArmyID awe::map::getTileOwner(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileOwner operation failed: tile at position ({},{}) is "
			"out of bounds with the map's size of ({},{})!",
			pos.x, pos.y, getMapSize().x, getMapSize().y);
		return awe::army::NO_ARMY;
	}
	return _tiles[pos.x][pos.y].getTileOwner();
}

awe::UnitID awe::map::getUnitOnTile(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) {
		_logger.error("getUnitOnTile operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return 0;
	}
	auto u = _tiles[pos.x][pos.y].getUnit();
	if (u != 0 && _units.at(u).isOnMap()) return u;
	return 0;
}

std::unordered_set<sf::Vector2u> awe::map::getAvailableTiles(
	const sf::Vector2u tile, unsigned int startFrom, const unsigned int endAt)
	noexcept {
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
	sf::Vector2<int64_t> highest = {(int64_t)tile.x,
		(int64_t)tile.y - (int64_t)endAt};
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

void awe::map::setSelectedTile(const sf::Vector2u pos) noexcept {
	if (!_isOutOfBounds(pos)) {
		_sel_old = _sel;
		if (_sel != pos) {
			_sel = pos;
			_updateTilePane = true;
		}
	}
}

void awe::map::moveSelectedTileUp() noexcept {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y - 1));
}

void awe::map::moveSelectedTileDown() noexcept {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y + 1));
}

void awe::map::moveSelectedTileLeft() noexcept {
	setSelectedTile(sf::Vector2u(getSelectedTile().x - 1, getSelectedTile().y));
}

void awe::map::moveSelectedTileRight() noexcept {
	setSelectedTile(sf::Vector2u(getSelectedTile().x + 1, getSelectedTile().y));
}

sf::Vector2u awe::map::getSelectedTile() const noexcept {
	return _sel;
}

void awe::map::setSelectedTileByPixel(const sf::Vector2i pixel) noexcept {
	const auto MAP_SIZE = getMapSize();
	const auto REAL_TILE_MIN_WIDTH = awe::tile::MIN_WIDTH * _mapScalingFactor
		* _scalingCache;
	const auto REAL_TILE_MIN_HEIGHT = awe::tile::MIN_HEIGHT * _mapScalingFactor
		* _scalingCache;
	sf::Vector2f pixelF(pixel);
	// Can the map fit on the screen?
	sf::Vector2f mapCenterOffset = sf::Vector2f(
		_targetSizeCache.x / 2.0f - MAP_SIZE.x * REAL_TILE_MIN_WIDTH / 2.0f,
		_targetSizeCache.y / 2.0f - MAP_SIZE.y * REAL_TILE_MIN_HEIGHT / 2.0f
	);
	// X
	if (mapCenterOffset.x < 0.0f)
		pixelF.x -= _mapOffset.x;
	else
		pixelF.x -= mapCenterOffset.x;
	pixelF.x /= REAL_TILE_MIN_WIDTH;
	// Y
	if (mapCenterOffset.y < 0.0f)
		pixelF.y -= _mapOffset.y;
	else
		pixelF.y -= mapCenterOffset.y;
	pixelF.y /= REAL_TILE_MIN_HEIGHT;
	// Select the tile.
	setSelectedTile(sf::Vector2u(pixelF));

}

void awe::map::selectArmy(const awe::ArmyID army) noexcept {
	if (_isArmyPresent(army))
		_currentArmy = army;
	else
		_logger.error("selectArmy operation cancelled: army with ID {} does not "
			"exist!", army);
}

awe::ArmyID awe::map::getSelectedArmy() const noexcept {
	return _currentArmy;
}

awe::ArmyID awe::map::getNextArmy() const noexcept {
	if (_currentArmy == awe::army::NO_ARMY) return awe::army::NO_ARMY;
	auto itr = ++_armies.find(_currentArmy);
	if (itr == _armies.end()) itr = _armies.begin();
	return itr->first;
}

void awe::map::setMapScalingFactor(const float factor) noexcept {
	_mapScalingFactor = factor;
	_updateTilePane = true;
	_changedScaleFactor = true;
	_mapOffset = sf::Vector2f(0.0f, 0.0f);
}

sf::Vector2u awe::map::getTileSize() const noexcept {
	return sf::Vector2u(awe::tile::MIN_WIDTH * (sf::Uint32)_scalingCache *
		(sf::Uint32)_mapScalingFactor,
		awe::tile::MIN_HEIGHT * (sf::Uint32)_scalingCache *
		(sf::Uint32)_mapScalingFactor);
}

void awe::map::setTileSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_tile = sheet;
	// Go through all of the tiles and set the new spritesheet to each one.
	for (auto& column : _tiles) {
		for (auto& tile : column) {
			tile.setSpritesheet(sheet);
		}
	}
}

void awe::map::setUnitSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_unit = sheet;
	// Go through all of the units and set the new icon spritesheet to each one.
	for (auto& unit : _units) unit.second.setSpritesheet(sheet);
}

void awe::map::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_icon = sheet;
	_cursor.setSpritesheet(sheet);
	_cursor.setSprite("cursor");
	_tilePane.setSpritesheet(sheet);
	// Go through all of the units and set the new spritesheet to each one.
	for (auto& unit : _units) unit.second.setIconSpritesheet(sheet);
}

void awe::map::setCOSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sheet_co = sheet;
	_armyPane.setSpritesheet(sheet);
}

void awe::map::setFont(const std::shared_ptr<sf::Font>& font) noexcept {
	if (!font) {
		_logger.error("setFont operation failed: nullptr was given!");
		return;
	}
	_armyPane.setFont(font);
	_tilePane.setFont(font);
}

void awe::map::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) noexcept {
	if (!dict) {
		_logger.error("setLanguageDictionary operation failed: nullptr was "
			"given!");
		return;
	}
	_tilePane.setLanguageDictionary(dict);
}

bool awe::map::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	_scalingCache = (float)scaling;
	_targetSizeCache = sf::Vector2f(target.getSize());
	auto mapSize = getMapSize();
	auto REAL_TILE_MIN_WIDTH = awe::tile::MIN_WIDTH * _mapScalingFactor *
		(float)scaling;
	auto REAL_TILE_MIN_HEIGHT = awe::tile::MIN_HEIGHT * _mapScalingFactor *
		(float)scaling;
	if (_updateTilePane) {
		if (_changedScaleFactor) {
			// If the scale factor has changed, ensure that the cursor appears
			// central to the screen.
			auto cursorX = _sel.x * REAL_TILE_MIN_WIDTH + (_mapOffset.x),
				cursorY = _sel.y * REAL_TILE_MIN_HEIGHT + (_mapOffset.y);
			_mapOffset.x = (((float)target.getSize().x - REAL_TILE_MIN_WIDTH * 2)
				/ 2.0f) - ((float)mapSize.x / 2.0f) - REAL_TILE_MIN_WIDTH / 2.0f;
			_mapOffset.y = (((float)target.getSize().y - REAL_TILE_MIN_HEIGHT * 2)
				/ 2.0f) - ((float)mapSize.y / 2.0f) - REAL_TILE_MIN_HEIGHT / 2.0f;
			_mapOffset.x -= cursorX;
			_mapOffset.y -= cursorY;
			_changedScaleFactor = false;
		}
		// Predict the cursor's new position
		auto newCursorX = _sel.x * REAL_TILE_MIN_WIDTH + (_mapOffset.x),
			newCursorY = _sel.y * REAL_TILE_MIN_HEIGHT + (_mapOffset.y);
		auto sel_old_cpyX = _sel_old.x * REAL_TILE_MIN_WIDTH + (_mapOffset.x),
			sel_old_cpyY = _sel_old.y * REAL_TILE_MIN_HEIGHT + (_mapOffset.y);
		// Update _mapOffset if the new selected tile is outside of the screen.
		if (newCursorX < REAL_TILE_MIN_WIDTH) {
			_mapOffset.x += (sel_old_cpyX - newCursorX);
		}
		if (newCursorY < REAL_TILE_MIN_HEIGHT) {
			_mapOffset.y += (sel_old_cpyY - newCursorY);
		}
		if (newCursorX > target.getSize().x - REAL_TILE_MIN_WIDTH * 2) {
			_mapOffset.x -= (newCursorX - sel_old_cpyX);
		}
		if (newCursorY > target.getSize().y - REAL_TILE_MIN_HEIGHT * 2) {
			_mapOffset.y -= (newCursorY - sel_old_cpyY);
		}
	}
	// Calculate if any of the screen would be left black by the current
	// _mapOffset, and adjust the offset so that all of the screen is drawn over if
	// this offset is used.
	if (_mapOffset.x > 0.0f) _mapOffset.x = 0.0f;
	if (_mapOffset.y > 0.0f) _mapOffset.y = 0.0f;
	sf::Vector2f _mapOffsetLL(_mapOffset.x + mapSize.x * REAL_TILE_MIN_WIDTH,
		_mapOffset.y + mapSize.y * REAL_TILE_MIN_HEIGHT);
	if (_mapOffsetLL.x < target.getSize().x)
		_mapOffset.x = target.getSize().x - mapSize.x * REAL_TILE_MIN_WIDTH;
	if (_mapOffsetLL.y < target.getSize().y)
		_mapOffset.y = target.getSize().y - mapSize.y * REAL_TILE_MIN_HEIGHT;
	// Step 0. calculate offset to make map central to the render target.
	sf::Vector2f mapCenterOffset = sf::Vector2f(
		(float)target.getSize().x / 2.0f - mapSize.x * REAL_TILE_MIN_WIDTH / 2.0f,
		(float)target.getSize().y / 2.0f - mapSize.y * REAL_TILE_MIN_HEIGHT / 2.0f
	);
	mapCenterOffset = sf::Vector2f(ceil(mapCenterOffset.x / _mapScalingFactor),
		ceil(mapCenterOffset.y / _mapScalingFactor));
	mapCenterOffset = sf::Vector2f(ceil(mapCenterOffset.x / (float)scaling),
		ceil(mapCenterOffset.y / (float)scaling));
	// Replace map centre offset values with _mapOffset if either of them are -ive.
	if (mapCenterOffset.x < 0.0f)
		mapCenterOffset.x = _mapOffset.x / _mapScalingFactor / (float)scaling;
	if (mapCenterOffset.y < 0.0f)
		mapCenterOffset.y = _mapOffset.y / _mapScalingFactor / (float)scaling;
	// Step 1. the tiles.
	// Also update the position of the cursor here!
	float tiley = 0.0;
	for (sf::Uint32 y = 0, height = getMapSize().y; y < height; ++y) {
		float tilex = 0.0;
		for (sf::Uint32 x = 0, width = getMapSize().x; x < width; ++x) {
			auto& tile = _tiles[x][y];
			tile.animate(target, scaling);
			sf::Vector2u tilePos = sf::Vector2u(x, y);
			// Position the tile and their unit if they are in the visible portion.
			if (_tileIsVisible(tilePos)) {
				sf::Uint32 tileWidth = 0, tileHeight = 0;
				auto type = tile.getTileType();
				if (type) {
					tileWidth = (sf::Uint32)tile.getPixelSize().x;
					tileHeight = (sf::Uint32)tile.getPixelSize().y;
					/*if (tile.getTileOwner() == awe::army::NO_ARMY) {
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
				tile.setPixelPosition(tilex + mapCenterOffset.x, tiley -
					(float)(tileHeight - tile.MIN_HEIGHT) + mapCenterOffset.y);
				if (!selectedUnitRenderData.renderUnitAtDestination) {
					if (tile.getUnit()) {
						_units.at(tile.getUnit()).setPixelPosition(
							tilex + mapCenterOffset.x, tiley + mapCenterOffset.y
						);
					}
				} else if (selectedUnitRenderData.renderUnitAtDestination) {
					if (tile.getUnit()) {
						if (tile.getUnit() !=
							selectedUnitRenderData.selectedUnit) {
							_units.at(tile.getUnit()).setPixelPosition(
								tilex + mapCenterOffset.x,
								tiley + mapCenterOffset.y
							);
						}
					}
					if (sf::Vector2u(x, y) ==
						selectedUnitRenderData.closedList.back().tile) {
						_units.at(selectedUnitRenderData.selectedUnit).
							setPixelPosition(
							tilex + mapCenterOffset.x,
							tiley + mapCenterOffset.y
						);
					}
				}
				// Update cursor position.
				if (getSelectedTile() == tilePos) {
					_cursor.setPosition(
						sf::Vector2f(tilex + mapCenterOffset.x,
							tiley + mapCenterOffset.y)
					);
				}
				tilex += (float)tileWidth;
			}
		}
		tiley += (float)awe::tile::MIN_HEIGHT;
	}
	// Step 2. the selected unit closed list tile icons.
	if (selectedUnitRenderData.selectedUnit > 0) {
		for (auto& pathNode : selectedUnitRenderData.closedList) {
			pathNode.sprite.animate(target, scaling);
			auto pos = _tiles[pathNode.tile.x][pathNode.tile.y].getPixelPosition();
			auto h = _tiles[pathNode.tile.x][pathNode.tile.y].getPixelSize().y;
			if (h > awe::tile::MIN_HEIGHT) pos.y += h - awe::tile::MIN_HEIGHT;
			pathNode.sprite.setPosition(pos);
		}
	}
	// Step 3. the units.
	// Note that unit positioning was carried out in step 1.
	for (auto& unit : _units) {
		unit.second.animate(target, scaling);
	}
	// Step 4. the cursor.
	_cursor.animate(target, scaling);
	// Step 5. set the general location of the panes.
	if (_cursor.getPosition().x < target.getSize().x / _mapScalingFactor /
		(float)scaling / 2.0f) {
		_armyPane.setGeneralLocation(awe::army_pane::location::Right);
		_tilePane.setGeneralLocation(awe::tile_pane::location::Right);
	} else {
		_armyPane.setGeneralLocation(awe::army_pane::location::Left);
		_tilePane.setGeneralLocation(awe::tile_pane::location::Left);
	}
	// Step 6. the army pane.
	if (_currentArmy != awe::army::NO_ARMY) {
		_armyPane.setArmy(_armies.at(_currentArmy));
		_armyPane.animate(target, scaling);
	}
	// Step 7. the tile pane.
	if (_updateTilePane) {
		const awe::tile& tile = _tiles[_sel.x][_sel.y];
		_tilePane.setTile(tile);
		_tilePane.clearUnits();
		if (tile.getUnit()) {
			const awe::unit& unit = _units.at(tile.getUnit());
			_tilePane.addUnit(unit);
			std::unordered_set<awe::UnitID> loaded = unit.loadedUnits();
			for (auto& u : loaded) _tilePane.addUnit(_units.at(u));
		}
		_updateTilePane = false;
	}
	_tilePane.animate(target, scaling);
	// End.
	return false;
}

void awe::map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_transformCache = states.transform;
	sf::RenderStates mapStates = states;
	mapStates.transform = sf::Transform().scale(sf::Vector2f(_mapScalingFactor,
		_mapScalingFactor)).combine(states.transform);
	// Step 1. the tiles.
	auto mapSize = getMapSize();
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			if (selectedUnitRenderData.selectedUnit > 0 &&
				!selectedUnitRenderData.renderUnitAtDestination) {
				sf::Vector2u currentTile(x, y);
				sf::RenderStates tileStates = mapStates;
				if (selectedUnitRenderData.availableTiles.find(currentTile) !=
					selectedUnitRenderData.availableTiles.end()) {
					// Apply configured shading.
					switch (selectedUnitRenderData.availableTileShader) {
					case awe::selected_unit_render_data::shader::Yellow:
						tileStates.shader = &_availableTileShader;
						break;
					case awe::selected_unit_render_data::shader::Red:
						tileStates.shader = &_attackableTileShader;
						break;
					}
				} else { // Not an available tile. Grey out.
					tileStates.shader = &_unavailableTileShader;
				}
				target.draw(_tiles[x][y], tileStates);
			} else {
				target.draw(_tiles[x][y], mapStates);
			}
		}
	}
	// Step 2. the selected unit closed list tiles.
	if (selectedUnitRenderData.selectedUnit > 0 &&
		!selectedUnitRenderData.renderUnitAtDestination) {
		for (auto& pathNode : selectedUnitRenderData.closedList) {
			target.draw(pathNode.sprite, mapStates);
		}
	}
	// Step 3. the units.
	// Loop through all visible tiles only, and retrieve their units, instead of
	// looping through all units. Unfortunately they have to be looped through
	// separately to prevent tiles taller than the minimum height from drawing over
	// units.
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			const awe::UnitID unit = _tiles[x][y].getUnit();
			if (unit > 0 && isUnitOnMap(unit)) {
				sf::RenderStates unitStates = mapStates;
				unitStates.shader = &_unavailableTileShader;
				if (selectedUnitRenderData.selectedUnit > 0 &&
					!selectedUnitRenderData.renderUnitAtDestination &&
					unit != selectedUnitRenderData.selectedUnit) {
					target.draw(_units.at(unit), unitStates);
				} else {
					if (isUnitWaiting(unit)) {
						target.draw(_units.at(unit), unitStates);
					} else {
						target.draw(_units.at(unit), mapStates);
					}
				}
			}
		}
	}
	// Step 4. the cursor.
	// But only if it is within the visible portion!
	// To tell the truth the cursor should never be not visible...
	if (_tileIsVisible(getSelectedTile())) target.draw(_cursor, mapStates);
	// Step 5. army pane.
	target.draw(_armyPane, states);
	// Step 6. tile pane.
	target.draw(_tilePane, states);
}

bool awe::map::_isOutOfBounds(const sf::Vector2u pos) const noexcept {
	return pos.x >= getMapSize().x || pos.y >= getMapSize().y;
}

bool awe::map::_tileIsVisible(const sf::Vector2u pos) const noexcept {
	return true;
}

bool awe::map::_isArmyPresent(const awe::ArmyID id) const noexcept {
	return _armies.find(id) != _armies.end();
}

bool awe::map::_isUnitPresent(const awe::UnitID id) const noexcept {
	return _units.find(id) != _units.end();
}

awe::UnitID awe::map::_findUnitID() {
	if (_units.size() == 0) return _lastUnitID;
	// Minus 1 to account for the reserved value, 0.
	if (_units.size() == (~((awe::UnitID)0)) - 1) 
		throw std::bad_alloc();
	awe::UnitID temp = _lastUnitID + 1;
	while (_isUnitPresent(temp)) {
		if (temp == ~((awe::UnitID)0))
			temp = 1;
		else
			temp++;
	}
	_lastUnitID = temp;
	return temp;
}

void awe::map::_CWM_Header(const bool isSave, unsigned char version) {
	sf::Uint32 finalVersion = FIRST_FILE_VERSION + version;
	if (isSave) {
		_file.writeNumber(finalVersion);
	} else {
		finalVersion = _file.readNumber<sf::Uint32>();
		version = finalVersion - FIRST_FILE_VERSION;
	}
	switch (version) {
	case 0:
		_CWM_0(isSave);
		break;
	case 1:
		_CWM_1(isSave);
		break;
	case 2:
		_CWM_2(isSave);
		break;
	default:
		_logger.error("CWM version {} is unrecognised!", version);
		throw std::exception("read above");
	}
}

void awe::map::_CWM_0(const bool isSave) {
	if (isSave) {
		_file.writeString(getMapName());
		_file.writeNumber((sf::Uint32)getMapSize().x);
		_file.writeNumber((sf::Uint32)getMapSize().y);
		_file.writeNumber((sf::Uint32)_armies.size());
		for (auto& army : _armies) {
			_file.writeNumber(army.second.getCountry()->getID());
			_file.writeNumber(army.second.getFunds());
		}
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto& tile = _tiles[x][y];
				_file.writeNumber(tile.getTileType()->getID());
				_file.writeNumber(tile.getTileHP());
				_file.writeNumber(tile.getTileOwner());
				if (tile.getUnit()) {
					_CWM_0_Unit(isSave, tile.getUnit(), sf::Vector2u(x, y));
				}
				// Covers the following cases:
				// 1. Tile is vacant.
				// 2. Unit has no loaded units on it.
				// 3. Unit has loaded units on it, but there are no more to load.
				_file.writeNumber(awe::army::NO_ARMY);
			}
		}
	} else {
		setMapName(_file.readString());
		sf::Uint32 width = _file.readNumber<sf::Uint32>();
		sf::Uint32 height = _file.readNumber<sf::Uint32>();
		setMapSize(sf::Vector2u(width, height));
		sf::Uint32 armyCount = _file.readNumber<sf::Uint32>();
		for (sf::Uint64 army = 0; army < armyCount; army++) {
			auto pCountry = (*_countries)[_file.readNumber<awe::BankID>()];
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
				if (setTileType(pos,
					(*_tileTypes)[_file.readNumber<awe::BankID>()])) {
					awe::HP hp = _file.readNumber<awe::HP>();
					setTileHP(pos, hp);
					awe::ArmyID army = _file.readNumber<awe::ArmyID>();
					setTileOwner(pos, army);
					_CWM_0_Unit(isSave, 0, sf::Vector2u(x, y));
				} else {
					throw std::exception("read above");
				}
			}
		}
	}
}

void awe::map::_CWM_1(const bool isSave) {
	if (isSave) {
		_file.writeString(getMapName());
		_file.writeNumber((sf::Uint32)getMapSize().x);
		_file.writeNumber((sf::Uint32)getMapSize().y);
		_file.writeNumber((sf::Uint32)getSelectedTile().x);
		_file.writeNumber((sf::Uint32)getSelectedTile().y);
		_file.writeNumber((sf::Uint32)_armies.size());
		for (auto& army : _armies) {
			_file.writeNumber(army.second.getCountry()->getID());
			_file.writeNumber(army.second.getFunds());
			// There should always be a current CO...
			if (army.second.getCurrentCO()) {
				_file.writeNumber(army.second.getCurrentCO()->getID());
			} else {
				_file.writeNumber(awe::army::NO_ARMY);
			}
			if (army.second.getTagCO()) {
				_file.writeNumber(army.second.getTagCO()->getID());
			} else {
				_file.writeNumber(awe::army::NO_ARMY);
			}
		}
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto& tile = _tiles[x][y];
				_file.writeNumber(tile.getTileType()->getID());
				_file.writeNumber(tile.getTileHP());
				_file.writeNumber(tile.getTileOwner());
				if (tile.getUnit()) {
					_CWM_0_Unit(isSave, tile.getUnit(), sf::Vector2u(x, y));
				}
				// Covers the following cases:
				// 1. Tile is vacant.
				// 2. Unit has no loaded units on it.
				// 3. Unit has loaded units on it, but there are no more to load.
				_file.writeNumber(awe::army::NO_ARMY);
			}
		}
	} else {
		setMapName(_file.readString());
		sf::Uint32 width = _file.readNumber<sf::Uint32>();
		sf::Uint32 height = _file.readNumber<sf::Uint32>();
		setMapSize(sf::Vector2u(width, height));
		sf::Uint32 sel_x = _file.readNumber<sf::Uint32>();
		sf::Uint32 sel_y = _file.readNumber<sf::Uint32>();
		setSelectedTile(sf::Vector2u(sel_x, sel_y));
		sf::Uint32 armyCount = _file.readNumber<sf::Uint32>();
		for (sf::Uint64 army = 0; army < armyCount; army++) {
			auto pCountry = (*_countries)[_file.readNumber<awe::BankID>()];
			if (createArmy(pCountry)) {
				awe::Funds funds = _file.readNumber<awe::Funds>();
				setArmyFunds(pCountry->getID(), funds);
				awe::BankID currentCO = _file.readNumber<awe::BankID>();
				awe::BankID tagCO = _file.readNumber<awe::BankID>();
				std::shared_ptr<const awe::commander> primaryCO = nullptr,
					secondaryCO = nullptr;
				if (currentCO != awe::army::NO_ARMY) {
					primaryCO = (*_commanders)[currentCO];
				}
				if (tagCO != awe::army::NO_ARMY) {
					secondaryCO = (*_commanders)[tagCO];
				}
				setArmyCOs(pCountry->getID(), primaryCO, secondaryCO);
			} else {
				throw std::exception("read above");
			}
		}
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto pos = sf::Vector2u(x, y);
				if (setTileType(pos,
					(*_tileTypes)[_file.readNumber<awe::BankID>()])) {
					awe::HP hp = _file.readNumber<awe::HP>();
					setTileHP(pos, hp);
					awe::ArmyID army = _file.readNumber<awe::ArmyID>();
					setTileOwner(pos, army);
					_CWM_0_Unit(isSave, 0, sf::Vector2u(x, y));
				} else {
					throw std::exception("read above");
				}
			}
		}
	}
}

bool awe::map::_CWM_0_Unit(const bool isSave, awe::UnitID id,
	const sf::Vector2u& curtile, const awe::UnitID loadOnto) {
	if (isSave) {
		auto& unit = _units.at(id);
		_file.writeNumber(unit.getArmy());
		_file.writeNumber(unit.getType()->getID());
		_file.writeNumber(unit.getHP());
		_file.writeNumber(unit.getFuel());
		_file.writeNumber(unit.getAmmo());
		auto loaded = unit.loadedUnits();
		if (loaded.size()) {
			for (auto loadedUnitID : loaded) {
				_CWM_0_Unit(isSave, loadedUnitID, curtile);
			}
			_file.writeNumber(awe::army::NO_ARMY);
		}
		return true;
	} else {
		auto ownerArmy = _file.readNumber<awe::ArmyID>();
		if (ownerArmy != awe::army::NO_ARMY) {
			auto unitID = createUnit(
				(*_unitTypes)[_file.readNumber<awe::BankID>()], ownerArmy);
			if (unitID) {
				auto hp = _file.readNumber<awe::HP>();
				setUnitHP(unitID, hp);
				auto fuel = _file.readNumber<awe::Fuel>();
				setUnitFuel(unitID, fuel);
				auto ammo = _file.readNumber<awe::Ammo>();
				setUnitAmmo(unitID, ammo);
				if (loadOnto)
					loadUnit(unitID, loadOnto);
				else
					setUnitPosition(unitID, curtile);
				while (_CWM_0_Unit(isSave, id, curtile,
					((loadOnto) ? (loadOnto) : (unitID))));
			} else {
				throw std::exception("read above");
			}
			return true;
		} else {
			return false;
		}
	}
}

void awe::map::_CWM_2(const bool isSave) {
	if (isSave) {
		_file.writeString(getMapName());
		_file.writeNumber((sf::Uint32)getMapSize().x);
		_file.writeNumber((sf::Uint32)getMapSize().y);
		_file.writeNumber((sf::Uint32)getSelectedTile().x);
		_file.writeNumber((sf::Uint32)getSelectedTile().y);
		_file.writeNumber(getDay());
		_file.writeNumber((sf::Uint32)getArmyCount());
		for (auto& army : _armies) {
			_file.writeNumber(army.second.getCountry()->getID());
			_file.writeNumber(army.second.getTeam());
			_file.writeNumber(army.second.getFunds());
			// There should always be a current CO...
			if (army.second.getCurrentCO()) {
				_file.writeNumber(army.second.getCurrentCO()->getID());
			} else {
				_file.writeNumber(awe::army::NO_ARMY);
			}
			if (army.second.getTagCO()) {
				_file.writeNumber(army.second.getTagCO()->getID());
			} else {
				_file.writeNumber(awe::army::NO_ARMY);
			}
		}
		_file.writeNumber(_currentArmy);
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto& tile = _tiles[x][y];
				_file.writeNumber(tile.getTileType()->getID());
				_file.writeNumber(tile.getTileHP());
				_file.writeNumber(tile.getTileOwner());
				if (tile.getUnit()) {
					_CWM_2_Unit(isSave, tile.getUnit(), sf::Vector2u(x, y));
				}
				// Covers the following cases:
				// 1. Tile is vacant.
				// 2. Unit has no loaded units on it.
				// 3. Unit has loaded units on it, but there are no more to load.
				_file.writeNumber(awe::army::NO_ARMY);
			}
		}
	} else {
		setMapName(_file.readString());
		sf::Uint32 width = _file.readNumber<sf::Uint32>();
		sf::Uint32 height = _file.readNumber<sf::Uint32>();
		setMapSize(sf::Vector2u(width, height));
		sf::Uint32 sel_x = _file.readNumber<sf::Uint32>();
		sf::Uint32 sel_y = _file.readNumber<sf::Uint32>();
		setSelectedTile(sf::Vector2u(sel_x, sel_y));
		awe::Day day = _file.readNumber<awe::Day>();
		setDay(day);
		sf::Uint32 armyCount = _file.readNumber<sf::Uint32>();
		for (sf::Uint64 army = 0; army < armyCount; army++) {
			auto pCountry = (*_countries)[_file.readNumber<awe::BankID>()];
			if (createArmy(pCountry)) {
				awe::TeamID team = _file.readNumber<awe::TeamID>();
				setArmyTeam(pCountry->getID(), team);
				awe::Funds funds = _file.readNumber<awe::Funds>();
				setArmyFunds(pCountry->getID(), funds);
				awe::BankID currentCO = _file.readNumber<awe::BankID>();
				awe::BankID tagCO = _file.readNumber<awe::BankID>();
				std::shared_ptr<const awe::commander> primaryCO = nullptr,
					secondaryCO = nullptr;
				if (currentCO != awe::army::NO_ARMY) {
					primaryCO = (*_commanders)[currentCO];
				}
				if (tagCO != awe::army::NO_ARMY) {
					secondaryCO = (*_commanders)[tagCO];
				}
				setArmyCOs(pCountry->getID(), primaryCO, secondaryCO);
			} else {
				throw std::exception("read above");
			}
		}
		awe::ArmyID currentArmy = _file.readNumber<awe::ArmyID>();
		selectArmy(currentArmy);
		for (sf::Uint32 y = 0; y < getMapSize().y; y++) {
			for (sf::Uint32 x = 0; x < getMapSize().x; x++) {
				auto pos = sf::Vector2u(x, y);
				if (setTileType(pos,
					(*_tileTypes)[_file.readNumber<awe::BankID>()])) {
					awe::HP hp = _file.readNumber<awe::HP>();
					setTileHP(pos, hp);
					awe::ArmyID army = _file.readNumber<awe::ArmyID>();
					setTileOwner(pos, army);
					_CWM_2_Unit(isSave, 0, sf::Vector2u(x, y));
				} else {
					throw std::exception("read above");
				}
			}
		}
	}
}

bool awe::map::_CWM_2_Unit(const bool isSave, awe::UnitID id,
	const sf::Vector2u& curtile, const awe::UnitID loadOnto) {
	if (isSave) {
		auto& unit = _units.at(id);
		_file.writeNumber(unit.getArmy());
		_file.writeNumber(unit.getType()->getID());
		_file.writeNumber(unit.getHP());
		_file.writeNumber(unit.getFuel());
		_file.writeNumber(unit.getAmmo());
		_file.writeBool(unit.isWaiting());
		auto loaded = unit.loadedUnits();
		if (loaded.size()) {
			for (auto loadedUnitID : loaded) {
				_CWM_2_Unit(isSave, loadedUnitID, curtile);
			}
			_file.writeNumber(awe::army::NO_ARMY);
		}
		return true;
	} else {
		auto ownerArmy = _file.readNumber<awe::ArmyID>();
		if (ownerArmy != awe::army::NO_ARMY) {
			auto unitID = createUnit(
				(*_unitTypes)[_file.readNumber<awe::BankID>()], ownerArmy);
			if (unitID) {
				auto hp = _file.readNumber<awe::HP>();
				setUnitHP(unitID, hp);
				auto fuel = _file.readNumber<awe::Fuel>();
				setUnitFuel(unitID, fuel);
				auto ammo = _file.readNumber<awe::Ammo>();
				setUnitAmmo(unitID, ammo);
				auto isWaiting = _file.readBool();
				waitUnit(unitID, isWaiting);
				if (loadOnto)
					loadUnit(unitID, loadOnto);
				else
					setUnitPosition(unitID, curtile);
				while (_CWM_2_Unit(isSave, id, curtile,
					((loadOnto) ? (loadOnto) : (unitID))));
			} else {
				throw std::exception("read above");
			}
			return true;
		} else {
			return false;
		}
	}
}

void awe::map::_initShaders() noexcept {
	_unavailableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xyz /= 2.0; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_unavailableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_availableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xy *= 1.1; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_availableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_attackableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.x += 0.85; pixel.yz += 0.05;"
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_attackableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
}