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

/**
 * @file engine.h
 * Declares the class which brings together all the others to maintain the engine as it executes.
 */

#pragma once

#include "language.h"
#include "fonts.h"
#include "audio.h"
#include "renderer.h"
#include "userinput.h"
#include "gui.h"
#include "game.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	class game_engine : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "engine."
		 * @sa    \c global::logger
		 */
		game_engine(const std::string& name = "engine") noexcept;

		/**
		 * Executes the game based on given game data.
		 * This method is blocking, meaning that it will not return so long as the game is still running.
		 * @return \c 0 upon successful execution, \c !0 upon a fatal error occurring.
		 */
		int run() noexcept;

		/**
		 * Sets the engine's available countries.
		 * @param ptr Pointer to the data.
		 */
		void setCountries(const std::shared_ptr<awe::bank<const awe::country>>& ptr) noexcept;

		/**
		 * Sets the engine's available weather states.
		 * @param ptr Pointer to the data.
		 */
		void setWeathers(const std::shared_ptr<awe::bank<const awe::weather>>& ptr) noexcept;

		/**
		 * Sets the engine's available environments.
		 * @param ptr Pointer to the data.
		 */
		void setEnvironments(const std::shared_ptr<awe::bank<const awe::environment>>& ptr) noexcept;

		/**
		 * Sets the engine's available movement types.
		 * @param ptr Pointer to the data.
		 */
		void setMovements(const std::shared_ptr<awe::bank<const awe::movement_type>>& ptr) noexcept;

		/**
		 * Sets the engine's available terrain types.
		 * @param ptr Pointer to the data.
		 */
		void setTerrains(const std::shared_ptr<awe::bank<const awe::terrain>>& ptr) noexcept;

		/**
		 * Sets the engine's available tiles.
		 * @param ptr Pointer to the data.
		 */
		void setTiles(const std::shared_ptr<awe::bank<const awe::tile_type>>& ptr) noexcept;

		/**
		 * Sets the engine's available unit types.
		 * @param ptr Pointer to the data.
		 */
		void setUnits(const std::shared_ptr<awe::bank<const awe::unit_type>>& ptr) noexcept;

		/**
		 * Sets the engine's available commanders.
		 * @param ptr Pointer to the data.
		 */
		void setCommanders(const std::shared_ptr<awe::bank<const awe::commander>>& ptr) noexcept;

		/**
		 * Sets the engine's available languages.
		 * @param ptr Pointer to the data.
		 */
		void setDictionary(const std::shared_ptr<i18n::language_dictionary>& ptr) noexcept;

		/**
		 * Sets the engine's available fonts.
		 * @param ptr Pointer to the data.
		 */
		void setFonts(const std::shared_ptr<sfx::fonts>& ptr) noexcept;

		/**
		 * Sets the engine's available sounds.
		 * @param ptr Pointer to the data.
		 */
		void setSounds(const std::shared_ptr<sfx::audio>& ptr) noexcept;

		/**
		 * Sets the engine's available BGM.
		 * @param ptr Pointer to the data.
		 */
		void setMusic(const std::shared_ptr<sfx::audio>& ptr) noexcept;

		/**
		 * Sets the engine's renderer.
		 * @param ptr Pointer to the data.
		 */
		void setRenderer(const std::shared_ptr<sfx::renderer>& ptr) noexcept;

		/**
		 * Sets the engine's user input object.
		 * @param ptr Pointer to the data.
		 */
		void setUserInput(const std::shared_ptr<sfx::user_input>& ptr) noexcept;

		/**
		 * Sets the engine's available scripts.
		 * @param ptr Pointer to the data.
		 */
		void setScripts(const std::shared_ptr<engine::scripts>& ptr) noexcept;

		/**
		 * Sets the engine's available menus.
		 * @param ptr Pointer to the data.
		 */
		void setGUI(const std::shared_ptr<engine::gui>& ptr) noexcept;
	private:
		/**
		 * Method called at the start of \c run() to make preliminary checks on internal data.
		 * All shared pointers are tested here: if at least one of them is \c nullptr, \c 1 will be returned.
		 * @return \c 0 if all tests passed, \c 1 if not.
		 */
		int _initCheck() const noexcept;

		//================================
		//==========   GAME DATA==========
		//================================
		/**
		 * Pointer to the static information on all the countries available.
		 */
		std::shared_ptr<awe::bank<const awe::country>> _countries;

		/**
		 * Pointer to the static information on all the types of weather available.
		 */
		std::shared_ptr<awe::bank<const awe::weather>> _weathers;

		/**
		 * Pointer to the static information on all the environments available.
		 */
		std::shared_ptr<awe::bank<const awe::environment>> _environments;

		/**
		 * Pointer to the static information on all the types of movement available.
		 */
		std::shared_ptr<awe::bank<const awe::movement_type>> _movements;

		/**
		 * Pointer to the static information on all the types of terrain available.
		 */
		std::shared_ptr<awe::bank<const awe::terrain>> _terrains;

		/**
		 * Pointer to the static information on all the tiles available.
		 */
		std::shared_ptr<awe::bank<const awe::tile_type>> _tiles;

		/**
		 * Pointer to the static information on all the types of units available.
		 */
		std::shared_ptr<awe::bank<const awe::unit_type>> _units;

		/**
		 * Pointer to the static information on all the commanders available.
		 */
		std::shared_ptr<awe::bank<const awe::commander>> _commanders;
		
		/**
		 * Stores a game's data, including its map and the armies.
		 */
		awe::game _game;

		//================================
		//==========BACKEND DATA==========
		//================================
		/**
		 * The internal logger object.
		 */
		mutable global::logger _logger;
		
		/**
		 * Pointer to the internationalisation object.
		 */
		std::shared_ptr<i18n::language_dictionary> _dictionary;
		
		/**
		 * Pointer to the fonts of the game.
		 */
		std::shared_ptr<sfx::fonts> _fonts;
		
		/**
		 * Pointer to the game's sound objects.
		 */
		std::shared_ptr<sfx::audio> _sounds;

		/**
		 * Pointer to the game's BGM objects.
		 */
		std::shared_ptr<sfx::audio> _music;
		
		/**
		 * Pointer to the renderer to render to.
		 */
		std::shared_ptr<sfx::renderer> _renderer;
		
		/**
		 * Pointer to the player's user input information.
		 */
		std::shared_ptr<sfx::user_input> _userinput;

		// spritesheets

		/**
		 * Pointer to the scripts object containing all the scripts.
		 */
		std::shared_ptr<engine::scripts> _scripts;
		
		/**
		 * Pointer to the GUI object containing all the menus.
		 */
		std::shared_ptr<engine::gui> _gui;
	};

	/*class game_engine_2 : sf::NonCopyable {
	public:
		static const char* STATE_TERMINATED;

		game_engine(const std::string& scriptsFolder = "assets/script",
			const std::string& JSON_AudioSound = "assets/audio/sound/audiosound.json",
			const std::string& JSON_AudioMusic = "assets/audio/music/audiomusic.json",
			const std::string& JSON_Fonts = "assets/fonts/fonts.json",
			const std::string& JSON_Renderer = "assets/renderer/renderer.json",
			const std::string& JSON_Spritesheet_GUI = "assets/sprites/gui/spritesgui.json",
			const std::string& JSON_Spritesheet_CO = "assets/sprites/gui/spritesco.json",
			const std::string& JSON_UserInput = "assets/userinput/userinput.json",
			const std::string& JSON_GUI = "assets/gui/gui.json",
			const std::string& JSON_Language = "assets/lang/lang.json",
			const std::string& JSON_Countries = "assets/property/country.json",
			const std::string& JSON_Weathers = "assets/property/weather.json",
			const std::string& JSON_Environments = "assets/property/environment.json",
			const std::string& JSON_Movements = "assets/property/movement.json",
			const std::string& JSON_Terrains = "assets/property/terrain.json",
			const std::string& JSON_Tiles = "assets/property/tile.json",
			const std::string& JSON_Units = "assets/property/unit.json",
			const std::string& JSON_COs = "assets/property/co.json",
			const std::string& name = "game_loop") noexcept;
		~game_engine() noexcept;

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
		// language dictionary object
		i18n::language_dictionary _dictionary;
		// renderer object
		sfx::renderer _renderer;
		// spritesheet objects
		sfx::animated_spritesheet _spritesheet_GUI;
		sfx::animated_spritesheet _spritesheet_CO;
		// user input objects
		sfx::user_input _userinput;
		// scripts object
		std::shared_ptr<engine::scripts> _scripts;
		// GUI object
		engine::gui _gui;
	};*/
}