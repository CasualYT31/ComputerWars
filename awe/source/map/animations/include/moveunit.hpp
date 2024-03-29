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

/**@file moveunit.hpp
 * Defines the animation played when a unit is moving along a path.
 */

#pragma once

#include "map.hpp"

namespace awe {
	/**
	 * The animation played when a unit is moving.
	 */
	class move_unit : public awe::animation {
	public:
		/**
		 * Represents a single position along a \c move_unit path.
		 */
		struct node {
			/**
			 * Initialises the node.
			 */
			node(const sf::Vector2f& p,
				const std::shared_ptr<sfx::animated_spritesheet>& s,
				const std::string& f, const std::string& n) :
				position(p), sheet(s), firstSound(f), secondSound(n) {}

			/**
			 * The position to move the unit sprite to.
			 */
			const sf::Vector2f position;

			/**
			 * The spritesheet to apply to the unit sprite when moving towards this
			 * position.
			 * The \c sheet in the first node is ignored.
			 */
			const std::shared_ptr<sfx::animated_spritesheet> sheet;

			/**
			 * The name of the sound to play when the unit is travelling to this
			 * node during the first half of the journey.
			 * If there was a move sound playing previously in this animation, then
			 * that sound will be stopped.\n
			 * If this value is empty, or the same as the name of the sound
			 * currently playing in the animation, then nothing changes.
			 * @warning The final sound in the animation will \b not be stopped! So
			 *          make sure this is carried out manually. This was done to
			 *          prevent move sounds from seemingly ending "abruptly."
			 */
			const std::string firstSound;

			/**
			 * The name of the sound to play when the unit is travelling to this
			 * node during the second half of the journey.
			 * @sa \c firstSound.
			 */
			const std::string secondSound;
		};

		/**
		 * Sets up the move unit animation.
		 * Since animations are animated after units, we can safely override the
		 * unit's pixel position without touching any of its data. Once the
		 * animation ends, the unit's original pixel position will be reassigned
		 * during the next iteration of the game loop.
		 * @param unitSprite The unit sprite whose position will be overidden.
		 * @param path       A list of pixel positions to move the unit sprite to,
		 *                   starting from its current position. It cannot have
		 *                   less than two elements. The first element dictates the
		 *                   starting position of the unit, and subsequent elements
		 *                   dictate the destinations, in order.
		 * @param speed      The speed the unit moves at, in pixels per second.
		 * @param sounds     The audio object containing the sounds to play when
		 *                   moving the unit.
		 */
		move_unit(const std::shared_ptr<awe::animated_unit>& unitSprite,
			const std::vector<node>& path, const float speed,
			const std::shared_ptr<sfx::audio>& sounds);

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
		 * When a new destination needs to be selected, invoke this method.
		 */
		void _setupNextDestination();

		/**
		 * Play the next sound in the list.
		 * @param sound The sound to play.
		 * @sa    \c move_unit::node::firstSound.
		 */
		void _playNextSound(const std::string& sound);

		/**
		 * The unit sprite that will be moved during this animation.
		 */
		const std::shared_ptr<awe::animated_unit> _unit;

		/**
		 * The path the unit will move along.
		 */
		const std::vector<node> _path;

		/**
		 * Speed the unit moves at, in pixels per second.
		 */
		const float _speed;

		/**
		 * Contains the sounds to play.
		 */
		const std::shared_ptr<sfx::audio> _sounds;

		/**
		 * The index of the \c _path location the unit is currently moving to.
		 */
		std::size_t _tile = 0;

		/**
		 * Keeps track of which sound this animation is currently playing.
		 */
		std::string _sound;
	};
}
