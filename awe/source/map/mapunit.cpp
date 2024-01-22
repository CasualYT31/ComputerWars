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

#include "map.hpp"
#include "fmtawe.hpp"

awe::UnitID awe::map::createUnit(const std::shared_ptr<const awe::unit_type>& type,
	const awe::ArmyID army) {
	if (!type) _logger.warning("createUnit warning: creating a unit for army {} "
		"without a type!", army);
	if (!_isArmyPresent(army)) {
		_logger.error("createUnit operation failed: attempted to create \"{}\" "
			"for army with ID {} that didn't exist!",
			((type) ? (type->getName()) : ("[NULL]")), army);
		return awe::NO_UNIT;
	}
	awe::UnitID id;
	try {
		id = _findUnitID();
	} catch (const std::bad_alloc&) {
		_logger.critical("createUnit fatal error: could not generate a unique ID "
			"for a new unit. There are too many units allocated!");
		return awe::NO_UNIT;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::CREATE_UNIT));
	// TODO-2.
	_units.insert({ id, unit_data({ _logger.getData().sink, "unit" },
		[&](const std::function<void(void)>& func) { _animationQueue.push(func); },
		type, army, (*_sheets)[type->getIdleSpritesheet()], (*_sheets)["icon"])});
	_armies.at(army).addUnit(id);
	return id;
}

awe::UnitID awe::map::createUnit(const std::string& type, const awe::ArmyID army) {
	return createUnit(_unitTypes->operator[](type), army);
}

void awe::map::deleteUnit(const awe::UnitID id) {
	if (!_isUnitPresent(id)) {
		_logger.error("deleteUnit operation cancelled: attempted to delete unit "
			"with ID {} that didn't exist!", id);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::DELETE_UNIT));
	_updateCapturingUnit(id);
	// Firstly, remove the unit from the tile, if it was on a tile.
	// We don't need to check if the unit "is actually on the map or not," since
	// the tile will always hold the index to the unit in either case: which is why
	// we need the "actually" check to begin with.
	const auto position = _units.at(id).data.getPosition();
	if (!_isOutOfBounds(position))
		_tiles[position.x][position.y].data.setUnit(awe::NO_UNIT);
	// Secondly, remove the unit from the army's list.
	if (_isArmyPresent(_units.at(id).data.getArmy())) {
		_armies.at(_units.at(id).data.getArmy()).removeUnit(id);
	} else {
		_logger.warning("deleteUnit warning: unit with ID {} didn't have a valid "
			"owning army ID, which was {}.", id, _units.at(id).data.getArmy());
	}
	// Thirdly, delete all units that are loaded onto this one.
	const auto loaded = _units.at(id).data.loadedUnits();
	for (const auto unit : loaded) deleteUnit(unit);
	// Fourthly, if this unit was loaded onto another, remove it from that unit's
	// list.
	const auto loadedOnto = _units.at(id).data.loadedOnto();
	if (loadedOnto != awe::NO_UNIT && !_units.at(loadedOnto).data.unloadUnit(id)) {
		_logger.warning("deleteUnit warning: unit with ID {}, that is being "
			"deleted, was loaded onto unit with ID {}, but the former could not "
			"be unloaded from the latter!", id, loadedOnto);
	}
	// Fifthly, if this unit was selected, deselect it if it's on top of the
	// stack. If it is further down the stack, then it will have to be removed
	// later: see popSelectedUnit().
	if (getSelectedUnit() == id) setSelectedUnit(awe::NO_UNIT);
	// Sixthly, animate the destroyed unit now, if it has a position on the map.
	// Retain the unit's sprite and location override as it may not be destroyed
	// immediately. However, if there will be no destroy unit animation, remove the
	// location override immediately.
	if (loadedOnto == awe::NO_UNIT) {
		_unitsBeingDestroyed.insert({ id, _units.at(id).sprite });
		_animationQueue.push(std::bind([&](const awe::UnitID deletingID) {
			// Remove the sprite now.
			if (isPreviewUnit(deletingID)) removePreviewUnit(deletingID);
			_unitsBeingDestroyed.erase(deletingID);
		}, id));
		// TODO-2.
		const auto type = _units.at(id).data.getType();
		animateParticle(position, "particle", type->getDestroyedUnit(
			_units.at(id).data.getArmy()), { 0.5f, 1.0f }, "sound",
			type->getDestroySound());
	} else {
		if (isPreviewUnit(id)) removePreviewUnit(id);
	}
	// Finally, delete the unit from the main list.
	_units.erase(id);
}

