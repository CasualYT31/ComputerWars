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

/**@file animated_unit.hpp
 * Declares the class which represents an in-game unit sprite.
 */

#include "renderer.hpp"
#include "texture.hpp"

#pragma once

namespace awe {
	/**
	 * Class which represents a single unit sprite on a map.
	 */
	class animated_unit : public sfx::animated_drawable {
	public:
		/**
		 * Creates a new unit sprite.
		 * @param data The data used to initialise the animated sprite's logger
		 *             object.
		 */
		animated_unit(const engine::logger::data& data);

		/**
		 * Sets the spritesheet to use with this unit.
		 * @param  sheet Pointer to the spritesheet to use with this unit.
		 * @safety No guarantee.
		 */
		inline void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
			_sprite.setSpritesheet(sheet);
		}

		/**
		 * Sets the icon spritesheet to use with this unit.
		 * @param  sheet Pointer to the icon spritesheet to use with this unit.
		 * @safety No guarantee.
		 */
		void setIconSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Gets the spritesheet used with this unit.
		 * @return Pointer to the spritesheet used with this unit.
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
			_sprite.setSprite(spriteID);
		}

		/**
		 * Finds out the sprite name used with this unit's internal sprite.
		 * @return The name of the sprite from the spritesheet used with this unit.
		 */
		inline std::string getSprite() const {
			return _sprite.getSprite();
		}

		/**
		 * Sets the sprite to display in the HP icon.
		 * @param spriteID The ID of the sprite from the icon spritesheet to use.
		 */
		inline void setHPIconSprite(const std::string& spriteID) {
			_hpIcon.setSprite(spriteID);
		}

		/**
		 * Sets the sprite to display in the fuel/ammo icon.
		 * @param spriteID The ID of the sprite from the icon spritesheet to use.
		 */
		inline void setFuelAmmoIconSprite(const std::string& spriteID) {
			_fuelAmmoIcon.setSprite(spriteID);
		}

		/**
		 * Sets the sprite to display in the loaded icon.
		 * @param spriteID The ID of the sprite from the icon spritesheet to use.
		 */
		inline void setLoadedIconSprite(const std::string& spriteID) {
			_loadedIcon.setSprite(spriteID);
		}

		/**
		 * Sets the sprite to display in the capturing/hiding icon.
		 * @param spriteID The ID of the sprite from the icon spritesheet to use.
		 */
		inline void setCapturingHidingIconSprite(const std::string& spriteID) {
			_capturingHidingIcon.setSprite(spriteID);
		}

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
		 * Calls the internal sprite's \c animate() method. Also determines which
		 * icons to display and animates them.
		 * @remark Idea for future optimisation: move \c setSprite() calls to other
		 *         methods, e.g. move \c _loadedIcon.setSprite() to \c loadUnit().
		 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
		 *         call.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws the unit to the screen along with any icons it should display.
		 * @param target The target to render the tile to.
		 * @param states The render states to apply to the tile. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * The unit's animated sprite object.
		 */
		sfx::animated_sprite _sprite;

		/**
		 * The unit's HP icon sprite object.
		 */
		sfx::animated_sprite _hpIcon;

		/**
		 * The unit's fuel and ammo shortage icon sprite object.
		 */
		sfx::animated_sprite _fuelAmmoIcon;

		/**
		 * The unit's loaded icon sprite object.
		 */
		sfx::animated_sprite _loadedIcon;

		/**
		 * The unit's capturing and hiding icon sprite object.
		 */
		sfx::animated_sprite _capturingHidingIcon;
	};
}
