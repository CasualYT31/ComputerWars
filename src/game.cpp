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

#include "game.h"
#include <iostream>

const char* awe::game::STATE_TERMINATED = "terminated";

awe::game::game(const std::string& scriptsFolder,
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
		_scripts(scriptsFolder), _gui(&_scripts) {
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

awe::game::~game() noexcept {
	_dictionary.save();
}

int awe::game::run() noexcept {
	_renderer.openWindow();

	while (_state != awe::game::STATE_TERMINATED) {
		if (_state != _gui.getGUI()) _gui.setGUI(_state);

		_userinput.update();

		_rendererEventPolling();

		_rendererRendering();
	}

	_renderer.close();
	return 0;
}

void awe::game::_rendererEventPolling() noexcept {
	sf::Event event;
	while (_renderer.pollEvent(event)) {
		_checkTerminated(event);
		_gui.handleEvent(event);
	}
}

void awe::game::_checkTerminated(sf::Event& event) noexcept {
	if (event.type == sf::Event::Closed) {
		_state = awe::game::STATE_TERMINATED;
	}
}

void awe::game::_rendererRendering() noexcept {
	_gui.drawBackground(&_spritesheet_GUI);
	_gui.drawForeground(&_spritesheet_GUI);
	_renderer.display();
}