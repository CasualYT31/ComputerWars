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

/**@file unit.hpp
 * Declares the class which represents an in-game unit.
 */

#include "bank.hpp"
#include "army.hpp"
#include "animated_unit.hpp"

#pragma once

namespace awe {
	/**
	 * Class which represents a single unit on a map.
	 * @warning Note that, since this class does not have a default constructor, if
	 *          it is used with a \c map container, the \c operator[]() method
	 *          cannot be used with that map. Use \c at() instead.
	 */
	class unit {
	public:
		/**
		 * Reserved value representing no position on the map.
		 * Assigned the maximum value of \c sf::Vector2u.
		 */
		static const sf::Vector2u NO_POSITION;

		/**
		 * \c NO_POSITION that can be assigned to a script's interface.
		 */
		static sf::Vector2u NO_POSITION_SCRIPT;

		/**
		 * Creates a new unit.
		 * @warning \c army \b must hold a valid turn order ID: checks must be
		 *          carried out outside of this class!
		 * @param   animatedUnit   Pointer to this unit's animated sprite.
		 * @param   spriteCallback When an update to the unit's sprite is required,
		 *                         this callback is to be invoked. The function
		 *                         that will perform operations on the animated
		 *                         unit must be given.
		 * @param   type           The type of the unit, which can't be changed.
		 * @param   army           The army the unit belongs to, which can't be
		 *                         changed.
		 * @param   sheet          Pointer to the spritesheet to use with this
		 *                         unit's sprite.
		 * @param   icons          Pointer to the icon spritesheet to use with this
		 *                         unit's sprite.
		 */
		unit(const std::shared_ptr<awe::animated_unit>& animatedUnit,
			const std::function<void(const std::function<void(void)>&)>&
				spriteCallback,
			const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army = 0,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr,
			const std::shared_ptr<sfx::animated_spritesheet>& icons = nullptr);

		/**
		 * Gets the unit's type.
		 * @return The information on the unit's type.
		 */
		inline std::shared_ptr<const awe::unit_type> getType() const {
			return _type;
		}

		/**
		 * Gets the unit's army affiliation.
		 * @return The ID of the army which owns this unit.
		 */
		inline awe::ArmyID getArmy() const noexcept {
			return _army;
		}

		/**
		 * Sets this unit's X and Y location.
		 * @param  pos The X and Y location of the tile this unit occupies.
		 * @safety No guarantee.
		 */
		inline void setPosition(const sf::Vector2u pos) {
			_location = pos;
		}

		/**
		 * Gets this unit's location.
		 * @return The X and Y location of the tile this unit occupies.
		 */
		inline sf::Vector2u getPosition() const {
			return _location;
		}

		/**
		 * Used to determine if this unit is on a tile in the map.
		 * Useful for loaded units, which are in the game but not on the map.
		 * @return \c TRUE if the unit occupies a tile on the map, \c FALSE if not.
		 * @safety No guarantee.
		 */
		inline bool isOnMap() const {
			return _location != awe::unit::NO_POSITION;
		}

		/**
		 * Sets this unit's HP.
		 * If <tt>< 0</tt> is given, \c 0 will be stored.\n
		 * Note that this method expects an internal HP value, and not one that
		 * the user would see (see \c getDisplayedHP()).
		 * @param hp The new HP of this unit.
		 */
		inline void setHP(const awe::HP hp) noexcept {
			_hp = ((hp < 0) ? (0) : (hp));
			_updateHPIcon();
		}

		/**
		 * Gets this unit's HP.
		 * @return The current HP of this unit.
		 */
		inline awe::HP getHP() const noexcept {
			return _hp;
		}

		/**
		 * Gets this unit's HP as displayed to the user.
		 * This method will divide the internal HP value by the granularity of HP
		 * values and round up to return what the user should see as this unit's
		 * HP.
		 * @return The current HP of this unit in a user-friendly format.
		 * @sa     awe::unit_type::HP_GRANULARITY
		 */
		inline awe::HP getDisplayedHP() const noexcept {
			return awe::unit_type::getDisplayedHP(_hp);
		}

		/**
		 * Sets this unit's fuel.
		 * If <tt>< 0</tt> is given, \c 0 will be stored.
		 * @param fuel The new fuel of this unit.
		 */
		inline void setFuel(const awe::Fuel fuel) noexcept {
			_fuel = ((fuel < 0) ? (0) : (fuel));
			_updateFuelAmmoIcon();
		}

		/**
		 * Gets this unit's fuel.
		 * @return The current fuel of this unit.
		 */
		inline awe::Fuel getFuel() const noexcept {
			return _fuel;
		}

		/**
		 * Sets this unit's ammo.
		 * If <tt>< 0</tt> is given, \c 0 will be stored.
		 * @param weapon The script name of the weapon to amend.
		 * @param ammo   The new ammo of this unit.
		 */
		inline void setAmmo(const std::string& weapon, const awe::Ammo ammo) {
			_ammos[weapon] = ((ammo < 0) ? (0) : (ammo));
			_updateFuelAmmoIcon();
		}

		/**
		 * Gets this unit's ammo.
		 * @param  weapon The script name of the weapon to query.
		 * @return The current ammo of this unit.
		 */
		inline awe::Ammo getAmmo(const std::string& weapon) const {
			return ((_ammos.find(weapon) == _ammos.end()) ? (0) :
				(_ammos.at(weapon)));
		}

		/**
		 * Checks if this unit is fully replenish.
		 * @param  heal If \c TRUE, also checks if the unit is fully healed.
		 * @return \c TRUE if the unit's internal HP, fuel, and all of its ammo is
		 *         at full. \c FALSE otherwise. Infinite fuel or ammo is counted as
		 *         being "at full."
		 */
		bool isReplenished(const bool heal = false) const;

