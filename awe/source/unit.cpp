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

awe::unit::unit(const std::shared_ptr<awe::animated_unit>& animatedUnit,
	const std::function<void(const std::function<void(void)>&)>& spriteCallback,
	const std::shared_ptr<const awe::unit_type>& type, const awe::ArmyID army,
	const std::shared_ptr<sfx::animated_spritesheet>& icons) :
	_type(type), _army(army), _unitSprite(animatedUnit),
	_updateSprite(spriteCallback) {
	// Initialise all ammos.
	for (std::size_t i = 0, weaponCount = type->getWeaponCount();
		i < weaponCount; ++i) {
		const auto weapon = type->getWeaponByIndex(i);
		_ammos[weapon->getScriptName()] = 0;
	}
	// Initialise the sprite.
	animatedUnit->setIconSpritesheet(icons);
	animatedUnit->setSprite((type) ? (type->getUnit(army)) : (""));
	animatedUnit->setHPIconSprite("nohpicon");
	animatedUnit->setFuelAmmoIconSprite("nostatusicon");
	animatedUnit->setLoadedIconSprite("nostatusicon");
	animatedUnit->setCapturingHidingIconSprite("nostatusicon");
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

bool awe::unit::_isLowOnAmmo() const {
	// TODO-1: determine when we should show the low ammo icon. What we have now
	// might be fine.
	const auto weapon = _type->getFirstWeaponWithFiniteAmmo();
	if (!weapon) return false;
	return getAmmo(weapon->getScriptName()) <= weapon->getMaxAmmo() / 2;
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
		const std::shared_ptr<const awe::unit_type>& _type,
		const awe::Fuel fuel, const bool lowAmmo) {
			if (_unitSprite.expired()) return;
			const auto unitSprite = _unitSprite.lock();
			const bool lowFuel =
				(!_type->hasInfiniteFuel() && fuel <= _type->getMaxFuel() / 2);
			if (lowFuel && lowAmmo) {
				unitSprite->setFuelAmmoIconSprite("fuelammolow");
			} else if (lowFuel) {
				unitSprite->setFuelAmmoIconSprite("fuellow");
			} else if (lowAmmo) {
				unitSprite->setFuelAmmoIconSprite("ammolow");
			} else {
				unitSprite->setFuelAmmoIconSprite("nostatusicon");
			}
		}, _unitSprite, _type, getFuel(), _isLowOnAmmo())
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
