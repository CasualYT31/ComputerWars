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

/**@file tileparticles.hpp
 * Allows you to animate particles on tiles.
 */

#pragma once

#include "animation.hpp"
#include "animated_tile.hpp"
#include "script.hpp"

namespace awe {
	/**
	 * Stores information pertaining to a single tile's particle effect, as well as
	 * its sprite override.
	 */
	struct tile_particle_node :
		public engine::script_reference_type<awe::tile_particle_node>,
		public sfx::animated_drawable {
		/**
		 * Registers this struct with the script interface, if it hasn't been
		 * already.
		 * @safety No guarantee.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Creates the tile particle node.
		 */
		tile_particle_node(const sf::Vector2u& t = {}, const std::string& p = {},
			const sf::Vector2f& o = {}, const std::string& s = {},
			const float d = 0.f);

		/**
		 * Creates the tile particle node.
		 * @return Pointer to the tile particle node.
		 */
		static awe::tile_particle_node* Create();

		/**
		 * Creates the tile particle node.
		 * @return Pointer to the tile particle node.
		 */
		static awe::tile_particle_node* Create(const sf::Vector2u& t = {},
			const std::string& p = {}, const sf::Vector2f& o = {},
			const std::string& s = {}, const float d = 0.f);

		/**
		 * The location of the tile who's having this particle animated over it.
		 */
		sf::Vector2u tile;

		/**
		 * The sprite ID of the particle to apply to this tile.
		 */
		std::string particle;

		/**
		 * The origin of the particle effect, which also maps to its position on
		 * the tile.
		 */
		sf::Vector2f origin;

		/**
		 * The ID of the sprite to apply to this tile whilst the particle is
		 * animating.
		 * Leave empty to prevent overriding the tile's sprite.
		 */
		std::string spriteOverride;

		/**
		 * The delay, in seconds, between when the \c tile_particles animation
		 * first starts animating, and when this tile's particle starts animating.
		 */
		float delay = 0.f;

		/**
		 * Mapping for AngelScript as std::optional is not registered.
		 */
		inline void setPosition(const sf::Vector2f& p) {
			position = p;
		}

		/**
		 * Use this method when you want to retrieve the position.
		 */
		inline sf::Vector2f getPosition() const {
			return position ? *position : origin;
		}
	private:
		/**
		 * The position of the particle within the tile as a percentage (0.0-1.0)
		 * of its X and Y size.
		 * If this value is empty, then it must be = to \c origin.
		 */
		std::optional<sf::Vector2f> position;
	public:
		/**
		 * The particle sprite.
		 * @warning \c tile_particles will need to assign the spritesheet, as well
		 *          as the \c particle sprite ID.
		 */
		sfx::animated_sprite particleSprite;

		/**
		 * Pointer to the tile sprite.
		 * @warning Must be assigned before creating \c tile_particles.
		 */
		std::shared_ptr<awe::animated_tile> tileSprite;

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
	};

	/**
	 * The tile particles animation.
	 * The particle sprite will be animated from start to finish once. Either that,
	 * or 1.5 seconds will have elapsed, before this drawable marks itself as
	 * finished.
	 */
	class tile_particles : public awe::animation {
	public:
		/**
		 * Sets up the tile particles animation.
		 * @param particles The list of particles to animate.
		 * @param sheet     The spritesheet containing the particle sprites.
		 */
		tile_particles(const std::vector<tile_particle_node>& particles,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

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
		 * The particle nodes.
		 */
		std::vector<tile_particle_node> _particles;
	};
}