		/**
		 * Replenishs this unit's fuel and all of its ammo.
		 * @param heal If \c TRUE, fully heal the unit, too.
		 */
		void replenish(const bool heal = false);

		/**
		 * Sets this unit's waiting/moved status.
		 * By default, a unit is created with the waiting status turned on.
		 * @param moved \c TRUE if the unit is in wait mode, \c FALSE if it can be
		 *              moved.
		 */
		inline void wait(const bool moved) noexcept {
			_waiting = moved;
		}

		/**
		 * Gets this unit's waiting/moved status.
		 * @return \c TRUE if this unit is waiting, \c FALSE if not.
		 */
		inline bool isWaiting() const noexcept {
			return _waiting;
		}

		/**
		 * Sets this unit's capturing status.
		 * By default, a unit is created with the capturing status turned off.
		 * @param capturing \c TRUE if the unit is in capture mode, \c FALSE if
		 *                  not.
		 */
		inline void capturing(const bool capturing) noexcept {
			_capturing = capturing;
			_updateCapturingHidingIcon();
		}

		/**
		 * Gets this unit's capturing status.
		 * @return \c TRUE if this unit is capturing, \c FALSE if not.
		 */
		inline bool isCapturing() const noexcept {
			return _capturing;
		}

		/**
		 * Sets this unit's hiding status.
		 * By default, a unit is created with the hiding status turned off.
		 * @param hiding \c TRUE if the unit is hiding, \c FALSE if not.
		 */
		inline void hiding(const bool hiding) noexcept {
			_hiding = hiding;
			_updateCapturingHidingIcon();
		}

		/**
		 * Gets this unit's hiding status.
		 * @return \c TRUE if this unit is hiding, \c FALSE if not.
		 */
		inline bool isHiding() const noexcept {
			return _hiding;
		}

		/**
		 * Loads another unit onto this one.
		 * @param  id The ID of the unit to load onto this one.
		 * @safety Strong guarantee.
		 */
		inline void loadUnit(const awe::UnitID id) {
			_loaded.insert(id);
			_updateLoadedIcon();
		}

		/**
		 * Unloads a unit from this one, if it exists.
		 * @param  id The ID of the unit to unload.
		 * @return \c TRUE if the unit was unloaded successfully, \c FALSE if the
		 *         unit wasn't loaded.
		 * @safety Strong guarantee.
		 */
		inline bool unloadUnit(const awe::UnitID id) {
			const auto res = _loaded.erase(id);
			_updateLoadedIcon();
			return res;
		}

		/**
		 * Copies the internal list of loaded units and returns it.
		 * @return The IDs of the all the units that are loaded onto this one.
		 */
		inline std::unordered_set<awe::UnitID> loadedUnits() const {
			return _loaded;
		}

		/**
		 * Loads this unit onto another.
		 * Provide \c NO_UNIT to indicate that this unit is not to be loaded onto
		 * another unit.
		 * @param id The ID of the unit this one is loaded onto.
		 */
		inline void loadOnto(const awe::UnitID id) noexcept {
			_loadedOnto = id;
		}

		/**
		 * Identifies which unit this unit is loaded onto.
		 * @return The ID of the unit this unit is loaded onto. \c awe::NO_UNIT if
		 *         none.
		 */
		inline awe::UnitID loadedOnto() const noexcept {
			return _loadedOnto;
		}
	private:
		/**
		 * Finds out if this unit's first finite ammo weapon is low on ammo.
		 * @return \c TRUE if this unit has at least one weapon with finite ammo
		 *         that has at most half of its ammo left. \c FALSE otherwise.
		 */
		bool _isLowOnAmmo() const;

		/**
		 * Updates the sprite's HP icon.
		 */
		void _updateHPIcon();

		/**
		 * Updates the sprite's fuel and ammo icon.
		 */
		void _updateFuelAmmoIcon();

		/**
		 * Updates the sprite's loaded icon.
		 */
		void _updateLoadedIcon();

		/**
		 * Updates the sprite's capturing and hiding icon.
		 */
		void _updateCapturingHidingIcon();

		/**
		 * The type of the unit.
		 */
		std::shared_ptr<const awe::unit_type> _type;

		/**
		 * The ID of the army the unit belongs to.
		 */
		awe::ArmyID _army = awe::NO_ARMY;

		/**
		 * The tile this unit occupies.
		 */
		sf::Vector2u _location = awe::unit::NO_POSITION;

		/**
		 * The HP of this unit.
		 */
		awe::HP _hp = 0;

		/**
		 * The fuel of this unit.
		 */
		awe::Fuel _fuel = 0;

		/**
		 * The ammo belonging to each of the unit's weapons.
		 */
		std::unordered_map<std::string, awe::Ammo> _ammos;

		/**
		 * The waiting/moved state of this unit.
		 */
		bool _waiting = true;

		/**
		 * The capturing state of this unit.
		 */
		bool _capturing = false;

		/**
		 * The hiding state of this unit.
		 */
		bool _hiding = false;

		/**
		 * The list of units loaded onto this one.
		 */
		std::unordered_set<awe::UnitID> _loaded;

		/**
		 * The unit this unit is loaded onto.
		 */
		awe::UnitID _loadedOnto = awe::NO_UNIT;

		/**
		 * Weak pointer to this unit's animated sprite.
		 */
		std::weak_ptr<awe::animated_unit> _unitSprite;

		/**
		 * Callback to be invoked when a change is to be made to \c _unitSprite.
		 */
		std::function<void(std::function<void(void)>)> _updateSprite;
	};
}
