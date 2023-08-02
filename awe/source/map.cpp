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

#include "map.hpp"
#include "fmtawe.hpp"

/* \c NO_ARMY that can be assigned to a script's interface. Due to limitations of
AngelScript, I unfortunately cannot register a constant with the script interface,
despite it being a constant as far as the scripts are concerned. So I had to make
this an evil global non-const. */
awe::ArmyID NO_ARMY_SCRIPT = awe::NO_ARMY;
auto MIN_TILE_WIDTH = awe::tile::MIN_WIDTH;
auto MIN_TILE_HEIGHT = awe::tile::MIN_HEIGHT;

static const std::runtime_error NO_SCRIPTS("No scripts object was given to this "
	"map object!");

awe::closed_list_node::closed_list_node(const sf::Vector2u& tileIn,
	const int gIn) : tile(tileIn), g(gIn) {}

void awe::closed_list_node::Register(asIScriptEngine * engine,
		const std::shared_ptr<DocumentationGenerator>&document) {
	auto r = RegisterType(engine, "ClosedListNode");
	document->DocumentObjectType(r, "Holds information on a node in a closed "
		"list.");

	r = engine->RegisterObjectProperty("ClosedListNode", "Vector2 tile",
		asOFFSET(awe::closed_list_node, tile));

	r = engine->RegisterObjectProperty("ClosedListNode", "int g",
		asOFFSET(awe::closed_list_node, g));
}

void awe::map::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if (!engine->GetTypeInfoByName("Map")) {
		//////////////////
		// DEPENDENCIES //
		//////////////////
		engine::RegisterVectorTypes(engine, document);
		engine::RegisterRectTypes(engine, document);
		engine::RegisterFileType(engine, document);
		awe::RegisterGameTypedefs(engine, document);
		awe::closed_list_node::Register(engine, document);

		/////////////////
		// SHADER ENUM //
		/////////////////
		auto r = engine->RegisterEnum("AvailableTileShader");
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
			"\"no army\". Used to signify \"no ownership.\"");
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
		// MAP TYPE //
		//////////////
		r = engine->RegisterObjectType("Map", 0, asOBJ_REF | asOBJ_NOCOUNT);
		document->DocumentObjectType(r, "Class representing a map.");

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool save(const string&in = \"\")",
			asMETHODPR(awe::map, save, (const std::string&), bool),
			asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool hasChanged()",
			asMETHOD(awe::map, hasChanged), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setMapName(const string&in)",
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
			"bool isOutOfBounds(const Vector2&in) const",
			asMETHOD(awe::map, _isOutOfBounds), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setDay(const Day)",
			asMETHOD(awe::map, setDay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Day getDay() const",
			asMETHOD(awe::map, getDay), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"bool defaultWinCondition() const",
			asMETHOD(awe::map, defaultWinCondition), asCALL_THISCALL);

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

		r = engine->RegisterObjectMethod("Map",
			"array<Vector2>@ getTilesOfArmy(const ArmyID) const",
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
			"void replenishUnit(const UnitID, const bool = false)",
			asMETHOD(awe::map, replenishUnit), asCALL_THISCALL);

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
			"is loaded on, if any. Returns 0 otherwise.");

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

		/////////////////////
		// TILE OPERATIONS //
		/////////////////////
		r = engine->RegisterObjectMethod("Map",
			"bool setTileType(const Vector2&in, const string&in)",
			asMETHODPR(awe::map, setTileType, (const sf::Vector2u&,
				const std::string&), bool), asCALL_THISCALL);

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

		r = engine->RegisterObjectMethod("Map", "array<Vector2>@ "
			"getAvailableTiles(const Vector2&in, uint, uint) const",
			asMETHOD(awe::map, getAvailableTilesAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<ClosedListNode>@ "
			"findPath(const Vector2&in origin, const Vector2&in dest, "
			"const Movement&in moveType, const uint movePoints, const Fuel fuel, "
			"const TeamID team, const ArmyID army, array<UnitID>@ = null) const",
			asMETHOD(awe::map, findPathAsArray), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map", "array<ClosedListNode>@ "
			"findPathForUnloadUnit(const Vector2&in origin, "
			"const Vector2&in dest, const Movement&in moveType, const ArmyID, "
			"array<UnitID>@ = null) const",
			asMETHOD(awe::map, findPathAsArrayUnloadUnit), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"int scanPath(const array<ClosedListNode>@, const UnitID, uint = 0) "
			"const",
			asMETHOD(awe::map, scanPath), asCALL_THISCALL);

		////////////////////////
		// DRAWING OPERATIONS //
		////////////////////////
		r = engine->RegisterObjectMethod("Map",
			"void alwaysShowHiddenUnits(const bool)",
			asMETHOD(awe::map, alwaysShowHiddenUnits), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setSelectedTile(const Vector2&in)",
			asMETHOD(awe::map, setSelectedTile), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void moveSelectedTileUp()",
			asMETHOD(awe::map, moveSelectedTileUp), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void moveSelectedTileDown()",
			asMETHOD(awe::map, moveSelectedTileDown), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void moveSelectedTileLeft()",
			asMETHOD(awe::map, moveSelectedTileLeft), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void moveSelectedTileRight()",
			asMETHOD(awe::map, moveSelectedTileRight), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"void setSelectedTileByPixel(const MousePosition&in)",
			asMETHOD(awe::map, setSelectedTileByPixel), asCALL_THISCALL);

		r = engine->RegisterObjectMethod("Map",
			"Vector2 getSelectedTile() const",
			asMETHOD(awe::map, getSelectedTile), asCALL_THISCALL);

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
			"void setMapScalingFactor(const float) const",
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
	}
}

awe::map::map(const engine::logger::data& data) : _logger(data),
	_file({ data.sink, data.name + "_binary_file" }),
	_cursor({ data.sink, data.name + "_cursor_sprite" }) {
	_initShaders();
}

awe::map::map(const std::shared_ptr<awe::bank<awe::country>>& countries,
	const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
	const std::shared_ptr<awe::bank<awe::unit_type>>& units,
	const std::shared_ptr<awe::bank<awe::commander>>& commanders,
	const engine::logger::data& data) : _logger(data),
	_file({ data.sink, data.name + "_binary_file" }),
	_cursor({data.sink, data.name + "_cursor_sprite"}) {
	_countries = countries;
	_tileTypes = tiles;
	_unitTypes = units;
	_commanders = commanders;
	_initShaders();
}

bool awe::map::load(std::string file, const unsigned char version) {
	if (file == "") file = _filename;
	// Clear state.
	_sel = sf::Vector2u(0, 0);
	_currentArmy = awe::NO_ARMY;
	_lastUnitID = 1;
	_armies.clear();
	_units.clear();
	_tiles.clear();
	_mapName = "";
	_day = 0;
	_viewOffsetX.reset();
	_viewOffsetY.reset();
	// Load new state.
	try {
		_file.open(file, true);
		_filename = file;
		if (!_scripts) {
			throw std::runtime_error("no scripts object!");
		} else if (!_scripts->functionDeclExists(
			"void LoadMap(BinaryFile@, Map@, uint8)")) {
			throw std::runtime_error("void LoadMap(BinaryFile@, Map@, uint8) not "
				"found in the scripts!");
		}
		_scripts->callFunction("LoadMap", &_file, this, version);
		_file.close();
	} catch (const std::exception& e) {
		_logger.critical("Map loading operation: couldn't load map file \"{}\": "
			"{}", file, e);
		_file.close();
		return false;
	}
	_changed = false;
	return true;
}

bool awe::map::save(std::string file, const unsigned char version) {
	if (file == "") file = _filename;
	try {
		_file.open(file, false);
		_filename = file;
		if (!_scripts) {
			throw std::runtime_error("no scripts object!");
		} else if (!_scripts->functionDeclExists(
			"void SaveMap(BinaryFile@, Map@, uint8)")) {
			throw std::runtime_error("void SaveMap(BinaryFile@, Map@, uint8) not "
				"found in the scripts!");
		}
		_scripts->callFunction("SaveMap", &_file, this, version);
		_file.close();
	} catch (const std::exception& e) {
		_logger.critical("Map saving operation: couldn't save map file \"{}\": {}",
			file, e);
		_file.close();
		return false;
	}
	_changed = false;
	return true;
}

bool awe::map::save(const std::string& file) {
	return save(file, 0);
}

void awe::map::setScripts(const std::shared_ptr<engine::scripts>& scripts) {
	if (scripts) {
		if (_selectedUnitRenderData.empty()) {
			_selectedUnitRenderData.emplace(*scripts);
		}
	}
	_scripts = scripts;
}

bool awe::map::hasChanged() const {
	return _changed;
}

bool awe::map::periodic() {
	return defaultWinCondition();
}

