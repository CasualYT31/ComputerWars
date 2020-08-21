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

#include "map.h"

sf::Vector2u awe::map::setSize(const sf::Vector2u& dim) noexcept {
	auto old = getSize();
	_tiles.resize(dim.x);
	for (auto itr = _tiles.begin(), enditr = _tiles.end(); itr != enditr; itr++) {
		itr->resize(dim.y);
	}
	_size = dim;
	return old;
}

sf::Vector2u awe::map::getSize() const noexcept {
	return _size;
}

std::string awe::map::setName(const std::string& newName) noexcept {
	auto old = getName();
	_name = newName;
	return old;
}

std::string awe::map::getName() const noexcept {
	return _name;
}

awe::tile& awe::map::getTile(const sf::Vector2u& pos) {
	if (pos.x >= _tiles.size() || pos.y >= _tiles[0].size()) throw std::out_of_range("Tile position was out of range");
	return _tiles[pos.x][pos.y];
}