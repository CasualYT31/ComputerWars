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
		 * @param type        Pointer to the unit's static information defining its type.
		 * @param hp          The initial HP of the unit. Is set using \c setHP().
		 * @param fuel        The initial fuel of the unit. Is set using \c setFuel().
		 * @param ammo        The initial ammo of the unit. Is set using \c setAmmo().
		 * @param initForUUID The initial value of this object type's \c uuid::_id_counter.
		 * @sa    engine::uuid::uuid()
		 */
		unit(const std::shared_ptr<const unit_type>& type = nullptr, const sf::Int32 hp = -1, const sf::Int32 fuel = -1, const sf::Int32 ammo = -1, const sf::Uint64 initForUUID = 1) noexcept;

		/**
		 * This unit's UUID object.
		 */
		engine::uuid<awe::unit> UUID;

		/**
		 * Updates the unit's type.
		 * When updating the unit's type, its HP, fuel, and ammo will be reset to the type's max stats.
		 * It will also unload all units.
		 * @param newType Pointer to the static information of the unit's new type.
		 */
		void setType(const std::shared_ptr<const awe::unit_type>& newType) noexcept;

		/**
		 * Retrieves information on the unit's type.
		 * @return Pointer to the static information on the unit.
		 */
		const std::shared_ptr<const unit_type> getType() const noexcept;

		/**
		 * Updates the unit's HP.
		 * If an HP value is given outside of the range (0 to unit type's maximum health),
		 * it will automatically be adjusted to the max health.
		 * @param  newHP The new HP.
		 * @return The old HP.
		 */
		sf::Int32 setHP(const sf::Int32 newHP) noexcept;

		/**
		 * Retrieves the unit's current HP.
		 * @return The current HP.
		 */
		sf::Int32 getHP() const noexcept;

		/**
		 * Updates the unit's fuel.
		 * If a fuel value is given outside of the range (0 to unit type's maximum fuel),
		 * it will automatically be adjusted to the max fuel.
		 * @param  newFuel The new fuel.
		 * @return The old fuel.
		 */
		sf::Int32 setFuel(const sf::Int32 newFuel) noexcept;

		/**
		 * Retrieves the unit's current fuel.
		 * @return The current fuel.
		 */
		sf::Int32 getFuel() const noexcept;

		/**
		 * Updates the unit's ammo.
		 * If an ammo value is given outside of the range (0 to unit type's maximum ammo),
		 * it will automatically be adjusted to the max ammo.
		 * @param  newAmmo The new ammo.
		 * @return The old ammo.
		 */
		sf::Int32 setAmmo(const sf::Int32 newAmmo) noexcept;

		/**
		 * Retrieves the unit's current primary ammo.
		 * @return The current ammo.
		 */
		sf::Int32 getAmmo() const noexcept;

		/**
		 * Loads a unit onto this one if the unit's type allows for it.
		 * @param  unit Reference to the unit to load.
		 * @return \c TRUE if the unit was loaded successfully, \c FALSE if not.
		 */
		bool loadUnit(const std::shared_ptr<awe::unit>& unit) noexcept;

		/**
		 * Unloads a unit from this one if it exists.
		 * @param  unitToUnload Reference to the unit to unload. If an empty pointer was given, \c FALSE will be returned.
		 * @return \c TRUE if the unit was unloaded successfully, \c FALSE if not.
		 */
		bool unloadUnit(const std::shared_ptr<awe::unit>& unitToUnload) noexcept;

		/**
		 * Copies the internal list of references to each loaded unit and returns it.
		 * Care should be taken to ensure all weak references are valid before accessing them.
		 * @return A list of references to units that are loaded onto this one.
		 */
		std::vector<std::weak_ptr<awe::unit>> loadedUnits() const noexcept;

		/**
		 * Tests if a given \c awe::unit object is equivalent to this one.
		 * If the two units given have the same internal ID, they are equivalent.
		 * If they are different, they are two different units.
		 * @param  rhs Right-hand side argument. The \c awe::unit object to test against.
		 * @return \c TRUE if both objects are equivalent, \c FALSE if not.
		 * @sa     operator!=()
		 */
		bool operator==(const awe::unit& rhs) const noexcept;

		/**
		 * Tests if a given \c awe::unit object is not equivalent to this one.
		 * If the two units given have the same internal ID, they are equivalent.
		 * If they are different, they are two different units.
		 * @param  rhs Right-hand side argument. The \c awe::unit object to test against.
		 * @return \c TRUE if both objects are not equivalent, \c FALSE if they are.
		 * @sa     operator==()
		 */
		bool operator!=(const awe::unit& rhs) const noexcept;
	private:
		/**
		 * Pointer to the unit's type information.
		 */
		std::shared_ptr<const unit_type> _unitType = nullptr;

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

		/**
		 * A list of weak references to units which are loaded onto this one.
		 */
		std::vector<std::weak_ptr<awe::unit>> _loadedUnits;
	};
}