void awe::map::setMapName(const std::string& name) {
	_mapName = name;
	_changed = true;
}

std::string awe::map::getMapName() const {
	return _mapName;
}

void awe::map::setMapSize(const sf::Vector2u& dim,
	const std::shared_ptr<const awe::tile_type>& tile, const awe::ArmyID owner) {
	if (dim == getMapSize()) return;
	// First, resize the tiles vectors accordingly.
	bool mapHasShrunk = (getMapSize().x > dim.x || getMapSize().y > dim.y);
	_tiles.resize(dim.x);
	for (std::size_t x = 0; x < dim.x; ++x) {
		_tiles[x].resize(dim.y, { { _logger.getData().sink, "tile" }, tile, owner,
			_sheet_tile });
	}
	_mapSizeCache = dim;
	if (mapHasShrunk) {
		// Then, go through all owned tiles in each army and delete those that are
		// now out of bounds.
		for (auto& army : _armies) {
			auto tiles = army.second.getTiles();
			for (auto& tile : tiles) {
				if (_isOutOfBounds(tile)) army.second.removeTile(tile);
			}
		}
		// Then, go through all units and delete those that are out of bounds.
		std::vector<awe::UnitID> unitsToDelete;
		for (auto& itr : _units) {
			if (_isOutOfBounds(itr.second.getPosition()))
				unitsToDelete.push_back(itr.first);
		}
		// Check if they are still present, as some of those IDs may be for units
		// that are loaded, which will be deleted as their holder unit is deleted.
		for (auto& itr : unitsToDelete) if (_isUnitPresent(itr)) deleteUnit(itr);
		// Finally, if the currently selected tile is now out of bounds, adjust it.
		if (_isOutOfBounds(_sel)) {
			if (dim.x == 0)
				// Will still be out of bounds: this should be checked for anyway
				// in the drawing code.
				_sel.x = 0;
			else if (_sel.x >= dim.x)
				_sel.x = dim.x - 1;
			if (dim.y == 0)
				// Will still be out of bounds: this should be checked for anyway
				// in the drawing code.
				_sel.y = 0;
			else if (_sel.y >= dim.y)
				_sel.y = dim.y - 1;
		}
	}
	_changed = true;
}

void awe::map::setMapSize(const sf::Vector2u& dim, const std::string& tile,
	const awe::ArmyID owner) {
	setMapSize(dim, _tileTypes->operator[](tile), owner);
}

void awe::map::setDay(const awe::Day day) noexcept {
	_day = day;
	_changed = true;
}

awe::Day awe::map::getDay() const noexcept {
	return _day;
}

bool awe::map::defaultWinCondition() const {
	if (_armies.size() == 0) return true;
	const auto firstArmysTeam = _armies.begin()->second.getTeam();
	for (const auto& army : _armies) {
		if (army.second.getTeam() != firstArmysTeam) return false;
	}
	return true;
}

bool awe::map::createArmy(const std::shared_ptr<const awe::country>& country) {
	if (!country) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with no country!");
		return false;
	}
	if (_isArmyPresent(country->getTurnOrder())) {
		_logger.error("createArmy operation cancelled: attempted to create an "
			"army with a country, \"{}\", that already existed on the map!",
			country->getName());
		return false;
	}
	// Create the army.
	_armies.insert(
		std::pair<awe::ArmyID, awe::army>(country->getTurnOrder(), country)
	);
	// This will miss out the maximum value for a team ID, but I don't care.
	if (_teamIDCounter == std::numeric_limits<awe::TeamID>::max())
		_teamIDCounter = 0;
	_armies.at(country->getTurnOrder()).setTeam(_teamIDCounter++);
	_changed = true;
	return true;
}

bool awe::map::createArmy(const std::string& country) {
	return createArmy(_countries->operator[](country));
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
	// Firstly, delete all units belonging to the army.
	auto units = _armies.at(army).getUnits();
	for (auto unit : units) {
		deleteUnit(unit);
	}
	// Then, disown all tiles.
	auto tiles = _armies.at(army).getTiles();
	for (auto& tile : tiles) {
		setTileOwner(tile, transferOwnership);
	}
	// Finally, delete the army from the army list.
	_armies.erase(army);
	_changed = true;
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
	if (_scripts) {
		auto set = getArmyIDs();
		CScriptArray* ret = _scripts->createArray("ArmyID");
		for (auto element : set) {
			ret->InsertLast(&element);
		}
		return ret;
	} else {
		throw NO_SCRIPTS;
	}
}

void awe::map::setArmyTeam(const awe::ArmyID army, const awe::TeamID team) {
	if (_isArmyPresent(army)) {
		_armies.at(army).setTeam(team);
		// First, stop all of the army's units from capturing.
		const auto units = getUnitsOfArmy(army);
		for (auto& unit : units) _updateCapturingUnit(unit);
		// Then, stop all of the units capturing that are on this army's tiles.
		const auto tiles = getTilesOfArmy(army);
		for (auto& tile : tiles) _updateCapturingUnit(getUnitOnTile(tile));
		_changed = true;
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
		_armies.at(army).setFunds(funds);
		_changed = true;
	} else {
		_logger.error("setArmyFunds operation cancelled: attempted to set {} "
			"funds to an army, {}, that didn't exist!", funds, army);
	}
}

