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

/**@file unit.h
 * Declares the class which represents an in-game unit.
 */

#include "bank.h"
#include <set>

#pragma once

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class which represents a single unit on a map.
	 */
	class unit {
	public:
		/**
		 * Creates a new unit.
		 * @param type The type of the unit, which can't be changed.
		 * @param army The army the unit belongs to, which can't be changed.
		 */
		unit(const std::shared_ptr<const awe::unit_type>& type, const awe::UUIDValue army) noexcept;

		/**
		 * Gets the unit's type.
		 * @return The information on the unit's type.
		 */
		std::shared_ptr<const awe::unit_type> getType() const noexcept;

		/**
		 * Gets the unit's army affiliation.
		 * @return The ID of the army which owns this unit.
		 */
		awe::UUIDValue getArmy() const noexcept;

		/**
		 * Sets this unit's X and Y location.
		 * @param pos The X and Y location of the tile this unit occupies.
		 */
		void setPosition(const sf::Vector2u pos) noexcept;

		/**
		 * Gets this unit's location.
		 * @return The X and Y location of the tile this unit occupies.
		 */
		sf::Vector2u getPosition() const noexcept;

		/**
		 * Used to determine if this unit is on a tile in the map.
		 * Useful for loaded units, which are in the game but not on the map.
		 * @return \c TRUE if the unit occupies a tile on the map, \c FALSE if not.
		 */
		bool isOnMap() const noexcept;

		/**
		 * Sets this unit's HP.
		 * If < 0 is given, 0 will be stored.
		 * @param hp The new HP of this unit.
		 */
		void setHP(const awe::HP hp) noexcept;

		/**
		 * Gets this unit's HP.
		 * @return The current HP of this unit.
		 */
		awe::HP getHP() const noexcept;

		/**
		 * Sets this unit's fuel.
		 * If < 0 is given, 0 will be stored.
		 * @param fuel The new fuel of this unit.
		 */
		void setFuel(const awe::Fuel fuel) noexcept;

		/**
		 * Gets this unit's fuel.
		 * @return The current fuel of this unit.
		 */
		awe::Fuel getFuel() const noexcept;

		/**
		 * Sets this unit's ammo.
		 * If < 0 is given, 0 will be stored.
		 * @param ammo The new ammo of this unit.
		 */
		void setAmmo(const awe::Ammo ammo) noexcept;

		/**
		 * Gets this unit's ammo.
		 * @return The current ammo of this unit.
		 */
		awe::Ammo getAmmo() const noexcept;

		/**
		 * Loads another unit onto this one.
		 * @param id The ID of the unit to load onto this one.
		 */
		void loadUnit(const awe::UnitID id) noexcept;

		/**
		 * Unloads a unit from this one, if it exists.
		 * @param  id The ID of the unit to unload.
		 * @return \c TRUE if the unit was unloaded successfully, \c FALSE if the unit wasn't loaded.
		 */
		bool unloadUnit(const awe::UnitID id) noexcept;

		/**
		 * Loads this unit onto another.
		 * Provide \c 0 to indicate that this unit is not to be loaded onto another unit.
		 * @param id The ID of the unit this one is loaded onto.
		 */
		void loadOnto(const awe::UnitID id) noexcept;

		/**
		 * Identifies which unit this unit is loaded onto.
		 * @return The ID of the unit this unit is loaded onto. 0 if none.
		 */
		awe::UnitID loadedOnto() const noexcept;
	private:
		/**
		 * The type of the unit.
		 */
		const std::shared_ptr<const awe::unit_type> _type;

		/**
		 * The ID of the army the unit belongs to.
		 */
		const awe::UUIDValue _army = engine::uuid<awe::country>::INVALID;

		/**
		 * The tile this unit occupies.
		 */
		sf::Vector2u _location;

		/**
		 * The HP of this unit.
		 */
		awe::HP _hp = 0;

		/**
		 * The fuel of this unit.
		 */
		awe::Fuel _fuel = 0;

		/**
		 * The ammo of this unit.
		 */
		awe::Ammo _ammo = 0;

		/**
		 * The list of units loaded onto this one.
		 */
		std::set<awe::UnitID> _loaded;

		/**
		 * The unit this unit is loaded onto.
		 */
		awe::UnitID _loadedOnto = 0;
	};
}