std::shared_ptr<const awe::unit_type> awe::map::getUnitType(
	const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getType();
	_logger.error("getUnitType operation failed: unit with ID {} doesn't exist!",
		id);
	return nullptr;
}

const awe::unit_type* awe::map::getUnitTypeObject(const awe::UnitID id) const {
	auto ret = getUnitType(id);
	if (ret) {
		return ret.get();
	} else {
		throw std::out_of_range("This unit does not exist!");
	}
}

void awe::map::setUnitPosition(const awe::UnitID id, const sf::Vector2u& pos) {
	if (!_isUnitPresent(id)) {
		_logger.error("setUnitPosition operation cancelled: unit with ID {} "
			"doesn't exist!", id);
		return;
	}
	if (_isOutOfBounds(pos) && pos != awe::unit::NO_POSITION) {
		_logger.error("setUnitPosition operation cancelled: attempted to move "
			"unit with ID {} to position {}, which is out of bounds with the "
			"map's size {}!", id, pos, getMapSize());
		return;
	}
	const auto idOfUnitOnTile = ((pos == awe::unit::NO_POSITION) ? (awe::NO_UNIT) :
		(getUnitOnTile(pos)));
	if (idOfUnitOnTile == id) {
		// If the unit's position is being set to the tile it is on, then drop the
		// call.
		return;
	} else if (idOfUnitOnTile != awe::NO_UNIT) {
		_logger.error("setUnitPosition operation cancelled: attempted to move "
			"unit with ID {} to position {}, which is currently occupied by unit "
			"with ID {}!", id, pos, idOfUnitOnTile);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::UNIT_POSITION));
	_updateCapturingUnit(id);
	// Make new tile occupied.
	if (pos != awe::unit::NO_POSITION) _tiles[pos.x][pos.y].data.setUnit(id);
	// Make old tile vacant.
	if (_units.at(id).data.isOnMap()) {
		const auto oldLocation = _units.at(id).data.getPosition();
		_tiles[oldLocation.x][oldLocation.y].data.setUnit(awe::NO_UNIT);
	}
	// Assign new location to unit.
	_units.at(id).data.setPosition(pos);
}

sf::Vector2u awe::map::getUnitPosition(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitPosition operation failed: unit with ID {} doesn't "
			"exist!", id);
		return awe::unit::NO_POSITION;
	}
	return _units.at(id).data.getPosition();
}

bool awe::map::isUnitOnMap(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("isUnitOnMap operation failed: unit with ID {} doesn't "
			"exist!", id);
		return false;
	}
	return _units.at(id).data.isOnMap();
}

void awe::map::setUnitHP(const awe::UnitID id, const awe::HP hp) {
	if (_isUnitPresent(id)) {
		if (hp == getUnitHP(id)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_HP));
		_units.at(id).data.setHP(hp);
	} else {
		_logger.error("setUnitHP operation cancelled: attempted to assign HP {} "
			"to unit with ID {}, which doesn't exist!", hp, id);
	}
}

awe::HP awe::map::getUnitHP(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getHP();
	_logger.error("getUnitHP operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

awe::HP awe::map::getUnitDisplayedHP(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getDisplayedHP();
	_logger.error("getUnitDisplayedHP operation failed: unit with ID {} doesn't "
		"exist!", id);
	return 0;
}

void awe::map::setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) {
	if (_isUnitPresent(id)) {
		if (fuel == getUnitFuel(id)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_FUEL));
		_units.at(id).data.setFuel(fuel);
	} else {
		_logger.error("setUnitFuel operation cancelled: attempted to assign fuel "
			"{} to unit with ID {}, which doesn't exist!", fuel, id);
	}
}

