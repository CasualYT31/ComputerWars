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

bool awe::map::createArmy(const std::string& country) {
	const auto& countries = *_banks->get<awe::country>();
	if (!countries.contains(country)) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with a country, \"{}\", that doesn't exist!", country);
		return false;
	}
	const auto turnOrder = countries[country]->turnOrder();
	if (_isArmyPresent(turnOrder)) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with a country, \"{}\", that already existed on the map!",
			country);
		return false;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::CREATE_ARMY));
	// Create the army.
	_armies.insert(
		std::pair<awe::ArmyID, awe::army>(turnOrder, awe::army(_banks, country,
			engine::logger::data{ _logger.getData().sink, "army_view" }))
	);
	// This will miss out the maximum value for a team ID, but I don't care.
	if (_teamIDCounter == std::numeric_limits<awe::TeamID>::max())
		_teamIDCounter = std::numeric_limits<awe::TeamID>::min();
	_armies.at(turnOrder).setTeam(_teamIDCounter++);
	return true;
}

void awe::map::deleteArmy(const awe::ArmyID army,
	const awe::ArmyID transferOwnership) {
	if (!_isArmyPresent(army)) {
		_logger.error("deleteArmy operation cancelled: attempted to delete an "
			"army, {}, that didn't exist on the map!", army);
		return;
	}
	if (!_isArmyPresent(transferOwnership) &&
		transferOwnership != awe::NO_ARMY) {
		_logger.error("deleteArmy operation cancelled: attempted to transfer "
			"ownership of army {}'s tiles to an army of ID {}, which doesn't "
			"exist on the map!", army, transferOwnership);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::DELETE_ARMY));
	// Firstly, delete all units belonging to the army.
	auto units = _armies.at(army).getUnits();
	for (auto unit : units) {
		// Ignore loaded units, as they will be handled automatically by
		// deleteUnit().
		if (_isUnitPresent(unit) &&
			_units.at(unit).data.loadedOnto() == awe::NO_UNIT) deleteUnit(unit);
	}
	// Then, disown all tiles.
	auto tiles = _armies.at(army).getTiles();
	for (auto& tile : tiles) setTileOwner(tile, transferOwnership);
	// If this army was assigned to the selected army override, remove it.
	if (_currentArmyOverride && *_currentArmyOverride == army)
		clearSelectedArmyOverride();
	// Finally, delete the army from the army list.
	_armies.erase(army);
}

std::size_t awe::map::getArmyCount() const noexcept {
	return _armies.size();
}

std::set<awe::ArmyID> awe::map::getArmyIDs() const {
	std::set<awe::ArmyID> ret;
	for (auto& a : _armies) ret.insert(a.first);
	return ret;
}

CScriptArray* awe::map::getArmyIDsAsArray() const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto armyIDs = getArmyIDs();
	return _scripts->createArrayFromContainer("ArmyID", armyIDs);
}

void awe::map::setArmyTeam(const awe::ArmyID army, const awe::TeamID team) {
	if (_isArmyPresent(army)) {
		if (team == getArmyTeam(army)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::ARMY_TEAM));
		_armies.at(army).setTeam(team);
		// First, stop all of the army's units from capturing.
		const auto units = getUnitsOfArmy(army);
		for (auto& unit : units) _updateCapturingUnit(unit);
		// Then, stop all of the units capturing that are on this army's tiles.
		const auto tiles = getTilesOfArmy(army);
		for (auto& tile : tiles) _updateCapturingUnit(getUnitOnTile(tile));
	} else {
		_logger.error("setArmyTeam operation cancelled: attempted to set an army "
			"{}'s team to {}, but that army didn't exist!", army, team);
	}
}

