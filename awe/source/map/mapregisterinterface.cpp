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
#include "animations/include/tileparticles.hpp"

/* \c NO_ARMY that can be assigned to a script's interface. Due to limitations of
AngelScript, I unfortunately cannot register a constant with the script interface,
despite it being a constant as far as the scripts are concerned. */
static awe::ArmyID NO_ARMY_SCRIPT = awe::NO_ARMY;
static awe::UnitID NO_UNIT_SCRIPT = awe::NO_UNIT;
static auto MIN_TILE_WIDTH = awe::animated_tile::MIN_WIDTH;
static auto MIN_TILE_HEIGHT = awe::animated_tile::MIN_HEIGHT;

void awe::closed_list_node::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if ((!engine->GetTypeInfoByName("ClosedListNode"))) {
		auto r = RegisterType(engine, "ClosedListNode",
			[](asIScriptEngine* engine, const std::string& type) {
				engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
					std::string(type + "@ f()").c_str(),
					asFUNCTION(awe::closed_list_node::Create), asCALL_CDECL);
			});
		document->DocumentObjectType(r, "Holds information on a node in a closed "
			"list.");

		r = engine->RegisterObjectProperty("ClosedListNode", "Vector2 tile",
			asOFFSET(awe::closed_list_node, tile));

		r = engine->RegisterObjectProperty("ClosedListNode", "int g",
			asOFFSET(awe::closed_list_node, g));
	}
}

void awe::disable_mementos::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if ((!engine->GetTypeInfoByName("DisableMementos"))) {
		auto r = RegisterType(engine, "DisableMementos",
			[](asIScriptEngine* engine, const std::string& type) {
				engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
					std::string(type + "@ f(Map@ const, const string&in)").c_str(),
					asFUNCTION(awe::disable_mementos::Create), asCALL_CDECL);
			});
		document->DocumentObjectType(r, "A memento disable token. Used to disable "
			"creating mementos when performing operations on a map, then create a "
			"memento once those operations are complete.");

		r = engine->RegisterObjectMethod("DisableMementos", "void discard()",
			asMETHOD(awe::disable_mementos, discard), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "If an operation fails, but a disable "
			"token has already been made, you can use this method to cancel "
			"creating the memento.");
	}
}

