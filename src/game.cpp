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
#include <fstream>

awe::game::game(const std::string& name) noexcept : _logger(name) {}

bool awe::game::read(std::string filename) noexcept {
	// perform initial checks
	if (!_countries) {
		_logger.error("Reading map operation cancelled: no countries bank has been assigned to this game object.");
		return false;
	}
	if (!_commanders) {
		_logger.error("Reading map operation cancelled: no commanders bank has been assigned to this game object.");
		return false;
	}
	if (!_tiles) {
		_logger.error("Reading map operation cancelled: no tile types bank has been assigned to this game object.");
		return false;
	}
	if (!_units) {
		_logger.error("Reading map operation cancelled: no unit types bank has been assigned to this game object.");
		return false;
	}
	// read
	std::shared_ptr<awe::map> map = std::make_shared<awe::map>();
	std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies = std::make_shared<std::vector<std::shared_ptr<awe::army>>>();
	try {
		std::shared_ptr<engine::binary_file> file = _openFile(filename, true);
		sf::Uint32 version = file->readNumber<sf::Uint32>();

		if (version == 1297564416) { // first version of the standard format
			_read_CWM_1(file, map, armies);
		} else {
			_logger.error("The version of the format used in the map file \"{}\" is unsupported: {}.", filename, version);
			return false;
		}
	} catch (std::exception& e) {
		_logger.error("An error occurred when attempting to read map file \"{}\": {}", filename, e.what());
		return false;
	}
	_map = map;
	_armies = armies;
	_filename = filename;
	return true;
}

bool awe::game::write(std::string filename) noexcept {
	try {
		std::shared_ptr<engine::binary_file> file = _openFile(filename, false);

		file->writeNumber(VERSION_NUMBER);


	} catch (std::exception& e) {
		_logger.error("An error occurred when attempting to write to map file \"{}\": {}", filename, e.what());
		return false;
	}
	_filename = filename;
	return true;
}

void awe::game::_read_CWM_1(const std::shared_ptr<engine::binary_file>& file, std::shared_ptr<awe::map> map, std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies) {
	// 1 - read number of armies in play
	sf::Uint8 armyCount = file->readNumber<sf::Uint8>();
	// 2 - read each army's information
	for (sf::Uint16 a = 0; a < armyCount; a++) {
		armies->push_back(std::make_shared<awe::army>());
		awe::army& army = *(*armies)[a];
		// a - read team ID
		army.setTeam(file->readNumber<awe::TeamID>());
		// b - read country
		army.setCountry((*_countries)[file->readNumber<awe::bank<const awe::country>::index>()]);
		// c - read funds
		army.setFunds(file->readNumber<unsigned int>());
		// d - read commanders: first CO, second CO
		awe::bank<const awe::commander>::index first = file->readNumber<awe::bank<const awe::commander>::index>(),
			second = file->readNumber<awe::bank<const awe::commander>::index>();
		army.setCommanders(( (first != NO_CO) ? ((*_commanders)[first]) : (nullptr) ),
			( (second != NO_CO) ? ((*_commanders)[second]) : (nullptr) ));
	}
	// 3 - read map name
	map->setName(file->readString());
	// 4 - read map dimensions: X, Y
	map->setSize(sf::Vector2u(file->readNumber<unsigned int>(), file->readNumber<unsigned int>()));
	// 5 - read each map tile, reading left-to-right, then top-to-bottom
	for (sf::Uint32 y = 0, endY = map->getSize().y; y < endY; y++) {
		for (sf::Uint32 x = 0, endX = map->getSize().x; x < endX; x++) {
			std::shared_ptr<awe::tile> tile = map->getTile(sf::Vector2u(x, y));
			// a - read tile type
			tile->setTile((*_tiles)[file->readNumber<awe::bank<awe::tile_type>::index>()]);
			// b - read tile HP
			tile->setHP(file->readNumber<sf::Int32>());
			// c - read owner army ID
			sf::Uint8 owningArmy = file->readNumber<sf::Uint8>();
			if (owningArmy != NO_ARMY) {
				if (owningArmy >= armyCount) {
					_logger.error("Illegal owner ID, {}, with army count {}, found at tile ({}, {}), byte position {}.", owningArmy, armyCount, x, y, file->position());
				} else {
					std::shared_ptr<awe::army> army = (*armies)[owningArmy];
					tile->setOwner(army);
					army->addOwnedTile(tile);
				}
			}
			// d - read unit occupying tile, if any
			std::shared_ptr<awe::unit> unit = _read_unit_CWM_1(file, armyCount, x, y, armies);
			if (unit) tile->setUnit(unit);
		}
	}
}

std::shared_ptr<awe::unit> awe::game::_read_unit_CWM_1(const std::shared_ptr<engine::binary_file>& file, sf::Uint8 armyCount, sf::Uint32 x, sf::Uint32 y, std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies) {
	// 1 - read unit's owner
	sf::Uint8 owningArmy = file->readNumber<sf::Uint8>();
	if (owningArmy != NO_ARMY) {
		if (owningArmy >= armyCount) {
			_logger.error("Illegal army ID, {}, with army count {}, for unit at tile ({}, {}), byte position {}.", owningArmy, armyCount, x, y, file->position());
		} else {
			std::shared_ptr<awe::army> army = (*armies)[owningArmy];
			// 2 - read unit type
			std::shared_ptr<awe::unit> unit = army->addUnit((*_units)[file->readNumber<awe::bank<awe::unit_type>::index>()]);
			// 3 - read unit HP
			unit->setHP(file->readNumber<sf::Int32>());
			// 4 - read unit fuel
			unit->setFuel(file->readNumber<sf::Int32>());
			// 5 - read unit ammo
			unit->setAmmo(file->readNumber<sf::Int32>());
			// 6 - read loaded units info (recursive calls)
			while (std::shared_ptr<awe::unit> lUnit = _read_unit_CWM_1(file, armyCount, x, y, armies)) {
				// there was a loaded unit, so load it onto this one
				unit->loadUnit(lUnit);
			}
			return unit;
		}
	}
	return nullptr;
}

std::shared_ptr<engine::binary_file> awe::game::_openFile(std::string& filename, bool forInput) {
	if (filename == "") filename = _filename;
	std::shared_ptr<engine::binary_file> ret = std::make_shared<engine::binary_file>();
	ret->open(filename, forInput);
	return ret;
}

std::shared_ptr<awe::map> awe::game::getMap() const noexcept {
	return _map;
}

std::shared_ptr<awe::army> awe::game::getArmy(std::size_t i) const noexcept {
	if (i < getNumberOfArmies()) return (*_armies)[i];
	return std::shared_ptr<awe::army>();
}

std::size_t awe::game::getNumberOfArmies() const noexcept {
	if (_armies) return _armies->size();
	return 0;
}

void awe::game::setCountries(const std::shared_ptr<awe::bank<const awe::country>>& ptr) noexcept {
	_countries = ptr;
}

void awe::game::setCommanders(const std::shared_ptr<awe::bank<const awe::commander>>& ptr) noexcept {
	_commanders = ptr;
}

void awe::game::setTiles(const std::shared_ptr<awe::bank<const awe::tile_type>>& ptr) noexcept {
	_tiles = ptr;
}

void awe::game::setUnits(const std::shared_ptr<awe::bank<const awe::unit_type>>& ptr) noexcept {
	_units = ptr;
}