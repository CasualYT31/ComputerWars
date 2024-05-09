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

/**@file army.hpp
 * Declares the class which represents an in-game army.
 */

#pragma once

#include "bank-v2.hpp"
#include "maths.hpp"
#include "SFML/System/Vector2.hpp"
#include <unordered_set>

namespace awe {
	/**
	 * Class which represents a single army on a map.
	 * @warning Note that, since this class does not have a default constructor, if
	 *          it is used with a \c map container, the \c operator[]() method
	 *          cannot be used with that map. Use \c at() instead.
	 */
	class army {
	public:
		/**
		 * Constructs a new army.
		 * @param banks   Banks pointer.
		 * @param country The country the army belongs to, which can't be changed.
		 * @param data    Logger data to pass onto \c _view objects.
		 */
		army(const std::shared_ptr<const awe::banks>& banks, const std::string& country,
			const engine::logger::data& data);

		/**
		 * Sets the team that this army belongs to.
		 * @param teamID The new team to assign to this army.
		 */
		inline void setTeam(const awe::TeamID teamID) noexcept {
			_team = teamID;
		}

		/**
		 * Gets the team that this army belongs to.
		 * @return The ID of the team this army belongs to.
		 */
		inline awe::TeamID getTeam() const noexcept {
			return _team;
		}

		/**
		 * Gets the army's country information.
		 * @return The army's country information.
		 */
		inline engine::CScriptWrapper<awe::country_view> getCountry() const {
			const auto view = awe::country_view::Create(_loggerData, _banks, _country);
			view->overrides.commander(_co_1);
			return view;
		}

		inline std::string getCountryScriptName() const {
			return _country;
		}

		/**
		 * Updates the army's fund count.
		 * If a value below \c 0 is given, \c 0 will be stored.
		 * @param funds The fund count to replace this army's old fund count with.
		 */
		inline void setFunds(const awe::Funds funds) noexcept {
			_funds = ((funds <= 0) ? (0) : (funds));
		}

		/**
		 * Retrieves the army's fund count.
		 * By default, it is \c 0.
		 * @return The funds this army obtains.
		 */
		inline awe::Funds getFunds() const noexcept {
			return _funds;
		}

		/**
		 * Sets the COs that are in charge of this army.
		 * If \c current is empty, but \c tag is not, then \c tag will be assigned
		 * as the primary CO and there will be no secondary CO.
		 * @param current The primary CO in charge of this army.
		 * @param tag     The secondary CO who is currently not the one in charge,
		 *                yet could be after a call to \c tagCOs(). If there will
		 *                be no secondary CO, an empty string should be passed.
		 */
		void setCOs(const std::string& current,
			const std::string& tag = "") noexcept;

		/**
		 * Swaps the two COs of this army.
		 * This call will be ignored if there is no secondary CO to tag with.
		 */
		inline void tagCOs() noexcept {
			if (hasTagCO()) std::swap(_co_1, _co_2);
		}

		/**
		 * Retrieves a pointer to the information on the CO currently in charge of
		 * this army.
		 * @return Information on the current CO.
		 */
		inline engine::CScriptWrapper<awe::commander_view> getCurrentCO() const {
			return awe::commander_view::Create(_loggerData, _banks, _co_1);
		}

		/**
		 * If you just need to know who the current CO is, without its information,
		 * then use this method.
		 * @return The script name of the current CO.
		 */
		inline std::string getCurrentCOScriptName() const {
			return _co_1;
		}

		/**
		 * Retrieves a pointer to the information on the tag CO of this army.
		 * @return Information on the tag CO.
		 */
		inline engine::CScriptWrapper<awe::commander_view> getTagCO() const {
			return awe::commander_view::Create(_loggerData, _banks, _co_2);
		}

		/**
		 * If you just need to know who the tag CO is, without its information,
		 * then use this method.
		 * @return The script name of the tag CO.
		 */
		inline std::string getTagCOScriptName() const {
			return _co_2;
		}

