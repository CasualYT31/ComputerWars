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

/**@file tagcos.hpp
 * Defines the animation played when an army's COs have been tagged.
 */

#pragma once

#include "animation.hpp"
#include "bank-v2.hpp"
#include "language.hpp"
#include "texture.hpp"

namespace awe {
	/**
	 * The animation played when an army's COs have been tagged.
	 */
	class tag_cos : public awe::animation {
	public:
		/**
		 * Sets up the tag COs animation.
		 * @param country      Points to the properties of the country whose COs
		 *                     are tagging.
		 * @param oldCurrentCO Points to the commander who is being made the tag
		 *                     CO.
		 * @param newCurrentCO Points to the commander who is being made the
		 *                     current CO.
		 * @param coSheet      Points to the CO spritesheet.
		 * @param translate    Lets the animation translate its tag string.
		 * @param font         The font to apply to the tag text.
		 */
		tag_cos(const engine::CScriptWrapper<awe::country_view>& country,
			const engine::CScriptWrapper<awe::commander_view>& oldCurrentCO,
			const engine::CScriptWrapper<awe::commander_view>& newCurrentCO,
			const std::shared_ptr<sfx::animated_spritesheet>& coSheet,
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
		 * Updates a drawable's position and origin.
		 * @tparam T        The type of the drawable.
		 * @param  drawable The drawable to update.
		 * @param  pos      The position to apply to the drawable.
		 * @param  originX  The X coordinate of the origin to apply to the
		 *                  drawable.
		 * @param  originY  The Y coordinate of the origin to apply to the
		 *                  drawable.
		 */
		template<typename T>
		void _updateDrawablePosition(T& drawable, const sf::Vector2f& pos,
			const float originX = 0.0f, const float originY = 0.0f) {
			drawable.setOrigin(sf::Vector2f(originX, originY));
			drawable.setPosition(pos);
		}

		/**
		 * Let's keep it simple for now and fade in a tag text, then fade it out
		 * again.
		 */
		sf::Text _text;

		/**
		 * The portrait of the old current CO.
		 */
		sfx::animated_sprite _oldCurrentCO;

		/**
		 * The portrait of the new current CO.
		 */
		sfx::animated_sprite _newCurrentCO;

		/**
		 * Controls the transparency of the tag text.
		 */
		float _alpha = 0;

		/**
		 * Cache of the target width used to correct portrait X coordinates if the
		 * target resizes.
		 */
		float _targetWidthCache = 0.0f;

		/**
		 * Caches the country's colour.
		 */
		const sf::Color& _colour;

		/**
		 * The duration of the text fades, in seconds.
		 */
		static constexpr float FADE_DURATION = 0.2f;

		/**
		 * The amount of time CO portraits move fast, in seconds.
		 * Portraits move fast, then slow, then fast again.
		 */
		static constexpr float FAST_DURATION = 1.0f;

		/**
		 * The approximate duration of the animation, in seconds.
		 */
		static constexpr float DURATION = 2.5f;

		/**
		 * The fastest speed the CO portraits move at, as a percentage of the
		 * target's width in pixels, per second.
		 */
		static constexpr float FASTEST = 0.5f;

		/**
		 * The slowest speed the CO portraits move at, as a percentage of the
		 * target's width in pixels, per second.
		 */
		static constexpr float SLOWEST = 0.1f;
	};
}
