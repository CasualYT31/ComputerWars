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
#include "fmtengine.hpp"
#include <iostream>

void awe::random_particles::resetParticles(const particle_set& particles,
	const sf::Vector2u& targetSize) {
	std::uniform_real_distribution<float>
		xDistribution(0.0f, static_cast<float>(targetSize.x)),
		yDistribution(0.0f, static_cast<float>(targetSize.y));
	_particleSet = particles;
	for (auto& particle : _particleSet) {
		for (auto& sprite : particle.sprites) {
			sprite.setSpritesheet(particle.sheet);
			sprite.setSprite(particle.spriteID);
			sprite.setPosition({ xDistribution(*_prng), yDistribution(*_prng) });
		}
	}
}

bool awe::random_particles::animate(const sf::RenderTarget& target) {
	const auto delta = calculateDelta();
	for (auto& particle : _particleSet) for (auto& sprite : particle.sprites) {
		sprite.animate(target);
		sprite.setOrigin(sprite.getUnscaledSize() * 0.5f);
		auto newPos = sprite.getPosition() + particle.vector * delta;
		const sf::Vector2f targetSize(target.getSize());
		if (!sf::FloatRect(0.0f, 0.0f, targetSize.x, targetSize.y).contains(
			newPos)) {
			// 1. Find the angle where the random trajectory angle starts from.
			//    Since the trajectory pointing to the new location is in the
			//    opposite direction of the vector, we first find the angle of the
			//    vector relative to the X axis and then add 90 degrees to it.
			const auto startAngle = engine::angleOfPoint(particle.vector) + 90.0f;
			// 2. The angle pointing to the new location of the particle that's in
			//    the opposite direction of the vector.
			const auto trajectory = startAngle + _angleDistribution(*_prng);
			// 3. The new position will be calculated as a point along the circle
			//    with a radius that ensures it covers the entire target.
			const auto centroid = targetSize * 0.5f;
			const auto radius = engine::magnitude(centroid);
			//_line[0].position = { radius * engine::cos(startAngle),
			//	radius * engine::sin(startAngle) };
			//_line[0].position += centroid;
			//_line[0].color = sf::Color::White;
			//_line[1].position = { radius * engine::cos(startAngle + 180.0f),
			//	radius * engine::sin(startAngle + 180.0f) };
			//_line[1].position += centroid;
			//_line[1].color = sf::Color::White;
			//_line2[0].position = centroid;
			//_line2[0].color = sf::Color::Red;
			//_line2[1].position = centroid + particle.vector;
			//_line2[1].color = sf::Color::Red;
			//_line3[0].position = centroid;
			//_line3[0].color = sf::Color::Green;
			//_line3[1].position = { radius * engine::cos(trajectory),
			//	radius * engine::sin(trajectory) };
			//_line3[1].position += centroid;
			//_line3[1].color = sf::Color::Green;
			newPos.x = radius * engine::cos(trajectory);
			newPos.y = radius * engine::sin(trajectory);
			newPos += centroid;
			// 4. Now we snap the point to the target.
			if (newPos.x < 0.0f) newPos.x = 0.0f;
			if (newPos.x > targetSize.x) newPos.x = targetSize.x;
			if (newPos.y < 0.0f) newPos.y = 0.0f;
			if (newPos.y > targetSize.y) newPos.y = targetSize.y;
		}
		sprite.setPosition(newPos);
	}
	return false;
}

void awe::random_particles::drawWithIndependentView(sf::RenderTarget& target,
	sf::RenderStates states) const {
	for (const auto& particle : _particleSet)
		for (const auto& sprite : particle.sprites) target.draw(sprite, states);
	//target.draw(_line, states);
	//target.draw(_line2, states);
	//target.draw(_line3, states);
}
