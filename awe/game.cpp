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

awe::game::game(const std::string& file,
	const std::shared_ptr<sfx::scripts>& ptr,
	const std::shared_ptr<awe::bank<awe::country>>& countries,
	const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
	const std::shared_ptr<awe::bank<awe::unit_type>>& units,
	const std::shared_ptr<awe::bank<awe::commander>>& commanders,
	const std::string& name) noexcept :
	_logger(name), _map(countries, tiles, units, commanders), _mapFileName(file),
	_scripts(ptr) {
	// Register the interface, then load the scripts.
	_scripts->addRegistrant(this);
	_scripts->loadScripts("assets/scripts"); // Add parameter for this soon.
}

void awe::game::registerInterface(asIScriptEngine* engine) noexcept {
	// VECTOR2 TYPE
	engine->RegisterObjectType("Vector2", sizeof(sf::Vector2u),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2u>());
	engine->RegisterObjectProperty("Vector2", "uint x", asOFFSET(sf::Vector2u, x));
	engine->RegisterObjectProperty("Vector2", "uint y", asOFFSET(sf::Vector2u, y));

	// MAP FUNCTIONS
	engine->RegisterGlobalFunction(
		asUnitID.substr().append(" getUnitOnTile(const Vector2)").c_str(),
		asMETHOD(awe::map, getUnitOnTile), asCALL_THISCALL_ASGLOBAL, &_map);
}

bool awe::game::load() noexcept {
	auto ret = _map.load(_mapFileName);
	_map.selectArmy(0);
	_map.setVisiblePortionOfMap(sf::Rect<sf::Uint32>(0, 0, _map.getMapSize().x,
		_map.getMapSize().y));
	return ret;
}

bool awe::game::save() noexcept {
	return _map.save(_mapFileName);
}

void awe::game::handleInput(const std::shared_ptr<sfx::user_input>& ui) noexcept {
	if ((*ui)["left"]) {
		_map.setSelectedTile(sf::Vector2u(_map.getSelectedTile().x - 1,
			_map.getSelectedTile().y));
	} else if ((*ui)["right"]) {
		_map.setSelectedTile(sf::Vector2u(_map.getSelectedTile().x + 1,
			_map.getSelectedTile().y));
	} else if ((*ui)["up"]) {
		_map.setSelectedTile(sf::Vector2u(_map.getSelectedTile().x,
			_map.getSelectedTile().y - 1));
	} else if ((*ui)["down"]) {
		_map.setSelectedTile(sf::Vector2u(_map.getSelectedTile().x,
			_map.getSelectedTile().y + 1));
	} else if ((*ui)["select"]) {
		// load();
		_scripts->callFunction("tileHasBeenSelected", &_map.getSelectedTile());
	}
}

void awe::game::setTileSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_map.setTileSpritesheet(sheet);
}

void awe::game::setUnitSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_map.setUnitSpritesheet(sheet);
}

void awe::game::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_map.setIconSpritesheet(sheet);
}

void awe::game::setCOSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_map.setCOSpritesheet(sheet);
}

void awe::game::setFont(const std::shared_ptr<sf::Font>& font) noexcept {
	_map.setFont(font);
}

bool awe::game::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	return _map.animate(target, scaling);
}

void awe::game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_map, states);
}