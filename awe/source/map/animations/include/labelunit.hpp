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

/**@file labelunit.hpp
 * Defines the animation played when a unit is labelled.
 */

#pragma once

#include "renderer.hpp"
#include "unit.hpp"

namespace awe {
	/**
	 * The animation played when a unit is labelled.
	 */
	class label_unit : public sfx::animated_drawable {
	public:
		/**
		 * Sets up the label unit animation.
		 * @param unit       The unit being labelled.
		 * @param unitSprite The sprite of the unit.
		 * @param sheet      The spritesheet containing the label sprites.
		 * @param sprite     The label sprite to use.
		 * @param leftSide   \c TRUE if the label should be on the left side of the
		 *                   unit, \c FALSE if it should be on the right.
		 * @param duration   The animation will finish when these seconds have
		 *                   elapsed.
		 */
		label_unit(const awe::unit& unit,
			const std::shared_ptr<awe::animated_unit>& unitSprite,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet,
			const std::string& sprite, const bool leftSide,
			const float duration = 0.7f);

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the animation to.
		 * @return \c TRUE if the animation has completed, \c FALSE otherwise.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the animation to.
		 * @param states The render states to apply to the animation.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * Pointer to the unit sprite.
		 */
		std::shared_ptr<awe::animated_unit> _unitSprite;

		/**
		 * The label sprite.
		 */
		sfx::animated_sprite _label;

		/**
		 * The final position of the label.
		 */
		sf::Vector2f _finalPosition;

		/**
		 * Should this label be displayed to the unit's left or right side?
		 */
		const bool _leftSide;

		/**
		 * The maximum duration the label should be visible for, in seconds.
		 */
		const float _duration;

		/**
		 * The number of seconds the label should move for at the start of the
		 * animation.
		 */
		static constexpr float MOVE_IN_DURATION = 0.05f;

		/**
		 * The initial X offset the label should have from the final position.
		 */
		static constexpr float INITIAL_OFFSET = 8.0f;
	};
}
