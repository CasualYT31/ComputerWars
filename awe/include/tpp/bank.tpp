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

#pragma once

template<typename T>
awe::bank<T>::bank(const std::shared_ptr<engine::scripts>& scripts,
	const std::string& name, const engine::logger::data& data) :
	engine::json_script({ data.sink, "json_script" }), _logger(data),
	_scripts(scripts), _propertyName(name) {
	if (scripts) _scripts->addRegistrant(this);
}

template<typename T>
awe::bank<T>::~bank() noexcept {
	if (_scriptNamesAsArray) _scriptNamesAsArray->Release();
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
		indexOpStrDecl(
			"const " + _propertyName + "@ opIndex(const string&in) const"),
		getFirstItemDecl(
			"const " + _propertyName + "@ get_first() const property"),
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
	r = engine->RegisterObjectMethod(bankTypeName.c_str(),
		getFirstItemDecl.c_str(),
		asMETHOD(awe::bank<T>, _getFirstItem), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns a handle to the first property in "
		"the bank, as defined by the order specified in the JSON script.");
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
	if (_scriptNamesAsArray) _scriptNamesAsArray->Release();
	_scriptNamesAsArray =
		_scripts->createArrayFromContainer("string", _scriptNames);
	return true;
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
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_sound() const property",
		asMETHOD(T, getSound), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sound played when this weather is "
		"set in-game.");
}

