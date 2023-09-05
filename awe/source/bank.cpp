/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

#include "bank.hpp"
#include <algorithm>
#include <unordered_set>

/**
 * Creates a map keyed by turn order ID from another map keyed by country script
 * name.
 * @warning \c dest will be cleared at the beginning of the function.
 * @tparam  T         The type of values stored in the maps.
 * @param   src       The map keyed by country script names.
 * @param   dest      The map keyed by turn order IDs.
 * @param   countries The country bank allowing the function to match script names
 *                    with turn order IDs.
 * @safety  Basic guarantee.
 */
template<typename T>
void updateTurnOrderMap(const std::unordered_map<std::string, T>& src,
	std::unordered_map<awe::ArmyID, T>& dest,
	const awe::bank<awe::country>& countries) {
	dest.clear();
	for (const auto& pair : src) {
		if (countries.contains(pair.first)) {
			dest[countries[pair.first]->getTurnOrder()] = pair.second;
		}
	}
}

//*********
//*BANK ID*
//*********
const std::string awe::bank_id::EMPTY_STRING = "";
const sf::Vector2i awe::bank_id::EMPTY_VECTOR_I = { 0, 0 };

//*******************
//*COMMON PROPERTIES*
//*******************
awe::common_properties::common_properties(const std::string& scriptName,
	engine::json& j) : bank_id(scriptName) {
	j.apply(_name, { "longname" }, true);
	j.apply(_shortName, { "shortname" }, true);
	j.apply(_iconKey, { "icon" }, true);
	j.apply(_description, { "description" }, true);
}

//*********
//*COUNTRY*
//*********
awe::ArmyID awe::country::_turnOrderCounter = 0;
awe::country::country(const std::string& scriptName, engine::json& j) :
	common_properties(scriptName, j) {
	j.applyColour(_colour, { "colour" }, true);
	_turnOrder = _turnOrderCounter++;
	if (j.keysExist({ "turnorder" })) {
		j.apply(_turnOrder, { "turnorder" }, true);
	}
}

//*********
//*WEATHER*
//*********

//*************
//*ENVIRONMENT*
//*************

//***************
//*MOVEMENT TYPE*
//***************

//*********
//*TERRAIN*
//*********
awe::terrain::terrain(const std::string& scriptName, engine::json& j) :
	common_properties(scriptName, j) {
	j.apply(_maxHP, { "hp" }, true);
	if (_maxHP > INT_MAX) _maxHP = INT_MAX;
	j.apply(_defence, { "defence" }, true);
	if (j.keysExist({"movecosts"})) {
		j.applyMap(_movecosts, { "movecosts" });
		j.resetState();
	}
	if (j.keysExist({ "pictures" })) {
		j.applyMap(_pictures, { "pictures" });
		j.resetState();
	}
}
void awe::terrain::updatePictureMap(
	const awe::bank<awe::country>& countries) const {
	updateTurnOrderMap(_pictures, _picturesTurnOrder, countries);
}

//******
//*TILE*
//******
awe::tile_type::tile_type(const std::string& scriptName, engine::json& j) :
	bank_id(scriptName) {
	j.apply(_terrainTypeScriptName, { "type" }, true);
	j.apply(_neutralTile, { "neutral" }, true);
	if (j.keysExist({ "tiles" })) {
		j.applyMap(_ownedTiles, { "tiles" });
		j.resetState();
	}
	if (j.keysExist({ "alwayspaintable" }))
		j.apply(_alwaysPaintable, { "alwayspaintable" }, true);
}
awe::tile_type::~tile_type() noexcept {
	if (_structureScriptNames) _structureScriptNames->Release();
}
void awe::tile_type::updateOwnedTilesMap(
	const awe::bank<awe::country>& countries) const {
	updateTurnOrderMap(_ownedTiles, _ownedTilesTurnOrder, countries);
}
void awe::tile_type::updateStructures(
	const awe::bank<awe::structure>& structureBank,
	const std::shared_ptr<engine::scripts>& scripts) const {
	_structures.clear();
	if (!_structureScriptNames)
		_structureScriptNames = scripts->createArray("string");
	else _structureScriptNames->Resize(0);
	for (const auto& structure : structureBank) {
		if (structure.second->containsTileType(getScriptName())) {
			_structures.push_back(structure.second);
			std::string scriptName = structure.first;
			_structureScriptNames->InsertLast(&scriptName);
		}
	}
}

