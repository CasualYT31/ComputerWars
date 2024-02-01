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

#include <chrono>
#include <filesystem>
#include "engine.hpp"
#include "army.hpp"

awe::game_engine::game_engine(const engine::logger::data& data) :
	engine::json_script({ data.sink, "json_script" }), _logger(data) {}

int awe::game_engine::run() {
	const auto r = _initCheck();
	if (r) return r;

	try {
		_gui->setScalingFactor(_scaling);
		// The events that must always be handled by the GUI engine, even if input
		// is not being accepted.
		static const std::unordered_set<sf::Event::EventType> REQUIRED_EVENTS{
			sf::Event::Closed,
			sf::Event::Resized,
			sf::Event::LostFocus,
			sf::Event::GainedFocus
		};
		// Used to prevent _gui->handleInput() from reading a select that was
		// intended only to skip an animation.
		bool mapAnimationSkipped = false;
		while (_renderer->isOpen()) {
			float rawDelta = 0.0f;
			const auto delta = accumulatedDelta(rawDelta);

			for (auto& audio : *_audios) audio.second->process();

			const bool acceptInput = !_guiTransition && (!_map ||
				!_map->animationInProgress());

			// Handle menu user input first before handling the events.
			// Use case: Map menu and MapMenu menu. Selecting a vacant tile in Map
			// will trigger the MapMenu menu. Clicking on the save button will
			// trigger the Map menu. But then a click could be configured to be
			// "select" for Map, so then the Map menu sees the select immediately
			// after and triggers MapMenu again, ensuring the MapMenu never goes
			// away. By handling the click in MapMenu last, Map doesn't get to see
			// the click and so safely ignores it for that iteration.
			// NB: it's also important to handle GUI input before updating the user
			// input. If not, then clicks can "leak" through to other menus if a
			// click were to update the current menu.
			if (acceptInput && !mapAnimationSkipped) _gui->handleInput(_userinput);
			mapAnimationSkipped = false;
			_userinput->update();

			_renderer->handleEvents([&](const sf::Event& e) {
				if (e.type == sf::Event::Closed) _renderer->close();
				// We need to pass in resizing-based events all of the time, or
				// else the entire GUI screws up if the target is resized.
				if (acceptInput || REQUIRED_EVENTS.count(e.type))
					_gui->handleEvent(e);
			});

			if (_map && _map->animationInProgress() &&
				(*_userinput)[_gui->getSelectControl()])
				mapAnimationSkipped = _map->skipAnimationIfPossible();

			_renderer->clear();
			_sprites->updateGlobalFrameIDs();
			_animateGUITransition();
			_renderer->animate(*_gui);
			if (_map) {
				_renderer->animate(*_map);
				_renderer->draw(*_map);
			}
			_renderer->draw(*_gui);
			if (_guiTransition) _renderer->draw(*_guiTransition);
			_drawColourFlash(rawDelta, delta);
			_renderer->display();

			if (_map && _map->periodic()) {
				boxer::show("The game has ended!", "Thanks for Playing!",
					boxer::Style::Info);
				_script_quitMap("MainMenu");
			}
		}
	} catch (const std::exception& e) {
		_logger.error("Exception: {}", e);
		throw e;
	}

	return 0;
}

void awe::game_engine::_drawColourFlash(const float rawDelta, const float delta) {
	if (delta < _colourFlashDuration) {
		auto alphaOffset =
			_colourFlashMaxAlpha / (_colourFlashDuration / 2.f) * rawDelta;
		if (delta > _colourFlashDuration * 0.5f)
			alphaOffset = -alphaOffset;
		_colourForFlashA += alphaOffset;
		if (_colourForFlashA < 0.f) _colourForFlashA = 0.f;
		else if (_colourForFlashA > _colourFlashMaxAlpha)
			_colourForFlashA = _colourFlashMaxAlpha;
		_colourForFlash.a = static_cast<sf::Uint8>(_colourForFlashA);
		_colourFlash.setFillColor(_colourForFlash);
		_colourFlash.setSize(sf::Vector2f(_renderer->getSize()));
		_renderer->draw(_colourFlash);
	}
}

