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

/**@file scroll.hpp
 * Scrolls the map's view to bring a given tile into view.
 */

#pragma once

#include "animation.hpp"
#include "animated_tile.hpp"

namespace awe {
	/**
	 * Used to gradually scroll the view of an \c awe::map to bring a tile into
	 * full view.
	 */
	class scroll : public awe::animation {
	public:
		/**
		 * Sets up the scroll animation.
		 * @param viewOffsetX Reference to the offset the view has in the X
		 *                    direction. If there is no need to scroll in this
		 *                    direction, the optional must be empty.
		 * @param viewOffsetY Reference to the offset the view has in the Y
		 *                    direction. If there is no need to scroll in this
		 *                    direction, the optional must be empty.
		 * @param target      Points to the target being used.
		 * @param end         The global coordinates pointing to the pixel that
		 *                    needs to be visible by the end of the animation.
		 * @param speed       The speed the view scrolls at, in pixels per second.
		 * @param padding     Distances from the edges of the screen. The space
		 *                    between the edges and this distance is considered
		 *                    "not within view."
		 * @param drawCursors \c TRUE if the cursor graphics should still render.
		 */
		scroll(std::optional<float>& viewOffsetX,
			std::optional<float>& viewOffsetY,
			const std::shared_ptr<const sf::RenderTarget>& target,
			const sf::Vector2f& end, const float speed,
			const sf::Vector2f& mapPixelSize, const float scaling,
			const bool drawCursors = false, sf::Vector2f padding =
			{ awe::animated_tile::MIN_WIDTH * 2.0f,
				awe::animated_tile::MIN_HEIGHT * 2.0f });

		/**
		 * Should the cursor graphics remain on the screen whilst this animation is
		 * in progress?
		 * @return \c TRUE if so, \c FALSE if not.
		 */
		inline bool enableCursorGraphics() const override final {
			return _enableCursorGraphics;
		}

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
		 * Reference to the view X offset to update.
		 */
		std::optional<float>& _viewOffsetX;

		/**
		 * Reference to the view Y offset to update.
		 */
		std::optional<float>& _viewOffsetY;

		/**
		 * Speed of the scroll animation, in pixels per second.
		 */
		const float _speed;

		/**
		 * \c TRUE if the cursor graphics should still be drawn if this animation
		 * is playing out.
		 */
		const bool _enableCursorGraphics;

		/**
		 * The remaining distance the view offsets must travel for.
		 * Neither value can go below \c 0.0. Once both reach \c 0.0, the animation
		 * is finished.
		 */
		sf::Vector2f _distance;

		/**
		 * The initial version of \c _distance.
		 */
		sf::Vector2f _firstDistance;
	};
}
