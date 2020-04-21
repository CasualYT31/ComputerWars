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

#include "terrain.h"

awe::terrain_bank::terrain_bank(awe::movement* movetypes, const std::string& name) noexcept : _movementTypes(movetypes), _logger(name) {
	if (!movetypes) {
		_logger.error("No movement types bank has been provided for this terrain bank.");
	}
}

bool awe::terrain_bank::find(const std::size_t& id) const noexcept {
	return id < _types.size();
}

const awe::terrain* awe::terrain_bank::operator[](const std::size_t& id) const noexcept {
	if (!find(id)) return nullptr;
	return &_types[id];
}

bool awe::terrain_bank::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		awe::terrain newtype;
		j.apply(newtype.nativeName, { i.key(), "longname" }, &newtype.nativeName, true);
		j.apply(newtype.nativeShortName, { i.key(), "shortname" }, &newtype.nativeShortName, true);
		j.apply(newtype.spriteKey, { i.key(), "picture" }, &newtype.spriteKey, true);
		j.apply(newtype.description, { i.key(), "description" }, &newtype.description, true);
		j.apply(newtype.max_hp, { i.key(), "hp" }, &newtype.max_hp, true);
		j.apply(newtype.defence, { i.key(), "defence" }, &newtype.defence, true);
		if (_movementTypes) {
			if (i.value().find("movecosts") != i.value().end()) {
				if (i.value()["movecosts"].is_object()) {
					for (auto& mc : i.value()["movecosts"].items()) {
						if (mc.value().is_number_integer()) {
							newtype.movecost[(*_movementTypes)[mc.key()].id] = mc.value();
						}
					}
					if (newtype.movecost.empty()) {
						_logger.error("No valid movement cost values have been provided for terrain of key \"{}\".", i.key());
					}
				} else {
					_logger.error("\"movecosts\" does not hold an object, for terrain of key \"{}\".", i.key());
				}
			} else {
				_logger.error("No movement costs have been provided for terrain of key \"{}\".", i.key());
			}
		}
		newtype.id = (unsigned int)_types.size();
		_types.push_back(newtype);
	}
	return true;
}

bool awe::terrain_bank::_save(nlohmann::json& j) noexcept {
	return false;
}

awe::terrain_tile_bank::terrain_tile_bank(awe::terrain_bank* tBank, awe::country* countries, const std::string& name) noexcept : _bank(tBank), _countries(countries), _logger(name) {
	if (!tBank) {
		_logger.error("No terrain bank has been provided for this terrain tile bank.");
	}
	if (!countries) {
		_logger.error("No country bank has been provided for this terrain tile bank.");
	}
}

bool awe::terrain_tile_bank::find(const std::size_t& id) const noexcept {
	return id < _tiles.size();
}

const awe::terrain_tile* awe::terrain_tile_bank::operator[](const std::size_t& id) const noexcept {
	if (!find(id)) return nullptr;
	return &_tiles[id];
}

bool awe::terrain_tile_bank::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		awe::terrain_tile newtile;
		if (_bank) {
			unsigned int tiletype = 0;
			j.apply(tiletype, { i.key(), "type" }, &tiletype, true);
			if (_bank->find(tiletype)) {
				newtile.type = (*_bank)[tiletype];
			} else {
				_logger.error("Could not find terrain type with ID \"{}\", for terrain tile of key \"{}\".", tiletype, i.key());
			}
		}
		if (_countries) {
			if (i.value().find("tiles") != i.value().end()) {
				if (i.value()["tiles"].is_object()) {
					for (auto& mc : i.value()["tiles"].items()) {
						if (mc.value().is_number_unsigned()) {
							newtile.tile[(*_countries)[mc.key()].id] = mc.value();
						}
					}
					if (newtile.tile.empty()) {
						_logger.error("No valid sprite keys have been provided for terrain tile of key \"{}\".", i.key());
					}
				} else {
					_logger.error("\"tiles\" does not hold an object, for terrain tile of key \"{}\".", i.key());
				}
			} else {
				_logger.error("No sprite keys have been provided for terrain tile of key \"{}\".", i.key());
			}
		}
		_tiles.push_back(newtile);
	}
	return true;
}

bool awe::terrain_tile_bank::_save(nlohmann::json& j) noexcept {
	return false;
}

awe::tile::tile(const awe::terrain_tile* tile, const unsigned int owner, const int hp) noexcept : _tileType(tile), _hp(hp), _owner(owner) {}

const awe::terrain_tile* awe::tile::setTile(const awe::terrain_tile* newTile) noexcept {
	auto old = getTile();
	_tileType = newTile;
	return old;
}

const awe::terrain_tile* awe::tile::getTile() const noexcept {
	return _tileType;
}

int awe::tile::setHP(const int newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	return old;
}

int awe::tile::getHP() const noexcept {
	return _hp;
}

unsigned int awe::tile::setOwner(const unsigned int newOwner) noexcept {
	auto old = getOwner();
	_owner = newOwner;
	return old;
}

unsigned int awe::tile::getOwner() const noexcept {
	return _owner;
}