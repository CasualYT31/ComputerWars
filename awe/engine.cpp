/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

/*// INCOMPLETE: functionality will be added as and when necessary, print methods
// will be removed at some point
void engine::scripts::_registerInterface() noexcept {
    _logger.write("Registering script interface...");
    _engine->RegisterGlobalFunction("void print(const string &in)",
        asFUNCTION(print), asCALL_CDECL);
    _engine->RegisterGlobalFunction("void printno(const int)",
        asFUNCTION(printNumber), asCALL_CDECL);
    _engine->RegisterGlobalFunction("void printfloat(const float)",
        asFUNCTION(printFloat), asCALL_CDECL);
    _logger.write("Finished registering script interface.");
}*/

awe::game_engine::game_engine(const std::string& name) noexcept : _logger(name) {}

int awe::game_engine::run(const std::string& file) noexcept {
	auto r = _initCheck();
	if (r) return r;

	awe::map map(file, _countries, _tiles, _units);
	map.selectTile(sf::Vector2u(0, 0));
	map.selectArmy(0);
	map.setVisiblePortionOfMap(sf::Rect<sf::Uint32>(0, 0, map.getMapSize().x,
		map.getMapSize().y));
	map.setTileSpritesheet(_sprites->tile->normal);
	map.setUnitSpritesheet(_sprites->unit->idle);

	// test scripts
	_scripts->callFunction("main");

	try {
		while (_renderer->isOpen()) {
			sf::Event event;
			while (_renderer->pollEvent(event)) {
				if (event.type == sf::Event::Closed) _renderer->close();
			}
			_renderer->clear();
			_renderer->animate(map);
			_renderer->draw(map);
			_renderer->display();
		}
	} catch (std::exception&) {
		
	}

	return 0;
}

// script interface

void awe::game_engine::initialiseScripts(const std::string& folder) noexcept {
	if (_scripts) {
		_scripts->registerInterface(
			std::bind(&awe::game_engine::_registerInterface, this,
				std::placeholders::_1));
		_scripts->loadScripts(folder);
	} else {
		_logger.error("initialiseScripts() was called before setting a scripts "
			"object!");
	}
}

void awe::game_engine::_registerInterface(asIScriptEngine* engine) noexcept {
	// register the global functions
	engine->RegisterGlobalFunction("void info(const string& in)",
		asMETHOD(awe::game_engine, _script_info), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void warn(const string& in)",
		asMETHOD(awe::game_engine, _script_warn), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void error(const string& in)",
		asMETHOD(awe::game_engine, _script_error), asCALL_THISCALL_ASGLOBAL, this);
}

void awe::game_engine::_script_info(std::string& in) {
	_logger.write(in);
}

void awe::game_engine::_script_warn(std::string& in) {
	_logger.warning(in);
}

void awe::game_engine::_script_error(std::string& in) {
	_logger.error(in);
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
}

void awe::game_engine::setUserInput(const std::shared_ptr<sfx::user_input>& ptr)
	noexcept {
	_userinput = ptr;
}

void awe::game_engine::setSpritesheets(
	const std::shared_ptr<awe::spritesheets>& ptr) noexcept {
	_sprites = ptr;
}

void awe::game_engine::setScripts(const std::shared_ptr<engine::scripts>& ptr)
	noexcept {
	_scripts = ptr;
}

void awe::game_engine::setGUI(const std::shared_ptr<sfx::gui>& ptr) noexcept {
	_gui = ptr;
}