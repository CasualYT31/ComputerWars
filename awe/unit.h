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

/**@file unit.h
 * Declares the class which represents an in-game unit.
 */

#include "bank.h"
#include "army.h"

#pragma once

namespace awe {
	/**
	 * Class which represents a single unit on a map.
	 * @warning Note that, since this class does not have a default constructor, if
	 *          it is used with a \c map container, the \c operator[]() method
	 *          cannot be used with that map. Use \c at() instead.
	 */
	class unit : public sfx::animated_drawable {
	public:
		/**
		 * Creates a new unit.
		 * @warning \c army \b must hold a valid country ID: checks must be carried
		 *          out outside of this class!
		 * @param   type  The type of the unit, which can't be changed.
		 * @param   army  The army the unit belongs to, which can't be changed.
		 * @param   sheet Pointer to the spritesheet to use with this unit.
		 */
		unit(const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army = 0,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr)
			noexcept;

		/**
		 * Gets the unit's type.
		 * @return The information on the unit's type.
		 */
		std::shared_ptr<const awe::unit_type> getType() const noexcept;

		/**
		 * Gets the unit's army affiliation.
		 * @return The ID of the army which owns this unit.
		 */
		awe::ArmyID getArmy() const noexcept;

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
		 * If <tt>< 0</tt> is given, \c 0 will be stored.
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
		 * If <tt>< 0</tt> is given, \c 0 will be stored.
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
		 * If <tt>< 0</tt> is given, \c 0 will be stored.
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
		 * @return \c TRUE if the unit was unloaded successfully, \c FALSE if the
		 *         unit wasn't loaded.
		 */
		bool unloadUnit(const awe::UnitID id) noexcept;

		/**
		 * Copies the internal list of loaded units and returns it.
		 * @return The IDs of the all the units that are loaded onto this one.
		 */
		std::unordered_set<awe::UnitID> loadedUnits() const noexcept;

		/**
		 * Loads this unit onto another.
		 * Provide \c 0 to indicate that this unit is not to be loaded onto another
		 * unit.
		 * @param id The ID of the unit this one is loaded onto.
		 */
		void loadOnto(const awe::UnitID id) noexcept;

		/**
		 * Identifies which unit this unit is loaded onto.
		 * @return The ID of the unit this unit is loaded onto. 0 if none.
		 */
		awe::UnitID loadedOnto() const noexcept;

		/**
		 * Sets the spritesheet to use with this unit.
		 * @param sheet Pointer to the spritesheet to use with this unit.
		 */
		void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept;

		/**
		 * Sets the unit's pixel position to the internal sprite.
		 * @param x The X position of the tile.
		 * @param y The Y position of the tile.
		 */
		void setPixelPosition(float x, float y) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * Simply calls the internal sprite's \c animate() method.
		 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
		 *         call.
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Simply draws \c _sprite to the screen.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * The type of the unit.
		 */
		std::shared_ptr<const awe::unit_type> _type;

		/**
		 * The ID of the army the unit belongs to.
		 */
		awe::ArmyID _army = awe::army::NO_ARMY;

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
		std::unordered_set<awe::UnitID> _loaded;

		/**
		 * The unit this unit is loaded onto.
		 */
		awe::UnitID _loadedOnto = 0;

		/**
		 * The unit's animated sprite object.
		 */
		sfx::animated_sprite _sprite;
	};
}