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

/**@file daybegin.hpp
 * Defines the animation played at the beginning of an army's day.
 */

#pragma once

#include "renderer.hpp"
#include "bank.hpp"
#include "language.hpp"

namespace awe {
	/**
	 * The animation played at the beginning of an army's day.
	 */
	class day_begin : public sfx::animated_drawable {
	public:
		/**
		 * Sets up the day begin animation.
		 * @param country   Points to the properties of the country whose turn is
		 *                  starting.
		 * @param day       The day that's starting.
		 * @param translate Lets the animation translate its day string.
		 * @param font      The font to apply to the day text.
		 */
		day_begin(const std::shared_ptr<const awe::country>& country,
			const awe::Day day,
			const std::shared_ptr<engine::language_dictionary>& translate,
			const std::shared_ptr<sf::Font>& font);

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
		 * The different states of this animation.
		 */
		enum class State {
			FadeIn,               //!< Text is fading in.
			Display,              //!< Text is displayed at max alpha.
			FadeOut               //!< Text is fading out.
		} _state = State::FadeIn; //!< Stores the state of this animation.

		/**
		 * Let's keep it simple for now and fade in a day text, then fade it out
		 * again.
		 */
		sf::Text _text;

		/**
		 * Controls the transparency of the day text.
		 */
		float _alpha = 0;

		/**
		 * Caches the country's colour.
		 */
		const sf::Color& _colour;

		/**
		 * The duration of the fades, in seconds.
		 */
		static constexpr float FADE_DURATION = 0.5f;
	};
}