void awe::map::offsetArmyFunds(const awe::ArmyID army, const awe::Funds funds) {
	if (_isArmyPresent(army)) {
		setArmyFunds(army, getArmyFunds(army) + funds);
		_changed = true;
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

std::shared_ptr<const awe::country>
	awe::map::getArmyCountry(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getCountry();
	_logger.error("getArmyCountry operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return nullptr;
}

const awe::country* awe::map::getArmyCountryObject(const awe::ArmyID army) const {
	auto ret = getArmyCountry(army);
	if (ret) {
		return ret.get();
	} else {
		throw std::out_of_range("This army does not exist!");
	}
}

void awe::map::setArmyCOs(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& current,
	const std::shared_ptr<const awe::commander>& tag) {
	if (_isArmyPresent(army)) {
		if (!current && !tag) {
			_logger.error("setCOs operation failed: army with ID {} was given no "
				"COs!", army);
		} else {
			if (!current && tag) {
				_logger.warning("setCOs operation: army with ID {} was given a "
					"tag CO but not current CO! The army will instead be assigned "
					"a current CO and it will not be assigned a tag CO.", army);
			}
			_armies.at(army).setCOs(current, tag);
			_changed = true;
		}
	} else {
		_logger.error("setCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
	}
}

void awe::map::setArmyCOs(const awe::ArmyID army, const std::string& current,
	const std::string& tag) {
	if (current.empty() && tag.empty()) {
		_logger.error("setArmyCOs operation cancelled: both the current and tag "
			"CO script names given were blank!");
		return;
	}
	if (tag.empty()) {
		setArmyCOs(army, _commanders->operator[](current), nullptr);
	} else {
		setArmyCOs(army, _commanders->operator[](current),
			_commanders->operator[](tag));
	}
}

void awe::map::setArmyCurrentCO(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& current) {
	setArmyCOs(army, current, getArmyTagCO(army));
}

void awe::map::setArmyCurrentCO(const awe::ArmyID army,
	const std::string& current) {
	setArmyCOs(army, _commanders->operator[](current), getArmyTagCO(army));
}

void awe::map::setArmyTagCO(const awe::ArmyID army,
	const std::shared_ptr<const awe::commander>& tag) {
	setArmyCOs(army, getArmyCurrentCO(army), tag);
}

void awe::map::setArmyTagCO(const awe::ArmyID army, const std::string& tag) {
	if (tag.empty()) {
		setArmyCOs(army, getArmyCurrentCO(army), nullptr);
	} else {
		setArmyCOs(army, getArmyCurrentCO(army), _commanders->operator[](tag));
	}
}

void awe::map::tagArmyCOs(const awe::ArmyID army) {
	if (!_isArmyPresent(army)) {
		_logger.error("tagCOs operation failed: army with ID {} didn't exist at "
			"the time of calling!", army);
	} else {
		if (_armies.at(army).getTagCO()) {
			_armies.at(army).tagCOs();
			_changed = true;
		} else {
			_logger.error("tagCOs operation failed: army with ID {} didn't have a "
				"secondary CO at the time of calling!", army);
		}
	}
}

std::shared_ptr<const awe::commander> awe::map::getArmyCurrentCO(
	const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getCurrentCO();
	_logger.error("getCurrentCO operation failed: army with ID {} didn't exist at "
		"the time of calling!", army);
	return nullptr;
}

std::string awe::map::getArmyCurrentCOScriptName(const awe::ArmyID army) const {
	auto co = getArmyCurrentCO(army);
	if (co) {
		return co->getScriptName();
	} else {
		return "";
	}
}

std::shared_ptr<const awe::commander> awe::map::getArmyTagCO(
	const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCO();
	_logger.error("getTagCO operation failed: army with ID {} didn't exist at the "
		"time of calling!", army);
	return nullptr;
}

std::string awe::map::getArmyTagCOScriptName(const awe::ArmyID army) const {
	auto co = getArmyTagCO(army);
	if (co) {
		return co->getScriptName();
	} else {
		return "";
	}
}

bool awe::map::tagCOIsPresent(const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getTagCO().operator bool();
	_logger.error("tagCOIsPresent operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return false;
}

std::unordered_set<sf::Vector2u> awe::map::getTilesOfArmy(
	const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getTiles();
	_logger.error("getTilesOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<sf::Vector2u>();
}

CScriptArray* awe::map::getTilesOfArmyAsArray(const awe::ArmyID army) const {
	if (_scripts) {
		auto set = getTilesOfArmy(army);
		CScriptArray* ret = _scripts->createArray("Vector2");
		for (auto tile : set) {
			ret->InsertLast(&tile);
		}
		return ret;
	} else {
		throw NO_SCRIPTS;
	}
}

std::unordered_set<awe::UnitID> awe::map::getUnitsOfArmy(
	const awe::ArmyID army) const {
	if (_isArmyPresent(army)) return _armies.at(army).getUnits();
	_logger.error("getUnitsOfArmy operation failed: army with ID {} didn't exist "
		"at the time of calling!", army);
	return std::unordered_set<awe::UnitID>();
}

CScriptArray* awe::map::getUnitsOfArmyAsArray(const awe::ArmyID army) const {
	if (_scripts) {
		auto set = getUnitsOfArmy(army);
		CScriptArray* ret = _scripts->createArray("UnitID");
		for (auto unit : set) {
			ret->InsertLast(&unit);
		}
		return ret;
	} else {
		throw NO_SCRIPTS;
	}
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
		ret[_units.at(unit).getType()->getTurnStartPriority()].insert(unit);
	return ret;
}

CScriptArray* awe::map::getUnitsOfArmyByPriorityAsArray(
	const awe::ArmyID army) const {
	if (_scripts) {
		auto set = getUnitsOfArmyByPriority(army);
		CScriptArray* ret = _scripts->createArray("array<UnitID>@");
		// Loop through backwards: see documentation on unit_type::unit_type().
		for (auto itr = set.rbegin(), enditr = set.rend(); itr != enditr; ++itr) {
			CScriptArray* list = _scripts->createArray("UnitID");
			for (auto unit : itr->second) {
				list->InsertLast(&unit);
			}
			ret->InsertLast(&list);
			list->Release();
		}
		return ret;
	} else {
		throw NO_SCRIPTS;
	}
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
		const auto type = getTileType(tile);
		if (type) {
			const auto terrain = type->getType();
			if (terrain && terrain->getScriptName() == terrainType) ++counter;
		}
	}
	return counter;
}

awe::UnitID awe::map::createUnit(const std::shared_ptr<const awe::unit_type>& type,
	const awe::ArmyID army) {
	if (!type) _logger.warning("createUnit warning: creating a unit for army {} "
		"without a type!", army);
	if (!_isArmyPresent(army)) {
		_logger.error("createUnit operation failed: attempted to create \"{}\" "
			"for army with ID {} that didn't exist!",
			((type) ? (type->getName()) : ("[NULL]")), army);
		return 0;
	}
	awe::UnitID id;
	try {
		id = _findUnitID();
	} catch (const std::bad_alloc&) {
		_logger.critical("createUnit fatal error: could not generate a unique ID "
			"for a new unit. There are too many units allocated!");
		return 0;
	}
	_units.insert({ id, awe::unit({_logger.getData().sink, "unit"}, type, army,
		_sheet_unit, _sheet_icon) });
	_armies.at(army).addUnit(id);
	_changed = true;
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
	_updateCapturingUnit(id);
	// Firstly, remove the unit from the tile, if it was on a tile.
	// We don't need to check if the unit "is actually on the map or not," since
	// the tile will always hold the index to the unit in either case: which is why
	// we need the "actually" check to begin with.
	if (!_isOutOfBounds(_units.at(id).getPosition()))
		_tiles[_units.at(id).getPosition().x]
		      [_units.at(id).getPosition().y].setUnit(0);
	// Secondly, remove the unit from the army's list.
	if (_isArmyPresent(_units.at(id).getArmy())) {
		_armies.at(_units.at(id).getArmy()).removeUnit(id);
	} else {
		_logger.warning("deleteUnit warning: unit with ID {} didn't have a valid "
			"owning army ID, which was {}.", id, _units.at(id).getArmy());
	}
	// Thirdly, delete all units that are loaded onto this one.
	auto loaded = _units.at(id).loadedUnits();
	for (awe::UnitID unit : loaded) {
		deleteUnit(unit);
	}
	// Fourthly, if this unit was selected, deselect it if it's on top of the
	// stack. If it is further down the stack, then it will have to be removed
	// later: see popSelectedUnit().
	if (getSelectedUnit() == id) setSelectedUnit(0);
	// Fifthly, if this unit has a location override, remove it from the map.
	if (isPreviewUnit(id)) removePreviewUnit(id);
	// Finally, delete the unit from the main list.
	_units.erase(id);
	_changed = true;
}

std::shared_ptr<const awe::unit_type> awe::map::getUnitType(
	const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).getType();
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
	const auto idOfUnitOnTile = ((pos == awe::unit::NO_POSITION) ? (0) :
		(getUnitOnTile(pos)));
	if (idOfUnitOnTile == id) {
		// If the unit's position is being set to the tile it is on, then drop the
		// call.
		return;
	} else if (idOfUnitOnTile > 0) {
		_logger.error("setUnitPosition operation cancelled: attempted to move "
			"unit with ID {} to position {}, which is currently occupied by unit "
			"with ID {}!", id, pos, idOfUnitOnTile);
		return;
	}
	_updateCapturingUnit(id);
	// Make new tile occupied.
	if (pos != awe::unit::NO_POSITION) {
		_tiles[pos.x][pos.y].setUnit(id);
	}
	// Make old tile vacant.
	if (_units.at(id).isOnMap()) {
		const auto oldLocation = _units.at(id).getPosition();
		_tiles[oldLocation.x][oldLocation.y].setUnit(0);
	}
	// Assign new location to unit.
	_units.at(id).setPosition(pos);
	_changed = true;
}

sf::Vector2u awe::map::getUnitPosition(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("getUnitPosition operation failed: unit with ID {} doesn't "
			"exist!", id);
		return awe::unit::NO_POSITION;
	}
	return _units.at(id).getPosition();
}

bool awe::map::isUnitOnMap(const awe::UnitID id) const {
	if (!_isUnitPresent(id)) {
		_logger.error("isUnitOnMap operation failed: unit with ID {} doesn't "
			"exist!", id);
		return false;
	}
	return _units.at(id).isOnMap();
}

void awe::map::setUnitHP(const awe::UnitID id, const awe::HP hp) {
	if (_isUnitPresent(id)) {
		_units.at(id).setHP(hp);
		_changed = true;
	} else {
		_logger.error("setUnitHP operation cancelled: attempted to assign HP {} "
			"to unit with ID {}, which doesn't exist!", hp, id);
	}
}

awe::HP awe::map::getUnitHP(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).getHP();
	_logger.error("getUnitHP operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

awe::HP awe::map::getUnitDisplayedHP(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).getDisplayedHP();
	_logger.error("getUnitDisplayedHP operation failed: unit with ID {} doesn't "
		"exist!", id);
	return 0;
}

void awe::map::setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) {
	if (_isUnitPresent(id)) {
		_units.at(id).setFuel(fuel);
		_changed = true;
	} else {
		_logger.error("setUnitFuel operation cancelled: attempted to assign fuel "
			"{} to unit with ID {}, which doesn't exist!", fuel, id);
	}
}

void awe::map::burnUnitFuel(const awe::UnitID id, const awe::Fuel fuel) {
	if (_isUnitPresent(id)) {
		setUnitFuel(id, getUnitFuel(id) - fuel);
		_changed = true;
	} else {
		_logger.error("burnUnitFuel operation cancelled: attempted to offset unit "
			"{}'s fuel by {}. This unit doesn't exist!", id, fuel);
	}
}

awe::Fuel awe::map::getUnitFuel(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).getFuel();
	_logger.error("getUnitFuel operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

void awe::map::setUnitAmmo(const awe::UnitID id, const std::string& weapon,
		const awe::Ammo ammo) {
	if (_isUnitPresent(id)) {
		_units.at(id).setAmmo(weapon, ammo);
		_changed = true;
	} else {
		_logger.error("setUnitAmmo operation cancelled: attempted to assign ammo "
			"{} to unit with ID {}'s weapon \"{}\". This unit doesn't exist!",
			ammo, id, weapon);
	}
}

awe::Ammo awe::map::getUnitAmmo(const awe::UnitID id,
	const std::string& weapon) const {
	if (_isUnitPresent(id)) return _units.at(id).getAmmo(weapon);
	_logger.error("getUnitAmmo operation with weapon \"{}\" failed: unit with ID "
		"{} doesn't exist!", weapon, id);
	return 0;
}

void awe::map::replenishUnit(const awe::UnitID id, const bool heal) {
	if (_isUnitPresent(id)) {
		_units.at(id).replenish(heal);
		_changed = true;
	} else {
		_logger.error("replenishUnit operation cancelled: attempted to replenish "
			"{}unit with ID {}. This unit doesn't exist!", heal ? "and heal " : "",
			id);
	}
}

void awe::map::waitUnit(const awe::UnitID id, const bool waiting) {
	if (_isUnitPresent(id)) {
		_units.at(id).wait(waiting);
		_changed = true;
	} else {
		_logger.error("waitUnit operation cancelled: attempted to assign waiting "
			"state {} to unit with ID {}, which doesn't exist!", waiting, id);
	}
}

bool awe::map::isUnitWaiting(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).isWaiting();
	_logger.error("isUnitWaiting operation failed: unit with ID {} doesn't exist!",
		id);
	return false;
}

void awe::map::unitCapturing(const awe::UnitID id, const bool capturing) {
	if (_isUnitPresent(id)) {
		_units.at(id).capturing(capturing);
		_changed = true;
	} else {
		_logger.error("unitCapturing operation cancelled: attempted to assign "
			"capturing state {} to unit with ID {}, which doesn't exist!",
			capturing, id);
	}
}

bool awe::map::isUnitCapturing(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).isCapturing();
	_logger.error("isUnitCapturing operation failed: unit with ID {} doesn't "
		"exist!", id);
	return false;
}

void awe::map::unitHiding(const awe::UnitID id, const bool hiding) {
	if (_isUnitPresent(id)) {
		_units.at(id).hiding(hiding);
		_changed = true;
	} else {
		_logger.error("unitHiding operation cancelled: attempted to assign hiding "
			"state {} to unit with ID {}, which doesn't exist!", hiding, id);
	}
}

bool awe::map::isUnitHiding(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).isHiding();
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
	// 0. It is on the map.
	if (!isUnitOnMap(unit)) return false;
	// 1. It isn't hiding.
	if (!isUnitHiding(unit)) return true;
	// 2. It is hiding, but it belongs to the same team as the given army.
	const auto armyTeam = getArmyTeam(army);
	if (getTeamOfUnit(unit) == armyTeam) return true;
	// 3. It is hiding, but it is located on a tile that belongs to `army`'s team.
	const auto unitPos = getUnitPosition(unit);
	const auto tileOwner = getTileOwner(unitPos);
	if (tileOwner != awe::NO_ARMY && getArmyTeam(tileOwner) == armyTeam)
		return true;
	// 4. It is hiding, but it is adjacent to a unit that belongs to the same team
	//    as `army`.
	const auto adjacentTiles = getAvailableTiles(unitPos, 1, 1);
	for (const auto tile : adjacentTiles) {
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
	if (_units.at(load).loadedOnto()) {
		_logger.warning("loadUnit warning: unit with ID {} was already loaded "
			"onto unit with ID {}", load, onto);
		return;
	}
	_updateCapturingUnit(load);
	// Make the tile that `load` was on vacant, and remove the unit ID from the
	// tile.
	if (_units.at(load).isOnMap()) {
		const auto location = _units.at(load).getPosition();
		_tiles[location.x][location.y].setUnit(0);
	}
	_units.at(load).setPosition(awe::unit::NO_POSITION);
	// Perform loads.
	_units.at(onto).loadUnit(load);
	_units.at(load).loadOnto(onto);
	_changed = true;
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
	if (auto u = getUnitOnTile(onto)) {
		if (u) {
			_logger.error("unloadUnit operation cancelled: attempted to unload "
				"unit with ID {} from unit with ID {}, to position {}, which has "
				"a unit with ID {} already occupying it!", unload, from, onto, u);
		} else {
			_logger.error("unloadUnit operation cancelled: attempted to unload "
				"unit with ID {} from unit with ID {}, to position {}, which "
				"is out of bounds with the map's size of {}!", unload, from, onto,
				getMapSize());
		}
		return;
	}
	if (_units.at(from).unloadUnit(unload)) {
		// Unload successful, continue with operation.
		_units.at(unload).loadOnto(0);
		setUnitPosition(unload, onto);
	} else {
		_logger.error("unloadUnit operation failed: unit with ID {} was not "
			"loaded onto unit with ID {}", unload, from);
	}
	_changed = true;
}

awe::UnitID awe::map::getUnitWhichContainsUnit(const awe::UnitID unit) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("getUnitWhichContainsUnit operation failed: unit with ID {} "
			"does not exist!", unit);
		return 0;
	}
	return _units.at(unit).loadedOnto();
}