void awe::game_engine::_animateGUITransition() {
	if (_guiTransition) {
		const auto finished = _guiTransition->animate(*_renderer);
		if (finished && !_guiTransition->isFadingIn()) {
			_gui->setGUI(_transitionToGUIMenu);
			_guiTransition = std::make_unique<awe::transition::rectangle>(true,
				_guiTransitionDuration);
			_guiTransition->animate(*_renderer);
		} else if (finished) {
			_guiTransition = nullptr;
			_transitionToGUIMenu.clear();
		}
	} else if (!_transitionToGUIMenu.empty()) _guiTransition =
		std::make_unique<awe::transition::rectangle>(false,
			_guiTransitionDuration);
}

// Script interface.

void awe::game_engine::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	sfx::joystick::Register(engine, document);
	engine::RegisterVectorTypes(engine, document);
	engine::RegisterTimeTypes(engine, document);
	engine::RegisterColourType(engine, document);
	awe::map::Register(engine, document);

	if (!engine->GetTypeInfoByName("AnimationPreset")) {
		auto r = engine->RegisterEnum("AnimationPreset");
		engine->RegisterEnumValue("AnimationPreset", "VisualA",
			static_cast<int>(awe::animation_preset::VisualA));
		engine->RegisterEnumValue("AnimationPreset", "VisualB",
			static_cast<int>(awe::animation_preset::VisualB));
		engine->RegisterEnumValue("AnimationPreset", "VisualC",
			static_cast<int>(awe::animation_preset::VisualC));
		engine->RegisterEnumValue("AnimationPreset", "VisualD",
			static_cast<int>(awe::animation_preset::VisualD));
		engine->RegisterEnumValue("AnimationPreset", "NoVisual",
			static_cast<int>(awe::animation_preset::NoVisual));
		document->DocumentObjectEnum(r, "Represents a map animation preset.");
	}

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

	r = engine->RegisterGlobalFunction("void critical(const string&in)",
		asMETHOD(engine::scripts, criticalToLog),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Writes to the log using the critical "
		"level.");

	r = engine->RegisterGlobalFunction("void stacktrace()",
		asMETHOD(engine::scripts, stacktraceToLog),
		asCALL_THISCALL_ASGLOBAL, _scripts.get());
	document->DocumentGlobalFunction(r, "Writes the stacktrace to the log.");

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

	r = engine->RegisterGlobalFunction("Map@ createMap(const string&in, "
		"const string&in)",
		asMETHOD(awe::game_engine, _script_createMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Saves a blank map (its file path being "
		"the first parameter), then loads it, and returns a handle to it, if "
		"successful. If there is already a map open at the time of the call, then "
		"an error will be logged and no changes will occur. A null handle will be "
		"returned if the map couldn't be created!\n"
		"The second string parameter must be the name of the class defined by the "
		"scripts that represents a playable map.");

	r = engine->RegisterGlobalFunction("Map@ loadMap(const string&in, "
		"const string&in)",
		asMETHOD(awe::game_engine, _script_loadMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Opens a map (its file path being the "
		"first parameter), and returns a handle to it if it could be loaded. If "
		"there is already a map open at the time of the call, then an error will "
		"be logged and no changes will occur. A null handle will be returned if "
		"the map couldn't be loaded!\n"
		"The second string parameter must be the name of the class defined by the "
		"scripts that represents a playable map.");

	r = engine->RegisterGlobalFunction("void quitMap(const string&in)",
		asMETHOD(awe::game_engine, _script_quitMap),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes the currently open map and "
		"switches to the specified menu.");

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

	r = engine->RegisterGlobalFunction(
		"string translate(const string&in, array<any>@ = null)",
		asMETHOD(awe::game_engine, _script_translate),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Translates a string with or without "
		"variables.");

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

	r = engine->RegisterGlobalFunction("float frand(const float)",
		asMETHOD(awe::game_engine, _script_frand),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Generates a random number between 0.0 "
		"and the given value (inclusive).");

	r = engine->RegisterGlobalFunction("bool doesPathExist(const string&in)",
		asMETHOD(awe::game_engine, _script_doesPathExist),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if the given path "
		"is either a file or directory that exists. <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction("string getLatestLogEntry()",
		asMETHOD(awe::game_engine, _script_getLatestLogEntry),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "The latest line written in the log.");

	r = engine->RegisterGlobalFunction("string formatBool(const bool)",
		asMETHOD(awe::game_engine, _script_formatBool),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Converts a bool into a string.");

	r = engine->RegisterGlobalFunction(
		"void flashColour(const Colour&in, const float = 2.0)",
		asMETHOD(awe::game_engine, _script_flashColour),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"void transitionToGUI(const string&in, const float = 1.0)",
		asMETHOD(awe::game_engine, _script_transitionToGUI),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"void play(const string&in, const string&in, const float = 1.0)",
		asMETHOD(awe::game_engine, _script_play),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"void stop(const string&in, const float = 1.0)",
		asMETHOD(awe::game_engine, _script_stop),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"void setSelectedMapAnimationPreset(const AnimationPreset)",
		asMETHOD(awe::game_engine, _script_setSelectedMapAnimationPreset),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"AnimationPreset getSelectedMapAnimationPreset()",
		asMETHOD(awe::game_engine, _script_getSelectedMapAnimationPreset),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"AnimationPreset selectNextMapAnimationPreset()",
		asMETHOD(awe::game_engine, _script_selectNextMapAnimationPreset),
		asCALL_THISCALL_ASGLOBAL, this);

	r = engine->RegisterGlobalFunction(
		"void saveGlobalSettings()",
		asMETHOD(awe::game_engine, _script_saveGlobalSettings),
		asCALL_THISCALL_ASGLOBAL, this);
}

bool awe::game_engine::_load(engine::json& j) {
	// Find the base path of the assets folder and make it the CWD.
	std::filesystem::path basePath = getScriptPath();
	basePath = basePath.parent_path();
	std::filesystem::current_path(basePath);
	// Load the renderer as soon as possible, so that we can display the logs.
	// Additionally, opening the renderer now will prevent glFlush() SFML errors
	// from plaguing standard output when we load images in the
	// animated_spritesheet objects.
	if (!_loadObject(_renderer, j, { "renderer" }, true,
		engine::logger::data{ _logger.getData().sink, "renderer" })) return false;
	_renderer->openWindow();
	// Load the fonts now so that we can use them when printing the logs.
	if (!_loadObject(_fonts, j, { "fonts" }, true,
		engine::logger::data{ _logger.getData().sink, "fonts" })) return false;
	// Create a list of load operations to carry out. Functions must return FALSE
	// when the load operation failed, TRUE if it succeeded.
	std::forward_list<std::function<bool(void)>> loadOperations = {
		[&]() {
			return _loadObject(_dictionary, j, { "languages" }, true,
				engine::logger::data{ _logger.getData().sink,
					"language_dictionary" });
		},
		[&]() {
			return _loadObject(_audios, j, { "audios" }, true,
				engine::logger::data{ _logger.getData().sink, "audios" });
		},
		[&]() {
			// Allocate GUI and scripts objects, but don't initialise yet.
			_scripts = std::make_shared<engine::scripts>(
				engine::logger::data{ _logger.getData().sink, "scripts" });
			_gui = std::make_shared<sfx::gui>(_scripts,
				engine::logger::data{_logger.getData().sink, "gui"});
			return _scripts && _gui;
		},
		[&]() {
			return _loadObject(_userinput, j, { "userinput" }, true,
				engine::logger::data{ _logger.getData().sink, "user_input" });
		},
		[&]() {
			return _loadObject(_sprites, j, { "spritesheets" }, true,
				engine::logger::data{ _logger.getData().sink, "spritesheet" });
		},
		[&]() {
			return _loadObject(_countries, j, { "countries" }, true, _scripts,
				"Country",
				engine::logger::data{ _logger.getData().sink, "country_bank" });
		},
		[&]() {
			return _loadObject(_weathers, j, { "weathers" }, true, _scripts,
				"Weather",
				engine::logger::data{ _logger.getData().sink, "weather_bank" });
		},
		[&]() {
			return _loadObject(_environments, j, { "environments" }, true,
				_scripts, "Environment",
				engine::logger::data{ _logger.getData().sink,
				"environment_bank" });
		},
		[&]() {
			return _loadObject(_movements, j, { "movements" }, true, _scripts,
				"Movement", engine::logger::data{ _logger.getData().sink,
				"movement_bank" });
		},
		[&]() {
			return _loadObject(_terrains, j, { "terrains" }, true, _scripts,
				"Terrain",
				engine::logger::data{ _logger.getData().sink, "terrain_bank" });
		},
		[&]() {
			return _loadObject(_tiles, j, { "tiles" }, true, _scripts, "TileType",
				engine::logger::data{ _logger.getData().sink, "tile_bank" });
		},
		[&]() {
			return _loadObject(_weapons, j, { "weapons" }, true, _scripts,
				"Weapon",
				engine::logger::data{ _logger.getData().sink, "weapon_bank" });
		},
		[&]() {
			return _loadObject(_units, j, { "units" }, true, _scripts, "UnitType",
				engine::logger::data{ _logger.getData().sink, "unit_bank" });
		},
		[&]() {
			return _loadObject(_commanders, j, { "commanders" }, true, _scripts,
				"Commander", engine::logger::data{ _logger.getData().sink,
				"commander_bank" });
		},
		[&]() {
			return _loadObject(_structures, j, { "structures" }, true, _scripts,
				"Structure", engine::logger::data{ _logger.getData().sink,
				"structure_bank" });
		},
		[&]() {
			return _loadObject(_mapStrings, j, { "mapstrings" }, true,
				engine::logger::data{ _logger.getData().sink, "map_strings" });
		},
		[&]() {
			// Finish initialisation of banks.
			if (!awe::checkCountryTurnOrderIDs(*_countries)) {
				_logger.critical("The turn order IDs assigned to each configured "
					"country are not valid. See the log for more information.");
				for (const auto& pCountry : *_countries) {
					_logger.error("Turn order ID for country {} = {}",
						pCountry.first, pCountry.second->getTurnOrder());
				}
				return false;
			}
			return true;
		},
		[&]() {
			awe::updateTileTypeBank(*_tiles, *_terrains, *_countries, *_structures,
					_scripts);
			return true;
		},
		[&]() {
			awe::updateTerrainBank(*_terrains, *_countries, *_tiles);
			return true;
		},
		[&]() {
			awe::updateUnitTypeBank(*_units, *_movements, *_terrains, *_weapons,
				*_countries, _logger.getData().sink);
			return true;
		},
		[&]() {
			awe::updateStructureBank(*_structures, *_tiles, *_countries);
			return true;
		},
		[&]() {
			_scripts->addRegistrant(this);
			return _loadObject(_scripts, j, { "scripts" }, false,
				engine::logger::data{ _logger.getData().sink, "scripts" });
		},
		[&]() {
			_scripts->generateDocumentation();
			return true;
		},
		[&]() {
			std::string guiPath;
			j.apply(guiPath, { "gui" });
			if (!j.inGoodState()) return false;
			_gui->setSpritesheets(_sprites);
			_gui->setLanguageDictionary(_dictionary);
			_gui->setFonts(_fonts);
			_gui->setAudios(_audios);
			_gui->setTarget(*_renderer);
			_gui->load(guiPath);
			return _gui->inGoodState();
		},
		[&]() {
			_userinput->tieWindow(_renderer);
			_userinput->setGUI(_gui);
			return true;
		},
		[&]() {
			return _loadObject(_settings, j, { "settings" }, true,
				engine::logger::data{ _logger.getData().sink, "settings" },
				_audios, _dictionary);
		}
	};
	// Render the logs to the screen in between each load operation.
	const auto font = (*_fonts)["Monospace"]; // TODO-2.
	sf::Text logs("", *font, 16);
	logs.setPosition(5.f, 5.f);
	const sf::View old = _renderer->getView();
	bool failed = false; // Lets us display the logs one last time before breaking.
	for (const auto& loadOperation : loadOperations) {
		_renderer->handleEvents([&](const sf::Event& e) {
			if (e.type == sf::Event::Closed) throw load_cancelled();
		});
		_renderer->clear();
		sf::View v(sf::FloatRect(0.0f, 0.0f,
			static_cast<float>(_renderer->getSize().x),
			static_cast<float>(_renderer->getSize().y)));
		v.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
		_renderer->setView(v);
		logs.setString(_logger.getData().sink->getLog());
		_renderer->draw(logs, sf::Transform().translate(0.f,
			-std::max(0.f, logs.getGlobalBounds().height -
				(static_cast<float>(_renderer->getSize().y)) + 5.f)));
		_renderer->display();
		if (failed) break;
		if (!loadOperation()) failed = true;
	}
	_renderer->setView(old);
	return !failed;
}

bool awe::game_engine::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

int awe::game_engine::_initCheck() const {
	std::string errstring = "";
	if (!_countries) errstring += "countries\n";
	if (!_weathers) errstring += "weathers\n";
	if (!_environments) errstring += "environments\n";
	if (!_movements) errstring += "movements\n";
	if (!_terrains) errstring += "terrains\n";
	if (!_tiles) errstring += "tiles\n";
	if (!_units) errstring += "units\n";
	if (!_commanders) errstring += "commanders\n";
	if (!_structures) errstring += "structures\n";
	if (!_mapStrings) errstring += "map strings\n";
	if (!_dictionary) errstring += "dictionary\n";
	if (!_fonts) errstring += "fonts\n";
	if (!_audios) errstring += "audios\n";
	if (!_renderer) errstring += "renderer\n";
	if (!_userinput) errstring += "userinput\n";
	if (!_sprites) errstring += "spritesheets\n";
	if (!_scripts) errstring += "scripts\n";
	if (!_gui) errstring += "gui\n";
	if (!_settings) errstring += "settings\n";
	if (errstring.length()) {
		_logger.critical("Fatal error: could not run the game engine due to the "
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

awe::map* awe::game_engine::_script_createMap(const std::string& file,
	const std::string& playableMapTypeName) {
	if (_map) {
		_logger.error("Attempted to create a map file \"{}\" whilst map \"{}\" "
			"was still loaded!", file, _map->getMapName());
		return nullptr;
	}
	// Create a blank map, save it, then load it using _script_loadMap().
	try {
		_map = std::make_unique<awe::map>(_countries, _environments, _weathers,
			_tiles, _terrains, _units, _commanders, _structures,
			engine::logger::data{ _logger.getData().sink, "map" });
	} catch (const std::bad_alloc& e) {
		_logger.error("Couldn't allocate the map object for creation: {}", e);
		return nullptr;
	}
	_map->setScripts(_scripts);
	if (_map->save(file)) {
		_map = nullptr;
		return _script_loadMap(file, playableMapTypeName);
	} else {
		_map = nullptr;
		_logger.error("Couldn't create map file \"{}\".", file);
		return nullptr;
	}
}

awe::map* awe::game_engine::_script_loadMap(const std::string& file,
	const std::string& playableMapTypeName) {
	// Create the game.
	if (_map) {
		_logger.error("Attempted to load map file \"{}\" whilst map \"{}\" was "
			"still loaded!", file, _map->getMapName());
		return nullptr;
	} else {
		try {
			_map = std::make_unique<awe::map>( _countries, _environments,
				_weathers, _tiles, _terrains, _units, _commanders, _structures,
				engine::logger::data{_logger.getData().sink, "map"});
		} catch (const std::bad_alloc& e) {
			_logger.error("Couldn't allocate the map object for loading: {}", e);
			return nullptr;
		}
		_map->setMapObjectType(playableMapTypeName);
		_map->setTarget(_renderer);
		_map->setSpritesheets(_sprites);
		_map->setFonts(_fonts);
		_map->setAudios(_audios);
		_map->setLanguageDictionary(_dictionary);
		_map->setMapStrings(_mapStrings);
		_map->setScripts(_scripts);
		_map->setGUI(_gui);
		_map->setUserInput(_userinput);
		_map->setSelectedAnimationPreset(_settings->getAnimationPreset());
		auto r = _map->load(file);
		if (r) {
			return _map.get();
		} else {
			_map = nullptr;
			_logger.error("Couldn't load map file \"{}\".", file);
			return nullptr;
		}
	}
}

void awe::game_engine::_script_quitMap(const std::string& menu) {
	_map = nullptr;
	_gui->setGUI(menu);
}

std::string awe::game_engine::_script_translate(const std::string& nativeString,
	CScriptArray* variables) const {
	if (variables) {
		std::string translatedString = (*_dictionary)(nativeString);
		for (asUINT i = 0, len = variables->GetSize(); i < len; ++i) {
			CScriptAny* var = static_cast<CScriptAny*>(variables->At(i));
			auto type = var->GetTypeId();
			if (type == _scripts->getTypeID("int64")) {
				asINT64 val = 0;
				var->Retrieve(val);
				translatedString =
					engine::expand_string::insert(translatedString, val);
			} else if (type == _scripts->getTypeID("double")) {
				double val = 0.0;
				var->Retrieve(val);
				translatedString =
					engine::expand_string::insert(translatedString, val);
			} else if (type == _scripts->getTypeID("string")) {
				std::string val;
				var->Retrieve(&val, type);
				translatedString =
					engine::expand_string::insert(translatedString, val);
			} else {
				_logger.warning("Unsupported type \"{}\" given when translating "
					"string \"{}\": inserting blank string instead.",
					_scripts->getTypeName(type), nativeString);
				translatedString =
					engine::expand_string::insert(translatedString, "");
			}
		}
		variables->Release();
		return translatedString;
	} else return (*_dictionary)(nativeString);
}

sf::Vector2i awe::game_engine::_script_scaledMousePosition() const {
	auto ret = _userinput->mousePosition();
	if (ret != sfx::INVALID_MOUSE) {
		return { (int)((float)ret.x / _scaling), (int)((float)ret.y / _scaling) };
	} else {
		return ret;
	}
}

unsigned int awe::game_engine::_script_rand(const unsigned int max) {
	// Credit: https://stackoverflow.com/a/13446015/6928376.
	std::uniform_int_distribution<unsigned> distribution(0, max);
	return distribution(*_prng);
}

float awe::game_engine::_script_frand(const float max) {
	std::uniform_real_distribution<float> distribution(0.0f, max);
	return distribution(*_prng);
}

bool awe::game_engine::_script_doesPathExist(const std::string& path) const {
	return std::filesystem::exists(path);
}

std::string awe::game_engine::_script_getLatestLogEntry() const {
	const auto log = _logger.getData().sink->getLog();
	return log.substr(log.rfind("\n", log.size() - 2) + 1);
}

std::string awe::game_engine::_script_formatBool(const bool b) const {
	return b ? "true" : "false";
}

void awe::game_engine::_script_flashColour(const sf::Color& c, const float d) {
	if (accumulatedDelta() < _colourFlashDuration) return;
	_colourForFlash = c;
	_colourForFlashA = 0.0f;
	_colourFlashMaxAlpha = c.a;
	_colourFlashDuration = d;
	resetDeltaAccumulation();
}

void awe::game_engine::_script_transitionToGUI(const std::string& menu,
	const float duration) {
	if (_guiTransition) return;
	_transitionToGUIMenu = menu;
	_guiTransitionDuration = sf::seconds(duration * 0.5f);
}

void awe::game_engine::_script_play(const std::string& audioObject,
	const std::string& audioName, const float duration) {
	if (!_audios->exists(audioObject)) {
		_logger.error("Attempted to play piece of audio \"{}\" from audio object "
			"\"{}\", the latter of which does not exist.", audioName, audioObject);
		return;
	}
	(*_audios)[audioObject]->play(audioName, sf::seconds(duration));
}

void awe::game_engine::_script_stop(const std::string& audioObject,
	const float duration) {
	if (!_audios->exists(audioObject)) {
		_logger.error("Attempted to stop the currently playing music from audio "
			"object \"{}\", which does not exist.", audioObject);
		return;
	}
	(*_audios)[audioObject]->stop(sf::seconds(duration));
}

void awe::game_engine::_script_setSelectedMapAnimationPreset(
	const awe::animation_preset p) {
	_settings->setAnimationPreset(p);
	if (_map) _map->setSelectedAnimationPreset(p);
}

awe::animation_preset
	awe::game_engine::_script_getSelectedMapAnimationPreset() const {
	return _settings->getAnimationPreset();
}

awe::animation_preset awe::game_engine::_script_selectNextMapAnimationPreset() {
	auto p = _settings->getAnimationPreset();
	_script_setSelectedMapAnimationPreset(++p);
	return p;
}

void awe::game_engine::_script_saveGlobalSettings() {
	_settings->save();
}
