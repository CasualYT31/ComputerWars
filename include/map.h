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
 * @file map.h
 * This header file declares the \c map class, which represents a grid of tiles and their units.
 */

#include "terrain.h"

#pragma once

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class which represents a grid of tiles and their units.
	 */
	class map {
	public:
		/**
		 * Resizes the map to the given dimensions.
		 * If the map shrinks, tiles towards the right and bottom sides of the map will be completely erased.
		 * If the map grows, tiles will be added to the right and bottom sides of the map.
		 * @param  dim The new dimensions of the map, in tiles.
		 * @return The old dimensions of the map.
		 * @sa     _size
		 */
		sf::Vector2u setSize(const sf::Vector2u& dim) noexcept;

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The current dimensions of the map.
		 * @sa     _size
		 */
		sf::Vector2u getSize() const noexcept;

		/**
		 * Updates the name of the map.
		 * @param  newName The new name to give the map.
		 * @return The old name of the map.
		 */
		std::string setName(const std::string& newName) noexcept;

		/**
		 * Retrieves a copy of the map's name.
		 * @return The name of the map.
		 */
		std::string getName() const noexcept;

		/**
		 * Retrieves a reference to a specified tile.
		 * @param  pos The position of the tile to retrieve.
		 * @return A reference to the tile at the given position.
		 * @throws std::out_of_range if the given position was outside of the map's size.
		 * @sa     _tiles
		 * @sa     awe::tile
		 */
		std::shared_ptr<awe::tile> getTile(const sf::Vector2u& pos);
	private:
		/**
		 * Stores the map's tiles.
		 * The outer vector stores each set of vertical tiles, and the inner vector stores each tile in a column.
		 * This means the /em first index will refer to the tile in the horizontal axis, and the /em second index will
		 * identify the tile in the vertical axis.
		 */
		std::vector<std::vector<std::shared_ptr<awe::tile>>> _tiles;

		/**
		 * Stores the map's dimensions, in tiles.
		 * /c x will hold the width, /c y will hold the height.
		 */
		sf::Vector2u _size;

		/**
		 * Stores the map's name.
		 */
		std::string _name = "";
	};
}