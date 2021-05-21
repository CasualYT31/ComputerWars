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
 * @file game.h
 * Declares the class used to bring together the \c awe::map and \c awe::army classes.
 */

#pragma once

#include "file.h"
#include "map.h"
#include "unit.fwd.h"
#include "unit.h"
#include "army.fwd.h"
#include "army.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class that represents a single game of Advance Wars, with a map and its armies.
	 * This class is only responsible for the storage and direct manipulation of the game,
	 * which means game rules are defined in another class.
	 */
	class game : sf::NonCopyable {
	public:
		/**
		 * Current version number representing the file format in use.
		 * The standard implementation uses the four bytes "#CWM",
		 * where # is a 0-255 value representing the version of the standard map file format in use.
		 * \c 1297564416 is the 32-bit integer value representing "[NUL]", "C", "W", and "M". Adding
		 * one to this value will increase the first byte (little endian is used).\n
		 * It is a \b custom format that is in no way based on the map formats of the original game.
		 */
		static const sf::Uint32 VERSION_NUMBER = 1297564416;

		/**
		 * Value representing no commander in-file.
		 */
		static const awe::bank<const awe::commander>::index NO_CO = UINT_MAX;

		/**
		 * Value representing no army in-file.
		 */
		static const unsigned char NO_ARMY = 0xFF;

		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "game."
		 * @sa    \c global::logger
		 */
		game(const std::string& name = "game") noexcept;

		/**
		 * Opens a binary file and reads the contents to overwrite what's currently stored in this object.
		 * This method has strong exception safety.\n
		 * \c FALSE is returned if any of the banks or spritesheets haven't been assigned yet (see, for example, \c setCountries()).
		 * @param   filename The name of the file to open. If a blank string, \c _filename will be substitued.
		 * @return  \c TRUE if reading was successful, \c FALSE otherwise.
		 */
		bool read(std::string filename = "") noexcept;

		/**
		 * Opens a binary file and replaces its contents with what's stored in this object.
		 * @param  filename The name of the file to open. If a blank string, \c _filename will be substitued.
		 * @return \c TRUE if writing was successful, \c FALSE otherwise.
		 */
		bool write(std::string filename = "") noexcept;

		/**
		 * Updates the map's size.
		 * @param newSize The new size of the map in tiles.
		 * @sa    \c awe::map::setSize()
		 */
		void setMapSize(const sf::Vector2u& newSize) noexcept;

		/**
		 * Retrieves the map's size.
		 * @return The size of the map in tiles.
		 * @sa     \c awe::map::getSize()
		 */
		sf::Vector2u getMapSize() const noexcept;

		/**
		 * Updates the map's name.
		 * @param newName The new name of the map.
		 * @sa    \c awe::map::setName()
		 */
		void setMapName(const std::string& newName) noexcept;

		/**
		 * Retrieves the map's name.
		 * @return The name of the map.
		 * @sa     \c awe::map::getName()
		 */
		std::string getMapName() const noexcept;

		/**
		 * Creates a new army and adds it to the internal list.
		 * The new army is given \c 0 funds, and it has no commanders.
		 * Its team ID will be the same as the country ID given.
		 * @param id The ID of the country of the army, which also defines the turn order of the army.
		 */
		void createArmy(awe::bank<awe::country>::index id) noexcept;

		/**
		 * Deletes an existing army from the internal list.
		 * This will delete all of the army's units and will indirectly turn all owned tiles to neutral.
		 * Note that if any strong references to the army or the unit remain,
		 * it will not be deleted until those are dropped, too.
		 * @param ptr Pointer to the country of the army to remove.
		 */
		void deleteArmy(const std::shared_ptr<const awe::country>& ptr) noexcept;

		/**
		 * Assigns the given army a new team.
		 * If the given army doesn't exist, or if \c ptr is \c NULL, the call will be ignored.
		 * @param ptr     Pointer to the country of the army to amend.
		 * @param newTeam The ID of the team to assign this army to.
		 */
		void setArmysTeam(const std::shared_ptr<const awe::country>& ptr, awe::TeamID newTeam) noexcept;

		/**
		 * Retrieves the team of a given army.
		 * @param ptr Pointer to the country of the army to retrieve the team of.
		 * @return The team ID of the specified army, if that army exists.
		 *         If the army doesn't exist or \c ptr is \c NULL, \c NO_ARMY will be returned.
		 */
		awe::TeamID getArmysTeam(const std::shared_ptr<const awe::country>& ptr) const noexcept;

		/**
		 * Sets the number of funds a given army possesses.
		 * If the given army doesn't exist, or if \c ptr is \c NULL, the call will be ignored.
		 * @param ptr   Pointer to the country of the army to amend.
		 * @param funds The new fund amount.
		 */
		void setArmysFunds(const std::shared_ptr<const awe::country>& ptr, awe::Funds funds) noexcept;

		/**
		 * Retrieves the funds a given army possesses.
		 * @param ptr Pointer to the country of the army to retrieve the funds of.
		 * @return The funds the specified army owns, if that army exists.
		 *         If the army doesn't exist or \c ptr is \c NULL, \c -1 will be returned.
		 */
		awe::Funds getArmysFunds(const std::shared_ptr<const awe::country>& ptr) const noexcept;

		/**
		 * Sets an army's commanders.
		 * \c nullptr signifies that no commander should be present.
		 * In the event that \c firstCO is \c nullptr and \c secondCO is not, these two values will be swapped.
		 * @param ptr      Pointer to the country of the army to amend.
		 * @param firstCO  Pointer to the first CO's static information.
		 * @param secondCO Pointer to the second CO's static information.
		 */
		void setArmysCommanders(const std::shared_ptr<const awe::country>& ptr,
								std::shared_ptr<const awe::commander> firstCO,
								std::shared_ptr<const awe::commander> secondCO = nullptr) noexcept;

		/**
		 * Retrieves the static information of the first commander.
		 * By default, no CO is set to an army, so care should be taken when accessing the returned pointer.
		 * @param ptr Pointer to the country of the army to retrieve the first CO of.
		 * @return Pointer to the first CO's information, or \c nullptr if the specified army doesn't exist.
		 */
		std::shared_ptr<const awe::commander> getFirstCommander(const std::shared_ptr<const awe::country>& ptr) const noexcept;

		/**
		 * Retrieves the static information of the second commander.
		 * Special care should be taken when accessing the returned pointer, as an army may well have no secondary commander.
		 * @param ptr Pointer to the country of the army to retrieve the second CO of.
		 * @return Pointer to the second CO's information, or \c nullptr if the specified army doesn't exist.
		 */
		std::shared_ptr<const awe::commander> getSecondCommander(const std::shared_ptr<const awe::country>& ptr) const noexcept;

		/**
		 * Finds out if an army has two COs.
		 * @param ptr Pointer to the country of the army to test.
		 * @return \c TRUE if an army has been assigned two COs, \c FALSE otherwise.
		 */
		bool isTagTeam(const std::shared_ptr<const awe::country>& ptr) const noexcept;

		/**
		 * Allows access to the map object.
		 * @return Pointer to the map object, or an empty pointer if a map hasn't been allocated yet.
		 */
		std::shared_ptr<awe::map> getMap() const noexcept;

		/**
		 * Allows access to a given army object.
		 * @param  i The 0-based index of the army to retrieve.
		 * @return Pointer to the army object, or an empty pointer if \c i is out of range.
		 */
		std::shared_ptr<awe::army> getArmy(std::size_t i) const noexcept;

		/**
		 * Retrieves the number of armies allocated.
		 * @return The number of armies allocated.
		 */
		std::size_t getNumberOfArmies() const noexcept;

		// game methods

		/**
		 * Creates a new unit.
		 * This method will fail under the following circumstances:
		 * <ol><li>\c owningArmy is an empty pointer.</li>
		 * <li>\c type is an empty pointer.</li>
		 * <li>\c _map is an empty pointer.</li>
		 * <li>\c _map->getTile(location) is an empty pointer or throws.</li>
		 * <li>The tile at \c location is occupied.</li></ol>
		 * @param  owningArmy The owner of the unit.
		 * @param  type       The type of unit to create.
		 * @param  location   The starting location of the new unit.
		 * @return Pointer to the created unit, \c nullptr if it could not be created (errors will be logged).
		 */
		std::shared_ptr<awe::unit> createUnit(const std::shared_ptr<awe::army>& owningArmy, const std::shared_ptr<const awe::unit_type>& type, sf::Vector2u location) noexcept;

		/**
		 * Deletes an existing unit.
		 * @remark Remember that a \c shared_ptr is provided: if the parameter is still active once the call finishes,
		 *         the unit will still remain active. It will only be removed from the game if all shared references
		 *         to the unit have been dropped.
		 * @param  ref Strong reference to the unit to delete.
		 * @return \c TRUE if the unit was successfully deleted, \c FALSE if \c ref was an empty pointer (an error will be logged).
		 */
		bool deleteUnit(const std::shared_ptr<awe::unit>& ref) noexcept;

		/**
		 * Changes the owner of a given tile.
		 * @param  ref           Strong reference to the tile in question.
		 * @param  newOwningArmy The new owner of the tile, \c nullptr for no owner.
		 * @return \c TRUE if the owner was changed successfully, \c FALSE if \c ref was empty (errors will be logged).
		 */
		bool changeTileOwner(const std::shared_ptr<awe::tile>& ref, const std::shared_ptr<awe::army>& newOwningArmy) noexcept;

		/**
		 * Moves a given unit to a given location on the map.
		 * @param  ref         Strong reference to the unit to move.
		 * @param  newLocation New location to set the unit to.
		 * @return \c TRUE if the unit was moved successfully, \c FALSE if not (errors will be logged).
		 */
		bool moveUnit(const std::shared_ptr<awe::unit>& ref, sf::Vector2u newLocation) noexcept;

		/**
		 * Loads a unit onto this one.
		 * @warning Note that this method removes the tile reference from the loaded unit,
		 *          as well as the loaded unit reference from this tile.
		 * @param   dest The unit which will hold \c src.
		 * @param   src  The unit to load onto \c dest.
		 * @return  \c TRUE if the operation was completed successfully, \c FALSE if either reference was empty, or if the load operation failed.
		 */
		bool loadUnit(const std::shared_ptr<awe::unit>& dest, const std::shared_ptr<awe::unit>& src) noexcept;

		/**
		 * Unloads a unit from another.
		 * @param  dest        The unit which holds \c src.
		 * @param  src         The unit to unload from \c dest.
		 * @param  newLocation The tile on which \c src should be unloaded to.
		 * @return \c TRUE if the operation was completed successfully, \c FALSE if either reference was empty, or if the unload operation failed,
		 *         or if the move operation failed (\c newLocation was not valid).
		 */
		bool unloadUnit(const std::shared_ptr<awe::unit>& dest, const std::shared_ptr<awe::unit>& src, sf::Vector2u newLocation) noexcept;

		// setup methods

		/**
		 * Sets the country bank of this game.
		 * @param ptr Pointer to the country information to pull from.
		 */
		void setCountries(const std::shared_ptr<awe::bank<const awe::country>>& ptr) noexcept;

		/**
		 * Sets the commander bank of this game.
		 * @param ptr Pointer to the commander information to pull from.
		 */
		void setCommanders(const std::shared_ptr<awe::bank<const awe::commander>>& ptr) noexcept;

		/**
		 * Sets the tile bank of this game.
		 * @param ptr Pointer to the tile information to pull from.
		 */
		void setTiles(const std::shared_ptr<awe::bank<const awe::tile_type>>& ptr) noexcept;

		/**
		 * Sets the unit bank of this game.
		 * @param ptr Pointer to the unit information to pull from.
		 */
		void setUnits(const std::shared_ptr<awe::bank<const awe::unit_type>>& ptr) noexcept;

		/**
		 * Sets the spritesheets of this game.
		 * @param ptr Pointer to the spritesheet information to pull from.
		 */
		void setSpritesheets(const std::shared_ptr<awe::spritesheets>& ptr) noexcept;
	private:
		/**
		 * Retrieves a vector iterator for a given army.
		 * @param  index The 0-based index of the army to retrieve the iterator of.
		 * @return The iterator pointing to the army referenced by \c index. \c end() if \c index is out of range.
		 * @throws \c std::out_of_range if \c _armies was \c nullptr at the time of calling.
		 */
		std::vector<std::shared_ptr<awe::army>>::iterator _getArmyIterator(unsigned int index) const;

		/**
		 * Initialises a file stream for either input or output.
		 * @param  filename The path of the file to open.
		 * @param  forInput \c TRUE if opening the file for input, \c FALSE for output.
		 * @return Pointer to the opened file.
		 */
		std::shared_ptr<engine::binary_file> _openFile(std::string& filename, bool forInput);

		/**
		 * Reads a map file in the first version of the CWM format.
		 * @param file   The binary file to read from.
		 * @param map    Pointer to the map object to update.
		 * @param armies Pointer to the armies vector to update.
		 */
		void _read_CWM_1(const std::shared_ptr<engine::binary_file>& file,
			std::shared_ptr<awe::map> map,
			std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies);

		/**
		 * Reads unit information from a map file in the first version of the CWM format.
		 * @param  file      The binary file to read from.
		 * @param  armyCount The number of armies that should be in play.
		 * @param  armies    Pointer to the armies vector to update.
		 * @return Pointer to the created unit, or \c nullptr if no unit was found.
		 */
		std::shared_ptr<awe::unit> _read_unit_CWM_1(const std::shared_ptr<engine::binary_file>& file,
			sf::Uint8 armyCount, sf::Uint32 x, sf::Uint32 y,
			std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies);

		/**
		 * Internal logger object.
		 */
		mutable global::logger _logger;

		/**
		 * The name of the map file last opened \em successfully using \c read() or \c write().
		 */
		std::string _filename = "";

		/**
		 * The map.
		 */
		std::shared_ptr<awe::map> _map;

		/**
		 * The armies involved in the game.
		 */
		std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> _armies;

		/**
		 * The countries involved in the game.
		 */
		std::shared_ptr<awe::bank<const awe::country>> _countries;

		/**
		 * The commanders involved in the game.
		 */
		std::shared_ptr<awe::bank<const awe::commander>> _commanders;

		/**
		 * The tiles involved in the game.
		 */
		std::shared_ptr<awe::bank<const awe::tile_type>> _tiles;

		/**
		 * The units involved in the game.
		 */
		std::shared_ptr<awe::bank<const awe::unit_type>> _units;

		/**
		 * The spritesheets involved in the game.
		 */
		std::shared_ptr<awe::spritesheets> _sprites;
	};
}