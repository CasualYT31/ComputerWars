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

#include "map.h"

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
		_tiles[x].resize(dim.y, tile);
	}
	if (mapHasShrunk) {
		// next, go through each army's owned tiles and units and remove all out of bounds objects
		for (auto itr = _armys.begin(), enditr = _armys.end(); itr != enditr; itr++) {
			// do stuff
		}
		// then, go through all units and delete those that are out of bounds
		// I avoided using erase in the first loop in case that might invalidate the iterator
		std::vector<awe::UnitID> unitsToDelete;
		for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
			if (_isOutOfBounds(itr->second.getUnitPosition())) unitsToDelete.push_back(itr->first);
		}
		for (auto itr = unitsToDelete.begin(), enditr = unitsToDelete.end(); itr != enditr; itr++) {
			_units.erase(*itr);
		}
	}
}

sf::Vector2u awe::map::getMapSize() const noexcept {
	sf::Vector2u ret(_tiles.size(), 0);
	if (ret.x) ret.y = _tiles.at(0).size();
	return ret;
}

void awe::map::createArmy(const std::shared_ptr<const awe::country>& country) noexcept {
	if (!country || _isArmyPresent(country->UUID.getID())) return;
	_armys.insert({ country->UUID.getID(), awe::army(country) });
}

void awe::map::deleteArmy(const awe::UUIDValue army) noexcept {
	if (!_isArmyPresent(army)) return;
	// firstly, delete all units belonging to the army
	
	// then, disown all tiles
	
	// finally, delete the army from the army list
	_armys.erase(army);
}

awe::UnitID awe::map::createUnit(const std::shared_ptr<const awe::unit_type>& type, const awe::UUIDValue army) noexcept {
	if (!_isArmyPresent(army)) return 0;
	awe::UnitID id;
	try {
		id = _findUnitID();
	} catch (std::bad_alloc&) {
		// log
		return 0;
	}
	_units.insert({ id, awe::unit(type, army) });
	return id;
}

void awe::map::deleteUnit(const awe::UnitID id) noexcept {
	if (!_isUnitPresent(id)) return;
	// firstly, remove the unit from the tile, if it was on a tile
	
	// secondly, remove the unit from the army's list

	// thirdly, delete all units that are loaded onto this one
	
	// finally, delete the unit from the main list
	_units.erase(id);
}

void awe::map::setUnitPosition(const awe::UnitID id, const sf::Vector2u pos) noexcept {
	if (!_isUnitPresent(id) || _isOutOfBounds(pos)) return;
	if (getUnitOnTile(pos)) return;
	// make new tile occupied
	_tiles[pos.x][pos.y].setUnit(id);
	// make old tile vacant
	if (_units[id].isOnMap()) _tiles[_units[id].getPosition().x][_units[id].getPosition().y].setUnit(0);
	// assign new location to unit
	_units[id].setPosition(pos);
}

sf::Vector2u awe::map::getUnitPosition(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) return sf::Vector2u(0, 0);
	return _units.at(id).getPosition();
}

bool awe::map::isUnitOnMap(const awe::UnitID id) const noexcept {
	if (!_isUnitPresent(id)) return false;
	return _units.at(id).isOnMap();
}

void awe::map::setUnitHP(const awe::UnitID id, const awe::HP hp) noexcept {
	if (_isUnitPresent(id)) _units[id].setHP(hp);
}

awe::HP awe::map::getUnitHP(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getHP();
	return 0;
}

void awe::map::setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) noexcept {
	if (_isUnitPresent(id)) _units[id].setFuel(fuel);
}

awe::Fuel awe::map::getUnitFuel(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getFuel();
	return 0;
}

void awe::map::setUnitAmmo(const awe::UnitID id, const awe::Ammo ammo) noexcept {
	if (_isUnitPresent(id)) _units[id].setAmmo(ammo);
}

awe::Ammo awe::map::getUnitAmmo(const awe::UnitID id) const noexcept {
	if (_isUnitPresent(id)) return _units.at(id).getAmmo();
	return 0;
}

void awe::map::loadUnit(const awe::UnitID load, const awe::UnitID onto) noexcept {
	if (!_isUnitPresent(load) || !_isUnitPresent(onto)) return;
	if (_units[load].loadedOnto()) return;
	_units[onto].loadUnit(load);
	_units[load].loadOnto(onto);
}

void awe::map::unloadUnit(const awe::UnitID unload, const awe::UnitID from, const sf::Vector2u onto) noexcept {
	if (!_isUnitPresent(unload) || !_isUnitPresent(from)) return;
	if (getUnitOnTile(onto)) return;
	if (_units[from].unloadUnit(unload)) { // unload successful, continue with operation
		_units[unload].loadOnto(0);
		setUnitPosition(unload, onto);
	}
}

void awe::map::setTileType(const sf::Vector2u pos, const std::shared_ptr<const awe::tile_type>& type) noexcept {
	if (_isOutOfBounds(pos)) return;
	_tiles[pos.x][pos.y].setTileType(type);
	// remove ownership of the tile from the army who owns it, if any army does
	setTileOwner(pos, engine::uuid<awe::country>::INVALID);
}

std::shared_ptr<const awe::tile_type> awe::map::getTileType(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) return nullptr;
	return _tiles[pos.x][pos.y].getTileType();
}

void awe::map::setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept {
	if (!_isOutOfBounds(pos)) _tiles[pos.x][pos.y].setTileHP(hp);
}

awe::HP awe::map::getTileHP(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) return 0;
	return _tiles[pos.x][pos.y].getTileHP();
}

void awe::map::setTileOwner(const sf::Vector2u pos, awe::UUIDValue army) noexcept {
	if (_isOutOfBounds(pos)) return;
	auto& tile = _tiles[pos.x][pos.y];
	// first, remove the tile from the army who currently owns it
	if (_isArmyPresent(tile.getTileOwner())) _armys[tile.getTileOwner()].removeTile(pos);
	// now assign it to the real owner, if any
	if (_isArmyPresent(army)) _armys[army].addTile(pos);
	// update the actual tile now
	tile.setTileOwner(army);
}

awe::UUIDValue awe::map::getTileOwner(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) return engine::uuid<awe::country>::INVALID;
	return _tiles[pos.x][pos.y].getTileOwner();
}

awe::UnitID awe::map::getUnitOnTile(const sf::Vector2u pos) const noexcept {
	if (_isOutOfBounds(pos)) return 0;
	auto u = _tiles[pos.x][pos.y].getUnit();
	if (u != 0 && _units.at(u).isOnMap()) return u;
	return 0;
}

bool awe::map::_isOutOfBounds(const sf::Vector2u pos) const noexcept {
	return pos.x >= getMapSize().x || pos.y >= getMapSize().y;
}

bool awe::map::_isArmyPresent(const awe::UUIDValue id) const noexcept {
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
	while (temp == 0 || _isUnitPresent(temp)) temp++;
	_lastUnitID = temp;
	return temp;
}