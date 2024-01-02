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

/**@file zoom.hpp
 * Defines the animation played when zooming in or out.
 */

#pragma once

#include "renderer.hpp"

namespace awe {
	/**
	 * The animation played when the map's scaling factor is adjusted.
	 * A simple mechanism via which the map's scaling factor is gradually increased
	 * or decreased.
	 */
	class zoom : public sfx::animated_drawable {
	public:
		/**
		 * Sets up the zoom animation.
		 * @param scalingToUpdate Points to the scaling value to update.
		 * @param newScaleValue   The final value that should be assigned to
		 *                        \c scalingToUpdate.
		 * @param duration        The duration of the zoom animation, in seconds.
		 */
		zoom(float& scalingToUpdate, const float newScaleValue,
			const float duration = 0.1f);

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
		 * Reference to the scaling value that's being updated.
		 */
		float& _scaling;

		/**
		 * The value we're moving away from.
		 */
		const float _scaleFrom;

		/**
		 * The value to eventually set \c _scaling to.
		 */
		const float _scaleTo;

		/**
		 * The duration of the zoom animation, in seconds.
		 */
		const float _duration;
	};
}
