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

/**@file map.hpp
 * Declares the class which is used to manage a map.
 * Unit, armies, and tiles are all stored and managed here.
 */

#pragma once

#include "tile.hpp"
#include "unit.hpp"
#include "army.hpp"
#include "script.hpp"
#include "language.hpp"
#include "gui.hpp"
#include "binary.hpp"
#include "mapstrings.hpp"
#include "animation.hpp"
#include "audio.hpp"
#include "maths.hpp"
#include "random_particles.hpp"
#include <stack>
#include <optional>
#include <queue>

namespace awe {
	/**
	 * Calculates the manhattan distance between two tiles.
	 * @param  lhs The source tile.
	 * @param  rhs The destination tile.
	 * @return The distance, in tiles.
	 */
	unsigned int inline distance(const sf::Vector2u& lhs, const sf::Vector2u& rhs)
		noexcept {
		return (unsigned int)(abs((int64_t)lhs.x - (int64_t)rhs.x) +
			abs((int64_t)lhs.y - (int64_t)rhs.y));
	}

	/**
	 * A node along a closed list path.
	 */
	struct closed_list_node :
		public engine::script_reference_type<awe::closed_list_node> {
		/**
		 * Registers this struct with the script interface, if it hasn't been
		 * already.
		 * @safety No guarantee.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Constructs a closed list node.
		 * @param tileIn The tile to construct the node with.
		 * @param gIn    The G score to construct the node with.
		 */
		closed_list_node(const sf::Vector2u& tileIn = {}, const int gIn = 0);

		/**
		 * Creates the closed list node.
		 * @return Pointer to the closed list node.
		 */
		static awe::closed_list_node* Create();

		/**
		 * The tile which this node represents.
		 */
		sf::Vector2u tile;

		/**
		 * The G score for this node.
		 */
		int g = 0;

		/**
		 * The animated sprite of the icon.
		 */
		sfx::animated_sprite sprite;
	};

	class map;

	/**
	 * An RAII memento disable token.
	 * When a disable token is created, it is given a pointer to an \c awe::map,
	 * and \c disableMementos() is invoked on that \c map. When the disable token's
	 * reference counter reaches \c 0, \c enableMementos() is invoked. This allows
	 * the client to disable and enable mementos on a \c map object without having
	 * to do so manually. When a bunch of operations have to be performed on a map,
	 * you can create a disable token within a scope, perform the operations, then
	 * let the control flow leave that scope. You can also have multiple disable
	 * tokens active at one time, across the scripts and the game engine proper.
	 * Only when the first token is destroyed will a memento be created.
	 */
	class disable_mementos :
		public engine::script_reference_type<awe::disable_mementos> {
	public:
		/**
		 * Registers this struct with the script interface, if it hasn't been
		 * already.
		 * @safety No guarantee.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Constructs a memento disable token.
		 * If \c nullptr is given, this token won't do anything.
		 * @param map  Pointer to the map to manage the mementos of.
		 * @param name The name to give the memento created when this token is
		 *             destroyed.
		 */
		disable_mementos(awe::map* const map, const std::string& name);

		/**
		 * When a disable token is deleted, mementos are re-enabled.
		 */
		~disable_mementos() noexcept;

		/**
		 * Creates the disable token.
		 * @param  map  Pointer to the map to manage the mementos of.
		 * @param  name The name to give the memento created when this token is
		 *              destroyed.
		 * @return Pointer to the disable token.
		 */
		static awe::disable_mementos* Create(awe::map* const map,
			const std::string& name);

		/**
		 * Enables mementos on the map but does not go on to create a memento, and
		 * sets the internal \c map pointer to \c nullptr.
		 * Used when a token is created, but an operation failed, and so a memento
		 * should not be created. Only employ this method when necessary; the
		 * better alternative is to perform checks before creating the token and
		 * performing operations so that you can guarantee they will succeed.
		 */
		void discard();
	private:
		/**
		 * The map the disable token is acting upon.
		 */
		awe::map* _map;

		/**
		 * The name to give the memento created from this token.
		 */
		const std::string _name;
	};

	/**
	 * The list of shaders that can be applied to the tiles that are available.
	 */
	enum class available_tile_shader {
		None,
		Yellow,
		Red
	};

	/**
	 * Represents quadrants of a rectangle such as a render target.
	 */
	enum class quadrant {
		UpperLeft,
		UpperRight,
		LowerLeft,
		LowerRight
	};

	/**
	 * Represents orthogonal directions.
	 */
	enum class direction {
		Up,
		Down,
		Left,
		Right
	};

	/**
	 * The list of available animation presets.
	 */
	enum class animation_preset {
		VisualA,  //!< Battle and capture animations are on.
		VisualB,  //!< Fast unit speed, all animations.
		VisualC,  //!< Fast unit speed, battle animations only.
		VisualD,  //!< Fast unit speed, no CPU battle animations. Redundant atm.
		NoVisual, //!< Only basic animations.
		Debug,    //!< Used to skip some longer animations such as Day Begin.
		Count     //!< C++-only, used to know when to wrap when cycling.
	};

	/**
	 * Cycle the given \c animation_preset forward by one.
	 * @param  p The enum value to increment.
	 * @return The given enum value.
     */
	animation_preset& operator++(animation_preset& p) noexcept;

	/**
	 * Class which represents a map, and the armies and units that play on it.
	 * Only basic checks are carried out in this class; all game logic is separate.
	 * If any of these basic checks fail, they will be logged.
	 * @sa @c awe::game
	 */
	class map : sf::NonCopyable, public sfx::animated_drawable {
	public:
		//////////////////////////////////////////
		// SCRIPT INTERFACE AND FILE OPERATIONS //
		//////////////////////////////////////////
		/**
		 * Registers the \c Map script object type, if it hasn't already been
		 * registered.
		 * @safety No guarantee.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Initialises this object with \c bank pointers.
		 * Also initialises the internal logger object.\n
		 * @param countries    Information on the countries available to the map.
		 * @param environments Information on the environments available to the
		 *                     map. Cannot be \c nullptr.
		 * @param weathers     Information on the weathers available to the map.
		 *                     Cannot be \c nullptr.
		 * @param tiles        Information on the tile types available to the map.
		 * @param terrains     Information on the terrains available to the map.
		 * @param units        Information on the unit types available to the map.
		 * @param commanders   Information on the COs available to the map.
		 * @param structures   Information on the structures available to the map.
		 * @param data         The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		map(const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::environment>>& environments,
			const std::shared_ptr<awe::bank<awe::weather>>& weathers,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::terrain>>& terrains,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders,
			const std::shared_ptr<awe::bank<awe::structure>>& structures,
			const engine::logger::data& data);

		/**
		 * Makes sure to release any given script callbacks and modules.
		 */
		~map() noexcept;

		/**
		 * Replaces the state of this object with that given in the binary file.
		 * For full documentation on Computer Wars' map file format, please see the
		 * CWM Format Specification.md file in the root folder of the repository.\n
		 * Note that the original state of the @c map object will be lost,
		 * regardless of whether or not loading succeeds.
		 * @param  file    Path to the binary file to load from. If a blank string,
		 *                 the file given previously, in either a call to \c load()
		 *                 or a call to \c save(), will be used.
		 * @param  version The 0-based number identifying the iteration of the
		 *                 format to use.
		 * @return \c TRUE if the load was successful, \c FALSE if the file
		 *         couldn't be loaded (reason will be logged).
		 * @safety No guarantee.
		 */
		bool load(std::string file, const unsigned char version = 0);

		/**
		 * Saves this \c map object's state to a given binary file.
		 * @param  file    Path to the binary file to save to. If a blank string,
		 *                 the file given previously, in either a call to \c load()
		 *                 or a call to \c save(), will be used.
		 * @param  version The 0-based number identifying the iteration of the
		 *                 format to use.
		 * @return \c TRUE if the save was successful, \c FALSE if the file
		 *         couldn't be saved (reason will be logged).
		 * @safety No guarantee.
		 */
		bool save(std::string file, const unsigned char version);

		/**
		 * Gives access to the scripts to save the map.
		 * @param  file The path to save the map to. If an empty string, the last
		 *              known location will be used.
		 * @return \c TRUE if the save was successful, \c FALSE if the file
		 *         couldn't be saved (reason will be logged).
		 * @safety No guarantee.
		 */
		bool save(const std::string& file = "");

		/**
		 * Stores additional data into the map.
		 * @param data The data to store. Can take any format the scripts desire.
		 *             It can also store NULL bytes.
		 */
		void setAdditionalData(const std::string& data);

		/**
		 * Copies the additional data stored in the map, and returns it.
		 * @return A copy of the additional data that's currently stored.
		 */
		std::string getAdditionalData() const;

		/**
		 * The \c scripts object which will allow this \c map object to create
		 * arrays for the scripts.
		 * If the given pointer was not \c nullptr, then the closed list will be
		 * allocated here.
		 * @warning Undefined behaviour will occur if you call this method more
		 *          than once!
		 * @param   scripts Pointer to the \c scripts object.
		 * @safety  Strong guarantee.
		 * @sa      @c getClosedList().
		 */
		void setScripts(const std::shared_ptr<engine::scripts>& scripts);

		/**
		 * Allows the scripts to pass a pointer to an object that represents the
		 * map, in the current case, a \c PlayableMap object.
		 * @param mapObject Pointer to the map object.
		 */
		void setMapObject(CScriptHandle mapObject);

		/**
		 * Provide the name of the type of the script's map object.
		 * @warning This must be given before loading any map file! This is because
		 *          the scripts within the map rely on the map object's type name!
		 * @param   typeName The name of the type of the map object.
		 */
		void setMapObjectType(const std::string& typeName);

		/**
		 * Has the map been changed since it was last saved successfully?
		 * Only tests relevant map data, not if spritesheets were changed, or the
		 * scripts object was changed, etc.
		 * @return \c TRUE if the map has changed, \c FALSE if not.
		 */
		bool hasChanged() const;

		/**
		 * Carry out periodic tasks, such as checking for win conditions.
		 * @return If \c TRUE is returned, it signifies that the map's win
		 *         condition has been met, and the map object should be deleted.
		 */
		bool periodic();

		/**
		 * Enables or disables the periodic checks.
		 * They are enabled by default.
		 * @param enabled Pass \c FALSE to disable the periodic method, and \c TRUE
		 *                to enable it.
		 */
		void enablePeriodic(const bool enabled);

		/**
		 * The default win condition of a map.
		 * If there is only one team remaining on the map, then the win condition
		 * has been met. If there is no team remaining, then \c TRUE will be
		 * returned.
		 * @return \c TRUE if the win condition has been met, \c FALSE otherwise.
		 */
		bool defaultWinCondition() const;

		/**
		 * If this map defines a \c beginTurnForOwnedTile() function, invoke it.
		 * This script function is invoked at the beginning of every army's turn.
		 * Each of their owned tiles will be passed to it. This function will be
		 * invoked \em before healing handled by properties, and plane crashes and
		 * ship sinkings, etc., but after planes and ships have burned their fuel.
		 * @param  tile        The tile that needs to be handled.
		 * @param  terrain     The terrain of the tile.
		 * @param  currentArmy The owner of the tile.
		 * @return \c TRUE if the script function was invoked, and it should
		 *         prevent/override the default behaviour of that tile. \c FALSE
		 *         otherwise.
		 */
		bool beginTurnForOwnedTile(sf::Vector2u tile, awe::terrain* const terrain,
			const awe::ArmyID currentArmy);

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		/**
		 * Sets the map's name.
		 * By default, a map's name is a blank string.\n
		 * If there any \c ~ characters in the given string, then they will be
		 * removed and a warning will be logged.
		 * @param  name The name to give to the map to replace the old name.
		 * @safety Strong guarantee.
		 */
		void setMapName(std::string name);

		/**
		 * Retrieves the map's name.
		 * @return The current name of the map.
		 */
		std::string getMapName() const;

		/**
		 * Sets the map's size, in tiles.
		 * If any tile should become out of bounds after the resize, they <b>as
		 * well as the unit on them</b> will be deleted. Any indices to these
		 * objects within any army object will also be dropped.\n
		 * If any new tiles should be created, then they will be assigned \c tile
		 * as their new type as and when they are created.\n
		 * The currently selected tile will be adjusted in case it should become
		 * out of bounds after the resize. Only the out of bounds axes will be
		 * adjusted if this is the case.
		 * @warning Maps cannot be the maximum size of an unsigned int in either
		 *          dimension, they must be smaller than this! This is because the
		 *          engine relies on the fact that \c awe::unit::NO_POSITION is the
		 *          maximum size of the vector type in use, and \c _isOutOfBounds()
		 *          is sometimes given a unit's position directly—this position may
		 *          be \c NO_POSITION!
		 * @remark  Since this operation shouldn't be a bottleneck, I've not
		 *          concerned myself much with efficiency in its implementation.
		 *          The main way to improve efficiency would be to loop through
		 *          each relevant tile and remove their unit before the resize,
		 *          rather than looping through \em all units and removing out of
		 *          bounds ones, after the resize.
		 * @param   dim   The width (x) and height (y) to make the map.
		 * @param   tile  The type of tile to assign to new tiles.
		 * @param   owner The owner to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u& dim,
			const std::shared_ptr<const awe::tile_type>& tile = nullptr,
			const awe::ArmyID owner = awe::NO_ARMY);

		/**
		 * Overload of \c setMapSize() which accepts a tile type script name.
		 * @param dim   The new size of the map.
		 * @param tile  The script name of the tile type to assign to new tiles.
		 * @param owner The owner to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u& dim, const std::string& tile,
			const awe::ArmyID owner = awe::NO_ARMY);

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The map's size. X = width, Y = height.
		 */
		inline sf::Vector2u getMapSize() const {
			return _mapSizeCache;
		}

		/**
		 * Fills the map with a given type of tile, and sets their owners.
		 * If \c nullptr is given, an error will be logged and the method will
		 * fail.
		 * @param  type  The type of tile to change each tile in the map to.
		 * @param  owner The owner to assign to each tile. Can be \c NO_ARMY.
		 * @return \c TRUE if filling the map was successful, \c FALSE otherwise.
		 */
		bool fillMap(const std::shared_ptr<const awe::tile_type>& type,
			const awe::ArmyID owner);

		/**
		 * Version of \c fillMap() which accepts a tile type script name.
		 */
		bool fillMap(const std::string& type, const awe::ArmyID owner);

		/**
		 * Fills a selection of tiles on the map with a given type of tile, and
		 * sets their owners.
		 * If \c nullptr is given, an error will be logged and the method will
		 * fail.
		 * @param  start The tile from which the rectangle spreads.
		 * @param  end   The tile which the rectangle spreads to.
		 * @param  type  The type of tile to change each tile in the rectangle to.
		 * @param  owner The owner to assign to each tile. Can be \c NO_ARMY.
		 * @return \c TRUE if filling the tiles was successful, \c FALSE otherwise.
		 */
		bool rectangleFillTiles(const sf::Vector2u& start, const sf::Vector2u& end,
			const std::shared_ptr<const awe::tile_type>& type,
			const awe::ArmyID owner);

		/**
		 * Version of \c rectangleFillTiles() which accepts a tile type script
		 * name.
		 */
		bool rectangleFillTiles(const sf::Vector2u& start, const sf::Vector2u& end,
			const std::string& type, const awe::ArmyID owner);

