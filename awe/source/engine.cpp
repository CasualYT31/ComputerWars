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

#include "engine.hpp"
#include "game.hpp"
#include "army.hpp"
#include "options.hpp"

awe::game_engine::game_engine(const std::string& name) noexcept : _logger(name) {}

int awe::game_engine::run() noexcept {
	auto r = _initCheck();
	if (r) return r;

	const float scaling = 2.0f;
	try {
		while (_renderer->isOpen()) {
			// Handle menu user input first before handling the events.
			// Use case: Map menu and MapMenu menu. Selecting a vacant tile in Map
			// will trigger the MapMenu menu. Clicking on the save button will
			// trigger the Map menu. But then a click could be configured to be
			// "select" for Map, so then the Map menu sees the select immediately
			// after and triggers MapMenu again, ensuring the MapMenu never goes
			// away. By handling the click in MapMenu last, Map doesn't get to see
			// the click and so safely ignores it for that iteration.
			_userinput->update();
			_gui->handleInput(_userinput);
			_userinput->update();
			// Now handle the events.
			sf::Event event;
			while (_renderer->pollEvent(event)) {
				if (event.type == sf::Event::Closed) _renderer->close();
				_gui->handleEvent(event);
			}

			_renderer->clear();
			_renderer->animate(*_gui, scaling);
			_renderer->animate(_game, scaling);
			_renderer->draw(_game,
				sf::RenderStates().transform.scale(scaling, scaling));
			_renderer->draw(*_gui,
				sf::RenderStates().transform.scale(scaling, scaling));
			_renderer->display();
		}
	} catch (std::exception& e) {
		_logger.error("Exception: {}", e.what());
	}

	return 0;
}

// Script interface.

sf::Vector2i awe::game_engine::_INVALID_MOUSE = sfx::INVALID_MOUSE;

void AWEVector2iTypeConstructor(const int x, const int y, void* memory) {
	new(memory) sf::Vector2i(x, y);
}

void AWEVector2TypeConstructor(const unsigned int x, const unsigned int y,
	void* memory) {
	new(memory) sf::Vector2u(x, y);
}

std::string AWEVector2TypeToString(void* memory) {
	if (memory) {
		sf::Vector2u* v = (sf::Vector2u*)memory;
		return "(" + std::to_string(v->x) + ", " + std::to_string(v->y) + ")";
	}
	return "";
}

awe::HP getDisplayedHP(const awe::HP hp) noexcept {
	auto ret = (awe::HP)ceil((double)hp / (double)awe::unit_type::HP_GRANULARITY);
	return ret;
}

// Wrapper for Vector2u and Vector2i operator==s.