bool awe::map::isUnitLoadedOntoUnit(const awe::UnitID unit,
	const awe::UnitID on) const {
	if (!_isUnitPresent(unit)) {
		_logger.error("isUnitLoadedOntoUnit operation failed: unit with ID {} "
			"does not exist!", unit);
		return 0;
	}
	if (!_isUnitPresent(on)) {
		_logger.error("isUnitLoadedOntoUnit operation failed: unit with ID {} "
			"does not exist!", on);
		return 0;
	}
	const auto units = _units.at(on).loadedUnits();
	return units.find(unit) != units.end();
}

awe::ArmyID awe::map::getArmyOfUnit(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).getArmy();
	_logger.error("getArmyOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return awe::NO_ARMY;
}

awe::TeamID awe::map::getTeamOfUnit(const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _armies.at(_units.at(id).getArmy()).getTeam();
	_logger.error("getTeamOfUnit operation failed: unit with ID {} doesn't exist!",
		id);
	return 0;
}

std::unordered_set<awe::UnitID> awe::map::getLoadedUnits(
	const awe::UnitID id) const {
	if (_isUnitPresent(id)) return _units.at(id).loadedUnits();
	_logger.error("getLoadedUnits operation failed: unit with ID {} doesn't "
		"exist!", id);
	return {};
}


CScriptArray* awe::map::getLoadedUnitsAsArray(const awe::UnitID id) const {
	auto set = getLoadedUnits(id);
	CScriptArray* ret = _scripts->createArray("UnitID");
	for (auto unit : set) {
		ret->InsertLast(&unit);
	}
	return ret;
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

bool awe::map::setTileType(const sf::Vector2u& pos,
	const std::shared_ptr<const awe::tile_type>& type) {
	if (!type) _logger.warning("setTileType warning: assigning the tile at "
		"position {} an empty type!", pos);
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileType operation cancelled: attempted to assign type "
			"\"{}\" to tile at position {}, which is out of bounds with the map's "
			"size of {}!", ((type) ? (type->getType()->getName()) : ("[NULL]")),
			pos, getMapSize());
		return false;
	}
	_updateCapturingUnit(getUnitOnTile(pos));
	_tiles[pos.x][pos.y].setTileType(type);
	// Remove ownership of the tile from the army who owns it, if any army does.
	setTileOwner(pos, awe::NO_ARMY);
	_changed = true;
	return true;
}

bool awe::map::setTileType(const sf::Vector2u& pos, const std::string& type) {
	return setTileType(pos, _tileTypes->operator[](type));
}

std::shared_ptr<const awe::tile_type> awe::map::getTileType(
	const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileType operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return nullptr;
	}
	return _tiles[pos.x][pos.y].getTileType();
}

const awe::tile_type* awe::map::getTileTypeObject(const sf::Vector2u& pos) const {
	auto ret = getTileType(pos);
	if (ret) {
		return ret.get();
	} else {
		throw std::out_of_range("This tile does not exist!");
	}
}

void awe::map::setTileHP(const sf::Vector2u& pos, const awe::HP hp) {
	if (!_isOutOfBounds(pos)) {
		_tiles[pos.x][pos.y].setTileHP(hp);
		_changed = true;
	} else {
		_logger.error("setTileHP operation cancelled: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
	}
}

awe::HP awe::map::getTileHP(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileHP operation failed: tile at position {} is out of "
			"bounds with the map's size of {}!", pos, getMapSize());
		return 0;
	}
	return _tiles[pos.x][pos.y].getTileHP();
}

