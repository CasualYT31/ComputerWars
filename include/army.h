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
 * @file army.h
 * Declares the class which represents an army in a match.
 */

#pragma once

#include "army.fwd.h"
#include "terrain.fwd.h"
#include "terrain.h"
#include "unit.fwd.h"
#include "unit.h"
#include "typedef.h"
#include "bank.h"
#include "spritesheets.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	class army : public sfx::animated_drawable {
	public:
		/**
		 * Creates a new army object with its team ID and country information.
		 * @param country The information pertaining to the army's country.
		 * @param team    The team ID of the army.
		 */
		army(const std::shared_ptr<const awe::country>& country, awe::TeamID team) noexcept;

		/**
		 * Sets this army's country affiliation.
		 * @param country The army's new country. Cannot be \c nullptr. If so, call is ignored.
		 */
		void setCountry(const std::shared_ptr<const awe::country>& country) noexcept;

		/**
		 * Gets this army's country affiliation.
		 * @return The information of this army's country.
		 */
		std::shared_ptr<const awe::country> getCountry() const noexcept;

		/**
		 * Sets this army's team.
		 * @param team The new team ID of this army.
		 */
		void setTeam(const awe::TeamID team) noexcept;

		/**
		 * Gets this army's team.
		 * @return The army's current team ID.
		 */
		awe::TeamID getTeam() const noexcept;

		/**
		 * Sets the spritesheet to use for all units on the map.
		 * @param ptr Pointer to the spritesheet to assign to all units of this army.
		 */
		void setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept;

		/**
		 * Create new unit and add it to the army.
		 * By default, all stats (HP, fuel, ammo) are at maximum.
		 * @param unitType The type of unit to create.
		 */
		void createUnit(const std::shared_ptr<const awe::unit_type>& unitType) noexcept;

		/**
		 * Delete a unit from the army's list.
		 * Also checks if the unit to delete is loaded within another;
		 * loaded units won't be accessible from this army list directly.
		 * @param  uuid The UUID of the unit to delete.
		 * @return \c TRUE if the unit was deleted, \c FALSE if this army didn't have the specified unit.
		 */
		bool deleteUnit(engine::uuid<awe::unit> uuid) noexcept;
	private:
		/**
		 * Finds unit based on UUID search criterion.
		 * @param  uuid The UUID of the unit to find.
		 * @return Pointer to the unit if it exists in this army's list, \c nullptr if not.
		 */
		std::shared_ptr<awe::unit> _findUnit(engine::uuid<awe::unit> uuid) const noexcept;

		/**
		 * Country of the army.
		 */
		std::shared_ptr<const awe::country> _country;

		/**
		 * Team of the army.
		 */
		awe::TeamID _team;

		/**
		 * List of units that belong to this army.
		 */
		std::vector<std::shared_ptr<awe::unit>> _units;
	};
}

