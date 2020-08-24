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

awe::unit_linked_list::unit_linked_list(std::weak_ptr<awe::unit> init) noexcept {
	_unit = init;
}

void awe::unit_linked_list::append(std::weak_ptr<awe::unit> unit) noexcept {
	if (_unit.expired()) {
		_unit = unit;
	} else {
		if (_nextUnit) {
			_nextUnit->append(unit);
		} else {
			_nextUnit = std::make_unique<awe::unit_linked_list>(unit);
		}
	}
}

void awe::unit_linked_list::remove(std::weak_ptr<awe::unit> remove) noexcept {
	if (remove.expired()) return;
	if (_unit.lock())
}

void awe::unit_linked_list::clear() noexcept {
	if (_nextUnit) {
		_nextUnit->clear();
		_nextUnit.reset(nullptr);
	}
}

sf::Uint64 awe::unit::_id_counter = 0;

awe::unit::unit(const unit_type* type, const int hp, const int fuel, const int ammo) noexcept : _unitType(type) {
	setHP(hp);
	setFuel(fuel);
	setAmmo(ammo);
	// generate unit's ID
	_id = _id_counter++;
	if (_id_counter == UINT64_MAX) _id_counter = 0;
}

sf::Uint64 awe::unit::getID() const noexcept {
	return _id;
}

const awe::unit_type* awe::unit::setType(const awe::unit_type* newType) noexcept {
	auto old = getType();
	_unitType = newType;
	setHP(getHP());
	setFuel(getFuel());
	setAmmo(getAmmo());
	_loadedUnits.clear();
	return old;
}

const awe::unit_type* awe::unit::getType() const noexcept {
	return _unitType;
}

int awe::unit::setHP(const int newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	if (_hp < 0) {
		_hp = 0;
	} else if (_hp > (int)_unitType->getMaxHP()) {
		_hp = (int)_unitType->getMaxHP();
	}
	return old;
}

int awe::unit::getHP() const noexcept {
	return _hp;
}

int awe::unit::setFuel(const int newFuel) noexcept {
	auto old = getFuel();
	_fuel = newFuel;
	if (_fuel < 0) {
		_fuel = 0;
	} else if (!_unitType->isInfiniteFuel() && _fuel > _unitType->getMaxFuel()) {
		_fuel = (int)_unitType->getMaxFuel();
	}
	return old;
}

int awe::unit::getFuel() const noexcept {
	return _fuel;
}

int awe::unit::setAmmo(const int newAmmo) noexcept {
	auto old = getAmmo();
	_ammo = newAmmo;
	if (_ammo < 0) {
		_ammo = 0;
	} else if (!_unitType->isInfiniteAmmo() && _ammo > _unitType->getMaxAmmo()) {
		_ammo = _unitType->getMaxAmmo();
	}
	return old;
}

int awe::unit::getAmmo() const noexcept {
	return _ammo;
}

bool awe::unit::operator==(const awe::unit& rhs) const noexcept {
	return getID() == rhs.getID();
}

bool awe::unit::operator!=(const awe::unit& rhs) const noexcept {
	return !(*this == rhs);
}