awe::TeamID awe::map::getArmyTeam(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getTeam();
	_logger.error("getArmyTeam operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return 0;
}

void awe::map::setArmyFunds(const awe::ArmyID army, const awe::Funds funds) {
	if (_isArmyPresent(army)) {
		if (funds == getArmyFunds(army)) return;
		awe::disable_mementos token(this,
			_getMementoName(awe::map_strings::operation::ARMY_FUNDS));
		_armies.at(army).setFunds(funds);
	} else {
		_logger.error("setArmyFunds operation cancelled: attempted to set {} "
			"funds to an army, {}, that didn't exist!", funds, army);
	}
}

void awe::map::offsetArmyFunds(const awe::ArmyID army, const awe::Funds funds) {
	if (_isArmyPresent(army)) {
		setArmyFunds(army, getArmyFunds(army) + funds);
	} else {
		_logger.error("offsetArmyFunds operation cancelled: attempted to award {} "
			"funds to an army, {}, that didn't exist!", funds, army);
	}
}

awe::Funds awe::map::getArmyFunds(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getFunds();
	_logger.error("getArmyFunds operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return -1;
}

const engine::CScriptWrapper<awe::country_view> awe::map::getArmyCountry(const awe::ArmyID army) const {
	if (!_isArmyPresent(army)) {
		_logger.error("getArmyCountry operation failed: army with ID {} didn't "
			"exist at the time of calling!", army);
		return nullptr;
	}
	return _armies.at(army).getCountry();
}

const awe::country_view* awe::map::getRawArmyCountry(const awe::ArmyID army) const {
	const auto country = getArmyCountry(army);
	if (!country.operator->()) return nullptr;
	country->AddRef();
	return country.operator->();
}

std::string awe::map::getArmyCountryScriptName(const awe::ArmyID army) const {
	if (!_isArmyPresent(army)) {
		_logger.error("getArmyCountryScriptName operation failed: army with ID {} "
			"didn't exist at the time of calling!", army);
		return "";
	}
	return _armies.at(army).getCountryScriptName();
}

void awe::map::setArmyCOs(const awe::ArmyID army, const std::string& current,
	const std::string& tag) {
	if (!_isArmyPresent(army)) {
		_logger.error("setCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
		return;
	}
	if (current.empty() && tag.empty()) {
		_logger.error("setCOs operation failed: army with ID {} was given no "
			"COs!", army);
		return;
	}
	if (current == getArmyCurrentCOScriptName(army) &&
		tag == getArmyTagCOScriptName(army)) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::ARMY_COS));
	if (current.empty() && !tag.empty()) {
		_logger.warning("setCOs operation: army with ID {} was given a "
			"tag CO but not current CO! The army will instead be assigned "
			"a current CO and it will not be assigned a tag CO.", army);
	}
	_armies.at(army).setCOs(current, tag);
}

void awe::map::setArmyCurrentCO(const awe::ArmyID army,
	const std::string& current) {
	setArmyCOs(army, current, getArmyTagCOScriptName(army));
}

void awe::map::setArmyTagCO(const awe::ArmyID army, const std::string& tag) {
	setArmyCOs(army, getArmyCurrentCOScriptName(army), tag);
}

void awe::map::tagArmyCOs(const awe::ArmyID army) {
	if (!_isArmyPresent(army)) {
		_logger.error("tagCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
	} else {
		if (tagCOIsPresent(army)) {
			awe::disable_mementos token(this,
				_getMementoName(awe::map_strings::operation::TAG_COS));
			_armies.at(army).tagCOs();
		} else {
			_logger.error("tagCOs operation failed: army with ID {} didn't have a "
				"secondary CO at the time of calling!", army);
		}
	}
}

const engine::CScriptWrapper<awe::commander_view> awe::map::getArmyCurrentCO(const awe::ArmyID army) {
	if (_isArmyPresent(army)) return _armies.at(army).getCurrentCO();
	_logger.error("getCurrentCO operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return nullptr;
}

const awe::commander_view* awe::map::getRawArmyCurrentCO(const awe::ArmyID army) {
	const auto co = getArmyCurrentCO(army);
	if (!co.operator->()) return nullptr;
	co->AddRef();
	return co.operator->();
}

std::string awe::map::getArmyCurrentCOScriptName(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getCurrentCOScriptName();
	_logger.error("getCurrentCOScriptName operation failed: army with ID {} "
		"didn't exist at the time of calling!", army);
	return "";
}

const engine::CScriptWrapper<awe::commander_view> awe::map::getArmyTagCO(const awe::ArmyID army) {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCO();
	_logger.error("getTagCO operation failed: army with ID {} didn't exist at the "
		"time of calling!", army);
	return nullptr;
}

const awe::commander_view* awe::map::getRawArmyTagCO(const awe::ArmyID army) {
	const auto co = getArmyTagCO(army);
	if (!co.operator->()) return nullptr;
	co->AddRef();
	return co.operator->();
}

std::string awe::map::getArmyTagCOScriptName(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCOScriptName();
	_logger.error("getTagCOScriptName operation failed: army with ID {} didn't "
		"exist at the time of calling!", army);
	return "";
}

bool awe::map::tagCOIsPresent(const awe::ArmyID army) {
	if (_isArmyPresent(army)) return _armies.at(army).hasTagCO();
	_logger.error("tagCOIsPresent operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return false;
}

std::unordered_set<sf::Vector2u> awe::map::getTilesOfArmy(const awe::ArmyID army,
	const std::unordered_set<std::string>& filter) const {
	if (_isArmyPresent(army)) {
		auto result = _armies.at(army).getTiles();
		if (!filter.empty()) {
			// remove_if won't work on an unordered_set, so have to convert it.
			std::vector<sf::Vector2u> converted(result.begin(), result.end());
			auto begin = converted.begin(), end = converted.end();
			end = std::remove_if(begin, end, [&](const sf::Vector2u& tile) {
				return filter.find(getTerrainOfTileScriptName(tile))
					== filter.end();
				});
			return std::unordered_set<sf::Vector2u>(begin, end);
		}
		return result;
	}
	_logger.error("getTilesOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<sf::Vector2u>();
}

CScriptArray* awe::map::getTilesOfArmyAsArray(const awe::ArmyID army,
	const CScriptArray* const filter) const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto scriptNames =
		engine::ConvertCScriptArray<std::unordered_set<std::string>, std::string>(
			filter);
	return _scripts->createArrayFromContainer("Vector2", scriptNames);
}

std::unordered_set<awe::UnitID> awe::map::getUnitsOfArmy(
	const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getUnits();
	_logger.error("getUnitsOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<awe::UnitID>();
}

CScriptArray* awe::map::getUnitsOfArmyAsArray(const awe::ArmyID army) const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto unitsOfArmy = getUnitsOfArmy(army);
	return _scripts->createArrayFromContainer("UnitID", unitsOfArmy);
}

std::map<unsigned int, std::unordered_set<awe::UnitID>>
awe::map::getUnitsOfArmyByPriority(const awe::ArmyID army) const {
	if (!_isArmyPresent(army)) {
		_logger.error("getUnitsOfArmyByPriority operation failed: army with ID {} "
			"didn't exist at the time of calling!", army);
		return {};
	}
	std::map<unsigned int, std::unordered_set<awe::UnitID>> ret;
	auto units = getUnitsOfArmy(army);
	for (auto& unit : units)
		ret[getUnitType(unit)->turnStartPriority()].insert(unit);
	return ret;
}

CScriptArray* awe::map::getUnitsOfArmyByPriorityAsArray(
	const awe::ArmyID army) const {
	if (!_scripts) throw NO_SCRIPTS;
	auto set = getUnitsOfArmyByPriority(army);
	CScriptArray* ret = _scripts->createArray("array<UnitID>@");
	// Loop through backwards: see documentation on unit_type::unit_type().
	for (auto itr = set.rbegin(), enditr = set.rend(); itr != enditr; ++itr) {
		CScriptArray* list =
			_scripts->createArrayFromContainer("UnitID", itr->second);
		ret->InsertLast(&list);
		list->Release();
	}
	return ret;
}

std::size_t awe::map::countTilesBelongingToArmy(const awe::ArmyID army,
	const std::string& terrainType) const {
	if (!_isArmyPresent(army)) {
		_logger.error("countTilesBelongingToArmy operation failed: army with ID "
			"{} didn't exist at the time of calling!", army);
		return 0;
	}
	std::size_t counter = 0;
	const auto tiles = getTilesOfArmy(army);
	for (const auto& tile : tiles) {
		const auto terrain = getTerrainOfTileScriptName(tile);
		if (terrain == terrainType) ++counter;
	}
	return counter;
}
