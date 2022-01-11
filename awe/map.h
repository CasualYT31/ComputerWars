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

/**@file map.h
 * Declares the class which is used to manage a map.
 * Unit, armies, and tiles are all stored and managed here.
 */

#include "tile_pane.h"
#include "army_pane.h"
#include "script.h"
#include "file.h"

#pragma once

namespace awe {
	/**
	 * Class which represents a map, and the armies and units that play on it.
	 * Only basic checks are carried out in this class; all game logic is separate.
	 * If any of these basic checks fail, they will be logged.
	 */
	class map : sf::NonCopyable, public sfx::animated_drawable {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "map."
		 * @sa    \c engine::logger
		 */
		map(const std::string& name = "map") noexcept;

		/////////////////////
		// FILE OPERATIONS //
		/////////////////////
		/**
		 * Version number of the CWM format representing the very first version.
		 * \c 1297564416 is the 32-bit integer value representing "[NUL]", "C",
		 * "W", and "M". Adding a number of up to 255 to this value will increase
		 * the first byte (little endian is used), so that different versions of
		 * the CWM format can be checked for easily.
		 */
		static const sf::Uint32 FIRST_FILE_VERSION = 1297564416;

		/**
		 * The latest version of the CWM format.
		 * Can be used with calls to the constructor and the \c save() method.
		 */
		static const unsigned char LATEST_VERSION = 1;

		/**
		 * Version number of the CWM format representing the latest version.
		 * @sa \c awe::map::FIRST_FILE_VERSION
		 * @sa \c awe::map::LATEST_VERSION
		 */
		static const sf::Uint32 LATEST_FILE_VERSION =
			FIRST_FILE_VERSION + LATEST_VERSION;

		/**
		 * Loads a given binary file containing map data and initialises this map
		 * based off this data.
		 * Also initialises the internal logger object.\n
		 * For full documentation on Computer Wars' map file format, please see the
		 * CWM Format Specification.md file in the root folder of the repository.
		 * @param file       Path to the binary file to load.
		 * @param countries  Information on the countries to search through when
		 *                   reading country IDs.
		 * @param tiles      Information on the tile types to search through when
		 *                   reading tile type IDs.
		 * @param units      Information on the unit types to search through when
		 *                   reading unit type IDs.
		 * @param commanders Information on the commanders to search through when
		 *                   reading CO IDs.
		 * @param version    The 0-based number identifying the iteration of the
		 *                   format to use.
		 * @param name       The name to give this particular instantiation within
		 *                   the log file. Defaults to "map."
		 * @sa    \c engine::logger
		 */
		map(const std::string& file,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders,
			const unsigned char version = LATEST_VERSION,
			const std::string& name = "map")
			noexcept;

		/**
		 * Saves this \c map object's state to a given binary file.
		 * @param  file    Path to the binary file to save to. If a blank string,
		 *                 the file given previously, in either the constructor or
		 *                 a call to \c save(), will be used.
		 * @param  version The 0-based number identifying the iteration of the
		 *                 format to use.
		 * @return \c TRUE if the save was successful, \c FALSE if the file
		 *         couldn't be saved (reason will be logged).
		 */
		bool save(std::string file,
			const unsigned char version = LATEST_VERSION) noexcept;

		////////////////////
		// MAP OPERATIONS //
		////////////////////
		/**
		 * Sets the map's name.
		 * By default, a map's name is a blank string.
		 * @param name The name to give to the map to replace the old name.
		 */
		void setMapName(const std::string& name) noexcept;

		/**
		 * Retrieves the map's name.
		 * @return The current name of the map.
		 */
		std::string getMapName() const noexcept;

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
		 * @remark Since this operation shouldn't be a bottleneck, I've not
		 *         concerned myself much with efficiency in its implementation. The
		 *         main way to improve efficiency would be to loop through each
		 *         relevant tile and remove their unit before the resize, rather
		 *         than looping through \em all units and removing out of bounds
		 *         ones, after the resize.
		 * @param  dim  The width (x) and height (y) to make the map.
		 * @param  tile The type of tile to assign to new tiles.
		 */
		void setMapSize(const sf::Vector2u dim,
			const std::shared_ptr<const awe::tile_type>& tile = nullptr) noexcept;

		/**
		 * Retrieves the size of the map, in tiles.
		 * @return The map's size. X = width, Y = height.
		 */
		sf::Vector2u getMapSize() const noexcept;

