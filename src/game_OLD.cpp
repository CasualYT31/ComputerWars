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

#include "game.h"

awe::game::game(const std::string& name) noexcept : _logger(name) {}

void awe::game::setMapName(const std::string& name) noexcept {
	_map->setName(name);
}

std::string awe::game::getMapName() const noexcept {
	return _map->getName();
}

void awe::game::setMapSize(const sf::Vector2u dim) noexcept {
	// automatically handles tile deletion
	_map->setSize(dim);
	// now handle unit deletion
	// assign current spritesheet again in case new tiles are created
	setTileSpritesheet(_tileSheet);
}

sf::Vector2u awe::game::getMapSize() const noexcept {
	return _map->getSize();
}

void awe::game::setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
	_map->setTileSpritesheet(ptr);
	_tileSheet = ptr;
}

std::shared_ptr<sfx::animated_spritesheet> awe::game::getTileSpritesheet() const noexcept {
	return _tileSheet;
}

void awe::game::setTileType(const sf::Vector2u pos, const std::shared_ptr<const awe::tile_type>& type) noexcept {
	_map->setTileType(pos, type);
}

std::shared_ptr<const awe::tile_type> awe::game::getTileType(const sf::Vector2u pos) const noexcept {
	return _map->getTileType(pos);
}

void awe::game::setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept {
	_map->setTileHP(pos, hp);
}

awe::HP awe::game::getTileHP(const sf::Vector2u pos) const noexcept {
	return _map->getTileHP(pos);
}

void awe::game::setTileOwner(const sf::Vector2u pos, const std::shared_ptr<const awe::country>& country) noexcept {
	auto a = _findArmy(country);
	if (a) _map->setTileOwner(pos, a);
}

std::weak_ptr<awe::army> awe::game::getTileOwner(const sf::Vector2u pos) const noexcept {
	return _map->getTileOwner(pos);
}

bool awe::game::isTileVacant(const sf::Vector2u pos) const noexcept {
	return _map->isTileVacant(pos);
}

void awe::game::createArmy(const std::shared_ptr<const awe::country>& country) noexcept {
	if (!_findArmy(country)) _armies.push_back(std::make_shared<awe::army>(country, (awe::TeamID)country->UUID.getID()));
}

void awe::game::deleteArmy(const std::shared_ptr<const awe::country>& country) noexcept {
	// automatically handles unit deletion
	for (auto itr = _armies.begin(), enditr = _armies.end(); itr != enditr; itr++) {
		if (*(*itr)->getCountry() == *country) {
			_armies.erase(itr);
			break;
		}
	}
	// now handle disowning of tiles
}

void awe::game::setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
	for (auto itr = _armies.begin(), enditr = _armies.end(); itr != enditr; itr++) {
		(*itr)->setUnitSpritesheet(ptr);
	}
	_unitSheet = ptr;
}

std::shared_ptr<sfx::animated_spritesheet> awe::game::getUnitSpritesheet() const noexcept {
	return _unitSheet;
}

void awe::game::createUnit(const std::shared_ptr<const awe::country>& country, const std::shared_ptr<const awe::unit_type>& unitType) noexcept {
	if (unitType) {
		auto a = _findArmy(country);
		if (a) {
			a->createUnit(unitType);
		} else {
			_logger.error("Army \"{}\" does not exist.", ((country) ? (country->getName()) : ("[NULL]")));
		}
	} else {
		_logger.error("No unit type was given when attempting to create unit for army \"{}\".", ((country) ? (country->getName()) : ("[NULL]")));
	}
}

void awe::game::deleteUnit(const engine::uuid<awe::unit> uuid) noexcept {
	for (auto itr = _armies.begin(), enditr = _armies.end(); itr != enditr; itr++) {
		if ((*itr)->deleteUnit(uuid)) break;
	}
}

void awe::game::setUnitPosition(const engine::uuid<awe::unit> uuid, const sf::Vector2u pos) noexcept {
	auto map = getMapSize();
	if (pos.x > map.x || pos.y > map.y) {
		_logger.error("Attempted to move a unit with UUID {} to location ({},{}), which is out of bounds with the current map of size ({},{}).",
			uuid.getID(), pos.x, pos.y, map.x, map.y);
	} else {
		// set unit's location and set unit to tile at location
		// ... can't do it because i need a shared_ptr to tile which game can't get...
	}
}

engine::uuid<awe::unit> awe::game::unitAt(const sf::Vector2u pos) const noexcept {
	// access via map class
}