		/**
		 * Fills a selection of tiles on the map with a given type of unit, and
		 * deletes any old units that were occupying any of those tiles.
		 * The newly created units will not be waiting, and will be fully healed
		 * and replenished.\n
		 * If \c nullptr or \c NO_ARMY are given, an error will be logged and the
		 * method will fail.\n
		 * If the the given army doesn't exist, it will be created.
		 * @param  start The tile from which the rectangle spreads.
		 * @param  end   The tile which the rectangle spreads to.
		 * @param  type  The type of unit to create on each tile in the rectangle.
		 * @param  army  The army to assign to each unit.
		 * @return \c TRUE if creating the units was successful, \c FALSE
		 *         otherwise.
		 */
		bool rectangleFillUnits(const sf::Vector2u& start, const sf::Vector2u& end,
			const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army);

		/**
		 * Version of \c rectangleFillUnits() which accepts a unit type script
		 * name.
		 */
		bool rectangleFillUnits(const sf::Vector2u& start, const sf::Vector2u& end,
			const std::string& type, const awe::ArmyID army);

		/**
		 * Deletes a selection of units from the map.
		 * Goes through each tile in a rectangle, and if the tile has a unit upon
		 * it, it shall be deleted.
		 * @param  start The tile from which the rectangle spreads.
		 * @param  end   The tile which the rectangle spreads to.
		 * @return The number of units deleted.
		 */
		std::size_t rectangleDeleteUnits(const sf::Vector2u& start,
			const sf::Vector2u& end);

		/**
		 * Sets the current day.
		 * @param day The new day.
		 */
		void setDay(const awe::Day day);

		/**
		 * Gets the current day.
		 * @return The current day.
		 */
		awe::Day getDay() const noexcept;

		/**
		 * Enables Fog of War on this map.
		 * @param enabled \c TRUE if Fog of War should be enabled, \c FALSE if it
		 *                should be disabled.
		 */
		void enableFoW(const bool enabled);

		/**
		 * Is Fog of War currently enabled on this map?
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		inline bool isFoWEnabled() const noexcept {
			return _fow;
		}

		/**
		 * Sets this map's weather.
		 * @param weather Pointer to the weather to set.
		 */
		void setWeather(const std::shared_ptr<const awe::weather>& weather);

		/**
		 * Overload of \c setWeather() which accepts a script name.
		 * @param name Script name of the weather to set.
		 */
		void setWeather(const std::string& name);

		/**
		 * Retrieves the currently set weather.
		 * @return Pointer to the information on the current weather.
		 */
		std::shared_ptr<const awe::weather> getWeather() const;

		/**
		 * Retrieves the current weather, for use with the scripts.
		 * @return Information on the current weather.
		 * @throws \c std::out_of_range If there is currently no weather set.
		 */
		const awe::weather* getWeatherObject() const;

		/////////////////////
		// ARMY OPERATIONS //
		/////////////////////
		/**
		 * Allocates a new army.
		 * If the army with the given country already exists, or \c nullptr is
		 * given, the call will be logged.
		 * @warning By default, a newly created army will be on its own team.
		 *          However, a simple counter is used to achieve this, so if the
		 *          client reassigns team IDs, this behaviour can't be guaranteed.
		 * @param  country The country of the army.
		 * @return \c TRUE if the army was created, \c FALSE otherwise.
		 */
		bool createArmy(const std::shared_ptr<const awe::country>& country);

		/**
		 * Overload of \c createArmy() which accepts a country script name.
		 * @param country The script name of the country of the army.
		 */
		bool createArmy(const std::string& country);

		/**
		 * Deletes an army entirely from the map.
		 * Deleting an army removes the army from the army list, deletes all the
		 * units belonging to the army, and disowns all owned tiles.\n
		 * If at least one of the army IDs given was invalid, the operation will be
		 * cancelled and logged (\c NO_ARMY cannot be given as the first
		 * parameter).\n
		 * Note that this will also transfer structures to the given army! Each
		 * structure will have its own needs, either generally (such as HQ: we want
		 * to destroy it to a city and assign the owner to \c transferOwnership),
		 * or on a per-map basis (it would probably be strange to have the Volcano
		 * erupt later in the day, so it would really need to transfer to the next
		 * first army if the first army is deleted). These needs must be addressed
		 * separately prior to deleting the army!
		 * @param army              The ID of the army to delete.
		 * @param transferOwnership The ID of the army who will assume ownership of
		 *                          all the deleted army's tiles. By default,
		 *                          ownership is set to \c NO_ARMY, i.e. back to
		 *                          neutral.
		 */
		void deleteArmy(const awe::ArmyID army,
			const awe::ArmyID transferOwnership = awe::NO_ARMY);

		/**
		 * Retrieves the number of armies currently on the map.
		 * @return The number of armies currently in play.
		 */
		std::size_t getArmyCount() const noexcept;

		/**
		 * Creates a set of each army's \c ArmyID, in their turn order.
		 * @return The set.
		 */
		std::set<awe::ArmyID> getArmyIDs() const;

		/**
		 * Calls \c getArmyIDs() and converts the result into a \c CScriptArray.
		 * @return The array listing each army's \c ArmyID in turn order.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 */
		CScriptArray* getArmyIDsAsArray() const;

		/**
		 * Sets the team that a given army belongs to.
		 * @param army The ID of the army to change.
		 * @param team The ID of the team to assign this army to.
		 */
		void setArmyTeam(const awe::ArmyID army, const awe::TeamID team);

		/**
		 * Gets the team that a given army belongs to.
		 * @param  army The ID of the army to query.
		 * @return The ID of the team that the given army belongs to. \c 0 if the
		 *         given army doesn't exist.
		 */
		awe::TeamID getArmyTeam(const awe::ArmyID army) const;

		/**
		 * Sets the amount of funds a specified army obtains.
		 * If \c 0 or less is given, \c 0 will be stored.
		 * @param army  The ID of the army to amend.
		 * @param funds The new fund amount to assign to the army.
		 */
		void setArmyFunds(const awe::ArmyID army, const awe::Funds funds);

		/**
		 * Offsets the amount of funds a specified army obtains.
		 * If the result of the operation would result in a fund amount of \c 0 or
		 * less, \c 0 will be stored.
		 * @param army  The ID of the army to amend.
		 * @param funds The number of funds to award to the army.
		 */
		void offsetArmyFunds(const awe::ArmyID army, const awe::Funds funds);

		/**
		 * Retrieves an army's fund count.
		 * @param  army The ID of the army to inspect.
		 * @return The amount of funds this army possesses, or < 0 if the given
		 *         army doesn't exist.
		 */
		awe::Funds getArmyFunds(const awe::ArmyID army) const;

		/**
		 * Retrieves an army's country.
		 * @param  army The ID of the army whose country is to be retrieved.
		 * @return Pointer to the information on the given army's country.
		 *         \c nullptr is returned if the given army did not exist at the
		 *         time of calling.
		 */
		std::shared_ptr<const awe::country> getArmyCountry(
			const awe::ArmyID army) const;

		/**
		 * Retrieves an army's country, for use with the scripts.
		 * @param  army The ID of the army whose country is to be reteieved.
		 * @return Information on the given army's country.
		 * @throws std::out_of_range if an army with the given ID didn't exist.
		 */
		const awe::country* getArmyCountryObject(const awe::ArmyID army) const;

		/**
		 * Sets the COs that are in charge of a specified army.
		 * If \c current was \c nullptr, but \c tag was not, the current CO will be
		 * assigned \c tag and a warning will be logged. If both pointer parameters
		 * are \c nullptr, an error will be logged. If the specified army did not
		 * exist at the time of calling, an error will be logged.
		 * @param army    The ID of the army to set the COs of.
		 * @param current The primary CO of the specified army.
		 * @param tag     The tag CO of the specified army. Should be \c nullptr if
		 *                there will not be one.
		 */
		void setArmyCOs(const awe::ArmyID army,
			const std::shared_ptr<const awe::commander>& current,
			const std::shared_ptr<const awe::commander>& tag = nullptr);

		/**
		 * Overload of \c setArmyCOs() which accepts script names.
		 * @param army    The ID of the army to set the COs of.
		 * @param current The primary CO of the specified army.
		 * @param tag     The tag CO of the specified army. Should be an empty
		 *                string if there will not be one.
		 */
		void setArmyCOs(const awe::ArmyID army, const std::string& current,
			 const std::string& tag = "");

		/**
		 * Sets the current CO of a specified army.
		 * @param army    The ID of the army to set the CO of.
		 * @param current The primary CO of the specified army.
		 * @sa    @c setArmyCOs()
		 */
		void setArmyCurrentCO(const awe::ArmyID army,
			const std::shared_ptr<const awe::commander>& current);

		/**
		 * Overload of \c setArmyCurrentCO() which accepts a script name.
		 * @param army    The ID of the army to set the CO of.
		 * @param current The primary CO of the specified army.
		 */
		void setArmyCurrentCO(const awe::ArmyID army, const std::string& current);

		/**
		 * Sets the tag CO of a specified army.
		 * @param army The ID of the army to set the CO of.
		 * @param tag  The tag CO of the specified army.
		 * @sa    @c setArmyCOs()
		 */
		void setArmyTagCO(const awe::ArmyID army,
			const std::shared_ptr<const awe::commander>& tag);

		/**
		 * Overload of \c setArmyTagCO() which accepts a script name.
		 * @param army The ID of the army to set the CO of.
		 * @param tag  The tag CO of the specified army. Can be an empty string to
		 *             mean a lack of a tag CO.
		 */
		void setArmyTagCO(const awe::ArmyID army, const std::string& tag);

		/**
		 * Performs a tag on a given army.
		 * If the given army ID did not identify an army, or if there were not two
		 * COs to perform the tag with, then an error will be logged.
		 * @param army The ID of the army to perform the tag on.
		 */
		void tagArmyCOs(const awe::ArmyID army);

		/**
		 * Retrieves an army's primary/current CO.
		 * @param  army The ID of the army to retrieve the current CO of.
		 * @return Pointer to the information on the given army's current CO.
		 *         \c nullptr is returned if the given army did not exist at the
		 *         time of calling, or if there was no current CO.
		 */
		std::shared_ptr<const awe::commander> getArmyCurrentCO(
			const awe::ArmyID army) const;

		/**
		 * Gets the army's primary CO's script name.
		 * @param  army The ID of the army to retrieve the current CO of.
		 * @return The script name of the primary CO assigned to this army, or an
		 *         empty string if one is not assigned.
		 */
		std::string getArmyCurrentCOScriptName(const awe::ArmyID army) const;

		/**
		 * Retrieves an army's secondary/tag CO.
		 * @param  army The ID of the army to retrieve the tag CO of.
		 * @return Pointer to the information on the given army's tag CO.
		 *         \c nullptr is returned if the given army did not exist at the
		 *         time of calling, or if there was no tag CO.
		 */
		std::shared_ptr<const awe::commander> getArmyTagCO(
			const awe::ArmyID army) const;

		/**
		 * Gets the army's secondary CO's script name.
		 * @param  army The ID of the army to retrieve the current CO of.
		 * @return The script name of the secondary CO assigned to this army, or an
		 *         empty string if one is not assigned.
		 */
		std::string getArmyTagCOScriptName(const awe::ArmyID army) const;

		/**
		 * Finds out if an army has a tag CO.
		 * @param  army The ID of the army who should be queried.
		 * @return \c TRUE if the specified army has two COs, \c FALSE if it only
		 *         has one (or if the specified army didn't exist at the time of
		 *         calling).
		 */
		bool tagCOIsPresent(const awe::ArmyID army) const;

		/**
		 * Retrieves a list of tiles that belong to a specified army.
		 * If the specified army doesn't exist, an empty set will be returned.
		 * @param  army   The ID of the army to retrieve the owned tiles of.
		 * @param  filter A selection of terrains that a tile must match one of if
		 *                they are to be included in the result. An empty filter
		 *                means that all tiles that belong to an army are included
		 *                in the result.
		 * @return A list of locations of tiles that belong to the given army.
		 */
		std::unordered_set<sf::Vector2u> getTilesOfArmy(const awe::ArmyID army,
			const std::unordered_set<std::shared_ptr<const awe::terrain>>&
			filter = {}) const;

		/**
		 * Converts the result of a \c getTilesOfArmy() call into a
		 * \c CScriptArray.
		 * @param  filter Array of terrain script names to act as a filter.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesOfArmy().
		 */
		CScriptArray* getTilesOfArmyAsArray(const awe::ArmyID army,
			const CScriptArray* const filter = nullptr) const;

		/**
		 * Retrieves a list of units that belong to a specified army.
		 * If the specified army doesn't exist, an empty set will be returned.
		 * @param  army The ID of the army to retrieve the units of.
		 * @return A list of IDs identifying the units that belong to this army.
		 */
		std::unordered_set<awe::UnitID> getUnitsOfArmy(
			const awe::ArmyID army) const;

		/**
		 * Converts the result of a \c getUnitsOfArmy() call into a
		 * \c CScriptArray.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getUnitsOfArmy().
		 */
		CScriptArray* getUnitsOfArmyAsArray(const awe::ArmyID army) const;

		/**
		 * Retrieves a list of units that belong to a given army, ordered by
		 * priority.
		 * For each of an army's units, their turn start priority will be queried.
		 * They will then be categorised into the resulting map based on this
		 * priority.\n
		 * If the specified army doesn't exist, an empty map will be returned.
		 * @param  army The ID of the army to retrieve the units of.
		 * @return Unit IDs, categorised by their turn start priority.
		 */
		std::map<unsigned int, std::unordered_set<awe::UnitID>>
			getUnitsOfArmyByPriority(const awe::ArmyID army) const;

		/**
		 * Converts the result of a \c getUnitsOfArmyByPriority() call into a
		 * \c CScriptArray.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getUnitsOfArmyByPriority().
		 */
		CScriptArray* getUnitsOfArmyByPriorityAsArray(
			const awe::ArmyID army) const;

		/**
		 * Counts how many tiles of a specific terrain type the given army owns.
		 * @param  army        The ID of the army to query.
		 * @param  terrainType The terrain to search for.
		 * @return The number of tiles belonging to the army that have the given
		 *         terrain type.
		 */
		std::size_t countTilesBelongingToArmy(const awe::ArmyID army,
			const std::string& terrainType) const;

		/////////////////////
		// UNIT OPERATIONS //
		/////////////////////
		/**
		 * Creates a new unit.
		 * The unit won't be created if the army ID isn't valid.
		 * @param  type The type of unit to create.
		 * @param  army The ID of the army who will own this unit.
		 * @return The ID of the unit created. Will be \c NO_UNIT if the unit
		 *         couldn't be created.
		 */
		awe::UnitID createUnit(const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army);

		/**
		 * Overload of \c createUnit() which accepts a unit type script name.
		 * @param  type The script name of the type of unit to create.
		 * @param  army The ID of the army who will own this unit.
		 * @return The ID of the unit created. Will be \c NO_UNIT if the unit
		 *         couldn't be created.
		 */
		awe::UnitID createUnit(const std::string& type, const awe::ArmyID army);

		/**
		 * Deletes a unit.
		 * A deleted unit will be removed from the map's and owning army's list, as
		 * well as the tile it was on. Any location override associated with the
		 * unit will also be deleted.
		 * @warning Any loaded units will \b also be deleted. Additionally, if the
		 *          deleted unit was selected, then \c setSelectedUnit(NO_UNIT)
		 *          will be called!
		 * @param   id The ID of the unit to delete.
		 */
		void deleteUnit(const awe::UnitID id);

