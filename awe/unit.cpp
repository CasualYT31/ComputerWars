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
	const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::shared_ptr<sfx::animated_spritesheet>& icons) noexcept :
	_type(type), _army(army),
	_sprite(sheet, ((type) ? (type->getUnit(army)) : (""))),
	_hpIcon(icons, "nohpicon"), _fuelAmmoIcon(icons, "nostatusicon"),
	_loadedIcon(icons, "nostatusicon") {}

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
	return (awe::HP)ceil((double)_hp / (double)awe::unit_type::HP_GRANULARITY);
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

void awe::unit::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_hpIcon.setSpritesheet(sheet);
	_fuelAmmoIcon.setSpritesheet(sheet);
	_loadedIcon.setSpritesheet(sheet);
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
	// determine which icons to set
	if (getDisplayedHP() == 0 || getDisplayedHP() > 9) {
		_hpIcon.setSprite("nohpicon");
	} else {
		_hpIcon.setSprite(std::to_string(getDisplayedHP()));
	}
	if (_loaded.size()) {
		_loadedIcon.setSprite("loaded");
	} else {
		_loadedIcon.setSprite("nostatusicon");
	}
	const bool lowFuel =
		(!_type->hasInfiniteFuel() && getFuel() <= _type->getMaxFuel() / 2);
	const bool lowAmmo =
		(!_type->hasInfiniteAmmo() && _type->getMaxAmmo() &&
			getAmmo() <= _type->getMaxAmmo() / 2);
	if (lowFuel && lowAmmo) {
		_fuelAmmoIcon.setSprite("fuelammolow");
	} else if (lowFuel) {
		_fuelAmmoIcon.setSprite("fuellow");
	} else if (lowAmmo) {
		_fuelAmmoIcon.setSprite("ammolow");
	} else {
		_fuelAmmoIcon.setSprite("nostatusicon");
	}
	// animate sprites
	_hpIcon.animate(target);
	_fuelAmmoIcon.animate(target);
	_loadedIcon.animate(target);
	bool ret = _sprite.animate(target);
	// calculate icon positions
	sf::Vector2f pos = _sprite.getPosition();
	sf::Vector2f size = _sprite.getSize();
	_hpIcon.setPosition(sf::Vector2f(pos.x + size.x - _hpIcon.getSize().x,
		pos.y + size.y - _hpIcon.getSize().y));
	_fuelAmmoIcon.setPosition(sf::Vector2f(pos.x, pos.y + size.y -
		_fuelAmmoIcon.getSize().y));
	_loadedIcon.setPosition(pos);
	// return main unit graphic animation result
	return ret;
}

void awe::unit::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
	target.draw(_loadedIcon, states);
	target.draw(_fuelAmmoIcon, states);
	target.draw(_hpIcon, states);
}