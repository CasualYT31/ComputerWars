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

	awe::game game;
	game.setCountries(_countries);
	game.setCommanders(_commanders);
	game.setTiles(_tiles);
	game.setUnits(_units);
	game.setSpritesheets(_sprites);

	try {
		game.read("assets/map/test.map");
		
		// test create unit
		auto pUnit = game.createUnit(game.getArmy(0), (*_units)[0], sf::Vector2u(0, 0));
		if (pUnit) {
			// expected values:
			// army holds reference to unit
			// unit holds reference to army
			// tile holds reference to unit
			// unit holds reference to tile
			_logger.write("{}", game.getArmy(0)->isArmysUnit(pUnit));
			_logger.write("{}", pUnit->getOwner().lock() == game.getArmy(0));
			_logger.write("{}", game.getMap()->getTile(sf::Vector2u(0, 0)) == pUnit->getTile().lock());
			// HOLY SHIT IT WORKS?????!!?!?!?
		}

		// test delete unit
		game.deleteUnit(pUnit);
		pUnit = nullptr; // createUnit() didn't work the first time because we still held a reference to the unit, so the weak reference at the tile still pointed to a valid unit
		pUnit = game.createUnit(game.getArmy(0), (*_units)[1], sf::Vector2u(0, 0));

		// test changeTileOwner
		_logger.write("{}", game.getMap()->getTile(sf::Vector2u(0, 0))->getOwner().expired());
		game.changeTileOwner(game.getMap()->getTile(sf::Vector2u(0, 0)), game.getArmy(0));
		_logger.write("{}", game.getMap()->getTile(sf::Vector2u(0, 0))->getOwner().expired());
		_logger.write("{}", game.getMap()->getTile(sf::Vector2u(0, 0))->getOwner().expired());
		game.changeTileOwner(game.getMap()->getTile(sf::Vector2u(0, 0)), nullptr);
		_logger.write("{}", game.getMap()->getTile(sf::Vector2u(0, 0))->getOwner().expired());

		// test moveUnit
		_logger.write("{}", game.moveUnit(pUnit, sf::Vector2u(0, 0)));
		_logger.write("{}", game.moveUnit(pUnit, sf::Vector2u(0, 1)));
		// expected values:
		// old tile no longer holds reference to unit
		// new tile holds reference to unit
		// unit holds reference to new tile
		_logger.write("\n{}", game.getMap()->getTile(sf::Vector2u(0, 0))->getUnit().expired());
		_logger.write("\n{}", game.getMap()->getTile(sf::Vector2u(0, 1))->getUnit().lock() == pUnit);
		_logger.write("\n{}", game.getMap()->getTile(sf::Vector2u(0, 1)) == pUnit->getTile().lock());

		// test loadUnit


		// test unloadUnit

	} catch (std::exception&) {
		
	}

	return 0;
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
		_logger.error("Fatal error: could not run the game engine due to the following objects not being allocated correctly:\n{}Game will now shut down.", errstring);
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

void awe::game_engine::setSpritesheets(const std::shared_ptr<awe::spritesheets>& ptr) noexcept {
	_sprites = ptr;
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