void awe::map::burnUnitFuel(const awe::UnitID id, const awe::Fuel fuel) {
	if (_isUnitPresent(id)) {
		setUnitFuel(id, getUnitFuel(id) - fuel);
	} else {
		_logger.error("burnUnitFuel operation cancelled: attempted to offset unit "
			"{}'s fuel by {}. This unit doesn't exist!", id, fuel);
	}
}

awe::Fuel awe::map::getUnitFuel(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getFuel();
	_logger.error("getUnitFuel operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

void awe::map::setUnitAmmo(const awe::UnitID id, const std::string& weapon,
	const awe::Ammo ammo) {
	if (_isUnitPresent(id)) {
		if (ammo == getUnitAmmo(id, weapon)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_AMMO));
		_units.at(id).data.setAmmo(weapon, ammo);
	} else {
		_logger.error("setUnitAmmo operation cancelled: attempted to assign ammo "
			"{} to unit with ID {}'s weapon \"{}\". This unit doesn't exist!",
			ammo, id, weapon);
	}
}

awe::Ammo awe::map::getUnitAmmo(const awe::UnitID id,
	const std::string& weapon) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getAmmo(weapon);
	_logger.error("getUnitAmmo operation with weapon \"{}\" failed: unit with ID "
		"{} doesn't exist!", weapon, id);
	return 0;
}

unsigned int awe::map::getUnitVision(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitVision operation failed: unit with ID {} doesn't "
			"exist!", id);
		return 0;
	}
	const auto& unit = _units.at(id).data;
	const auto vision = unit.getType()->getVision();
	const auto position = unit.getPosition();
	if (position == awe::unit::NO_POSITION) return vision;
	const auto terrain =
		_tiles[position.x][position.y].data.getTileType()->getType();
	return static_cast<unsigned int>(std::max(1, static_cast<int>(vision) +
		terrain->getVisionOffsetForUnitType(unit.getType()->getScriptName())));
}

void awe::map::replenishUnit(const awe::UnitID id, const bool heal) {
	if (_isUnitPresent(id)) {
		if (isUnitReplenished(id, heal)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_REPLENISH));
		_units.at(id).data.replenish(heal);
	} else {
		_logger.error("replenishUnit operation cancelled: attempted to replenish "
			"{}unit with ID {}. This unit doesn't exist!", heal ? "and heal " : "",
			id);
	}
}

bool awe::map::isUnitReplenished(const awe::UnitID id, const bool hp) const {
	if (_isUnitPresent(id)) return _units.at(id).data.isReplenished(hp);
	_logger.error("isUnitReplenished operation failed: unit with ID {} doesn't "
		"exist!", id);
	return false;
}

void awe::map::waitUnit(const awe::UnitID id, const bool waiting) {
	if (_isUnitPresent(id)) {
		if (waiting == isUnitWaiting(id)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_WAIT));
		_units.at(id).data.wait(waiting);
	} else {
		_logger.error("waitUnit operation cancelled: attempted to assign waiting "
			"state {} to unit with ID {}, which doesn't exist!", waiting, id);
	}
}

bool awe::map::isUnitWaiting(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.isWaiting();
	_logger.error("isUnitWaiting operation failed: unit with ID {} doesn't exist!",
		id);
	return false;
}

void awe::map::unitCapturing(const awe::UnitID id, const bool capturing) {
	if (_isUnitPresent(id)) {
		if (capturing == isUnitCapturing(id)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_CAPTURE));
		_units.at(id).data.capturing(capturing);
	} else {
		_logger.error("unitCapturing operation cancelled: attempted to assign "
			"capturing state {} to unit with ID {}, which doesn't exist!",
			capturing, id);
	}
}

bool awe::map::isUnitCapturing(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.isCapturing();
	_logger.error("isUnitCapturing operation failed: unit with ID {} doesn't "
		"exist!", id);
	return false;
}