void awe::map::setTileOwner(const sf::Vector2u& pos, awe::ArmyID army) {
	if (_isOutOfBounds(pos)) {
		_logger.error("setTileOwner operation cancelled: army with ID {} couldn't "
			"be assigned to tile at position {}, as it is out of bounds with the "
			"map's size of {}!", army, pos, getMapSize());
		return;
	}
	_updateCapturingUnit(getUnitOnTile(pos));
	auto& tile = _tiles[pos.x][pos.y];
	// First, remove the tile from the army who currently owns it.
	if (_isArmyPresent(tile.getTileOwner()))
		_armies.at(tile.getTileOwner()).removeTile(pos);
	// Now assign it to the real owner, if any.
	if (_isArmyPresent(army)) _armies.at(army).addTile(pos);
	// Update the actual tile now.
	tile.setTileOwner(army);
	_changed = true;
}

awe::ArmyID awe::map::getTileOwner(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getTileOwner operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return awe::NO_ARMY;
	}
	return _tiles[pos.x][pos.y].getTileOwner();
}

awe::UnitID awe::map::getUnitOnTile(const sf::Vector2u& pos) const {
	if (_isOutOfBounds(pos)) {
		_logger.error("getUnitOnTile operation failed: tile at position {} is out "
			"of bounds with the map's size of {}!", pos, getMapSize());
		return 0;
	}
	auto u = _tiles[pos.x][pos.y].getUnit();
	if (u != 0 && _units.at(u).isOnMap()) return u;
	return 0;
}

std::unordered_set<sf::Vector2u> awe::map::getAvailableTiles(
	const sf::Vector2u& tile, unsigned int startFrom,
	const unsigned int endAt) const {
	// Checking.
	const sf::Vector2u mapSize = getMapSize();
	if (_isOutOfBounds(tile)) {
		_logger.error("getAvailableTiles operation failed: tile at position {} is "
			"out of bounds with the map's size of {}!", tile, mapSize);
		return {};
	}
	if (startFrom == 0) startFrom = 1;
	if (startFrom > endAt) return {};

	std::unordered_set<sf::Vector2u> tiles;
	// Get highest tile in range and add it to the list for consideration.
	unsigned int widthOfLine = 1;
	sf::Vector2<int64_t> highest = {(int64_t)tile.x,
		(int64_t)tile.y - (int64_t)endAt};
	if (highest.y < 0) {
		widthOfLine += (unsigned int)(2 * abs(highest.y + 1) + 1);
		highest.y = 0;
	}
	// Now go down the map and add tiles as appropriate, until we either hit the
	// bottom of the map or the end of the given range.
	while (true) {
		int leftMostX = (int)tile.x - (int)(widthOfLine / 2),
			rightMostX = (int)tile.x + (int)(widthOfLine / 2);
		for (int64_t x = (leftMostX < 0 ? 0 : leftMostX);
			x <= rightMostX && x < mapSize.x; ++x) {
			sf::Vector2u newTile = { (unsigned int)x, (unsigned int)highest.y };
			if (distance(newTile, tile) >= startFrom) tiles.insert(newTile);
		}
		++highest.y;
		if (highest.y > tile.y + endAt || highest.y >= mapSize.y) break;
		if (highest.y <= tile.y)
			widthOfLine += 2;
		else
			widthOfLine -= 2;
	}
	return tiles;
}

CScriptArray* awe::map::getAvailableTilesAsArray(
	const sf::Vector2u& tile, unsigned int startFrom,
	const unsigned int endAt) const {
	auto set = getAvailableTiles(tile, startFrom, endAt);
	CScriptArray* ret = _scripts->createArray("Vector2");
	for (auto tile : set) {
		ret->InsertLast(&tile);
	}
	return ret;
}

