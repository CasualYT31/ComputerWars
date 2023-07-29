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

#pragma once

template<typename T>
awe::bank<T>::bank(const std::shared_ptr<engine::scripts>& scripts,
	const std::string& name, const engine::logger::data& data) :
	engine::json_script({ data.sink, "json_script" }), _logger(data),
	_scripts(scripts), _propertyName(name) {
	if (scripts) _scripts->addRegistrant(this);
}

template<typename T>
void awe::bank<T>::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	// 1. Register the game typedefs to ensure that they are defined.
	awe::RegisterGameTypedefs(engine, document);
	// 2. Register the value type that this bank stores (i.e. T).
	auto r = engine->RegisterObjectType(_propertyName.c_str(), 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	T::Register<T>(_propertyName, engine, document);
	// 3. Register a single reference type, called _propertyName + "Bank".
	const std::string bankTypeName(_propertyName + "Bank"),
		indexOpStrDecl("const " + _propertyName + "@ opIndex(const string&in)"),
		globalPropDecl(bankTypeName + " " +
			tgui::String(_propertyName).toLower().toStdString());
	r = engine->RegisterObjectType(bankTypeName.c_str(), 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	document->DocumentObjectType(r, "Holds a collection of related game "
		"properties.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(), indexOpStrDecl.c_str(),
		asMETHOD(awe::bank<T>, _opIndexStr), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Access a game property by its script "
		"name.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(), "uint length() const",
		asMETHOD(awe::bank<T>, size), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the number of game properties stored "
		"in this bank.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(),
		"bool contains(const string&in) const",
		asMETHOD(awe::bank<T>, contains), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns true if an entry with the given "
		"script name exists within the bank, false otherwise.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(),
		"array<string>@ get_scriptNames() const property",
		asMETHOD(awe::bank<T>, _getScriptNamesArray), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the script name of each game "
		"property stored in this bank, in the order they were given to the bank.");
	// 4. Register the global point of access to the _propertyName + "Bank" object.
	engine->RegisterGlobalProperty(globalPropDecl.c_str(), this);
	document->DocumentExpectedFunction(globalPropDecl, "The single point of "
		"access to the relevant game properties. Declared by the game engine.");
}

template<typename T>
bool awe::bank<T>::_load(engine::json& j) {
	bank_type bank;
	std::vector<std::string> scriptNames;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		// Loop through each object, allowing the template type T to construct its
		// values based on each object.
		engine::json input(i.value(), {_logger.getData().sink, "json"});
		bank[i.key()] = std::make_shared<const T>(i.key(), input);
		scriptNames.push_back(i.key());
	}
	_bank = std::move(bank);
	_scriptNames = std::move(scriptNames);
	return true;
}

template<typename T>
CScriptArray* awe::bank<T>::_getScriptNamesArray() const {
	const auto names = getScriptNames();
	CScriptArray* ret = _scripts->createArray("string");
	for (auto name : names) ret->InsertLast(&name);
	return ret;
}

template<typename T>
void awe::bank_id::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_scriptName() const property",
		asMETHOD(T, getScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the script name of this game "
		"property.");
}

template<typename T>
void awe::common_properties::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document,
	const std::string& extraIconDoc) {
	awe::bank_id::Register<T>(type, engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_name() const property",
		asMETHOD(T, getName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the translation key of the long name "
		"of this game property.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_shortName() const property",
		asMETHOD(T, getShortName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the translation key of the short name "
		"of this game property.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_iconName() const property",
		asMETHOD(T, getIconName), asCALL_THISCALL);
	std::string doc = "Gets the sprite key of the icon of this game property." +
		(extraIconDoc.empty() ? "" : " " + extraIconDoc);
	document->DocumentObjectMethod(r, doc.c_str());
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_description() const property",
		asMETHOD(T, getDescription), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the translation key of the "
		"description of this game property.");
}

template<typename T>
void awe::country::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document);
	engine::RegisterColourType(engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const Colour& get_colour() const property",
		asMETHOD(T, getColour), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the colour of the country.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"ArmyID get_turnOrder() const property",
		asMETHOD(T, getTurnOrder), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the turn order of this country.");
}

template<typename T>
void awe::weather::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::environment::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::movement_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::terrain::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document,
		"For terrain types, this holds the sprite ID of the picture shown "
		"for a tile that has no owner.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_maxHP() const property",
		asMETHOD(T, getMaxHP), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the maximum HP of this terrain type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_defence() const property",
		asMETHOD(T, getDefence), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the number of defence stars that this "
		"terrain type has.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int get_moveCost(const string&in) const property",
		asMETHOD(T, getMoveCost), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the movement cost of this terrain "
		"type, given a movement type script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& picture(const string&in) const",
		asMETHODPR(T, getPicture, (const std::string&) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this terrain type's "
		"picture, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& picture(const ArmyID) const",
		asMETHODPR(T, getPicture, (const awe::ArmyID) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this terrain type's "
		"picture, given a country turn order ID.");
}