		/**
		 * Gets a unit's type.
		 * @param  id The 1-based ID of the unit to query.
		 * @return Pointer to the unit's type properties. \c nullptr if the unit ID
		 *         was invalid.
		 */
		std::shared_ptr<const awe::unit_type> getUnitType(
			const awe::UnitID id) const;

		/**
		 * Script version of \c getUnitType().
		 * @throws std::out_of_range when the given ID did not identify a unit.
		 * @sa     @c getUnitType().
		 */
		const awe::unit_type* getUnitTypeObject(const awe::UnitID id) const;

		/**
		 * Sets a unit's position on the map.
		 * The operation will be cancelled if the specified tile is already
		 * occupied.
		 * @param id  The ID of the unit to move.
		 * @param pos The X and Y coordinate of the tile to move the unit to.
		 */
		void setUnitPosition(const awe::UnitID id, const sf::Vector2u& pos);

		/**
		 * Retrieves a unit's position, indicating the tile it is occupying.
		 * This method does not take into account if the unit is \em actually on a
		 * tile: please use this method in conjunction with \c isUnitOnMap().\n
		 * If the unit doesn't exist, <tt>awe::unit::NO_POSITION</tt> will be
		 * returned.
		 * @param  id The ID of the unit.
		 * @return The X and Y location of the unit on the map.
		 */
		sf::Vector2u getUnitPosition(const awe::UnitID id) const;

		/**
		 * Finds out if a unit occupies a tile or not.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if this unit occupies a tile, \c FALSE if not.
		 */
		bool isUnitOnMap(const awe::UnitID id) const;

		/**
		 * Sets a unit's HP.
		 * If < 0 is given, 0 will be stored.
		 * @param id The ID of the unit to amend.
		 * @param hp The new HP of the unit. In internal format.
		 */
		void setUnitHP(const awe::UnitID id, const awe::HP hp);

		/**
		 * Gets a unit's HP.
		 * @param  id The ID of the unit to inspect.
		 * @return The internal HP of the unit. \c 0 if unit doesn't exist.
		 */
		awe::HP getUnitHP(const awe::UnitID id) const;

		/**
		 * Gets a unit's HP in a user-friendly format.
		 * @param  id The ID of the unit to inspect.
		 * @return The user-friendly HP of the unit. \c 0 if unit doesn't exist.
		 */
		awe::HP getUnitDisplayedHP(const awe::UnitID id) const;

		/**
		 * Sets a unit's fuel.
		 * If < 0 is given, 0 will be stored.
		 * @param id   The ID of the unit to amend.
		 * @param fuel The new fuel of the unit.
		 */
		void setUnitFuel(const awe::UnitID id, const awe::Fuel fuel);

		/**
		 * Offsets a unit's fuel.
		 * If the result of the operation would set the unit's fuel to < 0, then 0
		 * will be stored.
		 * @param id   The ID of the unit whose fuel is being burned.
		 * @param fuel The fuel units that are being removed from the unit.
		 */
		void burnUnitFuel(const awe::UnitID id, const awe::Fuel fuel);

		/**
		 * Gets a unit's fuel.
		 * @param  id The ID of the unit to inspect.
		 * @return The fuel of the unit. \c 0 if unit doesn't exist.
		 */
		awe::Fuel getUnitFuel(const awe::UnitID id) const;

		/**
		 * Sets a unit's ammo.
		 * If < 0 is given, 0 will be stored.
		 * @param id     The ID of the unit to amend.
		 * @param weapon The script name of the weapon to update.
		 * @param ammo   The new ammo of the unit's weapon.
		 */
		void setUnitAmmo(const awe::UnitID id, const std::string& weapon,
			const awe::Ammo ammo);

		/**
		 * Gets a unit's ammo.
		 * @param  id     The ID of the unit to inspect.
		 * @param  weapon The script name of the weapon to query.
		 * @return The ammo of the unit's weapon. \c 0 if unit or weapon doesn't
		 *         exist.
		 */
		awe::Ammo getUnitAmmo(const awe::UnitID id,
			const std::string& weapon) const;

		/**
		 * Gets a unit's vision.
		 * Retrieves the unit type's base vision, and then applies any offsets to
		 * it based on the tile the unit is currently on, if any.
		 * @param  id The ID of the unit to query.
		 * @return The vision of the unit, with any offsets applied. Will never be
		 *         lower than \c 1. Will return \c 0, however, if the given unit
		 *         does not exist.
		 */
		unsigned int getUnitVision(const awe::UnitID id) const;

		/**
		 * Replenishes a unit's fuel and all of its ammo.
		 * @param id   The ID of the unit to replenish.
		 * @param heal If \c TRUE, the unit's HP is set to max, too.
		 */
		void replenishUnit(const awe::UnitID id, const bool heal = false);

		/**
		 * Is this unit at max fuel, ammos, and (optionally) HP?
		 * @param  id The ID of the unit to query.
		 * @param  hp \c TRUE if this method should also check for HP. \c FALSE if
		 *            it should just check for fuel and ammos.
		 * @return \c TRUE if this unit exists and has full (or infinite) fuel and
		 *         ammos, \em and has full internal HP if \c hp is \c TRUE.
		 *         \c FALSE otherwise.
		 */
		bool isUnitReplenished(const awe::UnitID id, const bool hp = false) const;

		/**
		 * Sets a unit's waiting state.
		 * @param id      The ID of the unit to amend.
		 * @param waiting \c TRUE if the unit should wait, \c FALSE if not.
		 */
		void waitUnit(const awe::UnitID id, const bool waiting);

		/**
		 * Gets a unit's waiting state.
		 * @param  id The ID of the unit to query.
		 * @return \c TRUE if the unit is waiting, \c FALSE if it is not. \c FALSE
		 *         if the unit does not exist.
		 */
		bool isUnitWaiting(const awe::UnitID id) const;

		/**
		 * Sets a unit's capturing state.
		 * @param id      The ID of the unit to amend.
		 * @param capturing \c TRUE if the unit should capture, \c FALSE if not.
		 */
		void unitCapturing(const awe::UnitID id, const bool capturing);

		/**
		 * Gets a unit's capturing state.
		 * @param  id The ID of the unit to query.
		 * @return \c TRUE if the unit is capturing, \c FALSE if it is not.
		 *         \c FALSE if the unit does not exist.
		 */
		bool isUnitCapturing(const awe::UnitID id) const;

		/**
		 * Sets a unit's hiding state.
		 * @param id     The ID of the unit to amend.
		 * @param hiding \c TRUE if the unit should hide, \c FALSE if not.
		 */
		void unitHiding(const awe::UnitID id, const bool hiding);

		/**
		 * Gets a unit's hiding state.
		 * @param  id The ID of the unit to query.
		 * @return \c TRUE if the unit is hiding, \c FALSE if it is not. \c FALSE
		 *         if the unit does not exist.
		 */
		bool isUnitHiding(const awe::UnitID id) const;

		/**
		 * Figures out if a given unit is visible on the map from the perspective
		 * of the given army.
		 * @param  unit The ID of the unit to check.
		 * @param  army The ID of the army who is trying to see the unit. Can be
		 *              \n NO_ARMY.
		 * @return \c TRUE if \c army can see \c unit on the map,
		 *         \c FALSE otherwise.
		 */
		bool isUnitVisible(const awe::UnitID unit, const awe::ArmyID army) const;

		/**
		 * Loads one unit onto another.
		 * If the unit to load is already loaded onto another unit, this call will
		 * be ignored and a warning will be logged. If both unit IDs given are the
		 * same, then an error will be logged and the call will be cancelled.
		 * @warning This method does not consider any load limits imposed by any
		 *          unit type.
		 * @param   load The ID of the unit to load onto another.
		 * @param   onto The ID of the unit which will load the unit.
		 */
		void loadUnit(const awe::UnitID load, const awe::UnitID onto);

		/**
		 * Unloads one unit onto a given tile.
		 * If the given location is out of bounds or occupied, then the operation
		 * will be cancelled. If \c unload is not loaded onto \c from, then the
		 * call will be logged.
		 * @param unload The ID of the unit to unload.
		 * @param from   The ID of the unit which houses the unit to unload.
		 * @param onto   The X and Y location to move the unloaded unit to.
		 */
		void unloadUnit(const awe::UnitID unload, const awe::UnitID from,
			const sf::Vector2u& onto);

		/**
		 * Gets the ID of the unit that a given unit is loaded on, if any.
		 * If the given unit doesn't exist, then an error will be logged and
		 * \c awe::NO_UNIT will be returned.
		 * @param  unit The ID of the unit which should be loaded onto another.
		 * @return If \c unit is loaded onto another unit, then the latter's ID
		 *         will be returned. Otherwise, \c awe::NO_UNIT will be returned.
		 */
		awe::UnitID getUnitWhichContainsUnit(const awe::UnitID unit) const;

		/**
		 * Gets the ID of the unit that a given unit is loaded on, directly or
		 * indirectly.
		 * Gets the ID of the unit that has a valid position on the map and that
		 * either directly or indirectly contains the given unit. If the given unit
		 * is not loaded onto any other unit, then \c awe::NO_UNIT will be
		 * returned.
		 * @param  unit The ID of the unit which should be loaded onto another.
		 * @return If the given unit is loaded, this method will find the unit it
		 *         is loaded on, and if that unit is loaded, it will then find the
		 *         unit \em it is loaded on, etc. When an unloaded unit is
		 *         discovered, its ID will be returned. If the given unit was
		 *         present, but unloaded, \c unit will be returned.
		 */
		awe::UnitID getUnloadedUnitWhichContainsUnit(const awe::UnitID unit) const;

		/**
		 * Finds out if a unit is loaded onto another.
		 * @param  unit The ID of the unit to test for.
		 * @param  on   The ID of the unit which should have \c unit on it.
		 * @return \c TRUE if \c unit is loaded onto \c on, \c FALSE otherwise.
		 */
		bool isUnitLoadedOntoUnit(const awe::UnitID unit,
			const awe::UnitID on) const;

		/**
		 * Retrieves the ID of the army a specified unit belongs to.
		 * <tt>awe::army::NO_ARMY</tt> will be returned if the unit doesn't exist.
		 * @param  id The ID of the unit to inspect.
		 * @return The ID of the army that owns this unit.
		 */
		awe::ArmyID getArmyOfUnit(const awe::UnitID id) const;

		/**
		 * Retrieves the ID of the team a specified unit belongs to.
		 * <tt>0</tt> will be returned if the unit doesn't exist.
		 * @param  id The ID of the unit to inspect.
		 * @return The ID of the team of the army that owns this unit.
		 */
		awe::TeamID getTeamOfUnit(const awe::UnitID id) const;

		/**
		 * Retrieves the units that are loaded onto a specified one.
		 * @param  id The ID of the unit to query.
		 * @return The IDs of the loaded units. An empty set is returned if the
		 *         given ID was invalid.
		 */
		std::unordered_set<awe::UnitID> getLoadedUnits(const awe::UnitID id) const;

		/**
		 * Version of \c getLoadedUnits() that converts the returned set into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getLoadedUnits().
		 */
		CScriptArray* getLoadedUnitsAsArray(const awe::UnitID id) const;

		/**
		 * Finds out the number of defence stars this unit has based on its type
		 * and current position.
		 * Loaded units will always give a defence rating of \c 0, as they are not
		 * internally positioned on any tile.
		 * @param  id The ID of the unit to get the defence of.
		 * @return The defence rating that this unit currently obtains.
		 */
		unsigned int getUnitDefence(const awe::UnitID id) const;

		/**
		 * Sets a given unit's main spritesheet.
		 * Logs an error if the given unit or spritesheet does not exist, and will
		 * not make any changes if either of these are the case.
		 * @param id   The ID of the unit to update.
		 * @param name The name of the spritesheet to assign.
		 */
		void setUnitSpritesheet(const awe::UnitID id, const std::string& name);

		/////////////////////
		// TILE OPERATIONS //
		/////////////////////
		/**
		 * Sets a specified tile's type.
		 * By default, a tile does not have a type, unless it was given in the call
		 * to \c setMapSize().\n
		 * Changing a tile's type will automatically remove any ownership of the
		 * tile, and will reset its HP to the maximum allowed by the given type.
		 * @param  pos  The X and Y coordinate of the tile to change.
		 * @param  type The type to assign to the tile.
		 * @return \c TRUE if setting the tile's type was successful, \c FALSE
		 *         otherwise.
		 */
		bool setTileType(const sf::Vector2u& pos,
			const std::shared_ptr<const awe::tile_type>& type);

		/**
		 * Version of \c setTileType() which accepts a tile type script name.
		 */
		bool setTileType(const sf::Vector2u& pos, const std::string& type);

		/**
		 * Without setting a tile type, query which tiles will be changed from the
		 * operation.
		 * @param  pos The X and Y coordinate of the tile that will be changing.
		 * @return A set of the tiles that will be changing. If the given tile
		 *         doesn't form part of a structure the result will always be a set
		 *         with \c pos and no other members.
		 */
		std::unordered_set<sf::Vector2u> querySetTileTypeChangedTiles(
			const sf::Vector2u& pos) const;

		/**
		 * Version of \c querySetTileTypeChangedTiles() that converts the result
		 * into a \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c querySetTileTypeChangedTiles().
		 */
		CScriptArray* querySetTileTypeChangedTilesAsArray(
			const sf::Vector2u& pos) const;

		/**
		 * Retrieves the specified tile's type.
		 * If an out of bounds coordinate is given, \c nullptr is returned.
		 * @param  pos The X and Y coordinate of the tile to query.
		 * @return The type of the tile and its information.
		 */
		std::shared_ptr<const awe::tile_type> getTileType(
			const sf::Vector2u& pos) const;

		/**
		 * Script version of \c getTileType().
		 * @throws std::out_of_range if the given tile was out of bounds.
		 * @sa     @c getTileType().
		 */
		const awe::tile_type* getTileTypeObject(const sf::Vector2u& pos) const;

		/**
		 * Sets a tile's HP.
		 * If a negative value is given, it will be adjusted to \c 0.
		 * @param pos The X and Y coordinate of the tile to change.
		 * @param hp  The HP to assign to the tile.
		 */
		void setTileHP(const sf::Vector2u& pos, const awe::HP hp);

		/**
		 * Retrieves a tile's HP.
		 * @param  pos The X and Y coordinate of the tile to retrieve from.
		 * @return The HP of the tile, or \c 0 if the given coordinate was out of
		 *         bounds.
		 */
		awe::HP getTileHP(const sf::Vector2u& pos) const;

		/**
		 * Sets a tile's owner.
		 * @param pos  The X and Y coordinate of the tile to change.
		 * @param army The ID of the army who now owns this tile.
		 *             \c awe::army::NO_ARMY can be given to signal that the tile
		 *             should not have an owner.
		 */
		void setTileOwner(const sf::Vector2u& pos, awe::ArmyID army);

		/**
		 * Gets a tile's owner.
		 * If the coorindate is out of bounds, \c awe::army::NO_ARMY is returned.
		 * @param  pos The X and Y coordinate of the tile to query.
		 * @return The ID of the army who owns this tile, or \c awe::army::NO_ARMY
		 *         if no army owns it.
		 */
		awe::ArmyID getTileOwner(const sf::Vector2u& pos) const;

