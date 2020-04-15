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

awe::game::game(const std::string& name) noexcept :
		_logger(name), _audio_Sound("audio_sound"), _audio_Music("audio_music"), _renderer("renderer"),
		_language_GUI("language_gui"), _language_Game("language_game"), _language_Dialogue("language_dialogue"),
		_spritesheet_GUI("spritesheet_gui"), _spritesheet_CO("spritesheet_co"), _spritesheet_Unit("spritesheet_unit"),
		_spritesheet_Tile("spritesheet_tile"), _userinput(_renderer) {
	// load JSON configurations for each backend object
	_audio_Sound.load("assets/audio/sound/audiosound.json");
	_audio_Music.load("assets/audio/music/audiomusic.json");
	_fonts.load("assets/fonts/fonts.json");
	_renderer.load("assets/renderer/renderer.json");
	_spritesheet_GUI.load("assets/sprites/gui/spritesgui.json");
	_spritesheet_CO.load("assets/sprites/co/spritesco.json");
	_spritesheet_Unit.load("assets/sprites/unit/spritesunit.json");
	_spritesheet_Tile.load("assets/sprites/tile/spritestile.json");
	_userinput.load("assets/userinput/userinput.json");
	_gui.load("assets/gui/gui.json");
	// setup translation object
	i18n::translation::setLanguageScriptPath("assets/lang");
	i18n::translation::addLanguageObject("gui", &_language_GUI);
	i18n::translation::addLanguageObject("game", &_language_Game);
	i18n::translation::addLanguageObject("dialogue", &_language_Dialogue);
	i18n::translation::setLanguage("GB");
	// setup GUI objects
	_gui.setTarget(_renderer);
	// construction completed
	_state = awe::game::state::MainMenu;
}

int awe::game::run() noexcept {
	_renderer.openWindow();

	_gui.setGUI("main");

	while (_state != awe::game::state::Terminated) {
		_userinput.update();

		if (_userinput["up"]) std::cout << "up\n";
		if (_userinput["down"]) std::cout << "down\n";
		if (_userinput["left"]) std::cout << "left\n";
		if (_userinput["right"]) std::cout << "right\n";
		if (_userinput["select"]) {
			std::cout << "select\n";
			_gui.setGUI("settings");
		}
		if (_userinput["back"]) {
			std::cout << "back\n";
			_gui.setGUI("main");
		}

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
		_state = awe::game::state::Terminated;
	}
}

void awe::game::_rendererRendering() noexcept {
	_gui.drawBackground(&_spritesheet_GUI);
	_gui.drawForeground(&_spritesheet_GUI);
	_renderer.display();
}