/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

#include "tile.hpp"
#include "unit.hpp"
#include "army.hpp"
#include "script.hpp"
#include "file.hpp"
#include "language.hpp"
#include <cmath>
#include <stack>
#include <optional>
#include "damage_sprite.hpp"

#pragma once

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
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		map(const engine::logger::data& data);

		/**
		 * Initialises this object with \c bank pointers.
		 * Also initialises the internal logger object.\n
		 * @param countries  Information on the countries to search through when
		 *                   reading country IDs from map files.
		 * @param tiles      Information on the tile types to search through when
		 *                   reading tile type IDs from map files.
		 * @param units      Information on the unit types to search through when
		 *                   reading unit type IDs from map files.
		 * @param commanders Information on the commanders to search through when
		 *                   reading CO IDs from map files.
		 * @param data       The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		map(const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders,
			const engine::logger::data& data);

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
		bool save(std::string file, const unsigned char version = 0);

		/**
		 * Gives access to the scripts to save the map.
		 * @return \c TRUE if the save was successful, \c FALSE if the file
		 *         couldn't be saved (reason will be logged).
		 * @safety No guarantee.
		 */
		bool save();

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
		 * Carry out periodic tasks, such as checking for win conditions.
		 * @return If \c TRUE is returned, it signifies that the map's win
		 *         condition has been met, and the map object should be deleted.
		 */
		bool periodic();

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		/**
		 * Sets the map's name.
		 * By default, a map's name is a blank string.
		 * @param  name The name to give to the map to replace the old name.
		 * @safety Strong guarantee.
		 */
		void setMapName(const std::string& name);

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
		 * @param   dim  The width (x) and height (y) to make the map.
		 * @param   tile The type of tile to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u& dim,
			const std::shared_ptr<const awe::tile_type>& tile = nullptr);

		/**
		 * Overload of \c setMapSize() which accepts a tile type script name.
		 * @param dim  The new size of the map.
		 * @param tile The script name of the tile type to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u& dim, const std::string& tile);

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The map's size. X = width, Y = height.
		 */
		inline sf::Vector2u getMapSize() const {
			return _mapSizeCache;
		}

		/**
		 * Sets the current day.
		 * @param day The new day.
		 */
		void setDay(const awe::Day day) noexcept;

		/**
		 * Gets the current day.
		 * @return The current day.
		 */
		awe::Day getDay() const noexcept;

		/**
		 * The default win condition of a map.
		 * If there is only one team remaining on the map, then the win condition
		 * has been met. If there is no team remaining, then \c TRUE will be
		 * returned.
		 * @return \c TRUE if the win condition has been met, \c FALSE otherwise.
		 */
		bool defaultWinCondition() const;

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
		 * parameter).
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
		 * @param  army The ID of the army to retrieve the owned tiles of.
		 * @return A list of locations of tiles that belong to the given army.
		 */
		std::unordered_set<sf::Vector2u> getTilesOfArmy(
			const awe::ArmyID army) const;

		/**
		 * Converts the result of a \c getTilesOfArmy() call into a
		 * \c CScriptArray.
		 * @throws std::runtime_error if \c _scripts was \c nullptr.
		 * @sa     @c getTilesOfArmy().
		 */
		CScriptArray* getTilesOfArmyAsArray(const awe::ArmyID army) const;

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
		 * @return The 1-based ID of the unit created. Will be \c 0 if the unit
		 *         couldn't be created.
		 */
		awe::UnitID createUnit(const std::shared_ptr<const awe::unit_type>& type,
			const awe::ArmyID army);

		/**
		 * Overload of \c createUnit() which accepts a unit type script name.
		 * @param  type The script name of the type of unit to create.
		 * @param  army The ID of the army who will own this unit.
		 * @return The 1-based ID of the unit created. Will be \c 0 if the unit
		 *         couldn't be created.
		 */
		awe::UnitID createUnit(const std::string& type, const awe::ArmyID army);

		/**
		 * Deletes a unit.
		 * A deleted unit will be removed from the map's and owning army's list, as
		 * well as the tile it was on. Any location override associated with the
		 * unit will also be deleted.
		 * @warning Any loaded units will \b also be deleted. Additionally, if the
		 *          deleted unit was selected, then \c setSelectedUnit(0) will be
		 *          called!
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
		 * @param  army The ID of the army who is trying to see the unit.
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
		 * If the given unit doesn't exist, then an error will be logged and \c 0
		 * will be returned.
		 * @param  unit The ID of the unit which should be loaded onto another.
		 * @return If \c unit is loaded onto another unit, then the latter's ID
		 *         will be returned. Otherwise, \c 0 will be returned.
		 */
		awe::UnitID getUnitWhichContainsUnit(const awe::UnitID unit) const;

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
		 * @sa @c getLoadedUnits().
		 */
		CScriptArray* getLoadedUnitsAsArray(const awe::UnitID id) const;

		/**
		 * Finds out the number of defence stars this unit has based on its type
		 * and current position.
		 * Loaded units will always give a defence rating of \c 0, as they are not
		 * internally positioned on any tile.
		 * @param  id The ID of the unit to getthe defence of.
		 * @return The defence rating that this unit currently obtains.
		 */
		unsigned int getUnitDefence(const awe::UnitID id) const;

		/////////////////////
		// TILE OPERATIONS //
		/////////////////////
		/**
		 * Sets a specified tile's type.
		 * By default, a tile does not have a type, unless it was given in the call
		 * to \c setMapSize().\n
		 * Changing a tile's type will automatically remove any ownership of the
		 * tile, but it will not reset the tile's HP.
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
		 * Retrieves the specified tile's type.
		 * If an out of bounds coordinate is given, \c nullptr is returned.
		 * @param  pos The X and Y coordinate of the tile to change.
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
		 * This method will return 0 if there is a unit on this tile, but it is not
		 * physically on the map, such as if the unit is loaded onto another unit.
		 * @param  pos The X and Y coordinate of the tile to inspect.
		 * @return The ID of the unit occupying this tile. \c 0 if the tile is
		 *         vacant or out of bounds.
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u& pos) const;

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
		 * @sa @c getAvailableTiles().
		 */
		CScriptArray* getAvailableTilesAsArray(
			const sf::Vector2u& tile, unsigned int startFrom,
			const unsigned int endAt) const;

		/**
		 * Finds the shortest path from the origin to the destination.
		 * If \c nullptr is given for a pointer parameter, their respective check
		 * will be ignored. For example, if \c fuel is \c nullptr, it can be
		 * assumed that there are an infinite number of units of fuel available. Or
		 * if \c team is \c nullptr, then any occupied tile will be considered
		 * impassable, even if the tile is occupied by a unit on the same team.
		 * @param  origin       The origin tile.
		 * @param  dest         The intended destination.
		 * @param  moveType     The movement type used for traversal calculations.
		 * @param  movePoints   Pointer to the movement points available.
		 * @param  fuel         Pointer to the units of fuel that we have to work
		 *                      with.
		 * @param  team         Pointer to the team the moving unit is on.
		 * @param  army         Pointer to the army the moving unit is on.
		 * @param  ignoredUnits A set of units that can be moved through,
		 *                      regardless of team.
		 * @return The shortest path, if a path could be found. An empty vector if
		 *         no path could be found.
		 */
		std::vector<awe::closed_list_node> findPath(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const unsigned int* const movePoints, const awe::Fuel* const fuel,
			const awe::TeamID* const team, const awe::ArmyID* const army,
			const std::unordered_set<awe::UnitID>& ignoredUnits) const;

		/**
		 * Version of \c findPath() which converts the result into a
		 * \c CScriptArray.
		 * @sa @c findPath().
		 */
		CScriptArray* findPathAsArray(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const unsigned int movePoints, const awe::Fuel fuel,
			const awe::TeamID team, const awe::ArmyID army,
			CScriptArray* ignoredUnits) const;

		/**
		 * Version of \c findPath() which passes in \c nullptr where possible, and
		 * converts the result into a \c CScriptArray.
		 * @sa @c findPath().
		 */
		CScriptArray* findPathAsArrayUnloadUnit(const sf::Vector2u& origin,
			const sf::Vector2u& dest, const awe::movement_type& moveType,
			const awe::ArmyID army, CScriptArray* ignoredUnits) const;

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

		//////////////////////////////////////
		// SELECTED UNIT DRAWING OPERATIONS //
		//////////////////////////////////////
		/**
		 * Selects a unit on the map.
		 * If an invalid unit ID is given, the call won't have any effect, and it
		 * will be logged.\n
		 * Note that no rendering effects for available tiles or a selected unit,
		 * etc., will be employed if the currently selected unit is \c 0.
		 * @param  unit The ID of the unit to select. If \c 0 is given, all of the
		 *              selected unit rendering data will be cleared.
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
		 *          call <tt>setSelectedUnit(0)</tt>.
		 * @warning If the newly selected unit is no longer present after the pop,
		 *          then \c setSelectedUnit(0) will be called and a warning will be
		 *          logged.
		 */
		void popSelectedUnit();

		/**
		 * Gets the currently selected unit.
		 * @return The ID of the currently selected unit. \c 0 if no unit is
		 *         selected.
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
		// DRAWING OPERATIONS //
		////////////////////////
		/**
		 * Sets the target used with this map.
		 * @param target Pointer to the target this map will be drawn on.
		 */
		void setTarget(const std::shared_ptr<sf::RenderTarget>& target) noexcept;

		/**
		 * Selects a tile on the map.
		 * Used only to let \c map know what tile to draw information on, as well
		 * as where to draw the cursor. If the given location is out of bounds, the
		 * call will be ignored.
		 * @param pos The X and Y location of the tile which is selected.
		 */
		void setSelectedTile(const sf::Vector2u& pos);

		/**
		 * Moves the cursor up one tile, if possible.
		 * If not possible, the call is ignored.
		 */
		void moveSelectedTileUp();

		/**
		 * Moves the cursor down one tile, if possible.
		 * If not possible, the call is ignored.
		 */
		void moveSelectedTileDown();

		/**
		 * Moves the cursor left one tile, if possible.
		 * If not possible, the call is ignored.
		 */
		void moveSelectedTileLeft();

		/**
		 * Moves the cursor right one tile, if possible.
		 * If not possible, the call is ignored.
		 */
		void moveSelectedTileRight();

		/**
		 * Selects a tile on the map based on a pixel.
		 * The tile that is drawn underneath the pixel will be selected, if there
		 * is one.\n
		 * Note that if \c _target is \c nullptr this method will have no effect.
		 * @param pixel The pixel relative to the render target to use to identify
		 *              a tile to select.
		 */
		void setSelectedTileByPixel(const sf::Vector2i& pixel);

		/**
		 * Gets the position of the currently selected tile.
		 * @return The X and Y location of the selected tile.
		 */
		inline sf::Vector2u getSelectedTile() const {
			return _sel;
		}

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
		 * Sets the amount by which the map is scaled.
		 * If it is detected that a value at or below \c 0.0f is given, an error
		 * will be logged and \c _scaling will not be changed.
		 * @param factor The factor by which to scale the map.
		 */
		void setMapScalingFactor(const float factor);

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
		 * Sets the spritesheet used for drawing tiles.
		 * @param sheet Pointer to the animated spritesheet to use for tiles.
		 */
		void setTileSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing units.
		 * @param sheet Pointer to the animated spritesheet to use for units.
		 */
		void setUnitSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing map icons.
		 * @param sheet Pointer to the animated spritesheet to use for icons.
		 */
		void setIconSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing COs.
		 * @param sheet Pointer to the animated spritesheet to use for COs.
		 */
		void setCOSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the font used with this map.
		 * If \c nullptr is given, an error will be logged.
		 * @param font Pointer to the font to use with this map.
		 */
		void setFont(const std::shared_ptr<sf::Font>& font);

		/**
		 * Sets the language dictionary to use with this map.
		 * If \c nullptr is given, an error will be logged.
		 * @param dict Pointer to the dictionary to use with this map.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& dict);

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
		 * A unit ID cannot be \c 0. Automatically assigns the returned value to
		 * \c _lastUnitID.
		 * @throws std::bad_alloc in the [practically impossible] case that a new
		 *                        unique unit ID cannot be generated.
		 */
		awe::UnitID _findUnitID();

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		//////////
		// FILE //
		//////////
		/**
		 * File name of the binary file previously read from or written to.
		 */
		std::string _filename = "";

		/**
		 * Binary file object used to read files.
		 */
		engine::binary_file _file;

		/**
		 * Pointer to a \c scripts object.
		 */
		std::shared_ptr<engine::scripts> _scripts = nullptr;

		//////////
		// DATA //
		//////////
		/**
		 * Stores the map's name.
		 */
		std::string _mapName;

		/**
		 * The tiles in this map.
		 * The first vector stores each column of tiles, from left to right. The
		 * second vector stores each tile of each column, from top to bottom. This
		 * means that the first index will be the X coordinate, moving from left to
		 * right, and the second index will be the Y coordinate, moving from top to
		 * bottom. Counting starts from the top left corner of the map.
		 */
		std::vector<std::vector<awe::tile>> _tiles;

		/**
		 * Cache of the map's size as configured via \c setMapSize().
		 */
		sf::Vector2u _mapSizeCache;

		/**
		 * The units on this map.
		 */
		std::unordered_map<awe::UnitID, awe::unit> _units;

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
		awe::TeamID _teamIDCounter = 0;

		/**
		 * The ID of the last unit created.
		 * Used to generate unit IDs once the initial unit has been created.
		 */
		awe::UnitID _lastUnitID = 1;

		/**
		 * Stores which day it currently is.
		 */
		awe::Day _day = 0;

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
			 * \c 0 represents no selected unit and can be used to disable selected
			 * unit rendering.
			 */
			awe::UnitID selectedUnit = 0;

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
		 * The currently selected tile.
		 */
		sf::Vector2u _sel;

		/**
		 * The army who is having their turn.
		 * This field is initialised to \c awe::army::NO_ARMY in \c load().
		 * @warning The initial state of \c awe::army::NO_ARMY cannot be set again
		 *          by the client. However, the drawing code must still check for
		 *          it and act accordingly!
		 */
		awe::ArmyID _currentArmy = awe::NO_ARMY;

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

		// BASE DAMAGE SPRITE //

		/**
		 * The [temporary] damage tooltip.
		 */
		awe::damage_tooltip _damageTooltip;

		//////////////////
		// SPRITESHEETS //
		//////////////////
		/**
		 * Spritesheet used with all tiles.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_tile = nullptr;

		/**
		 * Spritesheet used with all units.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_unit = nullptr;

		/**
		 * Spritesheet used with all map icons.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_icon = nullptr;

		/**
		 * Spritesheet used with all armies.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_co = nullptr;

		///////////
		// BANKS //
		///////////
		/**
		 * The country data to read from when reading map files.
		 */
		std::shared_ptr<awe::bank<awe::country>> _countries = nullptr;

		/**
		 * The tile type data to read from when reading map files.
		 */
		std::shared_ptr<awe::bank<awe::tile_type>> _tileTypes = nullptr;

		/**
		 * The unit type data to read from when reading map files.
		 */
		std::shared_ptr<awe::bank<awe::unit_type>> _unitTypes = nullptr;

		/**
		 * The CO data to read from when reading map files.
		 */
		std::shared_ptr<awe::bank<awe::commander>> _commanders = nullptr;
	};
}