bool iEqI(void* pLhs, const sf::Vector2i& rhs) noexcept {
	auto lhs = (const sf::Vector2i*)pLhs;
	return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool iEqU(void* pLhs, const sf::Vector2u& rhs) noexcept {
	auto lhs = (const sf::Vector2i*)pLhs;
	return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool uEqI(void* pLhs, const sf::Vector2i& rhs) noexcept {
	auto lhs = (const sf::Vector2u*)pLhs;
	return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool uEqU(void* pLhs, const sf::Vector2u& rhs) noexcept {
	auto lhs = (const sf::Vector2u*)pLhs;
	return lhs->x == rhs.x && lhs->y == rhs.y;
}

void awe::game_engine::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	// Register the object types.
	int r = engine->RegisterObjectType("joystick_axis", sizeof(sfx::joystick),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sfx::joystick>() |
		asOBJ_APP_CLASS_ALLINTS);
	engine->RegisterObjectProperty("joystick_axis", "uint axis",
		asOFFSET(sfx::joystick, axis));
	engine->RegisterObjectProperty("joystick_axis", "int direction",
		asOFFSET(sfx::joystick, direction));
	document->DocumentObjectType(r, "Represents a joystick axis input. This class "
		"stores the ID of the axis being input, and which direction the axis is "
		"currently being pressed.\n"
		"<tt>axis</tt> is an <tt>sf::Joystick::Axis</tt> value. If too large of "
		"an ID is given, via this object, into a function, it will be lowered "
		"down to the maximum possible value and a warning will be logged.\n"
		"A positive <tt>direction</tt> value represents the positive direction "
		"(including <tt>0</tt>), and a negative value represents the negative "
		"direction.");

	r = engine->RegisterObjectType("Vector2", sizeof(sf::Vector2u),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2u>());
	engine->RegisterObjectProperty("Vector2", "uint x",
		asOFFSET(sf::Vector2u, x));
	engine->RegisterObjectProperty("Vector2", "uint y",
		asOFFSET(sf::Vector2u, y));
	engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,
		"void Vector2(const uint, const uint)",
		asFUNCTION(AWEVector2TypeConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vector2", "string toString() const",
		asFUNCTION(AWEVector2TypeToString), asCALL_CDECL_OBJLAST);
	document->DocumentObjectType(r, "Represents a 2D vector.");

	r = engine->RegisterObjectType("MousePosition", sizeof(sf::Vector2i),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2i>());
	document->DocumentObjectType(r, "Represents a mouse position.");
	r = engine->RegisterGlobalProperty("const MousePosition INVALID_MOUSE",
		&awe::game_engine::_INVALID_MOUSE);
	engine->RegisterObjectProperty("MousePosition", "int x",
		asOFFSET(sf::Vector2i, x));
	engine->RegisterObjectProperty("MousePosition", "int y",
		asOFFSET(sf::Vector2i, y));
	engine->RegisterObjectBehaviour("MousePosition", asBEHAVE_CONSTRUCT,
		"void MousePosition(const int, const int)",
		asFUNCTION(AWEVector2iTypeConstructor), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectMethod("MousePosition",
		"bool opEquals(const MousePosition&in) const",
		asFUNCTION(iEqI), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MousePosition",
		"bool opEquals(const Vector2&in) const",
		asFUNCTION(iEqU), asCALL_CDECL_OBJFIRST);

	// Vector 2 opEquals
	r = engine->RegisterObjectMethod("Vector2",
		"bool opEquals(const Vector2&in) const",
		asFUNCTION(uEqU), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("Vector2",
		"bool opEquals(const MousePosition&in) const",
		asFUNCTION(uEqI), asCALL_CDECL_OBJFIRST);

	// Time class.
	r = engine->RegisterObjectType("Time", sizeof(sf::Time),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Time>());
	document->DocumentObjectType(r, "Represents a time value.");
	r = engine->RegisterObjectMethod("Time", "float asSeconds()",
		asMETHOD(sf::Time, asSeconds), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Return the time value as a number of "
		"seconds.");
	r = engine->RegisterObjectMethod("Time", "int32 asMilliseconds()",
		asMETHOD(sf::Time, asMilliseconds), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Return the time value as a number of "
		"milliseconds.");
	r = engine->RegisterObjectMethod("Time", "int64 asMicroseconds()",
		asMETHOD(sf::Time, asMicroseconds), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Return the time value as a number of "
		"microseconds.");
	// Time class factory functions.
	r = engine->RegisterGlobalFunction("Time seconds(const float)",
		asFUNCTION(sf::seconds), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Constructs a Time object using seconds.");
	r = engine->RegisterGlobalFunction("Time milliseconds(const int32)",
		asFUNCTION(sf::milliseconds), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Constructs a Time object using "
		"milliseconds.");
	r = engine->RegisterGlobalFunction("Time microseconds(const int64)",
		asFUNCTION(sf::microseconds), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Constructs a Time object using "
		"microseconds.");

	// Clock class.
	r = engine->RegisterObjectType("Clock", sizeof(sf::Clock),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Clock>());
	document->DocumentObjectType(r, "Used to calculate elapsed time.");
	r = engine->RegisterObjectMethod("Clock", "Time getElapsedTime()",
		asMETHOD(sf::Clock, getElapsedTime), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Calculates the elapsed time since the "
		"clock was constructed or since <tt>restart()</tt> was called.");
	r = engine->RegisterObjectMethod("Clock", "Time restart()",
		asMETHOD(sf::Clock, restart), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Restarts the clock. Returns the time "
		"elapsed.");

	// GameOptions class.
	awe::game_options::registerGameOptionsType(engine, document, _scripts);

	// GameInterface.
	awe::RegisterGameTypedefs(engine, document);
	r = engine->RegisterObjectType("GameInterface", 0,
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
		"const Tile getTileType(const Vector2&in)",
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
		"const Unit getUnitType(const UnitID)",
		asMETHOD(awe::game, getUnitType), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns details on a unit's type.");

	r = engine->RegisterObjectMethod("GameInterface",
		"ArmyID getArmyOfUnit(const UnitID)",
		asMETHOD(awe::game, getArmyOfUnit), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the ID of the army who owns a "
		"specified unit.");

	r = engine->RegisterObjectMethod("GameInterface",
		"HP getUnitHP(const UnitID)",
		asMETHOD(awe::game, getUnitHP), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the HP that a given unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Fuel getUnitFuel(const UnitID)",
		asMETHOD(awe::game, getUnitFuel), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the fuel that a given unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"Ammo getUnitAmmo(const UnitID)",
		asMETHOD(awe::game, getUnitAmmo), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the ammo that a given unit has.");

	r = engine->RegisterObjectMethod("GameInterface",
		"array<UnitID>@ getLoadedUnits(const UnitID)",
		asMETHOD(awe::game, getLoadedUnits), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Returns the IDs of the units that are "
		"loaded onto the one specified.");

	// Register game global property and related constants.
	r = engine->RegisterGlobalProperty("const ArmyID NO_ARMY",
		&awe::army::NO_ARMY_SCRIPT);
	document->DocumentExpectedFunction("const ArmyID NO_ARMY", "Represents \"no "
		"army\". Used to signify \"no ownership.\"");
	r = engine->RegisterGlobalProperty("GameInterface game", &_game);

	// Register the global functions.
	r = engine->RegisterGlobalFunction("void info(const string&in)",
		asMETHOD(engine::scripts, writeToLog),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Writes to the log using the info level.");

	r = engine->RegisterGlobalFunction("void warn(const string&in)",
		asMETHOD(engine::scripts, warningToLog),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Writes to the log using the warning "
		"level.");

	r = engine->RegisterGlobalFunction("void error(const string&in)",
		asMETHOD(engine::scripts, errorToLog),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Writes to the log using the error "
		"level.");

	r = engine->RegisterGlobalFunction("float getSoundVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	document->DocumentGlobalFunction(r, "Gets the current sound volume.");

	r = engine->RegisterGlobalFunction("void setSoundVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	document->DocumentGlobalFunction(r, "Sets the current sound volume. The value "
		"must be between <tt>0.0</tt> and <tt>100.0</tt>. If under or over the "
		"bounds, it will be adjusted upwards or downwards to the bound "
		"accordingly, and a warning will be logged.");

	r = engine->RegisterGlobalFunction("float getMusicVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	document->DocumentGlobalFunction(r, "Gets the current music volume.");

	r = engine->RegisterGlobalFunction("void setMusicVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	document->DocumentGlobalFunction(r, "Sets the current music volume. The value "
		"must be between <tt>0.0</tt> and <tt>100.0</tt>. If under or over the "
		"bounds, it will be adjusted upwards or downwards to the bound "
		"accordingly, and a warning will be logged.");

	r = engine->RegisterGlobalFunction("void setFullscreen(const bool)",
		asMETHOD(awe::game_engine, _script_setFullscreen),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the full screen setting. See "
		"saveRendererConfig() for more information.");

	r = engine->RegisterGlobalFunction("bool getFullscreen()",
		asMETHOD(awe::game_engine, _script_getFullscreen),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets the full screen setting.");

	r = engine->RegisterGlobalFunction("void setVSync(const bool)",
		asMETHOD(awe::game_engine, _script_setVSync),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the V-sync setting. See "
		"saveRendererConfig() for more information.");

	r = engine->RegisterGlobalFunction("bool getVSync()",
		asMETHOD(awe::game_engine, _script_getVSync),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets the V-sync setting.");

	r = engine->RegisterGlobalFunction("void loadMusicConfig()",
		asMETHOD(awe::game_engine, _script_loadMusicConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Loads the music configuration script. "
		"This will replace all of the configurations that are currently loaded "
		"(including the music volume).");

	r = engine->RegisterGlobalFunction("void saveMusicConfig()",
		asMETHOD(awe::game_engine, _script_saveMusicConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Saves the music configuration (i.e. the "
		"music volume).");

	r = engine->RegisterGlobalFunction("void loadSoundConfig()",
		asMETHOD(awe::game_engine, _script_loadSoundConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Loads the sound configuration script. "
		"This will replace all of the configurations that are currently loaded "
		"(including the sound volume).");

	r = engine->RegisterGlobalFunction("void saveSoundConfig()",
		asMETHOD(awe::game_engine, _script_saveSoundConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Saves the sound configuration (i.e. the "
		"sound volume).");

	r = engine->RegisterGlobalFunction("void loadRendererConfig()",
		asMETHOD(awe::game_engine, _script_loadRendererConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Restores the renderer settings from the "
		"configuration script. Also reopens the render window. See "
		"<tt>saveRendererConfig()</tt> for more information.");

	r = engine->RegisterGlobalFunction("void saveRendererConfig()",
		asMETHOD(awe::game_engine, _script_saveRendererConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "All of the renderer settings (e.g. the "
		"full screen setting, V-sync) are only temporarily stored and are not "
		"applied when they are set via the script. To apply them, the script has "
		"to save them using this function. This is not the case for other "
		"configurations.");

	r = engine->RegisterGlobalFunction("uint getJoystickID()",
		asMETHOD(sfx::user_input, getJoystickID), asCALL_THISCALL_ASGLOBAL,
		_userinput.get());
	document->DocumentGlobalFunction(r, "Gets the ID of the joystick that is "
		"currently configured to provide input to the game.");

	r = engine->RegisterGlobalFunction("void setJoystickID(uint)",
		asMETHOD(sfx::user_input, setJoystickID), asCALL_THISCALL_ASGLOBAL,
		_userinput.get());
	document->DocumentGlobalFunction(r, "Sets the ID of the joystick that will "
		"provide input to the game. Does not make any changes if the given ID "
		"could not identify a joystick.");

	r = engine->RegisterGlobalFunction("float getJoystickAxisThreshold()",
		asMETHOD(sfx::user_input, getJoystickAxisThreshold),
		asCALL_THISCALL_ASGLOBAL, _userinput.get());
	document->DocumentGlobalFunction(r, "Gets the configured axis threashold. "
		"Axis inputs will only be recognised by the game if they are at or over "
		"this threshold.");

	r = engine->RegisterGlobalFunction("void setJoystickAxisThreshold(float)",
		asMETHOD(sfx::user_input, setJoystickAxisThreshold),
		asCALL_THISCALL_ASGLOBAL, _userinput.get());
	document->DocumentGlobalFunction(r, "Updates the axis threshold "
		"configuration. The value must be between <tt>5.0</tt> and <tt>95.0</tt>. "
		"If under or over the bounds, it will be adjusted upwards or downwards to "
		"the bound accordingly, and a warning will be logged.");

	r = engine->RegisterGlobalFunction("void loadUIConfig()",
		asMETHOD(awe::game_engine, _script_loadUIConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Loads the UI configuration script. This "
		"will replace all of the configurations that are currently loaded "
		"(including the joystick ID and axis threashold).");

	r = engine->RegisterGlobalFunction("void saveUIConfig()",
		asMETHOD(awe::game_engine, _script_saveUIConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Saves the UI configuration (i.e. the "
		"joystick ID and axis threashold).");

	r = engine->RegisterGlobalFunction(
		"void loadMap(const string& in, const string& in)",
		asMETHOD(awe::game_engine, _script_loadMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Opens a map (its file path being the "
		"first parameter), and then switches to the menu given in the second "
		"parameter. The menu should be \"empty\" so as to display the map on the "
		"screen. If there is already a map open at the time of the call, then an "
		"error will be logged and no changes will occur.");

	r = engine->RegisterGlobalFunction("void saveMap()",
		asMETHOD(awe::game_engine, _script_saveMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Saves the currently open map. If there "
		"is no open map, then a warning will be logged.");

	r = engine->RegisterGlobalFunction("void quitMap()",
		asMETHOD(awe::game_engine, _script_quitMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes the currently open map and "
		"switches back to the menu that was being displayed when loadMap() was "
		"originally called. If there is no open map, then a warning will be "
		"logged.");

	r = engine->RegisterGlobalFunction("MousePosition mousePosition()",
		asMETHOD(sfx::user_input, mousePosition),
		asCALL_THISCALL_ASGLOBAL, _userinput.get());
	document->DocumentGlobalFunction(r, "Retrieves the current position of the "
		"mouse, in pixels, relative to the game window's upper left corner of the "
		"client area. Will return <tt>INVALID_MOUSE</tt> if the game's window "
		"does not have focus.");

	r = engine->RegisterGlobalFunction("Vector2 getWindowSize()",
		asMETHOD(sfx::renderer, getSize),
		asCALL_THISCALL_ASGLOBAL, _renderer.get());
	document->DocumentGlobalFunction(r, "Returns the render window's client "
		"region's size, in pixels.");

	r = engine->RegisterGlobalFunction("HP getDisplayedHP(const HP)",
		asFUNCTION(getDisplayedHP), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Receives an internal unit HP value and "
		"returns the unit HP value that is displayed to the user.");
}

bool awe::game_engine::_load(engine::json& j) noexcept {
	// Find the base path of the assets folder and make it the CWD.
	std::filesystem::path basePath = getScriptPath();
	basePath = basePath.parent_path();
	std::filesystem::current_path(basePath);
	// Retrieve a few of the paths manually instead of via _loadObject().
	// Don't forget to apply the base path to them.
	std::string scriptsPath, guiPath;
	j.apply(scriptsPath, { "scripts" });
	j.apply(guiPath, { "gui" });
	if (!j.inGoodState()) return false;
	// Allocate spritesheets object.
	_sprites = std::make_shared<awe::spritesheets>();
	_sprites->unit = std::make_shared<awe::spritesheets::units>();
	_sprites->tile = std::make_shared<awe::spritesheets::tiles>();
	_sprites->tilePicture = std::make_shared<awe::spritesheets::tile_pictures>();
	// Load most of the objects.
	bool ret = _loadObject(_dictionary, j, { "languages" })
		&& _loadObject(_fonts, j, { "fonts" })
		&& _loadObject(_sounds, j, { "sounds" })
		&& _loadObject(_music, j, { "music" })
		&& _loadObject(_renderer, j, { "renderer" });
	if (!ret) return false;
	// Opening the renderer now will prevent glFlush() SFML errors from plaguing
	// standard output when I load images in the animated_spritesheet objects
	// below.
	_renderer->openWindow();
	// Allocate GUI and scripts objects, but don't initialise yet.
	_scripts = std::make_shared<engine::scripts>();
	_game.setScripts(_scripts);
	_gui = std::make_shared<sfx::gui>(_scripts);
	// Continue loading most of the objects.
	ret =  _loadObject(_userinput, j, { "userinput" })
		&& _loadObject(_sprites->CO, j, { "spritesheets", "co" })
		&& _loadObject(_sprites->unit->idle, j,
			{ "spritesheets", "unit", "idle" })
		&& _loadObject(_sprites->tile->normal, j,
			{ "spritesheets", "tile", "normal" })
		&& _loadObject(_sprites->unitPicture, j,
			{ "spritesheets", "unit", "pictures" })
		&& _loadObject(_sprites->tilePicture->normal, j,
			{ "spritesheets", "tile", "normalpictures" })
		&& _loadObject(_sprites->icon, j, { "spritesheets", "icon" })
		&& _loadObject(_sprites->GUI, j, { "spritesheets", "gui" })
		&& _loadObject(_countries, j, { "countries" }, _scripts, "Country",
			"country_bank")
		&& _loadObject(_weathers, j, { "weathers" }, _scripts, "Weather",
			"weather_bank")
		&& _loadObject(_environments, j, { "environments" }, _scripts,
			"Environment", "environment_bank")
		&& _loadObject(_movements, j, { "movements" }, _scripts, "Movement",
			"movement_bank")
		&& _loadObject(_terrains, j, { "terrains" }, _scripts, "Terrain",
			"terrain_bank")
		&& _loadObject(_tiles, j, { "tiles" }, _scripts, "Tile",
			"tile_bank")
		&& _loadObject(_units, j, { "units" }, _scripts, "Unit",
			"unit_bank")
		&& _loadObject(_commanders, j, { "commanders" }, _scripts, "Commander",
			"commander_bank");
	if (!ret) return false;
	// Ignore the state of these objects for now. Can't load them currently
	// because I have no tile or unit pictures to configure with.
	_loadObject(_sprites->unitPicture, j,
		{ "spritesheets", "unit", "pictures" });
	_loadObject(_sprites->tilePicture->normal, j,
		{ "spritesheets", "tile", "normalpictures" });
	j.resetState();
	// Initialise GUIs and the scripts.
	_scripts->addRegistrant(this);
	_scripts->loadScripts(scriptsPath);
	_scripts->generateDocumentation();
	_gui->addSpritesheet("icon", _sprites->icon);
	_gui->addSpritesheet("co", _sprites->CO);
	_gui->addSpritesheet("tilePicture.normal", _sprites->tilePicture->normal);
	_gui->addSpritesheet("unitPicture", _sprites->unitPicture);
	_gui->addSpritesheet("tile.normal", _sprites->tile->normal);
	_gui->addSpritesheet("unit", _sprites->unit->idle);
	_gui->setLanguageDictionary(_dictionary);
	_gui->setFonts(_fonts);
	_gui->setTarget(*_renderer);
	_gui->load(guiPath);
	if (!_gui->inGoodState()) return false;
	// Finish initialisation of banks.
	awe::updateAllTerrains(*_tiles, *_terrains);
	awe::updateAllMovementsAndLoadedUnits(*_units, *_movements);
	// If one of the objects failed to initialise properly, return FALSE.
	_userinput->tieWindow(_renderer);
	return true;
}

bool awe::game_engine::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

void awe::game_engine::_script_setFullscreen(const bool in) {
	_tempRendererSettings.style.fullscreen = in;
}

bool awe::game_engine::_script_getFullscreen() {
	return _tempRendererSettings.style.fullscreen;
}

void awe::game_engine::_script_setVSync(const bool in) {
	_tempRendererSettings.style.vsync = in;
}

bool awe::game_engine::_script_getVSync() {
	return _tempRendererSettings.style.vsync;
}

void awe::game_engine::_script_loadMusicConfig() {
	_music->load();
}

void awe::game_engine::_script_saveMusicConfig() {
	_music->save();
}

void awe::game_engine::_script_loadSoundConfig() {
	_sounds->load();
}

void awe::game_engine::_script_saveSoundConfig() {
	_sounds->save();
}

void awe::game_engine::_script_loadRendererConfig() {
	_renderer->load();
	_renderer->openWindow();
	_tempRendererSettings = _renderer->getSettings();
}

void awe::game_engine::_script_saveRendererConfig() {
	_renderer->setSettings(_tempRendererSettings);
	_renderer->save();
}

void awe::game_engine::_script_loadUIConfig() {
	_userinput->load();
}

void awe::game_engine::_script_saveUIConfig() {
	_userinput->save();
}

void awe::game_engine::_script_loadMap(const std::string& file,
	const std::string& menu) {
	// Create the game.
	_game.load(file, _countries, _tiles, _units, _commanders,
		_sprites->tile->normal, _sprites->unit->idle, _sprites->icon, _sprites->CO,
		(*_fonts)["AW2"], _dictionary);
	// Remember what the last menu was so that we can easily go back to it when
	// the user quits.
	_menuBeforeMapLoad = _gui->getGUI();
	_gui->setGUI(menu);
}

void awe::game_engine::_script_saveMap() {
	if (!_game.save())
		_logger.error("Call to \"saveMap()\" couldn't save the current map.");
}

void awe::game_engine::_script_quitMap() {
	_game.quit();
	_gui->setGUI(_menuBeforeMapLoad);
}

// initCheck()

int awe::game_engine::_initCheck() const noexcept {
	std::string errstring = "";
	if (!_countries) errstring += "countries\n";
	if (!_weathers) errstring += "weathers\n";
	if (!_environments) errstring += "environments\n";
	if (!_movements) errstring += "movements\n";
	if (!_terrains) errstring += "terrains\n";
	if (!_tiles) errstring += "tiles\n";
	if (!_units) errstring += "units\n";
	if (!_commanders) errstring += "commanders\n";
	if (!_dictionary) errstring += "dictionary\n";
	if (!_fonts) errstring += "fonts\n";
	if (!_sounds) errstring += "sounds\n";
	if (!_music) errstring += "music\n";
	if (!_renderer) errstring += "renderer\n";
	if (!_userinput) errstring += "userinput\n";
	if (!_sprites) {
		errstring += "spritesheet collection\n";
	} else {
		_sprites->test(errstring);
	}
	if (!_scripts) errstring += "scripts\n";
	if (!_gui) errstring += "gui\n";
	if (errstring.length()) {
		_logger.error("Fatal error: could not run the game engine due to the "
			"following objects not being allocated correctly:\n{}Game will now "
			"shut down.", errstring);
		return 1;
	}
	return 0;
}