		/**
		 * Retrieves the unit currently occupying a specified tile.
		 * This method will return \c awe::NO_UNIT if there is a unit on this tile,
		 * but it is not physically on the map, such as if the unit is loaded onto
		 * another unit.
		 * @param  pos The X and Y coordinate of the tile to inspect.
		 * @return The ID of the unit occupying this tile. \c awe::NO_UNIT if the
		 *         tile is vacant or out of bounds.
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u& pos) const;

		/**
		 * Sets a tile's structure information.
		 * If a tile forms part of a structure, that structure's type and the tile
		 * offset should be stored with that tile. This is to ensure structure
		 * deletion works correctly in \c setTileType().\n
		 * Will log errors (and not make any changes to the map) if the given tile
		 * is out-of-bounds, or if <tt>tile + offset</tt> is out-of-bounds.\n
		 * Will log warnings (but still permit changes) if the given tile's type
		 * doesn't match with what the structure expects, or if the given offset
		 * did not describe a tile in the structure.
		 * @param pos       The X and Y coordinate of the tile that's part of a
		 *                  structure.
		 * @param structure The type of structure this tile's a part of. \c nullptr
		 *                  to set no structure.
		 * @param offset    Which tile from the structure this tile's a part of,
		 *                  defined as the offset from the root tile.
		 * @param destroyed \c TRUE if this structure is destroyed, \c FALSE if
		 *                  not.
		 */
		void setTileStructureData(const sf::Vector2u& pos,
			const std::shared_ptr<const awe::structure>& structure,
			const sf::Vector2i& offset, const bool destroyed);

		/// Script interface version of \c setTileStructureData().
		/// Provide an empty string to set no structure.
		void setTileStructureData(const sf::Vector2u& pos,
			const std::string& structure, const sf::Vector2i& offset,
			const bool destroyed);

		/**
		 * Retrieves the specified tile's registered structure type.
		 * If an out of bounds coordinate is given, \c nullptr is returned.
		 * @param  pos The X and Y coordinate of the tile to query.
		 * @return The type of structure this tile is a part of. If this tile is
		 *         not part of a structure, will return \c nullptr.
		 */
		std::shared_ptr<const awe::structure> getTileStructure(
			const sf::Vector2u& pos) const;

		/**
		 * Script version of \c getTileStructure().
		 * @throws std::out_of_range if the given tile was out of bounds or if the
		 *                           tile wasn't part of a structure.
		 * @sa     @c getTileStructure().
		 */
		const awe::structure* getTileStructureObject(
			const sf::Vector2u& pos) const;
		
		/**
		 * Does the given tile form part of a structure?
		 * @param  pos The tile to query.
		 * @return \c TRUE if the given tile forms part of a structure, \c FALSE
		 *         otherwise.
		 */
		bool isTileAStructureTile(const sf::Vector2u& pos) const;

		/**
		 * Finds out which tile the specified tile is in its structure.
		 * If an out of bounds coordinate is given, <tt>(0, 0)</tt> is returned.
		 * @param  pos The X and Y coordinate of the tile to query.
		 * @return The offset, in tiles, that this tile is from its structure's
		 *         root tile.
		 */
		sf::Vector2i getTileStructureOffset(const sf::Vector2u& pos) const;

		/**
		 * Finds out if this tile is destroyed or not.
		 * Note that this method should be used in conjunction with
		 * \c isTileAStructureTile(), as this method will simply query the
		 * destroyed flag directly.
		 * @param  pos The X and Y coordinate of the tile to query.
		 * @return \c TRUE if this tile's destroyed flag is \c TRUE, \c FALSE if it
		 *         is \c FALSE.
		 */
		bool isTileDestroyed(const sf::Vector2u& pos) const;

		/**
		 * Figures out if a given tile is visible on the map from the perspective
		 * of the given army.
		 * @param  pos  The X and Y coordinate of the tile to check.
		 * @param  army The ID of the army who is trying to see the tile.
		 * @return \c TRUE if \c army can see \c tile on the map,
		 *         \c FALSE otherwise. Will always return \c TRUE if Fog of War is
		 *         disabled, unless the given tile is out-of-bounds or the given
		 *         army doesn't exist, which case this will always return \c FALSE.
		 *         No error will be logged if \c NO_ARMY is given. If \c NO_ARMY is
		 *         given, the result of \c !isFoWEnabled() will be returned.
		 */
		bool isTileVisible(const sf::Vector2u& pos, const awe::ArmyID army) const;

		/**
		 * Calculates the tiles available from a specified tile.
		 * @param  tile      The tile to calculate from.
		 * @param  startFrom The number of tiles away from the given tile to start
		 *                   counting from. If \c 0 is given, it will be increased
		 *                   to \c 1.
		 * @param  endAt     All tiles within the given distance will be considered
		 *                   for addage to the result.
		 * @return The set of tiles that are within range. An empty set will be
		 *         returned if the given tile was out of bounds (an error will be
		 *         logged), or if \c startFrom was larger than \c endAt.
		 */
		std::unordered_set<sf::Vector2u> getAvailableTiles(
			const sf::Vector2u& tile, unsigned int startFrom,
			const unsigned int endAt) const;

		/**
		 * Version of \c getAvailableTiles() which converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getAvailableTiles().
		 */
		CScriptArray* getAvailableTilesAsArray(const sf::Vector2u& tile,
			const unsigned int startFrom, const unsigned int endAt) const;

		/**
		 * Returns a list of tiles that are within a cone-shape from the given
		 * tile.
		 * Using the \ Up direction to illustrate; a cone starts at the given tile.
		 * The next row of the cone will then be one tile above the given tile, and
		 * will be three tiles in width, centring on the origin tile. The row above
		 * that will be five tiles in width, again centring on the origin tile, and
		 * so on. A cone that starts from 0 and ends at 3 will look like this:
		 * <code>
		 * ---------
		 * -XXXXXXX-
		 * --XXXXX--
		 * ---XXX---
		 * ----O----
		 * ---------
		 * </code>
		 * Where \c O represents the origin tile, and \c X represents tiles within
		 * the cone. In this case, the origin tile is included in the cone. A cone
		 * that ends at 3 but starts at 2 will look like this:
		 * <code>
		 * ---------
		 * -XXXXXXX-
		 * --XXXXX--
		 * ---------
		 * ----O----
		 * ---------
		 * </code>
		 * And in this case, the origin tile is not included in the cone.
		 * @param  tile      The first tile of the cone.
		 * @param  dir       The direction the tiles will be 'drawn' towards.
		 * @param  startFrom The number of tiles away from the given tile to begin
		 *                   the cone. Essentially, the cone is 'drawn' as normal,
		 *                   then the first \c startFrom rows/columns will be
		 *                   removed from the result.
		 * @param  endAt     The number of rows/columns to draw the cone from the
		 *                   origin tile.
		 * @return A list of tiles within the cone.
		 */
		std::unordered_set<sf::Vector2u> getTilesInCone(sf::Vector2u tile,
			const direction dir, const unsigned int startFrom,
			const unsigned int endAt) const;

		/**
		 * Version of \c getTilesInCone() that converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesInCone().
		 */
		CScriptArray* getTilesInConeAsArray(const sf::Vector2u& tile,
			const direction dir, const unsigned int startFrom,
			const unsigned int endAt) const;

		/**
		 * Returns a list of tiles that share either an X or Y coordinate with a
		 * given tile.
		 * @param  tile The tile to measure from.
		 * @return A list of tiles that form a crosshair, centred on the given
		 *         tile.
		 */
		std::unordered_set<sf::Vector2u> getTilesInCrosshair(
			const sf::Vector2u& tile) const;

		/**
		 * Version of \c getTilesInCrosshair() that converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesInCrosshair().
		 */
		CScriptArray* getTilesInCrosshairAsArray(const sf::Vector2u& tile) const;

		/**
		 * Returns a list of tiles that are on the same X or Y axis as the given
		 * tile, in the given direction.
		 * @param  tile     The tile to traverse from. This tile will be included
		 *                  in the result.
		 * @param  dir      The direction to traverse in, from the given tile.
		 * @param  distance The number of tiles to traverse. Default (0) means that
		 *                  the method will travel to the end of the map.
		 * @return Returns the line of tiles.
		 */
		std::unordered_set<sf::Vector2u> getTilesInLine(sf::Vector2u tile,
			const direction dir, unsigned int distance = 0) const;

		/**
		 * Version of \c getTilesInLine() that converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesInLine().
		 */
		CScriptArray* getTilesInLineAsArray(const sf::Vector2u& tile,
			const direction dir, const unsigned int distance = 0) const;

		/**
		 * Returns a rectangle of tiles that are within two given tiles.
		 * If the given tiles are out-of-bounds, they will be adjusted to be within
		 * bounds.
		 * @param  tile1 One of the corners of the rectangle.
		 * @param  tile2 The opposite corner of \c tile1.
		 * @return Returns a selection of tiles that will include the corner tiles.
		 */
		std::unordered_set<sf::Vector2u> getTilesInArea(sf::Vector2u tile1,
			sf::Vector2u tile2) const;

		/**
		 * Version of \c getTilesInArea() that converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesInArea().
		 */
		CScriptArray* getTilesInAreaAsArray(const sf::Vector2u& tile1,
			const sf::Vector2u& tile2) const;

		/**
		 * Finds the shortest path from the origin to the destination.
		 * If \c nullptr is given for a pointer parameter, their respective check
		 * will be ignored. For example, if \c fuel is \c nullptr, it can be
		 * assumed that there are an infinite number of units of fuel available. Or
		 * if \c team is \c nullptr, then any occupied tile will be considered
		 * impassable, even if the tile is occupied by a unit on the same team.
		 * @param  origin           The origin tile.
		 * @param  dest             The intended destination.
		 * @param  moveType         The movement type used for traversal
		 *                          calculations.
		 * @param  movePoints       Pointer to the movement points available.
		 * @param  fuel             Pointer to the units of fuel that we have to
		 *                          work with.
		 * @param  team             Pointer to the team the moving unit is on.
		 * @param  army             Pointer to the army the moving unit is on.
		 * @param  hasInfiniteFuel  Does the actor that is moving have infinite
		 *                          fuel?
		 * @param  ignoreUnitChecks If \c TRUE, a tile will be traversable even if
		 *                          a unit is positioned on it.
		 * @param  ignoredUnits     A set of units that can be moved through,
		 *                          regardless of team.
		 * @return The shortest path, if a path could be found. An empty vector if
		 *         no path could be found.
		 */
		std::vector<awe::closed_list_node> findPath(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const unsigned int* const movePoints, const awe::Fuel* const fuel,
			const awe::TeamID* const team, const awe::ArmyID* const army,
			const bool hasInfiniteFuel, const bool ignoreUnitChecks,
			const std::unordered_set<awe::UnitID>& ignoredUnits) const;

		/**
		 * Version of \c findPath() which converts the result into a
		 * \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c findPath().
		 */
		CScriptArray* findPathAsArray(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const unsigned int movePoints, const awe::Fuel fuel,
			const awe::TeamID team, const awe::ArmyID army,
			const bool hasInfiniteFuel, const bool ignoreUnitChecks,
			const CScriptArray* const ignoredUnits) const;

		/**
		 * Version of \c findPath() which passes in \c nullptr where possible, and
		 * converts the result into a \c CScriptArray.
		 * @throws @c std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c findPath().
		 */
		CScriptArray* findPathAsArrayUnloadUnit(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const awe::ArmyID army, const CScriptArray* const ignoredUnits) const;

		/**
		 * Finds out if there is an obstruction in a given path.
		 * As of right now, an obstruction in the context of this method is defined
		 * \em only as an invisible unit that does not belong to the same team as
		 * the moving unit. Any other type of obstruction must be handled in the
		 * \c findPath() method.\n
		 * The path is scanned, from the first tile to the last tile. If an
		 * obstruction is found along the path, then its index will be returned. If
		 * one could not be found, then \c <0 will be returned.
		 * @param  path    The path to scan.
		 * @param  unit    The unit that is moving along the path.
		 * @param  ignores This stores the number of obstructions that will be
		 *                 ignored when performing the search. This effectively
		 *                 acts as an index, where if \c ignores is \c 1, the
		 *                 second obstruction will be returned, if any. Etc.
		 * @return The index to the obstruction, if any.
		 */
		int scanPath(CScriptArray* path, const awe::UnitID unit,
			std::size_t ignores = 0) const;

		/**
		 * Converts tiles from a list to a given tile type if their tile type
		 * matches a given tile type.
		 * For every out-of-bounds tile given, an error will be logged.
		 * @param tiles             The tiles that could be converted.
		 * @param fromTileType      If a tile from \c tiles is of this tile type,
		 *                          it will be converted.
		 * @param toTileType        The tile type to assign to converted tiles.
		 * @param transferOwnership If the tile is converted, its owner will also
		 *                          be set to the given army.
		 */
		void convertTiles(const std::vector<sf::Vector2u>& tiles,
			const std::shared_ptr<const awe::tile_type>& fromTileType,
			const std::shared_ptr<const awe::tile_type>& toTileType,
			const awe::ArmyID transferOwnership);

		/// Script interface version of \c convertTiles().
		void convertTiles(const CScriptArray* const tiles,
			const std::string& fromTileType, const std::string& toTileType,
			const awe::ArmyID transferOwnership);

		/**
		 * Can the given structure fit when placed from the given tile?
		 * @param  fromTile  The root tile of the structure will be placed here.
		 * @param  structure The structure to place.
		 * @return \c TRUE if the root and dependent tiles can all fit within the
		 *         map, \c FALSE if not.
		 */
		bool canStructureFit(const sf::Vector2u& fromTile,
			const std::shared_ptr<const awe::structure>& structure) const;

		/// Script interface version of \c canStructureFit().
		bool canStructureFit(const sf::Vector2u& fromTile,
			const std::string& structure) const;

		/**
		 * Destroy a structure attached to a given tile.
		 * Destroying a structure will convert all of its tiles into its
		 * configured destroyed tile types, and all tiles pertaining to the
		 * structure will be disowned. However, the tiles will still be registered
		 * as being part of a structure internally. This is to ensure that the
		 * deletion of a structure within \c setTileType() is handled correctly.
		 * @param tile The tile that is attached to a structure.
		 */
		void destroyStructure(sf::Vector2u tile);

		/**
		 * Delete a structure attached to a given tile.
		 * Deleting a structure will convert all of its tiles into its
		 * configured deleted tile types, and all tiles pertaining to the structure
		 * will be disowned. Additionally, each tile in the structure will have its
		 * structure data removed.\n
		 * Calling \c setTileType() will automatically delete the structure before
		 * updating the tile type. Use this method when you don't wish to update
		 * the type of the given tile beyond what's changed when deleting the
		 * structure.
		 * @param tile The tile that is attached to a structure.
		 */
		void deleteStructure(sf::Vector2u tile);

		/**
		 * Is the given tile type the root tile of a non-paintable structure?
		 * @param  type The type of tile to test.
		 * @return If there is a structure that is not paintable, and whose root
		 *         tile is the given tile type, return a pointer to its properties.
		 *         \c nullptr is returned if no structure could be found that
		 *         matches the criteria.
		 */
		std::shared_ptr<const awe::structure> getTileTypeStructure(
			const std::shared_ptr<const awe::tile_type>& type) const;

		/// Version of \c getTileTypeStructure() that accepts a script name to a
		/// tile type, and returns either the script name of the found structure,
		/// or an empty string if no structure was found.
		std::string getTileTypeStructure(const std::string& type) const;