std::vector<awe::closed_list_node> awe::map::findPath(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const unsigned int* const movePoints, const awe::Fuel* const fuel,
	const awe::TeamID* const team, const awe::ArmyID* const army,
	const std::unordered_set<awe::UnitID>& ignoredUnits) const {
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
				{ {currentTile, gScore[currentTile]} };
			while (cameFrom.find(currentTile) != cameFrom.end()) {
				currentTile = cameFrom[currentTile];
				ret.insert(ret.begin(), { currentTile, gScore[currentTile] });
			}
			return ret;
		}

		openSet.erase(currentTile);
		auto adjacentTiles = getAvailableTiles(currentTile, 1, 1);
		for (auto& adjacentTile : adjacentTiles) {
			// Get the movement cost for this terrain.
			const auto moveCost = getTileType(adjacentTile)->getType()->
				getMoveCost(moveType.getScriptName());

			// If this unit cannot traverse the terrain, do not add it to any set.
			if (moveCost < 0) continue;

			int tentativeGScore = gScore[currentTile] + moveCost;

			// If:
			// 1. The unit does not have enough fuel.
			// 2. The unit has ran out of movement points.
			// 3. The tile has a unit belonging to an opposing team that isn't
			//    ignored or invisible/hidden.
			// then it cannot traverse the tile, so don't add it to the open set.
			const auto unitOnAdjacentTile = getUnitOnTile(adjacentTile);
			const auto fuelCheck = fuel == nullptr || tentativeGScore <= *fuel;
			const auto mpCheck = movePoints == nullptr ||
				static_cast<unsigned int>(tentativeGScore) <= *movePoints;
			const auto unitCheck = !_isUnitPresent(unitOnAdjacentTile) ||
				(ignoredUnits.find(unitOnAdjacentTile) != ignoredUnits.end()) ||
				(army == nullptr || !isUnitVisible(unitOnAdjacentTile, *army)) ||
				(team != nullptr && getTeamOfUnit(unitOnAdjacentTile) == *team);
			if (fuelCheck && mpCheck && unitCheck) {
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

/**
 * Converts a \c CScriptArray into an unordered set of unit IDs.
 * @warning This function will release the array if the pointer is not \c nullptr!
 * @param   ignoredUnits Pointer to the array.
 * @return  The set. Empty if \c ignoredUnits is \c nullptr.
 */
static std::unordered_set<awe::UnitID> convertCScriptArrayIntoSet(
	CScriptArray* ignoredUnits) {
	std::unordered_set<awe::UnitID> ret;
	if (ignoredUnits) {
		for (asUINT i = 0; i < ignoredUnits->GetSize(); ++i) {
			ret.insert(*(awe::UnitID*)ignoredUnits->At(i));
		}
		ignoredUnits->Release();
	}
	return ret;
}

CScriptArray* awe::map::findPathAsArray(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const unsigned int movePoints, const awe::Fuel fuel,
	const awe::TeamID team, const awe::ArmyID army,
	CScriptArray* ignoredUnits) const {
	const auto vec = findPath(origin, dest, moveType, &movePoints, &fuel, &team,
		&army, convertCScriptArrayIntoSet(ignoredUnits));
	CScriptArray* ret = _scripts->createArray("ClosedListNode");
	for (auto& node : vec) {
		ret->InsertLast(&awe::closed_list_node());
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->tile = node.tile;
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->g = node.g;
	}
	return ret;
}

CScriptArray* awe::map::findPathAsArrayUnloadUnit(const sf::Vector2u& origin,
	const sf::Vector2u& dest, const awe::movement_type& moveType,
	const awe::ArmyID army, CScriptArray* ignoredUnits) const {
	const auto vec = findPath(origin, dest, moveType, nullptr, nullptr, nullptr,
		&army, convertCScriptArrayIntoSet(ignoredUnits));
	CScriptArray* ret = _scripts->createArray("ClosedListNode");
	for (auto& node : vec) {
		ret->InsertLast(&awe::closed_list_node());
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->tile = node.tile;
		((awe::closed_list_node*)ret->At(ret->GetSize() - 1))->g = node.g;
	}
	return ret;
}

int awe::map::scanPath(CScriptArray* path, const awe::UnitID unit,
	std::size_t ignores) const {
	if (path && _isUnitPresent(unit)) {
		asUINT len = path->GetSize();
		const auto armyID = getArmyOfUnit(unit);
		for (asUINT i = 0; i < len; ++i) {
			const auto unitID =
				getUnitOnTile(((awe::closed_list_node*)path->At(i))->tile);
			if (_isUnitPresent(unitID)) {
				if (!isUnitVisible(unitID, armyID)) {
					if (ignores == 0) {
						if (path) path->Release();
						return static_cast<int>(i);
					} else {
						--ignores;
					}
				}
			}
		}
	} else {
		_logger.error("scanPath operation failed: unit with ID {} exist{}",
			((_isUnitPresent(unit)) ? ("does") : ("does not")),
			((path) ? (", path is not NULL.") : (", path is NULL.")));
	}
	if (path) path->Release();
	return -1;
}

bool awe::map::setSelectedUnit(const awe::UnitID unit) {
	if (unit == 0) {
		_selectedUnitRenderData.top().selectedUnit = 0;
		_selectedUnitRenderData.top().clearState();
		return true;
	}
	if (_isUnitPresent(unit)) {
		// if (_units.at(unit).isOnMap()) {
			_selectedUnitRenderData.top().selectedUnit = unit;
			return true;
		// } else {
		//	_logger.error("setSelectedUnit operation failed: cannot select unit "
		//		"with ID {} as it is not on the map.", unit);
		// }
	} else {
		_logger.error("setSelectedUnit operation failed: unit with ID {} doesn't "
			"exist!", unit);
	}
	return false;
}

bool awe::map::pushSelectedUnit(const awe::UnitID unit) {
	_selectedUnitRenderData.emplace(*_scripts);
	const auto ret = setSelectedUnit(unit);
	if (!ret) {
		_selectedUnitRenderData.pop();
		_logger.error("pushSelectedUnit operation failed: see above.");
	}
	return ret;
}

void awe::map::popSelectedUnit() {
	if (_selectedUnitRenderData.size() > 1) {
		_selectedUnitRenderData.pop();
		// At some point, the previously selected unit might have been deleted, and
		// if this is the case, we need to deselect it.
		if (!_isUnitPresent(_selectedUnitRenderData.top().selectedUnit) &&
			_selectedUnitRenderData.top().selectedUnit != 0) {
			_logger.warning("popSelectUnit operation: newly selected unit with ID "
				"{} is now no longer present: the selected unit render data state "
				"will now be cleared!",
				_selectedUnitRenderData.top().selectedUnit);
			setSelectedUnit(0);
		}
	} else {
		_logger.error("popSelectUnit operation failed: the size of the stack was "
			"{}, which is too low!", _selectedUnitRenderData.size());
	}
}

awe::UnitID awe::map::getSelectedUnit() const {
	return _selectedUnitRenderData.top().selectedUnit;
}

void awe::map::addAvailableTile(const sf::Vector2u& tile) {
	if (_isOutOfBounds(tile)) {
		_logger.error("addAvailableTile operation failed: tile {} is out of "
			"bounds!", tile);
	} else {
		if (_selectedUnitRenderData.top().availableTiles.find(tile) ==
			_selectedUnitRenderData.top().availableTiles.end()) {
			_selectedUnitRenderData.top().availableTiles.insert(tile);
		}
	}
}

bool awe::map::isAvailableTile(const sf::Vector2u& tile) const {
	if (_isOutOfBounds(tile)) {
		_logger.error("isAvailableTile operation failed: tile {} is out of "
			"bounds!", tile);
		return false;
	} else {
		return _selectedUnitRenderData.top().availableTiles.find(tile) !=
			_selectedUnitRenderData.top().availableTiles.end();
	}
}

void awe::map::clearAvailableTiles() {
	_selectedUnitRenderData.top().availableTiles.clear();
}

void awe::map::setAvailableTileShader(const awe::available_tile_shader shader) {
	_selectedUnitRenderData.top().availableTileShader = shader;
}

awe::available_tile_shader awe::map::getAvailableTileShader() const {
	return _selectedUnitRenderData.top().availableTileShader;
}

CScriptArray* awe::map::getClosedList() {
	return _selectedUnitRenderData.top().closedList;
}

void awe::map::disableSelectedUnitRenderingEffects(const bool val) {
	_selectedUnitRenderData.top().disableRenderingEffects = val;
}

void awe::map::disableShaderForAvailableUnits(const bool val) {
	_selectedUnitRenderData.top().disableShaderForAvailableUnits = val;
}

void awe::map::regenerateClosedListSprites() {
	// Starting from the beginning; calculate the arrow sprites to draw for
	// each tile.
	CScriptArray* list = _selectedUnitRenderData.top().closedList;
	for (asUINT i = 0, length = list->GetSize(); i < length; ++i) {
		awe::closed_list_node* const current =
			(awe::closed_list_node* const)list->At(i);
		current->sprite.setSpritesheet(_sheet_icon);
		if (i == 0) {
			current->sprite.setSpritesheet(nullptr);
		} else if (i == length - 1) {
			awe::closed_list_node* const prev =
				(awe::closed_list_node* const)list->At(i - 1);
			if (prev->tile.x < current->tile.x) {
				current->sprite.setSprite("unitArrowRight");
			} else if (prev->tile.x > current->tile.x) {
				current->sprite.setSprite("unitArrowLeft");
			} else if (prev->tile.y < current->tile.y) {
				current->sprite.setSprite("unitArrowDown");
			} else if (prev->tile.y > current->tile.y) {
				current->sprite.setSprite("unitArrowUp");
			}
		} else {
			awe::closed_list_node* const prev =
				(awe::closed_list_node* const)list->At(i - 1);
			awe::closed_list_node* const next =
				(awe::closed_list_node* const)list->At(i + 1);
			if ((prev->tile.x < current->tile.x &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowHori");
			} else if ((prev->tile.y < current->tile.y &&
				current->tile.y < next->tile.y) ||
				(prev->tile.y > current->tile.y &&
					current->tile.y > next->tile.y)) {
				current->sprite.setSprite("unitArrowVert");
			} else if ((prev->tile.y < current->tile.y &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.y > next->tile.y)) {
				current->sprite.setSprite("unitArrowNE");
			} else if ((prev->tile.y > current->tile.y &&
				current->tile.x < next->tile.x) ||
				(prev->tile.x > current->tile.x &&
					current->tile.y < next->tile.y)) {
				current->sprite.setSprite("unitArrowSE");
			} else if ((prev->tile.x < current->tile.x &&
				current->tile.y > next->tile.y) ||
				(prev->tile.y < current->tile.y &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowNW");
			} else if ((prev->tile.x < current->tile.x &&
				current->tile.y < next->tile.y) ||
				(prev->tile.y > current->tile.y &&
					current->tile.x > next->tile.x)) {
				current->sprite.setSprite("unitArrowSW");
			}
		}
	}
}

void awe::map::addPreviewUnit(const awe::UnitID unit, const sf::Vector2u& pos) {
	if (!_isUnitPresent(unit)) {
		_logger.error("addPreviewUnit operation failed: unit with ID {} does not "
			"exist.", unit);
		return;
	}
	if (_isOutOfBounds(pos)) {
		_logger.error("addPreviewUnit operation failed: tile {} is out of bounds.",
			pos);
		return;
	}
	_unitLocationOverrides[unit] = pos;
}

void awe::map::removePreviewUnit(const awe::UnitID unit) {
	if (_unitLocationOverrides.find(unit) == _unitLocationOverrides.end()) {
		_logger.error("removePreviewUnit operation failed: unit with ID {} did "
			"not have a position override at the time of calling.", unit);
	} else {
		_unitLocationOverrides.erase(unit);
	}
}

void awe::map::removeAllPreviewUnits() {
	_unitLocationOverrides.clear();
}

std::size_t awe::map::getUnitPreviewsCount() const {
	return _unitLocationOverrides.size();
}

void awe::map::setTarget(
	const std::shared_ptr<sf::RenderTarget>& target) noexcept {
	_target = target;
}

void awe::map::alwaysShowHiddenUnits(const bool alwaysShow) noexcept {
	_alwaysShowHiddenUnits = alwaysShow;
}

void awe::map::setSelectedTile(const sf::Vector2u& pos) {
	if (!_isOutOfBounds(pos)) _sel = pos;
}

void awe::map::moveSelectedTileUp() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y - 1));
}

void awe::map::moveSelectedTileDown() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x, getSelectedTile().y + 1));
}

void awe::map::moveSelectedTileLeft() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x - 1, getSelectedTile().y));
}

void awe::map::moveSelectedTileRight() {
	setSelectedTile(sf::Vector2u(getSelectedTile().x + 1, getSelectedTile().y));
}

void awe::map::setSelectedTileByPixel(const sf::Vector2i& pixel) {
	if (!_target) return;
	const auto coord = _target->mapPixelToCoords(pixel, _view);
	auto sel = getSelectedTile(), size = getMapSize();

	// Since each tile MUST be a set height, we can easily calculate the row.
	if (coord.y < 0.0f) {
		sel.y = 0;
	} else if (coord.y >= size.y * awe::tile::MIN_HEIGHT) {
		sel.y = size.y - 1;
	} else {
		sel.y = static_cast<sf::Uint32>(static_cast<float>(coord.y) /
			awe::tile::MIN_HEIGHT);
	}

	// However, since tiles can technically be any width (though they really
	// shouldn't be), we need to iterate through each column.
	sel.x = 0;
	if (coord.x >= 0.0f) {
		for (float xCounter = 0.0f; sel.x < size.x; ++sel.x) {
			auto tileWidth = _tiles[sel.x][sel.y].getPixelSize().x;
			if (tileWidth < static_cast<float>(awe::tile::MIN_WIDTH))
				tileWidth = static_cast<float>(awe::tile::MIN_WIDTH);
			if (coord.x < (xCounter += tileWidth)) break;
		}
		if (sel.x >= size.x) sel.x = size.x - 1;
	}

	setSelectedTile(sel);
}

