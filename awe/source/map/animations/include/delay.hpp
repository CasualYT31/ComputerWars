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

/**@file delay.hpp
 * Allows the animation queue to be delayed.
 */

#pragma once

#include "animation.hpp"

namespace awe {
	/**
	 * Allows one to insert a delay into the animation queue that may or may not be
	 * skipped.
	 */
	class delay : public awe::animation {
	public:
		/**
		 * Sets up the delay animation.
		 * @param duration The duration of the delay, in seconds.
		 */
		inline delay(const float duration, const bool skippable = false) :
			_duration(duration), _skippable(skippable) {}

		/**
		 * Can this delay be skipped by the user?
		 * @return \c TRUE if so, \c FALSE if not.
		 */
		inline bool isSkippable() const override final {
			return _skippable;
		}

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the animation to.
		 * @return \c TRUE if the animation has completed, \c FALSE otherwise.
		 */
		inline bool animate(const sf::RenderTarget& target) final {
			return accumulatedDelta() >= _duration;
		}
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the animation to.
		 * @param states The render states to apply to the animation.
		 */
		inline void draw(sf::RenderTarget& target,
			sf::RenderStates states) const final {}

		/**
		 * The duration of the delay, in seconds.
		 */
		const float _duration;

		/**
		 * Is this delay skippable by the user?
		 */
		const bool _skippable;
	};
}
