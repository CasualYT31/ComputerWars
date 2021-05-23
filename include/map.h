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

/**@file map.h
 * Declares the class which is used to manage a map.
 * Unit, armies, and tiles are all stored and managed here.
 */

#include "tile.h"
#include "unit.h"
#include "army.h"
#include "spritesheets.h"

#pragma once

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class which represents a map, and the armies and units that play on it.
	 * Only basic checks are carried out in this class; all game logic is separate.
	 * If any of these basic checks fail, they will be logged.
	 */
	class map : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "map."
		 * @sa    \c global::logger
		 */
		map(const std::string& name = "map") noexcept;

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		/**
		 * Sets the map's name.
		 * By default, a map's name is a blank string.
		 * @param name The name to give to the map to replace the old name.
		 */
		void setMapName(const std::string& name) noexcept;

		/**
		 * Retrieves the map's name.
		 * @return The current name of the map.
		 */
		std::string getMapName() const noexcept;

		/**
		 * Sets the map's size, in tiles.
		 * If any tile should become out of bounds after the resize,
		 * they <b>as well as the unit on them</b> will be deleted.
		 * Any indicies to these objects within any army object will also
		 * be dropped.\n
		 * If any new tiles should be created, then they will be assigned
		 * \c tile as their new type as and when they are created.
		 * @remark Since this operation shouldn't be a bottleneck,
		 *         I've not concerned myself much with efficiency in its
		 *         implementation. The main way to improve efficiency
		 *         would be to loop through each relevant tile and
		 *         remove their unit before the resize, rather than
		 *         looping through \em all units and removing out of
		 *         bounds ones, after the resize.
		 * @param  dim  The width (x) and height (y) to make the map.
		 * @param  tile The type of tile to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u dim, const std::shared_ptr<const awe::tile_type>& tile = nullptr) noexcept;

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The map's size. X = width, Y = height.
		 */
		sf::Vector2u getMapSize() const noexcept;

		/////////////////////
		// ARMY OPERATIONS //
		/////////////////////
		/**
		 * Allocates a new army.
		 * If the army with the given country already exists, or \c nullptr is given,
		 * the call will be ignored.
		 * @param country The country of the army.
		 */
		void createArmy(const std::shared_ptr<const awe::country>& country) noexcept;

		/**
		 * Deletes an army entirely from the map.
		 * Deleting an army removes the army from the army list, deletes all the units belonging to the army,
		 * and disowns all owned tiles.
		 * @param army The ID of the army to delete.
		 */
		void deleteArmy(const awe::UUIDValue army) noexcept;

		/**
		 * Sets the amount of funds a specified army obtains.
		 * If \c 0 or less is given, \c 0 will be stored.
		 * @param army  The ID of the army to amend.
		 * @param funds The new fund amount to assign to the army.
		 */
		void setArmyFunds(const awe::UUIDValue army, const awe::Funds funds) noexcept;

		/**
		 * Retrieves an army's fund count.
		 * @param  army The ID of the army to inspect.
		 * @return The amount of funds this army possesses, or \c 0 if the given army doesn't exist.
		 */
		awe::Funds getArmyFunds(const awe::UUIDValue army) const noexcept;

		/////////////////////
		// UNIT OPERATIONS //
		/////////////////////
		/**
		 * Creates a new unit.
		 * The unit won't be created if the army ID isn't valid.
		 * @param  type The type of unit to create.
		 * @param  army The ID of the army who will own this unit.
		 * @return The 1-based ID of the unit created. Will be \c 0 if the unit couldn't be created.
		 */
		awe::UnitID createUnit(const std::shared_ptr<const awe::unit_type>& type, const awe::UUIDValue army) noexcept;

		/**
		 * Deletes a unit.
		 * A deleted unit will be removed from the map's and owning army's list, as well as the tile it was on.\n
		 * @warning Any loaded units will \b also be deleted.
		 * @param   id The ID of the unit to delete.
		 */
		void deleteUnit(const awe::UnitID id) noexcept;

		/**
		 * Sets a unit's position on the map.
		 * The operation will be cancelled if the specified tile is already occupied.
		 * @param id  The ID of the unit to move.
		 * @param pos The X and Y coordinate of the tile to move the unit to.
		 */
		void setUnitPosition(const awe::UnitID id, const sf::Vector2u pos) noexcept;

		/**
		 * Retrieves a unit's position, indicating the tile it is occupying.
		 * This method does not take into account if the unit is \em actually on a tile:
		 * please use this method in conjunction with \c isUnitOnMap().\n
		 * If the unit doesn't exist, (0, 0) will be returned.
		 * @param  id The ID of the unit.
		 * @return The X and Y location of the unit on the map.
		 */
		sf::Vector2u getUnitPosition(const awe::UnitID id) const noexcept;

		/**
		 * Finds out if this unit occupies a tile or not.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if this unit occupies a tile, \c FALSE if not.
		 */
		bool isUnitOnMap(const awe::UnitID id) const noexcept;

		/**
		 * Sets this unit's HP.
		 * If < 0 is given, 0 will be stored.
		 * @param id The ID of the unit to amend.
		 * @param hp The new HP of the unit.
		 */
		void setUnitHP(const awe::UnitID id, const awe::HP hp) noexcept;

		/**
		 * Gets this unit's HP.
		 * @param  id The ID of the unit to inspect.
		 * @return The HP of the unit. \c 0 if unit doesn't exist.
		 */
		awe::HP getUnitHP(const awe::UnitID id) const noexcept;

		/**
		 * Sets this unit's fuel.
		 * If < 0 is given, 0 will be stored.
		 * @param id   The ID of the unit to amend.
		 * @param fuel The new fuel of the unit.
		 */
		void setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) noexcept;

		/**
		 * Gets this unit's fuel.
		 * @param  id The ID of the unit to inspect.
		 * @return The fuel of the unit. \c 0 if unit doesn't exist.
		 */
		awe::Fuel getUnitFuel(const awe::UnitID id) const noexcept;

		/**
		 * Sets this unit's ammo.
		 * If < 0 is given, 0 will be stored.
		 * @param id   The ID of the unit to amend.
		 * @param ammo The new ammo of the unit.
		 */
		void setUnitAmmo(const awe::UnitID id, const awe::Ammo ammo) noexcept;

		/**
		 * Gets this unit's ammo.
		 * @param  id The ID of the unit to inspect.
		 * @return The ammo of the unit. \c 0 if unit doesn't exist.
		 */
		awe::Ammo getUnitAmmo(const awe::UnitID id) const noexcept;

		/**
		 * Loads one unit onto another.
		 * If the unit to load is already loaded onto another unit, this call will be ignored.\n
		 * This method does not consider any load limits imposed by any unit type.
		 * @param load The ID of the unit to load onto another.
		 * @param onto The ID of the unit which will load the unit.
		 */
		void loadUnit(const awe::UnitID load, const awe::UnitID onto) noexcept;

		/**
		 * Unloads one unit onto a given tile.
		 * If the given location is out of bounds or occupied, then the operation will be cancelled.
		 * If \c unload is not loaded onto \c from, then the call will be ignored.
		 * @param unload The ID of the unit to unload.
		 * @param from   The ID of the unit which houses the unit to unload.
		 * @param onto   The X and Y location to move the unloaded unit to.
		 */
		void unloadUnit(const awe::UnitID unload, const awe::UnitID from, const sf::Vector2u onto) noexcept;

		/////////////////////
		// TILE OPERATIONS //
		/////////////////////
		/**
		 * Sets a specified tile's type.
		 * By default, a tile does not have a type, unless it was given in the call to \c setMapSize().\n
		 * Changing a tile's type will automatically remove any ownership of the tile,
		 * but it will not reset the tile's HP.
		 * @param pos  The X and Y coordinate of the tile to change.
		 * @param type The type to assign to the tile.
		 */
		void setTileType(const sf::Vector2u pos, const std::shared_ptr<const awe::tile_type>& type) noexcept;

		/**
		 * Retrieves the specified tile's type.
		 * If an out of bounds coordinate is given, \c nullptr is returned.
		 * @param  pos The X and Y coordinate of the tile to change.
		 * @return The type of the tile and its information.
		 */
		std::shared_ptr<const awe::tile_type> getTileType(const sf::Vector2u pos) const noexcept;

		/**
		 * Sets a tile's HP.
		 * If a negative value is given, it will be adjusted to \c 0.
		 * @param pos The X and Y coordinate of the tile to change.
		 * @param hp  The HP to assign to the tile.
		 */
		void setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept;

		/**
		 * Retrieves a tile's HP.
		 * @param  post The X and Y coordinate of the tile to retrieve from.
		 * @return The HP of the tile, or \c 0 if the given coordinate was out of bounds.
		 */
		awe::HP getTileHP(const sf::Vector2u pos) const noexcept;

		/**
		 * Sets a tile's owner.
		 * @param pos  The X and Y coordinate of the tile to change.
		 * @param army The ID of the army who now owns this tile.
		 *             \c INVALID can be given to signal that the tile should not have an owner.
		 */
		void setTileOwner(const sf::Vector2u pos, awe::UUIDValue army) noexcept;

		/**
		 * Gets a tile's owner.
		 * If the coorindate is out of bounds, \c INVALID is returned.
		 * @param  pos The X and Y coordinate of the tile to change.
		 * @return The ID of the army who owns this tile, or \c INVALID if no army owns it.
		 */
		awe::UUIDValue getTileOwner(const sf::Vector2u pos) const noexcept;

		/**
		 * Retrieves the unit currently occupying a specified tile.
		 * This method will return 0 if there is a unit on this tile, but it is not physically on the map,
		 * such as if the unit is loaded onto another unit.
		 * @param  pos The X and Y coordinate of the tile to inspect.
		 * @return The ID of the unit occupying this tile. 0 if the tile is vacant or out of bounds.
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u pos) const noexcept;
	private:
		/**
		 * Checks if a given X and Y coordinate are out of bounds with the map's current size.
		 * @param  pos The position to test.
		 * @return \c TRUE if the position is out of bounds, \c FALSE if not.
		 */
		bool _isOutOfBounds(const sf::Vector2u pos) const noexcept;

		/**
		 * Checks if a given army ID is present on the map.
		 * @param  id The ID of the army to check.
		 * @return \c TRUE if the army is on the map, \c FALSE if they are not.
		 */
		bool _isArmyPresent(const awe::UUIDValue id) const noexcept;

		/**
		 * Checks if a unit is present in the game.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if the unit is on the map, \c FALSE if they are not.
		 */
		bool _isUnitPresent(const awe::UnitID id) const noexcept;

		/**
		 * Determines the ID the next unit should have.
		 * A unit ID cannot be 0
		 * Automatically assigns the returned value to \c _lastUnitID.
		 * @warning \c UINT32_MAX is used interally to test if unit capacity has been reached.
		 *          Don't forget to change this if you change the size of \c awe::UnitID!
		 * @throws  std::bad_alloc In the [practically impossible] case that a new unique unit ID cannot be generated.
		 */
		awe::UnitID _findUnitID();

		/**
		 * Internal logger object.
		 */
		mutable global::logger _logger;

		/**
		 * Stores the map's name.
		 */
		std::string _mapName = "";

		/**
		 * The tiles in this map.
		 * The first vector stores each column of tiles, from left to right.
		 * The second vector stores each tile of each column, from top to bottom.
		 * This means that the first index will be the X coordinate, moving from left to right,
		 * and the second index will be the Y coordinate, moving from top to bottom.
		 * Counting starts from the top left corner of the map.
		 */
		std::vector<std::vector<awe::tile>> _tiles;

		/**
		 * The units on this map.
		 */
		std::unordered_map<awe::UnitID, awe::unit> _units;

		/**
		 * The ID of the last unit created.
		 * Used to generate unit IDs once the initial unit has been created.
		 */
		awe::UnitID _lastUnitID = 1;

		/**
		 * The armys on this map.
		 * To retain turn order, which is defined by countries, an ordered map was chosen.\n
		 * @warning Please ensure that an army with the ID \c INVALID isn't created.
		 */
		std::map<awe::UUIDValue, awe::army> _armys;
	};
}