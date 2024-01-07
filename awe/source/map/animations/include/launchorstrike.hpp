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

/**@file launchorstrike.hpp
 * Defines the animation played when launching a sprite from a tile, or striking a
 * tile with a sprite.
 */

#pragma once

#include "animation.hpp"
#include "texture.hpp"
#include "animated_tile.hpp"

namespace awe {
	/**
	 * Launches a sprite from a tile to over the top of the render target, or
	 * strikes a tile with a sprite that translates down from over the top of the
	 * render target.
	 */
	class launch_or_strike : public awe::animation {
	public:
		/**
		 * Sets up the launch or strike animation.
		 * @warning The given sprite is assumed to have a constant height!
		 * @param   launch     \c TRUE if this animation will move the given sprite
		 *                     from the tile centroid to out of the render target,
		 *                     \c FALSE if vice versa.
		 * @param   sprite     The sprite to move.
		 * @param   sheet      The sheet the sprite comes from.
		 * @param   scaling    Reference to the scaling factor currently used by
		 *                     the map. Make sure the reference remains alive
		 * @param   view       Reference to the view currently used by the map.
		 *                     Make sure the reference remains alive during the
		 *                     animation of this drawable!
		 * @param   tileSprite Points to the source/destination tile's sprite.
		 * @param   duration   In seconds, how long the sprite should take to move
		 *                     to its destination.
		 */
		launch_or_strike(const bool launch, const std::string& sprite,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet,
			const float& scaling, const sf::View& view,
			const std::shared_ptr<awe::animated_tile>& tileSprite,
			const float duration = 1.0f);

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
		 * \c TRUE if launching, \c FALSE is striking.
		 */
		const bool _launch;

		/**
		 * Reference to the scaling factor the map is using.
		 */
		const float& _scaling;

		/**
		 * Reference to the view the map is using.
		 */
		const sf::View& _view;

		/**
		 * The duration, in seconds, of this animation.
		 */
		const float _duration;

		/**
		 * Points to the source (if launching) or destination (if striking) tile's
		 * sprite.
		 */
		std::shared_ptr<awe::animated_tile> _tileSprite;

		/**
		 * The sprite that is launching from/striking a tile.
		 */
		sfx::animated_sprite _movingSprite;

		/**
		 * The speed (and direction) of the moving sprite, in pixels per second.
		 */
		float _speed = 0.0f;

		/**
		 * The destination of the moving sprite along the Y axis.
		 */
		float _destinationY = 0.0f;
	};
}
