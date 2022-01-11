/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file unit_pane.h
 * Declares the class which allows the client to draw the units within a tile pane.
 */

#include "unit.h"

#pragma once

namespace awe {
	/**
	 * Class used with \c awe::tile_pane to draw information on a single unit.
	 */
	class unit_pane : public sfx::animated_drawable {
	public:
		/**
		 * Sets up the static properties of the internal drawing objects.
		 */
		unit_pane() noexcept;

		/**
		 * Sets the unit that this pane displays information on.
		 * @param unit The unit to draw information on.
		 */
		void setUnit(const awe::unit& unit) noexcept;

		/**
		 * Sets the rect of the pane.
		 * @warning This \b must be called before \c animate()!
		 * @param   rect The position and size of the unit pane.
		 */
		void setRect(const sf::FloatRect& rect) noexcept;

		/**
		 * Sets the icon spritesheet to use with this unit pane.
		 * @param sheet Pointer to the icon spritesheet to use with this unit pane.
		 */
		void setSpritesheet(
			const std::shared_ptr<const sfx::animated_spritesheet>& sheet)
			noexcept;

		/**
		 * Sets the font used with this unit pane.
		 * If \c nullptr is given, the call will be ignored.
		 * @param font Pointer to the font to use with this unit pane.
		 */
		void setFont(const std::shared_ptr<const sf::Font>& font) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * @warning Make sure to call \c setRect() \b before calling this method!
		 * @return  Always returns \c TRUE.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the tile pane to.
		 * @param states The render states to apply to the pane. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Pointer to the unit to draw information on.
		 */
		std::shared_ptr<const awe::unit> _unit = nullptr;

		/**
		 * The position and the size of the unit pane.
		 */
		sf::FloatRect _rect;
		
		/**
		 * The icon of the unit.
		 */
		sfx::animated_sprite _unitIcon;

		/**
		 * The short name of the unit.
		 */
		sf::Text _unitName;

		/**
		 * The HP icon.
		 */
		sfx::animated_sprite _unitHPIcon;

		/**
		 * The fuel icon.
		 */
		sfx::animated_sprite _unitFuelIcon;

		/**
		 * The ammo icon.
		 */
		sfx::animated_sprite _unitAmmoIcon;

		/**
		 * The HP.
		 */
		sf::Text _unitHP;

		/**
		 * The fuel.
		 */
		sf::Text _unitFuel;

		/**
		 * The ammo.
		 */
		sf::Text _unitAmmo;
	};
}