		//////////////////////////////////////
		// SELECTED UNIT DRAWING OPERATIONS //
		//////////////////////////////////////
		/**
		 * Selects a unit on the map.
		 * If an invalid unit ID is given, the call won't have any effect, and it
		 * will be logged.\n
		 * Note that no rendering effects for available tiles or a selected unit,
		 * etc., will be employed if the currently selected unit is
		 * \c awe::NO_UNIT.
		 * @param  unit The ID of the unit to select. If \c awe::NO_UNIT is given,
		 *              all of the selected unit rendering data will be cleared.
		 * @return \c TRUE if the unit was successfully selected, \c FALSE if not.
		 */
		bool setSelectedUnit(const awe::UnitID unit);

		/**
		 * Selects a new unit on the map and remembers the previously selected
		 * unit/s.
		 * If \c FALSE is returned, the stack's new element will be removed.
		 * @param  unit The ID of the unit to select.
		 * @return \c TRUE if the unit was selected successfully, \c FALSE if not.
		 * @sa     @c setSelectedUnit().
		 */
		bool pushSelectedUnit(const awe::UnitID unit);

		/**
		 * Pops the currently selected unit from the stack and reselects the
		 * unit that was selected before the popped one.
		 * A call to this method will fail if there is only one element in the
		 * stack.
		 * @warning This is \em not used to deselect the current unit! To do that,
		 *          call <tt>setSelectedUnit(awe::NO_UNIT)</tt>.
		 * @warning If the newly selected unit is no longer present after the pop,
		 *          then \c setSelectedUnit(awe::NO_UNIT) will be called and a
		 *          warning will be logged.
		 */
		void popSelectedUnit();

		/**
		 * Gets the currently selected unit.
		 * @return The ID of the currently selected unit. \c awe::NO_UNIT if no
		 *         unit is selected.
		 */
		awe::UnitID getSelectedUnit() const;

		/**
		 * Adds a tile to the available tile set.
		 * If the given tile was out of bounds, the call will be logged and the
		 * tile won't be added.
		 * @param tile The tile to add. If a tile is given that was already in the
		 *             set, the tile won't be added again.
		 */
		void addAvailableTile(const sf::Vector2u& tile);

		/**
		 * Finds out if a given tile was previously added to the available tiles
		 * set.
		 * If the given tile was out of bounds, the call will be logged and
		 * \c FALSE will be returned.
		 * @param  tile The tile to search for.
		 * @return \c TRUE if \c tile was added as an available tile, \c FALSE
		 *         otherwise.
		 */
		bool isAvailableTile(const sf::Vector2u& tile) const;

		/**
		 * Removes all available tiles.
		 */
		void clearAvailableTiles();

		/**
		 * Sets the shader to use for available tiles.
		 * @param shader The shader to use.
		 */
		void setAvailableTileShader(const awe::available_tile_shader shader);

		/**
		 * Gets the shader currently used for available tiles.
		 * @return The shader in use.
		 */
		awe::available_tile_shader getAvailableTileShader() const;

		/**
		 * Returns a pointer to the closed list.
		 * The returned array has elements of type \c awe::closed_list_node.
		 * @return Pointer to the \c CScriptArray holding the closed list.
		 * @sa     @c setScripts().
		 */
		CScriptArray* getClosedList();

		/**
		 * Disables rendering effects for a selected unit without deselecting the
		 * unit.
		 * Note that this does not include unit location overrides!
		 * @param val \c TRUE if the rendering effects for selected units are to be
		 *            disabled, \c FALSE if they are to be enabled.
		 */
		void disableSelectedUnitRenderingEffects(const bool val);

		/**
		 * Disables greyed out unit shading for units that are on an available
		 * tile.
		 */
		void disableShaderForAvailableUnits(const bool val);

		/**
		 * Iterates through the closed list in its current state and updates each
		 * node's animated sprite based on the stored path.
		 */
		void regenerateClosedListSprites();

		/**
		 * Add a unit location override.
		 * When the given unit is rendered, it will be rendered at the tile \c pos
		 * instead of its real location.\n
		 * If the location is out of range, an error will be logged and the mapping
		 * won't be added. If the unit ID is out of range, the same thing will
		 * occur.\n
		 * If the location is \c awe::unit::NO_POSITION, then the unit will never
		 * be rendered until that override is removed. This can be used to
		 * temporarily hide units from the player whilst keeping their data
		 * intact.\n
		 * If there is already an override for the given unit, the previous
		 * override will be replaced only if the new override is valid.
		 * @warning If a unit is given a location override, it will be rendered
		 *          even if the unit would otherwise not be, such as if it is
		 *          loaded or hidden!
		 * @param   unit The ID of the unit to provide an override for.
		 * @param   pos  The tile to render the unit at.
		 */
		void addPreviewUnit(const awe::UnitID unit, const sf::Vector2u& pos);

		/**
		 * Removes a unit location override.
		 * If a mapping for the given unit does not exist, an error will be logged
		 * and the state of the map will not change.
		 * @param unit ID of the unit to remove the location override for.
		 */
		void removePreviewUnit(const awe::UnitID unit);

		/**
		 * Removes all unit location overrides.
		 */
		void removeAllPreviewUnits();

		/**
		 * Finds out if a unit has a location override.
		 * @param  unit ID of the unit to query.
		 * @return \c TRUE if a unit has a mapping that was previously given,
		 *         \c FALSE otherwise.
		 */
		inline bool isPreviewUnit(const awe::UnitID unit) const {
			return _unitLocationOverrides.find(unit) !=
				_unitLocationOverrides.end();
		}

		/**
		 * Finds out if a tile has a given unit on it via a location override.
		 * @param  tile The tile to query.
		 * @param  unit The ID of the unit that the given tile should be tested
		 *              for.
		 * @return \c TRUE if \c unit maps to \c tile, \c FALSE otherwise.
		 */
		inline bool tileHasPreviewUnit(const sf::Vector2u& tile,
			const awe::UnitID unit) const {
			return isPreviewUnit(unit) && _unitLocationOverrides.at(unit) == tile;
		}

		/**
		 * Retrieves the location override assigned to a unit.
		 * @param  unit The ID of the unit to search for.
		 * @return The assigned location override, or the unit's real location if
		 *         it has no location override.
		 */
		inline sf::Vector2u getUnitPreviewPosition(const awe::UnitID unit) const {
			return (isPreviewUnit(unit)) ? (_unitLocationOverrides.at(unit)) :
				(getUnitPosition(unit));
		}

		/**
		 * Retrieves the number of unit location overrides currently configured.
		 * @return The number of unit location overrides currently configured.
		 */
		std::size_t getUnitPreviewsCount() const;

		////////////////////////
		// MEMENTO OPERATIONS //
		////////////////////////
		/**
		 * Captures the current state of the \c map object, stores it in a binary
		 * output stream, and pushes it to the front of the undo deque.
		 * Additionally, the redo deque is cleared.\n
		 * If the number of mementos in the undo deque exceeds the limit, then the
		 * back memento will be popped.\n
		 * If \c disableMementos() has been called at least once, then a call to
		 * this method will do nothing. For each call to \c disableMementos(), a
		 * call to \c enableMementos() needs to be made, before \c addMemento() can
		 * add mementos again.
		 * @param  name The name to give to the new memento.
		 * @safety If an exception is thrown when writing the map's data, both
		 *         deques will not be changed.
		 * @sa     \c disableMementos().
		 */
		inline void addMemento(const std::string& name) {
			if (_mementoDisableCounter == 0) _createMemento(name);
		}

		/**
		 * Pops memento off the front of the undo deque, pushes it to the front of
		 * the redo deque, and replaces the map's state with the next memento in
		 * the undo deque.
		 * Does nothing if the undo deque has only one memento stored (or none).\n
		 * If the number of undos specified is too much, then it will be reduced to
		 * the maximum possible value and a warning will be logged.
		 * @param additionalUndos If more than \c 0 is given, it defines the number
		 *                        of additional mementos to pop off the undo deque
		 *                        and push on to the redo deque before replacing
		 *                        the map's state with the next memento in the undo
		 *                        deque.
		 */
		void undo(std::size_t additionalUndos = 0);

		/**
		 * Pops memento off the front of the redo deque, pushes it to the front of
		 * the undo deque, and replaces the map's state with that memento.
		 * Does nothing if the redo deque is empty.\n
		 * If the number of redos specified is too much, then it will be reduced to
		 * the maximum possible value and a warning will be logged.
		 * @param additionalRedos If more than \c 0 is given, it defines the number
		 *                        of additional mementos to pop off the redo deque
		 *                        and push on to the undo deque before replacing
		 *                        the map's state with the last memento that was
		 *                        popped then pushed.
		 */
		void redo(std::size_t additionalRedos = 0);

		/**
		 * Prevents \c addMemento() from creating mementos.
		 * Used to prevent many mementos from being created when performing larger
		 * operations (such as fill operations), so that an entire fill can be
		 * undone at once instead of having to undo each new unit (and each
		 * property that is set for each unit) individually, for example.\n
		 * Each call to \c disableMementos() must be paired with a call to
		 * \c enableMementos().
		 * @sa \c enableMementos().
		 */
		inline void disableMementos() {
			++_mementoDisableCounter;
		}

		/**
		 * Allows \c addMemento() to create mementos again if the number of calls
		 * to \c disableMementos() matches the number of calls to this method.
		 * This means that, if \c disableMementos() is called twice,
		 * \c enableMementos() must also be called twice before \c addMemento() can
		 * create mementos again. On the second call to \c enableMementos(), a
		 * memento will be created automatically.
		 * @param  name The name to give the created memento. If set to an empty
		 *              string, does not automatically create any memento.
		 * @return \c TRUE if a memento was created, \c FALSE if not.
		 * @sa     \c disableMementos().
		 */
		inline bool enableMementos(const std::string& name) {
			if (_mementoDisableCounter > 0 && --_mementoDisableCounter == 0 &&
				!name.empty()) {
				_createMemento(name);
				return true;
			} else return false;
		}

		/**
		 * Allows the scripts to tell the map that its state has changed.
		 * This flag is automatically managed by the \c map class when mementos are
		 * created and when the map is saved/loaded. However, there can be cases
		 * where you want to change the map's state without creating a memento. In
		 * such cases, it is still usually beneficial to mark the fact that the
		 * state has changed. This method can be used to achieve this.
		 */
		void markChanged();

		/**
		 * Gets the names of every memento currently in the undo and redo deque.
		 * @param  lastKnownMemento The 0-based index of the memento that is at the
		 *                          front of the undo deque is set to this
		 *                          variable.
		 * @return Returns the mementos in order of creation time, with the one
		 *         that is most recent being placed first in the vector.
		 */
		std::vector<std::string> getMementos(std::size_t& lastKnownMemento) const;

		/**
		 * Calls \c getMementos() and converts the result into a \c CScriptArray.
		 * @param  lastKnownMemento The 0-based index of the memento that is at the
		 *                          front of the undo deque is set to this
		 *                          variable.
		 * @return The array listing each memento's name in order.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 */
		CScriptArray* getMementosAsArray(std::size_t& lastKnownMemento) const;

		/**
		 * Gets the name of the next memento in the undo deque.
		 * @return The name of the memento to undo next.
		 */
		std::string getNextUndoMementoName() const;

		/**
		 * Gets the name of the next memento in the redo deque.
		 * @return The name of the memento to redo next, or a blank string if there
		 *         is none.
		 */
		std::string getNextRedoMementoName() const;

		/**
		 * Adds a script callback that is invoked when a memento is added or
		 * removed, or undone or redone.
		 * This callback is particularly useful when the scripts need to refresh
		 * data that depends on the additional data stored in the map. Mementos
		 * allow one to undo and redo the additional data itself, but since this
		 * data is stored separately by the scripts, they need to be notified when
		 * it changes, and reload their data accordingly.\n
		 * Invokes callbacks in the order they were given in.
		 * @param callback Pointer to the callback. Logs an error when \c nullptr
		 *                 is given, and does not add the \c nullptr.
		 */
		void addMementoStateChangedCallback(asIScriptFunction* const callback);

		///////////////////////
		// SCRIPT OPERATIONS //
		///////////////////////
		/**
		 * Adds a new script, or updates an existing one.
		 * @param name The name of the new script, or the name of the script to
		 *             update.
		 * @param code The code of the new script, or the new code of the existing
		 *             script.
		 */
		void addScriptFile(const std::string& name, const std::string& code);

		/**
		 * Renames an existing script file.
		 * This operation will fail if \c oldName didn't refer to an existing
		 * script, \c newName was already taken by an existing script, or both
		 * names were identical.
		 * @param oldName The old name of the script.
		 * @param newName The new name of the script.
		 */
		void renameScriptFile(const std::string& oldName,
			const std::string& newName);

		/**
		 * Removes a script file.
		 * An error will be logged if the given script does not exist.
		 * @param name The name of the script to remove.
		 */
		void removeScriptFile(const std::string& name);

		/**
		 * Builds the scripts.
		 * If the build was successful, any previous builds will be discarded.
		 * @return If an error ocurred, a message describing the reason shall be
		 *         returned. If the build was successful, an empty string will be
		 *         returned.
		 * @throws \c std::runtime_error if no \c scripts object is present at the
		 *         time of calling.
		 */
		std::string buildScriptFiles();

		/**
		 * Retrieves the last known build result.
		 * @return If the last build was successful, an empty string will be
		 *         returned. Otherwise, the error message will be returned.
		 */
		std::string getLastKnownBuildResult() const;

		/**
		 * Does a script with the given name exist?
		 * @param  name The name to test.
		 * @return \c TRUE if there is a script with the given name stored in this
		 *         map \c FALSE otherwise.
		 */
		inline bool doesScriptExist(const std::string& name) const {
			return _scriptFiles.find(name) != _scriptFiles.end();
		}

		/**
		 * Gets a script.
		 * @param  name The name of the script to retrieve.
		 * @return The code in the script, or a blank string if a script with the
		 *         given name doesn't exist. In this case, an error will be logged.
		 */
		std::string getScript(const std::string& name) const;

		/**
		 * Gets a list of the names of the scripts stored with this map.
		 * @return The name of each script stored with this map.
		 */
		std::vector<std::string> getScriptNames() const;

		/// Version of \c getScriptNames() that converts the result into a script
		/// array.
		/// @throws \c std::runtime_error if no \c scripts object is currently
		///         stored in this \c map object.
		CScriptArray* getScriptNamesAsArray() const;

		////////////////////////
		// DRAWING OPERATIONS //
		////////////////////////
		/**
		 * Sets the target used with this map.
		 * @param target Pointer to the target this map will be drawn on.
		 */
		void setTarget(const std::shared_ptr<sf::RenderTarget>& target) noexcept;

		/**
		 * Sets the viewport that the map uses.
		 * Each parameter is a value between \c 0.0 and \c 1.0 that describes how
		 * far away from each edge of the screen the viewport should start, as a
		 * fraction of the screen's size. For example, if every value was \c 0.5,
		 * the map would be invisible.\n
		 * The default viewport covers the entire screen (so each value is \c 0.0).
		 * @param left   How far away the viewport should start from the left.
		 * @param top    How far away the viewport should start from the top.
		 * @param right  How far away the viewport should start from the right.
		 * @param bottom How far away the viewport should start from the bottom.
		 */
		void setViewport(const float left, const float top, const float right,
			const float bottom);

		/**
		 * Used to always draw units that are hidden from the perspective of the
		 * current army.
		 * @param alwaysShow \c TRUE if all hidden units should be shown, \c FALSE
		 *                   to not override a unit's hidden flag.
		 */
		void alwaysShowHiddenUnits(const bool alwaysShow) noexcept;

