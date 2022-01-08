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

/**@file army_pane.h
 * Declares the class which allows the client to draw the army pane of an army.
 */

#include "army.h"

#pragma once

namespace awe {
	/**
	 * Class which represents an army pane.
	 * I decided to keep this functionality separate from the \c army class to save
	 * memory. Since there would only ever be one army pane drawn at a time, I saw
	 * it as needlessly wasteful to store sprites and drawing shapes for each and
	 * every army (there could be as many as there are countries available!), when
	 * I could just store it all once and update it as required.
	 */
	class army_pane : public sfx::animated_drawable {
		/**
		 * Defines the different types of army pane that can be drawn.
		 * The pane can either be drawn onto the left side of a target, or onto the
		 * right side. These values are intended to be used with
		 * \c setGeneralLocation() to determine if the pane should be rounded on
		 * the left or right.
		 * @sa setGeneralLocation()
		 * @sa draw()
		 */
		enum class location {
			Left,
			Right
		};

		/**
		 * Sets this pane's army.
		 * A shared pointer is created from the given object, so even if it goes
		 * out of scope the army pane should still function as if it existed.
		 * @param army The army whose information is to be drawn onto this pane.
		 */
		void setArmy(const awe::army& army) noexcept;

		/**
		 * Sets this pane's general location.
		 * The default location of an army pane is left.
		 * @param location The location property of this army.
		 * @sa    \c awe::army_pane::location
		 */
		void setGeneralLocation(const awe::army_pane::location& location) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * Used to set the CO sprite: it does not actually animate the sprite. In
		 * the future, this will also animate the CO power meter.\n
		 * This method also queries the army object for updated information.
		 * @return Always returns \c TRUE. This will change in the future.
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * This will draw a background shape with the colour of the army's country,
		 * the funds that the army possesses, and the current (i.e. not tag) CO's
		 * face sprite. In the future, the CO power meter will also be drawn.\n
		 * If the pane is drawn to the left side of the target, the right side of
		 * the pane will be rounded. If the pane is drawn to the right side of the
		 * target, the left side of the pane will be rounded.
		 * @remark To position this army pane onto the screen, use the \c states
		 *         parameter.
		 * @param  target The target to render the army pane to.
		 * @param  states The render states to apply to the tile. Applying
		 *                transforms is perfectly valid and will not alter the
		 *                internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Called to animate this pane if it is to the left.
		 */
		void _animateLeft() noexcept;

		/**
		 * Called to animate this pane if it is to the right.
		 */
		void _animateRight() noexcept;
		
		/**
		 * Pointer to the army whose information is being drawn in the pane.
		 */
		std::shared_ptr<const awe::army> _army;

		/**
		 * Defines the type of army pane to draw.
		 */
		awe::army_pane::location _location = awe::army_pane::location::Left;

		/**
		 * The rectangle of the pane.
		 */
		sf::RectangleShape _bg;

		/**
		 * The circle of the pane.
		 * Makes either the left or right side of the pane rounded when drawn.
		 */
		sf::CircleShape _rounded_bg;

		/**
		 * Sprite representing the current CO's face.
		 */
		sfx::animated_sprite _co;

		/**
		 * Text displaying the funds that the army obtains.
		 */
		sf::Text _funds;
	};
}