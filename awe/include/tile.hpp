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

/**@file tile.hpp
 * Declares the class which represents an in-game tile.
 */

#include "army.hpp"
#include "animated_tile.hpp"

#pragma once

namespace awe {
	/**
	 * Class which represents a single tile on a map.
	 */
	class tile {
	public:
		/**
		 * Construct a new tile with a given type.
		 * @param animatedUnit   Pointer to this tile's animated sprite.
		 * @param spriteCallback When an update to the tile's sprite is required,
		 *                       this callback is to be invoked. The function that
		 *                       will perform operations on the animated tile must
		 *                       be given.
		 * @param type           The type of tile to create. \c nullptr if you
		 *                       don't wish to provide a type at this time.
		 * @param owner          The owner of the tile. \c NO_ARMY represents no
		 *                       owner.
		 * @param sheet          Pointer to the spritesheet to use with this tile.
		 */
		tile(const std::shared_ptr<awe::animated_tile>& animatedTile,
			const std::function<void(const std::function<void(void)>&)>&
				spriteCallback,
			const std::shared_ptr<const awe::tile_type>& type = nullptr,
			const awe::ArmyID owner = awe::NO_ARMY,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr);

		/**
		 * Update's the tile's type.
		 * This method does not handle any references to this tile in \c army
		 * objects. See \c map::setTileType().
		 * @param  type The type to give to the tile.
		 * @safety No guarantee.
		 */
		void setTileType(const std::shared_ptr<const awe::tile_type>& type);

		/**
		 * Returns the tile's type.
		 * @return Information on the tile's type.
		 */
		inline std::shared_ptr<const awe::tile_type> getTileType() const {
			return _type;
		}

		/**
		 * Sets the owner of this tile.
		 * Set this to <tt>awe::army::NO_ARMY</tt> to set no owner.
		 * @param  owner The army ID of the owner of this tile.
		 * @safety No guarantee.
		 */
		void setTileOwner(const awe::ArmyID owner);

		/**
		 * Retrieves the army ID of the army who owns this tile.
		 * @return The army ID of the owning army. \c awe::army::NO_ARMY if the
		 *         tile isn't owned.
		 */
		inline awe::ArmyID getTileOwner() const noexcept {
			return _owner;
		}

		/**
		 * Sets the tile's HP.
		 * If a negative number is given, it will be adjusted to \c 0.
		 * @param hp The HP to set to the tile.
		 */
		inline void setTileHP(const awe::HP hp) noexcept {
			_hp = std::max(0, hp);
		}

		/**
		 * Retrieves the tile's HP.
		 * @return The HP of the tile.
		 */
		inline awe::HP getTileHP() const noexcept {
			return _hp;
		}

		/**
		 * Sets the unit currently occupying the tile.
		 * @param id The ID of the unit now occupying the tile. \c NO_UNIT
		 *           indicates this tile should be vacant.
		 */
		inline void setUnit(const awe::UnitID id) noexcept {
			_unit = id;
		}

		/**
		 * Retrieves the ID of the unit currently occupying the tile.
		 * @return The ID of the unit occupying the tile. \c 0 if the tile is
		 *         vacant.
		 */
		inline awe::UnitID getUnit() const noexcept {
			return _unit;
		}

		/**
		 * Define if this tile forms part of a structure or not.
		 * @param structure Pointer to the type of structure that this tile helps
		 *                  form. If \c nullptr, this tile is not a member of a
		 *                  structure.
		 */
		inline void setStructureType(
			const std::shared_ptr<const awe::structure>& structure) {
			_structure = structure;
		}

		/**
		 * Gets the type of structure that this tile helps form, if any.
		 * @return If this tile forms part of a structure, the type of structure is
		 *         returned. Otherwise, \c nullptr.
		 */
		inline std::shared_ptr<const awe::structure> getStructureType() const {
			return _structure;
		}

		/**
		 * Stores which tile from a structure this tile is.
		 * @param offset The offset from the root tile of the structure. Can be
		 *               <tt>(0, 0)</tt> to represent the root tile.
		 */
		inline void setStructureTile(const sf::Vector2i offset) {
			_offset = offset;
		}

		/**
		 * Gets this tile's offset from the root tile of the structure it's a part
		 * of.
		 * Should be ignored if this tile doesn't form part of a structure.
		 * @return If this tile is the root tile, returns <tt>(0, 0)</tt>,
		 *         otherwise, returns the offset from the root tile, in tiles.
		 */
		inline sf::Vector2i getStructureTile() const {
			return _offset;
		}

		/**
		 * Updates the destroyed flag on this tile.
		 * @param isDestroyed Set to \c TRUE if this tile forms part of a structure
		 *                    that's destroyed. \c FALSE otherwise.
		 */
		inline void setStructureDestroyed(const bool isDestroyed) {
			_destroyed = isDestroyed;
		}

		/**
		 * Retrieves the destroyed flag in this tile.
		 * @return \c TRUE or \c FALSE.
		 */
		inline bool getStructureDestroyed() const {
			return _destroyed;
		}

		/**
		 * Updates the visibility status of this tile.
		 * @param visible \c TRUE if the tile is visible to the current army,
		 *                \c FALSE if it is hidden.
		 */
		void setVisibility(const bool visible);

		/**
		 * Updates the sprite ID to use with this tile based on its type, owner,
		 * and visibility.
		 */
		void updateSpriteID();
	private:
		/**
		 * Same as \c updateSpriteID() except the sprite changes are not given to
		 * the callback but are applied immediately.
		 */
		void _updateSpriteID(const std::weak_ptr<awe::animated_tile>& _tileSprite,
			const awe::ArmyID owner,
			const std::shared_ptr<const awe::tile_type>& type, const bool visible);

		/**
		 * The type of this tile.
		 */
		std::shared_ptr<const awe::tile_type> _type;

		/**
		 * The army ID of the owner of the tile.
		 */
		awe::ArmyID _owner = awe::NO_ARMY;

		/**
		 * The tile's HP.
		 */
		awe::HP _hp = 0;

		/**
		 * The ID of the unit currently occupying the tile.
		 * \c NO_UNIT indicates vacancy.
		 */
		awe::UnitID _unit = awe::NO_UNIT;

		/**
		 * If this tile forms part of a structure, this points to the type of
		 * structure.
		 */
		std::shared_ptr<const awe::structure> _structure;

		/**
		 * This tile's offset from the root tile of the structure, in tiles.
		 */
		sf::Vector2i _offset;

		/**
		 * Does this tile form part of a destroyed structure?
		 */
		bool _destroyed = false;

		/**
		 * Is this tile visible to the current army?
		 */
		bool _visible = true;

		/**
		 * Weak pointer to this tile's animated sprite.
		 */
		std::weak_ptr<awe::animated_tile> _tileSprite;

		/**
		 * Callback to be invoked when a change is to be made to \c _tileSprite.
		 */
		std::function<void(std::function<void(void)>)> _updateSprite;
	};
}
