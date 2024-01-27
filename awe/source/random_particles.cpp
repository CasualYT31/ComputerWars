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

#include "random_particles.hpp"

void awe::random_particles::resetParticles(
	const std::vector<awe::random_particles::data>& particles) {
	_particleSet = particles;
}

bool awe::random_particles::animate(const sf::RenderTarget& target) {
	const auto delta = calculateDelta();
	static const float MIN_AREA =
		static_cast<float>(sfx::renderer::MIN_SIZE.x * sfx::renderer::MIN_SIZE.y);
	const sf::Vector2f targetSize(target.getSize());
	const float targetArea = targetSize.x * targetSize.y;
	for (auto& particle : _particleSet) {
		const auto newParticleCount = std::max(static_cast<std::size_t>(1),
			static_cast<std::size_t>(std::max(targetArea, MIN_AREA) / MIN_AREA *
				particle.density));
		if (particle.sprites.size() != newParticleCount) {
			// If the target's resized enough, clear out all of our current sprites
			// and generate new ones.
			std::uniform_real_distribution<float>
				xDistribution(0.0f, targetSize.x),
				yDistribution(0.0f, targetSize.y);
			particle.sprites.clear();
			particle.sprites.resize(newParticleCount);
			for (auto& sprite : particle.sprites) {
				sprite.sprite.setSpritesheet(particle.sheet);
				sprite.sprite.setSprite(particle.spriteID);
				sprite.sprite.animate(target);
				sprite.sprite.setPosition({ xDistribution(*_prng),
					yDistribution(*_prng) });
				_calculateOrigin(sprite.sprite, targetSize);
			}
		} else {
			// Otherwise, move the sprites we have right now.
			for (auto& sprite : particle.sprites) {
				sprite.sprite.animate(target);
				auto newPos = sprite.sprite.getPositionWithoutOffset() +
					particle.vector * delta;
				if (!sf::FloatRect(0.0f, 0.0f, targetSize.x, targetSize.y)
					.contains(newPos)) {
					if ((particle.respawnDelay == sf::Time::Zero) ||
						(sprite.despawned &&
						sprite.clock.getElapsedTime() >= particle.respawnDelay)) {
						sprite.despawned = false;

						// 0. Since all sprites are created at once, every sprite
						//    will animate as if they have a global frame ID, which
						//    looks bad if the sprite you're trying to use doesn't
						//    have one. So reset a sprite's animation to the
						//    beginning when it respawns to make sprites animate
						//    out of sync. If a sprite has a global frame ID,
						//    though, this won't affect it.
						sprite.sprite.setCurrentFrame(0);
						sprite.sprite.animate(target);
						// 1. Find the angle where the random trajectory angle
						//    starts from. Since the trajectory pointing to the new
						//    location is in the opposite direction of the vector,
						//    we first find the angle of the vector relative to the
						//    X axis and then add 90 degrees to it.
						const auto startAngle =
							engine::angleOfPoint(particle.vector) + 90.0f;
						// 2. The angle pointing to the new location of the
						//    particle that's in the opposite direction of the
						//    vector.
						const auto trajectory =
							startAngle + _angleDistribution(*_prng);
						// 3. The new position will be calculated as a point along
						//    the circle with a radius that ensures it covers the
						//    entire target. Make sure to clamp it to the target
						//    size.
						const auto centroid = targetSize * 0.5f;
						const auto radius = engine::magnitude(centroid);
						newPos.x = std::clamp(
							radius * engine::cos(trajectory) + centroid.x,
							0.0f,
							targetSize.x
						);
						newPos.y = std::clamp(
							radius * engine::sin(trajectory) + centroid.y,
							0.0f,
							targetSize.y
						);

						//_line[0].position = {
						//	radius * engine::cos(startAngle) + centroid.x,
						//	radius * engine::sin(startAngle) + centroid.y
						//};
						//_line[0].color = sf::Color::White;
						//_line[1].position = {
						//	radius * engine::cos(startAngle + 180.0f) + centroid.x,
						//	radius * engine::sin(startAngle + 180.0f) + centroid.y
						//};
						//_line[1].color = sf::Color::White;

						//_line2[0].position = centroid;
						//_line2[0].color = sf::Color::Red;
						//_line2[1].position = centroid + particle.vector;
						//_line2[1].color = sf::Color::Red;

						//_line3[0].position = centroid;
						//_line3[0].color = sf::Color::Green;
						//_line3[1].position = {
						//	radius * engine::cos(trajectory) + centroid.x,
						//	radius * engine::sin(trajectory) + centroid.y
						//};
						//_line3[1].color = sf::Color::Green;
					} else if (!sprite.despawned) {
						// Despawn the sprite: start the timer and wait for the
						// delay to expire.
						sprite.despawned = true;
						sprite.clock.restart();
					}
				}
				sprite.sprite.setPosition(newPos);
				_calculateOrigin(sprite.sprite, targetSize);
			}
		}
	}
	return false;
}

void awe::random_particles::drawWithIndependentView(sf::RenderTarget& target,
	sf::RenderStates states) const {
	for (const auto& particle : _particleSet)
		for (const auto& sprite : particle.sprites)
			if (!sprite.despawned) target.draw(sprite.sprite, states);
	//target.draw(_line, states);
	//target.draw(_line2, states);
	//target.draw(_line3, states);
}

void awe::random_particles::_calculateOrigin(sfx::animated_sprite& sprite,
	const sf::Vector2f& targetSize) {
	const auto pos = sprite.getPositionWithoutOffset();
	sprite.setOrigin({
		std::clamp(1.0f - pos.x / targetSize.x, 0.0f, 1.0f),
		std::clamp(1.0f - pos.y / targetSize.y, 0.0f, 1.0f)
	});
}