		/////////////////////
		// ARMY OPERATIONS //
		/////////////////////
		/**
		 * Allocates a new army.
		 * If the army with the given country already exists, or \c nullptr is
		 * given, the call will be logged.
		 * @param  country The country of the army.
		 * @return \c TRUE if the army was created, \c FALSE otherwise.
		 */
		bool createArmy(const std::shared_ptr<const awe::country>& country)
			noexcept;

		/**
		 * Deletes an army entirely from the map.
		 * Deleting an army removes the army from the army list, deletes all the
		 * units belonging to the army, and disowns all owned tiles.
		 * @param army The ID of the army to delete.
		 */
		void deleteArmy(const awe::ArmyID army) noexcept;

		/**
		 * Sets the amount of funds a specified army obtains.
		 * If \c 0 or less is given, \c 0 will be stored.
		 * @param army  The ID of the army to amend.
		 * @param funds The new fund amount to assign to the army.
		 */
		void setArmyFunds(const awe::ArmyID army, const awe::Funds funds) noexcept;

		/**
		 * Retrieves an army's fund count.
		 * @param  army The ID of the army to inspect.
		 * @return The amount of funds this army possesses, or \c 0 if the given
		 *         army doesn't exist.
		 */
		awe::Funds getArmyFunds(const awe::ArmyID army) const noexcept;

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
			const std::shared_ptr<const awe::commander>& tag = nullptr) noexcept;

		/**
		 * Performs a tag on a given army.
		 * If the given army ID did not identify an army, or if there were not two
		 * COs to perform the tag with, then an error will be logged.
		 * @param army The ID of the army to perform the tag on.
		 */
		void tagArmyCOs(const awe::ArmyID army) noexcept;

		/**
		 * Retrieves an army's primary/current CO.
		 * @param  army The ID of the army to retrieve the current CO of.
		 * @return Pointer to the information on the given army's current CO.
		 *         \c nullptr is returned if the given army did not exist at the
		 *         time of calling, or if there was no current CO.
		 */
		std::shared_ptr<const awe::commander> getArmyCurrentCO(
			const awe::ArmyID army) const noexcept;

		/**
		 * Retrieves an army's secondary/tag CO.
		 * @param  army The ID of the army to retrieve the tag CO of.
		 * @return Pointer to the information on the given army's tag CO.
		 *         \c nullptr is returned if the given army did not exist at the
		 *         time of calling, or if there was no tag CO.
		 */
		std::shared_ptr<const awe::commander> getArmyTagCO(const awe::ArmyID army)
			const noexcept;

		/**
		 * Retrieves a list of tiles that belong to a specified army.
		 * If the specified army doesn't exist, an empty set will be returned.
		 * @param  army The ID of the army to retrieve the owned tiles of.
		 * @return A list of locations of tiles that belong to the given army.
		 */
		std::unordered_set<sf::Vector2u> getTilesOfArmy(const awe::ArmyID army)
			const noexcept;