void awe::map::setSelectedArmy(const awe::ArmyID army) {
	if (_isArmyPresent(army)) {
		_currentArmy = army;
		_changed = true;
	} else {
		_logger.error("setSelectedArmy operation cancelled: army with ID {} does "
			"not exist!", army);
	}
}

awe::ArmyID awe::map::getSelectedArmy() const {
	return _currentArmy;
}

awe::ArmyID awe::map::getNextArmy() const {
	if (_currentArmy == awe::NO_ARMY) return awe::NO_ARMY;
	auto itr = ++_armies.find(_currentArmy);
	if (itr == _armies.end()) itr = _armies.begin();
	return itr->first;
}

void awe::map::setMapScalingFactor(const float factor) {
	if (factor <= 0.0f) {
		_logger.error("setMapScalingFactor operation failed: attempted to assign "
			"a map scaling factor {} that was at or below 0.0.", factor);
	} else {
		_scaling = factor;
	}
}

bool awe::map::isCursorOnLeftSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).x < _target->getSize().x / 2.0f;
}

bool awe::map::isCursorOnTopSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).y < _target->getSize().y / 2.0f;
}

awe::quadrant awe::map::getCursorQuadrant() const {
	const bool isTop = isCursorOnTopSide();
	if (isCursorOnLeftSide()) {
		if (isTop) {
			return awe::quadrant::UpperLeft;
		} else {
			return awe::quadrant::LowerLeft;
		}
	} else {
		if (isTop) {
			return awe::quadrant::UpperRight;
		} else {
			return awe::quadrant::LowerRight;
		}
	}
}

void awe::map::setULCursorSprite(const std::string& sprite) {
	_ulCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setULCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setURCursorSprite(const std::string& sprite) {
	_urCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setURCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLLCursorSprite(const std::string& sprite) {
	_llCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLLCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

void awe::map::setLRCursorSprite(const std::string& sprite) {
	_lrCursorSprite = sprite;
	if (_sheet_icon && !_sheet_icon->doesSpriteExist(sprite)) {
		_logger.warning("setLRCursorSprite was just given sprite with ID \"{}\", "
			"which doesn't exist in the spritesheet!", sprite);
	}
}

sf::IntRect awe::map::getCursorBoundingBox() const {
	const auto pos = _cursor.getPosition(), size = _cursor.getSize();
	const auto ul = _target->mapCoordsToPixel(pos, _view);
	const auto guiScaling = _gui ? static_cast<int>(_gui->getScalingFactor()) : 1;
	return { ul / guiScaling,
		(_target->mapCoordsToPixel(pos + size, _view) - ul) / guiScaling };
}

sf::IntRect awe::map::getMapBoundingBox() const {
	// Map is always drawn at { 0, 0 } before the view is applied.
	auto mapSize = sf::Vector2f(getMapSize());
	mapSize.x *= static_cast<float>(awe::tile::MIN_WIDTH);
	mapSize.y *= static_cast<float>(awe::tile::MIN_HEIGHT);
	const auto ul = _target->mapCoordsToPixel({ 0.0f, 0.0f }, _view);
	return { ul, _target->mapCoordsToPixel(mapSize, _view) - ul };
}

void awe::map::setTileSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_tile = sheet;
	// Go through all of the tiles and set the new spritesheet to each one.
	for (auto& column : _tiles) {
		for (auto& tile : column) {
			tile.setSpritesheet(sheet);
		}
	}
}

void awe::map::setUnitSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_unit = sheet;
	// Go through all of the units and set the new icon spritesheet to each one.
	for (auto& unit : _units) unit.second.setSpritesheet(sheet);
}

void awe::map::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_icon = sheet;
	_cursor.setSpritesheet(sheet);
	// Go through all of the units and set the new spritesheet to each one.
	for (auto& unit : _units) unit.second.setIconSpritesheet(sheet);
}

void awe::map::setCOSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_co = sheet;
}

void awe::map::setFont(const std::shared_ptr<sf::Font>& font) {
	if (!font) {
		_logger.error("setFont operation failed: nullptr was given!");
		return;
	}
}

void awe::map::setGUI(const std::shared_ptr<sfx::gui>& gui) {
	if (!gui) {
		_logger.error("setGUI operation failed: nullptr was given!");
		return;
	}
	_gui = gui;
}

void awe::map::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) {
	if (!dict) {
		_logger.error("setLanguageDictionary operation failed: nullptr was "
			"given!");
		return;
	}
}

bool awe::map::animate(const sf::RenderTarget& target) {
	// Create map of tiles -> units from _unitLocationOverrides.
	std::unordered_map<sf::Vector2u, awe::UnitID> unitLocationOverrides;
	for (auto pair : _unitLocationOverrides)
		unitLocationOverrides[pair.second] = pair.first;

	// Step 1. the tiles.
	// Also update the position of the cursor here!
	const auto mapSize = getMapSize(), selectedTile = getSelectedTile();
	float tiley = 0.0f;
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		float tilex = 0.0f;
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			auto& tile = _tiles[x][y];
			tile.animate(target);

			sf::Vector2u tilePos = sf::Vector2u(x, y);
			sf::Uint32 tileWidth = 0, tileHeight = 0;
			auto type = tile.getTileType();
			if (type) {
				tileWidth = (sf::Uint32)tile.getPixelSize().x;
				tileHeight = (sf::Uint32)tile.getPixelSize().y;
				/*if (tile.getTileOwner() == awe::NO_ARMY) {
					tileWidth = (sf::Uint32)_sheet_tile->getFrameRect(
						type->getNeutralTile()
					).width;
					tileHeight = (sf::Uint32)_sheet_tile->accessSprite(
						type->getNeutralTile()
					).height;
				} else {
					tileWidth = (sf::Uint32)_sheet_tile->accessSprite(
						type->getOwnedTile(tile.getTileOwner())
					).width;
					tileHeight = (sf::Uint32)_sheet_tile->accessSprite(
						type->getOwnedTile(tile.getTileOwner())
					).height;
				}*/
			}
			if (tileWidth < tile.MIN_WIDTH) tileWidth = tile.MIN_WIDTH;
			if (tileHeight < tile.MIN_HEIGHT) tileHeight = tile.MIN_HEIGHT;
			tile.setPixelPosition(tilex, tiley -
				static_cast<float>((tileHeight - tile.MIN_HEIGHT)));

			// Update the tile's unit's pixel position accordingly, if it
			// doesn't have an override.
			const auto tilesUnit = tile.getUnit();
			if (tilesUnit && _unitLocationOverrides.find(tilesUnit) ==
				_unitLocationOverrides.end()) {
				_units.at(tilesUnit).setPixelPosition(tilex, tiley);
			}

			// Check if this tile has a unit's location overridded onto it.
			if (unitLocationOverrides.find(tilePos) !=
				unitLocationOverrides.end()) {
				_units.at(unitLocationOverrides[tilePos]).setPixelPosition(
					tilex, tiley
				);
			}

			// Update cursor position.
			if (selectedTile == tilePos)
				_cursor.setPosition(sf::Vector2f(tilex, tiley));

			tilex += static_cast<float>(tileWidth);
		}
		tiley += static_cast<float>(awe::tile::MIN_HEIGHT);
	}

	// Step 2. the selected unit closed list tile icons.
	if (_selectedUnitRenderData.top().selectedUnit > 0) {
		for (asUINT i = 0, size = _selectedUnitRenderData.top().closedList->
			GetSize(); i < size; ++i) {
			awe::closed_list_node* pathNode = (awe::closed_list_node*)
				_selectedUnitRenderData.top().closedList->At(i);
			pathNode->sprite.animate(target);
			auto pos =
				_tiles[pathNode->tile.x][pathNode->tile.y].getPixelPosition();
			const auto h =
				_tiles[pathNode->tile.x][pathNode->tile.y].getPixelSize().y;
			if (h > awe::tile::MIN_HEIGHT) pos.y += h - awe::tile::MIN_HEIGHT;
			pathNode->sprite.setPosition(pos);
		}
	}

	// Step 3. the units.
	// Note that unit positioning was carried out in step 1.
	for (auto& unit : _units) unit.second.animate(target);

	// Step 4. the cursor.
	const auto quadrant = getCursorQuadrant();
	switch (quadrant) {
	case awe::quadrant::LowerLeft:
		_cursor.setSprite(_llCursorSprite);
		break;
	case awe::quadrant::LowerRight:
		_cursor.setSprite(_lrCursorSprite);
		break;
	case awe::quadrant::UpperRight:
		_cursor.setSprite(_urCursorSprite);
		break;
	default: // Let awe::quadrant::UpperLeft be the default.
		_cursor.setSprite(_ulCursorSprite);
	}
	_cursor.animate(target);

	// Step 5. update the view to match the target's size, and apply the scaling.
	// Additionally, update the view offset.
	auto mapPixelSize = mapSize; // Ignore fancy tile heights and widths.
	mapPixelSize.x *= awe::tile::MIN_WIDTH;
	mapPixelSize.y *= awe::tile::MIN_HEIGHT;
	const auto rect = sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(target.getSize().x) / _scaling,
		static_cast<float>(target.getSize().y) / _scaling);
	const auto cursorRect = sf::FloatRect(
		sf::Vector2f(_target->mapCoordsToPixel(_cursor.getPosition(), _view)),
		_cursor.getSize() * _scaling);
	_view.reset(rect);
	_view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	static const auto moveOffsetAxis = [](const float viewSize,
		const float mapPixelSize, std::optional<float>& viewOffset,
		const float cursorPos, const float cursorSize, const float padding,
		const float screenSize) {
		if (viewSize > mapPixelSize) {
			// Map appears smaller than the screen along this axis, so centre it on
			// that axis. Also, reset the view offset to ensure that when the map
			// appears larger in the future, the view will centre itself on the
			// cursor along this axis.
			viewOffset.reset();
			return -(viewSize / 2.0f - mapPixelSize / 2.0f);
		} else {
			if (viewOffset) {
				// If the cursor is too close to either edge of the screen, adjust
				// the offset to bring the cursor a little further into the centre
				// of the screen.
				if (cursorPos <= padding * 2.0f) {
					*viewOffset -= padding;
				} else if (cursorPos + cursorSize >= screenSize - padding * 2.0f) {
					*viewOffset += padding;
				}
			} else {
				// Map appears larger immediately after appearing smaller. Default
				// offset to centre on the cursor.
				viewOffset = (cursorPos + cursorSize / 2.0f) - viewSize / 2.0f;
			}
			// Make sure the map fully fills the screen along this axis.
			if (*viewOffset < 0.0f) {
				viewOffset = 0.0f;
			} else if (*viewOffset + viewSize >= mapPixelSize) {
				viewOffset = mapPixelSize - viewSize;
			}
			return *viewOffset;
		}
	};
	_view.move(
		moveOffsetAxis(rect.width, static_cast<float>(mapPixelSize.x),
			_viewOffsetX, cursorRect.left, cursorRect.width,
			static_cast<float>(awe::tile::MIN_WIDTH),
			static_cast<float>(target.getSize().x)),
		moveOffsetAxis(rect.height, static_cast<float>(mapPixelSize.y),
			_viewOffsetY, cursorRect.top, cursorRect.height,
			static_cast<float>(awe::tile::MIN_HEIGHT),
			static_cast<float>(target.getSize().y))
	);

	// End.
	return false;
}

