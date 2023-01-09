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

#include "engine.hpp"
#include "army.hpp"
#include <chrono>

awe::game_engine::game_engine(const engine::logger::data& data) noexcept :
	engine::json_script({ data.sink, "json_script" }), _logger(data) {
	// Credit: https://stackoverflow.com/a/13446015/6928376.
	std::random_device randomDevice;
	std::mt19937::result_type seed = randomDevice() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count()
	);
	_prng = std::make_unique<std::mt19937>(seed);
}

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

			// Now handle the events.
			sf::Event event;
			while (_renderer->pollEvent(event)) {
				if (event.type == sf::Event::Closed) _renderer->close();
				_gui->handleEvent(event);
			}

			_renderer->clear();
			_renderer->animate(*_gui, _scaling);
			if (_map) {
				_renderer->animate(*_map, _scaling);
				sf::View oldView = _renderer->getView();
				sf::View view;
				view.reset(sf::FloatRect(0.0f, 0.0f, (float)_renderer->getSize().x,
					(float)_renderer->getSize().y));
				view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
				_renderer->setView(view);
				_renderer->draw(*_map,
					sf::RenderStates().transform.scale(_scaling, _scaling));
				_renderer->setView(oldView);
			}
			_renderer->draw(*_gui,
				sf::RenderStates().transform.scale(_scaling, _scaling));
			_renderer->display();

			// This will very likely not be the way I handle win conditions in the
			// final version of the engine.
			if (_map && _map->periodic()) {
				boxer::show("The game has ended!", "Thanks for Playing!",
					boxer::Style::Info);
				_script_quitMap();
			}
		}
	} catch (std::exception& e) {
		_logger.error("Exception: {}", e.what());
	}

	return 0;
}

// Script interface.

