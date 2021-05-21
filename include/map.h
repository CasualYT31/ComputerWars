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

#include "terrain.fwd.h"
#include "terrain.h"
#include "spritesheets.h"

#pragma once

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class which represents a grid of tiles and their units.
	 */
	class map : public sfx::animated_drawable {
	public:
		/**
		 * Resizes the map to the given dimensions.
		 * If the map shrinks, tiles towards the right and bottom sides of the map will be completely erased.
		 * If the map grows, tiles will be added to the right and bottom sides of the map.
		 * @param dim The new dimensions of the map, in tiles.
		 * @sa    _size
		 */
		void setSize(const sf::Vector2u& dim) noexcept;

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The current dimensions of the map.
		 * @sa     _size
		 */
		sf::Vector2u getSize() const noexcept;

		/**
		 * Updates the name of the map.
		 * @param newName The new name to give the map.
		 */
		void setName(const std::string& newName) noexcept;

		/**
		 * Retrieves a copy of the map's name.
		 * @return The name of the map.
		 */
		std::string getName() const noexcept;

		/**
		 * Sets each tile's spritesheet.
		 * @param ptr Pointer to the spritesheet that this map will use.
		 */
		void setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept;

		/**
		 * Sets a specified tile's type.
		 * @param pos  The X and Y position of the tile to change.
		 * @param type The static information of the type of tile to set this tile to.
		 */
		void setTileType(sf::Vector2u pos, const std::shared_ptr<const awe::tile_type>& type) noexcept;

		/**
		 * Gets a specified tile's type.
		 * @param  pos The X and Y position of the tile to inspect.
		 * @return The static information of the specified tile, or \c nullptr if \c pos was an invalid coordinate.
		 */
		std::shared_ptr<const awe::tile_type> getTileType(sf::Vector2u pos) const noexcept;

		/**
		 * Sets a specified tile's HP.
		 * @param pos The X and Y position of the tile to change.
		 * @param hp  The new HP value to assign to the tile.
		 */
		void setTileHP(sf::Vector2u pos, awe::HP hp) noexcept;

		/**
		 * Retrieves the specified tile's HP.
		 * @param  pos The X and Y position of the tile to inspect.
		 * @return The current HP of the tile, or \c -1 if the tile coordinate given was invalid.
		 */
		awe::HP getTileHP(sf::Vector2u pos) const noexcept;

		/**
		 * Sets the owner of a given tile.
		 * @param pos  The X and Y location of the tile to change.
		 * @param army A pointer to the army object which now owns the specified tile.
		 */
		void setTileOwner(sf::Vector2u pos, const std::shared_ptr<awe::army>& owner) noexcept;

		/**
		 * Finds the owner of a given tile.
		 * @param  pos The X and Y location of the tile to find the owner of.
		 * @return A pointer to the army object which owns the specified tile.
		 *         If the pointer is expired, it indicates that the tile doesn't
		 *         have an owner, or the given tile coordinate was invalid.
		 */
		std::weak_ptr<awe::army> getTileOwner(sf::Vector2u pos) const noexcept;

		/**
		 * Sets the visible portion of the map.
		 * If <tt>(0, 0, 0, 0)</tt> is given, all the of the map becomes visible.
		 * @param  portion The new portion of the map to draw, all measurements are in tiles.
		 * @return \c TRUE if the portion was updated, \c FALSE if the portion was out of the map's range and thus was not updated.
		 * @sa     _visiblePortion
		 *
		bool setVisiblePortion(const sf::Rect<unsigned int> portion) noexcept;

		/**
		 * Retrieves a copy of the visible portion of the map.
		 * @return The visible portion of the map.
		 *
		sf::Rect<unsigned int> getVisiblePortion() const noexcept;*/

		/**
		 * This drawable's \c animate() method.
		 * This will animate each of the map's tile sprites.
		 * @return \c FALSE.
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws the map's visible tiles.
		 * @param target The target to render the map to.
		 * @param states The render states to apply to the map. Applying transforms is perfectly valid and will not alter the internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Retrieves a pointer to a tile given its location.
		 * @param  pos The position of the tile on the map.
		 * @return A pointer to the tile, or \c nullptr if the tile is not within the map.
		 */
		std::shared_ptr<awe::tile> _findTile(sf::Vector2u pos) const noexcept;

		/**
		 * Stores the map's tiles.
		 * The outer vector stores each set of vertical tiles, and the inner vector stores each tile in a column.
		 * This means the \em first index will refer to the tile in the horizontal axis, and the \em second index will
		 * identify the tile in the vertical axis.
		 */
		std::vector<std::vector<std::shared_ptr<awe::tile>>> _tiles;

		/**
		 * Stores the map's dimensions, in tiles.
		 * \c x will hold the width, \c y will hold the height.
		 */
		sf::Vector2u _size;

		/**
		 * Stores the map's name.
		 */
		std::string _name = "";

		/**
		 * The portion of the map that is rendered to the screen.
		 * By default, all of the map is drawn. Note also that this does not affect the tiles that are animated, as
		 * all tiles are always animated.
		 */
		sf::Rect<unsigned int> _visiblePortion = sf::Rect<unsigned int>(0, 0, 0, 0);
	};
}