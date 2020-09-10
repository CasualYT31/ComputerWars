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

#include "army.h"

awe::unit::unit(const std::shared_ptr<const unit_type>& type, const sf::Int32 hp, const sf::Int32 fuel, const sf::Int32 ammo, const sf::Uint64 initForUUID) noexcept : UUID(initForUUID), _unitType(type) {
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

sf::Int32 awe::unit::setHP(const sf::Int32 newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	if (_unitType) {
		if (_hp < 0 || _hp >(int)_unitType->getMaxHP()) {
			_hp = (int)_unitType->getMaxHP();
		}
	}
	return old;
}

sf::Int32 awe::unit::getHP() const noexcept {
	return _hp;
}

sf::Int32 awe::unit::setFuel(const sf::Int32 newFuel) noexcept {
	auto old = getFuel();
	_fuel = newFuel;
	if (_unitType) {
		if (_fuel < 0 || (!_unitType->hasInfiniteFuel() && _fuel > _unitType->getMaxFuel())) {
			_fuel = (int)_unitType->getMaxFuel();
		}
	}
	return old;
}

sf::Int32 awe::unit::getFuel() const noexcept {
	return _fuel;
}

sf::Int32 awe::unit::setAmmo(const sf::Int32 newAmmo) noexcept {
	auto old = getAmmo();
	_ammo = newAmmo;
	if (_unitType) {
		if (_ammo < 0 || (!_unitType->hasInfiniteAmmo() && _ammo > _unitType->getMaxAmmo())) {
			_ammo = _unitType->getMaxAmmo();
		}
	}
	return old;
}

sf::Int32 awe::unit::getAmmo() const noexcept {
	return _ammo;
}

bool awe::unit::loadUnit(const std::shared_ptr<awe::unit>& unit) noexcept {
	if (unit && _unitType && _unitType->canLoad(*unit->getType())) {
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

void awe::unit::setOwner(awe::army* ptr) noexcept {
	_owner = ptr;
}

awe::army* awe::unit::getOwner() const noexcept {
	return _owner;
}

bool awe::unit::operator==(const awe::unit& rhs) const noexcept {
	return UUID == rhs.UUID;
}

bool awe::unit::operator!=(const awe::unit& rhs) const noexcept {
	return !(*this == rhs);
}

void awe::unit::setSpritesheets(const std::shared_ptr<awe::spritesheets::units>& ptr) noexcept {
	_spritesheets = ptr;
}

bool awe::unit::animate(const sf::RenderTarget& target) noexcept {
	// update sprite's sheets
	if (_spritesheets && _spritesheets->idle) {
		_sprite.setSpritesheet(_spritesheets->idle);
	} else {
		_sprite.setSpritesheet(nullptr);
	}
	// update sprite's sprite
	if (_unitType && _owner && _owner->getCountry()) {
		_sprite.setSprite(_unitType->getUnit(_owner->getCountry()->UUID.getID()));
	}
	// animate
	return _sprite.animate(target);
}

void awe::unit::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
}