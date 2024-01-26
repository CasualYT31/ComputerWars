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

/**
 * @file random_particles.hpp
 * Allows you to randomly generate animated particles.
 */

#pragma once

#include "renderer.hpp"
#include "texture.hpp"
#include "maths.hpp"
#include <unordered_set>

namespace awe {
	/**
	 * Randomly draws a set of particles that move from one edge of the target to
	 * another.
	 */
	class random_particles : public sfx::animated_drawable_with_independent_view {
	public:
		/**
		 * Configurations for a particle in a particle set.
		 */
		struct data {
			/**
			 * The spritesheet in which the particle sprite is stored.
			 */
			std::shared_ptr<sfx::animated_spritesheet> sheet;

			/**
			 * The sprite ID of the particle to create.
			 */
			std::string spriteID;

			/**
			 * The size of this vector defines the maximum number of this type of
			 * sprite to render at a time.
			 * The objects themselves are initialised by \c random_particles.
			 */
			std::vector<sfx::animated_sprite> sprites;

			/**
			 * If these particles move across the target, this vector will describe
			 * the direction of their movement, where new particles spawn in from,
			 * and where particles despawn.
			 * The vector's magnitude will also define the speed of the particles,
			 * in pixels per second.
			 */
			sf::Vector2f vector;
		};

		/**
		 * Represents a set of particles to render.
		 */
		using particle_set = std::vector<data>;

		/**
		 * Initialises the angle distribution.
		 */
		random_particles() : _angleDistribution(0.0f, 180.0f)/*,
			_line(sf::LineStrip, 2), _line2(sf::LineStrip, 2),
			_line3(sf::LineStrip, 2)*/ {}

		/**
		 * Removes all current particles, and initialises a new set of particles.
		 * Initially, the maximum amount of each particle will spawn at random
		 * positions. Then, they will either stay stationary or start moving.\n
		 * If a particle is not configured to move, then the particle sprites will
		 * animate, and that's all. They will not be added or removed unless
		 * \c resetParticles() is called.\n
		 * If they move, however, they will all move in the direction defined by
		 * their vector. The line that is perpendicular to this vector divides the
		 * target into two halves. The half that the vector points \em away from
		 * will be the half where new particles spawn in, and the half that the
		 * vector points \em towards will be where they despawn. Particles will
		 * only spawn and despawn \em outside of the target, however, and never
		 * inside, unless they are spawned when the particles are reset using this
		 * method.
		 * @warning This drawable will not render anything until this method has
		 *          been invoked.
		 * @param   particles  The different types of particles to render at once.
		 * @param   targetSize The size of the target. Used to know where to place
		 *                     the particles at initialisation.
		 */
		void resetParticles(const particle_set& particles,
			const sf::Vector2u& targetSize);

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the animation to.
		 * @return \c FALSE.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the animation to.
		 * @param states The render states to apply to the animation.
		 */
		void drawWithIndependentView(sf::RenderTarget& target,
			sf::RenderStates states) const final;

		/**
		 * Generates a random position within 
		 */
		sf::Vector2f _generatePosition(const sf::Vector2f& min,
			const sf::Vector2f& max) const;

		/**
		 * The pseudo-random number sequence generator.
		 */
		const std::unique_ptr<std::mt19937> _prng = engine::RNGFactory();

		/**
		 * Used when repositioning particles after they've moved out of the target.
		 */
		std::uniform_real_distribution<float> _angleDistribution;

		/**
		 * The types of particles to keep track of.
		 */
		particle_set _particleSet;

		// Debug ling showing the line perpendicular to particle.vector.
		//sf::VertexArray _line;
		// Debug line showing the particle.vector.
		//sf::VertexArray _line2;
		// Debug line showing the latest randomly generated trajectory.
		//sf::VertexArray _line3;
	};
}
