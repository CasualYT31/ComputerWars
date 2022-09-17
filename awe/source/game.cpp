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

#include "game.hpp"
#include "engine.hpp"

const std::runtime_error NO_MAP("No map is currently loaded");
const std::runtime_error NO_SCRIPTS(
	"No scripts object was given to this game object");
const std::runtime_error INVALID_UNIT_ID("The given unit ID was invalid");

awe::game::game(const std::string& name) noexcept : _logger(name) {}

void awe::game::setScripts(const std::shared_ptr<engine::scripts>& scripts)
	noexcept {
	_scripts = scripts;
}

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
	sf::View oldView = target.getView();
	sf::View view;
	view.reset(sf::FloatRect(0, 0, target.getSize().x, target.getSize().y));
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	target.setView(view);
	if (_map) target.draw(*_map, states);
	target.setView(oldView);
}

//////////////////////
// SCRIPT INTERFACE //
//////////////////////

void awe::game::moveSelectedTileUp() {
	if (_map)
		_map->moveSelectedTileUp();
	else
		throw NO_MAP;
}

void awe::game::moveSelectedTileDown() {
	if (_map)
		_map->moveSelectedTileDown();
	else
		throw NO_MAP;
}

void awe::game::moveSelectedTileLeft() {
	if (_map)
		_map->moveSelectedTileLeft();
	else
		throw NO_MAP;
}

void awe::game::moveSelectedTileRight() {
	if (_map)
		_map->moveSelectedTileRight();
	else
		throw NO_MAP;
}

sf::Vector2u awe::game::getSelectedTile() const {
	if (_map)
		return _map->getSelectedTile();
	else
		throw NO_MAP;
}

awe::UnitID awe::game::getUnitOnTile(const sf::Vector2u tile) const {
	if (_map)
		return _map->getUnitOnTile(tile);
	else
		throw NO_MAP;
}

void awe::game::zoomIn() {
	if (_map) {
		_mapScalingFactor += 1.0f;
		if (_mapScalingFactor >= 3.9f) _mapScalingFactor = 3.0f;
		_map->setMapScalingFactor(_mapScalingFactor);
	} else {
		throw NO_MAP;
	}
}

void awe::game::zoomOut() {
	if (_map) {
		_mapScalingFactor -= 1.0f;
		if (_mapScalingFactor < 0.9f) _mapScalingFactor = 1.0f;
		_map->setMapScalingFactor(_mapScalingFactor);
	} else {
		throw NO_MAP;
	}
}

void awe::game::setSelectedTileByPixel(const sf::Vector2i pixel) {
	if (_map)
		_map->setSelectedTileByPixel(pixel);
	else
		throw NO_MAP;
}

sf::Vector2u awe::game::getTileSize() const {
	if (_map)
		return _map->getTileSize();
	else
		throw NO_MAP;
}

const awe::commander awe::game::getArmyCurrentCO(const awe::ArmyID army) const {
	if (_map) {
		auto co = _map->getArmyCurrentCO(army);
		if (co)
			return *co;
		else
			throw std::runtime_error("Could not retrieve the army's current CO");
	}
	throw NO_MAP;
}

const awe::commander awe::game::getArmyTagCO(const awe::ArmyID army) const {
	if (_map) {
		auto co = _map->getArmyTagCO(army);
		if (co)
			return *co;
		else
			throw std::runtime_error("Could not retrieve the army's tag CO");
	}
	throw NO_MAP;
}

const awe::country awe::game::getArmyCountry(const awe::ArmyID army) const {
	if (_map) {
		auto country = _map->getArmyCountry(army);
		if (country)
			return *country;
		else
			throw std::runtime_error("Could not retrieve the army's country");
	}
	throw NO_MAP;
}

awe::Funds awe::game::getArmyFunds(const awe::ArmyID army) const {
	if (_map) {
		auto funds = _map->getArmyFunds(army);
		if (funds >= 0)
			return funds;
		else
			throw std::runtime_error("Could not retrieve the army's funds");
	}
	throw NO_MAP;
}

bool awe::game::tagCOIsPresent(const awe::ArmyID army) const {
	if (_map)
		return _map->tagCOIsPresent(army);
	else
		throw NO_MAP;
}

std::size_t awe::game::getArmyCount() const {
	if (_map)
		return _map->getArmyCount();
	else
		throw NO_MAP;
}

const awe::tile_type awe::game::getTileType(const sf::Vector2u& pos) const {
	if (_map) {
		auto type = _map->getTileType(pos);
		if (type)
			return *type;
		else
			throw std::runtime_error("Could not retrieve the property of a tile. "
				"The tile coordinate given was out of range.");
	} else {
		throw NO_MAP;
	}
}

const awe::terrain awe::game::getTerrainOfTile(const sf::Vector2u& pos) const {
	return *getTileType(pos).getType();
}

awe::ArmyID awe::game::getTileOwner(const sf::Vector2u& pos) const {
	if (_map)
		return _map->getTileOwner(pos);
	else
		throw NO_MAP;
}

awe::HP awe::game::getTileHP(const sf::Vector2u& pos) const {
	if (_map)
		return _map->getTileHP(pos);
	else
		throw NO_MAP;
}

const awe::unit_type awe::game::getUnitType(const awe::UnitID id) const {
	if (_map) {
		auto type = _map->getUnitType(id);
		if (type)
			return *type;
		else
			throw INVALID_UNIT_ID;
	} else {
		throw NO_MAP;
	}
}

awe::ArmyID awe::game::getArmyOfUnit(const awe::UnitID id) const {
	if (_map) {
		auto army = _map->getArmyOfUnit(id);
		if (army == awe::army::NO_ARMY) {
			throw INVALID_UNIT_ID;
		} else {
			return army;
		}
	} else {
		throw NO_MAP;
	}
}

awe::HP awe::game::getUnitHP(const awe::UnitID id) const {
	if (_map)
		return _map->getUnitHP(id);
	else
		throw NO_MAP;
}

awe::Fuel awe::game::getUnitFuel(const awe::UnitID id) const {
	if (_map)
		return _map->getUnitFuel(id);
	else
		throw NO_MAP;
}

awe::Ammo awe::game::getUnitAmmo(const awe::UnitID id) const {
	if (_map)
		return _map->getUnitAmmo(id);
	else
		throw NO_MAP;
}

CScriptArray* awe::game::getLoadedUnits(const awe::UnitID id) const {
	if (_map) {
		if (_scripts) {
			auto set = _map->getLoadedUnits(id);
			CScriptArray* ret = _scripts->createArray("UnitID");
			if (ret) {
				for (auto id : set) ret->InsertLast(&id);
				return ret;
			} else {
				throw std::runtime_error("Could not create array");
			}
		} else {
			throw NO_SCRIPTS;
		}
	} else {
		throw NO_MAP;
	}
}

//////////////////////////////
// INTERFACE HELPER METHODS //
//////////////////////////////
