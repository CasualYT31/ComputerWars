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
#include <limits>

const sf::Vector2u awe::unit::NO_POSITION(std::numeric_limits<unsigned int>::max(),
	std::numeric_limits<unsigned int>::max());

sf::Vector2u awe::unit::NO_POSITION_SCRIPT = awe::unit::NO_POSITION;

awe::unit::unit(const view_callbacks& callbacks,
	const std::shared_ptr<const awe::banks>& banks,
	const std::shared_ptr<awe::animated_unit>& animatedUnit,
	const std::function<void(const std::function<void(void)>&)>& spriteCallback,
	const std::string& type, const engine::logger::data& data,
	const awe::ArmyID army,
	const std::shared_ptr<sfx::animated_spritesheet>& icons) :
	_viewCallbacks(callbacks), _type(type), _army(army),
	_unitSprite(animatedUnit), _updateSprite(spriteCallback),
	_loggerData(data), _banks(banks) {
	// Initialise all ammos.
	const auto unitType = getType();
	for (std::size_t i = 0, end = unitType->weapons().vector.size(); i < end; ++i) {
		const auto weapon = getWeapon(i);
		_ammos[weapon->scriptName] = 0;
	}
	// Initialise the sprite.
	animatedUnit->setIconSpritesheet(icons);
	animatedUnit->setSprite(unitType->spriteInfo().sprite);
	animatedUnit->setHPIconSprite("nohpicon");
	animatedUnit->setFuelAmmoIconSprite("nostatusicon");
	animatedUnit->setLoadedIconSprite("nostatusicon");
	animatedUnit->setCapturingHidingIconSprite("nostatusicon");
}

engine::CScriptWrapper<awe::unit_type_view> awe::unit::getType() const {
	const auto unitType = awe::unit_type_view::Create(_loggerData, _banks, _type);
	unitType->overrides
		.commander(_viewCallbacks.commander(_army))
		.country(_viewCallbacks.country(_army));
	if (_location != NO_POSITION) {
		unitType->overrides
			.structure(_viewCallbacks.structure(_location))
			.tileType(_viewCallbacks.tileType(_location))
			.terrain(_viewCallbacks.terrain(_location));
	}
	return unitType;
}

engine::CScriptWrapper<awe::movement_type_view> awe::unit::getMovementType() const {
	const auto unitType = getType();
	const auto movementType = awe::movement_type_view::Create(_loggerData, _banks,
		unitType->movementType());
	movementType->overrides
		.commander(unitType->overrides.commander())
		.country(unitType->overrides.country());
	return movementType;
}

engine::CScriptWrapper<awe::weapon_view> awe::unit::getWeapon(const std::size_t i) const {
	const auto unitType = getType();
	const auto& weapons = unitType->weapons().vector;
	const auto weaponName = i >= weapons.size() ? "" : weapons[i];
	const auto weapon = awe::weapon_view::Create(_loggerData, _banks, weaponName);
	weapon->overrides
		.commander(_viewCallbacks.commander(_army))
		.country(_viewCallbacks.country(_army))
		.unitType(_type);
	if (_location != NO_POSITION) {
		weapon->overrides
			.structure(_viewCallbacks.structure(_location))
			.tileType(_viewCallbacks.tileType(_location))
			.terrain(_viewCallbacks.terrain(_location));
	}
	return weapon;
}

bool awe::unit::isReplenished(const bool heal) const {
	const auto unitType = getType();
	// First check fuel.
	if (!unitType->hasInfiniteFuel() && getFuel() != unitType->maxFuel())
		return false;
	// Then check ammos.
	for (std::size_t i = 0, end = unitType->weapons().vector.size(); i < end; ++i) {
		const auto weapon = getWeapon(i);
		if (!weapon->hasInfiniteAmmo() && getAmmo(weapon->scriptName) != weapon->ammo())
			return false;
	}
	// Finally, check HP, if configured to do so.
	if (!heal) return true;
	return static_cast<sf::Uint32>(getHP()) == unitType->maxHP();
}