//********
//*WEAPON*
//********
awe::weapon::weapon(const std::string& scriptName, engine::json& j) :
	common_properties(scriptName, j), _jsonCache(j) {
	if (j.keysExist({ "ammo" })) j.apply(_maxAmmo, { "ammo" }, true);
	if (j.keysExist({ "canattackaftermoving" })) {
		j.apply(_canAttackAfterMoving, { "canattackaftermoving" }, true);
	}
	if (j.keysExist({ "cancounterattackdirectly" })) {
		j.apply(_canCounterattackDirectly, { "cancounterattackdirectly" }, true);
	}
	if (j.keysExist({ "cancounterattackindirectly" })) {
		j.apply(_canCounterattackIndirectly, { "cancounterattackindirectly" },
			true);
	}
	if (j.keysExist({ "lowrange" })) j.apply(_range.x, { "lowrange" }, true);
	if (j.keysExist({ "highrange" })) j.apply(_range.y, { "highrange" }, true);
	if (_range.x > _range.y) std::swap(_range.x, _range.y);
	if (j.keysExist({ "units" })) {
		j.applyMap(_canAttackTheseUnits, { "units" });
		j.resetState();
	}
	if (j.keysExist({ "terrains" })) {
		j.applyMap(_canAttackTheseTerrains, { "terrains" });
		j.resetState();
	}
	if (j.keysExist({ "hiddenunits" })) {
		static nlohmann::ordered_json dataTypeTest = int();
		nlohmann::ordered_json jj = j.nlohmannJSON()["hiddenunits"];
		for (const auto& hiddenUnit : jj.items()) {
			if (hiddenUnit.value().is_boolean() && hiddenUnit.value()) {
				if (_canAttackTheseUnits.find(hiddenUnit.key()) !=
					_canAttackTheseUnits.end()) {
					_canAttackTheseHiddenUnits[hiddenUnit.key()] =
						_canAttackTheseUnits.at(hiddenUnit.key());
				}
			} else if (j.equalType(dataTypeTest, hiddenUnit.value())) {
				_canAttackTheseHiddenUnits[hiddenUnit.key()] = hiddenUnit.value();
			} else {
				// Gotta log it somehow...
			}
		}
	}
}