// OLD
//	/**
//	 * Class which represents a single army on the map.
//	 */
//	class army : public sfx::animated_drawable {
//	public:
//		/**
//		 * Initialises an army object.
//		 * @param team     The team ID of the army.
//		 * @param country  The country information of the army.
//		 */
//		army(awe::TeamID team = 0, const std::shared_ptr<const awe::country>& country = nullptr) noexcept;
//
//		/**
//		 * Sets the army's team.
//		 * Armies on the same team have the same team ID.
//		 * @param newTeam An ID representing a team.
//		 */
//		void setTeam(awe::TeamID newTeam) noexcept;
//
//		/**
//		 * Retrieves the army's team.
//		 * @return The team ID assigned to the army.
//		 */
//		awe::TeamID getTeam() const noexcept;
//
//		/**
//		 * Sets the army's country.
//		 * If an invalid/empty pointer is given, the call will be ignored.
//		 * @param newCountry Pointer the static information on the country of the army.
//		 */
//		void setCountry(std::shared_ptr<const awe::country> newCountry) noexcept;
//
//		/**
//		 * Retrieves information on this army's country.
//		 * By default, an army has no country, so care should be taken when accessing the returned pointer.
//		 * @return Pointer to the country information of this army.
//		 */
//		std::shared_ptr<const awe::country> getCountry() const noexcept;
//
//		/**
//		 * Sets the funds this army obtains.
//		 * If a negative value is given, \c 0 will be assigned instead.
//		 * @param newFunds The new fund amount.
//		 */
//		void setFunds(awe::Funds newFunds) noexcept;
//
//		/**
//		 * Retrieves the funds this army currently obtains.
//		 * @return The fund amount.
//		 */
//		awe::Funds getFunds() const noexcept;
//
//		/**
//		 * Sets the commanders of this army.
//		 * \c nullptr signifies that no commander should be present.
//		 * In the event that \c firstCO is \c nullptr and \c secondCO is not, these two values will be swapped.
//		 * @param firstCO  Pointer to the first CO's static information.
//		 * @param secondCO Pointer to the second CO's static information.
//		 */
//		void setCommanders(std::shared_ptr<const awe::commander> firstCO, std::shared_ptr<const awe::commander> secondCO = nullptr) noexcept;
//
//		/**
//		 * Retrieves the static information of the first commander.
//		 * By default, no CO is set to an army, so care should be taken when accessing the returned pointer.
//		 * @return Pointer to the first CO's information.
//		 */
//		std::shared_ptr<const awe::commander> getFirstCommander() const noexcept;
//
//		/**
//		 * Retrieves the static information of the second commander.
//		 * Special care should be taken when accessing the returned pointer, as an army may well have no secondary commander.
//		 * @return Pointer to the second CO's information.
//		 */
//		std::shared_ptr<const awe::commander> getSecondCommander() const noexcept;
//
//		/**
//		 * Adds a previously allocated tile to this army's collection of owned tiles.
//		 * If the given weak reference was expired at the time of the call, the call will be ignored.
//		 * @warning This method does \b not automatically call \c awe::tile::setOwner().
//		 * @param   ptr Weak reference to the tile.
//		 */
//		void addOwnedTile(const std::weak_ptr<awe::tile>& ptr) noexcept;
//
//		/**
//		 * Removes an owned tile from this army's collection.
//		 * Call is ignored if \c ptr is \c nullptr.
//		 * @warning This method does \b not automatically call \c awe::tile::setOwner().
//		 * @param   ptr Pointer to the tile to erase from this army's owned tile collection.
//		 */
//		void removeOwnedTile(const std::shared_ptr<awe::tile>& ptr) noexcept;
//
//		/**
//		 * Checks to see if a given tile is owned by this army.
//		 * @param  ptr Pointer to the tile to search for.
//		 * @return \c TRUE if the given pointer was found within the army's owned tile collection, \c FALSE otherwise.
//		 */
//		bool isOwnedTile(const std::shared_ptr<awe::tile>& ptr) const noexcept;
//
//		/**
//		 * Clears the army's owned tiles collection.
//		 * @warning This method does \b not automatically call \c awe::tile::setOwner().
//		 */
//		void clearOwnedTiles() noexcept;
//
//		/**
//		 * Counts the number of owned tiles that are in an army's collection based on a given filter.
//		 * If \c filter is empty, and \c inverted is \c FALSE, \c 0 will be returned.
//		 * If \c filter is empty, and \c inverted is \c TRUE, the number of owned tiles will be returned.\n
//		 * Individual tiles are not considered: rather their type of terrain is considered.
//		 * @param  filter   A collection of \c awe::terrain pointers that are to be searched for (or conversely, that are to be ignored, if \c inverted is \c TRUE).
//		 * @param  inverted If set to \c TRUE, the search will be inverted, i.e. tiles that are \em not any of the given types will be counted.
//		 * @return The number of tiles that match the search criteria.
//		 */
//		std::size_t ownedTilesCount(std::vector<std::shared_ptr<const awe::terrain>> filter = {}, const bool inverted = false) const noexcept;
//
//		/**
//		 * Allocates a new unit and adds it to the army's collection.
//		 * The given spritesheet pointer is automatically assigned to the newly created unit.
//		 * @warning This method does not automatically assign this army as the unit's owner. Please see \c awe::game::createUnit().
//		 * @param   typeInfo The type of unit to allocate.
//		 * @return  A pointer to the created unit.
//		 */
//		std::shared_ptr<awe::unit> addUnit(const std::shared_ptr<const awe::unit_type>& typeInfo) noexcept;
//
//		/**
//		 * Deletes a unit from the army's collection.
//		 * @param unitToDelete Pointer to the unit to delete.
//		 */
//		void removeUnit(const std::shared_ptr<awe::unit>& unitToDelete) noexcept;
//
//		/**
//		 * Finds out if a given unit belongs to this army.
//		 * @param  unitToSearch Pointer to the unit to search for.
//		 * @return \c TRUE if the given unit belongs to this army, \c FALSE otherwise.
//		 */
//		bool isArmysUnit(const std::shared_ptr<awe::unit>& unitToSearch) const noexcept;
//
//		/**
//		 * Deletes all of an army's units.
//		 */
//		void clearUnits() noexcept;
//
//		/**
//		 * Counts the number of units that are in an army's collection based on a given filter.
//		 * If \c filter is empty, and \c inverted is \c FALSE, \c 0 will be returned.
//		 * If \c filter is empty, and \c inverted is \c TRUE, the number of units will be returned.
//		 * @param  filter   A collection of \c awe::unit_type pointers that are to be searched for (or conversely, that are to be ignored, if \c inverted is \c TRUE).
//		 * @param  inverted If set to \c TRUE, the search will be inverted, i.e. units that are \em not any of the given types will be counted.
//		 * @return The number of units that match the search criteria.
//		 */
//		std::size_t unitCount(std::vector<std::shared_ptr<const awe::unit_type>> filter = {}, const bool inverted = false) const noexcept;
//
//		/**
//		 * Sets the unit spriteshees to be used with this army.
//		 * @param ptr Pointer to the spritesheet information to pull from.
//		 */
//		void setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept;
//
//		/**
//		 * Sets the unit picture spritesheet to be used with this army.
//		 * @param ptr Pointer to the spritesheet information to pull from.
//		 */
//		void setPictureSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept;
//
//		/**
//		 * This army's UUID object.
//		 */
//		engine::uuid<awe::army> UUID;
//
//		/**
//		 * Tests if a given \c awe::army object is equivalent to this one.
//		 * If the two armies given have the same internal ID, they are equivalent.
//		 * If they are different, they are two different armies.
//		 * @param  rhs Right-hand side argument. The \c awe::army object to test against.
//		 * @return \c TRUE if both objects are equivalent, \c FALSE if not.
//		 * @sa     operator!=()
//		 */
//		bool operator==(const awe::army& rhs) const noexcept;
//
//		/**
//		 * Tests if a given \c awe::army object is not equivalent to this one.
//		 * If the two armies given have the same internal ID, they are equivalent.
//		 * If they are different, they are two different armies.
//		 * @param  rhs Right-hand side argument. The \c awe::army object to test against.
//		 * @return \c TRUE if both objects are not equivalent, \c FALSE if they are.
//		 * @sa     operator==()
//		 */
//		bool operator!=(const awe::army& rhs) const noexcept;
//
//		/**
//		 * This drawable's \c animate() method.
//		 * This will essentially animate all of the units in play.
//		 * @return \c FALSE.
//		 */
//		virtual bool animate(const sf::RenderTarget& target) noexcept;
//	private:
//		/**
//		 * This drawable's \c draw() method.
//		 * Draws an army's units to the screen.
//		 * @param target The target to render the units to.
//		 * @param states The render states to apply to the sprite. Applying transforms is perfectly valid and will not alter the internal workings of the drawable.
//		 */
//		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
//
//		/**
//		 * Team ID.
//		 */
//		TeamID _team = 0;
//
//		/**
//		 * Country information.
//		 */
//		std::shared_ptr<const awe::country> _country = nullptr;
//
//		/**
//		 * Collection of units.
//		 */
//		std::vector<std::shared_ptr<awe::unit>> _units;
//
//		/**
//		 * Collection of owned tiles.
//		 */
//		std::vector<std::weak_ptr<awe::tile>> _ownedTiles;
//
//		/**
//		 * Funds.
//		 */
//		awe::Funds _funds = 0;
//
//		/**
//		 * First CO information.
//		 */
//		std::shared_ptr<const awe::commander> _firstCO = nullptr;
//
//		/**
//		 * Second CO information.
//		 */
//		std::shared_ptr<const awe::commander> _secondCO = nullptr;
//
//		/**
//		 * Pointer to the unit spritesheet.
//		 */
//		std::shared_ptr<sfx::animated_spritesheet> _unitSprites;
//
//		/**
//		 * Pointer to the unit picture spritesheet.
//		 */
//		std::shared_ptr<sfx::animated_spritesheet> _pictureSprites;
//	};
//}