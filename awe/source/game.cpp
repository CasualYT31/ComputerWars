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
#include <algorithm>

const std::runtime_error NO_MAP("No map is currently loaded");
const std::runtime_error NO_SCRIPTS(
	"No scripts object was given to this game object");
const std::runtime_error INVALID_UNIT_ID("The given unit ID was invalid");
const std::runtime_error INVALID_ARRAY("Could not create arrays");

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
	const std::shared_ptr<engine::language_dictionary>& dict,
	awe::game_options* options) noexcept {
	try {
		_map = std::make_unique<awe::map>(countries, tiles, units, commanders);
	} catch (std::bad_alloc) {
		_logger.error("Couldn't allocate the map object for the game object.");
		return false;
	}
	if (_map) {
		_unitBank = units;
		_map->setTileSpritesheet(tile_sheet);
		_map->setUnitSpritesheet(unit_sheet);
		_map->setIconSpritesheet(icon_sheet);
		_map->setCOSpritesheet(co_sheet);
		_map->setFont(font);
		_map->setLanguageDictionary(dict);
		auto r = _map->load(file);
		// If we have any game options, apply them.
		if (options) {
			// Assign current and tag COs.
			auto IDs = _map->getArmyIDs();
			for (auto& ID : IDs) {
				try {
					// Don't forget, weird behaviour will occur if nullptr is given
					// here. It will move the old tag CO to the current CO.
					_map->setArmyCurrentCO(ID,
						options->getCurrentCO(ID, commanders));
				} catch (const std::range_error& e) {
					std::string msg(e.what());
					if (!msg.empty()) {
						_logger.error("Couldn't override current CO for army {}: "
							"{}", ID, msg);
					}
				}
				try {
					_map->setArmyTagCO(ID, options->getTagCO(ID, commanders));
				} catch (const std::range_error& e) {
					std::string msg(e.what());
					if (!msg.empty()) {
						_logger.error("Couldn't override tag CO for army {}: {}",
							ID, msg);
					}
				}
			}
		}
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
	view.reset(sf::FloatRect(0.0f, 0.0f, (float)target.getSize().x,
		(float)target.getSize().y));
	view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	target.setView(view);
	if (_map) target.draw(*_map, states);
	target.setView(oldView);
}


/////////////////////////////////////
// SCRIPT INTERFACE UNIQUE TO GAME //
/////////////////////////////////////

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

void awe::game::endTurn() {
	if (_map) {
		const auto next = _map->getNextArmy();
		if (_map->getSelectedArmy() >= next) {
			// It can technically overflow, but I don't care! If they can play for
			// over 11,767,033 in-game years then they deserve what they get.
			_map->setDay(_map->getDay() + 1);
		}
		_map->selectArmy(next);
		// Begin next army's turn.
		if (_scripts->functionDeclExists("void BeginTurnForUnit(const UnitID, "
			"const Unit&in, const Vector2&in)")) {
			auto units = _map->getUnitsOfArmyByPriority(next);
			// Loop through backwards: see documentation on unit_type::unit_type().
			for (auto itr = units.rbegin(), enditr = units.rend(); itr != enditr;
				++itr) {
				for (auto& unit : itr->second) {
					auto type = _map->getUnitType(unit);
					auto pos = _map->getUnitPosition(unit);
					_scripts->callFunction("BeginTurnForUnit", unit,
						const_cast<awe::unit_type*>(type.get()), &pos);
				}
			}
		}
		auto tiles = _map->getTilesOfArmy(next);
		if (_scripts->functionDeclExists("void BeginTurnForTile(const Vector2&in, "
			"const Terrain&in, const ArmyID)")) {
			// Have to make tile copies here, otherwise callFunction won't work.
			for (auto tile : tiles) {
				auto type = _map->getTileType(tile)->getType();
				_scripts->callFunction("BeginTurnForTile", &tile,
					const_cast<awe::terrain*>(type.get()), next);
			}
		}
		if (_scripts->functionDeclExists("void BeginTurnForArmy("
			"const ArmyID, const array<UnitID>@, const array<Vector2>@)")) {
			auto units = _map->getUnitsOfArmy(next);
			CScriptArray* tilesArray = _scripts->createArray("Vector2");
			CScriptArray* unitsArray = _scripts->createArray("UnitID");
			if (!tilesArray || !unitsArray) throw INVALID_ARRAY;
			for (auto tile : tiles) tilesArray->InsertLast(&tile);
			for (auto unit : units) unitsArray->InsertLast(&unit);
			_scripts->callFunction("BeginTurnForArmy", next, unitsArray,
				tilesArray);
			unitsArray->Release();
			tilesArray->Release();
		}
	} else {
		throw NO_MAP;
	}
}

void awe::game::healUnit(const awe::UnitID unit, awe::HP hp) {
	if (_map) {
		if (hp <= 0) {
			throw std::runtime_error("HP value can't be below 0 for healUnit() "
				"operation");
		}
		auto type = _map->getUnitType(unit);
		if (type) {
			const unsigned int maxHP = _map->getUnitType(unit)->getMaxHP();
			awe::HP newHP = _map->getUnitHP(unit) + hp;
			if ((unsigned int)newHP > maxHP) {
				hp -= newHP - maxHP;
				newHP = (awe::HP)maxHP;
			}
			// See if the army can afford the heal. If not, attempt to heal 1 less.
			// If that doesn't work, keep going until we hit 0. At which point,
			// simply heal the unit back to "full health" for that HP, i.e. set an
			// intenal HP of 57 to 60 and don't charge anything.
			const awe::ArmyID currentArmy = _map->getSelectedArmy();
			const awe::Funds currentFunds = _map->getArmyFunds(currentArmy);
			while (true) {
				awe::Funds charge = type->getCost() /
					awe::unit_type::getDisplayedHP(type->getMaxHP()) *
					awe::unit_type::getDisplayedHP(hp);
				if (hp <= 0) {
					// Get internal HP. Then convert it to user HP. Finally,
					// converting it back into internal HP should return the full
					// HP amount.
					_map->setUnitHP(unit, awe::unit_type::getInternalHP(
						awe::unit_type::getDisplayedHP(
							_map->getUnitHP(unit))));
					break;
				} else if (charge > currentFunds) {
					hp -= awe::unit_type::getInternalHP(1);
					newHP -= awe::unit_type::getInternalHP(1);
				} else {
					_map->setUnitHP(unit, newHP);
					_map->setArmyFunds(currentArmy, currentFunds - charge);
					break;
				}
			}
		} else {
			throw INVALID_UNIT_ID;
		}
	} else {
		throw NO_MAP;
	}
}

void awe::game::replenishUnit(const awe::UnitID unit) {
	if (_map) {
		auto type = _map->getUnitType(unit);
		if (type) {
			if (!type->hasInfiniteFuel()) {
				_map->setUnitFuel(unit, type->getMaxFuel());
			}
			if (!type->hasInfiniteAmmo()) {
				_map->setUnitAmmo(unit, type->getMaxAmmo());
			}
		} else {
			throw INVALID_UNIT_ID;
		}
	} else {
		throw NO_MAP;
	}
}

CScriptArray* awe::game::getAdjacentUnits(const sf::Vector2u& position,
	const unsigned int startFrom, const unsigned int endAt) const {
	if (_map) {
		if (_scripts) {
			CScriptArray* ret = _scripts->createArray("UnitID");
			if (ret) {
				std::unordered_set<sf::Vector2u> positions =
					_map->getAvailableTiles(position, startFrom, endAt);
				for (auto& pos : positions) {
					auto id = _map->getUnitOnTile(pos);
					if (id) ret->InsertLast(&id);
				}
				return ret;
			} else {
				throw INVALID_ARRAY;
			}
		}
		throw NO_SCRIPTS;
	}
	throw NO_MAP;
}

void awe::game::burnFuel(const awe::UnitID unit, const awe::Fuel fuel) {
	if (_map) {
		auto currentFuel = _map->getUnitFuel(unit);
		_map->setUnitFuel(unit, currentFuel - fuel);
	} else {
		throw NO_MAP;
	}
}

void awe::game::deleteUnit(const awe::UnitID unit) {
	if (_map) {
		const auto army = _map->getArmyOfUnit(unit);
		if (army != awe::army::NO_ARMY) {
			if (army == _map->getSelectedArmy()) {
				_map->deleteUnit(unit);
			} else {
				throw std::runtime_error("You cannot delete units of a different "
					"army than the current army!");
			}
		} else {
			throw INVALID_UNIT_ID;
		}
	} else {
		throw NO_MAP;
	}
}

void awe::game::offsetFunds(const awe::ArmyID army, const awe::Funds funds) {
	if (_map) {
		const auto currentFunds = _map->getArmyFunds(army);
		if (currentFunds >= 0) {
			_map->setArmyFunds(army, currentFunds + funds);
		} else {
			throw std::runtime_error("The army does not exist!");
		}
	} else {
		throw NO_MAP;
	}
}

bool awe::game::buyUnit(const awe::BankID type) {
	if (_map) {
		const auto army = _map->getSelectedArmy();
		std::shared_ptr<const awe::unit_type> unitType = (*_unitBank)[type];
		const awe::Funds newFunds = _map->getArmyFunds(army) - unitType->getCost();
		if (newFunds < 0) return false;
		auto id = _map->createUnit(unitType, army);
		_map->setUnitPosition(id, _map->getSelectedTile());
		_map->setArmyFunds(army, newFunds);
		_map->setUnitHP(id, unitType->getMaxHP());
		if (!unitType->hasInfiniteFuel())
			_map->setUnitFuel(id, unitType->getMaxFuel());
		if (!unitType->hasInfiniteAmmo())
			_map->setUnitAmmo(id, unitType->getMaxAmmo());
		return true;
	} else {
		throw NO_MAP;
	}
	return false;
}

void awe::game::enableMoveMode() {
	if (_map) {
		//if (_map->selectedUnitRenderData.selectedUnit == 0) {
			_map->selectedUnitRenderData.selectedUnit =
				_map->getUnitOnTile(_map->getSelectedTile());
			_map->selectedUnitRenderData.availableTileShader =
				awe::selected_unit_render_data::shader::Yellow;
			_map->selectedUnitRenderData.closedList.push_back(
				{ _map->getSelectedTile(), "" });
			_map->selectedUnitRenderData.renderUnitAtDestination = false;
			// Filter the available tiles down based on the unit's movement type
			// and fuel.
			const auto unitType =
				_map->getUnitType(_map->selectedUnitRenderData.selectedUnit);
			auto allTiles = _map->getAvailableTiles(_map->getSelectedTile(), 1,
				unitType->getMovementPoints());
			for (auto& tile : allTiles) {
				if (!_findPath(_map->selectedUnitRenderData.closedList.back().tile,
					tile, unitType->getMovementType(),
					_map->getUnitFuel(_map->selectedUnitRenderData.selectedUnit)).
					empty()) {
					_map->selectedUnitRenderData.availableTiles.insert(tile);
				}
			}
			_map->selectedUnitRenderData.availableTiles.insert(
				_map->selectedUnitRenderData.closedList.back().tile);
		//} else {
		//	throw std::runtime_error("A unit is already in move mode!");
		//}
	} else {
		throw NO_MAP;
	}
}

void awe::game::disableMoveMode() {
	if (_map)
		_map->selectedUnitRenderData.clearState();
	else
		throw NO_MAP;
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

sf::Vector2u awe::game::getUnitPosition(const awe::UnitID id) const {
	if (_map)
		return _map->getUnitPosition(id);
	throw NO_MAP;
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
				throw INVALID_ARRAY;
			}
		} else {
			throw NO_SCRIPTS;
		}
	} else {
		throw NO_MAP;
	}
}

