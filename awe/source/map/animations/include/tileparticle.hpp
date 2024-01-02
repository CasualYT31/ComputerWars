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

/**@file tileparticle.hpp
 * Allows you to animate particles on tiles.
 */

#pragma once

#include "renderer.hpp"
#include "animated_tile.hpp"

namespace awe {
	/**
	 * The tile particle animation.
	 * The particle sprite will be animated from start to finish once. Either that,
	 * or 1.5 seconds will have elapsed, before this drawable marks itself as
	 * finished.
	 */
	class tile_particle : public sfx::animated_drawable {
	public:
		/**
		 * Sets up the tile particle animation.
		 * @param tileSprite The sprite of the tile who will have a particle drawn
		 *                   over it.
		 * @param sheet      The spritesheet containing the particle sprite.
		 * @param particle   The ID of the particle sprite.
		 * @param origin     Where the particle sprite is anchored to within the
		 *                   tile sprite. The particle sprite will also share the
		 *                   same position as its origin. This value works like
		 *                   TGUI origin values do.
		 */
		tile_particle(const awe::animated_tile& tileSprite,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet,
			const std::string& particle, const sf::Vector2f& origin);

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
		 * The particle sprite.
		 */
		sfx::animated_sprite _particle;

		/**
		 * The origin (and position) of the particle sprite.
		 */
		const sf::Vector2f _origin;

		/**
		 * If 1.5 seconds have elapsed since the first call to \c animate(),
		 * prematurely finish the animation.
		 */
		sf::Clock _timer;
	};
}