//******
//*UNIT*
//******
const unsigned int awe::unit_type::HP_GRANULARITY = 10;
awe::unit_type::unit_type(const std::string& scriptName, engine::json& j) :
	common_properties(scriptName, j) {
	j.apply(_movementTypeScriptName, { "movetype" }, true);
	j.apply(_cost, { "price" }, true);
	j.apply(_maxFuel, { "fuel" }, true);
	j.apply(_maxHP, { "hp" }, true);
	if (_maxHP > INT_MAX / HP_GRANULARITY) _maxHP = INT_MAX / HP_GRANULARITY;
	_maxHP *= HP_GRANULARITY;
	j.apply(_movementPoints, { "mp" }, true);
	j.apply(_vision, { "vision" }, true);
	if (j.keysExist({ "pictures" })) {
		j.applyMap(_pictures, { "pictures" });
		j.resetState();
	}
	j.applyMap(_units, { "sprites" });
	j.resetState();
	if (j.keysExist({ "canload" })) {
		j.applyVector(_canLoadThese, { "canload" });
		j.resetState();
	}
	if (j.keysExist({ "loadlimit" })) {
		j.apply(_loadLimit, { "loadlimit" }, true);
	}
	j.apply(_turnStartPriority, { "turnstartpriority" }, true);
	if (j.keysExist({ "cancapture" })) {
		j.applyVector(_canCaptureThese, { "cancapture" });
		j.resetState();
	}
	j.apply(_canHide, { "canhide" }, true);
	if (j.keysExist({ "canunloadfrom" })) {
		j.applyVector(_canUnloadFromThese, { "canunloadfrom" });
		j.resetState();
	}
	if (j.keysExist({ "weapons" })) {
		const auto jj = j.nlohmannJSON()["weapons"];
		for (const auto& weapon : jj.items()) {
			_baseWeapons.push_back({weapon.key(), jj});
		}
	}
	if (j.keysExist({ "ignoresdefence" })) {
		j.apply(_ignoreDefence, { "ignoresdefence" }, true);
	}
}
void awe::unit_type::updateUnitTypes(const awe::bank<awe::unit_type>& unitBank)
	const {
	_canLoadTheseUnitTypes.clear();
	for (const auto& unit : unitBank) {
		for (auto& u : _canLoadThese) {
			if (unit.first == u) {
				_canLoadTheseUnitTypes.push_back(unit.second);
				break;
			}
		}
	}
}
void awe::unit_type::updateTerrainTypes(
	const awe::bank<awe::terrain>& terrainBank) const {
	_canCaptureTheseTerrainTypes.clear();
	_canUnloadFromTheseTerrainTypes.clear();
	for (const auto& terrain : terrainBank) {
		for (auto& u : _canCaptureThese) {
			if (terrain.first == u) {
				_canCaptureTheseTerrainTypes.push_back(terrain.second);
				break;
			}
		}
		for (auto& u : _canUnloadFromThese) {
			if (terrain.first == u) {
				_canUnloadFromTheseTerrainTypes.push_back(terrain.second);
				break;
			}
		}
	}
}
std::shared_ptr<const awe::weapon>
	awe::unit_type::getFirstWeaponWithFiniteAmmo() const {
	for (std::size_t i = 0, weaponCount = getWeaponCount(); i < weaponCount; ++i) {
		const auto weapon = getWeaponByIndex(i);
		if (!weapon->hasInfiniteAmmo()) return weapon;
	}
	return nullptr;
}
static void updateJJNew(nlohmann::ordered_json& jjNew,
	const nlohmann::ordered_json& jjBase, const nlohmann::ordered_json& jjOver,
	const std::string& objectKey) {
	if (jjBase.contains(objectKey)) jjNew[objectKey] = jjBase[objectKey];
	if (jjOver.contains(objectKey) && jjOver[objectKey].is_object()) {
		for (const auto& dmg : jjOver[objectKey].items()) {
			if (jjBase[objectKey].contains(dmg.key())) {
				if (dmg.value().is_boolean()) {
					if (dmg.value()) {
						jjNew[objectKey][dmg.key()] = jjBase[objectKey][dmg.key()];
					} else {
						jjNew[objectKey].erase(dmg.key());
					}
				} else if (dmg.value().is_number()) {
					jjNew[objectKey][dmg.key()] = dmg.value();
				}
			} else {
				jjNew[objectKey][dmg.key()] = dmg.value();
			}
		}
	}
}
void awe::unit_type::updateWeapons(const awe::bank<awe::weapon>& weaponBank,
	const std::shared_ptr<engine::sink>& sink) const {
	_weapons.clear();
	for (const auto weapon : _baseWeapons) {
		if (weaponBank.contains(weapon.first)) {
			nlohmann::ordered_json jjBase =
				weaponBank[weapon.first]->getJSON().nlohmannJSON();
			nlohmann::ordered_json jjOver = weapon.second;
			jjOver = jjOver.items().begin().value();
			nlohmann::ordered_json jjNew = jjBase;
			// For non-object and non-array values, update() is sufficient.
			// However, this won't combine inner objects, merely override jjNew
			// completely. So we have to perform the combine manually.
			jjNew.update(jjOver);
			// Now, go through the units and terrains objects and find all the
			// cases where the value is a boolean. If the value is TRUE, then
			// retain the base damage from the base object (which is default
			// behaviour usually, but in this case we have to do it manually for
			// the user as it just got replaced with the boolean value). If the
			// value is FALSE, then remove the key-value pair entirely.
			updateJJNew(jjNew, jjBase, jjOver, "units");
			updateJJNew(jjNew, jjBase, jjOver, "terrains");
			// For hiddenunits, the override will completely replace the base
			// object, if an override is given. update() does this for us.
			const auto newWeapon = std::make_shared<const awe::weapon>(
				weapon.first, engine::json(jjNew, {sink, "json"}));
			_weapons.emplace(weapon.first, newWeapon);
		}
	}
}
void awe::unit_type::updateSpriteMaps(
	const awe::bank<awe::country>& countries) const {
	updateTurnOrderMap(_pictures, _picturesTurnOrder, countries);
	updateTurnOrderMap(_units, _unitsTurnOrder, countries);
}

//***********
//*COMMANDER*
//***********

