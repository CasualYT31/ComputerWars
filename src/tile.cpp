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

#include "tile.h"

awe::tile::tile(const std::shared_ptr<const awe::tile_type>& type) noexcept {
	setTileType(type);
}

void awe::tile::setTileType(const std::shared_ptr<const awe::tile_type>& type) noexcept {
	_type = type;
}

std::shared_ptr<const awe::tile_type> awe::tile::getTileType() const noexcept {
	return _type;
}

void awe::tile::setTileOwner(const awe::UUIDValue owner) noexcept {
	_owner = owner;
}

awe::UUIDValue awe::tile::getTileOwner() const noexcept {
	return _owner;
}

void awe::tile::setTileHP(const awe::HP hp) noexcept {
	if (hp <= 0) {
		_hp = 0;
	} else {
		_hp = hp;
	}
}

awe::HP awe::tile::getTileHP() const noexcept {
	return _hp;
}

void awe::tile::setUnit(const awe::UnitID id) noexcept {
	_unit = id;
}

awe::UnitID awe::tile::getUnit() const noexcept {
	return _unit;
}