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

#include "engine.h"
#include "game.h"

awe::game_engine::game_engine(const std::string& name) noexcept : _logger(name) {}

int awe::game_engine::run() noexcept {
	auto r = _initCheck();
	if (r) return r;

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
			_renderer->animate(*_gui, 2.0);
			if (_currentGame) {
				_renderer->animate(*_currentGame, 2.0);
				_renderer->draw(*_currentGame,
					sf::RenderStates().transform.scale(2.0f, 2.0f));
			}
			_renderer->draw(*_gui,
				sf::RenderStates().transform.scale(2.0f, 2.0f));
			_renderer->display();
		}
	} catch (std::exception& e) {
		_logger.error("Exception: {}", e.what());
	}

	return 0;
}

// script interface

void awe::game_engine::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	// register the object types
	int r = engine->RegisterObjectType("joystick_axis", sizeof(sfx::joystick),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sfx::joystick>() |
		asOBJ_APP_CLASS_ALLINTS);
	engine->RegisterObjectProperty("joystick_axis", "uint axis",
		asOFFSET(sfx::joystick, axis));
	engine->RegisterObjectProperty("joystick_axis", "int direction",
		asOFFSET(sfx::joystick, direction));
	document->DocumentObjectType(r, "Represents a joystick axis input. This class "
		"stores the ID of the axis being input, and which direction the axis is "
		"currently being pressed (either -1 or 1).");

	// register the global functions
	r = engine->RegisterGlobalFunction("void info(const string& in)",
		asMETHODPR(engine::logger, write, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	document->DocumentGlobalFunction(r, "Writes to the log using the info level.");

	r = engine->RegisterGlobalFunction("void warn(const string& in)",
		asMETHODPR(engine::logger, warning, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	document->DocumentGlobalFunction(r, "Writes to the log using the warning "
		"level.");

	r = engine->RegisterGlobalFunction("void error(const string& in)",
		asMETHODPR(engine::logger, error, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	document->DocumentGlobalFunction(r, "Writes to the log using the error "
		"level.");

	r = engine->RegisterGlobalFunction("float getSoundVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	document->DocumentGlobalFunction(r, "Gets the current sound volume.");

	r = engine->RegisterGlobalFunction("void setSoundVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	document->DocumentGlobalFunction(r, "Sets the current sound volume.");

	r = engine->RegisterGlobalFunction("float getMusicVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	document->DocumentGlobalFunction(r, "Gets the current music volume.");

	r = engine->RegisterGlobalFunction("void setMusicVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	document->DocumentGlobalFunction(r, "Sets the current music volume.");

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
		"configuration script. See saveRendererConfig() for more information.");

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
		"configuration.");

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
		"is no open map, then an error will be logged.");

	r = engine->RegisterGlobalFunction("void quitMap()",
		asMETHOD(awe::game_engine, _script_quitMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes the currently open map and "
		"switches back to the menu that was being displayed when loadMap() was "
		"originally called. If there is no open map, then an error will be "
		"logged.");

	r = engine->RegisterGlobalFunction("void handleMapInput(const dictionary@)",
		asMETHOD(awe::game_engine, _script_handleMapInput),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Should be called by the \"map\" menu's "
		"HandleInput() function to allow the game scripts to handle user input.");
}

bool awe::game_engine::_load(engine::json& j) noexcept {
	// Find the base path of the assets folder and make it the CWD.
	std::filesystem::path basePath = getScriptPath();
	basePath = basePath.parent_path();
	std::filesystem::current_path(basePath);
	// Retrieve a few of the paths manually instead of via _loadObject().
	// Don't forget to apply the base path to them.
	std::string guiScriptsPath, guiPath;
	j.apply(guiScriptsPath, { "guiscripts" });
	j.apply(guiPath, { "gui" });
	j.apply(_gameScriptsFolder, { "gamescripts" });
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
	// Continue loading most of the objects.
	ret =  _loadObject(_userinput, j, { "userinput" })
		&& _loadObject(_sprites->CO, j, { "spritesheets", "co" })
		&& _loadObject(_sprites->unit->idle, j,
			{ "spritesheets", "unit", "idle" })
		&& _loadObject(_sprites->tile->normal, j,
			{ "spritesheets", "tile", "normal" })
		// && _loadObject(_sprites->unitPicture, j,
			// { "spritesheets", "unit", "pictures" })
		// && _loadObject(_sprites->tilePicture->normal, j,
			// { "spritesheets", "tile", "normalpictures" })
		&& _loadObject(_sprites->icon, j, { "spritesheets", "icon" })
		&& _loadObject(_sprites->GUI, j, { "spritesheets", "gui" })
		&& _loadObject(_countries, j, { "countries" })
		&& _loadObject(_weathers, j, { "weathers" })
		&& _loadObject(_environments, j, { "environments" })
		&& _loadObject(_movements, j, { "movements" })
		&& _loadObject(_terrains, j, { "terrains" })
		&& _loadObject(_tiles, j, { "tiles" })
		&& _loadObject(_units, j, { "units" })
		&& _loadObject(_commanders, j, { "commanders" });
	if (!ret) return false;
	// Ignore the state of these objects for now. Can't load them currently
	// because I have no tile or unit pictures to configure with.
	_loadObject(_sprites->unitPicture, j,
		{ "spritesheets", "unit", "pictures" });
	_loadObject(_sprites->tilePicture->normal, j,
		{ "spritesheets", "tile", "normalpictures" });
	j.resetState();
	// Allocate GUIs and their scripts.
	_guiScripts = std::make_shared<engine::scripts>("gui_scripts");
	_gui = std::make_shared<sfx::gui>(_guiScripts);
	_guiScripts->addRegistrant(this);
	_guiScripts->loadScripts(guiScriptsPath);
	_guiScripts->generateDocumentation();
	_gui->addSpritesheet("icon", _sprites->icon);
	_gui->setLanguageDictionary(_dictionary);
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
	// If there is already a map loaded, log an error, otherwise continue.
	if (_currentGame) {
		_logger.error("Attempted to load a new map \"{}\" when map \"{}\" was "
			"already loaded!", file, "implement getMapName() in game");
	} else {
		// Make a new scripts instance for this new game.
		std::shared_ptr<engine::scripts> gameScripts =
			std::make_shared<engine::scripts>("game_scripts");
		gameScripts->addRegistrant(this);
		gameScripts->addRegistrant(_gui.get());
		// Create the game.
		_currentGame = std::make_unique<awe::game>(file, _gameScriptsFolder,
			gameScripts, _countries, _tiles, _units, _commanders);
		_currentGame->load();
		_currentGame->setTileSpritesheet(_sprites->tile->normal);
		_currentGame->setUnitSpritesheet(_sprites->unit->idle);
		_currentGame->setIconSpritesheet(_sprites->icon);
		_currentGame->setCOSpritesheet(_sprites->CO);
		_currentGame->setFont((*_fonts)["dialogue"]);
		// Remember what the last menu was so that we can easily go back to it when
		// the user quits.
		_menuBeforeMapLoad = _gui->getGUI();
		_gui->setGUI(menu);
	}
}

void awe::game_engine::_script_saveMap() {
	if (_currentGame) {
		if (!_currentGame->save()) {
			_logger.error("Call to \"saveMap()\" couldn't save the current map.");
		}
	} else {
		_logger.warning("Called \"saveMap()\" without there being a map loaded.");
	}
}

void awe::game_engine::_script_quitMap() {
	if (!_currentGame) {
		_logger.warning("Called \"quitMap()\" without there being a map loaded.");
	} else {
		_currentGame = nullptr;
		_gui->setGUI(_menuBeforeMapLoad);
	}
}

void awe::game_engine::_script_handleMapInput(CScriptDictionary* controls) {
	if (controls) {
		if (_currentGame)
			_currentGame->handleInput(controls);
		// When finished with the object handle it must be released.
		controls->Release();
	}
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
	if (!_guiScripts) errstring += "gui scripts\n";
	if (!_gui) errstring += "gui\n";
	if (errstring.length()) {
		_logger.error("Fatal error: could not run the game engine due to the "
			"following objects not being allocated correctly:\n{}Game will now "
			"shut down.", errstring);
		return 1;
	}
	return 0;
}