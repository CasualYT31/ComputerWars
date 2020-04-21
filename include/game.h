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
#include "texture.h"
#include "userinput.h"
#include "gui.h"
#include "script.h"
#include "terrain.h"
#include "unit.h"

namespace awe {
	class game : sf::NonCopyable {
	public:
		static const char* STATE_TERMINATED;

		game(const std::string& name = "game_loop") noexcept;
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
		awe::country _countries;
		awe::weather _weathers;
		awe::environment _environments;
		awe::movement _movements;
		// terrain-related data
		awe::terrain_bank _terrainTypeBank;
		awe::terrain_tile_bank _terrainTileBank;
		// unit-related data
		awe::unit_bank _unitTypeBank;
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
		sfx::spritesheet _spritesheet_Unit;
		sfx::spritesheet _spritesheet_Tile;
		// user input objects
		sfx::user_input _userinput;
		// scripts object
		awe::scripts _scripts;
		// GUI object
		awe::gui _gui;
	};
}