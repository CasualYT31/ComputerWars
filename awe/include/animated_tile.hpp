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

/**@file animated_tile.hpp
 * Declares the class which represents an in-game tile sprite.
 */

#include "renderer.hpp"
#include "texture.hpp"

#pragma once

namespace awe {
	/**
	 * Class which represents a single tile sprite on a map.
	 */
	class animated_tile : public sfx::animated_drawable {
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
		 * Creates a new tile sprite.
		 * @param data The data used to initialise the animated sprite's logger
		 *             object.
		 */
		animated_tile(const engine::logger::data& data);

		/**
		 * Sets the spritesheet to use with this tile.
		 * @param  sheet Pointer to the spritesheet to use with this tile.
		 * @safety No guarantee.
		 */
		inline void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
			_sprite.setSpritesheet(sheet);
		}

		/**
		 * Gets the spritesheet used with this tile.
		 * @return Pointer to the spritesheet used with this tile.
		 */
		inline std::shared_ptr<const sfx::animated_spritesheet>
			getSpritesheet() const {
			return _sprite.getSpritesheet();
		}

		/**
		 * Sets the sprite of this unit.
		 * @param spriteID The ID of the sprite from \c setSpritesheet()'s sheet to
		 *                 use.
		 */
		inline void setSprite(const std::string& spriteID) {
			if (_oldSprite) _oldSprite = spriteID;
			else _sprite.setSprite(spriteID);
		}

		/**
		 * Finds out the sprite name used with this unit's internal sprite.
		 * @return The name of the sprite from the spritesheet used with this unit.
		 */
		inline std::string getSprite() const {
			return _sprite.getSprite();
		}

		/**
		 * Sets this tile's sprite override.
		 * @param spriteID ID of the sprite to assign.
		 * @sa    \c _oldSprite.
		 * @sa    \c _targetCache.
		 */
		void setSpriteOverride(const std::string& spriteID);

		/**
		 * Clears this tile's sprite override.
		 * @sa \c _oldSprite.
		 * @sa \c _targetCache.
		 */
		void clearSpriteOverride();

		/**
		 * Sets the unit sprite's pixel position.
		 * @param x The X position of the unit sprite.
		 * @param y The Y position of the unit sprite.
		 */
		inline void setPixelPosition(float x, float y) {
			_sprite.setPosition(sf::Vector2f(x, y));
		}

		/**
		 * Gets the sprite's pixel position.
		 * @return The pixel position of the unit.
		 */
		inline sf::Vector2f getPixelPosition() const {
			return _sprite.getPosition();
		}

		/**
		 * Gets the sprite's pixel size.
		 * @return The pixel size of the unit.
		 */
		inline sf::Vector2f getPixelSize() const {
			return _sprite.getSize();
		}

		/**
		 * This drawable's \c animate() method.
		 * Calls the internal sprite's \c animate() method.
		 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
		 *         call.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws the tile to the screen.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * Cache the render target last given to \c animate() so the caller does
		 * not have to manually animate sprites when setting and clearing the
		 * sprite override.
		 */
		const sf::RenderTarget* _targetCache = nullptr;

		/**
		 * The tile's animated sprite object.
		 */
		sfx::animated_sprite _sprite;

		/**
		 * The tile's sprite as set via \c setSprite().
		 * When a tile sprite override is given, its previous sprite will be stored
		 * here. If \c setSprite() is called whilst the override is still active,
		 * the ID given will be stored here instead. Then, when the override is
		 * removed, \c setSprite() will be used to reinstate the last given sprite
		 * to \c setSprite(). If two sprite overrides are given back-to-back, then
		 * the second override will not cause the first override to be stored in
		 * here.
		 */
		std::optional<std::string> _oldSprite;
	};
}
