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

/**@file transitions.hpp
 * Classes used for drawing transitions to the screen.
 * Each class is designed to be single-use-then-discard. A transition is allocated
 * (either on the heap or the stack), then animated and drawn. Once \c animate()
 * returns \c TRUE, the program is to then carry out its next instructions. Each
 * transition object has been implemented in a way that causes it to animate and
 * draw only \e once in its lifetime, even if the original drawing loop isn't
 * amended to stop drawing the transition once it has completed.
 */

#pragma once

#include "renderer.hpp"

namespace transition {
	/**
	 * Defines the blueprint all transitions should conform to.
	 * All transitions should be single-use animations that do not repeat.
	 * Therefore, basic properties of transitions cannot be amended after
	 * construction.
	 */
	class base : public sfx::animated_drawable {
	public:
		/**
		 * Sets the basic properties of the transition.
		 * @param isFadingIn If \c TRUE, the transition should be fading \em in
		 *                   (that is, the transition should be used when the
		 *                   client wishes to fade the screen into view). If
		 *                   \c FALSE, the transition should be fading \em out
		 *                   (that is, the transition should be used when the
		 *                   client wishes to fade the screen out of view).
		 * @param duration   The approximate duration of the transition, from start
		 *                   to finish.
		 */
		base(const bool isFadingIn, const sf::Time& duration = sf::seconds(1.0f))
			noexcept;

		/**
		 * Finds out if this transition should fade in or fade out.
		 * @return \c TRUE if the transition should fade in, \c FALSE if it should
		 *         fade out.
		 * @sa     sfx::base::base()
		 */
		bool isFadingIn() const noexcept;

		/**
		 * Finds out the duration of this transition.
		 * @return The approximate duration of this transition.
		 * @sa     sfx::base::base()
		 */
		sf::Time duration() const noexcept;
	private:
		/**
		 * Stores the fade-in property of this transition.
		 */
		bool _isFadingIn;

		/**
		 * Stores the duration property of this transition.
		 */
		sf::Time _duration;
	};

	/**
	 * This transition uses two rectangles which grow or shrink from/to the upper
	 * left and lower right corners of the screen.
	 * @sa sfx::animated_drawable
	 * @sa sfx::base
	 */
	class rectangle : public transition::base {
	public:
		/**
		 * Sets the transition up, ready for drawing.
		 * @param isFadingIn The fade in property of this transition.
		 * @param duration   The duration of this transition.
		 * @param colour     The colour of the two rectangles.
		 * @sa    sfx::base::base()
		 */
		rectangle(const bool isFadingIn, const sf::Time& duration = sf::seconds(1),
			const sf::Color& colour = sf::Color()) noexcept;
		
		/**
		 * This drawable's \c animate() method.
		 * Performs all the calculations on the rectangle shapes.
		 * @param  target  The target to render the transition to.
		 * @param  scaling Scaling applied to the transition.
		 * @return \c TRUE if the transition has completed, \c FALSE otherwise.
		 * @sa     transitions.hpp
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Draws two rectangles to the screen. They will not move if the
		 * \c animate() method isn't called, so remember to call it before drawing!
		 * @param target The target to render the transition to.
		 * @param states The render states to apply to the transition. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		
		/**
		 * Tracks the size of both rectangles.
		 */
		sf::Vector2f _size;

		/**
		 * The rectangle which grows from or shrinks to the upper left corner.
		 */
		sf::RectangleShape _toprect;
		
		/**
		 * The rectangle which grows from or shrinks to the lower right corner.
		 */
		sf::RectangleShape _bottomrect;
	};
}
