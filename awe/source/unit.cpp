/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "unit.hpp"
#include <cmath>

const sf::Vector2u awe::unit::NO_POSITION(std::numeric_limits<unsigned int>::max(),
	std::numeric_limits<unsigned int>::max());

sf::Vector2u awe::unit::NO_POSITION_SCRIPT = awe::unit::NO_POSITION;

awe::unit::unit(const engine::logger::data& data,
	const std::shared_ptr<const awe::unit_type>& type, const awe::ArmyID army,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::shared_ptr<sfx::animated_spritesheet>& icons) : _type(type),
	_army(army),
	_sprite(sheet, ((type) ? (type->getUnit(army)) : ("")),
		{data.sink, data.name + "_animated_sprite"}),
	_hpIcon(icons, "nohpicon", { data.sink, data.name + "_hp_icon" }),
	_fuelAmmoIcon(icons, "nostatusicon",
		{ data.sink, data.name + "_fuel_ammo_icon" }),
	_loadedIcon(icons, "nostatusicon", { data.sink, data.name + "_loaded_icon" }),
	_capturingHidingIcon(icons, "nostatusicon",
		{ data.sink, data.name + "_status_icon" }) {
	// Initialise all ammos.
	for (std::size_t i = 0, weaponCount = type->getWeaponCount();
		i < weaponCount; ++i) {
		const auto weapon = type->getWeaponByIndex(i);
		_ammos[weapon->getScriptName()] = 0;
	}
}

bool awe::unit::isReplenished(const bool heal) const {
	// First check fuel.
	if (!_type->hasInfiniteFuel() && getFuel() != _type->getMaxFuel())
		return false;
	// Then check ammos.
	for (std::size_t i = 0, len = _type->getWeaponCount(); i < len; ++i) {
		const auto weaponType = _type->getWeaponByIndex(i);
		if (!weaponType->hasInfiniteAmmo() &&
			getAmmo(weaponType->getScriptName()) != weaponType->getMaxAmmo())
			return false;
	}
	// Finally, check HP, if configured to do so.
	if (!heal) return true;
	return static_cast<unsigned int>(getHP()) == _type->getMaxHP();
}

void awe::unit::replenish(const bool heal) {
	if (heal) setHP(_type->getMaxHP());
	if (!_type->hasInfiniteFuel()) setFuel(_type->getMaxFuel());
	for (std::size_t i = 0, len = _type->getWeaponCount(); i < len; ++i) {
		const auto weaponType = _type->getWeaponByIndex(i);
		if (!weaponType->hasInfiniteAmmo())
			setAmmo(weaponType->getScriptName(), weaponType->getMaxAmmo());
	}
}

void awe::unit::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_hpIcon.setSpritesheet(sheet);
	_fuelAmmoIcon.setSpritesheet(sheet);
	_loadedIcon.setSpritesheet(sheet);
	_capturingHidingIcon.setSpritesheet(sheet);
}

bool awe::unit::animate(const sf::RenderTarget& target) {
	// Determine which icons to set.
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
	if (isCapturing() && isHiding()) {
		_capturingHidingIcon.setSprite("capturinghiding");
	} else if (isCapturing()) {
		_capturingHidingIcon.setSprite("capturing");
	} else if (isHiding()) {
		_capturingHidingIcon.setSprite("hiding");
	} else {
		_capturingHidingIcon.setSprite("nostatusicon");
	}
	const bool lowFuel =
		(!_type->hasInfiniteFuel() && getFuel() <= _type->getMaxFuel() / 2);
	const bool lowAmmo = _isLowOnAmmo();
	if (lowFuel && lowAmmo) {
		_fuelAmmoIcon.setSprite("fuelammolow");
	} else if (lowFuel) {
		_fuelAmmoIcon.setSprite("fuellow");
	} else if (lowAmmo) {
		_fuelAmmoIcon.setSprite("ammolow");
	} else {
		_fuelAmmoIcon.setSprite("nostatusicon");
	}
	// Animate sprites.
	_hpIcon.animate(target);
	_fuelAmmoIcon.animate(target);
	_loadedIcon.animate(target);
	_capturingHidingIcon.animate(target);
	bool ret = _sprite.animate(target);
	// Calculate icon positions.
	sf::Vector2f pos = _sprite.getPosition();
	sf::Vector2f size = _sprite.getSize();
	_hpIcon.setPosition(sf::Vector2f(pos.x + size.x - _hpIcon.getSize().x,
		pos.y + size.y - _hpIcon.getSize().y));
	_fuelAmmoIcon.setPosition(sf::Vector2f(pos.x, pos.y + size.y -
		_fuelAmmoIcon.getSize().y));
	_loadedIcon.setPosition(pos);
	_capturingHidingIcon.setPosition(sf::Vector2f(
		pos.x + size.x - _capturingHidingIcon.getSize().x, pos.y));
	// Return main unit graphic animation result.
	return ret;
}

void awe::unit::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
	target.draw(_loadedIcon, states);
	target.draw(_capturingHidingIcon, states);
	target.draw(_fuelAmmoIcon, states);
	target.draw(_hpIcon, states);
}

bool awe::unit::_isLowOnAmmo() const {
	// TODO-1: determine when we should show the low ammo icon. What we have now
	// might be fine.
	const auto weapon = _type->getFirstWeaponWithFiniteAmmo();
	if (!weapon) return false;
	return getAmmo(weapon->getScriptName()) <= weapon->getMaxAmmo() / 2;
}
