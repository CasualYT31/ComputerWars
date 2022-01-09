/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

#include "tile_pane.h"

void awe::tile_pane::setTile(const awe::tile& tile) noexcept {
	_tile = std::make_shared<const awe::tile>(tile);
}

void awe::tile_pane::addUnit(const awe::unit& unit) noexcept {
	_units.push_back(std::make_shared<const awe::unit>(unit));
}

void awe::tile_pane::clearUnits() noexcept {
	_units.clear();
}

bool awe::tile_pane::animate(const sf::RenderTarget& target) noexcept {
	return true;
}

void awe::tile_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

}