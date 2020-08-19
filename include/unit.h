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

/**
 * @file unit.h
 * This header file declares the class which represents a single unit on the map.
 */

#pragma once

#include "bank.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class representing a single unit.
	 */
	class unit {
	public:
		/**
		 * Constructor which initialises a unit.
		 * @param type Pointer to the unit's static information defining its type.
		 * @param hp   The initial HP of the unit. Is set using \c setHP().
		 * @param fuel The initial fuel of the unit. Is set using \c setFuel().
		 * @param ammo The initial ammo of the unit. Is set using \c setAmmo().
		 */
		unit(const unit_type* type = nullptr, const int hp = 0, const int fuel = 0, const int ammo = 0) noexcept;

		/**
		 * Updates the unit's type.
		 * When updating the unit's type, its HP, fuel, and ammo will be reset to the type's max stats.
		 * @param  newType Pointer to the static information of the unit's new type.
		 * @return Pointer to the information on the unit's old type.
		 */
		const unit_type* setType(const unit_type* newType) noexcept;

		/**
		 * Retrieves information on the unit's type.
		 * @return Pointer to the static information on the unit.
		 */
		const unit_type* getType() const noexcept;

		/**
		 * Updates the unit's HP.
		 * If an HP value is given outside of the range (0 to unit type's maximum health),
		 * it will automatically be adjusted (0 if <0, max HP if >max HP).
		 * @param  newHP The new HP.
		 * @return The old HP.
		 */
		int setHP(const int newHP) noexcept;

		/**
		 * Retrieves the unit's current HP.
		 * @return The current HP.
		 */
		int getHP() const noexcept;

		/**
		 * Updates the unit's fuel.
		 * If a fuel value is given outside of the range (0 to unit type's maximum fuel),
		 * it will automatically be adjusted (0 if <0, max fuel if >max fuel).
		 * @param  newFuel The new fuel.
		 * @return The old fuel.
		 */
		int setFuel(const int newFuel) noexcept;

		/**
		 * Retrieves the unit's current fuel.
		 * @return The current fuel.
		 */
		int getFuel() const noexcept;

		/**
		 * Updates the unit's ammo.
		 * If an ammo value is given outside of the range (0 to unit type's maximum ammo),
		 * it will automatically be adjusted (0 if <0, max ammo if >max ammo).
		 * @param  newAmmo The new ammo.
		 * @return The old ammo.
		 */
		int setAmmo(const int newAmmo) noexcept;

		/**
		 * Retrieves the unit's current primary ammo.
		 * @return The current ammo.
		 */
		int getAmmo() const noexcept;
	private:
		/**
		 * Pointer to the unit's type information.
		 */
		const unit_type* _unitType = nullptr;

		/**
		 * The HP of the unit.
		 */
		int _hp = 0;

		/**
		 * The fuel of the unit.
		 */
		int _fuel = 0;

		/**
		 * The ammo of the unit.
		 */
		int _ammo = 0;
	};
}