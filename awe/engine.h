/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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
 * Declares the class which brings together all the others to maintain the engine
 * as it executes.
 */

#pragma once

#include "language.h"
#include "fonts.h"
#include "audio.h"
#include "renderer.h"
#include "userinput.h"
#include "gui.h"
#include "bank.h"
#include "spritesheets.h"
#include "game.h"

namespace awe {
	/**
	 * The game engine class.
	 */
	class game_engine : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "engine."
		 * @sa    \c engine::logger
		 */
		game_engine(const std::string& name = "engine") noexcept;

		/**
		 * Executes the game based on given game data.
		 * This method is blocking, meaning that it will not return so long as the
		 * game is still running.
		 * @param  file Temporary parameter storing the file path to a binary map
		 *              file to open.
		 * @return \c 0 upon successful execution, \c !0 upon a fatal error
		 *         occurring.
		 */
		int run(const std::string& file) noexcept;

		/**
		 * Registers the script interface and loads the script files.
		 * \c setScripts() must be called before calling this method. If
		 * \c _scripts is \c nullptr, an error will be logged.
		 * @param folder The folder containing the script files to load.
		 */
		void initialiseScripts(const std::string& folder) noexcept;

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * game engine functions with a \c scripts object.
		 * For a full rundown of the interface, please read <a
		 * href="https://github.com/CasualYT31/ComputerWars/wiki/Script-Interface"
		 * target="_blank">the GitHub repository wiki</a>.
		 * @param engine Pointer to the engine to register the interface with.
		 * @sa    \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine) noexcept;

		/**
		 * Sets the engine's available countries.
		 * @param ptr Pointer to the data.
		 */
		void setCountries(const std::shared_ptr<awe::bank<awe::country>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available weather states.
		 * @param ptr Pointer to the data.
		 */
		void setWeathers(const std::shared_ptr<awe::bank<awe::weather>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available environments.
		 * @param ptr Pointer to the data.
		 */
		void setEnvironments(const std::shared_ptr<awe::bank<awe::environment>>&
			ptr) noexcept;

		/**
		 * Sets the engine's available movement types.
		 * @param ptr Pointer to the data.
		 */
		void setMovements(const std::shared_ptr<awe::bank<awe::movement_type>>&
			ptr) noexcept;

		/**
		 * Sets the engine's available terrain types.
		 * @param ptr Pointer to the data.
		 */
		void setTerrains(const std::shared_ptr<awe::bank<awe::terrain>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available tiles.
		 * @param ptr Pointer to the data.
		 */
		void setTiles(const std::shared_ptr<awe::bank<awe::tile_type>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available unit types.
		 * @param ptr Pointer to the data.
		 */
		void setUnits(const std::shared_ptr<awe::bank<awe::unit_type>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available commanders.
		 * @param ptr Pointer to the data.
		 */
		void setCommanders(const std::shared_ptr<awe::bank<awe::commander>>& ptr)
			noexcept;

		/**
		 * Sets the engine's available languages.
		 * @param ptr Pointer to the data.
		 */
		void setDictionary(const std::shared_ptr<engine::language_dictionary>& ptr)
			noexcept;

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
		 * Sets the engine's spritesheets.
		 * @param ptr Pointer to the data.
		 */
		void setSpritesheets(const std::shared_ptr<awe::spritesheets>& ptr)
			noexcept;

		/**
		 * Sets the engine's available scripts.
		 * @param ptr Pointer to the data.
		 */
		void setScripts(const std::shared_ptr<engine::scripts>& ptr) noexcept;

		/**
		 * Sets the engine's available menus.
		 * @param ptr Pointer to the data.
		 */
		void setGUI(const std::shared_ptr<sfx::gui>& ptr) noexcept;
	private:
		/**
		 * Method called at the start of \c run() to make preliminary checks on
		 * internal data.
		 * All shared pointers are tested here: if at least one of them is
		 * \c nullptr, \c 1 will be returned.
		 * @return \c 0 if all tests passed, \c 1 if not.
		 */
		int _initCheck() const noexcept;

		//=============================
		//======SCRIPT INTERFACE=======
		//=============================
		/**
		 * Updates the temporary fullscreen renderer setting.
		 * I decided to not make these temporary renderer settings global
		 * properties so that I could remain consistent with the way audio works
		 * in scripts (with accessor methods, e.g. setMusicVolume()).
		 * @param in The new fullscreen setting.
		 */
		void _script_setFullscreen(const bool in);

		/**
		 * Retrieves the temporary fullscreen renderer setting.
		 * @return The fullscreen renderer setting.
		 * @sa     \c _script_setFullscreen()
		 */
		bool _script_getFullscreen();

		/**
		 * Updates the temporary v-sync renderer setting.
		 * @param in The new v-sync setting.
		 * @sa    \c _script_setFullscreen()
		 */
		void _script_setVSync(const bool in);

		/**
		 * Retrieves the temporary v-sync renderer setting.
		 * @return The v-sync renderer setting.
		 * @sa     \c _script_setFullscreen()
		 */
		bool _script_getVSync();

		/**
		 * Calls \c _music->load().
		 * I wanted to make the mapping directly, however, I have to define the
		 * string parameter in the AngelScript declaration explicitly, otherwise
		 * the engine will crash, and that has the problem of letting scripts load
		 * and save the JSON script from and to anywhere. So I have to write this
		 * intermediary method to avoid crashing the engine and avoid giving
		 * scripts more power than they need.
		 */
		void _script_loadMusicConfig();

		/**
		 * Calls \c _music->save().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_saveMusicConfig();

		/**
		 * Calls \c _sounds->load().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_loadSoundConfig();

		/**
		 * Calls \c _sounds->save().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_saveSoundConfig();

		/**
		 * Calls \c _userinput->load().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_loadUIConfig();

		/**
		 * Calls \c _userinput->save().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_saveUIConfig();

		/**
		 * Essentially calls \c _renderer->load().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_loadRendererConfig();

		/**
		 * Essentially calls \c _renderer->save().
		 * @sa \c _script_loadMusicConfig()
		 */
		void _script_saveRendererConfig();

		/**
		 * Saves the current map.
		 */
		void _script_saveMap();

		/**
		 * Quits the current map and goes back to the previous menu.
		 */
		void _script_quitMap();

		//=============================
		//==========GAME DATA==========
		//=============================
		/**
		 * Pointer to the static information on all the countries available.
		 */
		std::shared_ptr<awe::bank<awe::country>> _countries;

		/**
		 * Pointer to the static information on all the types of weather available.
		 */
		std::shared_ptr<awe::bank<awe::weather>> _weathers;

		/**
		 * Pointer to the static information on all the environments available.
		 */
		std::shared_ptr<awe::bank<awe::environment>> _environments;

		/**
		 * Pointer to the static information on all the types of movement
		 * available.
		 */
		std::shared_ptr<awe::bank<awe::movement_type>> _movements;

		/**
		 * Pointer to the static information on all the types of terrain available.
		 */
		std::shared_ptr<awe::bank<awe::terrain>> _terrains;

		/**
		 * Pointer to the static information on all the tiles available.
		 */
		std::shared_ptr<awe::bank<awe::tile_type>> _tiles;

		/**
		 * Pointer to the static information on all the types of units available.
		 */
		std::shared_ptr<awe::bank<awe::unit_type>> _units;

		/**
		 * Pointer to the static information on all the commanders available.
		 */
		std::shared_ptr<awe::bank<awe::commander>> _commanders;
		
		/**
		 * Stores a game's data, including its map and the armies.
		 */
		std::unique_ptr<awe::game> _currentGame;

		//================================
		//==========BACKEND DATA==========
		//================================
		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;
		
		/**
		 * Pointer to the internationalisation object.
		 */
		std::shared_ptr<engine::language_dictionary> _dictionary;
		
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
		 * Intermediary renderer settings object.
		 * This is used to allow scripts to change settings but not apply them
		 * until required. Is initialised when \c _renderer is set using
		 * \c setRenderer().
		 */
		sfx::renderer_settings _tempRendererSettings;
		
		/**
		 * Pointer to the player's user input information.
		 */
		std::shared_ptr<sfx::user_input> _userinput;

		/**
		 * Pointer to the spritesheets.
		 */
		std::shared_ptr<awe::spritesheets> _sprites;

		/**
		 * Pointer to the scripts object containing all the scripts.
		 */
		std::shared_ptr<engine::scripts> _scripts;
		
		/**
		 * Pointer to the GUI object containing all the menus.
		 */
		std::shared_ptr<sfx::gui> _gui;
	};
}