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
	public:
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
		 * Sets up the static properties of the drawable.
		 */
		army_pane() noexcept;

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
		 * @param location The location property of this army pane.
		 * @sa    \c awe::army_pane::location
		 */
		void setGeneralLocation(const awe::army_pane::location& location) noexcept;
		
		/**
		 * Sets the CO spritesheet to use with this army pane.
		 * @param sheet Pointer to the spritesheet to use with this army pane.
		 */
		void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept;

		/**
		 * Sets the font used with this army pane.
		 * If \c nullptr is given, the call will be ignored.
		 * @param font Pointer to the font to use with this army pane.
		 */
		void setFont(const std::shared_ptr<sf::Font>& font) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * This method queries the army object for updated information, so that any
		 * updates made to the given army object will be reflected in the pane.
		 * @return Always returns \c TRUE.
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
		 * @param target The target to render the army pane to.
		 * @param states The render states to apply to the pane. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Called to animate this pane if it is to the left.
		 * @param target The target which this pane will be drawn upon later.
		 */
		void _animateLeft(const sf::RenderTarget& target) noexcept;

		/**
		 * Called to animate this pane if it is to the right.
		 * @param target The target which this pane will be drawn upon later.
		 */
		void _animateRight(const sf::RenderTarget& target) noexcept;

		/**
		 * The thickness of the background border.
		 */
		static const float _outlineThickness;
		
		/**
		 * Pointer to the army whose information is being drawn in the pane.
		 */
		std::shared_ptr<const awe::army> _army;

		/**
		 * Defines the type of army pane to draw.
		 */
		awe::army_pane::location _location = awe::army_pane::location::Left;

		/**
		 * The translation transform to apply to the pane.
		 */
		sf::Transform _position;

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
		 * Used to cover the background outline that crosses over the circle.
		 */
		sf::RectangleShape _outlineCover;

		/**
		 * Sprite representing the current CO's face.
		 */
		sfx::animated_sprite _co;

		/**
		 * The ID of the CO being drawn.
		 */
		awe::BankID _oldCoSprite = awe::army::NO_ARMY;

		/**
		 * Text displaying the funds that the army obtains.
		 */
		sf::Text _funds;
	};
}