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

awe::unit::unit(const std::shared_ptr<const awe::unit_type>& type, const awe::UUIDValue army) noexcept : _type(type), _army(army) {}

std::shared_ptr<const awe::unit_type> awe::unit::getType() const noexcept {
	return _type;
}

awe::UUIDValue awe::unit::getArmy() const noexcept {
	return _army;
}

void awe::unit::setPosition(const sf::Vector2u pos) noexcept {
	_location = pos;
}

sf::Vector2u awe::unit::getPosition() const noexcept {
	return _location;
}

bool awe::unit::isOnMap() const noexcept {
	return loadedOnto() != 0;
}

void awe::unit::setHP(const awe::HP hp) noexcept {
	if (hp < 0) {
		_hp = 0;
	} else {
		_hp = hp;
	}
}

awe::HP awe::unit::getHP() const noexcept {
	return _hp;
}

void awe::unit::setFuel(const awe::Fuel fuel) noexcept {
	if (fuel < 0) {
		_fuel = 0;
	} else {
		_fuel = fuel;
	}
}

awe::Fuel awe::unit::getFuel() const noexcept {
	return _fuel;
}

void awe::unit::setAmmo(const awe::Ammo ammo) noexcept {
	if (ammo < 0) {
		_ammo = 0;
	} else {
		_ammo = ammo;
	}
}

awe::Ammo awe::unit::getAmmo() const noexcept {
	return _ammo;
}

void awe::unit::loadUnit(const awe::UnitID id) noexcept {
	_loaded.insert(id);
}

bool awe::unit::unloadUnit(const awe::UnitID id) noexcept {
	return _loaded.erase(id);
}

void awe::unit::loadOnto(const awe::UnitID id) noexcept {
	_loadedOnto = id;
}

awe::UnitID awe::unit::loadedOnto() const noexcept {
	return _loadedOnto;
}