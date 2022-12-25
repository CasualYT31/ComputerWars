/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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
#include <limits>

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
		 * @param   type  The type of the unit, which can't be changed.
		 * @param   army  The army the unit belongs to, which can't be changed.
		 * @param   sheet Pointer to the spritesheet to use with this unit.
		 * @param   icons Pointer to the icon spritesheet to use with this unit.
		 */
		unit(const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army = 0,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr,
			const std::shared_ptr<sfx::animated_spritesheet>& icons = nullptr)
			noexcept;

		/**
		 * Gets the unit's type.
		 * @return The information on the unit's type.
		 */
		inline std::shared_ptr<const awe::unit_type> getType() const noexcept {
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
		 * @param pos The X and Y location of the tile this unit occupies.
		 */
		inline void setPosition(const sf::Vector2u pos) noexcept {
			_location = pos;
		}

		/**
		 * Gets this unit's location.
		 * @return The X and Y location of the tile this unit occupies.
		 */
		inline sf::Vector2u getPosition() const noexcept {
			return _location;
		}

		/**
		 * Used to determine if this unit is on a tile in the map.
		 * Useful for loaded units, which are in the game but not on the map.
		 * @return \c TRUE if the unit occupies a tile on the map, \c FALSE if not.
		 */
		inline bool isOnMap() const noexcept {
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
		inline void setAmmo(const std::string& weapon, const awe::Ammo ammo)
			noexcept {
			_ammos[weapon] = ((ammo < 0) ? (0) : (ammo));
		}

		/**
		 * Gets this unit's ammo.
		 * @param  weapon The script name of the weapon to query.
		 * @return The current ammo of this unit.
		 */
		inline awe::Ammo getAmmo(const std::string& weapon) const noexcept {
			return ((_ammos.find(weapon) == _ammos.end()) ? (0) :
				(_ammos.at(weapon)));
		}

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
		 * @param id The ID of the unit to load onto this one.
		 */
		inline void loadUnit(const awe::UnitID id) noexcept {
			_loaded.insert(id);
		}

		/**
		 * Unloads a unit from this one, if it exists.
		 * @param  id The ID of the unit to unload.
		 * @return \c TRUE if the unit was unloaded successfully, \c FALSE if the
		 *         unit wasn't loaded.
		 */
		inline bool unloadUnit(const awe::UnitID id) noexcept {
			return _loaded.erase(id);
		}

		/**
		 * Copies the internal list of loaded units and returns it.
		 * @return The IDs of the all the units that are loaded onto this one.
		 */
		inline std::unordered_set<awe::UnitID> loadedUnits() const noexcept {
			return _loaded;
		}

		/**
		 * Loads this unit onto another.
		 * Provide \c 0 to indicate that this unit is not to be loaded onto another
		 * unit.
		 * @param id The ID of the unit this one is loaded onto.
		 */
		inline void loadOnto(const awe::UnitID id) noexcept {
			_loadedOnto = id;
		}

		/**
		 * Identifies which unit this unit is loaded onto.
		 * @return The ID of the unit this unit is loaded onto. 0 if none.
		 */
		inline awe::UnitID loadedOnto() const noexcept {
			return _loadedOnto;
		}

		/**
		 * Sets the spritesheet to use with this unit.
		 * @param sheet Pointer to the spritesheet to use with this unit.
		 */
		inline void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
			_sprite.setSpritesheet(sheet);
		}

		/**
		 * Sets the icon spritesheet to use with this unit.
		 * @param sheet Pointer to the icon spritesheet to use with this unit.
		 */
		void setIconSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept;

		/**
		 * Gets the spritesheet used with this unit.
		 * @return Pointer to the spritesheet used with this unit.
		 */
		inline std::shared_ptr<const sfx::animated_spritesheet> getSpritesheet()
			const noexcept {
			return _sprite.getSpritesheet();
		}

		/**
		 * Finds out the sprite name used with this unit's internal sprite.
		 * @return The name of the sprite from the spritesheet used with this unit.
		 */
		inline std::string getSprite() const noexcept {
			return _sprite.getSprite();
		}

		/**
		 * Sets the unit's pixel position to the internal sprite.
		 * @param x The X position of the tile.
		 * @param y The Y position of the tile.
		 */
		inline void setPixelPosition(float x, float y) noexcept {
			_sprite.setPosition(sf::Vector2f(x, y));
		}

		/**
		 * This drawable's \c animate() method.
		 * Calls the internal sprite's \c animate() method. Also determines which
		 * icons to display and animates them.
		 * @remark Idea for future optimisation: move \c setSprite() calls to other
		 *         methods, e.g. move \c _loadedIcon.setSprite() to \c loadUnit().
		 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
		 *         call.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws the unit to the screen along with any icons it should display.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Finds out if this unit's first finite ammo weapon is low on ammo.
		 * @return \c TRUE if this unit has at least one weapon with finite ammo
		 *         that has at most half of its ammo left. \c FALSE otherwise.
		 */
		bool _isLowOnAmmo() const noexcept;

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
		awe::UnitID _loadedOnto = 0;

		/**
		 * The unit's animated sprite object.
		 */
		sfx::animated_sprite _sprite;

		/**
		 * The unit's HP icon sprite object.
		 */
		sfx::animated_sprite _hpIcon;

		/**
		 * The unit's fuel and ammo shortage icon sprite object.
		 */
		sfx::animated_sprite _fuelAmmoIcon;

		/**
		 * The unit's loaded icon sprite object.
		 */
		sfx::animated_sprite _loadedIcon;

		/**
		 * The unit's capturing and hiding icon sprite object.
		 */
		sfx::animated_sprite _capturingHidingIcon;
	};
}