//***********
//*STRUCTURE*
//***********
awe::structure::structure(const std::string& scriptName, engine::json& j) :
	common_properties(scriptName, j) {
	// Really need to find some way to log errors.
	if (j.keysExist({ "ownedicons" })) {
		j.applyMap(_ownedIcons, { "ownedicons" });
		j.resetState();
	}
	j.apply(_rootTile, { "root", "tile" }, true);
	if (j.keysExist({ "root", "destroyed" }))
		j.apply(_rootDestroyedTile, { "root", "destroyed" }, true);
	if (j.keysExist({ "root", "deleted" }))
		j.apply(_rootDeletedTile, { "root", "deleted" }, true);
	if (j.keysExist({ "paintable" }))
		j.apply(_paintable, { "paintable" }, true);
	if (j.keysExist({ "destroyedlongname" }))
		j.apply(_destroyedLongName, { "destroyedlongname" }, true);
	if (j.keysExist({ "destroyediconname" }))
		j.apply(_destroyedIconName, { "destroyediconname" }, true);
	if (j.keysExist({ "dependent" })) {
		const auto json = j.nlohmannJSON()["dependent"];
		if (!json.is_array()) return;
		// Can't have a dependent tile that offsets to the root tile.
		std::list<sf::Vector2i> offsets = { { 0, 0 } };
		for (const auto& obj : json) {
			if (!obj.is_object() || !obj.contains("offset") ||
				!obj.contains("tile") || !obj["tile"].is_string()) continue;
			const auto& offsetObj = obj["offset"];
			if (!offsetObj.is_array() || offsetObj.size() != 2 ||
				!offsetObj[0].is_number_integer() ||
				!offsetObj[1].is_number_integer()) continue;
			sf::Vector2i offset = { offsetObj[0], offsetObj[1] };
			if (std::find(offsets.cbegin(), offsets.cend(), offset) !=
				offsets.cend()) continue;
			offsets.push_back(offset);

			_dependents.emplace_back(offset, obj["tile"]);
			if (obj.contains("destroyed") && obj["destroyed"].is_string())
				_dependents.back().destroyedTile = obj["destroyed"];
			if (obj.contains("deleted") && obj["deleted"].is_string())
				_dependents.back().deletedTile = obj["deleted"];
		}
	}
}
bool awe::structure::containsTileType(const std::string& tileType) const {
	if (_rootTile == tileType || _rootDestroyedTile == tileType ||
		_rootDeletedTile == tileType) return true;
	for (const auto& dependent : _dependents) {
		if (dependent.tile == tileType || dependent.destroyedTile == tileType ||
			dependent.deletedTile == tileType) return true;
	}
	return false;
}
void awe::structure::updateTileTypes(
	const awe::bank<awe::tile_type>& tileBank) const {
	_rootTileType = tileBank[_rootTile];
	if (hasRootDestroyedTileType())
		_rootDestroyedTileType = tileBank[_rootDestroyedTile];
	if (hasRootDeletedTileType())
		_rootDeletedTileType = tileBank[_rootDeletedTile];
	for (auto& dependent : _dependents) {
		dependent.tileType = tileBank[dependent.tile];
		if (dependent.hasDestroyedTileType())
			dependent.destroyedTileType = tileBank[dependent.destroyedTile];
		if (dependent.hasDeletedTileType())
			dependent.deletedTileType = tileBank[dependent.deletedTile];
	}
}
void awe::structure::updateOwnedIconsMap(
	const awe::bank<awe::country>& countries) const {
	updateTurnOrderMap(_ownedIcons, _ownedIconsTurnOrder, countries);
}
awe::structure::dependent_tile::dependent_tile(const sf::Vector2i& o,
	const std::string& t) : offset(o), tile(t) {}

//******************
//*HELPER FUNCTIONS*
//******************

void awe::updateTerrainBank(awe::bank<awe::terrain>& terrainBank,
	const awe::bank<awe::country>& countryBank) {
	for (const auto& terrain : terrainBank) {
		terrain.second->updatePictureMap(countryBank);
	}
}

void awe::updateTileTypeBank(awe::bank<awe::tile_type>& tileBank,
	const awe::bank<awe::terrain>& terrainBank,
	const awe::bank<awe::country>& countryBank,
	const awe::bank<awe::structure>& structureBank,
	const std::shared_ptr<engine::scripts>& scripts) {
	for (const auto& tile : tileBank) {
		tile.second->updateTerrain(terrainBank);
		tile.second->updateOwnedTilesMap(countryBank);
		tile.second->updateStructures(structureBank, scripts);
	}
}

void awe::updateUnitTypeBank(awe::bank<awe::unit_type>& unitBank,
	const awe::bank<awe::movement_type>& movementBank,
	const awe::bank<awe::terrain>& terrainBank,
	const awe::bank<awe::weapon>& weaponBank,
	const awe::bank<awe::country>& countryBank,
	const std::shared_ptr<engine::sink>& sink) {
	for (const auto& unit : unitBank) {
		unit.second->updateMovementType(movementBank);
		unit.second->updateUnitTypes(unitBank);
		unit.second->updateTerrainTypes(terrainBank);
		unit.second->updateWeapons(weaponBank, sink);
		unit.second->updateSpriteMaps(countryBank);
	}
}

void awe::updateStructureBank(awe::bank<awe::structure>& structureBank,
	const awe::bank<awe::tile_type>& tileBank,
	const awe::bank<awe::country>& countryBank) {
	for (const auto& structure : structureBank) {
		structure.second->updateTileTypes(tileBank);
		structure.second->updateOwnedIconsMap(countryBank);
	}
}

bool awe::checkCountryTurnOrderIDs(const awe::bank<awe::country>& countries) {
	std::unordered_set<awe::ArmyID> turnOrderIDs;
	for (const auto& country : countries) {
		const auto turnOrder = country.second->getTurnOrder();
		if (turnOrder == awe::NO_ARMY) return false;
		turnOrderIDs.insert(turnOrder);
	}
	// If the set isn't the same length as the number of countries, then we know
	// that at least two of the countries have the same turn order ID.
	return countries.size() == turnOrderIDs.size();
}
