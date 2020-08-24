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

#include "unit.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	class army;

	/**
	 * Class representing a single terrain tile on the map.
	 */
	class tile {
	public:
		/**
		 * Constructs a tile.
		 * All properties are set using their respective set methods.
		 * @param tile  The type of tile this object represents.
		 * @param owner The owner's army ID.
		 * @param hp    The initial HP of the tile.
		 * @sa    awe::tile_type
		 */
		tile(const tile_type* tile = nullptr, const int hp = 0, std::weak_ptr<army> owner = std::weak_ptr<army>()) noexcept;
		
		/**
		 * Updates the type of tile this object represents.
		 * @param  newTile A pointer to the static information on the tile type to assign.
		 * @return Pointer to the old tile type's information.
		 */
		const tile_type* setTile(const tile_type* newTile) noexcept;

		/**
		 * Retrieves the information on the tile's static information.
		 * @return Pointer to the information.
		 */
		const tile_type* getTile() const noexcept;

		/**
		 * Updates the HP of the tile.
		 * If outside of the range of the HP (0 to the tile type's maximum HP),
		 * it will automatically be adjusted (0 if <0, max HP if >max HP).
		 * @param  newHP The new HP
		 * @return The old HP.
		 */
		int setHP(const int newHP) noexcept;

		/**
		 * Retrieves the current HP of the tile.
		 * @return The current HP.
		 */
		int getHP() const noexcept;

		/**
		 * Updates the owner of the tile.
		 * If the given pointer is /em expired, this denotes that the tile should not be owned.
		 * @param A pointer to the army which owns the tile.
		 */
		void setOwner(std::weak_ptr<army> newOwner) noexcept;

		/**
		 * Retrieves the current owner of the tile.
		 * If the returned pointer is /em expired, this denotes that the tile is not owned.
		 * @return The army ID of the current owner.
		 */
		std::weak_ptr<army> getOwner() const noexcept;

		/**
		 * Allows the client to assign a weak reference to a unit that is occupying this tile.
		 * If the given pointer is /em expired, this denotes that the tile should not be occupied.
		 * @param newUnit The reference to the unit that is currently occupying this tile.
		 */
		void setUnit(std::weak_ptr<awe::unit> newUnit) noexcept;

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
	private:
		/**
		 * Pointer to the tile type's static information.
		 */
		const tile_type* _tileType = nullptr;

		/**
		 * Weak reference to the army which owns the tile.
		 */
		std::weak_ptr<army> _owner;

		/**
		 * The HP of the tile.
		 */
		int _hp = 0;

		/**
		 * Weak reference to the unit occupying this tile.
		 */
		std::weak_ptr<awe::unit> _unit;
	};
}