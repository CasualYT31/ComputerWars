/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

#include "terrain.h"

awe::tile::tile(const awe::tile_type* tile, const unsigned int owner, const int hp) noexcept : _tileType(tile), _hp(hp), _owner(owner) {}

const awe::tile_type* awe::tile::setTile(const awe::tile_type* newTile) noexcept {
	auto old = getTile();
	_tileType = newTile;
	return old;
}

const awe::tile_type* awe::tile::getTile() const noexcept {
	return _tileType;
}

int awe::tile::setHP(const int newHP) noexcept {
	auto old = getHP();
	_hp = newHP;
	if (_hp < 0) {
		_hp = 0;
	} else if (_hp > (int)_tileType->getType()->getMaxHP()) {
		_hp = _tileType->getType()->getMaxHP();
	}
	return old;
}

int awe::tile::getHP() const noexcept {
	return _hp;
}

unsigned int awe::tile::setOwner(const unsigned int newOwner) noexcept {
	auto old = getOwner();
	_owner = newOwner;
	return old;
}

unsigned int awe::tile::getOwner() const noexcept {
	return _owner;
}