void awe::map::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if (!engine->GetTypeInfoByName("Map")) {
		//////////////
		// MAP TYPE //
		//////////////
		// DisableMementos relies on Map, so declare it early.
		auto r = engine->RegisterObjectType("Map", 0, asOBJ_REF | asOBJ_NOCOUNT);
		document->DocumentObjectType(r, "Class representing a map.");

		//////////////////
		// DEPENDENCIES //
		//////////////////
		engine::RegisterVectorTypes(engine, document);
		engine::RegisterRectTypes(engine, document);
		engine::RegisterStreamTypes(engine, document);
		awe::RegisterGameTypedefs(engine, document);
		awe::closed_list_node::Register(engine, document);
		awe::disable_mementos::Register(engine, document);
		awe::tile_particle_node::Register(engine, document);

		/////////////////
		// SHADER ENUM //
		/////////////////
		r = engine->RegisterEnum("AvailableTileShader");
		engine->RegisterEnumValue("AvailableTileShader", "None",
			(int)awe::available_tile_shader::None);
		engine->RegisterEnumValue("AvailableTileShader", "Yellow",
			(int)awe::available_tile_shader::Yellow);
		engine->RegisterEnumValue("AvailableTileShader", "Red",
			(int)awe::available_tile_shader::Red);
		document->DocumentObjectEnum(r, "The list of shaders that can be applied "
			"to the tiles that are available.");

		///////////////////
		// QUADRANT ENUM //
		///////////////////
		r = engine->RegisterEnum("Quadrant");
		engine->RegisterEnumValue("Quadrant", "UpperLeft",
			static_cast<int>(awe::quadrant::UpperLeft));
		engine->RegisterEnumValue("Quadrant", "UpperRight",
			static_cast<int>(awe::quadrant::UpperRight));
		engine->RegisterEnumValue("Quadrant", "LowerLeft",
			static_cast<int>(awe::quadrant::LowerLeft));
		engine->RegisterEnumValue("Quadrant", "LowerRight",
			static_cast<int>(awe::quadrant::LowerRight));
		document->DocumentObjectEnum(r, "The different quadrants of a rectangle, "
			"such as a render target.");

		////////////////////
		// DIRECTION ENUM //
		////////////////////
		r = engine->RegisterEnum("Direction");
		engine->RegisterEnumValue("Direction", "Up",
			static_cast<int>(awe::direction::Up));
		engine->RegisterEnumValue("Direction", "Down",
			static_cast<int>(awe::direction::Down));
		engine->RegisterEnumValue("Direction", "Left",
			static_cast<int>(awe::direction::Left));
		engine->RegisterEnumValue("Direction", "Right",
			static_cast<int>(awe::direction::Right));
		document->DocumentObjectEnum(r, "Represents an orthogonal direction.");

		//////////////////////
		// GLOBAL FUNCTIONS //
		//////////////////////
		r = engine->RegisterGlobalFunction("HP GetDisplayedHP(const HP)",
			asFUNCTION(awe::unit_type::getDisplayedHP), asCALL_CDECL);

		r = engine->RegisterGlobalFunction("HP GetInternalHP(const HP)",
			asFUNCTION(awe::unit_type::getInternalHP), asCALL_CDECL);

		r = engine->RegisterGlobalFunction("uint Distance(const Vector2&in, "
			"const Vector2&in)", asFUNCTION(awe::distance), asCALL_CDECL);

		//////////////////////
		// GLOBAL CONSTANTS //
		//////////////////////
		r = engine->RegisterGlobalProperty("const ArmyID NO_ARMY",
			&NO_ARMY_SCRIPT);
		document->DocumentExpectedFunction("const ArmyID NO_ARMY", "Represents "
			"\"no army.\" Used to signify \"no ownership.\"");
		r = engine->RegisterGlobalProperty("const UnitID NO_UNIT",
			&NO_UNIT_SCRIPT);
		document->DocumentExpectedFunction("const UnitID NO_UNIT", "Represents "
			"\"no unit.\"");
		r = engine->RegisterGlobalProperty("const Vector2 NO_POSITION",
			&awe::unit::NO_POSITION_SCRIPT);
		document->DocumentExpectedFunction("const Vector2 NO_POSITION",
			"Represents \"no position/location\". Used to signify that a unit is "
			"not located on the map.");
		r = engine->RegisterGlobalProperty("const uint MIN_TILE_WIDTH",
			&MIN_TILE_WIDTH);
		document->DocumentExpectedFunction("const uint MIN_TILE_WIDTH",
			"A tile's minimum width, in pixels.");
		r = engine->RegisterGlobalProperty("const uint MIN_TILE_HEIGHT",
			&MIN_TILE_HEIGHT);
		document->DocumentExpectedFunction("const uint MIN_TILE_HEIGHT",
			"A tile's minimum height, in pixels.");

		//////////////
		// FUNCDEFS //
		//////////////
		r = engine->RegisterFuncdef("void MementoStateChangedCallback()");
		document->DocumentObjectFuncDef(r, "The signature of the callback that is "
			"invoked after memento state changes.");

		r = engine->RegisterFuncdef("void AnimationCode(any@ const)");
		document->DocumentObjectFuncDef(r, "The signature of functions that are "
			"added to the animation queue.");

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool save(const string&in = \"\")",
			asMETHODPR(awe::map, save, (const std::string&), bool),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setMapObject(ref@ const)",
			asMETHOD(awe::map, setMapObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool hasChanged()",
			asMETHOD(awe::map, hasChanged), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setMapName(string)",
			asMETHOD(awe::map, setMapName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getMapName() const",
			asMETHOD(awe::map, getMapName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void setMapSize("
			"const Vector2&in, const string&in, const ArmyID = NO_ARMY)",
			asMETHODPR(awe::map, setMapSize, (const sf::Vector2u&,
				const std::string&, const awe::ArmyID), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getMapSize() const",
			asMETHOD(awe::map, getMapSize), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool fillMap(const string&in, const ArmyID = NO_ARMY)",
			asMETHODPR(awe::map, fillMap, (const std::string&, const awe::ArmyID),
				bool), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool rectangleFillTiles(const Vector2&in, const Vector2&in,"
			"const string& in, const ArmyID = NO_ARMY)",
			asMETHODPR(awe::map, rectangleFillTiles, (const sf::Vector2u&,
				const sf::Vector2u&, const std::string&, const awe::ArmyID), bool),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool rectangleFillUnits(const Vector2&in, const Vector2&in,"
			"const string& in, const ArmyID)",
			asMETHODPR(awe::map, rectangleFillUnits, (const sf::Vector2u&,
				const sf::Vector2u&, const std::string&, const awe::ArmyID), bool),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"uint64 rectangleDeleteUnits(const Vector2&in, const Vector2&in)",
			asMETHOD(awe::map, rectangleDeleteUnits), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isOutOfBounds(const Vector2&in) const",
			asMETHOD(awe::map, _isOutOfBounds), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setDay(const Day)",
			asMETHOD(awe::map, setDay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Day getDay() const",
			asMETHOD(awe::map, getDay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void enableFoW(const bool)",
			asMETHOD(awe::map, enableFoW), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isFoWEnabled() const",
			asMETHOD(awe::map, isFoWEnabled), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setWeather(const string&in)",
			asMETHODPR(awe::map, setWeather, (const std::string&), void),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const Weather@ getWeather() const",
			asMETHOD(awe::map, getWeatherObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool defaultWinCondition() const",
			asMETHOD(awe::map, defaultWinCondition), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setAdditionalData(const string&in)",
			asMETHOD(awe::map, setAdditionalData), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getAdditionalData() const",
			asMETHOD(awe::map, getAdditionalData), asCALL_THISCALL);

		/////////////////////
		// ARMY OPERATIONS //
		/////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool createArmy(const string&in)",
			asMETHODPR(awe::map, createArmy, (const std::string&), bool),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void deleteArmy(const ArmyID, const ArmyID = NO_ARMY)",
			asMETHOD(awe::map, deleteArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"uint getArmyCount() const",
			asMETHOD(awe::map, getArmyCount), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<ArmyID>@ getArmyIDs() const",
			asMETHOD(awe::map, getArmyIDsAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isArmyPresent(const ArmyID) const",
			asMETHOD(awe::map, _isArmyPresent), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setArmyTeam(const ArmyID, const TeamID)",
			asMETHOD(awe::map, setArmyTeam), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"TeamID getArmyTeam(const ArmyID) const",
			asMETHOD(awe::map, getArmyTeam), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setArmyFunds(const ArmyID, const Funds)",
			asMETHOD(awe::map, setArmyFunds), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void offsetArmyFunds(const ArmyID, const Funds)",
			asMETHOD(awe::map, offsetArmyFunds), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Funds getArmyFunds(const ArmyID) const",
			asMETHOD(awe::map, getArmyFunds), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const Country@ getArmyCountry(const ArmyID) const",
			asMETHOD(awe::map, getArmyCountryObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void setArmyCOs(const ArmyID, "
			"const string&in, const string&in = \"\")",
			asMETHODPR(awe::map, setArmyCOs, (const awe::ArmyID,
				const std::string&, const std::string&), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setArmyCurrentCO(const ArmyID, const string&in)",
			asMETHODPR(awe::map, setArmyCurrentCO, (const awe::ArmyID,
				const std::string&), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setArmyTagCO(const ArmyID, const string&in)",
			asMETHODPR(awe::map, setArmyTagCO, (const awe::ArmyID,
				const std::string&), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void tagArmyCOs(const ArmyID)",
			asMETHOD(awe::map, tagArmyCOs), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getArmyCurrentCO(const ArmyID) const",
			asMETHOD(awe::map, getArmyCurrentCOScriptName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getArmyTagCO(const ArmyID) const",
			asMETHOD(awe::map, getArmyTagCOScriptName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool tagCOIsPresent(const ArmyID) const",
			asMETHOD(awe::map, tagCOIsPresent), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ getTilesOfArmy("
			"const ArmyID, const array<string>@ const = null) const",
			asMETHOD(awe::map, getTilesOfArmyAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<UnitID>@ getUnitsOfArmy(const ArmyID) const",
			asMETHOD(awe::map, getUnitsOfArmyAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<array<UnitID>@>@ "
			"getUnitsOfArmyByPriority(const ArmyID) const",
			asMETHOD(awe::map, getUnitsOfArmyByPriorityAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "uint64 "
			"countTilesBelongingToArmy(const ArmyID, const string&in) const",
			asMETHOD(awe::map, countTilesBelongingToArmy), asCALL_THISCALL);

		/////////////////////
		// UNIT OPERATIONS //
		/////////////////////
		r = engine->RegisterObjectMethod("Map",
			"UnitID createUnit(const string&in, const ArmyID)",
			asMETHODPR(awe::map, createUnit, (const std::string&,
				const awe::ArmyID), awe::UnitID),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void deleteUnit(const UnitID)",
			asMETHOD(awe::map, deleteUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const UnitType@ getUnitType(const UnitID) const",
			asMETHOD(awe::map, getUnitTypeObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setUnitPosition(const UnitID, const Vector2&in)",
			asMETHOD(awe::map, setUnitPosition), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getUnitPosition(const UnitID) const",
			asMETHOD(awe::map, getUnitPosition), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitOnMap(const UnitID) const",
			asMETHOD(awe::map, isUnitOnMap), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setUnitHP(const UnitID, const HP)",
			asMETHOD(awe::map, setUnitHP), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"HP getUnitHP(const UnitID) const",
			asMETHOD(awe::map, getUnitHP), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"HP getUnitDisplayedHP(const UnitID) const",
			asMETHOD(awe::map, getUnitDisplayedHP), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setUnitFuel(const UnitID, const Fuel)",
			asMETHOD(awe::map, setUnitFuel), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void burnUnitFuel(const UnitID, const Fuel)",
			asMETHOD(awe::map, burnUnitFuel), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Fuel getUnitFuel(const UnitID) const",
			asMETHOD(awe::map, getUnitFuel), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setUnitAmmo(const UnitID, const string&in, const Ammo)",
			asMETHOD(awe::map, setUnitAmmo), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Ammo getUnitAmmo(const UnitID, const string&in) const",
			asMETHOD(awe::map, getUnitAmmo), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"uint getUnitVision(const UnitID) const",
			asMETHOD(awe::map, getUnitVision), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void replenishUnit(const UnitID, const bool = false)",
			asMETHOD(awe::map, replenishUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitReplenished(const UnitID, const bool = false) const",
			asMETHOD(awe::map, isUnitReplenished), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void waitUnit(const UnitID, const bool)",
			asMETHOD(awe::map, waitUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitWaiting(const UnitID) const",
			asMETHOD(awe::map, isUnitWaiting), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void unitCapturing(const UnitID, const bool)",
			asMETHOD(awe::map, unitCapturing), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitCapturing(const UnitID) const",
			asMETHOD(awe::map, isUnitCapturing), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void unitHiding(const UnitID, const bool)",
			asMETHOD(awe::map, unitHiding), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitHiding(const UnitID) const",
			asMETHOD(awe::map, isUnitHiding), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitVisible(const UnitID, const ArmyID) const",
			asMETHOD(awe::map, isUnitVisible), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void loadUnit(const UnitID, const UnitID)",
			asMETHOD(awe::map, loadUnit), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "Loads the first unit onto the second "
			"one.");

		r = engine->RegisterObjectMethod("Map",
			"void unloadUnit(const UnitID, const UnitID, const Vector2&in)",
			asMETHOD(awe::map, unloadUnit), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "Unloads the first unit from the second "
			"one.");

		r = engine->RegisterObjectMethod("Map",
			"UnitID getUnitWhichContainsUnit(const UnitID)",
			asMETHOD(awe::map, getUnitWhichContainsUnit), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "Finds out the unit that a given unit "
			"is loaded on, if any. Returns NO_UNIT if none.");

		r = engine->RegisterObjectMethod("Map",
			"UnitID getUnloadedUnitWhichContainsUnit(const UnitID)",
			asMETHOD(awe::map, getUnloadedUnitWhichContainsUnit), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "Finds out the unloaded unit that a "
			"given unit is loaded on, directly or indirectly. Returns NO_UNIT if "
			"any unit in the chain was considered \"not present.\" Returns the "
			"given <tt>UnitID</tt> if the unit wasn't loaded onto another unit.");

		r = engine->RegisterObjectMethod("Map",
			"bool isUnitLoadedOntoUnit(const UnitID, const UnitID)",
			asMETHOD(awe::map, isUnitLoadedOntoUnit), asCALL_THISCALL);
		document->DocumentObjectMethod(r, "Returns TRUE if the first unit is "
			"loaded onto the second unit, FALSE otherwise.");

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getArmyOfUnit(const UnitID) const",
			asMETHOD(awe::map, getArmyOfUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"TeamID getTeamOfUnit(const UnitID) const",
			asMETHOD(awe::map, getTeamOfUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<UnitID>@ getLoadedUnits(const UnitID) const",
			asMETHOD(awe::map, getLoadedUnitsAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"uint getUnitDefence(const UnitID) const",
			asMETHOD(awe::map, getUnitDefence), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setUnitSpritesheet(const UnitID, const string&in)",
			asMETHOD(awe::map, setUnitSpritesheet), asCALL_THISCALL);

		/////////////////////
		// TILE OPERATIONS //
		/////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool setTileType(const Vector2&in, const string&in)",
			asMETHODPR(awe::map, setTileType, (const sf::Vector2u&,
				const std::string&), bool), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ "
			"querySetTileTypeChangedTiles(const Vector2&in)",
			asMETHOD(awe::map, querySetTileTypeChangedTilesAsArray),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const TileType@ getTileType(const Vector2&in) const",
			asMETHOD(awe::map, getTileTypeObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setTileHP(const Vector2&in, const HP)",
			asMETHOD(awe::map, setTileHP), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"HP getTileHP(const Vector2&in) const",
			asMETHOD(awe::map, getTileHP), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setTileOwner(const Vector2&in, const ArmyID)",
			asMETHOD(awe::map, setTileOwner), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getTileOwner(const Vector2&in) const",
			asMETHOD(awe::map, getTileOwner), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"UnitID getUnitOnTile(const Vector2&in) const",
			asMETHOD(awe::map, getUnitOnTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setTileStructureData(const Vector2&in, const string&in, "
			"const MousePosition&in, const bool)",
			asMETHODPR(awe::map, setTileStructureData, (const sf::Vector2u & pos,
				const std::string & structure, const sf::Vector2i & offset,
				const bool destroyed), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const Structure@ getTileStructure(const Vector2&in) const",
			asMETHOD(awe::map, getTileStructureObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isTileAStructureTile(const Vector2&in) const",
			asMETHOD(awe::map, isTileAStructureTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"MousePosition getTileStructureOffset(const Vector2&in) const",
			asMETHOD(awe::map, getTileStructureOffset), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isTileDestroyed(const Vector2&in) const",
			asMETHOD(awe::map, isTileDestroyed), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isTileVisible(const Vector2&in, const ArmyID) const",
			asMETHOD(awe::map, isTileVisible), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ "
			"getAvailableTiles(const Vector2&in, const uint, const uint) const",
			asMETHOD(awe::map, getAvailableTilesAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ "
			"getTilesInCone(const Vector2&in, const Direction, const uint, "
			"const uint) const",
			asMETHOD(awe::map, getTilesInConeAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<Vector2>@ getTilesInCrosshair(const Vector2&in) const",
			asMETHOD(awe::map, getTilesInCrosshairAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<Vector2>@ getTilesInLine(const Vector2&in, const Direction, "
			"const uint = 0) const",
			asMETHOD(awe::map, getTilesInLineAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ "
			"getTilesInArea(const Vector2&in, const Vector2&in) const",
			asMETHOD(awe::map, getTilesInAreaAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<ClosedListNode>@ "
			"findPath(const Vector2&in origin, const Vector2&in dest, "
			"const Movement&in moveType, const uint movePoints, const Fuel fuel, "
			"const TeamID team, const ArmyID army, const bool, const bool, "
			"const array<UnitID>@ const = null) const",
			asMETHOD(awe::map, findPathAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<ClosedListNode>@ "
			"findPathForUnloadUnit(const Vector2&in origin, "
			"const Vector2&in dest, const Movement&in moveType, const ArmyID, "
			"const array<UnitID>@ const = null) const",
			asMETHOD(awe::map, findPathAsArrayUnloadUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"int scanPath(const array<ClosedListNode>@, const UnitID, uint = 0) "
			"const",
			asMETHOD(awe::map, scanPath), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void convertTiles("
			"const array<Vector2>@ const, const string&in, const string&in, "
			"const ArmyID)",
			asMETHODPR(awe::map, convertTiles, (const CScriptArray* const,
				const std::string&, const std::string&, const awe::ArmyID), void),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool canStructureFit(const Vector2&in, const string&in) const",
			asMETHODPR(awe::map, canStructureFit, (const sf::Vector2u&,
				const std::string&) const, bool), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void destroyStructure(Vector2)",
			asMETHOD(awe::map, destroyStructure), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void deleteStructure(Vector2)",
			asMETHOD(awe::map, deleteStructure), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getTileTypeStructure(const string&in) const",
			asMETHODPR(awe::map, getTileTypeStructure, (const std::string&) const,
				std::string), asCALL_THISCALL);

		//////////////////////////////////////
		// SELECTED UNIT DRAWING OPERATIONS //
		//////////////////////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool setSelectedUnit(const UnitID)",
			asMETHOD(awe::map, setSelectedUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool pushSelectedUnit(const UnitID)",
			asMETHOD(awe::map, pushSelectedUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void popSelectedUnit()",
			asMETHOD(awe::map, popSelectedUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"UnitID getSelectedUnit() const",
			asMETHOD(awe::map, getSelectedUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void addAvailableTile(const Vector2&in)",
			asMETHOD(awe::map, addAvailableTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isAvailableTile(const Vector2&in) const",
			asMETHOD(awe::map, isAvailableTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void clearAvailableTiles()",
			asMETHOD(awe::map, clearAvailableTiles), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setAvailableTileShader(const AvailableTileShader)",
			asMETHOD(awe::map, setAvailableTileShader), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"AvailableTileShader getAvailableTileShader() const",
			asMETHOD(awe::map, getAvailableTileShader), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<ClosedListNode>& get_closedList() property",
			asMETHOD(awe::map, getClosedList), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void disableSelectedUnitRenderingEffects(const bool)",
			asMETHOD(awe::map, disableSelectedUnitRenderingEffects),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void disableShaderForAvailableUnits(const bool)",
			asMETHOD(awe::map, disableShaderForAvailableUnits),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void regenerateClosedListSprites()",
			asMETHOD(awe::map, regenerateClosedListSprites), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void addPreviewUnit(const UnitID, const Vector2&in)",
			asMETHOD(awe::map, addPreviewUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void removePreviewUnit(const UnitID)",
			asMETHOD(awe::map, removePreviewUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void removeAllPreviewUnits()",
			asMETHOD(awe::map, removeAllPreviewUnits), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isPreviewUnit(const UnitID) const",
			asMETHOD(awe::map, isPreviewUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool tileHasPreviewUnit(const Vector2&in, const UnitID) const",
			asMETHOD(awe::map, tileHasPreviewUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getUnitPreviewPosition(const UnitID) const",
			asMETHOD(awe::map, getUnitPreviewPosition), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"uint64 getUnitPreviewsCount() const",
			asMETHOD(awe::map, getUnitPreviewsCount), asCALL_THISCALL);

		////////////////////////
		// MEMENTO OPERATIONS //
		////////////////////////
		r = engine->RegisterObjectMethod("Map", "void addMemento(const string&in)",
			asMETHOD(awe::map, addMemento), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void undo(uint64 = 0)",
			asMETHOD(awe::map, undo), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void redo(uint64 = 0)",
			asMETHOD(awe::map, redo), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void disableMementos()",
			asMETHOD(awe::map, disableMementos), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool enableMementos(const string&in)",
			asMETHOD(awe::map, enableMementos), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void markChanged()",
			asMETHOD(awe::map, markChanged), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<string>@ getMementos(uint64&out) const",
			asMETHOD(awe::map, getMementosAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getNextUndoMementoName() const",
			asMETHOD(awe::map, getNextUndoMementoName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getNextRedoMementoName() const",
			asMETHOD(awe::map, getNextRedoMementoName), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void "
			"addMementoStateChangedCallback(MementoStateChangedCallback@ const)",
			asMETHOD(awe::map, addMementoStateChangedCallback), asCALL_THISCALL);

		////////////////////////
		// SCRIPTS OPERATIONS //
		////////////////////////
		r = engine->RegisterObjectMethod("Map",
			"void addScriptFile(const string&in, const string&in)",
			asMETHOD(awe::map, addScriptFile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void renameScriptFile(const string&in, const string&in)",
			asMETHOD(awe::map, renameScriptFile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void removeScriptFile(const string&in)",
			asMETHOD(awe::map, removeScriptFile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string buildScriptFiles()",
			asMETHOD(awe::map, buildScriptFiles), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getLastKnownBuildResult() const",
			asMETHOD(awe::map, getLastKnownBuildResult), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool doesScriptExist(const string&in) const",
			asMETHOD(awe::map, doesScriptExist), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getScript(const string&in) const",
			asMETHOD(awe::map, getScript), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"array<string>@ getScriptNames() const",
			asMETHOD(awe::map, getScriptNamesAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void enablePeriodic(const bool)",
			asMETHOD(awe::map, enablePeriodic), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool beginTurnForOwnedTile(Vector2, const Terrain@ const, "
			"const ArmyID)",
			asMETHOD(awe::map, beginTurnForOwnedTile), asCALL_THISCALL);

		////////////////////////
		// DRAWING OPERATIONS //
		////////////////////////
		r = engine->RegisterObjectMethod("Map",
			"void setViewport(const float, const float, const float, const float)",
			asMETHOD(awe::map, setViewport), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void alwaysShowHiddenUnits(const bool)",
			asMETHOD(awe::map, alwaysShowHiddenUnits), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool setSelectedTile(const Vector2&in)",
			asMETHOD(awe::map, setSelectedTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool moveSelectedTileUp()",
			asMETHOD(awe::map, moveSelectedTileUp), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool moveSelectedTileDown()",
			asMETHOD(awe::map, moveSelectedTileDown), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool moveSelectedTileLeft()",
			asMETHOD(awe::map, moveSelectedTileLeft), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool moveSelectedTileRight()",
			asMETHOD(awe::map, moveSelectedTileRight), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool setSelectedTileByPixel(const MousePosition&in)",
			asMETHOD(awe::map, setSelectedTileByPixel), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getSelectedTile() const",
			asMETHOD(awe::map, getSelectedTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setAdditionallySelectedTile(const Vector2&in)",
			asMETHOD(awe::map, setAdditionallySelectedTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void clearAdditionallySelectedTile()",
			asMETHOD(awe::map, clearAdditionallySelectedTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setULAdditionalCursorSprite(const string&in)",
			asMETHOD(awe::map, setULAdditionalCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setURAdditionalCursorSprite(const string&in)",
			asMETHOD(awe::map, setURAdditionalCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setLLAdditionalCursorSprite(const string&in)",
			asMETHOD(awe::map, setLLAdditionalCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setLRAdditionalCursorSprite(const string&in)",
			asMETHOD(awe::map, setLRAdditionalCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setSelectedArmy(const ArmyID)",
			asMETHOD(awe::map, setSelectedArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getSelectedArmy() const",
			asMETHOD(awe::map, getSelectedArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getNextArmy() const",
			asMETHOD(awe::map, getNextArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getFirstArmy() const",
			asMETHOD(awe::map, getFirstArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setSelectedArmyOverride(const ArmyID)",
			asMETHOD(awe::map, setSelectedArmyOverride), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"ArmyID getOverriddenSelectedArmy() const",
			asMETHOD(awe::map, getOverriddenSelectedArmy), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void clearSelectedArmyOverride()",
			asMETHOD(awe::map, clearSelectedArmyOverride), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setMapScalingFactor(const float, const bool = true)",
			asMETHOD(awe::map, setMapScalingFactor), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isCursorOnLeftSide() const",
			asMETHOD(awe::map, isCursorOnLeftSide), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool isCursorOnTopSide() const",
			asMETHOD(awe::map, isCursorOnTopSide), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Quadrant getCursorQuadrant() const",
			asMETHOD(awe::map, getCursorQuadrant), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setRectangleSelectionStart(const Vector2&in)",
			asMETHOD(awe::map, setRectangleSelectionStart), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setRectangleSelectionEnd(const Vector2&in)",
			asMETHOD(awe::map, setRectangleSelectionEnd), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void removeRectangleSelection()",
			asMETHOD(awe::map, removeRectangleSelection), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getRectangleSelectionStart() const",
			asMETHOD(awe::map, getRectangleSelectionStart), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getRectangleSelectionEnd() const",
			asMETHOD(awe::map, getRectangleSelectionEnd), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setULCursorSprite(const string&in)",
			asMETHOD(awe::map, setULCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setURCursorSprite(const string&in)",
			asMETHOD(awe::map, setURCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setLLCursorSprite(const string&in)",
			asMETHOD(awe::map, setLLCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setLRCursorSprite(const string&in)",
			asMETHOD(awe::map, setLRCursorSprite), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"IntRect getCursorBoundingBox() const",
			asMETHOD(awe::map, getCursorBoundingBox), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"IntRect getMapBoundingBox() const",
			asMETHOD(awe::map, getMapBoundingBox), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "void shake(const float = 1.5)",
			asMETHOD(awe::map, shakeMap), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setEnvironment(const string&in)",
			asMETHODPR(awe::map, setEnvironment, (const std::string&), void),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"const Environment@ getEnvironment() const",
			asMETHOD(awe::map, getEnvironmentObject), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getEnvironmentSpritesheet() const",
			asMETHOD(awe::map, getEnvironmentSpritesheet), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getEnvironmentPictureSpritesheet() const",
			asMETHOD(awe::map, getEnvironmentPictureSpritesheet), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"string getEnvironmentStructureIconSpritesheet() const",
			asMETHOD(awe::map, getEnvironmentStructureIconSpritesheet),
			asCALL_THISCALL);

		//////////////////////////
		// ANIMATION OPERATIONS //
		//////////////////////////
		r = engine->RegisterObjectMethod("Map",
			"void enableAnimations(const bool)",
			asMETHOD(awe::map, enableAnimations), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void queueCode(AnimationCode@ const, any@ const = null)",
			asMETHOD(awe::map, queueCode), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void queuePlay(const string&in, const string&in, const float = 1.0)",
			asMETHOD(awe::map, queuePlay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void queueStop(const string&in, const string&in)",
			asMETHODPR(awe::map, queueStop,
				(const std::string&, const std::string&), void), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool queueDelay(const float, const bool = false)",
			asMETHOD(awe::map, queueDelay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool animateDayBegin(const ArmyID, const Day, const string&in)",
			asMETHOD(awe::map, animateDayBegin), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool animateTagCO(const ArmyID, const string&in)",
			asMETHOD(awe::map, animateTagCO), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool animateParticles(const array<TileParticle>@ const, "
			"const string&in, const string&in = \"\", const string&in = \"\")",
			asMETHOD(awe::map, animateParticles), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool animateLabelUnit(const UnitID, const string&in, "
			"const string&in, const string&in = \"\", const string&in = \"\", "
			"const string&in = \"\", const string&in = \"\", const float = 0.7)",
			asMETHOD(awe::map, animateLabelUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool animateCapture(const Vector2&in, const UnitID, const HP, "
			"const HP, const string&in, const string&in, const string&in, "
			"const string&in)",
			asMETHOD(awe::map, animateCapture), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "bool animateMoveUnit("
			"const UnitID, const array<ClosedListNode>@ const)",
			asMETHOD(awe::map, animateMoveUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "bool animateScroll("
			"const Vector2&in, const float, const bool = false)",
			asMETHOD(awe::map, animateViewScroll), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "bool animateLaunchOrStrike("
			"const bool, const string&in, const string&in, const Vector2&in, "
			"const float = 1.0)",
			asMETHOD(awe::map, animateLaunchOrStrike), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "bool animateNextTurn("
			"const ArmyID, const ArmyID, const array<string>@ const)",
			asMETHOD(awe::map, animateNextTurn), asCALL_THISCALL);
	}
}

