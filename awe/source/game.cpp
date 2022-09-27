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

void awe::game::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::RegisterGameTypedefs(engine, document);
	engine::RegisterVectorTypes(engine, document);

	auto r = engine->RegisterObjectType("GameInterface", 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	document->DocumentObjectType(r, "Provides access to a game of Computer Wars.\n"
		"A global property called <tt>game</tt> is available to all scripts. If a "
		"map has been loaded, this property can be used to interact with the map. "
		"If a map hasn't been loaded or has been quit from, then functions won't "
		"have any effect, and if they return a value, they will return a blank "
		"value.");

	r = engine->RegisterObjectMethod("GameInterface", "void moveSelectedTileUp()",
		asMETHOD(awe::game, moveSelectedTileUp), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Moves the cursor to the tile above the "
		"tile where the cursor is currently located. If this is not possible, the "
		"call will be ignored.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void moveSelectedTileDown()",
		asMETHOD(awe::game, moveSelectedTileDown), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Moves the cursor to the tile below the "
		"tile where the cursor is currently located. If this is not possible, the "
		"call will be ignored.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void moveSelectedTileLeft()",
		asMETHOD(awe::game, moveSelectedTileLeft), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Moves the cursor to the tile to the left "
		"of the tile where the cursor is currently located. If this is not "
		"possible, the call will be ignored.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void moveSelectedTileRight()",
		asMETHOD(awe::game, moveSelectedTileRight), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Moves the cursor to the tile to the "
		"right of the tile where the cursor is currently located. If this is not "
		"possible, the call will be ignored.");

	r = engine->RegisterObjectMethod("GameInterface", "Vector2 getSelectedTile()",
		asMETHOD(awe::game, getSelectedTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the location of the cursor, in "
		"tiles. The coordinates are 0-based.");

	r = engine->RegisterObjectMethod("GameInterface",
		"UnitID getUnitOnTile(const Vector2)",
		asMETHOD(awe::game, getUnitOnTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Retrieves the ID of the unit on the "
		"specified tile. If 0, then the tile is unoccupied.");

	r = engine->RegisterObjectMethod("GameInterface", "void zoomIn()",
		asMETHOD(awe::game, zoomIn), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Zooms the map in by a scaling factor of "
		"1. The map scaling factor does not go above 3.");

	r = engine->RegisterObjectMethod("GameInterface", "void zoomOut()",
		asMETHOD(awe::game, zoomOut), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Zooms the map out by a scaling factor "
		"of 1. The map scaling factor does not go below 1.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void setSelectedTileByPixel(const MousePosition)",
		asMETHOD(awe::game, setSelectedTileByPixel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Selects a tile based on a given mouse "
		"position. Note that cases of <tt>INVALID_MOUSE</tt>, etc., should be "
		"accounted for within the scripts.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Vector2 getTileSize()",
		asMETHOD(awe::game, getTileSize), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the minimum pixel size of a tile "
		"after scaling has been applied.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const Commander getArmyCurrentCO(const ArmyID)",
		asMETHOD(awe::game, getArmyCurrentCO), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the properties of the army's current "
		"CO.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const Commander getArmyTagCO(const ArmyID)",
		asMETHOD(awe::game, getArmyTagCO), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the properties of the army's tag CO. "
		"<b>Warning:</b> if an army doesn't have a tag CO, the game engine will "
		"throw an exception which will halt script execution! Check if an army "
		"has a tag CO first using <tt>tagCOIsPresent()</tt>.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const Country getArmyCountry(const ArmyID)",
		asMETHOD(awe::game, getArmyCountry), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the properties of the army's "
		"country.");

	r = engine->RegisterObjectMethod("GameInterface",
		"int getArmyFunds(const ArmyID)",
		asMETHOD(awe::game, getArmyFunds), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets an army's fund count.");

	r = engine->RegisterObjectMethod("GameInterface",
		"bool tagCOIsPresent(const ArmyID)",
		asMETHOD(awe::game, tagCOIsPresent), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns <tt>true</tt> if the specified "
		"army has a tag CO, <tt>false</tt> in all other cases.");

	r = engine->RegisterObjectMethod("GameInterface",
		"uint getArmyCount()",
		asMETHOD(awe::game, getArmyCount), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the number of armies currently in "
		"play.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const TileType getTileType(const Vector2&in)",
		asMETHOD(awe::game, getTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns properties on a given tile.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const Terrain getTerrainOfTile(const Vector2&in)",
		asMETHOD(awe::game, getTerrainOfTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns properties on a given tile's "
		"terrain type.");

	r = engine->RegisterObjectMethod("GameInterface",
		"ArmyID getTileOwner(const Vector2&in)",
		asMETHOD(awe::game, getTileOwner), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the ArmyID of the army who owns "
		"the specified tile. <tt>NO_ARMY</tt> is returned if either the position "
		"was out of range, or if no army owns the tile.");

	r = engine->RegisterObjectMethod("GameInterface",
		"HP getTileHP(const Vector2&in)",
		asMETHOD(awe::game, getTileHP), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns a tile's current HP.");

	r = engine->RegisterObjectMethod("GameInterface",
		"const UnitType getUnitType(const UnitID)",
		asMETHOD(awe::game, getUnitType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on a unit's type.");

	r = engine->RegisterObjectMethod("GameInterface",
		"ArmyID getArmyOfUnit(const UnitID)",
		asMETHOD(awe::game, getArmyOfUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the ID of the army who owns a "
		"specified unit.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Vector2 getUnitPosition(const UnitID)",
		asMETHOD(awe::game, getUnitPosition), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the tile position of the given "
		"unit.");

	r = engine->RegisterObjectMethod("GameInterface",
		"HP getUnitHP(const UnitID)",
		asMETHOD(awe::game, getUnitDisplayedHP), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the user-friendly HP that a given "
		"unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Fuel getUnitFuel(const UnitID)",
		asMETHOD(awe::game, getUnitFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the fuel that a given unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Ammo getUnitAmmo(const UnitID)",
		asMETHOD(awe::game, getUnitAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the ammo that a given unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"array<UnitID>@ getLoadedUnits(const UnitID) const",
		asMETHOD(awe::game, getLoadedUnits), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the IDs of the units that are "
		"loaded onto the one specified.");

	r = engine->RegisterObjectMethod("GameInterface", "Day getDay() const",
		asMETHOD(awe::game, getDay), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the current day.");

	r = engine->RegisterObjectMethod("GameInterface", "void endTurn()",
		asMETHOD(awe::game, endTurn), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Ends the current army's turn.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void healUnit(const UnitID, HP)",
		asMETHOD(awe::game, healUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Adds HP to a given unit. Ensures that the "
		"unit's HP does not go over its max. The given HP can't be at or below 0. "
		"The given HP should be a user-friendly HP value.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void replenishUnit(const UnitID)",
		asMETHOD(awe::game, replenishUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Replenishes a given unit. Sets its fuel "
		"ammo back up to max. If the unit has infinite fuel, that unit's fuel "
		"won't be changed. Same for the ammo.");

	r = engine->RegisterObjectMethod("GameInterface", "array<UnitID>@ "
		"getAdjacentUnits(const Vector2&in, const uint, const uint) const",
		asMETHOD(awe::game, getAdjacentUnits), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets a list of units that are adjacent to "
		"a given tile. The first integer represents the number of tiles away from "
		"the given tile to start at. The second integer stores the upper limit of "
		"the range from the given tile to consider. For example, passing in 1 and "
		"1 will simply get the directly adjacent units.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void burnFuel(const UnitID, const Fuel)",
		asMETHOD(awe::game, burnFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Burns fuel from a unit.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void deleteUnit(const UnitID)",
		asMETHOD(awe::game, deleteUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Deletes a unit. Only a unit belonging to "
		"the current army can be deleted.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void offsetFunds(const ArmyID, const Funds)",
		asMETHOD(awe::game, offsetFunds), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Awards or takes away funds to/from an "
		"army.");

	r = engine->RegisterObjectMethod("GameInterface",
		"ArmyID getCurrentArmy() const",
		asMETHOD(awe::game, getCurrentArmy), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Retrieves the ID of the army who is having "
		"their turn.");

	r = engine->RegisterObjectMethod("GameInterface",
		"bool isUnitWaiting(const UnitID) const",
		asMETHOD(awe::game, isUnitWaiting), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns <tt>TRUE</tt> if a unit is "
		"waiting, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterObjectMethod("GameInterface",
		"bool buyUnit(const BankID)",
		asMETHOD(awe::game, buyUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Buys a unit for the current army and "
		"places it at the current cursor location.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void enableMoveMode()",
		asMETHOD(awe::game, enableMoveMode), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Enables move mode for the unit on the "
		"currently selected tile.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void disableMoveMode()",
		asMETHOD(awe::game, disableMoveMode), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Disables move mode for the unit currently "
		"in move mode.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void togglePreviewMoveMode(const bool)",
		asMETHOD(awe::game, togglePreviewMoveMode), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Temporarily moves the unit in move mode to "
		"the currently selected tile (but does not actually update the unit's "
		"position), if given <tt>TRUE</tt>.");

	r = engine->RegisterObjectMethod("GameInterface",
		"void moveUnit()",
		asMETHOD(awe::game, moveUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Moves the currently selected unit along "
		"the chosen path and makes it wait.");

	r = engine->RegisterObjectMethod("GameInterface",
		"UnitID getMovingUnit()",
		asMETHOD(awe::game, getMovingUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the ID of the unit that is currently "
		"in move mode. <tt>0</tt> if no unit is in move mode.");

	r = engine->RegisterObjectMethod("GameInterface",
		"TeamID getArmyTeam(const ArmyID)",
		asMETHOD(awe::game, getArmyTeam), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the team that the given army belongs "
		"to.");

	// Register game global property and related constants.
	r = engine->RegisterGlobalProperty("const ArmyID NO_ARMY",
		&awe::army::NO_ARMY_SCRIPT);
	document->DocumentExpectedFunction("const ArmyID NO_ARMY", "Represents \"no "
		"army\". Used to signify \"no ownership.\"");
	r = engine->RegisterGlobalProperty("GameInterface game", this);
}

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
		_iconSheet = icon_sheet;
		_map->setTileSpritesheet(tile_sheet);
		_map->setUnitSpritesheet(unit_sheet);
		_map->setIconSpritesheet(icon_sheet);
		_map->setCOSpritesheet(co_sheet);
		_map->setFont(font);
		_map->setLanguageDictionary(dict);
		auto r = _map->load(file);
		// If we have any game options, apply them.
		if (options) {
			// Apply overrides to armies.
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
				try {
					_map->setArmyTeam(ID, options->getTeam(ID));
				} catch (const std::range_error& e) {
					std::string msg(e.what());
					if (!msg.empty()) {
						_logger.error("Couldn't override team for army {}: {}",
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
		// Before selecting the next army, ensure that each of the previous army's
		// units are no longer in the waiting state.
		const auto previousArmyUnits =
			_map->getUnitsOfArmy(_map->getSelectedArmy());
		for (auto unit : previousArmyUnits) {
			_map->waitUnit(unit, false);
		}
		// Update the current army.
		const auto next = _map->getNextArmy();
		if (_map->getSelectedArmy() >= next) {
			// It can technically overflow, but I don't care! If they can play for
			// over 11,767,033 in-game years then they deserve what they get.
			_map->setDay(_map->getDay() + 1);
		}
		_map->selectArmy(next);
		// Begin next army's turn.
		if (_scripts->functionDeclExists("void BeginTurnForUnit(const UnitID, "
			"const UnitType&in, const Vector2&in)")) {
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
			awe::HP newHP = _map->getUnitHP(unit) +
				awe::unit_type::getInternalHP(hp);
			if ((unsigned int)newHP > maxHP) {
				hp -= awe::unit_type::getDisplayedHP(newHP - maxHP);
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
					awe::unit_type::getDisplayedHP(type->getMaxHP()) * hp;
				if (hp <= 0) {
					// Get internal HP. Then convert it to user HP. Finally,
					// converting it back into internal HP should return the full
					// HP amount.
					_map->setUnitHP(unit, awe::unit_type::getInternalHP(
						awe::unit_type::getDisplayedHP(
							_map->getUnitHP(unit))));
					break;
				} else if (charge > currentFunds) {
					hp -= 1;
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
		if (_map->selectedUnitRenderData.selectedUnit == 0) {
			_map->selectedUnitRenderData.selectedUnit =
				_map->getUnitOnTile(_map->getSelectedTile());
			if (_map->selectedUnitRenderData.selectedUnit == 0) {
				throw std::runtime_error("No unit is on the currently selected "
					"tile!");
			}
			_map->selectedUnitRenderData.availableTileShader =
				awe::selected_unit_render_data::shader::Yellow;
			_map->selectedUnitRenderData.closedList.push_back(
				{ _map->getSelectedTile(), { nullptr, ""}});
			_map->selectedUnitRenderData.renderUnitAtDestination = false;
			// Filter the available tiles down based on the unit's movement type,
			// movement points, and fuel.
			const auto unitType =
				_map->getUnitType(_map->selectedUnitRenderData.selectedUnit);
			auto allTiles = _map->getAvailableTiles(_map->getSelectedTile(), 1,
				unitType->getMovementPoints());
			for (auto& tile : allTiles) {
				if (!_findPath(_map->selectedUnitRenderData.closedList.back().tile,
					tile, unitType->getMovementType(),
					unitType->getMovementPoints(),
					_map->getUnitFuel(_map->selectedUnitRenderData.selectedUnit),
					_map->getTeamOfUnit(_map->selectedUnitRenderData.selectedUnit))
					.empty()) {
					_map->selectedUnitRenderData.availableTiles.insert(tile);
				}
			}
			_map->selectedUnitRenderData.availableTiles.insert(
				_map->selectedUnitRenderData.closedList.back().tile);
		} else {
			throw std::runtime_error("A unit is already in move mode!");
		}
	} else {
		throw NO_MAP;
	}
}

void awe::game::disableMoveMode() {
	if (_map) {
		_map->selectedUnitRenderData.clearState();
	} else {
		throw NO_MAP;
	}
}

void awe::game::togglePreviewMoveMode(const bool preview) {
	if (_map) {
		if (_map->selectedUnitRenderData.selectedUnit > 0) {
			if (_map->selectedUnitRenderData.availableTiles.find(
				_map->getSelectedTile()) !=
				_map->selectedUnitRenderData.availableTiles.end()) {
				_map->selectedUnitRenderData.renderUnitAtDestination = preview;
			}
		} else {
			throw std::runtime_error("A unit is not in move mode!");
		}
	} else {
		throw NO_MAP;
	}
}

void awe::game::moveUnit() {
	if (_map) {
		if (_map->selectedUnitRenderData.selectedUnit > 0) {
			// Burn the right amount of fuel, update the unit's position, and make
			// it wait.
			const auto id = _map->selectedUnitRenderData.selectedUnit;
			const auto& node = _map->selectedUnitRenderData.closedList.back();
			_map->setUnitFuel(id, _map->getUnitFuel(id) - node.g);
			_map->setUnitPosition(id, node.tile);
			_map->waitUnit(id, true);
			_map->selectedUnitRenderData.clearState();
		} else {
			throw std::runtime_error("A unit is not in move mode!");
		}
	} else {
		throw NO_MAP;
	}
}

//////////////////////
// SCRIPT INTERFACE //
//////////////////////

void awe::game::moveSelectedTileUp() {
	if (_map) {
		_map->moveSelectedTileUp();
		_updateMoveModeClosedList();
	} else {
		throw NO_MAP;
	}
}

void awe::game::moveSelectedTileDown() {
	if (_map) {
		_map->moveSelectedTileDown();
		_updateMoveModeClosedList();
	} else {
		throw NO_MAP;
	}
}

void awe::game::moveSelectedTileLeft() {
	if (_map) {
		_map->moveSelectedTileLeft();
		_updateMoveModeClosedList();
	} else {
		throw NO_MAP;
	}
}

void awe::game::moveSelectedTileRight() {
	if (_map) {
		_map->moveSelectedTileRight();
		_updateMoveModeClosedList();
	} else {
		throw NO_MAP;
	}
}

sf::Vector2u awe::game::getSelectedTile() const {
	if (_map) {
		return _map->getSelectedTile();
	} else {
		throw NO_MAP;
	}
}

awe::UnitID awe::game::getUnitOnTile(const sf::Vector2u tile) const {
	if (_map) {
		return _map->getUnitOnTile(tile);
	} else {
		throw NO_MAP;
	}
}

void awe::game::setSelectedTileByPixel(const sf::Vector2i pixel) {
	if (_map) {
		_map->setSelectedTileByPixel(pixel);
		_updateMoveModeClosedList();
	} else {
		throw NO_MAP;
	}
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
	if (_map) {
		return _map->getUnitPosition(id);
	}
	throw NO_MAP;
}

awe::HP awe::game::getUnitDisplayedHP(const awe::UnitID id) const {
	if (_map) {
		return _map->getUnitDisplayedHP(id);
	} else {
		throw NO_MAP;
	}
}

awe::Fuel awe::game::getUnitFuel(const awe::UnitID id) const {
	if (_map) {
		return _map->getUnitFuel(id);
	} else {
		throw NO_MAP;
	}
}

awe::Ammo awe::game::getUnitAmmo(const awe::UnitID id) const {
	if (_map) {
		return _map->getUnitAmmo(id);
	} else {
		throw NO_MAP;
	}
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

bool awe::game::isUnitWaiting(const awe::UnitID id) const {
	if (_map) {
		return _map->isUnitWaiting(id);
	}
	throw NO_MAP;
}

awe::UnitID awe::game::getMovingUnit() const {
	if (_map) {
		return _map->selectedUnitRenderData.selectedUnit;
	} else {
		throw NO_MAP;
	}
}

awe::TeamID awe::game::getArmyTeam(const awe::ArmyID id) const {
	if (_map) {
		return _map->getArmyTeam(id);
	} else {
		throw NO_MAP;
	}
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
	const unsigned int movePoints, const awe::Fuel fuel, const awe::TeamID team) {
	// openSet could be a min-heap or priority queue for added efficiency.
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
			// Path found.
			std::vector<awe::closed_list_node> ret =
				{ { currentTile, { _iconSheet, "" }, gScore[currentTile]} };
			while (cameFrom.find(currentTile) != cameFrom.end()) {
				currentTile = cameFrom[currentTile];
				ret.insert(ret.begin(), { currentTile, { _iconSheet, "" },
					gScore[currentTile] });
			}
			_updateClosedListArrowIcons(ret);
			return ret;
		}

		openSet.erase(currentTile);
		auto adjacentTiles = _map->getAvailableTiles(currentTile, 1, 1);
		for (auto& adjacentTile : adjacentTiles) {
			// Get the movement cost for this terrain.
			const auto moveCost =
				_map->getTileType(adjacentTile)->getType()->
				getMoveCost(moveType->getID());

			// If this unit cannot traverse the terrain, do not add it to any set.
			if (moveCost < 0) continue;

			int tentativeGScore = gScore[currentTile] + moveCost;

			// If:
			// 1. The unit does not have enough fuel.
			// 2. The unit has ran out of movement points.
			// 3. The tile has a unit belonging to an opposing team.
			// then it cannot traverse the tile, so don't add it to the open set.
			const auto unitOnAdjacentTile = _map->getUnitOnTile(adjacentTile);
			if (tentativeGScore <= fuel &&
				(unsigned int)tentativeGScore <= movePoints &&
				(unitOnAdjacentTile == 0 || ( unitOnAdjacentTile != 0 &&
					_map->getTeamOfUnit(unitOnAdjacentTile) == team))) {
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

void awe::game::_updateMoveModeClosedList() noexcept {
	// If in move mode, and the selection has changed, and the selection is of an
	// available tile, then attempt to append the currently selected tile to the
	// closed list. If the path won't work, then deduce the shortest path and use
	// that instead.
	const auto tile = _map->getSelectedTile();
	if (_map->selectedUnitRenderData.selectedUnit > 0 &&
		_map->selectedUnitRenderData.availableTiles.find(tile) !=
		_map->selectedUnitRenderData.availableTiles.end()) {
		const auto unitID = _map->selectedUnitRenderData.selectedUnit;
		const auto unitType = _map->getUnitType(unitID);

		// If the closed list is empty, fallback on the _findPath() method.
		if (_map->selectedUnitRenderData.closedList.empty()) {
			_map->selectedUnitRenderData.closedList = _findPath(
				_map->getUnitPosition(
					_map->selectedUnitRenderData.selectedUnit),
				tile,
				unitType->getMovementType(),
				unitType->getMovementPoints(),
				_map->getUnitFuel(_map->selectedUnitRenderData.selectedUnit),
				_map->getTeamOfUnit(_map->selectedUnitRenderData.selectedUnit)
			);

		} else if (tile != _map->selectedUnitRenderData.closedList.back().tile) {
			// If any distance between any two adjacent tiles in the closed list is
			// more than one, then we need to fix up the list. The easiest way to
			// do this is to just find the shortest path. The closed list can get
			// into this state if the mouse is moved very quickly, for example.
			for (std::size_t i = 1;
				i < _map->selectedUnitRenderData.closedList.size(); ++i) {
				if (awe::distance(
					_map->selectedUnitRenderData.closedList.at(i - 1).tile,
					_map->selectedUnitRenderData.closedList.at(i).tile) > 1) {
					_map->selectedUnitRenderData.closedList = _findPath(
						_map->getUnitPosition(
							_map->selectedUnitRenderData.selectedUnit),
						tile,
						unitType->getMovementType(),
						unitType->getMovementPoints(),
						_map->getUnitFuel(
							_map->selectedUnitRenderData.selectedUnit),
						_map->getTeamOfUnit(
							_map->selectedUnitRenderData.selectedUnit)
					);
					return;
				}
			}

			// If a tile has been selected that's already in the closed list, then
			// simply remove all tiles from the closed list that come after it.
			for (auto itr = _map->selectedUnitRenderData.closedList.begin(),
				enditr = _map->selectedUnitRenderData.closedList.end();
				itr != enditr; ++itr) {
				if (itr->tile == tile) {
					_map->selectedUnitRenderData.closedList.erase(itr + 1, enditr);
					_updateClosedListArrowIcons(
						_map->selectedUnitRenderData.closedList);
					return;
				}
			}

			const auto moveCost = _map->getTileType(tile)->getType()->
				getMoveCost(unitType->getMovementType()->getID());
			const auto tentativeGScore =
				_map->selectedUnitRenderData.closedList.back().g + moveCost;

			if (tentativeGScore <= _map->getUnitFuel(unitID) &&
				(unsigned int)tentativeGScore <= unitType->getMovementPoints()) {
				_map->selectedUnitRenderData.closedList.push_back({ tile,
					{ _iconSheet, "" }, tentativeGScore });
				_updateClosedListArrowIcons(
					_map->selectedUnitRenderData.closedList);
				// Because we've just appended a new node without checking it
				// thoroughly, the user could have moved their cursor off the
				// available tiles and joined back into the available tiles again
				// to a form a completely disjointed path. So we need to carry out
				// distance checking again!
				for (std::size_t i = 1;
					i < _map->selectedUnitRenderData.closedList.size(); ++i) {
					if (awe::distance(
						_map->selectedUnitRenderData.closedList.at(i - 1).tile,
						_map->selectedUnitRenderData.closedList.at(i).tile) > 1) {
						_map->selectedUnitRenderData.closedList = _findPath(
							_map->getUnitPosition(
								_map->selectedUnitRenderData.selectedUnit),
							tile,
							unitType->getMovementType(),
							unitType->getMovementPoints(),
							_map->getUnitFuel(
								_map->selectedUnitRenderData.selectedUnit),
							_map->getTeamOfUnit(
								_map->selectedUnitRenderData.selectedUnit)
						);
						return;
					}
				}
			} else {
				_map->selectedUnitRenderData.closedList = _findPath(
					_map->getUnitPosition(
						_map->selectedUnitRenderData.selectedUnit),
					tile,
					unitType->getMovementType(),
					unitType->getMovementPoints(),
					_map->getUnitFuel(_map->selectedUnitRenderData.selectedUnit),
					_map->getTeamOfUnit(_map->selectedUnitRenderData.selectedUnit)
				);
			}
		}
	}
}

void awe::game::_updateClosedListArrowIcons(
	std::vector<awe::closed_list_node>& ret) const noexcept {
	// Starting from the beginning; calculate the arrow sprites to draw for
	// each tile.
	for (std::size_t i = 0; i < ret.size(); ++i) {
		if (i == 0) {
			ret[i].sprite.setSpritesheet(nullptr);
		} else if (i == ret.size() - 1) {
			if (ret[i - 1].tile.x < ret[i].tile.x) {
				ret[i].sprite.setSprite("unitArrowRight");
			} else if (ret[i - 1].tile.x > ret[i].tile.x) {
				ret[i].sprite.setSprite("unitArrowLeft");
			} else if (ret[i - 1].tile.y < ret[i].tile.y) {
				ret[i].sprite.setSprite("unitArrowDown");
			} else if (ret[i - 1].tile.y > ret[i].tile.y) {
				ret[i].sprite.setSprite("unitArrowUp");
			}
		} else {
			if ((ret[i - 1].tile.x < ret[i].tile.x &&
				ret[i].tile.x < ret[i + 1].tile.x) ||
				(ret[i - 1].tile.x > ret[i].tile.x &&
					ret[i].tile.x > ret[i + 1].tile.x)) {
				ret[i].sprite.setSprite("unitArrowHori");
			} else if ((ret[i - 1].tile.y < ret[i].tile.y &&
				ret[i].tile.y < ret[i + 1].tile.y) ||
				(ret[i - 1].tile.y > ret[i].tile.y &&
					ret[i].tile.y > ret[i + 1].tile.y)) {
				ret[i].sprite.setSprite("unitArrowVert");
			} else if ((ret[i - 1].tile.y < ret[i].tile.y &&
				ret[i].tile.x < ret[i + 1].tile.x) ||
				(ret[i - 1].tile.x > ret[i].tile.x &&
					ret[i].tile.y > ret[i + 1].tile.y)) {
				ret[i].sprite.setSprite("unitArrowNE");
			} else if ((ret[i - 1].tile.y > ret[i].tile.y &&
				ret[i].tile.x < ret[i + 1].tile.x) ||
				(ret[i - 1].tile.x > ret[i].tile.x &&
					ret[i].tile.y < ret[i + 1].tile.y)) {
				ret[i].sprite.setSprite("unitArrowSE");
			} else if ((ret[i - 1].tile.x < ret[i].tile.x &&
				ret[i].tile.y > ret[i + 1].tile.y) ||
				(ret[i - 1].tile.y < ret[i].tile.y &&
					ret[i].tile.x > ret[i + 1].tile.x)) {
				ret[i].sprite.setSprite("unitArrowNW");
			} else if ((ret[i - 1].tile.x < ret[i].tile.x &&
				ret[i].tile.y < ret[i + 1].tile.y) ||
				(ret[i - 1].tile.y > ret[i].tile.y &&
					ret[i].tile.x > ret[i + 1].tile.x)) {
				ret[i].sprite.setSprite("unitArrowSW");
			}
		}
	}
}