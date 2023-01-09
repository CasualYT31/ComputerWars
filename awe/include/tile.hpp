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

/**@file tile.hpp
 * Declares the class which represents an in-game tile.
 */

#include "army.hpp"

#pragma once

namespace awe {
	/**
	 * Class which represents a single tile on a map.
	 */
	class tile : public sfx::animated_drawable {
	public:
		/**
		 * The minimum width a rendered tile can be, in pixels.
		 */
		static const sf::Uint32 MIN_WIDTH = 16;

		/**
		 * The minimum height a rendered tile can be, in pixels.
		 */
		static const sf::Uint32 MIN_HEIGHT = 16;

		/**
		 * Construct a new tile with a given type.
		 * @param data  Data used to initialise the sprite's logger object.
		 * @param type  The type of tile to create.
		 *              \c nullptr if you don't wish to provide a type at this
		 *              time.
		 * @param sheet Pointer to the spritesheet to use with this tile.
		 */
		tile(const engine::logger::data& data,
			const std::shared_ptr<const awe::tile_type>& type = nullptr,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr)
			noexcept;

		/**
		 * Update's the tile's type.
		 * This method does not handle any references to this tile in \c army
		 * objects. See \c map::setTileType().
		 * @param type The type to give to the tile.
		 */
		void setTileType(const std::shared_ptr<const awe::tile_type>& type)
			noexcept;

		/**
		 * Returns the tile's type.
		 * @return Information on the tile's type.
		 */
		std::shared_ptr<const awe::tile_type> getTileType() const noexcept;

		/**
		 * Sets the owner of this tile.
		 * Set this to <tt>awe::army::NO_ARMY</tt> to set no owner.
		 * @param owner The army ID of the owner of this tile.
		 */
		void setTileOwner(const awe::ArmyID owner) noexcept;

		/**
		 * Retrieves the army ID of the army who owns this tile.
		 * @return The army ID of the owning army. \c awe::army::NO_ARMY if the
		 *         tile isn't owned.
		 */
		awe::ArmyID getTileOwner() const noexcept;

		/**
		 * Sets the tile's HP.
		 * If a negative number is given, it will be adjusted to \c 0.
		 * @param hp The HP to set to the tile.
		 */
		void setTileHP(const awe::HP hp) noexcept;

		/**
		 * Retrieves the tile's HP.
		 * @return The HP of the tile.
		 */
		awe::HP getTileHP() const noexcept;

		/**
		 * Sets the unit currently occupying the tile.
		 * @param id The ID of the unit now occupying the tile. \c 0 indicates this
		 *           tile should be vacant.
		 */
		void setUnit(const awe::UnitID id) noexcept;

		/**
		 * Retrieves the ID of the unit currently occupying the tile.
		 * @return The ID of the unit occupying the tile. \c 0 if the tile is
		 *         vacant.
		 */
		awe::UnitID getUnit() const noexcept;

		/**
		 * Sets the spritesheet to use with this tile.
		 * @param sheet Pointer to the spritesheet to use with this tile.
		 */
		void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept;

		/**
		 * Gets the spritesheet used with this tile.
		 * @return Pointer to the spritesheet used with this tile.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> getSpritesheet() const
			noexcept;

		/**
		 * Finds out the sprite name used with this tile's internal sprite.
		 * @return The name of the sprite from the spritesheet used with this tile.
		 */
		std::string getSprite() const noexcept;

		/**
		 * Sets the tile's pixel position to the internal sprite.
		 * @param x The X position of the tile.
		 * @param y The Y position of the tile.
		 */
		void setPixelPosition(float x, float y) noexcept;

		/**
		 * Gets the tile's current pixel position.
		 * @return The tile's pixel position.
		 */
		sf::Vector2f getPixelPosition() const noexcept;

		/**
		 * Gets the tile's current pixel size.
		 * @return The tile's pixel size.
		 */
		sf::Vector2f getPixelSize() const noexcept;

		/**
		 * This drawable's \c animate() method.
		 * Simply calls the internal sprite's \c animate() method.
		 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
		 *         call.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Simply draws \c _sprite to the screen.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Updates the sprite ID to use with this tile.
		 * This can change in a variety of different circumstances, and there are
		 * quite a few conditions to check for, so it's best to keep it all in one
		 * method and call it where necessary.
		 */
		void _updateSpriteID() noexcept;

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
		 * \c 0 indicates vacancy.
		 */
		awe::UnitID _unit = 0;

		/**
		 * This tile's animated sprite object.
		 */
		sfx::animated_sprite _sprite;
	};
}
