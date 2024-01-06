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

#include "../include/moveunit.hpp"
#include "maths.hpp"

awe::move_unit::move_unit(const std::shared_ptr<awe::animated_unit>& unitSprite,
	const std::vector<node>& path, const float speed) :
	_unit(unitSprite), _path(path), _speed(speed),
	_previousSheet(unitSprite->getSpritesheet()) {
	assert(path.size() >= 2);
	_setupNextDestination();
}

bool awe::move_unit::animate(const sf::RenderTarget& target) {
	// If the game tried to overwrite the spritesheets of this unit, remember what
	// they were so we can apply them once we're done.
	const auto unitSheet = _unit->getSpritesheet();
	if (unitSheet != _previousSheet && unitSheet != _path[_tile].sheet)
		_previousSheet = unitSheet;
	const auto iconSheet = _unit->getIconSpritesheet();
	if (iconSheet && iconSheet != _iconSheet) _iconSheet = iconSheet;
	// Force icons to disappear. There is a case where if a unit is obstructed by a
	// hidden unit, the moving unit's icons are displayed for a frame (as the
	// scripts sets the idle spritesheet whilst this animation is on-going).
	_unit->setIconSpritesheet(nullptr);

	const auto delta = accumulatedDelta();

	const auto slope = _path[_tile].position - _path[_tile - 1].position;
	auto newPos = _path[_tile - 1].position;
	const auto normalisedSlope = engine::normalise(slope);
	if (slope.x != 0.0f) newPos.x += slope.x *
		((delta / (::abs(slope.x) / _speed)) * ::abs(normalisedSlope.x));
	if (slope.y != 0.0f) newPos.y += slope.y *
		((delta / (::abs(slope.y) / _speed)) * ::abs(normalisedSlope.y));
	if ((slope.x < 0 && newPos.x < _path[_tile].position.x) ||
		(slope.x >= 0 && newPos.x > _path[_tile].position.x))
		newPos.x = _path[_tile].position.x;
	if ((slope.y < 0 && newPos.y < _path[_tile].position.y) ||
		(slope.y >= 0 && newPos.y > _path[_tile].position.y))
		newPos.y = _path[_tile].position.y;
	_unit->setPixelPosition(newPos.x, newPos.y);

	if (engine::closeTo(newPos.x, _path[_tile].position.x) &&
		engine::closeTo(newPos.y, _path[_tile].position.y))
		_setupNextDestination();

	if (_tile >= _path.size()) {
		_unit->setSpritesheet(_previousSheet);
		_unit->setIconSpritesheet(_iconSheet);
		return true;
	} else return false;
}

void awe::move_unit::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	// Unit is already drawn by awe::map.
}

void awe::move_unit::_setupNextDestination() {
	resetDeltaAccumulation();
	if (++_tile >= _path.size()) return;
	_unit->setSpritesheet(_path[_tile].sheet);
}
