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

#include "unit.h"

awe::unit_bank::unit_bank(awe::movement* movetypes, const std::string& name) noexcept : _logger(name) {
	if (!movetypes) {
		_logger.error("No movement types bank has been provided for this unit bank.");
	}
}

bool awe::unit_bank::find(const std::size_t& id) const noexcept {
	return id < _types.size();
}

const awe::unit_data* awe::unit_bank::operator[](const std::size_t& id) const noexcept {
	if (!find(id)) return nullptr;
	return &_types[id];
}

bool awe::unit_bank::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		awe::unit_data newtype;
		j.apply(newtype.nativeName, { i.key(), "longname" }, &newtype.nativeName, true);
		j.apply(newtype.nativeShortName, { i.key(), "shortname" }, &newtype.nativeShortName, true);
		j.apply(newtype.spriteKey, { i.key(), "sprite" }, &newtype.spriteKey, true);
		j.apply(newtype.description, { i.key(), "description" }, &newtype.description, true);
		j.apply(newtype.cost, { i.key(), "cost" }, &newtype.cost, true);
		j.apply(newtype.max_hp, { i.key(), "hp" }, &newtype.max_hp, true);
		j.apply(newtype.max_fuel, { i.key(), "fuel" }, &newtype.max_fuel, true);
		j.apply(newtype.max_ammo, { i.key(), "ammo" }, &newtype.max_ammo, true);
		j.apply(newtype.movementTypeID, { i.key(), "movement" }, &newtype.movementTypeID, true);
		if (_movementTypes) {
			if (!_movementTypes->find(newtype.movementTypeID)) {
				_logger.error("Could not find movement type with ID {}, for unit with key \"{}\": expect erroneous behaviour.", newtype.movementTypeID, i.key());
			}
		}
		newtype.id = (unsigned int)_types.size();
		_types.push_back(newtype);
	}
	return true;
}

bool awe::unit_bank::_save(nlohmann::json& j) noexcept {
	return false;
}

awe::unit::unit(const unit_data* type, const unsigned int owner, const int hp, const int fuel, const int ammo) noexcept :
	_unitType(type), _owner(owner), _hp(hp), _fuel(fuel), _ammo(ammo) {}

const awe::unit_data* awe::unit::setType(const awe::unit_data* newType) noexcept {
	auto old = getType();
	_unitType = newType;
	return old;
}

const awe::unit_data* awe::unit::getType() const noexcept {
	return _unitType;
}

unsigned int awe::unit::setOwner(const unsigned int newOwner) noexcept {
	auto old = getOwner();
	_owner = newOwner;
	return old;
}

unsigned int awe::unit::getOwner() const noexcept {
	return _owner;
}

int awe::unit::setHP(const int newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	return old;
}

int awe::unit::getHP() const noexcept {
	return _hp;
}

int awe::unit::setFuel(const int newFuel) noexcept {
	auto old = getFuel();
	_fuel = newFuel;
	return old;
}

int awe::unit::getFuel() const noexcept {
	return _fuel;
}

int awe::unit::setAmmo(const int newAmmo) noexcept {
	auto old = getAmmo();
	_ammo = newAmmo;
	return old;
}

int awe::unit::getAmmo() const noexcept {
	return _ammo;
}