		/**
		 * Retrieves a list of units that belong to a specified army.
		 * If the specified army doesn't exist, an empty set will be returned.
		 * @param  army The ID of the army to retrieve the units of.
		 * @return A list of IDs identifying the units that belong to this army.
		 */
		std::unordered_set<awe::UnitID> getUnitsOfArmy(const awe::ArmyID army)
			const noexcept;

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
			const awe::ArmyID army) noexcept;

		/**
		 * Deletes a unit.
		 * A deleted unit will be removed from the map's and owning army's list, as
		 * well as the tile it was on.
		 * @warning Any loaded units will \b also be deleted.
		 * @param   id The ID of the unit to delete.
		 */
		void deleteUnit(const awe::UnitID id) noexcept;

		/**
		 * Sets a unit's position on the map.
		 * The operation will be cancelled if the specified tile is already
		 * occupied.
		 * @param id  The ID of the unit to move.
		 * @param pos The X and Y coordinate of the tile to move the unit to.
		 */
		void setUnitPosition(const awe::UnitID id, const sf::Vector2u pos)
			noexcept;

		/**
		 * Retrieves a unit's position, indicating the tile it is occupying.
		 * This method does not take into account if the unit is \em actually on a
		 * tile: please use this method in conjunction with \c isUnitOnMap().\n
		 * If the unit doesn't exist, <tt>(0, 0)</tt> will be returned.
		 * @param  id The ID of the unit.
		 * @return The X and Y location of the unit on the map.
		 */
		sf::Vector2u getUnitPosition(const awe::UnitID id) const noexcept;

		/**
		 * Finds out if a unit occupies a tile or not.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if this unit occupies a tile, \c FALSE if not.
		 */
		bool isUnitOnMap(const awe::UnitID id) const noexcept;

		/**
		 * Sets a unit's HP.
		 * If < 0 is given, 0 will be stored.
		 * @param id The ID of the unit to amend.
		 * @param hp The new HP of the unit.
		 */
		void setUnitHP(const awe::UnitID id, const awe::HP hp) noexcept;

		/**
		 * Gets a unit's HP.
		 * @param  id The ID of the unit to inspect.
		 * @return The HP of the unit. \c 0 if unit doesn't exist.
		 */
		awe::HP getUnitHP(const awe::UnitID id) const noexcept;

		/**
		 * Sets a unit's fuel.
		 * If < 0 is given, 0 will be stored.
		 * @param id   The ID of the unit to amend.
		 * @param fuel The new fuel of the unit.
		 */
		void setUnitFuel(const awe::UnitID id, const awe::Fuel fuel) noexcept;

		/**
		 * Gets a unit's fuel.
		 * @param  id The ID of the unit to inspect.
		 * @return The fuel of the unit. \c 0 if unit doesn't exist.
		 */
		awe::Fuel getUnitFuel(const awe::UnitID id) const noexcept;

		/**
		 * Sets a unit's ammo.
		 * If < 0 is given, 0 will be stored.
		 * @param id   The ID of the unit to amend.
		 * @param ammo The new ammo of the unit.
		 */
		void setUnitAmmo(const awe::UnitID id, const awe::Ammo ammo) noexcept;

		/**
		 * Gets a unit's ammo.
		 * @param  id The ID of the unit to inspect.
		 * @return The ammo of the unit. \c 0 if unit doesn't exist.
		 */
		awe::Ammo getUnitAmmo(const awe::UnitID id) const noexcept;

		/**
		 * Loads one unit onto another.
		 * If the unit to load is already loaded onto another unit, this call will
		 * be ignored and a warning will be logged.
		 * @warning This method does not consider any load limits imposed by any
		 *          unit type.
		 * @param   load The ID of the unit to load onto another.
		 * @param   onto The ID of the unit which will load the unit.
		 */
		void loadUnit(const awe::UnitID load, const awe::UnitID onto) noexcept;

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
			const sf::Vector2u onto) noexcept;

		/**
		 * Retrieves the ID of the army a specified unit belongs to.
		 * <tt>awe::army::NO_ARMY</tt> will be returned if the unit doesn't exist.
		 * @param  id The ID of the unit to inspect.
		 * @return The ID of the army that owns this unit.
		 */
		awe::ArmyID getArmyOfUnit(const awe::UnitID id) const noexcept;

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
		bool setTileType(const sf::Vector2u pos,
			const std::shared_ptr<const awe::tile_type>& type) noexcept;

		/**
		 * Retrieves the specified tile's type.
		 * If an out of bounds coordinate is given, \c nullptr is returned.
		 * @param  pos The X and Y coordinate of the tile to change.
		 * @return The type of the tile and its information.
		 */
		std::shared_ptr<const awe::tile_type> getTileType(const sf::Vector2u pos)
			const noexcept;

		/**
		 * Sets a tile's HP.
		 * If a negative value is given, it will be adjusted to \c 0.
		 * @param pos The X and Y coordinate of the tile to change.
		 * @param hp  The HP to assign to the tile.
		 */
		void setTileHP(const sf::Vector2u pos, const awe::HP hp) noexcept;

		/**
		 * Retrieves a tile's HP.
		 * @param  pos The X and Y coordinate of the tile to retrieve from.
		 * @return The HP of the tile, or \c 0 if the given coordinate was out of
		 *         bounds.
		 */
		awe::HP getTileHP(const sf::Vector2u pos) const noexcept;

		/**
		 * Sets a tile's owner.
		 * @param pos  The X and Y coordinate of the tile to change.
		 * @param army The ID of the army who now owns this tile.
		 *             \c awe::army::NO_ARMY can be given to signal that the tile
		 *             should not have an owner.
		 */
		void setTileOwner(const sf::Vector2u pos, awe::ArmyID army) noexcept;

		/**
		 * Gets a tile's owner.
		 * If the coorindate is out of bounds, \c awe::army::NO_ARMY is returned.
		 * @param  pos The X and Y coordinate of the tile to change.
		 * @return The ID of the army who owns this tile, or \c awe::army::NO_ARMY
		 *         if no army owns it.
		 */
		awe::ArmyID getTileOwner(const sf::Vector2u pos) const noexcept;

		/**
		 * Retrieves the unit currently occupying a specified tile.
		 * This method will return 0 if there is a unit on this tile, but it is not
		 * physically on the map, such as if the unit is loaded onto another unit.
		 * @param  pos The X and Y coordinate of the tile to inspect.
		 * @return The ID of the unit occupying this tile. \c 0 if the tile is
		 *         vacant or out of bounds.
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u pos) const noexcept;

		////////////////////////
		// DRAWING OPERATIONS //
		////////////////////////
		/**
		 * Selects a tile on the map.
		 * Used only to let \c map know what tile to draw information on, as well
		 * as where to draw the cursor. If the given location is out of bounds, the
		 * call will be ignored.
		 * @param pos The X and Y location of the tile which is selected.
		 */
		void setSelectedTile(const sf::Vector2u pos) noexcept;

		/**
		 * Gets the position of the currently selected tile.
		 * @return The X and Y location of the selected tile.
		 */
		sf::Vector2u getSelectedTile() const noexcept;

		/**
		 * Selects an army from the map.
		 * Used only to let \c map know what army to draw information on. If the
		 * given army didn't exist, the call will be cancelled and logged.
		 * @param army The ID of the army which should be having their turn at the
		 *             time of the call.
		 */
		void selectArmy(const awe::ArmyID army) noexcept;

		/**
		 * Sets the drawn portion of the map.
		 * To save resources, not all of a map should be drawn. Instead, only the
		 * portion visible to the user via the render window should be drawn.
		 * Everything will always be animated, though animation calculation should
		 * not take up nearly as many resources.\n
		 * If the given portion is at least partly out of bounds, an error
		 * will be logged and the visible portion will not be changed. A \c width
		 * and/or \c height of \c 0 will be rejected and logged.
		 * @warning By default, none of the map is selected to be rendered! Make
		 *          sure to call this method before you draw the map, and after
		 *          you've set the size of the map!
		 * @param   rect The portion of the map to render, in tiles.
		 */
		void setVisiblePortionOfMap(const sf::Rect<sf::Uint32>& rect) noexcept;

		/**
		 * Sets the spritesheet used for drawing tiles.
		 * @param sheet Pointer to the animated spritesheet to use for tiles.
		 */
		void setTileSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>&
			sheet) noexcept;

		/**
		 * Sets the spritesheet used for drawing units.
		 * @param sheet Pointer to the animated spritesheet to use for units.
		 */
		void setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>&
			sheet) noexcept;

		/**
		 * Sets the spritesheet used for drawing map icons.
		 * @param sheet Pointer to the animated spritesheet to use for icons.
		 */
		void setIconSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>&
			sheet) noexcept;

		/**
		 * Sets the spritesheet used for drawing COs.
		 * @param sheet Pointer to the animated spritesheet to use for COs.
		 */
		void setCOSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>&
			sheet) noexcept;

		/**
		 * Sets the font used with this map.
		 * If \c nullptr is given, an error will be logged.
		 * @param font Pointer to the font to use with this map.
		 */
		void setFont(const std::shared_ptr<sf::Font>& font) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the map to.
		 * @return \c FALSE, for now.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
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
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/////////////
		// UTILITY //
		/////////////
		/**
		 * Checks if a given X and Y coordinate are out of bounds with the map's
		 * current size.
		 * @param  pos The position to test.
		 * @return \c TRUE if the position is out of bounds, \c FALSE if not.
		 */
		bool _isOutOfBounds(const sf::Vector2u pos) const noexcept;

		/**
		 * Checks if a given X and Y coordinate are within the visible portion of
		 * the map as defined by \c _visiblePortion.
		 * @param  pos The position to test.
		 * @return \c TRUE if the position is within the visible portion, \c FALSE
		 *         if not.
		 */
		bool _tileIsVisible(const sf::Vector2u pos) const noexcept;

		/**
		 * Checks if a given army ID is present on the map.
		 * @param  id The ID of the army to check.
		 * @return \c TRUE if the army is on the map, \c FALSE if they are not.
		 */
		bool _isArmyPresent(const awe::ArmyID id) const noexcept;

		/**
		 * Checks if a unit is present in the game.
		 * @param  id The ID of the unit to check.
		 * @return \c TRUE if the unit is on the map, \c FALSE if they are not.
		 */
		bool _isUnitPresent(const awe::UnitID id) const noexcept;

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
		 * Either reads from or writes to a binary file.
		 * Also deduces the format which this file is to have or has.
		 * @param  isSave     \c TRUE if the file is to be written to, \c FALSE if
		 *                    the file is to be read from.
		 * @param  countries  Information on the countries to index when reading
		 *                    country IDs.
		 * @param  tiles      Information on the tile types to index when reading
		 *                    tile type IDs.
		 * @param  units      Information on the unit types to index when reading
		 *                    unit type IDs.
		 * @param  commanders Information on the CO types to index when reading CO
		 *                    type IDs.
		 * @param  version    If writing, the version should be given here.
		 * @throws std::exception if the header couldn't be read or written.
		 */
		void _CWM_Header(const bool isSave,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders,
			unsigned char version);

		/**
		 * First version of the CWM format.
		 * @param  isSave    \c TRUE if the file is to be written to, \c FALSE if
		 *                   the file is to be read from.
		 * @param  countries Information on the countries to index when reading
		 *                   country IDs.
		 * @param  tiles     Information on the tile types to index when reading
		 *                   tile type IDs.
		 * @param  units     Information on the unit types to index when reading
		 *                   unit type IDs.
		 * @throws std::exception if the file couldn't be read or written.
		 */
		void _CWM_0(const bool isSave,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units);

		/**
		 * Reads or writes information pertaining to a unit in 0CWM format.
		 * @param  isSave    \c TRUE if the file is to be written to, \c FALSE if
		 *                   the file is to be read from.
		 * @param  countries Information on the countries to index when reading
		 *                   country IDs.
		 * @param  tiles     Information on the tile types to index when reading
		 *                   tile type IDs.
		 * @param  units     Information on the unit types to index when reading
		 *                   unit type IDs.
		 * @param  id        The ID of the unit to write info on, if writing.
		 * @param  curtile   The location of the tile to create the unit on, if
		 *                   reading.
		 * @param  loadOnto  The ID of the unit to load the new unit onto. \c 0 if
		 *                   the unit should not be loaded onto another one.
		 * @throws std::exception if the unit info couldn't be read or written.
		 * @return When loading, \c TRUE if there was a unit created, \c FALSE if
		 *         no unit was created. When saving, always \c TRUE.
		 */
		bool _CWM_0_Unit(const bool isSave,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			awe::UnitID id, const sf::Vector2u& curtile,
			const awe::UnitID loadOnto = 0);

		/**
		 * Second version of the CWM format.
		 * @param  isSave     \c TRUE if the file is to be written to, \c FALSE if
		 *                    the file is to be read from.
		 * @param  countries  Information on the countries to index when reading
		 *                    country IDs.
		 * @param  tiles      Information on the tile types to index when reading
		 *                    tile type IDs.
		 * @param  units      Information on the unit types to index when reading
		 *                    unit type IDs.
		 * @param  commanders Information on the CO types to index when reading CO
		 *                    type IDs.
		 * @throws std::exception if the file couldn't be read or written.
		 */
		void _CWM_1(const bool isSave,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders);

		/**
		 * File name of the binary file previously read from or written to.
		 */
		std::string _filename = "";

		/**
		 * Binary file object used to read files.
		 */
		engine::binary_file _file;

		//////////
		// DATA //
		//////////
		/**
		 * Stores the map's name.
		 */
		std::string _mapName = "";

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
		 * The units on this map.
		 */
		std::unordered_map<awe::UnitID, awe::unit> _units;

		/**
		 * The armys on this map.
		 * To retain turn order, which is defined by countries, an ordered map was
		 * chosen.
		 * @warning Please ensure that an army with the ID \c awe::army::NO_ARMY
		 *          isn't created!
		 */
		std::map<awe::ArmyID, awe::army> _armys;

		/**
		 * The ID of the last unit created.
		 * Used to generate unit IDs once the initial unit has been created.
		 */
		awe::UnitID _lastUnitID = 1;

		/////////////
		// DRAWING //
		/////////////
		/**
		 * The currently selected tile.
		 */
		sf::Vector2u _sel = sf::Vector2u(0, 0);

		/**
		 * The army who is having their turn.
		 * @warning The initial state of \c awe::army::NO_ARMY cannot be set again
		 *          by the client. However, the drawing code must still check for
		 *          it and act accordingly!
		 */
		awe::ArmyID _currentArmy = awe::army::NO_ARMY;

		/**
		 * The visible portion of the map.
		 */
		sf::Rect<sf::Uint32> _visiblePortion = sf::Rect<sf::Uint32>(0, 0, 0, 0);

		/**
		 * The animated sprite representing the cursor.
		 * The cursor sprite is currently hard-coded to always be the first sprite
		 * of the icon spritesheet provided later.
		 */
		sfx::animated_sprite _cursor;

		/**
		 * The pane representing the current army.
		 */
		awe::army_pane _armyPane;

		/**
		 * The pane displaying information on the selected tile.
		 */
		awe::tile_pane _tilePane;

		/**
		 * Flag used to update \c _tilePane.
		 * When the selected tile is changed, this will be set to \c TRUE.
		 */
		bool _updateTilePane = false;

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
	};
}