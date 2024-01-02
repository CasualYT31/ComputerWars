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

#include "../include/tileparticle.hpp"

awe::tile_particle::tile_particle(const awe::animated_tile& tileSprite,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::string& particle, const sf::Vector2f& origin) :
	_particle(sheet, particle),
	_origin({ std::min(origin.x, 1.0f), std::min(origin.y, 1.0f) }) {
	auto pos = tileSprite.getPixelPosition();
	pos.x += tileSprite.getPixelSize().x * _origin.x;
	pos.y += tileSprite.getPixelSize().y * _origin.y;
	_particle.setPosition(pos);
}

bool awe::tile_particle::animate(const sf::RenderTarget& target) {
	if (firstTimeAnimated()) _timer.restart();
	const auto ret = _particle.animate(target);
	const auto particleSize = _particle.getSize();
	_particle.setOrigin(sf::Vector2f(particleSize.x * _origin.x,
		particleSize.y * _origin.y));
	return ret || _timer.getElapsedTime() >= sf::seconds(1.5f);
}

void awe::tile_particle::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	target.draw(_particle, states);
}