void awe::map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Step 0. temporarily apply our view.
	const auto oldView = target.getView();
	target.setView(_view);

	// Step 1. the tiles.
	auto mapSize = getMapSize();
	for (sf::Uint32 y = 0; y < mapSize.y; ++y) {
		for (sf::Uint32 x = 0; x < mapSize.x; ++x) {
			if (_selectedUnitRenderData.top().selectedUnit > 0 &&
				!_selectedUnitRenderData.top().disableRenderingEffects) {
				sf::Vector2u currentTile(x, y);
				sf::RenderStates tileStates = states;
				if (_selectedUnitRenderData.top().availableTiles.find(currentTile)
					!= _selectedUnitRenderData.top().availableTiles.end()) {
					// Apply configured shading.
					switch (_selectedUnitRenderData.top().availableTileShader) {
					case awe::available_tile_shader::Yellow:
						tileStates.shader = &_availableTileShader;
						break;
					case awe::available_tile_shader::Red:
						tileStates.shader = &_attackableTileShader;
						break;
					}
				} else { // Not an available tile. Grey out.
					tileStates.shader = &_unavailableTileShader;
				}
				target.draw(_tiles[x][y], tileStates);
			} else {
				target.draw(_tiles[x][y], states);
			}
		}
	}

	// Step 2. the selected unit closed list tiles.
	if (_selectedUnitRenderData.top().selectedUnit > 0 &&
		!_selectedUnitRenderData.top().disableRenderingEffects) {
		for (asUINT i = 0, size = _selectedUnitRenderData.top().closedList->
			GetSize(); i < size; ++i) {
			target.draw(((awe::closed_list_node*)
				_selectedUnitRenderData.top().closedList->At(i))->sprite,
				states);
		}
	}

	// Step 3. the units.
	// Unfortunately units have to be looped through separately to prevent tiles
	// taller than the minimum height from drawing over units. If a unit has a
	// location override, then render it, even if it isn't on the map according to
	// `isUnitOnMap()`.
	const auto currentArmy = getSelectedArmy();
	for (const auto unitsPair : _units) {
		const awe::UnitID unitID = unitsPair.first;
		if (_isUnitPresent(unitID) && ((isUnitOnMap(unitID) &&
			(_alwaysShowHiddenUnits || isUnitVisible(unitID, currentArmy))) ||
			_unitLocationOverrides.find(unitID) != _unitLocationOverrides.end())) {
			sf::RenderStates unitStates = states;
			unitStates.shader = &_unavailableTileShader;
			if (_selectedUnitRenderData.top().selectedUnit > 0 &&
				!_selectedUnitRenderData.top().disableRenderingEffects &&
				unitID != _selectedUnitRenderData.top().selectedUnit &&
				(!_selectedUnitRenderData.top().disableShaderForAvailableUnits ||
				_selectedUnitRenderData.top().availableTiles.find(
					getUnitPosition(unitID)) ==
					_selectedUnitRenderData.top().availableTiles.end())) {
				target.draw(_units.at(unitID), unitStates);
			} else {
				if (isUnitWaiting(unitID)) {
					target.draw(_units.at(unitID), unitStates);
				} else {
					target.draw(_units.at(unitID), states);
				}
			}
		}
	}

	// Step 4. the cursor.
	if (!_cursor.getSprite().empty()) target.draw(_cursor, states);

	// Step 5. restore old view.
	target.setView(oldView);
}

void awe::map::_updateCapturingUnit(const awe::UnitID id) {
	if (id > 0 && isUnitCapturing(id)) {
		const auto t = getUnitPosition(id);
		// If unit is out-of-bounds, don't do anything. This case can come about
		// when a capturing unit is deleted as a map is shrinking.
		if (!_isOutOfBounds(t))
			setTileHP(t, (awe::HP)getTileType(t)->getType()->getMaxHP());
		unitCapturing(id, false);
	}
}

awe::UnitID awe::map::_findUnitID() {
	if (_units.size() == 0) return _lastUnitID;
	// Minus 1 to account for the reserved value, 0.
	if (_units.size() == std::numeric_limits<awe::UnitID>::max() - 1) 
		throw std::bad_alloc();
	awe::UnitID temp = _lastUnitID + 1;
	while (_isUnitPresent(temp)) {
		if (temp == std::numeric_limits<awe::UnitID>::max()) temp = 1;
		else ++temp;
	}
	_lastUnitID = temp;
	return temp;
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	const engine::scripts& scripts) {
	closedList = scripts.createArray("ClosedListNode");
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	const awe::map::selected_unit_render_data& o) {
	*this = o;
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	awe::map::selected_unit_render_data&& o) noexcept {
	selectedUnit = std::move(o.selectedUnit);
	availableTiles = std::move(o.availableTiles);
	availableTileShader = std::move(o.availableTileShader);
	closedList = std::move(o.closedList);
	if (closedList) closedList->AddRef();
	disableRenderingEffects = std::move(o.disableRenderingEffects);
	disableShaderForAvailableUnits = std::move(o.disableShaderForAvailableUnits);
}

awe::map::selected_unit_render_data::~selected_unit_render_data() noexcept {
	if (closedList) closedList->Release();
}

awe::map::selected_unit_render_data&
	awe::map::selected_unit_render_data::operator=(
	const awe::map::selected_unit_render_data& o) {
	selectedUnit = o.selectedUnit;
	availableTiles = o.availableTiles;
	availableTileShader = o.availableTileShader;
	closedList = o.closedList;
	if (closedList) closedList->AddRef();
	disableRenderingEffects = o.disableRenderingEffects;
	disableShaderForAvailableUnits = o.disableShaderForAvailableUnits;
	return *this;
}

void awe::map::selected_unit_render_data::clearState() {
	selectedUnit = 0;
	availableTiles.clear();
	availableTileShader = awe::available_tile_shader::None;
	closedList->RemoveRange(0, closedList->GetSize());
	disableRenderingEffects = false;
	disableShaderForAvailableUnits = false;
}

void awe::map::_initShaders() {
	_unavailableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xyz /= 2.0; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_unavailableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_availableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xy *= 1.1; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_availableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_attackableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.x = 1.0; pixel.yz -= 0.25;"
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_attackableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
}
