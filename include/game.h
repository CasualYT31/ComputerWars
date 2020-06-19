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

#pragma once

#include "audio.h"
#include "fonts.h"
#include "language.h"
#include "renderer.h"
#include "userinput.h"
#include "gui.h"
#include "terrain.h"
#include "unit.h"

namespace awe {
	class game : sf::NonCopyable {
	public:
		static const char* STATE_TERMINATED;

		game(const std::string& scriptsFolder = "assets/script",
			const std::string& JSON_AudioSound = "assets/audio/sound/audiosound.json",
			const std::string& JSON_AudioMusic = "assets/audio/music/audiomusic.json",
			const std::string& JSON_Fonts = "assets/fonts/fonts.json",
			const std::string& JSON_Renderer = "assets/renderer/renderer.json",
			const std::string& JSON_Spritesheet_GUI = "assets/sprites/gui/spritesgui.json",
			const std::string& JSON_Spritesheet_CO = "assets/sprites/gui/spritesco.json",
			const std::string& JSON_UserInput = "assets/userinput/userinput.json",
			const std::string& JSON_GUI = "assets/gui/gui.json",
			const std::string& JSON_LanguageFolder = "assets/lang",
			const std::string& JSON_Countries = "assets/property/country.json",
			const std::string& JSON_Weathers = "assets/property/weather.json",
			const std::string& JSON_Environments = "assets/property/environment.json",
			const std::string& JSON_Movements = "assets/property/movement.json",
			const std::string& JSON_Terrains = "assets/property/terrain.json",
			const std::string& JSON_Tiles = "assets/property/tile.json",
			const std::string& JSON_Units = "assets/property/unit.json",
			const std::string& JSON_COs = "assets/property/co.json",
			const std::string& name = "game_loop") noexcept;
		~game() noexcept;

		int run() noexcept;
	private:
		//================================
		//==========LOOP MODULES==========
		//================================
		// loop modules
		void _rendererEventPolling() noexcept;
		void _rendererRendering() noexcept;
		// state monitoring methods
		void _checkTerminated(sf::Event& event) noexcept;
		//================================
		//========== STATE DATA ==========
		//================================
		// state
		std::string _state = "main";
		//================================
		//==========  GAME DATA ==========
		//================================
		// game properties
		awe::bank<awe::country> _countryBank;
		awe::bank<awe::weather> _weatherBank;
		awe::bank<awe::environment> _environmentBank;
		awe::bank<awe::movement_type> _movementBank;
		awe::bank<awe::terrain> _terrainBank;
		awe::bank<awe::tile_type> _tileBank;
		awe::bank<awe::unit_type> _unitBank;
		awe::bank<awe::commander> _commanderBank;
		//================================
		//==========BACKEND DATA==========
		//================================
		// logger
		global::logger _logger;
		// audio objects
		sfx::audio _audio_Sound;
		sfx::audio _audio_Music;
		// fonts object
		sfx::fonts _fonts;
		// language objects
		i18n::language _language_GUI;
		i18n::language _language_Game;
		i18n::language _language_Dialogue;
		// renderer object
		sfx::renderer _renderer;
		// spritesheet objects
		sfx::spritesheet _spritesheet_GUI;
		sfx::spritesheet _spritesheet_CO;
		// user input objects
		sfx::user_input _userinput;
		// scripts object
		engine::scripts _scripts;
		// GUI object
		engine::gui _gui;
	};
}