void awe::map::unitHiding(const awe::UnitID id, const bool hiding) {
	if (_isUnitPresent(id)) {
		if (hiding == isUnitHiding(id)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_HIDE));
		_units.at(id).data.hiding(hiding);
	} else {
		_logger.error("unitHiding operation cancelled: attempted to assign hiding "
			"state {} to unit with ID {}, which doesn't exist!", hiding, id);
	}
}

bool awe::map::isUnitHiding(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.isHiding();
	_logger.error("isUnitHiding operation failed: unit with ID {} doesn't exist!",
		id);
	return false;
}

bool awe::map::isUnitVisible(const awe::UnitID unit,
	const awe::ArmyID army) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("isUnitVisible operation failed: unit with ID {} doesn't "
			"exist!", unit);
		return false;
	}
	if (!_isArmyPresent(army)) {
		_logger.error("isUnitVisible operation failed: army with ID {} doesn't "
			"exist!", army);
		return false;
	}
	// A unit is visible if...
	// 1. It is on the map.
	if (!isUnitOnMap(unit)) return false;
	// 2. It isn't hiding.
	if (!isUnitHiding(unit)) return true;
	// 3. It is hiding, but it belongs to the same team as the given army.
	const auto armyTeam = getArmyTeam(army);
	if (getTeamOfUnit(unit) == armyTeam) return true;
	// 4. It is hiding, but it is located on a tile that belongs to `army`'s team.
	const auto unitPos = getUnitPosition(unit);
	const auto tileOwner = getTileOwner(unitPos);
	if (tileOwner != awe::NO_ARMY && getArmyTeam(tileOwner) == armyTeam)
		return true;
	// 5. It is hiding, but it is adjacent to a unit that belongs to the same team
	//    as `army`.
	const auto adjacentTiles = getAvailableTiles(unitPos, 1, 1);
	for (const auto& tile : adjacentTiles) {
		const auto tilesUnit = getUnitOnTile(tile);
		if (_isUnitPresent(tilesUnit) && getTeamOfUnit(tilesUnit) == armyTeam)
			return true;
	}
	// Otherwise, it is not visible.
	return false;
}

void awe::map::loadUnit(const awe::UnitID load, const awe::UnitID onto) {
	if (!_isUnitPresent(onto)) {
		_logger.error("loadUnit operation cancelled: attempted to load a unit "
			"onto unit with ID {}, the latter of which does not exist!", onto);
		return;
	}
	if (!_isUnitPresent(load)) {
		_logger.error("loadUnit operation cancelled: attempted to load unit with "
			"ID {} onto unit with ID {}, the former of which does not exist!",
			load);
		return;
	}
	if (load == onto) {
		_logger.error("loadUnit operation cancelled: attempted to load unit with "
			"ID {} onto itself.", load);
		return;
	}
	if (_units.at(load).data.loadedOnto()) {
		_logger.warning("loadUnit warning: unit with ID {} was already loaded "
			"onto unit with ID {}", load, onto);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::UNIT_LOAD));
	_updateCapturingUnit(load);
	// Make the tile that `load` was on vacant, and remove the unit ID from the
	// tile.
	if (_units.at(load).data.isOnMap()) {
		const auto location = _units.at(load).data.getPosition();
		_tiles[location.x][location.y].data.setUnit(awe::NO_UNIT);
	}
	_units.at(load).data.setPosition(awe::unit::NO_POSITION);
	// Perform loads.
	_units.at(onto).data.loadUnit(load);
	_units.at(load).data.loadOnto(onto);
}

