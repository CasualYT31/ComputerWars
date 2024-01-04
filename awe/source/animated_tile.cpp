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

#include "animated_tile.hpp"

awe::animated_tile::animated_tile(const engine::logger::data& data) :
	_sprite(data) {}

void awe::animated_tile::setSpriteOverride(const std::string& spriteID) {
	if (!_oldSprite) _oldSprite = getSprite();
	_sprite.setSprite(spriteID);
	if (_targetCache) animate(*_targetCache);
}

void awe::animated_tile::clearSpriteOverride() {
	if (!_oldSprite) return;
	const std::string oldSprite = *_oldSprite;
	_oldSprite.reset();
	setSprite(oldSprite);
	if (_targetCache) animate(*_targetCache);
}

bool awe::animated_tile::animate(const sf::RenderTarget& target) {
	_targetCache = &target;
	return _sprite.animate(target);
}

void awe::animated_tile::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	target.draw(_sprite, states);
}