// TODO: If an override introduces a new weapon, or gets rid of a weapon, how are
// we going to track that? Either we forbid overridding the weapons a unit possesses (likely option),
// or we continuously keep track of it somehow.

void awe::unit::replenish(const bool heal) {
	const auto unitType = getType();
	if (heal) setHP(unitType->maxHP());
	if (!unitType->hasInfiniteFuel()) setFuel(unitType->maxFuel());
	for (std::size_t i = 0, end = unitType->weapons().vector.size(); i < end; ++i) {
		const auto weapon = getWeapon(i);
		if (!weapon->hasInfiniteAmmo()) setAmmo(weapon->scriptName, weapon->ammo());
	}
}

bool awe::unit::_isLowOnAmmo() const {
	// TODO-1: determine when we should show the low ammo icon. What we have now
	// might be fine.
	const auto unitType = getType();
	for (std::size_t i = 0, end = unitType->weapons().vector.size(); i < end; ++i) {
		const auto weapon = getWeapon(i);
		if (weapon->hasInfiniteAmmo()) continue;
		return getAmmo(weapon->scriptName) <= weapon->ammo() / 2;
	}
	// All weapons have infinite ammo.
	return false;
}

void awe::unit::_updateHPIcon() {
	_updateSprite(std::bind([](
		const std::weak_ptr<awe::animated_unit>& _unitSprite,
		const awe::HP displayedHP) {
			if (_unitSprite.expired()) return;
			const auto unitSprite = _unitSprite.lock();
			if (displayedHP == 0 || displayedHP > 9) {
				unitSprite->setHPIconSprite("nohpicon");
			} else {
				unitSprite->setHPIconSprite(std::to_string(displayedHP));
			}
		}, _unitSprite, getDisplayedHP())
	);
}

void awe::unit::_updateFuelAmmoIcon() {
	_updateSprite(std::bind([](
		const std::weak_ptr<awe::animated_unit>& _unitSprite,
		engine::CScriptWrapper<awe::unit_type_view> unitType,
		const awe::Fuel fuel, const bool lowAmmo) {
			if (_unitSprite.expired()) return;
			const auto unitSprite = _unitSprite.lock();
			const bool lowFuel =
				(!unitType->hasInfiniteFuel() && fuel <= unitType->maxFuel() / 2);
			if (lowFuel && lowAmmo) {
				unitSprite->setFuelAmmoIconSprite("fuelammolow");
			} else if (lowFuel) {
				unitSprite->setFuelAmmoIconSprite("fuellow");
			} else if (lowAmmo) {
				unitSprite->setFuelAmmoIconSprite("ammolow");
			} else {
				unitSprite->setFuelAmmoIconSprite("nostatusicon");
			}
		}, _unitSprite, getType(), getFuel(), _isLowOnAmmo())
	);
}

void awe::unit::_updateLoadedIcon() {
	_updateSprite(std::bind([](
		const std::weak_ptr<awe::animated_unit>& _unitSprite, const bool empty) {
			if (_unitSprite.expired()) return;
			const auto unitSprite = _unitSprite.lock();
			if (empty) {
				unitSprite->setLoadedIconSprite("nostatusicon");
			} else {
				unitSprite->setLoadedIconSprite("loaded");
			}
	}, _unitSprite, _loaded.empty()));
}

void awe::unit::_updateCapturingHidingIcon() {
	_updateSprite(std::bind([](
		const std::weak_ptr<awe::animated_unit>& _unitSprite,
		const bool isCapturing, const bool isHiding) {
			if (_unitSprite.expired()) return;
			const auto unitSprite = _unitSprite.lock();
			if (isCapturing && isHiding) {
				unitSprite->setCapturingHidingIconSprite("capturinghiding");
			} else if (isCapturing) {
				unitSprite->setCapturingHidingIconSprite("capturing");
			} else if (isHiding) {
				unitSprite->setCapturingHidingIconSprite("hiding");
			} else {
				unitSprite->setCapturingHidingIconSprite("nostatusicon");
			}
		}, _unitSprite, isCapturing(), isHiding())
	);
}
