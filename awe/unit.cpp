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

#include "unit.h"
#include <cmath>

awe::unit::unit(const std::shared_ptr<const awe::unit_type>& type,
	const awe::ArmyID army,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept :
	_type(type), _army(army),
	_sprite(sheet, ((type) ? (type->getUnit(army)) : (""))) {}

std::shared_ptr<const awe::unit_type> awe::unit::getType() const noexcept {
	return _type;
}

awe::ArmyID awe::unit::getArmy() const noexcept {
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

awe::HP awe::unit::getDisplayedHP() const noexcept {
	awe::HP ret = (awe::HP)floor((double)_hp /
		(double)awe::unit_type::HP_GRANULARITY);
	if (_hp < _type->getMaxHP()) ret += 1;
	return ret;
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

std::unordered_set<awe::UnitID> awe::unit::loadedUnits() const noexcept {
	return _loaded;
}

void awe::unit::loadOnto(const awe::UnitID id) noexcept {
	_loadedOnto = id;
}

awe::UnitID awe::unit::loadedOnto() const noexcept {
	return _loadedOnto;
}

void awe::unit::setSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_sprite.setSpritesheet(sheet);
}

std::shared_ptr<const sfx::animated_spritesheet> awe::unit::getSpritesheet() const
	noexcept {
	return _sprite.getSpritesheet();
}

std::string awe::unit::getSprite() const noexcept {
	return _sprite.getSprite();
}

void awe::unit::setPixelPosition(float x, float y) noexcept {
	_sprite.setPosition(sf::Vector2f(x, y));
}

bool awe::unit::animate(const sf::RenderTarget& target) noexcept {
	return _sprite.animate(target);
}

void awe::unit::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
}