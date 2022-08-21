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

int awe::game_engine::run(const std::string& file) noexcept {
	auto r = _initCheck();
	if (r) return r;
	_gui->setTarget(*_renderer);

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

void awe::game_engine::initialiseScripts(const std::string& guiFolder) noexcept {
	if (_guiScripts) {
		// Add the registrants, then only load scripts for GUI.
		_guiScripts->addRegistrant(this);
		_guiScripts->loadScripts(guiFolder);
	} else {
		_logger.error("initialiseScripts() was called before setting the GUI "
			"scripts object!");
	}
}

void awe::game_engine::registerInterface(asIScriptEngine* engine) noexcept {
	// register the object types
	engine->RegisterObjectType("joystick_axis", sizeof(sfx::joystick),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sfx::joystick>() |
		asOBJ_APP_CLASS_ALLINTS);
	engine->RegisterObjectProperty("joystick_axis", "uint axis",
		asOFFSET(sfx::joystick, axis));
	engine->RegisterObjectProperty("joystick_axis", "int direction",
		asOFFSET(sfx::joystick, direction));

	// register the global functions
	engine->RegisterGlobalFunction("void info(const string& in)",
		asMETHODPR(engine::logger, write, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	engine->RegisterGlobalFunction("void warn(const string& in)",
		asMETHODPR(engine::logger, warning, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	engine->RegisterGlobalFunction("void error(const string& in)",
		asMETHODPR(engine::logger, error, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &_logger);
	engine->RegisterGlobalFunction("float getSoundVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	engine->RegisterGlobalFunction("void setSoundVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _sounds.get());
	engine->RegisterGlobalFunction("float getMusicVolume()",
		asMETHOD(sfx::audio, getVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	engine->RegisterGlobalFunction("void setMusicVolume(const float)",
		asMETHOD(sfx::audio, setVolume),
		asCALL_THISCALL_ASGLOBAL, _music.get());
	engine->RegisterGlobalFunction("void setFullscreen(const bool)",
		asMETHOD(awe::game_engine, _script_setFullscreen),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("bool getFullscreen()",
		asMETHOD(awe::game_engine, _script_getFullscreen),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setVSync(const bool)",
		asMETHOD(awe::game_engine, _script_setVSync),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("bool getVSync()",
		asMETHOD(awe::game_engine, _script_getVSync),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void loadMusicConfig()",
		asMETHOD(awe::game_engine, _script_loadMusicConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void saveMusicConfig()",
		asMETHOD(awe::game_engine, _script_saveMusicConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void loadSoundConfig()",
		asMETHOD(awe::game_engine, _script_loadSoundConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void saveSoundConfig()",
		asMETHOD(awe::game_engine, _script_saveSoundConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void loadRendererConfig()",
		asMETHOD(awe::game_engine, _script_loadRendererConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void saveRendererConfig()",
		asMETHOD(awe::game_engine, _script_saveRendererConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("uint getJoystickID()",
		asMETHOD(sfx::user_input, getJoystickID), asCALL_THISCALL_ASGLOBAL,
		_userinput.get());
	engine->RegisterGlobalFunction("void setJoystickID(uint)",
		asMETHOD(sfx::user_input, setJoystickID), asCALL_THISCALL_ASGLOBAL,
		_userinput.get());
	engine->RegisterGlobalFunction("float getJoystickAxisThreshold()",
		asMETHOD(sfx::user_input, getJoystickAxisThreshold),
		asCALL_THISCALL_ASGLOBAL, _userinput.get());
	engine->RegisterGlobalFunction("void setJoystickAxisThreshold(float)",
		asMETHOD(sfx::user_input, setJoystickAxisThreshold),
		asCALL_THISCALL_ASGLOBAL, _userinput.get());
	engine->RegisterGlobalFunction("void loadUIConfig()",
		asMETHOD(awe::game_engine, _script_loadUIConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void saveUIConfig()",
		asMETHOD(awe::game_engine, _script_saveUIConfig),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction(
		"void loadMap(const string& in, const string& in)",
		asMETHOD(awe::game_engine, _script_loadMap),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void saveMap()",
		asMETHOD(awe::game_engine, _script_saveMap),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void quitMap()",
		asMETHOD(awe::game_engine, _script_quitMap),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void handleMapInput(const dictionary@)",
		asMETHOD(awe::game_engine, _script_handleMapInput),
		asCALL_THISCALL_ASGLOBAL, this);
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
		std::shared_ptr<sfx::scripts> gameScripts =
			std::make_shared<sfx::scripts>();
		gameScripts->addRegistrant(this);
		gameScripts->addRegistrant(_gui.get());
		// Create the game.
		_currentGame = std::make_unique<awe::game>(file, _gui, gameScripts,
			_countries, _tiles, _units, _commanders);
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
	if (controls && _currentGame) {
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


// set methods

void awe::game_engine::setCountries(
	const std::shared_ptr<awe::bank<awe::country>>& ptr) noexcept {
	_countries = ptr;
}

void awe::game_engine::setWeathers(
	const std::shared_ptr<awe::bank<awe::weather>>& ptr) noexcept {
	_weathers = ptr;
}

void awe::game_engine::setEnvironments(
	const std::shared_ptr<awe::bank<awe::environment>>& ptr) noexcept {
	_environments = ptr;
}

void awe::game_engine::setMovements(
	const std::shared_ptr<awe::bank<awe::movement_type>>& ptr) noexcept {
	_movements = ptr;
}

void awe::game_engine::setTerrains(
	const std::shared_ptr<awe::bank<awe::terrain>>& ptr) noexcept {
	_terrains = ptr;
}

void awe::game_engine::setTiles(
	const std::shared_ptr<awe::bank<awe::tile_type>>& ptr) noexcept {
	_tiles = ptr;
}

void awe::game_engine::setUnits(
	const std::shared_ptr<awe::bank<awe::unit_type>>& ptr) noexcept {
	_units = ptr;
}

void awe::game_engine::setCommanders(
	const std::shared_ptr<awe::bank<awe::commander>>& ptr) noexcept {
	_commanders = ptr;
}

void awe::game_engine::setDictionary(
	const std::shared_ptr<engine::language_dictionary>& ptr) noexcept {
	_dictionary = ptr;
}

void awe::game_engine::setFonts(const std::shared_ptr<sfx::fonts>& ptr) noexcept {
	_fonts = ptr;
}

void awe::game_engine::setSounds(const std::shared_ptr<sfx::audio>& ptr) noexcept {
	_sounds = ptr;
}

void awe::game_engine::setMusic(const std::shared_ptr<sfx::audio>& ptr) noexcept {
	_music = ptr;
}

void awe::game_engine::setRenderer(const std::shared_ptr<sfx::renderer>& ptr)
	noexcept {
	_renderer = ptr;
	if (_renderer)
		_tempRendererSettings = _renderer->getSettings();
	else
		_tempRendererSettings = sfx::renderer_settings(); // blank settings object
}

void awe::game_engine::setUserInput(const std::shared_ptr<sfx::user_input>& ptr)
	noexcept {
	_userinput = ptr;
}

void awe::game_engine::setSpritesheets(
	const std::shared_ptr<awe::spritesheets>& ptr) noexcept {
	_sprites = ptr;
}

void awe::game_engine::setScripts(const std::shared_ptr<sfx::scripts>& guiPtr)
	noexcept {
	_guiScripts = guiPtr;
}

void awe::game_engine::setGUI(const std::shared_ptr<sfx::gui>& ptr) noexcept {
	_gui = ptr;
}