		/**
		 * Does this army have a tag CO?
		 * @return \c TRUE if this army has a tag CO, \c FALSE if not.
		 */
		inline bool hasTagCO() const noexcept {
			return !_co_2.empty();
		}

		/**
		 * Adds a unit to this army's unit list.
		 * @param  unit The ID of the unit to add.
		 * @safety Strong guarantee.
		 */
		inline void addUnit(const awe::UnitID unit) {
			_units.insert(unit);
		}

		/**
		 * Removes a unit from this army's unit list.
		 * @param  unit The ID of the unit to remove.
		 * @safety Strong guarantee.
		 */
		inline void removeUnit(const awe::UnitID unit) {
			_units.erase(unit);
		}

		/**
		 * Copies the internal list of all the units that belong to this army.
		 * @return The IDs of all the units that belong to this army.
		 */
		inline std::unordered_set<awe::UnitID> getUnits() const {
			return _units;
		}

		/**
		 * Adds a tile to this army's owned tiles list.
		 * @param  tile The X and Y location of the tile.
		 * @safety Strong guarantee.
		 */
		inline void addTile(const sf::Vector2u tile) {
			_tiles.insert(tile);
		}

		/**
		 * Removes a tile from this army's owned tiles list.
		 * @param  tile The X and Y location of the tile.
		 * @safety Strong guarantee.
		 */
		inline void removeTile(const sf::Vector2u tile) {
			_tiles.erase(tile);
		}

		/**
		 * Retrieves a list of all the tiles this army owns.
		 * @return The internal list of tile locations, copied.
		 */
		inline std::unordered_set<sf::Vector2u> getTiles() const {
			return _tiles;
		}

		/**
		 * Adds tiles to this army's visible tile cache.
		 * @param tiles The tiles that are now visible to one of this army's units.
		 */
		inline void addVisibleTiles(
			const std::unordered_set<sf::Vector2u>& tiles) {
			_visibleTiles.insert(tiles.begin(), tiles.end());
		}

		/**
		 * Removes tiles from this army's visible tile cache.
		 * @param tiles The tiles that are now no longer visible to one of this
		 *              army's units.
		 */
		void removeVisibleTiles(const std::unordered_set<sf::Vector2u>& tiles);

		/**
		 * Is the given tile visible to at least one of this army's units?
		 * @param  tile The tile to check.
		 * @return \c TRUE if this tile is visible to this army, considering only
		 *         each unit's vision range. \c FALSE if it is not.
		 */
		inline bool isTileVisible(const sf::Vector2u& tile) const {
			// count() is linear in complexity, whereas a simple find() is constant
			// in complexity (in the average case). We only need to know if there
			// is at least one of `tile` in this unordered multiset.
			return _visibleTiles.find(tile) != _visibleTiles.end();
		}
	private:
		/**
		 * The team that this army belongs to.
		 */
		awe::TeamID _team = 0;

		/**
		 * The script name of the country of the army.
		 */
		std::string _country;

		/**
		 * The funds this army obtains.
		 */
		awe::Funds _funds = 0;

		/**
		 * The script name of the primary/current CO.
		 */
		std::string _co_1;

		/**
		 * The script name of the secondary/tag CO.
		 */
		std::string _co_2;

		/**
		 * The units that belong to this army.
		 */
		std::unordered_set<awe::UnitID> _units;

		/**
		 * The tiles that belong to this army.
		 */
		std::unordered_set<sf::Vector2u> _tiles;

		/**
		 * A cache of tiles that are visible to all of the units belonging to this
		 * army, disregarding terrain visibility properties, tile ownership, etc.
		 * This is a multiset, as tiles may be visible to more than one unit at any
		 * given time.
		 */
		std::unordered_multiset<sf::Vector2u> _visibleTiles;

		/**
		 * Data used when initialising view objects.
		 */
		engine::logger::data _loggerData;

		/**
		 * Pointer to static game properties.
		 * Used to create view objects.
		 */
		std::shared_ptr<const awe::banks> _banks;
	};
}
