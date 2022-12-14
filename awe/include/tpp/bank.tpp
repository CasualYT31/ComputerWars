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

#pragma once

template<typename T>
awe::bank<T>::bank(const std::shared_ptr<engine::scripts>& scripts,
	const std::string& name, const std::string& logName) noexcept :
	_logger(logName), _scripts(scripts), _propertyName(name) {
	if (scripts) _scripts->addRegistrant(this);
}

template<typename T>
std::shared_ptr<const T> awe::bank<T>::operator[](const awe::BankID id) const
	noexcept {
	if (id >= size()) {
		_logger.error("Game property {} does not exist in this bank!", id);
		return nullptr;
	}
	return _bank[id];
}

template<typename T>
std::shared_ptr<const T> awe::bank<T>::operator[](const std::string& sn) const
	noexcept {
	for (auto& prop : _bank) if (prop->getScriptName() == sn) return prop;
	_logger.error("Game property \"{}\" does not exist in this bank!", sn);
	return nullptr;
}

template<typename T>
std::size_t awe::bank<T>::size() const noexcept {
	return _bank.size();
}

template<typename T>
void awe::bank<T>::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	// 1. Register the game typedefs to ensure that they are defined.
	awe::RegisterGameTypedefs(engine, document);
	// 2. Register the value type that this bank stores (i.e. T).
	auto r = engine->RegisterObjectType(_propertyName.c_str(), sizeof(T),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<T>());
	T::Register<T>(_propertyName, engine, document);
	// 3. Register a single reference type, called _propertyName + "Bank".
	const std::string bankTypeName(_propertyName + "Bank"),
		indexOpIntDecl("const " + _propertyName + " opIndex(BankID)"),
		indexOpStrDecl("const " + _propertyName + " opIndex(string)"),
		globalPropDecl(bankTypeName + " " +
			tgui::String(_propertyName).toLower().toStdString());
	r = engine->RegisterObjectType(bankTypeName.c_str(), 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	document->DocumentObjectType(r, "Holds a collection of related game "
		"properties.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(), indexOpIntDecl.c_str(),
		asMETHOD(awe::bank<T>, _opIndexInt), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Access a game property by its index.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(), indexOpStrDecl.c_str(),
		asMETHOD(awe::bank<T>, _opIndexStr), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Access a game property by its script "
		"name.");
	r = engine->RegisterObjectMethod(bankTypeName.c_str(), "uint length() const",
		asMETHOD(awe::bank<T>, size), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the number of game properties stored "
		"in this bank.");
	// 4. Register the global point of access to the _propertyName + "Bank" object.
	engine->RegisterGlobalProperty(globalPropDecl.c_str(), this);
	document->DocumentExpectedFunction(globalPropDecl, "The single point of "
		"access to the relevant game properties. Declared by the game engine.");
}

template<typename T>
bool awe::bank<T>::_load(engine::json& j) noexcept {
	_bank.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	awe::BankID id = 0;
	for (auto& i : jj.items()) {
		// Loop through each object, allowing the template type T to construct its
		// values based on each object.
		engine::json input(i.value());
		_bank.push_back(std::make_shared<const T>(id++, i.key(), input));
	}
	return true;
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

template<typename T>
const T awe::bank<T>::_opIndexInt(const awe::BankID id) const {
	auto ret = operator[](id);
	if (ret)
		return *ret;
	else
		throw std::runtime_error("Could not access game property");
}

template<typename T>
const T awe::bank<T>::_opIndexStr(const std::string name) const {
	auto ret = operator[](name);
	if (ret)
		return *ret;
	else
		throw std::runtime_error("Could not access game property");
}

template<typename T>
void awe::bank_id::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"BankID get_ID() const property",
		asMETHOD(T, getID), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the index of this game property.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"string get_scriptName() const property",
		asMETHOD(T, getScriptName), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the script name of this game "
		"property.");
}

template<typename T>
void awe::common_properties::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document,
	const std::string& extraIconDoc) noexcept {
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
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document);
	engine::RegisterColourType(engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const Colour& get_colour() const property",
		asMETHOD(T, getColour), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the colour of the country.");
}

template<typename T>
void awe::weather::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::environment::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::movement_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document);
}

template<typename T>
void awe::terrain::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
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
		"int get_moveCost(const BankID) const property",
		asMETHOD(T, getMoveCost), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the movement cost of this terrain "
		"type, given a movement type index.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_picture(const BankID) const property",
		asMETHOD(T, getPicture), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this terrain type's "
		"picture, given a country index.");
}

template<typename T>
void awe::tile_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::bank_id::Register<T>(type, engine, document);
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"BankID get_typeIndex() const property",
		asMETHOD(T, getTypeIndex), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the index of this tile's terrain "
		"type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Terrain get_type() const property",
		asMETHOD(T, getTypeObj), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on this tile's terrain "
		"type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_ownedTileSprite(const BankID) const property",
		asMETHOD(T, getOwnedTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's owned "
		"tile that is displayed on the map, given a country index.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_neutralTileSprite() const property",
		asMETHOD(T, getNeutralTile), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this tile's tile "
		"graphic that is displayed on the map.");
}

template<typename T>
void awe::unit_type::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document,
		"For unit types, this property is unused.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"BankID get_movementTypeIndex() const property",
		asMETHOD(T, getMovementTypeIndex), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's movement type index.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const Movement get_movementType() const property",
		asMETHOD(T, getMovementTypeObj), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on this unit's movement "
		"type.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_pictureSprite(const BankID) const property",
		asMETHOD(T, getPicture), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's "
		"picture, given a country index.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_unitSprite(const BankID) const property",
		asMETHOD(T, getUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of this unit's tile "
		"graphic that is displayed on the map, given a country index.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_cost() const property",
		asMETHOD(T, getCost), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's cost, in funds.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int get_maxFuel() const property",
		asMETHOD(T, getMaxFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's maximum fuel.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"int get_maxAmmo() const property",
		asMETHOD(T, getMaxAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's maximum ammo.");
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
		"uint get_lowerRange() const property",
		asMETHOD(T, getLowerRange), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's lowest attack range.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"uint get_higherRange() const property",
		asMETHOD(T, getHigherRange), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets this unit's highest attack range.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasInfiniteFuel() const property",
		asMETHOD(T, hasInfiniteFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit's maximum fuel "
		"is less than 0.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_hasInfiniteAmmo() const property",
		asMETHOD(T, hasInfiniteAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit's maximum ammo "
		"is less than 0.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canLoad(const BankID) const property",
		asMETHODPR(T, canLoad, (const awe::BankID) const, bool), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can load another "
		"type of unit, whose index is given.");
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
		"bool get_canCapture(const BankID) const property",
		asMETHODPR(T, canCapture, (const awe::BankID) const, bool),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can capture a "
		"type of terrain, whose index is given.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canUnloadFrom(const BankID) const property",
		asMETHODPR(T, canUnloadFrom, (const awe::BankID) const, bool),
		asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this unit can unload units "
		"from a type of terrain, whose index is given.");
	r = engine->RegisterObjectMethod(type.c_str(),
		"bool get_canHide() const property",
		asMETHOD(T, canHide), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns TRUE if this type of unit can "
		"hide, FALSE otherwise.");
}

template<typename T>
void awe::commander::Register(const std::string& type,
	asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::common_properties::Register<T>(type, engine, document,
		"For commanders, this holds the sprite ID of the CO face shown on army "
		"panels, etc.");
	auto r = engine->RegisterObjectMethod(type.c_str(),
		"const string& get_portrait() const property",
		asMETHOD(T, getPortrait), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Gets the sprite key of the portrait of "
		"this CO.");
}