		/**
		 * Selects a tile on the map.
		 * Used only to let \c map know what tile to draw information on, as well
		 * as where to draw the cursor. If the given location is out of bounds, the
		 * call will be ignored.
		 * @param  pos The X and Y location of the tile which is selected.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool setSelectedTile(const sf::Vector2u& pos);

		/**
		 * Moves the cursor up one tile, if possible.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool moveSelectedTileUp();

		/**
		 * Moves the cursor down one tile, if possible.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool moveSelectedTileDown();

		/**
		 * Moves the cursor left one tile, if possible.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool moveSelectedTileLeft();

		/**
		 * Moves the cursor right one tile, if possible.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool moveSelectedTileRight();

		/**
		 * Selects a tile on the map based on a pixel.
		 * The tile that is drawn underneath the pixel will be selected, if there
		 * is one.\n
		 * Note that if \c _target is \c nullptr this method will have no effect.
		 * @param  pixel The pixel relative to the render target to use to identify
		 *               a tile to select.
		 * @return \c TRUE if the selected tile was changed, \c FALSE if not.
		 */
		bool setSelectedTileByPixel(const sf::Vector2i& pixel);

		/**
		 * Gets the position of the currently selected tile.
		 * @return The X and Y location of the selected tile.
		 */
		inline sf::Vector2u getSelectedTile() const {
			return _sel;
		}

		/**
		 * Selects an additional tile on the map.
		 * You can use this mechanism to selected two tiles at once. You should
		 * primarily use \c setSelectedTile() as this additional cursor is for
		 * visual effect only.
		 * @param pos The X and Y location of the tile which is additionally
		 *            selected.
		 */
		void setAdditionallySelectedTile(const sf::Vector2u& pos);

		/**
		 * Removes the additionally selected tile cursor.
		 * Also resets the animation of the additional cursor.
		 */
		void clearAdditionallySelectedTile();

		/**
		 * Sets which sprite from the icon spritesheet to use as the UL corner of
		 * the additionally selected tile cursor.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the UL corner of the cursor will not
		 *          show up!
		 * @param   sprite The animated sprite to use as the UL corner.
		 */
		void setULAdditionalCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the UR corner of
		 * the additionally selected tile cursor.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the UR corner of the cursor will not
		 *          show up!
		 * @param   sprite The animated sprite to use as the UR corner.
		 */
		void setURAdditionalCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the LL corner of
		 * the additionally selected tile cursor.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the LL corner of the cursor will not
		 *          show up!
		 * @param   sprite The animated sprite to use as the LL corner.
		 */
		void setLLAdditionalCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the LR corner of
		 * the additionally selected tile cursor.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the LR corner of the cursor will not
		 *          show up!
		 * @param   sprite The animated sprite to use as the LR corner.
		 */
		void setLRAdditionalCursorSprite(const std::string& sprite);

		/**
		 * Selects an army from the map.
		 * If the given army didn't exist, the call will be cancelled and logged.
		 * @param army The ID of the army which should be having their turn at the
		 *             time of the call.
		 */
		void setSelectedArmy(const awe::ArmyID army);

		/**
		 * Returns the ID of the currently selected army.
		 * @return The current army.
		 */
		awe::ArmyID getSelectedArmy() const;

		/**
		 * Returns the ID of the army that should be selected next.
		 * @return The ID of the army after the current one, or the ID of the first
		 *         army if the current army is the last one in the list. Returns
		 *         \c NO_ARMY if \c _currentArmy is \c NO_ARMY.
		 */
		awe::ArmyID getNextArmy() const;

		/**
		 * Find the ID of an existing army that immediately proceeds the one given.
		 * @param  from The ID of the army to search from. May or may not exist.
		 * @return The ID of the army that follows the one given in the turn order.
		 *         \c NO_ARMY if there are no armies.
		 */
		awe::ArmyID findNextArmy(const awe::ArmyID from) const;

		/**
		 * Returns the ID of the army that should have their turn first.
		 * @return The ID of the first army, or \c NO_ARMY if there are no armies.
		 */
		awe::ArmyID getFirstArmy() const;

		/**
		 * Returns the ID of the army that should have their turn last.
		 * @return The ID of the last army, or \c NO_ARMY if there are no armies.
		 */
		awe::ArmyID getLastArmy() const;

		/**
		 * Force the map to render as if the current army was \c army, and not the
		 * actually selected army.
		 * If an invalid army is given, an error will be logged and nothing will
		 * change. You can give \c NO_ARMY.\n
		 * If the given army is later deleted, the override will be cleared.
		 * @param army ID of the army to override the selected army with.
		 */
		void setSelectedArmyOverride(const awe::ArmyID army);

		/**
		 * If there is a selected army override, return it.
		 * Otherwise, return the result of \c getSelectedArmy(). Should be used
		 * with rendering code, both in \c map and in the scripts.
		 * @return The current army override, or the current army if there is no
		 *         override.
		 */
		inline awe::ArmyID getOverriddenSelectedArmy() const {
			return
				_currentArmyOverride ? *_currentArmyOverride : getSelectedArmy();
		}

		/**
		 * Ensure the map is rendered as normal, without any selected army
		 * override.
		 */
		inline void clearSelectedArmyOverride() {
			_currentArmyOverride = std::nullopt;
		}

		/**
		 * Sets the amount by which the map is scaled.
		 * If it is detected that a value at or below \c 0.0f is given, an error
		 * will be logged and \c _scaling will not be changed.
		 * @param factor  The factor by which to scale the map.
		 * @param animate \c TRUE if the "zoom" is to be animated, \c FALSE if not.
		 *                Note that the value of \c _animationsEnabled is ignored
		 *                here.
		 */
		void setMapScalingFactor(const float factor, const bool animate = true);

		/**
		 * Determines if the cursor is on the left or right side of the target.
		 * @return \c TRUE if the UL of the cursor graphic is on the left half of
		 *         the target, \c FALSE if it is on the right half of the target,
		 *         or if \c _target is \c nullptr.
		 */
		bool isCursorOnLeftSide() const;

		/**
		 * Determines if the cursor is on the top or bottom side of the target.
		 * @return \c TRUE if the UL of the cursor graphic is on the top half of
		 *         the target, \c FALSE if it is on the bottom half of the target,
		 *         or if \c _target is \c nullptr.
		 */
		bool isCursorOnTopSide() const;

		/**
		 * Determines which quadrant of the render target the cursor is in.
		 * @return Where the UL of the cursor sprite is determined to be.
		 */
		awe::quadrant getCursorQuadrant() const;

		/**
		 * Determines which tile the selection tile graphic should be drawn from.
		 * @param tile The tile from which to draw the outline rectangle.
		 * @sa    \c setRectangleSelectionEnd().
		 */
		void setRectangleSelectionStart(const sf::Vector2u& tile);

		/**
		 * Determines which tile the selection tile graphic should be drawn to.
		 * @param tile The tile to which the outline rectangle should be drawn.
		 * @sa    \c setRectangleSelectionStart().
		 */
		void setRectangleSelectionEnd(const sf::Vector2u& tile);

		/**
		 * Clears the rectangle selection.
		 */
		void removeRectangleSelection();

		/**
		 * Retrieves the previously set start tile of the rectangle selection.
		 * @return The tile from which the rectangle selection is currently being
		 *         drawn, or <tt>(0, 0)</tt> if there isn't any being drawn.
		 */
		sf::Vector2u getRectangleSelectionStart() const;

		/**
		 * Retrieves the previously set end tile of the rectangle selection.
		 * @return The tile to which the rectangle selection is currently being
		 *         drawn, or <tt>(0, 0)</tt> if there isn't any being drawn.
		 */
		sf::Vector2u getRectangleSelectionEnd() const;

		/**
		 * Sets which sprite from the icon spritesheet to use as the cursor when
		 * the cursor is in the upper left quadrant of the screen.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the cursor will not show up if it is in
		 *          this quadrant!
		 * @param   sprite The animated sprite to use as the UL cursor.
		 */
		void setULCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the cursor when
		 * the cursor is in the upper right quadrant of the screen.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the cursor will not show up if it is in
		 *          this quadrant!
		 * @param   sprite The animated sprite to use as the UR cursor.
		 */
		void setURCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the cursor when
		 * the cursor is in the lower left quadrant of the screen.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the cursor will not show up if it is in
		 *          this quadrant!
		 * @param   sprite The animated sprite to use as the LL cursor.
		 */
		void setLLCursorSprite(const std::string& sprite);

		/**
		 * Sets which sprite from the icon spritesheet to use as the cursor when
		 * the cursor is in the lower right quadrant of the screen.
		 * If \c _sheet_icon isn't \c nullptr, and the given sprite doesn't exist
		 * in that sheet, then a warning will be logged, but the assignment will
		 * still go ahead.
		 * @warning Must be called, or else the cursor will not show up if it is in
		 *          this quadrant!
		 * @param   sprite The animated sprite to use as the LR cursor.
		 */
		void setLRCursorSprite(const std::string& sprite);

		/**
		 * Calculates where the cursor's bounding rectangle is relative to the GUI.
		 * @return The bounding rectangle of the cursor, in GUI coordinates.
		 */
		sf::IntRect getCursorBoundingBox() const;

		/**
		 * Calculates where the map's bounding rectangle is relative to the screen.
		 * If you are using this to check if the mouse position is within the map's
		 * graphic, you may have to be aware that \c INVALID_MOUSE stores
		 * <tt>(-1, -1)</tt>, so if the map is larger than the screen that may be
		 * considered within the map graphic! If you are pairing that check with a
		 * \c isMouseButtonTriggeringControl() check, though, you won't have to
		 * worry as all mouse buttons are ignored if the mouse position is
		 * \c INVALID_MOUSE.
		 * @warning This method assumes each tile is \c awe::tile::MIN_WIDTH in
		 *          width! A more complicated implementation would take variable
		 *          widths into consideration, but it's not something I'd ever use
		 *          so I'm not wasting time on it.
		 * @return  The bounding rectangle of the map, in pixel coordinates.
		 */
		sf::IntRect getMapBoundingBox() const;

		/**
		 * Trigger a map shake animation, if one is not already ongoing.
		 * @param duration The duration of the shake animation, in seconds.
		 */
		void shakeMap(const float duration = 1.5f);

		/**
		 * Sets this map's environment.
		 * @param environment Pointer to the environment to set.
		 */
		void setEnvironment(
			const std::shared_ptr<const awe::environment>& environment);

		/**
		 * Overload of \c setEnvironment() which accepts a script name.
		 * @param name Script name of the environment to set.
		 */
		void setEnvironment(const std::string& name);

		/**
		 * Retrieves the currently set environment.
		 * @return Pointer to the information on the current environment.
		 */
		std::shared_ptr<const awe::environment> getEnvironment() const;

		/**
		 * Retrieves the current environment, for use with the scripts.
		 * @return Information on the current environment.
		 * @throws \c std::out_of_range If there is currently no environment set.
		 */
		const awe::environment* getEnvironmentObject() const;

		/**
		 * Retrieves the spritesheet of the current environment, if there is one.
		 * @return The current environment's configured spritesheet, or an empty
		 *         string if there is no environment currently set.
		 */
		std::string getEnvironmentSpritesheet() const;

		/**
		 * Retrieves the tile picture spritesheet of the current environment, if
		 * there is one.
		 * @return The current environment's configured tile picture spritesheet,
		 *         or an empty string if there is no environment currently set.
		 */
		std::string getEnvironmentPictureSpritesheet() const;

		/**
		 * Retrieves the structure icon spritesheet of the current environment, if
		 * there is one.
		 * @return The current environment's configured structure icon spritesheet,
		 *         or an empty string if there is no environment currently set.
		 */
		std::string getEnvironmentStructureIconSpritesheet() const;

		//////////////////////////
		// ANIMATION OPERATIONS //
		//////////////////////////
		/**
		 * Is there an animation in progress?
		 * @return \c TRUE if there is an animation currently playing, \c FALSE if
		 *         there is no animation playing.
		 */
		inline bool animationInProgress() const {
			return _currentAnimation.operator bool();
		}

		/**
		 * Selects the current animation preset.
		 * @param preset The preset to select.
		 */
		void setSelectedAnimationPreset(const awe::animation_preset preset);

		/**
		 * Retrieves the currently selected animation preset.
		 * @return The selected animation preset.
		 */
		awe::animation_preset getSelectedAnimationPreset() const;

		/**
		 * Enables or disables animations.
		 * Disabling animations will not clear the animation queue.
		 * @param enabled \c TRUE to enable animations (default), \c FALSE to
		 *                disable animations.
		 */
		void enableAnimations(const bool enabled);

		/**
		 * Attempts to skip the currently playing animation, if there is one.
		 * @return \c TRUE if an animation was skipped via this method.
		 */
		bool skipAnimationIfPossible();

		/**
		 * Pushes code to the animation queue.
		 * @param func Pointer to the script function to execute.
		 * @param data Optional data to pass to the script function.
		 */
		void queueCode(asIScriptFunction* const func,
			CScriptAny* const data = nullptr);

		/**
		 * Queues an audio play operation.
		 * @param audio The name of the audio object containing the audio to play.
		 * @param name  The name of the audio to play.
		 * @param dur   If the play operation will cause any currently playing
		 *              music to stop, this will be the duration of the fade out,
		 *              in seconds.
		 */
		void queuePlay(const std::string& audio, const std::string& name,
			const float dur = 1.0f);

		/**
		 * Queues a sound stop operation.
		 * @param audio The name of the audio object containing the sound to stop.
		 * @param name  The name of the sound to stop.
		 */
		void queueStop(const std::string& audio, const std::string& name);

		/**
		 * Attempts to queue a "delay" animation.
		 * @param  duration  The duration of the delay, in seconds.
		 * @param  skippable \c TRUE if the user can skip the delay, \c FALSE if
		 *                   not.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool queueDelay(const float duration, const bool skippable = false);

		/**
		 * Attempts to queue a "day begin" animation.
		 * @param  armyID The ID of the army who's having their turn.
		 * @param  day    The day to display.
		 * @param  font   The font to apply to the day text.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateDayBegin(const awe::ArmyID armyID, const awe::Day day,
			const std::string& font);

		/**
		 * Attempts to queue a "tag CO" animation.
		 * @param  armyID The ID of the army who's having their COs tagged. The COs
		 *                must \b not have been tagged yet.
		 * @param  font   The font to apply to the tag CO text.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateTagCO(const awe::ArmyID armyID, const std::string& font);

		/**
		 * Attempts to queue a "tile particle" animation.
		 * Also automatically queues an \c awe::scroll animation before this one.\n
		 * If both \c audio and \c name are non-empty, a sound will be played
		 * between the scroll and the particle effect.
		 * @param  particles The particles to animate.
		 * @param  sheet     The name of the sheet to use for particle sprites.
		 * @param  audio     The name of the audio object containing the sound to
		 *                   queue.
		 * @param  name      The name of the audio to queue a play for.
		 * @return \c TRUE if both animations were queued, \c FALSE otherwise.
		 */
		bool animateParticles(const CScriptArray* const particles,
			const std::string& sheet, const std::string& audio = "",
			const std::string& name = "");

		/**
		 * Version of \c animateParticles() that's used by the C++ code to animate
		 * a single particle.
		 */
		bool animateParticle(const sf::Vector2u& tile, const std::string& sheet,
			const std::string& particle, const sf::Vector2f& origin,
			const std::string& audio = "", const std::string& name = "");

