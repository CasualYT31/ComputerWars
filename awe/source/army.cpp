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

#include "army.hpp"
#include <algorithm>

awe::army::army(const std::shared_ptr<const awe::country>& country) :
	_country(country) {}

void awe::army::setCOs(const std::shared_ptr<const awe::commander>& current,
	const std::shared_ptr<const awe::commander>& tag) noexcept {
	if (!current && tag) {
		_co_1 = tag;
		_co_2 = nullptr;
	} else {
		_co_1 = current;
		_co_2 = tag;
	}
}

void awe::army::removeVisibleTiles(const std::unordered_set<sf::Vector2u>& tiles) {
	for (const auto& tile : tiles) {
		const auto itr = _visibleTiles.find(tile);
		if (itr != _visibleTiles.end()) _visibleTiles.erase(itr);
	}
}
