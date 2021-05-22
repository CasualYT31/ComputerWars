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
 * Only basic checks are carried out in this class; all game
 * logic is separate.
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
	 */
	class map : sf::NonCopyable, public sfx::animated_drawable {
	public:
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
	private:
		/**
		 * Checks to see if a given X and Y coordinate are out of bounds with the map's current size.
		 * @param  pos The position to test.
		 * @return \c TRUE if the position is out of bounds, \c FALSE if not.
		 */
		bool _isOutOfBounds(const sf::Vector2u pos) const noexcept;

		/**
		 * Checks to see if a given army ID is present on the map.
		 * @param  id The ID of the army to check.
		 * @return \c TRUE if the army is on the map, \c FALSE if they are not.
		 */
		bool _isArmyPresent(const awe::UUIDValue id) const noexcept;

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
		awe::UnitID _lastUnitID = 0;

		/**
		 * The armys on this map.
		 * To retain turn order, which is defined by countries, an ordered map was chosen.\n
		 * @warning Please ensure that an army with the ID \c INVALID isn't created.
		 */
		std::map<awe::UUIDValue, awe::army> _armys;
	};
}