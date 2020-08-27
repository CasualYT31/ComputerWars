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

awe::unit::unit(const std::shared_ptr<const unit_type>& type, const int hp, const int fuel, const int ammo, const sf::Uint64 initForUUID) noexcept : UUID(initForUUID), _unitType(type) {
	setHP(hp);
	setFuel(fuel);
	setAmmo(ammo);
}

void awe::unit::setType(const std::shared_ptr<const unit_type>& newType) noexcept {
	_unitType = newType;
	setHP(getHP());
	setFuel(getFuel());
	setAmmo(getAmmo());
	_loadedUnits.clear();
}

const std::shared_ptr<const awe::unit_type> awe::unit::getType() const noexcept {
	return _unitType;
}

int awe::unit::setHP(const int newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	if (_hp < 0 || _hp > (int)_unitType->getMaxHP()) {
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
	if (_fuel < 0 || (!_unitType->hasInfiniteFuel() && _fuel > _unitType->getMaxFuel())) {
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
	if (_ammo < 0 || (!_unitType->hasInfiniteAmmo() && _ammo > _unitType->getMaxAmmo())) {
		_ammo = _unitType->getMaxAmmo();
	}
	return old;
}

int awe::unit::getAmmo() const noexcept {
	return _ammo;
}

bool awe::unit::loadUnit(const std::shared_ptr<awe::unit>& unit) noexcept {
	if (unit && _unitType->canLoad(*unit->getType())) {
		_loadedUnits.push_back(unit);
		return true;
	}
	return false;
}

bool awe::unit::unloadUnit(const std::shared_ptr<awe::unit>& unitToUnload) noexcept {
	if (!unitToUnload) return false;
	for (auto itr = _loadedUnits.begin(), enditr = _loadedUnits.end(); itr != enditr; itr++) {
		if (auto spLoadedUnit = itr->lock()) {
			if (*unitToUnload == *spLoadedUnit) {
				_loadedUnits.erase(itr);
				return true;
			}
		}
	}
	return false;
}

std::vector<std::weak_ptr<awe::unit>> awe::unit::loadedUnits() const noexcept {
	return _loadedUnits;
}

bool awe::unit::operator==(const awe::unit& rhs) const noexcept {
	return UUID == rhs.UUID;
}

bool awe::unit::operator!=(const awe::unit& rhs) const noexcept {
	return !(*this == rhs);
}