awe::Day awe::game::getDay() const {
	if (_map)
		return _map->getDay();
	throw NO_MAP;
}

awe::ArmyID awe::game::getCurrentArmy() const {
	if (_map)
		return _map->getSelectedArmy();
	throw NO_MAP;
}

//////////////////////////////
// INTERFACE HELPER METHODS //
//////////////////////////////

struct open_list_node {
	std::shared_ptr<open_list_node> parent = nullptr;
	sf::Vector2u tile;
	int g = 0;
	int h = 0;
	inline int f() const noexcept { return g + h; }
};

open_list_node tileWithLowestFScore(
	const std::unordered_set<open_list_node>& openList) {
	open_list_node ret = *openList.begin();
	for (auto& node : openList) {
		if (ret.f() > node.f()) ret = node;
	}
	return ret;
}

std::vector<awe::closed_list_node> awe::game::_findPath(const sf::Vector2u& origin,
	const sf::Vector2u& dest,
	const std::shared_ptr<const awe::movement_type>& moveType,
	const awe::Fuel fuel) {
	// openSet could be a min-heap or priority queue.
	std::unordered_set<sf::Vector2u> openSet = { origin };
	std::unordered_map<sf::Vector2u, sf::Vector2u> cameFrom;
	std::unordered_map<sf::Vector2u, int> gScore = { {origin, 0} };
	std::unordered_map<sf::Vector2u, int> fScore = { {origin, 0} };

	while (!openSet.empty()) {
		bool firstElement = true;
		sf::Vector2u currentTile;
		for (auto& node : openSet) {
			if (firstElement) {
				firstElement = false;
				currentTile = node;
			} else if (fScore.at(node) < fScore.at(currentTile)) {
				currentTile = node;
			}
		}

		if (currentTile == dest) {
			// Path found - also figure out which arrow icons to draw here.
			std::vector<awe::closed_list_node> ret = { { currentTile, "" } };
			while (cameFrom.find(currentTile) != cameFrom.end()) {
				currentTile = cameFrom[currentTile];
				ret.insert(ret.begin(), { currentTile, "" });
			}
			return ret;
		}

		openSet.erase(currentTile);
		auto adjacentTiles = _map->getAvailableTiles(currentTile, 1, 1);
		for (auto& adjacentTile : adjacentTiles) {
			auto moveCost =
				_map->getTileType(adjacentTile)->getType()->
				getMoveCost(moveType->getID());
			int tentativeGScore = gScore[currentTile] + moveCost;

			// If the unit does not have enough fuel, or if its movement type
			// cannot traverse the adjacent tile's terrain, then it cannot traverse
			// the tile, so don't add it to the open set.
			if (moveCost < 0 || tentativeGScore <= fuel) {
				if (gScore.find(adjacentTile) == gScore.end() ||
					tentativeGScore < gScore[adjacentTile]) {
					cameFrom[adjacentTile] = currentTile;
					gScore[adjacentTile] = tentativeGScore;
					fScore[adjacentTile] =
						tentativeGScore + awe::distance(adjacentTile, dest);
					if (openSet.find(adjacentTile) == openSet.end()) {
						openSet.insert(adjacentTile);
					}
				}
			}
		}
	}

	return {};
}