		/**
		 * Attempts to queue a "label unit" animation.
		 * Also automatically queues an \c awe::scroll animation before this one.
		 * @param  unitID          The ID of the unit that's being replenished.
		 * @param  leftLabel       ID of the label sprite to use when the label is
		 *                         on the left side of the unit.
		 * @param  rightLabel      ID of the label sprite to use when the label is
		 *                         on the right side of the unit.
		 * @param  loopSoundObject The name of the audio object containing the
		 *                         loop sound. Leave empty if you do not wish to
		 *                         play a loop sound.
		 * @param  loopSound       The name of the sound to play whilst showing
		 *                         the label unit animation. Leave empty if you do
		 *                         not wish to play a loop sound.
		 * @param  endSoundObject  The name of the audio object containing the
		 *                         sound to play once the animation has finished.
		 *                         Leave empty if you do not wish to play an end
		 *                         sound.
		 * @param  endSound        The name of the sound to play after finishing
		 *                         the label unit animation. Leave empty if you do
		 *                         not wish to play an end sound.
		 * @param  duration        The number of seconds the label should be
		 *                         visible for.
		 * @return \c TRUE if both animations were queued, \c FALSE otherwise.
		 */
		bool animateLabelUnit(const awe::UnitID unitID,
			const std::string& leftLabel, const std::string& rightLabel,
			const std::string& loopSoundObject = "",
			const std::string& loopSound = "",
			const std::string& endSoundObject = "",
			const std::string& endSound = "", const float duration = 0.7f);

		/**
		 * Attempts to queue a "property capture" animation.
		 * Empty strings can be given to any of the sound parameters to not play a
		 * particular sound, or not play any of them.
		 * @param  tile  The property tile being captured.
		 * @param  unit  The ID of the unit capturing the property.
		 * @param  oldHP The old HP of the tile.
		 * @param  newHP The new HP of the tile. If this is \c <= \c 0, the tile is
		 *               considered captured.
		 * @param  audioObject   The name of the audio object containing the sounds
		 *                       to play.
		 * @param  introSound    The name of the sound to play when the animation
		 *                       starts.
		 * @param  fallingSound  The name of the sound to play when the "falling"
		 *                       portion of the animation starts.
		 * @param  capturedSound The name of the sound to play when the "captured"
		 *                       portion of the animation starts.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateCapture(const sf::Vector2u& tile, const awe::UnitID unit,
			const awe::HP oldHP, const awe::HP newHP,
			const std::string& audioObject, const std::string& introSound,
			const std::string& fallingSound, const std::string& capturedSound);

		/**
		 * Attempts to queue a "unit move" animation.
		 * @param  unit       The ID of the unit to move.
		 * @param  closedList The closed list containing the path the unit is to
		 *                    move along. Must be at least two elements in size.
		 *                    The first element dictates where the unit starts
		 *                    from, and subsequent elements dictate the
		 *                    destinations of the unit, in order.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateMoveUnit(const awe::UnitID unit,
			const CScriptArray* const closedList);

		/**
		 * Attempts to queue a "scroll view" animation.
		 * @param  tile        The tile to bring into view.
		 * @param  speed       The speed of the scroll, in pixels per second.
		 * @param  drawCursors \c TRUE if the cursors should still be drawn whilst
		 *                     the view is scrolling, \c FALSE if not.
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateViewScroll(const sf::Vector2u& tile, const float speed,
			const bool drawCursors = false);

		/**
		 * Attempts to queue a "launch or strike" animation.
		 * Also automatically queues an \c awe::scroll animation before this one.
		 * @param  launch   \c TRUE if the sprite should launch from the given tile
		 *                  to over the top edge of the screen, \c FALSE if vice
		 *                  versa.
		 * @param  sprite   The name of the sprite to animate.
		 * @param  sheet    The name of the sheet where the sprite comes from.
		 * @param  tile     The tile to launch the sprite from, or strike with the
		 *                  sprite.
		 * @param  duration The number of seconds the sprite should remain in
		 *                  transit across the screen.
		 * @return \c TRUE if both animations were queued, \c FALSE otherwise.
		 */
		bool animateLaunchOrStrike(const bool launch, const std::string& sprite,
			const std::string& sheet, const sf::Vector2u& tile,
			const float duration = 1.0f);

		/**
		 * Attempts to queue a "next turn" animation.
		 * @param  prevArmy The ID of the army whose turn is ending. <b>Note that
		 *                  this will be assigned as the selected army override
		 *                  <u>immediately</u> if this method returns \c TRUE. Once
		 *                  the animation enters the \c TransitionOut state, the
		 *                  selected army override will be cleared.</b> This value
		 *                  is permitted to be \c NO_ARMY.
		 * @param  armyID   The ID of the army that is having their turn next.
		 * @param  controls A list of \c user_input control names that, if input,
		 *                  will close the next turn "screen."
		 * @return \c TRUE if the animation was queued, \c FALSE otherwise.
		 */
		bool animateNextTurn(const awe::ArmyID prevArmy, const awe::ArmyID armyID,
			const CScriptArray* const controls);

		//////////
		// MISC //
		//////////
		/**
		 * Sets the spritesheets used by this map.
		 * @param sheets Pointer to the animated spritesheets to use with this map.
		 */
		void setSpritesheets(
			const std::shared_ptr<sfx::animated_spritesheets>& sheets);

		/**
		 * Sets the fonts used with this map.
		 * If \c nullptr is given, an error will be logged.
		 * @param fonts Pointer to the fonts to use with this map.
		 */
		void setFonts(const std::shared_ptr<sfx::fonts>& fonts);

		/**
		 * Sets the audios used with this map.
		 * If \c nullptr is given, an error will be logged.
		 * @param audios Pointer to the audios to use with this map.
		 */
		void setAudios(const std::shared_ptr<sfx::audios>& audios);

		/**
		 * Sets the GUI engine to pull the GUI scaling factor from.
		 * @param gui Pointer to the GUI engine to use with this map.
		 */
		void setGUI(const std::shared_ptr<sfx::gui>& gui);

		/**
		 * Sets the language dictionary to use with this object.
		 * If \c nullptr is given, an error will be logged, and the internal
		 * pointer won't change.
		 * @param dict Pointer to the dictionary to use with this object.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& dict);

		/**
		 * Sets the strings to use with this map.
		 * If \c nullptr is given, an error will be logged, and the internal
		 * pointer won't change.
		 * @param strs Pointer to the strings to use with this map.
		 */
		void setMapStrings(const std::shared_ptr<awe::map_strings>& strs);

		/**
		 * Sets the user input object to use with this map.
		 * If \c nullptr is given, an error will be logged, and the internal
		 * pointer won't change.
		 * @param ui Pointer to the user input objectt to use with this map.
		 */
		void setUserInput(const std::shared_ptr<sfx::user_input>& ui);

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the map to.
		 * @return \c FALSE, for now.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Here is what will be rendered to the given target, in order:
		 * <ol><li>The map's tiles that are within the visible portion, starting
		 *         from the top row down.</li>
		 *     <li>All the units that are on the visible tiles, if they are to be
		 *         on the map according to \c isUnitOnMap(). This includes their
		 *         flashing icons.</li>
		 *     <li>The cursor, which will be drawn at the selected tile.</li>
		 *     <li>An information pane describing the selected army: their COs,
		 *         their funds, their country, and their power meters.</li>
		 *     <li>An information pane describing the selected tile: its type and
		 *         HP, and the unit's information, if there is a unit on the tile.
		 *     </li></ol>
		 * @param target The target to render the map to.
		 * @param states The render states to apply to the map. Applying transforms
		 *               is perfectly valid and will not alter the internal
		 *               workings of the drawable.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * Draws a unit if it doesn't have a \c NO_POSITION override.
		 * @param target The target to render the unit to.
		 * @param states The render states to apply to the unit.
		 * @param unitID The ID of the unit being drawn.
		 * @param sprite The unit sprite to draw.
		 */
		void drawUnit(sf::RenderTarget& target, const sf::RenderStates& states,
			const awe::UnitID unitID, sf::Drawable& sprite) const;

		/////////////
		// UTILITY //
		/////////////
		/**
		 * Checks if a given X and Y coordinate are out of bounds with the map's
		 * current size.
		 * @param  pos The position to test.
		 * @return \c TRUE if the position is out of bounds, \c FALSE if not.
		 */
		inline bool _isOutOfBounds(const sf::Vector2u& pos) const {
			return pos.x >= getMapSize().x || pos.y >= getMapSize().y;
		}

		/**
		 * Checks if a given army ID is present on the map.
		 * @param  id The ID of the army to check.
		 * @return \c TRUE if the army is on the map, \c FALSE if they are not.
		 */
		inline bool _isArmyPresent(const awe::ArmyID id) const {
			return _armies.find(id) != _armies.end();
		}

		/**
		 * Checks if a unit is present in the game.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if the unit is on the map, \c FALSE if they are not.
		 */
		inline bool _isUnitPresent(const awe::UnitID id) const {
			return _units.find(id) != _units.end();
		}

		/**
		 * Checks if a unit was put into a state where it can no longer capture.
		 * There are many circumstances where a capturing process needs to be
		 * stopped (the capturing unit = "a unit"):
		 * <ol><li>When a unit moves off a tile.</li>
		 *     <li>When a unit is loaded onto another unit.</li>
		 *     <li>When a unit is deleted.</li>
		 *     <li>When an army changes teams (all of its units must stop
		 *         capturing, and all of the units that were capturing its tiles
		 *         must also stop capturing).</li>
		 *     <li>When the tile the unit is capturing changes type <b>(regardless
		 *         of whether or not the type ends up being the same)</b>.</li>
		 *     <li>When the tile changes owner <b>(regardless of whether or not the
		 *         owner ends up being the same)</b>.</li></ol>
		 * This method will restore the tile and unit back to a pre-capturing
		 * state, if the given unit was capturing a tile.
		 * @param id The ID of the unit to check
		 */
		void _updateCapturingUnit(const awe::UnitID id);

		/**
		 * Determines the ID the next unit should have.
		 * A unit ID cannot be \c NO_UNIT. Automatically assigns the returned value
		 * to \c _lastUnitID.
		 * @throws std::bad_alloc in the [practically impossible] case that a new
		 *                        unique unit ID cannot be generated.
		 */
		awe::UnitID _findUnitID();

		/**
		 * Constructs a new binary input stream.
		 * @return A binary input stream constructed with logger data based on the
		 *         data given to the \c map object.
		 */
		inline engine::binary_istream _binaryIStreamFactory() const {
			return engine::binary_istream({ _logger.getData().sink,
				_logger.getData().name + "_binary_istream" });
		}

		/**
		 * Replaces the state of the map with data given in a binary input stream.
		 * @param  stream  The stream to read from.
		 * @param  version The 0-based number identifying the iteration of the
		 *                 format to use.
		 * @throws \c std::runtime_error if the \c LoadMap() script function cannot
		 *         be invoked.
		 * @safety If an \c std::runtime_error is thrown, the map's state will not
		 *         be cleared.
		 */
		void _loadMapFromInputStream(engine::binary_istream& stream,
			const unsigned char version);

		/**
		 * Writes the map's current state into a binary output stream.
		 * @param  version The 0-based number identifying the iteration of the
		 *                 format to use.
		 * @return Moves the binary output stream containing the map's data.
		 * @throws \c std::runtime_error if the \c SaveMap() script function cannot
		 *         be invoked.
		 */
		engine::binary_ostream _saveMapIntoOutputStream(
			const unsigned char version);

		/**
		 * Creates a memento and pushes it to the front of the undo deque.
		 * Also makes sure the undo deque doesn't surpass the limit, and that the
		 * redo deque is cleared.
		 * @param name The name to give to the new memento.
		 */
		void _createMemento(const std::string& name);

		/**
		 * Gets a map operation's name.
		 * @param  op The map operation to retrieve the name of.
		 * @return The translation key of the operation's name, or
		 *         \c awe::map_strings::ERROR_STRING if \c _mapStrings is
		 *         \c nullptr, or the translation key could not be found.
		 */
		inline std::string _getMementoName(awe::map_strings::operation op) {
			if (_mapStrings) return (*_mapStrings)[op];
			else return awe::map_strings::ERROR_STRING;
		}

		/**
		 * The state of the mementos has changes, so invoke callback if possible.
		 */
		inline void _mementosHaveChanged() {
			if (!_scripts) return;
			for (const auto& callback : _mementosChangedCallbacks)
				_scripts->callFunction(callback.operator->());
		}

		/**
		 * Can an animation be queued?
		 * @param  presets List of presets that are supported.
		 * @param  invert  If \c TRUE, \c presets will instead list presets that
		 *                 are \em not supported, and every other preset is assumed
		 *                 to be supported.
		 * @return \c TRUE if animations are enabled, and \c presets is empty or
		 *         the currently selected preset is within the given list (or not
		 *         in the list, if \c invert is \c TRUE). \c FALSE otherwise.
		 */
		bool _canAnimationBeQueued(
			const std::vector<awe::animation_preset>& presets = {},
			const bool invert = false) const;

		/**
		 * Updates every tile's spritesheet based on the current data stored in
		 * this map.
		 */
		void _regenerateTileSprites();

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Exception to throw when \c _scripts is \c nullptr when it shouldn't be.
		 */
		static const std::runtime_error NO_SCRIPTS;

		///////////////////////
		// FILE AND MEMENTOS //
		///////////////////////
		/**
		 * Represents a single memento.
		 */
		struct memento {
			/**
			 * The memento.
			 */
			std::shared_ptr<engine::binary_ostream> data;

			/**
			 * A description of the memento.
			 */
			std::string name;
		};

		/**
		 * File name of the binary file previously read from or written to.
		 */
		std::string _filename = "";

		/**
		 * Has a memento been added since the map was last successfully loaded or
		 * saved?
		 */
		bool _changed = false;

		/**
		 * Deque of mementos used to track states to undo to.
		 */
		std::deque<memento> _undoDeque;

		/**
		 * Deque of mementos used to track undone states that can be redone.
		 */
		std::deque<memento> _redoDeque;

		/**
		 * Mementos are only added via \c addMemento() if this field is \c 0.
		 */
		sf::Uint64 _mementoDisableCounter = 0;

		/**
		 * If \c TRUE, mementos are \b always disabled, regardless of how they are
		 * created.
		 * Used to prevent mementos from being created when loading and saving the
		 * map.
		 */
		bool _mementoHardDisable = false;

		/**
		 * Defines the number of mementos that can be stored in \c _undoDeque at
		 * one time.
		 */
		static constexpr std::size_t _MEMENTO_LIMIT = 30;

		/**
		 * The language dictionary to use with this object.
		 */
		std::shared_ptr<engine::language_dictionary> _dict;

		/**
		 * Stores the name of each map operation, to use with memento names.
		 */
		std::shared_ptr<awe::map_strings> _mapStrings;

		/**
		 * Pointers to script functions that are invoked when the state of the undo
		 * and/or redo deques have changed.
		 */
		std::list<engine::CScriptWrapper<asIScriptFunction>>
			_mementosChangedCallbacks;

		//////////
		// DATA //
		//////////
		/**
		 * Clears this map's state to its defaults.
		 */
		void _initState();

		/**
		 * Stores the map's name.
		 */
		std::string _mapName;

