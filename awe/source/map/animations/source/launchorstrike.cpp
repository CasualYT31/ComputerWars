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

#include "../include/launchorstrike.hpp"

awe::launch_or_strike::launch_or_strike(const bool launch,
	const std::string& sprite,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet, const float& scaling,
	const sf::View& view, const std::shared_ptr<awe::animated_tile>& tileSprite,
	const float duration) : _launch(launch), _scaling(scaling), _view(view),
	_duration(duration), _tileSprite(tileSprite), _movingSprite(sheet, sprite) {}

bool awe::launch_or_strike::animate(const sf::RenderTarget& target) {
	_movingSprite.animate(target);
	if (firstTimeAnimated()) {
		const auto movingSpriteHeight = _movingSprite.getSize().y * _scaling;
		const auto centroid = sf::Vector2f(target.mapCoordsToPixel(
			_tileSprite->getPixelPosition() + _tileSprite->getPixelSize() * 0.5f,
			_view));
		const auto sourceY = _launch ? centroid.y : -movingSpriteHeight;
		_movingSprite.setPosition({ centroid.x, sourceY });
		_destinationY = _launch ? -movingSpriteHeight : centroid.y;
		_speed = ::fabs(sourceY - _destinationY) / _duration;
		if (_launch) _speed = -_speed;
	}
	const auto delta = calculateDelta();
	if (_launch) _movingSprite.setOrigin(_movingSprite.getUnscaledSize() / 2.f);
	else _movingSprite.setOrigin({ _movingSprite.getUnscaledSize().x / 2.f,
		_movingSprite.getUnscaledSize().y });
	auto pos = _movingSprite.getPosition();
	pos.y += _speed * delta;
	_movingSprite.setPosition(pos);
	_movingSprite.setScale({ _scaling, _scaling });
	return (_speed < 0.0f && pos.y < _destinationY) ||
		(_speed > 0.0f && pos.y > _destinationY);
}

void awe::launch_or_strike::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	const sf::View oldView = target.getView();
	sf::View newView(sf::FloatRect({ 0.0f, 0.0f },
		sf::Vector2f(target.getSize())));
	target.setView(newView);
	target.draw(_movingSprite, states);
	target.setView(oldView);
}