template<typename T>
void awe::tile_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::bank_id::Register<T>(type, engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_typeScriptName() const property",
		asMETHOD(T, getTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the script name of this tile's "
		"terrain type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Terrain@ get_type() const property",
		asMETHOD(T, _getTypeObj), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on this tile's terrain "
		"type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasOwnedTiles() const property",
		asMETHOD(T, hasOwnedTiles), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns <tt>TRUE</tt> if this tile type "
		"has at least one owned tile sprite.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedTileSprite(const string&in) const",
		asMETHODPR(T, getOwnedTile, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's owned "
		"tile that is displayed on the map, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedTileSprite(const ArmyID) const",
		asMETHODPR(T, getOwnedTile, (const awe::ArmyID) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's owned "
		"tile that is displayed on the map, given a country turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_neutralTileSprite() const property",
		asMETHOD(T, getNeutralTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's tile "
		"graphic that is displayed on the map.");
}

template<typename T>
void awe::weapon::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document,
		"For weapon types, this property holds the sprite ID of the small ammo "
		"icon that is used with this weapon.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"int get_maxAmmo() const property",
		asMETHOD(T, getMaxAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this weapon's max ammo.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasInfiniteAmmo() const property",
		asMETHOD(T, hasInfiniteAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns true if this weapon has infinite "
		"ammo, false if maxAmmo returns >= 0.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Vector2& get_range() const property",
		asMETHOD(T, getRange), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this weapon's range. x stores the "
		"lower range, and y stores the higher range.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canAttackAfterMoving() const property",
		asMETHOD(T, canAttackAfterMoving), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "TRUE if this weapon can attack after the "
		"unit who's using it moves at least one tile.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canCounterattackDirectly() const property",
		asMETHOD(T, canCounterattackDirectly), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "TRUE if this weapon can counterattack  "
		"using a direct attack.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canCounterattackIndirectly() const property",
		asMETHOD(T, canCounterattackIndirectly), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "TRUE if this weapon can counterattack  "
		"using an indirect attack.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool canAttackUnit(const string&in, const bool = false) const",
		asMETHOD(T, canAttackUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this weapon can attack the "
		"given type of unit. If the bool parameter is TRUE, this will find out if "
		"this weapon can attack the given type of unit if it is hidden.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int getBaseDamageUnit(const string&in, const bool = false) const",
		asMETHOD(T, getBaseDamageUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the base damage that this weapon "
		"deals to the given type of unit. If the bool parameter is TRUE, this "
		"will find out the base damage this weapon inflicts upon the given type "
		"of unit if it is hidden. If this weapon cannot attack the given unit "
		"type, whether hidden and/or visible, 0 will be returned.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool canAttackTerrain(const string&in) const",
		asMETHOD(T, canAttackTerrain), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this weapon can attack the "
		"given type of terrain.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int getBaseDamageTerrain(const string&in) const",
		asMETHOD(T, getBaseDamageTerrain), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the base damage that this weapon "
		"deals to the given type of terrain. If this weapon cannot attack the "
		"given terrain type, 0 will be returned.");
}

template<typename T>
void awe::unit_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document,
		"For unit types, this property is unused.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_movementTypeScriptName() const property",
		asMETHOD(T, getMovementTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's movement type script "
		"name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Movement@ get_movementType() const property",
		asMETHOD(T, _getMovementTypeObj), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on this unit's movement "
		"type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& pictureSprite(const string&in) const",
		asMETHODPR(T, getPicture, (const std::string&) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's "
		"picture, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& pictureSprite(const ArmyID) const",
		asMETHODPR(T, getPicture, (const awe::ArmyID) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's "
		"picture, given a country turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& unitSprite(const string&in) const",
		asMETHODPR(T, getUnit, (const std::string&) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's tile "
		"graphic that is displayed on the map, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& unitSprite(const ArmyID) const",
		asMETHODPR(T, getUnit, (const awe::ArmyID) const, const std::string&),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's tile "
		"graphic that is displayed on the map, given a country turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_cost() const property",
		asMETHOD(T, getCost), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's cost, in funds.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int get_maxFuel() const property",
		asMETHOD(T, getMaxFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's maximum fuel.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_maxHP() const property",
		asMETHOD(T, getMaxHP), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's maximum HP in internal "
		"format.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_movementPoints() const property",
		asMETHOD(T, getMovementPoints), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's movement points.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_vision() const property",
		asMETHOD(T, getVision), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's vision range.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasInfiniteFuel() const property",
		asMETHOD(T, hasInfiniteFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit's maximum fuel "
		"is less than 0.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canLoad(const string&in) const property",
		asMETHODPR(T, canLoad, (const std::string&) const, bool), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can load another "
		"type of unit, whose script name is given.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_loadLimit() const property",
		asMETHOD(T, loadLimit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's load limit.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_turnStartPriority() const property",
		asMETHOD(T, getTurnStartPriority), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit type's turn start priority "
		"level.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canCapture(const string&in) const property",
		asMETHODPR(T, canCapture, (const std::string&) const, bool),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can capture a "
		"type of terrain, whose script name is given.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canUnloadFrom(const string&in) const property",
		asMETHODPR(T, canUnloadFrom, (const std::string&) const, bool),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can unload units "
		"from a type of terrain, whose script name is given.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canHide() const property",
		asMETHOD(T, canHide), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this type of unit can "
		"hide, FALSE otherwise.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Weapon@ weapon(const string&in) const",
		asMETHOD(T, _getWeapon), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Retrieves a unit's weapon, given its "
		"script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Weapon@ weapon(const uint64) const",
		asMETHOD(T, _getWeaponByIndex), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Retrieves a unit's weapon, given its index "
		"in the list. Used to filter weapons based on their precedence.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint64 get_weaponCount() const property",
		asMETHOD(T, getWeaponCount), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Retrieves the number of weapons a unit "
		"possesses.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_ignoresDefence() const property",
		asMETHOD(T, ignoresDefence), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit always has 0 "
		"defence, FALSE if this unit's defence is based on the terrain it is "
		"positioned on.");
}

template<typename T>
void awe::commander::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document,
		"For commanders, this holds the sprite ID of the CO face shown on army "
		"panels, etc.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_portrait() const property",
		asMETHOD(T, getPortrait), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of the portrait of "
		"this CO.");
}
