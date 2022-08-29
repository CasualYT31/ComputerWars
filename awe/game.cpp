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

#include "game.h"
#include "engine.h"

awe::game::game(const std::string& name) noexcept : _logger(name) {}

bool awe::game::load(const std::string& file,
	const std::shared_ptr<awe::bank<awe::country>>& countries,
	const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
	const std::shared_ptr<awe::bank<awe::unit_type>>& units,
	const std::shared_ptr<awe::bank<awe::commander>>& commanders,
	const std::shared_ptr<sfx::animated_spritesheet>& tile_sheet,
	const std::shared_ptr<sfx::animated_spritesheet>& unit_sheet,
	const std::shared_ptr<sfx::animated_spritesheet>& icon_sheet,
	const std::shared_ptr<sfx::animated_spritesheet>& co_sheet,
	const std::shared_ptr<sf::Font>& font,
	const std::shared_ptr<engine::language_dictionary>& dict) noexcept {
	try {
		_map = std::make_unique<awe::map>(countries, tiles, units, commanders);
	} catch (std::bad_alloc) {
		_logger.error("Couldn't allocate the map object for the game object.");
		return false;
	}
	if (_map) {
		_map->setTileSpritesheet(tile_sheet);
		_map->setUnitSpritesheet(unit_sheet);
		_map->setIconSpritesheet(icon_sheet);
		_map->setCOSpritesheet(co_sheet);
		_map->setFont(font);
		_map->setLanguageDictionary(dict);
		auto r = _map->load(file);
		_map->selectArmy(0);
		return r;
	}
	return false;
}

bool awe::game::save() noexcept {
	if (_map)
		return _map->save("");
	else
		return false;
}

void awe::game::quit() noexcept {
	_map = nullptr;
}

bool awe::game::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	if (_map) return _map->animate(target, scaling);
	return false;
}

void awe::game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_map) target.draw(*_map, states);
}

//////////////////////
// SCRIPT INTERFACE //
//////////////////////

void awe::game::moveSelectedTileUp() noexcept {
	if (_map) _map->moveSelectedTileUp();
}

void awe::game::moveSelectedTileDown() noexcept {
	if (_map) _map->moveSelectedTileDown();
}

void awe::game::moveSelectedTileLeft() noexcept {
	if (_map) _map->moveSelectedTileLeft();
}

void awe::game::moveSelectedTileRight() noexcept {
	if (_map) _map->moveSelectedTileRight();
}

sf::Vector2u awe::game::getSelectedTile() const noexcept {
	if (_map)
		return _map->getSelectedTile();
	else
		return sf::Vector2u();
}

awe::UnitID awe::game::getUnitOnTile(const sf::Vector2u tile) const noexcept {
	if (_map)
		return _map->getUnitOnTile(tile);
	else
		return awe::UnitID{};
}

void awe::game::zoomIn() noexcept {
	if (_map) {
		_mapScalingFactor += 1.0f;
		if (_mapScalingFactor >= 3.9f) _mapScalingFactor = 3.0f;
		_map->setMapScalingFactor(_mapScalingFactor);
	}
}

void awe::game::zoomOut() noexcept {
	if (_map) {
		_mapScalingFactor -= 1.0f;
		if (_mapScalingFactor < 0.9f) _mapScalingFactor = 1.0f;
		_map->setMapScalingFactor(_mapScalingFactor);
	}
}

void awe::game::setSelectedTileByPixel(const sf::Vector2i pixel) noexcept {
	if (_map) _map->setSelectedTileByPixel(pixel);
}

sf::Vector2u awe::game::getTileSize() const noexcept {
	if (_map)
		return _map->getTileSize();
	else
		return sf::Vector2u();
}

//////////////////////////////
// INTERFACE HELPER METHODS //
//////////////////////////////
