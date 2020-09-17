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
	// if the map has gotten bigger, we need to allocate new tiles
	if (old.x < dim.x || old.y < dim.y) {
		unsigned int x = 0;
		for (auto XItr = _tiles.begin(), endXItr = _tiles.end(); XItr != endXItr; XItr++) {
			unsigned int y = 0;
			for (auto YItr = XItr->begin(), endYItr = XItr->end(); YItr != endYItr; YItr++) {
				if (!*YItr) *YItr = std::make_shared<awe::tile>(sf::Vector2u(x, y));
				y++;
			}
			x++;
		}
	}
	_size = dim;
	return old;
}

sf::Vector2u awe::map::getSize() const noexcept {
	return _size;
}

bool awe::map::setVisiblePortion(const sf::Rect<unsigned int> portion) noexcept {
	if (portion.left >= _size.x || portion.top >= _size.y || portion.left + portion.width > _size.x || portion.top + portion.height > _size.y) return false;
	_visiblePortion = portion;
	return true;
}

sf::Rect<unsigned int> awe::map::getVisiblePortion() const noexcept {
	return _visiblePortion;
}

std::string awe::map::setName(const std::string& newName) noexcept {
	auto old = getName();
	_name = newName;
	return old;
}

std::string awe::map::getName() const noexcept {
	return _name;
}

std::shared_ptr<awe::tile> awe::map::getTile(const sf::Vector2u& pos) {
	if (pos.x >= _tiles.size() || pos.y >= _tiles[0].size()) throw std::out_of_range("Tile position was out of range");
	return _tiles[pos.x][pos.y];
}

void awe::map::setTileSpritesheet(const std::shared_ptr<awe::spritesheets::tiles>& ptr) noexcept {
	_tileSprites = ptr;
}

void awe::map::setPictureSpritesheet(const std::shared_ptr<awe::spritesheets::tile_pictures>& ptr) noexcept {
	_pictureSprites = ptr;
}

bool awe::map::animate(const sf::RenderTarget& target) noexcept {
	for (auto XItr = _tiles.begin(), XEndItr = _tiles.end(); XItr != XEndItr; XItr++) {
		for (auto YItr = XItr->begin(), YEndItr = XItr->end(); YItr != YEndItr; YItr++) {
			auto pTile = *YItr;
			pTile->animate(target);
			if (auto pUnit = pTile->getUnit().lock()) pUnit->animate(target);
		}
	}
}

void awe::map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	unsigned int width = ( _visiblePortion.width == 0 ? _size.x : _visiblePortion.left + _visiblePortion.width );
	unsigned int height = ( _visiblePortion.height == 0 ? _size.y : _visiblePortion.top + _visiblePortion.height );
	float xDrawing = 0.0, yDrawing = 0.0;
	for (unsigned int x = _visiblePortion.left; x < width; x++) {
		for (unsigned int y = _visiblePortion.top; y < height; y++) {
			// ensure that tiles higher than MIN_TILE_HEIGHT are anchored to the bottom edge
			// ensure unit is drawn

			auto pTile = _tiles[x][y];
			target.draw(*pTile, sf::RenderStates().transform.translate(xDrawing, yDrawing).combine(states.transform));
			// changes to the way spritesheets work in awe classes should make calculating this a lot easier, but for now, we're assuming the normal sheet is always used, as well as the neutral tile
			try {
				if (pTile->getSpritesheets() && pTile->getSpritesheets()->normal && pTile->getTile())
					xDrawing += pTile->getSpritesheets()->normal->accessSprite(pTile->getTile()->getNeutralTile()).width;
			} catch (std::exception&) {
				// accessSprite threw
			}
		}
		yDrawing += awe::tile::MIN_TILE_HEIGHT;
	}
}