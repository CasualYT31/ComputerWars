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

/**@file nextturn.hpp
 * Allows the game to separate turns if Fog of War is enabled.
 */

#pragma once

#include "animation.hpp"
#include "userinput.hpp"
#include "texture.hpp"
#include "bank-v2.hpp"
#include "language.hpp"

namespace awe {
	/**
	 * Produces a "next turn" screen that the next player has to close before they
	 * can begin their turn.
	 */
	class next_turn : public awe::animation {
	public:
		/**
		 * Sets up the next turn animation.
		 * @param country       The country who will be having their turn next.
		 * @param nextTurnLabel The "next turn" label.
		 * @param controls      The controls that allow the player to close the
		 *                      screen.
		 * @param ui            Pointer to the \c user_input object to accept input
		 *                      with.
		 * @param dict          The language dictionary to use.
		 * @param sheet         The spritesheet containing the country's icon
		 *                      graphic.
		 * @param font          The font to apply to all of the text in this
		 *                      animation.
		 * @param code          Code invoked once this animation enters the
		 *                      \c TransitionOut state.
		 */
		next_turn(const engine::CScriptWrapper<awe::country_view>& country,
			const std::string& nextTurnLabel,
			const std::unordered_set<std::string>& controls,
			const std::shared_ptr<sfx::user_input>& ui,
			const std::shared_ptr<engine::language_dictionary>& dict,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet,
			const std::shared_ptr<sf::Font>& font,
			const std::function<void(void)>& code);

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
		enum class state {
			TransitionIn, //!< Black circle spreads out from the centre.
			WaitForInput, //!< Wait for the user to input a control.
			TransitionOut //!< Alpha circle spreads out from the centre.
		} _state = state::TransitionIn; //!< The state of the animation.

		/**
		 * The duration of the circle transitions, in seconds.
		 */
		static constexpr float TRANSITION_DURATION = 1.f;

		/**
		 * The controls that allow the player to close the screen.
		 */
		const std::unordered_set<std::string> _controls;

		/**
		 * The user input object to accept input with.
		 */
		const std::shared_ptr<sfx::user_input> _ui;

		/**
		 * Invoke this once the \c TransitionOut state begins.
		 */
		const std::function<void(void)> _code;

		/**
		 * The circle used for the transition.
		 */
		sf::CircleShape _circle;

		/**
		 * The transition render texture.
		 */
		sf::RenderTexture _transition;
		
		/**
		 * Displays the country's icon.
		 */
		sfx::animated_sprite _countryIcon;

		/**
		 * Displays the country's long name.
		 */
		sf::Text _countryName;
		
		/**
		 * Displays the "next turn" label.
		 */
		sf::Text _nextTurnLabel;
	};
}
