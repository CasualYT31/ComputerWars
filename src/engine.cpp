/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

awe::game_engine::game_engine(const std::string& name) noexcept : _logger(name) {}

int awe::game_engine::run() noexcept {
	auto r = _initCheck();
	if (r) return r;



	return 0;
}

int awe::game_engine::_initCheck() const noexcept {
	std::string errstring = "";
	if (!_countries) errstring += "countries, ";
	if (!_weathers) errstring += "weathers, ";
	if (!_environments) errstring += "environments, ";
	if (!_movements) errstring += "movements, ";
	if (!_terrains) errstring += "terrains, ";
	if (!_tiles) errstring += "tiles, ";
	if (!_units) errstring += "units, ";
	if (!_commanders) errstring += "commanders, ";
	if (!_dictionary) errstring += "dictionary, ";
	if (!_fonts) errstring += "fonts, ";
	if (!_sounds) errstring += "sounds, ";
	if (!_music) errstring += "music, ";
	if (!_renderer) errstring += "renderer, ";
	if (!_userinput) errstring += "userinput, ";
	if (!_scripts) errstring += "scripts, ";
	if (!_gui) errstring += "gui, ";
	if (errstring.length()) {
		errstring.at(errstring.length() - 2) = '.';
		_logger.error("Fatal error: could not run the game engine due to the following objects not being allocated correctly: {}Game will now shutdown.", errstring);
		return 1;
	}
	return 0;
}


// set methods

void awe::game_engine::setCountries(const std::shared_ptr<awe::bank<const awe::country>>& ptr) noexcept {
	_countries = ptr;
}

void awe::game_engine::setWeathers(const std::shared_ptr<awe::bank<const awe::weather>>& ptr) noexcept {
	_weathers = ptr;
}

void awe::game_engine::setEnvironments(const std::shared_ptr<awe::bank<const awe::environment>>& ptr) noexcept {
	_environments = ptr;
}

void awe::game_engine::setMovements(const std::shared_ptr<awe::bank<const awe::movement_type>>& ptr) noexcept {
	_movements = ptr;
}

void awe::game_engine::setTerrains(const std::shared_ptr<awe::bank<const awe::terrain>>& ptr) noexcept {
	_terrains = ptr;
}

void awe::game_engine::setTiles(const std::shared_ptr<awe::bank<const awe::tile_type>>& ptr) noexcept {
	_tiles = ptr;
}

void awe::game_engine::setUnits(const std::shared_ptr<awe::bank<const awe::unit_type>>& ptr) noexcept {
	_units = ptr;
}

void awe::game_engine::setCommanders(const std::shared_ptr<awe::bank<const awe::commander>>& ptr) noexcept {
	_commanders = ptr;
}

void awe::game_engine::setDictionary(const std::shared_ptr<i18n::language_dictionary>& ptr) noexcept {
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

void awe::game_engine::setRenderer(const std::shared_ptr<sfx::renderer>& ptr) noexcept {
	_renderer = ptr;
}

void awe::game_engine::setUserInput(const std::shared_ptr<sfx::user_input>& ptr) noexcept {
	_userinput = ptr;
}

void awe::game_engine::setScripts(const std::shared_ptr<engine::scripts>& ptr) noexcept {
	_scripts = ptr;
}

void awe::game_engine::setGUI(const std::shared_ptr<engine::gui>& ptr) noexcept {
	_gui = ptr;
}

// OLD
/*
const char* awe::game_engine::STATE_TERMINATED = "terminated";

awe::game_engine::game_engine(const std::string& scriptsFolder,
			const std::string& JSON_AudioSound,
			const std::string& JSON_AudioMusic,
			const std::string& JSON_Fonts,
			const std::string& JSON_Renderer,
			const std::string& JSON_Spritesheet_GUI,
			const std::string& JSON_Spritesheet_CO,
			const std::string& JSON_UserInput,
			const std::string& JSON_GUI,
			const std::string& JSON_Language,
			const std::string& JSON_Countries,
			const std::string& JSON_Weathers,
			const std::string& JSON_Environments,
			const std::string& JSON_Movements,
			const std::string& JSON_Terrains,
			const std::string& JSON_Tiles,
			const std::string& JSON_Units,
			const std::string& JSON_COs,
			const std::string& name) noexcept :
		_logger(name), _audio_Sound("audio_sound"), _audio_Music("audio_music"), _renderer("renderer"),
		_spritesheet_GUI("spritesheet_gui"), _spritesheet_CO("spritesheet_co"), _userinput(_renderer),
		_scripts(std::make_shared<engine::scripts>(scriptsFolder)), _gui(_scripts) {
	// load JSON configurations for each backend object
	_audio_Sound.load(JSON_AudioSound);
	_audio_Music.load(JSON_AudioMusic);
	_fonts.load(JSON_Fonts);
	_renderer.load(JSON_Renderer);
	_spritesheet_GUI.load(JSON_Spritesheet_GUI);
	_spritesheet_CO.load(JSON_Spritesheet_CO);
	_userinput.load(JSON_UserInput);
	_gui.load(JSON_GUI);
	_dictionary.load(JSON_Language);
	// load JSON configurations for each property bank
	_countryBank.load(JSON_Countries);
	_weatherBank.load(JSON_Weathers);
	_environmentBank.load(JSON_Environments);
	_movementBank.load(JSON_Movements);
	_terrainBank.load(JSON_Terrains);
	_tileBank.load(JSON_Tiles);
	_unitBank.load(JSON_Units);
	_commanderBank.load(JSON_COs);
	awe::updateAllTerrains(_tileBank, _terrainBank);
	// setup GUI object
	_gui.setTarget(_renderer);
}

awe::game_engine::~game_engine() noexcept {
	_dictionary.save();
}

int awe::game_engine::run() noexcept {
	_renderer.openWindow();

	while (_state != awe::game_engine::STATE_TERMINATED) {
		if (_state != _gui.getGUI()) _gui.setGUI(_state);

		_userinput.update();

		_rendererEventPolling();

		_rendererRendering();
	}

	_renderer.close();
	return 0;
}

void awe::game_engine::_rendererEventPolling() noexcept {
	sf::Event event;
	while (_renderer.pollEvent(event)) {
		_checkTerminated(event);
		_gui.handleEvent(event);
	}
}

void awe::game_engine::_checkTerminated(sf::Event& event) noexcept {
	if (event.type == sf::Event::Closed) {
		_state = awe::game_engine::STATE_TERMINATED;
	}
}

void awe::game_engine::_rendererRendering() noexcept {
	_renderer.animate(_gui);
	_renderer.draw(_gui);
	_renderer.display();
}*/