std::shared_ptr<awe::army> awe::game::_findArmy(const std::shared_ptr<const awe::country>& country) const noexcept {
	if (country) {
		for (auto itr = _armies.begin(), enditr = _armies.end(); itr != enditr; itr++) {
			if (*(*itr)->getCountry() == *country) return *itr;
		}
	}
	return nullptr;
}

// OLD
//#include <fstream>
//
//awe::game::game(const std::string& name) noexcept : _logger(name) {}
//
//bool awe::game::read(std::string filename) noexcept {
//	// perform initial checks
//	if (!_countries) {
//		_logger.error("Reading map operation cancelled: no countries bank has been assigned to this game object.");
//		return false;
//	}
//	if (!_commanders) {
//		_logger.error("Reading map operation cancelled: no commanders bank has been assigned to this game object.");
//		return false;
//	}
//	if (!_tiles) {
//		_logger.error("Reading map operation cancelled: no tile types bank has been assigned to this game object.");
//		return false;
//	}
//	if (!_units) {
//		_logger.error("Reading map operation cancelled: no unit types bank has been assigned to this game object.");
//		return false;
//	}
//	if (!_sprites) {
//		_logger.error("Reading map operation cancelled: no spritesheets have been assigned to this game object.");
//		return false;
//	}
//	// read
//	std::shared_ptr<awe::map> map = std::make_shared<awe::map>();
//	std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies = std::make_shared<std::vector<std::shared_ptr<awe::army>>>();
//	try {
//		std::shared_ptr<engine::binary_file> file = _openFile(filename, true);
//		sf::Uint32 version = file->readNumber<sf::Uint32>();
//
//		if (version == 1297564416) { // first version of the standard format
//			_read_CWM_1(file, map, armies);
//		} else {
//			_logger.error("The version of the format used in the map file \"{}\" is unsupported: {}.", filename, version);
//			return false;
//		}
//	} catch (std::exception& e) {
//		_logger.error("An error occurred when attempting to read map file \"{}\": {}", filename, e.what());
//		return false;
//	}
//	_map = map;
//	_armies = armies;
//	_filename = filename;
//	return true;
//}
//
//bool awe::game::write(std::string filename) noexcept {
//	try {
//		std::shared_ptr<engine::binary_file> file = _openFile(filename, false);
//
//		file->writeNumber(VERSION_NUMBER);
//
//
//	} catch (std::exception& e) {
//		_logger.error("An error occurred when attempting to write to map file \"{}\": {}", filename, e.what());
//		return false;
//	}
//	_filename = filename;
//	return true;
//}
//
//void awe::game::_read_CWM_1(const std::shared_ptr<engine::binary_file>& file, std::shared_ptr<awe::map> map, std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies) {
//	// 1 - read number of armies in play
//	sf::Uint8 armyCount = file->readNumber<sf::Uint8>();
//	// 2 - read each army's information
//	for (sf::Uint16 a = 0; a < armyCount; a++) {
//		armies->push_back(std::make_shared<awe::army>());
//		awe::army& army = *(*armies)[a];
//		// a - read team ID
//		army.setTeam(file->readNumber<awe::TeamID>());
//		// b - read country
//		army.setCountry((*_countries)[file->readNumber<awe::bank<const awe::country>::index>()]);
//		// c - read funds
//		army.setFunds(file->readNumber<awe::Funds>());
//		// d - read commanders: first CO, second CO
//		awe::bank<const awe::commander>::index first = file->readNumber<awe::bank<const awe::commander>::index>(),
//			second = file->readNumber<awe::bank<const awe::commander>::index>();
//		army.setCommanders(( (first != NO_CO) ? ((*_commanders)[first]) : (nullptr) ),
//			( (second != NO_CO) ? ((*_commanders)[second]) : (nullptr) ));
//		// extra - assign spritesheet
//		army.setUnitSpritesheet(_sprites->unit->idle);
//	}
//	// 3 - read map name
//	map->setName(file->readString());
//	// 4 - read map dimensions: X, Y
//	auto w = file->readNumber<unsigned int>();
//	auto h = file->readNumber<unsigned int>();
//	map->setSize(sf::Vector2u(w, h));
//	// 5 - read each map tile, reading left-to-right, then top-to-bottom
//	for (sf::Uint32 y = 0, endY = map->getSize().y; y < endY; y++) {
//		for (sf::Uint32 x = 0, endX = map->getSize().x; x < endX; x++) {
//			std::shared_ptr<awe::tile> tile = map->getTile(sf::Vector2u(x, y));
//			// a - read tile type
//			tile->setTile((*_tiles)[file->readNumber<awe::bank<awe::tile_type>::index>()]);
//			// b - read tile HP
//			tile->setHP(file->readNumber<sf::Int32>());
//			// c - read owner army ID
//			sf::Uint8 owningArmy = file->readNumber<sf::Uint8>();
//			if (owningArmy != NO_ARMY) {
//				if (owningArmy >= armyCount) {
//					_logger.error("Illegal owner ID, {}, with army count {}, found at tile ({}, {}), byte position {}.", owningArmy, armyCount, x, y, file->position());
//				} else {
//					std::shared_ptr<awe::army> army = (*armies)[owningArmy];
//					tile->setOwner(army);
//					army->addOwnedTile(tile);
//				}
//			}
//			// d - read unit occupying tile, if any
//			std::shared_ptr<awe::unit> unit = _read_unit_CWM_1(file, armyCount, x, y, armies);
//			if (unit) tile->setUnit(unit);
//		}
//	}
//}
//
//std::shared_ptr<awe::unit> awe::game::_read_unit_CWM_1(const std::shared_ptr<engine::binary_file>& file, sf::Uint8 armyCount, sf::Uint32 x, sf::Uint32 y, std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies) {
//	// 1 - read unit's owner
//	sf::Uint8 owningArmy = file->readNumber<sf::Uint8>();
//	if (owningArmy != NO_ARMY) {
//		if (owningArmy >= armyCount) {
//			_logger.error("Illegal army ID, {}, with army count {}, for unit at tile ({}, {}), byte position {}.", owningArmy, armyCount, x, y, file->position());
//		} else {
//			std::shared_ptr<awe::army> army = (*armies)[owningArmy];
//			// 2 - read unit type
//			std::shared_ptr<awe::unit> unit = army->addUnit((*_units)[file->readNumber<awe::bank<awe::unit_type>::index>()]);
//			// 3 - read unit HP
//			unit->setHP(file->readNumber<sf::Int32>());
//			// 4 - read unit fuel
//			unit->setFuel(file->readNumber<sf::Int32>());
//			// 5 - read unit ammo
//			unit->setAmmo(file->readNumber<sf::Int32>());
//			// 6 - read loaded units info (recursive calls)
//			while (std::shared_ptr<awe::unit> lUnit = _read_unit_CWM_1(file, armyCount, x, y, armies)) {
//				// there was a loaded unit, so load it onto this one
//				unit->loadUnit(lUnit);
//			}
//			return unit;
//		}
//	}
//	return nullptr;
//}
//
//std::shared_ptr<engine::binary_file> awe::game::_openFile(std::string& filename, bool forInput) {
//	if (filename == "") filename = _filename;
//	std::shared_ptr<engine::binary_file> ret = std::make_shared<engine::binary_file>();
//	ret->open(filename, forInput);
//	return ret;
//}
//
//void awe::game::setMapSize(const sf::Vector2u& newSize) noexcept {
//	if (_map) _map->setSize(newSize);
//}
//
//sf::Vector2u awe::game::getMapSize() const noexcept {
//	if (_map) {
//		return _map->getSize();
//	} else {
//		return sf::Vector2u(0, 0);
//	}
//}
//
//void awe::game::setMapName(const std::string& newName) noexcept {
//	if (_map) _map->setName(newName);
//}
//
//std::string awe::game::getMapName() const noexcept {
//	if (_map) {
//		return _map->getName();
//	} else {
//		return "";
//	}
//}
//
//void awe::game::createArmy(awe::bank<awe::country>::index id) noexcept {
//	if (_armies && _countries) {
//		for (auto itr = _armies->begin(), enditr = _armies->end(); itr != enditr; itr++) {
//			if (*(*itr)->getCountry() == *(*_countries)[id]) {
//				_logger.warning("Attempted to create a new army with a country that already exists on the map, country ID {}.", id);
//				return;
//			}
//		}
//		try {
//			_armies->insert(_getArmyIterator((unsigned int)id), std::make_shared<awe::army>((awe::TeamID)id, (*_countries)[id]));
//		} catch (std::exception& e) {
//			_logger.error("Failed to create new army with country ID {}: {}", id, e.what());
//		}
//	}
//}
//
//void awe::game::deleteArmy(const std::shared_ptr<const awe::country>& ptr) noexcept {
//	if (_armies && ptr) {
//		for (auto itr = _armies->begin(), enditr = _armies->end(); itr != enditr; itr++) {
//			if (*(*itr)->getCountry() == *ptr) {
//				_armies->erase(itr);
//				break;
//			}
//		}
//	}
//}
//
//void awe::game::setArmysTeam(const std::shared_ptr<const awe::country>& ptr, awe::TeamID newTeam) noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->setTeam(newTeam);
//}
//
//awe::TeamID awe::game::getArmysTeam(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->getTeam();
//	return NO_ARMY;
//}
//
//void awe::game::setArmysFunds(const std::shared_ptr<const awe::country>& ptr, awe::Funds funds) noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->setFunds(funds);
//}
//
//awe::Funds awe::game::getArmysFunds(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->getFunds();
//	return -1;
//}
//
//void awe::game::setArmysCommanders(const std::shared_ptr<const awe::country>& ptr, std::shared_ptr<const awe::commander> firstCO, std::shared_ptr<const awe::commander> secondCO) noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->setCommanders(firstCO, secondCO);
//}
//
//std::shared_ptr<const awe::commander> awe::game::getFirstCommander(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->getFirstCommander();
//	return nullptr;
//}
//
//std::shared_ptr<const awe::commander> awe::game::getSecondCommander(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) army->getSecondCommander();
//	return nullptr;
//}
//
//bool awe::game::isTagTeam(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	auto army = _findArmyByCountry(ptr);
//	if (army) return army->getFirstCommander() && army->getSecondCommander();
//	return false;
//}
//
//std::size_t awe::game::getNumberOfArmies() const noexcept {
//	if (_armies) return _armies->size();
//	return 0;
//}
//
//std::shared_ptr<awe::unit> awe::game::createUnit(const std::shared_ptr<const awe::country>& owningCountry, const std::shared_ptr<const awe::unit_type>& type, sf::Vector2u location) noexcept {
//	auto owningArmy = _findArmyByCountry(owningCountry);
//	try {
//		if (owningArmy && type && _map && _map->getTile(location) && !_map->getTile(location)->isOccupied()) {
//			auto unit = owningArmy->addUnit(type);
//			unit->setOwner(owningArmy);
//			_map->getTile(location)->setUnit(unit);
//			unit->setTile(_map->getTile(location));
//			return unit;
//		} else if (_map->getTile(location)->isOccupied()) {
//			_logger.error("Tile was occupied!");
//		}
//	} catch (std::out_of_range&) { // given location was out of range
//		_logger.error("Attempted to create a new unit at ({}, {}) which is outside the map's range of ({}, {}).", location.x, location.y, _map->getSize().x, _map->getSize().y);
//	}
//	_logger.error("Failed to create unit of type \"{}\" for army \"{}\" at location ({}, {}).",
//		((type)?(type->getName()):("[NULL]")),
//		( (owningArmy) ? ((owningArmy->getCountry())?(owningArmy->getCountry()->getName()):("[No Country]")) : ("[NULL]") ),
//		location.x, location.y);
//	if (!_map) _logger.error("Map is unallocated!");
//	return nullptr;
//}
//
//bool awe::game::deleteUnit(const std::shared_ptr<awe::unit>& ref) noexcept {
//	if (ref) {
//		if (auto pOwner = ref->getOwner().lock()) {
//			pOwner->removeUnit(ref);
//			return true;
//		} else {
//			_logger.error("Attempted to delete a unit without an owning army.");
//		}
//	} else {
//		_logger.error("Could not delete unit, given reference was null.");
//	}
//	return false;
//}
//
//bool awe::game::changeTileOwner(const std::shared_ptr<awe::tile>& ref, const std::shared_ptr<const awe::country>& newOwningCountry) noexcept {
//	auto newOwningArmy = _findArmyByCountry(newOwningCountry);
//	if (ref) {
//		auto pOldOwner = ref->getOwner().lock();
//		if (pOldOwner) pOldOwner->removeOwnedTile(ref);
//		ref->setOwner(newOwningArmy);
//		if (newOwningArmy) newOwningArmy->addOwnedTile(ref);
//		return true;
//	} else {
//		_logger.error("Failed to give ownership of tile at ({}, {}) to army \"{}\".",
//			((ref)?(std::to_string(ref->getLocation().x)):("[NULL]")),
//			((ref)?(std::to_string(ref->getLocation().y)):("[NULL]")),
//			((newOwningArmy)?((newOwningArmy->getCountry())?(newOwningArmy->getCountry()->getName()):("[No Country]")):("[NULL]")));
//		return false;
//	}
//}
//
//bool awe::game::moveUnit(const std::shared_ptr<awe::unit>& ref, sf::Vector2u newLocation) noexcept {
//	try {
//		if (ref && _map && _map->getTile(newLocation)) {
//			auto pOldTile = ref->getTile().lock();
//			// should accept movement if the same tile is selected
//			if (_map->getTile(newLocation)->isOccupied() && (pOldTile ? (_map->getTile(newLocation) != pOldTile) : true)) {
//				_logger.error("Attempted to move a unit onto a tile occupied by another unit.");
//				return false;
//			}
//			// move unit
//			if (pOldTile) pOldTile->setUnit(nullptr);
//			// remove unit from old tile
//			_map->getTile(newLocation)->setUnit(ref); // add unit to new tile
//			ref->setTile(_map->getTile(newLocation)); // overwrite old tile reference in unit with new tile reference
//			return true;
//		} else {
//			_logger.error("Could not move unit at location ({}, {}) to ({}, {}).", 0, 0, newLocation.x, newLocation.y);
//		}
//	} catch (std::out_of_range&) { // given location was out of range
//		_logger.error("Attempted to move a unit to a location outside the map's range of ({}, {}).", _map->getSize().x, _map->getSize().y);
//	}
//	return false;
//}
//
//bool awe::game::loadUnit(const std::shared_ptr<awe::unit>& dest, const std::shared_ptr<awe::unit>& src) noexcept {
//	if (dest && src) {
//		auto ret = dest->loadUnit(src);
//		if (ret) {
//			src->setHolderUnit(dest);
//			auto pTile = src->getTile().lock();
//			if (pTile) {
//				pTile->setUnit(nullptr);
//				src->setTile(nullptr);
//			}
//		} else {
//			_logger.error("Attempt to load unit of type \"{}\" onto unit of type \"{}\" failed.", ((src->getType())?(src->getType()->getName()):("[NULL]")), ((dest->getType()) ? (dest->getType()->getName()) : ("[NULL]")));
//		}
//		return ret;
//	}
//	_logger.error("Attempted to call loadUnit() with empty references.");
//	return false;
//}
//
//bool awe::game::unloadUnit(const std::shared_ptr<awe::unit>& dest, const std::shared_ptr<awe::unit>& src, sf::Vector2u newLocation) noexcept {
//	if (dest && src) {
//		auto ret = moveUnit(src, newLocation);
//		if (ret) {
//			ret = dest->unloadUnit(src);
//			if (ret) src->setHolderUnit(nullptr);
//		} else {
//			_logger.error("Attempted to unload a unit onto an invalid tile.");
//		}
//		if (!ret) {
//			_logger.error("Attempt to unload unit of type \"{}\" from unit of type \"{}\" failed.", ((src->getType()) ? (src->getType()->getName()) : ("[NULL]")), ((dest->getType()) ? (dest->getType()->getName()) : ("[NULL]")));
//		}
//		return ret;
//	}
//	_logger.error("Attempted to call unloadUnit() with empty references.");
//	return false;
//}
//
//void awe::game::setCountries(const std::shared_ptr<awe::bank<const awe::country>>& ptr) noexcept {
//	_countries = ptr;
//}
//
//void awe::game::setCommanders(const std::shared_ptr<awe::bank<const awe::commander>>& ptr) noexcept {
//	_commanders = ptr;
//}
//
//void awe::game::setTiles(const std::shared_ptr<awe::bank<const awe::tile_type>>& ptr) noexcept {
//	_tiles = ptr;
//}
//
//void awe::game::setUnits(const std::shared_ptr<awe::bank<const awe::unit_type>>& ptr) noexcept {
//	_units = ptr;
//}
//
//void awe::game::setSpritesheets(const std::shared_ptr<awe::spritesheets>& ptr) noexcept {
//	_sprites = ptr;
//	// debugging
//	_map->setTileSpritesheet(ptr->tile);
//}
//
//std::vector<std::shared_ptr<awe::army>>::iterator awe::game::_getArmyIterator(unsigned int index) const {
//	if (!_armies) throw std::out_of_range("Internal armies vector was NULL at the time of calling.");
//	auto itr = _armies->begin();
//	for (unsigned int counter = 0; counter < index || itr == _armies->end(); counter++, itr++);
//	return itr;
//}
//
//std::shared_ptr<awe::army> awe::game::_findArmyByCountry(const std::shared_ptr<const awe::country>& ptr) const noexcept {
//	if (_armies && ptr) {
//		for (auto itr = _armies->begin(), enditr = _armies->end(); itr != enditr; itr++) {
//			if (*(*itr)->getCountry() == *ptr) {
//				return *itr;
//			}
//		}
//	}
//	return nullptr;
//}
//
//bool awe::game::animate(const sf::RenderTarget& target) noexcept {
//	if (_map) return _map->animate(target);
//	return false;
//}
//
//void awe::game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
//	if (_map) target.draw(*_map, states);
//}