void awe::map::unloadUnit(const awe::UnitID unload, const awe::UnitID from,
	const sf::Vector2u& onto) {
	if (!_isUnitPresent(from)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload a unit "
			"from unit with ID {}, the latter of which does not exist!", from);
		return;
	}
	if (!_isUnitPresent(unload)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload unit "
			"with ID {} from unit with ID {}, the former of which does not exist!",
			unload);
		return;
	}
	if (_isOutOfBounds(onto)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload unit "
			"with ID {} from unit with ID {}, to position {}, which is out of "
			"bounds with the map's size of {}!", unload, from, onto, getMapSize());
		return;
	}
	if (auto u = getUnitOnTile(onto)) {
		_logger.error("unloadUnit operation cancelled: attempted to unload unit "
			"with ID {} from unit with ID {}, to position {}, which has a unit "
			"with ID {} already occupying it!", unload, from, onto, u);
		return;
	}
	if (_units.at(from).data.unloadUnit(unload)) {
		// Unload successful, continue with operation.
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::UNIT_UNLOAD));
		_units.at(unload).data.loadOnto(awe::NO_UNIT);
		setUnitPosition(unload, onto);
	} else {
		_logger.error("unloadUnit operation failed: unit with ID {} was not "
			"loaded onto unit with ID {}", unload, from);
	}
}

awe::UnitID awe::map::getUnitWhichContainsUnit(const awe::UnitID unit) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("getUnitWhichContainsUnit operation failed: unit with ID {} "
			"does not exist!", unit);
		return awe::NO_UNIT;
	}
	return _units.at(unit).data.loadedOnto();
}

awe::UnitID awe::map::getUnloadedUnitWhichContainsUnit(
	const awe::UnitID unit) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("getUnloadedUnitWhichContainsUnit operation failed: unit "
			"with ID {} does not exist!", unit);
		return awe::NO_UNIT;
	}
	const auto loadedOnto = _units.at(unit).data.loadedOnto();
	if (loadedOnto == awe::NO_UNIT) return unit;
	// Either return NO_UNIT on first call or check if unit given by scripts ==
	// return.
	else return getUnloadedUnitWhichContainsUnit(loadedOnto);
}

bool awe::map::isUnitLoadedOntoUnit(const awe::UnitID unit,
	const awe::UnitID on) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("isUnitLoadedOntoUnit operation failed: unit with ID {} "
			"does not exist!", unit);
		return false;
	}
	if (!_isUnitPresent(on)) {
		_logger.error("isUnitLoadedOntoUnit operation failed: unit with ID {} "
			"does not exist!", on);
		return false;
	}
	const auto units = _units.at(on).data.loadedUnits();
	return units.find(unit) != units.end();
}

awe::ArmyID awe::map::getArmyOfUnit(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.getArmy();
	_logger.error("getArmyOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return awe::NO_ARMY;
}

awe::TeamID awe::map::getTeamOfUnit(const awe::UnitID id) const {
	if (_isUnitPresent(id))
		return _armies.at(_units.at(id).data.getArmy()).getTeam();
	_logger.error("getTeamOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

std::unordered_set<awe::UnitID> awe::map::getLoadedUnits(
	const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).data.loadedUnits();
	_logger.error("getLoadedUnits operation failed: unit with ID {} doesn't "
		"exist!", id);
	return {};
}


CScriptArray* awe::map::getLoadedUnitsAsArray(const awe::UnitID id) const {
	if (!_scripts) throw NO_SCRIPTS;
	return _scripts->createArrayFromContainer("UnitID", getLoadedUnits(id));
}

unsigned int awe::map::getUnitDefence(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitDefence operation failed: unit with ID {} doesn't "
			"exist!", id);
		return 0;
	}
	const auto type = getUnitType(id);
	if (!type) {
		_logger.error("getUnitDefence operation failed: couldn't deduce unit {}'s "
			"type.", id);
		return 0;
	}
	if (type->ignoresDefence() || !isUnitOnMap(id)) {
		return 0;
	} else {
		return getTileType(getUnitPosition(id))->getType()->getDefence();
	}
}

void awe::map::setUnitSpritesheet(const awe::UnitID id, const std::string& name) {
	if (!_isUnitPresent(id)) {
		_logger.error("setUnitSpritesheet operation failed: unit with ID {} "
			"doesn't exist!", id);
		return;
	}
	if (!_sheets->exists(name)) {
		_logger.error("setUnitSpritesheet operation failed: spritesheet with name "
			"\"{}\" doesn't exist!", name);
		return;
	}
	_units.at(id).sprite->setSpritesheet((*_sheets)[name]);
}