template<typename T>
void awe::environment::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	awe::common_properties::Register<T>(type, engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_spritesheet() const property",
		asMETHOD(T, getSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of the tile spritesheet to "
		"use with this environment.");

	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_pictureSpritesheet() const property",
		asMETHOD(T, getPictureSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of the tile picture "
		"spritesheet to use with this environment.");

	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_structureIconSpritesheet() const property",
		asMETHOD(T, getStructureIconSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of the structure icon "
		"spritesheet to use with this environment.");
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
	// fow_visibility enum.
	auto r = engine->RegisterEnum("FoWVisibility");
	engine->RegisterEnumValue("FoWVisibility", "Normal",
		(int)awe::terrain::fow_visibility::Normal);
	engine->RegisterEnumValue("FoWVisibility", "Hidden",
		(int)awe::terrain::fow_visibility::Hidden);
	engine->RegisterEnumValue("FoWVisibility", "Visible",
		(int)awe::terrain::fow_visibility::Visible);
	document->DocumentObjectEnum(r, "Describes the vision properties a terrain "
		"has during Fog of War.");
	// Terrain type.
	r = engine->RegisterObjectMethod(type.c_str(),
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
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_isPaintable() const property",
		asMETHOD(T, isPaintable), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Is this terrain paintable in the map "
		"maker?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_primaryTileTypeScriptName() const property",
		asMETHOD(T, getPrimaryTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this terrain's primary tile type "
		"script name that was given to it at construction. Will only be valid if "
		"<tt>isPaintable</tt> returns <tt>TRUE</tt>.");
	// Forward declare TileType. Not the most clean, but at least when TileType is
	// registered for real, RegisterObjectType() will only fail silently.
	r = engine->RegisterObjectType("TileType", 0, asOBJ_REF | asOBJ_NOCOUNT);
	r = engine->RegisterObjectMethod(type.c_str(),
		"const TileType@ get_primaryTileType() const property",
		asMETHOD(T, _getPrimaryTileTypeObj), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on this terrain's primary "
		"tile type. Do not call this method if <tt>isPaintable</tt> returns "
		"<tt>FALSE</tt>.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"FoWVisibility get_FoWVisibility() const property",
		asMETHOD(T, getFoWVisibility), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the Fog of War visibility "
		"properties of this terrain.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_showOwnerWhenHidden() const property",
		asMETHOD(T, showOwnerWhenHidden), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "<tt>TRUE</tt> if tiles of this terrain "
		"should show as normal if they are hidden, <tt>FALSE</tt> if tiles of "
		"this terrain should always show as neutral if hidden.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int visionOffset(const string&in) const",
		asMETHOD(T, getVisionOffsetForUnitType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "If the given unit type has a vision offset "
		"for this terrain, it will be returned. If not, 0 is returned.");
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
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasOwnedProperties() const property",
		asMETHOD(T, hasOwnedProperties), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns <tt>TRUE</tt> if this tile type "
		"has at least one owned property sprite.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedPropertySprite(const string&in) const",
		asMETHODPR(T, getOwnedProperty, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's owned "
		"property that is displayed when it's being captured, given a country "
		"script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedPropertySprite(const ArmyID) const",
		asMETHODPR(T, getOwnedProperty, (const awe::ArmyID) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's owned "
		"property that is displayed when it's being captured, given a country "
		"turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_neutralPropertySprite() const property",
		asMETHOD(T, getNeutralProperty), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's "
		"property graphic that is displayed when it's being captured.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_alwaysPaintable() const property",
		asMETHOD(T, alwaysPaintable), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Is this tile type always paintable "
		"individually in the map maker, even when it forms part of at least one "
		"structure?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_isPaintable() const property",
		asMETHOD(T, isPaintable), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Is this tile type paintable individually "
		"in the map maker?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const array<string>@ const get_structures() const property",
		asMETHOD(T, structureScriptNames), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns a handle to an array that holds "
		"script names of structures that this tile type belongs to.");
}

template<typename T>
void awe::weapon::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	engine::RegisterVectorTypes(engine, document);
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
		"const string& get_idleSpritesheet() const property",
		asMETHOD(T, getIdleSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of this unit's idle "
		"spritesheet.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_upSpritesheet() const property",
		asMETHOD(T, getUpSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of this unit's up "
		"spritesheet.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_downSpritesheet() const property",
		asMETHOD(T, getDownSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of this unit's down "
		"spritesheet.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_leftSpritesheet() const property",
		asMETHOD(T, getLeftSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of this unit's left "
		"spritesheet.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_rightSpritesheet() const property",
		asMETHOD(T, getRightSpritesheet), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of this unit's right "
		"spritesheet.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& selectedSpritesheet(const string&in) const",
		asMETHODPR(T, getSelectedSpritesheet, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of the spritesheet used when "
		"this unit is selected on the map, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& selectedSpritesheet(const ArmyID) const",
		asMETHODPR(T, getSelectedSpritesheet, (const awe::ArmyID) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the name of the spritesheet used when "
		"this unit is selected on the map, given a country turn order ID.");
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
		"const string& destroyedUnitSprite(const string&in) const",
		asMETHODPR(T, getDestroyedUnit, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's tile "
		"graphic that is displayed on the map when it is destroyed, given a "
		"country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& destroyedUnitSprite(const ArmyID) const",
		asMETHODPR(T, getDestroyedUnit, (const awe::ArmyID) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's tile "
		"graphic that is displayed on the map when it is destroyed, given a "
		"country turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& capturingUnitSprite(const string&in) const",
		asMETHODPR(T, getCapturingUnit, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's larger "
		"graphic that is displayed when it is capturing, given a country script "
		"name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& capturingUnitSprite(const ArmyID) const",
		asMETHODPR(T, getCapturingUnit, (const awe::ArmyID) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's larger "
		"graphic that is displayed when it is has completed a capture, given a "
		"country turn order ID.");
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
	r = engine->RegisterObjectMethod(type.c_str(),
		"string get_hideSound() const property",
		asMETHOD(T, getHideSound), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the name of this unit's hide "
		"sound, or an empty string if there is none.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"string get_unhideSound() const property",
		asMETHOD(T, getUnhideSound), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the name of this unit's unhide "
		"sound, or an empty string if there is none.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"string get_destroySound() const property",
		asMETHOD(T, getDestroySound), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the name of this unit's destroy "
		"sound, or an empty string if there is none.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"string get_moveSound(const string&in = \"\", const bool = false) const",
		asMETHOD(T, getMoveSound), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the name of this unit's moving "
		"sound, or an empty string if there is none. The string parameter is the "
		"script name of the terrain the unit is moving on, if this sound is "
		"desired. If one of these is not defined, or the parameter is empty, the "
		"default move sound will be returned. The bool parameter, if "
		"<tt>TRUE</tt>, returns the sounds configured to play if the unit is "
		"hidden, if they are defined. If they are not defined, then the "
		"non-hidden sounds will be returned. Vice versa is also true, if a hidden "
		"sound is defined, but not the non-hidden one.");
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
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_theme() const property",
		asMETHOD(T, getTheme), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the music name of the theme of this "
		"CO.");
}

template<typename T>
void awe::structure::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	engine::RegisterVectorTypes(engine, document);
	awe::common_properties::Register<T>(type, engine, document,
		"For structures, this holds the sprite ID of the pictorial representation "
		"of the structure, for use in GUIs, when the owner of the structure "
		"doesn't matter.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasOwnedIcons() const property",
		asMETHOD(T, hasOwnedIcons), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does this structure have owned icons?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedIconName(const string&in) const",
		asMETHODPR(T, getOwnedIconName, (const std::string&) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this structure's "
		"owned icon that is displayed on the GUI, given a country script name.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& ownedIconName(const ArmyID) const",
		asMETHODPR(T, getOwnedIconName, (const awe::ArmyID) const,
			const std::string&), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this structure's "
		"owned icon that is displayed on the GUI, given a country turn order ID.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_rootTileType() const property",
		asMETHOD(T, getRootTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the root tile of "
		"the structure should be when in a normal state.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasRootDestroyedTileType() const property",
		asMETHOD(T, hasRootDestroyedTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does this structure's root tile turn into "
		"a different type when the structure is destroyed?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_rootDestroyedTileType() const property",
		asMETHOD(T, getRootDestroyedTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the root tile of "
		"the structure should be when in a destroyed state.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasRootDeletedTileType() const property",
		asMETHOD(T, hasRootDeletedTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does this structure's root tile turn into "
		"a different type when the structure is deleted?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_rootDeletedTileType() const property",
		asMETHOD(T, getRootDeletedTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the root tile of "
		"the structure should turn into when the structure is deleted.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint64 get_dependentTileCount() const property",
		asMETHOD(T, getDependentTileCount), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the number of dependent tiles this "
		"structure has.");
	r = engine->RegisterObjectMethod(type.c_str(), "const MousePosition& "
		"get_dependentTileOffset(const uint64) const property",
		asMETHOD(T, getDependentTileOffset), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the offset from the root tile that "
		"the specified dependent tile has.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_dependentTileType(const uint64) const property",
		asMETHOD(T, getDependentTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the specified "
		"dependent tile of the structure should be when in a normal state.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasDependentDestroyedTileType(const uint64) const property",
		asMETHOD(T, hasDependentDestroyedTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does this structure's specified dependent "
		"tile turn into a different type when the structure is destroyed?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_dependentDestroyedTileType(const uint64) const property",
		asMETHOD(T, getDependentDestroyedTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the specified "
		"dependent tile of the structure should be when in a destroyed state.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasDependentDeletedTileType(const uint64) const property",
		asMETHOD(T, hasDependentDeletedTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does this structure's specified dependent "
		"tile turn into a different type when the structure is deleted?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_dependentDeletedTileType(const uint64) const property",
		asMETHOD(T, getDependentDeletedTileTypeScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the tile type that the specified "
		"dependent tile of the structure should turn into when the structure is "
		"deleted.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_isPaintable() const property",
		asMETHOD(T, isPaintable), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Can this structure be painted all at once, "
		"as a set of tiles, in the map maker?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_keepUnitsWhenPainted() const property",
		asMETHOD(T, keepUnitsWhenPainted), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "If this structure is painted in the map "
		"maker, will it delete the units that are on the tiles it is being "
		"painted on, or will they stay alive?");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_destroyedName() const property",
		asMETHOD(T, getDestroyedName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the translation key of the long name "
		"of this structure when it is destroyed. If it was not assigned one, the "
		"normal long name is returned instead.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_destroyedIconName() const property",
		asMETHOD(T, getDestroyedIconName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "The key of the sprite representing a "
		"destroyed version of this structure.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool containsTileType(const string&in) const",
		asMETHOD(T, containsTileType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Does the given tile type appear at least "
		"once in this structure?");
}