		/**
		 * Contains information pertaining to a single tile on this map.
		 */
		struct tile_data {
			/**
			 * Constructs a tile.
			 * @sa \c awe::tile::tile().
			 */
			tile_data(const engine::logger::data& logger_data,
				const std::function<void(const std::function<void(void)>&)>&
					spriteCallback,
				const std::shared_ptr<const awe::tile_type>& type = nullptr,
				const awe::ArmyID owner = awe::NO_ARMY,
				const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr)
				: sprite(std::make_shared<awe::animated_tile>(
					engine::logger::data{ logger_data.sink,
						logger_data.name + "_sprite" })),
				data(sprite, spriteCallback, type, owner, sheet) {}

			/**
			 * The tile's animated sprite.
			 */
			std::shared_ptr<awe::animated_tile> sprite;

			/**
			 * The tile's data.
			 */
			awe::tile data;
		};

		/**
		 * The tiles in this map.
		 * The first vector stores each column of tiles, from left to right. The
		 * second vector stores each tile of each column, from top to bottom. This
		 * means that the first index will be the X coordinate, moving from left to
		 * right, and the second index will be the Y coordinate, moving from top to
		 * bottom. Counting starts from the top left corner of the map.
		 */
		std::vector<std::vector<tile_data>> _tiles;

		/**
		 * Cache of the map's size as configured via \c setMapSize().
		 */
		sf::Vector2u _mapSizeCache;

		/**
		 * Contains information pertaining to a single unit on this map.
		 */
		struct unit_data {
			/**
			 * Constructs a unit.
			 * @sa \c awe::unit::unit().
			 */
			unit_data(const engine::logger::data& logger_data,
				const std::function<void(const std::function<void(void)>&)>&
					spriteCallback,
				const std::shared_ptr<const awe::unit_type>& type,
				const awe::ArmyID army = 0,
				const std::shared_ptr<sfx::animated_spritesheet>& sheet = nullptr,
				const std::shared_ptr<sfx::animated_spritesheet>& icons = nullptr)
				: sprite(std::make_shared<awe::animated_unit>(sheet,
					engine::logger::data{ logger_data.sink,
						logger_data.name + "_sprite" })),
				data(sprite, spriteCallback, type, army, icons) {}

			/**
			 * The unit's animated sprite.
			 */
			std::shared_ptr<awe::animated_unit> sprite;

			/**
			 * The unit's data.
			 */
			awe::unit data;
		};

		/**
		 * The units on this map.
		 */
		std::unordered_map<awe::UnitID, unit_data> _units;

		/**
		 * A list of units that have been deleted, but are still in the process of
		 * being destroyed.
		 */
		std::unordered_map<awe::UnitID, std::shared_ptr<awe::animated_unit>>
			_unitsBeingDestroyed;

		/**
		 * The armies on this map.
		 * To retain turn order, which is defined by countries, an ordered map was
		 * chosen.
		 * @warning Please ensure that an army with the ID \c awe::army::NO_ARMY
		 *          isn't created!
		 */
		std::map<awe::ArmyID, awe::army> _armies;

		/**
		 * The \c TeamID counter used to ensure each army is on their own team.
		 */
		awe::TeamID _teamIDCounter = std::numeric_limits<awe::TeamID>::min();

		/**
		 * The ID of the last unit created.
		 * Used to generate unit IDs once the initial unit has been created.
		 */
		awe::UnitID _lastUnitID = awe::ID_OF_FIRST_UNIT;

		/**
		 * Stores which day it currently is.
		 */
		awe::Day _day = 1;

		/**
		 * Is Fog of War enabled on this map?
		 */
		bool _fow = false;

		/**
		 * The additional data stored inside the map.
		 */
		std::string _additionalData;

		/////////////
		// SCRIPTS //
		/////////////
		/**
		 * Pointer to a \c scripts object.
		 */
		std::shared_ptr<engine::scripts> _scripts = nullptr;

		/**
		 * The scripts loaded with this map.
		 */
		engine::scripts::files _scriptFiles;

		/**
		 * The name of the module associated with this map.
		 */
		std::string _moduleName;

		/// If \c FALSE, \c periodic() will always return \c FALSE immediately.
		bool _periodicEnabled = true;

		/// The last known build result.
		std::string _lastKnownBuildResult;

		/// The script's map object that contains this \c awe::map object.
		CScriptHandle _mapObject;

		/// The name of the type of the script's map object.
		std::string _mapObjectTypeName;

		/////////////
		// DRAWING //
		/////////////
		/**
		 * Data \c map needs in order to render a selected unit.
		 */
		struct selected_unit_render_data {
			/**
			 * Default constructor.
			 * @param scripts The scripts engine to allocate the array with.
			 */
			selected_unit_render_data(const engine::scripts& scripts);

			/**
			 * Copy constructor.
			 * @param o The object to copy.
			 */
			selected_unit_render_data(
				const awe::map::selected_unit_render_data& o);

			/**
			 * Move constructor.
			 * @param o The object to move.
			 */
			selected_unit_render_data(
				awe::map::selected_unit_render_data&& o) noexcept;

			/**
			 * Releases the reference to the closed list \c CScriptArray, if it has
			 * been allocated.
			 */
			~selected_unit_render_data() noexcept;

			/**
			 * Copy assignment operator.
			 * @param o The object to copy.
			 */
			selected_unit_render_data& operator=(
				const awe::map::selected_unit_render_data& o);

			/**
			 * The ID of the unit that is selected.
			 * \c awe::NO_UNIT represents no selected unit and can be used to
			 * disable selected unit rendering.
			 */
			awe::UnitID selectedUnit = awe::NO_UNIT;

			/**
			 * The set of available tiles.
			 */
			std::unordered_set<sf::Vector2u> availableTiles;

			/**
			 * The shader to apply to all available tiles.
			 */
			awe::available_tile_shader availableTileShader =
				awe::available_tile_shader::None;

			/**
			 * The closed list, i.e. the currently selected path that a moving unit
			 * is considering to go along.
			 * The type stored in the array should be \c closed_list_node.
			 */
			CScriptArray* closedList = nullptr;

			/**
			 * Used to temporarily disable rendering effects without deselecting
			 * the unit.
			 */
			bool disableRenderingEffects = false;

			/**
			 * Used to disable greyed out shading on units if they are on an
			 * available tile.
			 */
			bool disableShaderForAvailableUnits = false;

			/**
			 * Clears the state of the object.
			 */
			void clearState();
		};

		/**
		 * The target set via \c setTarget().
		 */
		std::shared_ptr<sf::RenderTarget> _target;

		/**
		 * The view applied to the target whilst drawing.
		 */
		sf::View _view;

		/**
		 * The scaling factor to apply to the map when drawing.
		 */
		float _scaling = 1.0f;

		/**
		 * The amount to offset the view by in the X direction.
		 */
		std::optional<float> _viewOffsetX;

		/**
		 * The amount to offset the view by in the Y direction.
		 */
		std::optional<float> _viewOffsetY;
		
		/**
		 * Stores selected unit render data.
		 */
		std::stack<awe::map::selected_unit_render_data> _selectedUnitRenderData;

		/**
		 * Tells \c map to render given units at a different location than
		 * their real location.
		 */
		std::unordered_map<awe::UnitID, sf::Vector2u> _unitLocationOverrides;

		/**
		 * If a unit's location override has just been given, its ID will be stored
		 * in this set.
		 * Then, when \c animate() applies the override to the unit's position, it
		 * will be removed from this set.\n
		 * This set was added to handle cases where animation code assigns unit
		 * location overrides. Since animation code is run after the application of
		 * location overrides, you would see the unit's old location for a frame.
		 */
		std::unordered_set<awe::UnitID> _unitLocationOverrideHasNotYetBeenApplied;

		/**
		 * If set to \c TRUE, all units are drawn, regardless of whether they are
		 * hidden to the current army or not.
		 */
		bool _alwaysShowHiddenUnits = false;

		/**
		 * The currently selected tile.
		 */
		sf::Vector2u _sel;

		/**
		 * The additionally selected tile.
		 */
		std::optional<sf::Vector2u> _additionalSel;

		/**
		 * The start of the rectangle tile selection.
		 */
		std::optional<sf::Vector2u> _startOfRectSel;

		/**
		 * The end of the rectangle tile selection.
		 */
		std::optional<sf::Vector2u> _endOfRectSel;

		/**
		 * The rectangle selection graphic.
		 */
		sf::RectangleShape _rectangle;

		/**
		 * The army who is having their turn.
		 * This field is initialised to \c awe::army::NO_ARMY in \c load().
		 * @warning The initial state of \c awe::army::NO_ARMY cannot be set again
		 *          by the client. However, the drawing code must still check for
		 *          it and act accordingly!
		 */
		awe::ArmyID _currentArmy = awe::NO_ARMY;

		/**
		 * An override used to force the rendering code to display the map as if it
		 * were being viewed by the stored army.
		 */
		std::optional<awe::ArmyID> _currentArmyOverride;

		/**
		 * The animated sprite representing the cursor.
		 */
		sfx::animated_sprite _cursor;

		/**
		 * The ID of the sprite to use for the cursor when it is in the UL corner
		 * of the screen.
		 */
		std::string _ulCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the UR corner
		 * of the screen.
		 */
		std::string _urCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the LL corner
		 * of the screen.
		 */
		std::string _llCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the LR corner
		 * of the screen.
		 */
		std::string _lrCursorSprite;

		/**
		 * The UL corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorUL;

		/**
		 * The UR corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorUR;

		/**
		 * The LL corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorLL;

		/**
		 * The LR corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorLR;

		/**
		 * The currently selected environment.
		 */
		std::shared_ptr<const awe::environment> _environment;

		/**
		 * The currently selected weather.
		 */
		std::shared_ptr<const awe::weather> _weather;

		/**
		 * Weather particles.
		 */
		awe::random_particles _weatherParticles;

		/**
		 * Updates \c _weather, and resets the weather's particles.
		 * @param weather The new weather to set.
		 */
		void _setWeather(const std::shared_ptr<const awe::weather>& weather);

		// MOVE MODE DRAWING //

		/**
		 * Initialises the shaders used for selected unit rendering.
		 */
		void _initShaders();

		/**
		 * Yellow tiles shader.
		 */
		sf::Shader _availableTileShader;

		/**
		 * Red tiles shader.
		 */
		sf::Shader _attackableTileShader;

		/**
		 * Unavilable tiles shader.
		 */
		sf::Shader _unavailableTileShader;

		/**
		 * Normal tile shader for hidden tiles.
		 */
		sf::Shader _hiddenTileShaderFoW;

		/**
		 * Yellow tiles shader for hidden tiles.
		 */
		sf::Shader _availableTileShaderFoW;

		/**
		 * Red tiles shader for hidden tiles.
		 * Units should never cause this shader to be applied, but structure attack
		 * ranges can.
		 */
		sf::Shader _attackableTileShaderFoW;

		/**
		 * Unavilable tiles shader for hidden tiles.
		 */
		sf::Shader _unavailableTileShaderFoW;

		// ANIMATIONS //

		/**
		 * An animated drawable.
		 */
		typedef std::unique_ptr<awe::animation> animation;

		/**
		 * A piece of code to execute once it has reached the front of the
		 * animation queue.
		 */
		typedef std::function<void(void)> code;

		/**
		 * Allows the scripts to push code to the animation queue.
		 */
		struct script_code {
			/**
			 * Stores the script code and its optional data.
			 */
			script_code(asIScriptFunction* const c, CScriptAny* const d) :
				code(c), data(d) {}

			/**
			 * Points to the script code to execute.
			 */
			engine::CScriptWrapper<asIScriptFunction> code;

			/**
			 * Optional data to pass to the code when executed.
			 */
			engine::CScriptWrapper<CScriptAny> data;
		};

		/**
		 * The animation queue.
		 */
		std::queue<std::variant<animation, code, script_code>> _animationQueue;

		/**
		 * The currently playing animation.
		 */
		animation _currentAnimation;

		/**
		 * The currently selected animation preset.
		 */
		awe::animation_preset _selectedAnimationPreset =
			awe::animation_preset::Debug;

		/**
		 * Are animations enabled?
		 * @warning Please note that \c code must always be queued regardless of
		 *          the value of this field!
		 */
		bool _animationsEnabled = true;

		/**
		 * Flag allowing \c map to remember to destroy the first animation from the
		 * queue during the next iteration of the game loop.
		 */
		bool _destroyAnimation = false;

		/**
		 * Used to prevent drawing cursor graphics whilst an animation is on-going,
		 * and for an additional frame after the animation is destroyed.
		 */
		bool _drawCursors = true;

		// MISC DRAWING //

		/**
		 * A new offset during a map shake is only generated once this time has
		 * passed.
		 */
		static const sf::Time WAIT_DURATION_FOR_NEW_SHAKE;

		/**
		 * The pseudo-random number sequence generator.
		 */
		const std::unique_ptr<std::mt19937> _prng = engine::RNGFactory();

		/**
		 * The uniform distribution to use when generating random numbers for map
		 * shaking along the X axis.
		 */
		std::uniform_int_distribution<int> _mapShakeXDistribution =
			std::uniform_int_distribution<int>(-14, 14);

		/**
		 * The uniform distribution to use when generating random numbers for map
		 * shaking along the Y axis.
		 */
		std::uniform_int_distribution<int> _mapShakeYDistribution =
			std::uniform_int_distribution<int>(-6, 6);

		/**
		 * Shake the map graphic for this length of time.
		 */
		sf::Time _mapShakeTimeLeft;

		/**
		 * Wait for this duration of time before generating a new map shake offset.
		 */
		sf::Time _waitBeforeNextShake;

		/**
		 * The last randomly-generated offset.
		 */
		sf::Vector2f _mapShakeOffset;

		///////////////
		// RESOURCES //
		///////////////
		/**
		 * Spritesheets.
		 */
		std::shared_ptr<sfx::animated_spritesheets> _sheets;
		
		/**
		 * GUI engine.
		 */
		std::shared_ptr<sfx::gui> _gui;

		/**
		 * The fonts available to the map.
		 */
		std::shared_ptr<sfx::fonts> _fonts;

		/**
		 * The audios available to the map.
		 */
		std::shared_ptr<sfx::audios> _audios;

		/**
		 * The user input object.
		 */
		std::shared_ptr<sfx::user_input> _ui;

		///////////
		// BANKS //
		///////////
		/**
		 * Data pertaining to countries.
		 */
		std::shared_ptr<awe::bank<awe::country>> _countries = nullptr;

		/**
		 * Data pertaining to environments.
		 */
		std::shared_ptr<awe::bank<awe::environment>> _environments = nullptr;

		/**
		 * Data pertaining to weathers.
		 */
		std::shared_ptr<awe::bank<awe::weather>> _weathers = nullptr;

		/**
		 * Data pertaining to tile types.
		 */
		std::shared_ptr<awe::bank<awe::tile_type>> _tileTypes = nullptr;

		/**
		 * Data pertaining to terrains.
		 */
		std::shared_ptr<awe::bank<awe::terrain>> _terrains = nullptr;

		/**
		 * Data pertaining to unit types.
		 */
		std::shared_ptr<awe::bank<awe::unit_type>> _unitTypes = nullptr;

		/**
		 * Data pertaining to COs.
		 */
		std::shared_ptr<awe::bank<awe::commander>> _commanders = nullptr;

		/**
		 * Data pertaining to structures.
		 */
		std::shared_ptr<awe::bank<awe::structure>> _structures = nullptr;
	};
}
