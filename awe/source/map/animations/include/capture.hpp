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

/**@file capture.hpp
 * Allows you to animate captures.
 */

#pragma once

#include "animation.hpp"
#include "animated_tile.hpp"
#include "language.hpp"
#include "typedef.hpp"

namespace awe {
	/**
	 * The property capture animation.
	 */
	class capture : public awe::animation {
	public:
		/**
		 * Sets up the capture animation.
		 */
		capture(const std::shared_ptr<sfx::animated_spritesheet>& sheet,
			const std::string& background, const std::string& oldProperty,
			const std::string& newProperty, const std::string& capturing,
			const std::string& captured, const std::string capturedText,
			const std::shared_ptr<engine::language_dictionary>& dict,
			const awe::HP oldHP, const awe::HP newHP, const unsigned int maxHP,
			const awe::animated_tile& tileSprite,
			const std::shared_ptr<sf::Font>& hpTextFont,
			const std::shared_ptr<sf::Font>& capturedTextFont);

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
			Intro,               //!< Let the unit finish its capturing animation.
			Wait,                //!< Wait for a short period of time.
			Falling,             //!< Scale the property down now.
			Wait2,               //!< Wait for a second time.
			Captured,            //!< Property has been captured, scale it back up.
			Wait3                //!< Wait for a final time.
		} _state = State::Intro; //!< Stores the state of this animation.

		/**
		 * Updates the state of this animation.
		 * @param newState The new state of the animation.
		 */
		void setState(const State newState);

		/**
		 * The background sprite.
		 */
		sfx::animated_sprite _background;

		/**
		 * The property sprite.
		 */
		sfx::animated_sprite _property;

		/**
		 * The capturing unit sprite.
		 */
		sfx::animated_sprite _unit;

		/**
		 * The text displaying the HP of the property.
		 */
		sf::Text _hpText;

		/**
		 * The text displayed when the property has been captured.
		 */
		sf::Text _capturedText;

		/**
		 * The property's HP.
		 */
		float _hp;

		/**
		 * The property's old HP.
		 */
		const float _oldHP;

		/**
		 * The property's new HP.
		 */
		const float _newHP;

		/**
		 * The property's maximum HP.
		 */
		const float _maxHP;

		/**
		 * Set this sprite to \c _property when the property has been captured.
		 */
		const std::string _capturedProperty;

		/**
		 * Set this sprite to \c _unit when the property has been captured.
		 */
		const std::string _capturedUnit;

		/**
		 * Duration of the \c Wait state, in seconds.
		 */
		static constexpr float WAIT_DURATION = 0.2f;

		/**
		 * Duration of the \c Falling state, in seconds.
		 */
		static constexpr float FALLING_DURATION = 0.5f;

		/**
		 * Duration of the \c Wait2 state, in seconds.
		 */
		static constexpr float WAIT2_DURATION = 0.2f;

		/**
		 * Duration of the \c Captured state, in seconds.
		 */
		static constexpr float CAPTURED_DURATION = 0.5f;

		/**
		 * Duration of the \c Wait3 state, in seconds.
		 */
		static constexpr float WAIT3_DURATION = 0.2f;
	};
}
