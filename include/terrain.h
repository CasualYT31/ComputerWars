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
 * @file terrain.h
 * This header file declares the class used to represent a single tile on the map.
 */

#pragma once

#include "terrain.fwd.h"
#include "unit.fwd.h"
#include "unit.h"
#include "army.fwd.h"
#include "bank.h"
#include "spritesheets.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class representing a single terrain tile on the map.
	 */
	class tile : public sfx::animated_drawable {
	public:
		/**
		 * Minimum height a tile graphic should have, in native resolution pixels.
		 */
		static const unsigned int MIN_TILE_HEIGHT = 16;

		/**
		 * Constructs a tile.
		 * @param location The location of this tile in the map, which won't change after allocation.
		 */
		tile(sf::Vector2u location) noexcept;

		/**
		 * Retrieves the location this tile is on in its map.
		 * @return The location in tile coordinates.
		 */
		sf::Vector2u getLocation() const noexcept;

		/**
		 * Updates the type of tile this object represents.
		 * @param newTile A pointer to the static information on the tile type to assign.
		 */
		void setTileType(const std::shared_ptr<const tile_type>& newTile) noexcept;

		/**
		 * Retrieves the information on the tile's static information.
		 * @return Pointer to the information.
		 */
		std::shared_ptr<const tile_type> getTileType() const noexcept;

		/**
		 * Updates the HP of the tile.
		 * If outside of the range of the HP (0 to the tile type's maximum HP),
		 * it will automatically be adjusted to max HP, if the tile has been given a type.
		 * @param  newHP The new HP
		 * @return The old HP.
		 */
		void setHP(const awe::HP newHP) noexcept;

		/**
		 * Retrieves the current HP of the tile.
		 * @return The current HP.
		 */
		awe::HP getHP() const noexcept;

		/**
		 * Updates the owner of the tile.
		 * If the given pointer is /em expired, this denotes that the tile should not be owned.
		 * @param A pointer to the army which owns the tile.
		 */
		void setOwner(const std::shared_ptr<army>& newOwner) noexcept;

		/**
		 * Retrieves the current owner of the tile.
		 * If the returned pointer is /em expired, this denotes that the tile is not owned.
		 * @return The army ID of the current owner.
		 */
		std::weak_ptr<army> getOwner() const noexcept;

		/**
		 * Allows the client to assign a weak reference to a unit that is occupying this tile.
		 * If the given pointer is /em empty (\c nullptr), this denotes that the tile should not be occupied.
		 * @param newUnit The reference to the unit that is currently occupying this tile.
		 */
		void setUnit(const std::shared_ptr<awe::unit>& newUnit) noexcept;

		/**
		 * Retrieves a reference to the unit currently occupying this tile.
		 * If the returned pointer is /em expired, this denotes that the tile is not occupied.
		 * @return Information on the unit that is occupying this tile.
		 */
		std::weak_ptr<awe::unit> getUnit() const noexcept;

		/**
		 * The tile's UUID object.
		 */
		engine::uuid<awe::tile> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The other \c tile object to test against.
		 * @return \c TRUE if both tile's UUID objects are equivalent, \c FALSE if not.
		 */
		bool operator==(const awe::tile& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The other \c tile object to test against.
		 * @return \c TRUE if both tile's UUID objects are not equivalent, \c FALSE if they are.
		 */
		bool operator!=(const awe::tile& rhs) const noexcept;

		/**
		 * Sets the tile spritesheet used with this tile.
		 * @param ptr Pointer to the data.
		 */
		void setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept;

		/**
		 * Gets the spritesheet used with this tile.
		 * @return Pointer to the data.
		 */
		std::shared_ptr<sfx::animated_spritesheet> getTileSpritesheet() const noexcept;

		/**
		 * Gets the sprite ID used with this tile.
		 * @return The sprite ID assigned to the internal sprite object.
		 */
		unsigned int getTileSpriteID() const noexcept;

		/**
		 * This drawable's \c animate() method.
		 * This will animate the tile's sprite. Note that the occupying unit is not animated.
		 * @return The return value of the internal \c sfx::animated_sprite::animate() call.
		 * @sa     \c sfx::animated_sprite::animate()
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws the tile to the screen. Note that the occupying unit is not drawn.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying transforms is perfectly valid and will not alter the internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Updates the sprite graphic ID of the tile.
		 * The sprite graphic of a tile is automatically updated whenever the tile's owner, type, or spritesheet changes.
		 */
		void _updateSprite() noexcept;

		/**
		 * Pointer to the tile type's static information.
		 */
		std::shared_ptr<const tile_type> _tileType = nullptr;

		/**
		 * Weak reference to the army which owns the tile.
		 */
		std::weak_ptr<army> _owner;

		/**
		 * The HP of the tile.
		 */
		awe::HP _hp = 0;

		/**
		 * Weak reference to the unit occupying this tile.
		 */
		std::weak_ptr<awe::unit> _unit;

		/**
		 * Location of this tile in the map.
		 */
		sf::Vector2u _location;

		/**
		 * Pointer to the tile spritesheet used by this tile.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _tileSpritesheet;

		/**
		 * Tile's sprite.
		 */
		sfx::animated_sprite _sprite;
	};
}