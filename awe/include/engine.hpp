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

/**
 * @file engine.hpp
 * Declares the class which brings together all the others to maintain the engine
 * as it executes.
 */

#pragma once

#include "language.hpp"
#include "fonts.hpp"
#include "audio.hpp"
#include "renderer.hpp"
#include "userinput.hpp"
#include "gui.hpp"
#include "bank.hpp"
#include "map.hpp"
#include "mapstrings.hpp"
#include "transitions.hpp"
#include "settings.hpp"
#include "maths.hpp"
#include <filesystem>

namespace awe {
	/**
	 * The game engine class.
	 */
	class game_engine : sf::NonCopyable, public engine::script_registrant,
		public engine::json_script, public sfx::delta_timer {
	public:
		/**
		 * Thrown by \c load() when the user closes the window during loading.
		 */
		class load_cancelled : public std::exception {};

		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		game_engine(const engine::logger::data& data);

		/**
		 * Executes the game based on given game data.
		 * This method is blocking, meaning that it will not return so long as the
		 * game is still running.
		 * @return \c 0 upon successful execution, \c !0 upon a fatal error
		 *         occurring.
		 */
		int run();

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * game engine functions with a \c scripts object.
		 * @safety No guarantee.
		 * @sa     \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
	private:
		/**
		 * The JSON load method for this class.
		 * This JSON configuration contains all the paths leading to the JSON
		 * scripts which will be used to configure each component of the game
		 * engine. If even one is missing, an error will be logged, and execution
		 * behaviour becomes undefined. Each key (with the exception of one) has a
		 * string value which holds the path to the JSON script configuring the
		 * corresponding component of the game engine:
		 * <tt><ul><li>"countries"</li><li>"weathers"</li><li>"environments"</li>
		 * <li>"movements"</li><li>"terrains"</li><li>"tiles"</li><li>"units"</li>
		 * <li>"commanders"</li><li>"languages"</li><li>"fonts"</li>
		 * <li>"sounds"</li><li>"music"</li><li>"renderer"</li><li>"userinput"</li>
		 * <li>"gui"</li><li>"scripts"</li><li>"spritesheets"</li></ul></tt>
		 * The @c "spritesheets" key instead stores an object with the following
		 * key-value pairs, all the values of which are strings containing the
		 * paths of JSON scripts.
		 * <tt><ul><li>"co"</li><li>"unit"</li><li>"tile"</li><li>"icon"</li>
		 * <li>"gui"</li></ul></tt>
		 * The @c "unit" key stores an object containing more key-string pairs:
		 * <tt><ul><li>"idle"</li><li>"pictures"</li></ul></tt>
		 * The @c "tile" key stores an object containing more key-string pairs:
		 * <tt><ul><li>"normal"</li><li>"normalpictures"</li></ul></tt>
		 * If a key-value pair is given more than once, only the first instance
		 * will count and all subsequent pairs will be ignored.\n
		 * Loading a game engine instance will also generate the documentation of
		 * the script interface.
		 * @warning Note that the @c load() function for this class will change
		 *          the current working directory to whatever folder the given
		 *          configuration script was in at the time of calling.
		 * @param   j The \c engine::json object representing the contents of the
		 *            loaded script which this method reads.
		 * @return \c TRUE if loading was successful, \c FALSE otherwise.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * Loads an @c engine::json_script object using a path stored at @c keys.
		 * @tparam T                 The type of object to load.
		 * @tparam Ts                The types of the object's constructor
		 *                           parameters.
		 * @param  ptr               Stores a pointer to the newly created object
		 *                           in here.
		 * @param  j                 The JSON script containing the path of the
		 *                           JSON script.
		 * @param  keys              Key sequence leading to the path of the JSON
		 *                           script to load with.
		 * @param  constructorParams Additional objects that can be passed to the
		 *                           constructor of the object that @c ptr will
		 *                           point to.
		 * @return @c TRUE if @c j and @c *ptr are in a good state, @c FALSE if at
		 *         least one of them is not.
		 * @safety No guarantee.
		 */
		template<typename T, typename... Ts>
		bool _loadObject(std::shared_ptr<T>& ptr, engine::json& j,
			const engine::json::KeySequence& keys, Ts... constructorParams);

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Method called at the start of \c run() to make preliminary checks on
		 * internal data.
		 * All shared pointers are tested here: if at least one of them is
		 * \c nullptr, \c 1 will be returned.
		 * @return \c 0 if all tests passed, \c 1 if not.
		 */
		int _initCheck() const;

		//===================
		//======DRAWING======
		//===================
		/**
		 * Draws the colour flash animation, if one is currently in progress.
		 * @param rawDelta The value of \c calculateDelta().
		 * @param delta    The value of \c accumulatedDelta().
		 */
		void _drawColourFlash(const float rawDelta, const float delta);

		/**
		 * Animates the GUI transition, if it's currently animating.
		 */
		void _animateGUITransition();

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
		 * Creates a new map.
		 * If a map was already loaded, then an error will be logged, that map
		 * won't be unloaded, and no new map will be loaded.
		 * @warning If the given path points to an existing map, it will be
		 *          replaced!
		 * @param   file                The filepath of the new map.
		 * @param   playableMapTypeName The name of the type of the map object that
		 *                              is used by the scripts when playing the
		 *                              map.
		 * @return  Pointer to the \c map object to give to the scripts. Returns
		 *          \c nullptr if the map couldn't be loaded.
		 */
		awe::map* _script_createMap(const std::string& file,
			const std::string& playableMapTypeName);

		/**
		 * Loads a map.
		 * If a map was already loaded, then an error will be logged, that map
		 * won't be unloaded, and no new map will be loaded.
		 * @param  file                The map file to load.
		 * @param  playableMapTypeName The name of the type of the map object that
		 *                             is used by the scripts when playing the
		 *                             map.
		 * @return Pointer to the \c map object to give to the scripts. Returns
		 *         \c nullptr if the map couldn't be loaded.
		 */
		awe::map* _script_loadMap(const std::string& file,
			const std::string& playableMapTypeName);

		/**
		 * Quits the current map and opens a new menu.
		 * @warning This operation will invalidate the \c map pointer!
		 * @param menu The menu to open after closing the map.
		 */
		void _script_quitMap(const std::string& menu);

		/**
		 * Translates a string, with or without variables.
		 */
		std::string _script_translate(const std::string& nativeString,
			CScriptArray* variables = nullptr) const;

		/**
		 * Returns the current mouse position, scaled down by \c _scaling.
		 */
		sf::Vector2i _script_scaledMousePosition() const;

		/**
		 * Returns a randomly generated number between \c 0 and \c max.
		 * @param  max The maximum value the result can be.
		 * @return The random number.
		 */
		unsigned int _script_rand(const unsigned int max);

		/**
		 * Returns a randomly generated number between \c 0.0 and \c max.
		 * @param  max The maximum value the result can be.
		 * @return The random number.
		 */
		float _script_frand(const float max);

		/**
		 * Finds out if the given path exists as either a file or directory.
		 * @param  path The path to test.
		 * @return \c TRUE if \c path is either an existing path or file, \c FALSE
		 *         if not.
		 */
		bool _script_doesPathExist(const std::string& path) const;

		/**
		 * Returns the last log entry made.
		 * @return The last line of the log.
		 */
		std::string _script_getLatestLogEntry() const;

		/**
		 * Formats a bool.
		 * @param  b The bool value to convert into a string.
		 * @return "true" if \c b is != 0, or "false" if b is 0.
		 */
		std::string _script_formatBool(const bool b) const;

		/**
		 * Fade the entire screen into a solid colour, and then fade it back to
		 * normal again.
		 * Whilst a flash is on-going, a new one will not be made.
		 * @param c The colour to fade into.
		 * @param d The duration of the entire flash, in seconds.
		 */
		void _script_flashColour(const sf::Color& c, const float d = 2.0f);

		/**
		 * Invokes \c setGUI() after performing a fade out transition, and then
		 * animates a fade in transition.
		 * If a transition is already in progress, no changes will be made.
		 * @param menu     The name of the menu to set once the fade out animation
		 *                 has finished.
		 * @param duration The duration of the entire transition, in seconds.
		 */
		void _script_transitionToGUI(const std::string& menu,
			const float duration = 1.0f);

		/**
		 * Plays a sound or piece of music.
		 * If a piece of music is given, the currently playing music will be
		 * stopped first, if there is music currently playing.
		 * @param audioObject The name of the audio object containing the piece of
		 *                    audio to play.
		 * @param audioName   The name of the piece of audio to play.
		 * @param duration    The duration of the fade out. \c 0.0f if the music
		 *                    stops immediately without a fade out.
		 */
		void _script_play(const std::string& audioObject,
			const std::string& audioName, const float duration = 1.f);

		/**
		 * Stops the currently playing music from a given audio object.
		 * @param audioObject The name of the audio object containing the piece of
		 *                    music to stop.
		 * @param duration    The duration of the fade out. \c 0.0f if the music
		 *                    stops immediately without a fade out.
		 */
		void _script_stop(const std::string& audioObject,
			const float duration = 1.f);

		/**
		 * Updates the selected animation preset used with all maps.
		 * @param p The preset to apply.
		 */
		void _script_setSelectedMapAnimationPreset(const awe::animation_preset p);

		/**
		 * Gets the selected animation preset used with all maps.
		 * @return The selected animation preset.
		 */
		awe::animation_preset _script_getSelectedMapAnimationPreset() const;

		/**
		 * Selects the next map animation preset in the list.
		 * @return The newly selected animation preset.
		 */
		awe::animation_preset _script_selectNextMapAnimationPreset();

		/**
		 * Saves the global settings as they currently are.
		 */
		void _script_saveGlobalSettings();

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
		 * Pointer to the static information on all the weapons available.
		 */
		std::shared_ptr<awe::bank<awe::weapon>> _weapons;

		/**
		 * Pointer to the static information on all the types of units available.
		 */
		std::shared_ptr<awe::bank<awe::unit_type>> _units;

		/**
		 * Pointer to the static information on all the commanders available.
		 */
		std::shared_ptr<awe::bank<awe::commander>> _commanders;

		/**
		 * Pointer to the static information on all the structures available.
		 */
		std::shared_ptr<awe::bank<awe::structure>> _structures;

		/**
		 * Pointer to the map strings to use with all \c map objects.
		 */
		std::shared_ptr<awe::map_strings> _mapStrings;
		
		/**
		 * Stores a game's data, including its map and the armies.
		 */
		std::unique_ptr<awe::map> _map = nullptr;

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
		 * Pointer to the game's audio objects.
		 */
		std::shared_ptr<sfx::audios> _audios;
		
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
		std::shared_ptr<sfx::animated_spritesheets> _sprites;

		/**
		 * Pointer to the @c scripts object containing all the scripts.
		 */
		std::shared_ptr<engine::scripts> _scripts;
		
		/**
		 * Pointer to the GUI object containing all the menus.
		 */
		std::shared_ptr<sfx::gui> _gui;

		/**
		 * The global settings of the game.
		 */
		std::shared_ptr<awe::settings> _settings;

		//================================
		//==========ENGINE DATA===========
		//================================
		/**
		 * The scaling applied to all drawing.
		 */
		float _scaling = 2.0f;

		/**
		 * The pseudo-random number sequence generator.
		 */
		const std::unique_ptr<std::mt19937> _prng = engine::RNGFactory();

		/**
		 * The rectangle used to produce the colour flash.
		 */
		sf::RectangleShape _colourFlash;

		/**
		 * The colour last given to \c _script_flashColour().
		 */
		sf::Color _colourForFlash;

		/**
		 * Stores the duration last given to \c _script_flashColour().
		 */
		float _colourFlashDuration = 0.0f;

		/**
		 * Stores the alpha that was originally given to \c _script_flashColour()
		 * and uses it as a maximum alpha.
		 */
		float _colourFlashMaxAlpha = 255.f;

		/**
		 * Floating point holder for alpha.
		 */
		float _colourForFlashA = 0.f;

		/**
		 * The menu last given to \c _script_transitionToGUI().
		 */
		std::string _transitionToGUIMenu;

		/**
		 * The GUI transition animation.
		 */
		std::unique_ptr<awe::transition::rectangle> _guiTransition;

		/**
		 * Half the duration of the GUI transition given by the scripts.
		 * Half to account for the separate fade out and fade in.
		 */
		sf::Time _guiTransitionDuration;
	};
}

#include "tpp/engine.tpp"