void awe::game_engine::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	sfx::joystick::Register(engine, document);
	engine::RegisterVectorTypes(engine, document);
	engine::RegisterTimeTypes(engine, document);
	awe::map::Register(engine, document);

	// Register the global functions.
	auto r = engine->RegisterGlobalFunction("void info(const string&in)",
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

	r = engine->RegisterGlobalFunction("Map@ loadMap(const string&in)",
		asMETHOD(awe::game_engine, _script_loadMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Opens a map (its file path being the "
		"first parameter), and returns a handle to it if it could be loaded. If "
		"there is already a map open at the time of the call, then an error will "
		"be logged and no changes will occur. A null handle will be returned if "
		"the map couldn't be loaded!");

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

	r = engine->RegisterGlobalFunction("MousePosition scaledMousePosition()",
		asMETHOD(awe::game_engine, _script_scaledMousePosition),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Retrieves the current position of the "
		"mouse, in pixels, relative to the game window's upper left corner of the "
		"client area. Will return <tt>INVALID_MOUSE</tt> if the game's window "
		"does not have focus. Also scales it down appropriately depending on the "
		"scaling currently applied to all of the game's drawing. Useful for when "
		"you need to position a widget relative to the mouse position.");

	r = engine->RegisterGlobalFunction("Vector2 getWindowSize()",
		asMETHOD(sfx::renderer, getSize),
		asCALL_THISCALL_ASGLOBAL, _renderer.get());
	document->DocumentGlobalFunction(r, "Returns the render window's client "
		"region's size, in pixels.");

	r = engine->RegisterGlobalFunction("string translate(const string&in)",
		asMETHOD(awe::game_engine, _script_translate),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Translates a string without inserting "
		"any variables.");

	r = engine->RegisterGlobalFunction("string execute(string)",
		asMETHOD(engine::scripts, executeCode),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Executes code within the ComputerWars "
		"module.");

	r = engine->RegisterGlobalFunction("uint rand(const uint)",
		asMETHOD(awe::game_engine, _script_rand),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Generates a random number between 0 and "
		"the given value (inclusive).");
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
	bool ret = _loadObject(_dictionary, j, { "languages" },
			engine::logger::data{ _logger.getData().sink, "language_dictionary" })
		&& _loadObject(_fonts, j, { "fonts" },
			engine::logger::data{ _logger.getData().sink, "fonts" })
		&& _loadObject(_sounds, j, { "sounds" },
			engine::logger::data{ _logger.getData().sink, "sounds" })
		&& _loadObject(_music, j, { "music" },
			engine::logger::data{ _logger.getData().sink, "music" })
		&& _loadObject(_renderer, j, { "renderer" },
			engine::logger::data{ _logger.getData().sink, "renderer" });
	if (!ret) return false;
	// Opening the renderer now will prevent glFlush() SFML errors from plaguing
	// standard output when I load images in the animated_spritesheet objects
	// below.
	_renderer->openWindow();
	// Allocate GUI and scripts objects, but don't initialise yet.
	_scripts = std::make_shared<engine::scripts>(
		engine::logger::data{ _logger.getData().sink, "scripts" });
	_gui = std::make_shared<sfx::gui>(_scripts,
		engine::logger::data{_logger.getData().sink, "gui"});
	// Continue loading most of the objects.
	ret =  _loadObject(_userinput, j, { "userinput" },
			engine::logger::data{ _logger.getData().sink, "user_input" })
		&& _loadObject(_sprites->CO, j, { "spritesheets", "co" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->unit->idle, j,
			{ "spritesheets", "unit", "idle" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->tile->normal, j,
			{ "spritesheets", "tile", "normal" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->unitPicture, j,
			{ "spritesheets", "unit", "pictures" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->tilePicture->normal, j,
			{ "spritesheets", "tile", "normalpictures" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->icon, j, { "spritesheets", "icon" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_sprites->GUI, j, { "spritesheets", "gui" },
			engine::logger::data{ _logger.getData().sink, "spritesheet" })
		&& _loadObject(_countries, j, { "countries" }, _scripts, "Country",
			engine::logger::data{ _logger.getData().sink, "country_bank" })
		&& _loadObject(_weathers, j, { "weathers" }, _scripts, "Weather",
			engine::logger::data{ _logger.getData().sink, "weather_bank" })
		&& _loadObject(_environments, j, { "environments" }, _scripts,
			"Environment",
			engine::logger::data{ _logger.getData().sink, "environment_bank" })
		&& _loadObject(_movements, j, { "movements" }, _scripts, "Movement",
			engine::logger::data{ _logger.getData().sink, "movement_bank" })
		&& _loadObject(_terrains, j, { "terrains" }, _scripts, "Terrain",
			engine::logger::data{ _logger.getData().sink, "terrain_bank" })
		&& _loadObject(_tiles, j, { "tiles" }, _scripts, "TileType",
			engine::logger::data{ _logger.getData().sink, "tile_bank" })
		&& _loadObject(_weapons, j, { "weapons" }, _scripts, "Weapon",
			engine::logger::data{ _logger.getData().sink, "weapon_bank" })
		&& _loadObject(_units, j, { "units" }, _scripts, "UnitType",
			engine::logger::data{ _logger.getData().sink, "unit_bank" })
		&& _loadObject(_commanders, j, { "commanders" }, _scripts, "Commander",
			engine::logger::data{ _logger.getData().sink, "commander_bank" });
	if (!ret) return false;
	// Finish initialisation of banks.
	if (!awe::checkCountryTurnOrderIDs(*_countries)) {
		_logger.critical("The turn order IDs assigned to each configured country "
			"are not valid. See the log for more information.");
		for (const auto& pCountry : *_countries) {
			_logger.error("Turn order ID for country {} = {}",
				pCountry.first, pCountry.second->getTurnOrder());
		}
		return false;
	}
	awe::updateTerrainBank(*_terrains, *_countries);
	awe::updateTileTypeBank(*_tiles, *_terrains, *_countries);
	awe::updateUnitTypeBank(*_units, *_movements, *_terrains, *_weapons,
		*_countries, _logger.getData().sink);
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
	// If one of the objects failed to initialise properly, return FALSE.
	_userinput->tieWindow(_renderer);
	return true;
}

bool awe::game_engine::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

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

awe::map* awe::game_engine::_script_loadMap(const std::string& file) {
	// Create the game.
	if (_map) {
		_logger.error("Attempted to load map file \"{}\" whilst map \"{}\" was "
			"still loaded!", file, _map->getMapName());
		return nullptr;
	} else {
		try {
			_map = std::make_unique<awe::map>( _countries, _tiles, _units,
				_commanders, engine::logger::data{_logger.getData().sink, "map"});
		} catch (const std::bad_alloc& e) {
			_logger.error("Couldn't allocate the map object: {}", e.what());
			return nullptr;
		}
		_map->setTileSpritesheet(_sprites->tile->normal);
		_map->setUnitSpritesheet(_sprites->unit->idle);
		_map->setIconSpritesheet(_sprites->icon);
		_map->setCOSpritesheet(_sprites->CO);
		_map->setFont((*_fonts)["AW2"]);
		_map->setLanguageDictionary(_dictionary);
		_map->setScripts(_scripts);
		auto r = _map->load(file);
		if (r) {
			_menuBeforeMapLoad = _gui->getGUI();
			return _map.get();
		} else {
			_map = nullptr;
			_logger.error("Couldn't load map file \"{}\".", file);
			return nullptr;
		}
	}
}

void awe::game_engine::_script_quitMap() {
	_map = nullptr;
	_gui->setGUI(_menuBeforeMapLoad);
}

std::string awe::game_engine::_script_translate(const std::string& nativeString)
	const {
	return _dictionary->operator()(nativeString);
}

sf::Vector2i awe::game_engine::_script_scaledMousePosition() const {
	auto ret = _userinput->mousePosition();
	if (ret != sfx::INVALID_MOUSE) {
		return { (int)((float)ret.x / _scaling), (int)((float)ret.y / _scaling) };
	} else {
		return ret;
	}
}

unsigned int awe::game_engine::_script_rand(const unsigned int max) noexcept {
	// Credit: https://stackoverflow.com/a/13446015/6928376.
	std::uniform_int_distribution<unsigned> distribution(0, max);
	return distribution(*_prng);
}
