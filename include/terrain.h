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
#include "army.fwd.h"
#include "bank.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class representing a single terrain tile on the map.
	 */
	class tile {
	public:
		/**
		 * Constructs a tile.
		 * All properties are set using their respective set methods.
		 * @param location The location of this tile in the map.
		 * @param tile     The type of tile this object represents.
		 * @param hp       The initial HP of the tile.
		 * @param owner    The owner's army ID.
		 * @sa    awe::tile_type
		 */
		tile(sf::Vector2u location, const std::shared_ptr<const tile_type>& tile = nullptr, const sf::Int32 hp = -1, std::shared_ptr<awe::army> owner = nullptr) noexcept;
		
		/**
		 * Sets the location this tile has on its map.
		 * @param location The location in tile coordinates.
		 */
		void setLocation(sf::Vector2u location) noexcept;

		/**
		 * Retrieves the location this tile is on in its map.
		 * @return The location in tile coordinates.
		 */
		sf::Vector2u getLocation() const noexcept;

		/**
		 * Updates the type of tile this object represents.
		 * @param newTile A pointer to the static information on the tile type to assign.
		 */
		void setTile(const std::shared_ptr<const tile_type>& newTile) noexcept;

		/**
		 * Retrieves the information on the tile's static information.
		 * @return Pointer to the information.
		 */
		std::shared_ptr<const tile_type> getTile() const noexcept;

		/**
		 * Updates the HP of the tile.
		 * If outside of the range of the HP (0 to the tile type's maximum HP),
		 * it will automatically be adjusted to max HP, if the tile has been given a type.
		 * @param  newHP The new HP
		 * @return The old HP.
		 */
		sf::Int32 setHP(const sf::Int32 newHP) noexcept;

		/**
		 * Retrieves the current HP of the tile.
		 * @return The current HP.
		 */
		sf::Int32 getHP() const noexcept;

		/**
		 * Updates the owner of the tile.
		 * If the given pointer is /em expired, this denotes that the tile should not be owned.
		 * @param A pointer to the army which owns the tile.
		 */
		void setOwner(std::shared_ptr<army> newOwner) noexcept;

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
		void setUnit(std::shared_ptr<awe::unit> newUnit) noexcept;

		/**
		 * Retrieves a reference to the unit currently occupying this tile.
		 * If the returned pointer is /em expired, this denotes that the tile is not occupied.
		 * @return Information on the unit that is occupying this tile.
		 */
		std::weak_ptr<awe::unit> getUnit() const noexcept;

		/**
		 * Tests whether or not this tile is owned by another army.
		 * Note that this method will also return /c FALSE if the assigned army has been destroyed,
		 * rendering the weak reference invalid.
		 * @return /c TRUE if this tile has been assigned a valid owner, /c FALSE otherwise.
		 */
		bool isOwned() const noexcept;

		/**
		 * Tests whether or not this tile is currently occupied.
		 * Note that this method will also return /c FALSE if the assigned unit has been destroyed,
		 * rendering the weak reference invalid.
		 * @return /c TRUE if this tile is being occupied, /c FALSE otherwise.
		 */
		bool isOccupied() const noexcept;

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
	private:
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
		sf::Int32 _hp = 0;

		/**
		 * Weak reference to the unit occupying this tile.
		 */
		std::weak_ptr<awe::unit> _unit;

		/**
		 * Location of this tile in the map.
		 */
		sf::Vector2u _location;
	};
}