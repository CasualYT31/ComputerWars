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

/**@file army.hpp
 * Declares the class which represents an in-game army.
 */

#include "bank.hpp"
#include "sfml/System/Vector2.hpp"
#include <unordered_set>

#pragma once

namespace std {
	/**
	 * Custom specialisation of \c std::hash for \c sf::Vector2.
	 * Much thanks to https://en.cppreference.com/w/cpp/utility/hash and
	 * https://stackoverflow.com/questions/9927208/requirements-for-elements-in-stdunordered-set.
	 * Also thanks to Elias Daler and Laurent
	 * (https://en.sfml-dev.org/forums/index.php?topic=24275.0).
	 */
	template<typename T> struct hash<sf::Vector2<T>> {
		std::size_t operator()(sf::Vector2<T> const& s) const noexcept {
			// I don't actually think they need to be separate...
			// Only did it this way in order to preserve Elias' intended execution
			// (since the actual hashing was in a function in their example,
			// meaning there would be two separate hashers in execution)
			std::hash<T> hasherX, hasherY;
			std::size_t seed = 0;
			seed ^= hasherX(s.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hasherY(s.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

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
		 * @param country The country the army belongs to, which can't be changed.
		 */
		army(const std::shared_ptr<const awe::country>& country) noexcept;

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
		inline std::shared_ptr<const awe::country> getCountry() const noexcept {
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
		 * If \c current is \c nullptr, but \c tag is not, then \c tag will be
		 * assigned as the primary CO and there will be no secondary CO.
		 * @param current The primary CO in charge of this army.
		 * @param tag     The secondary CO who is currently not the one in charge,
		 *                yet could be after a call to \c tagCOs(). If there will
		 *                be no secondary CO, \c nullptr can be passed.
		 */
		void setCOs(const std::shared_ptr<const awe::commander>& current,
			const std::shared_ptr<const awe::commander>& tag = nullptr) noexcept;

		/**
		 * Swaps the two COs of this army.
		 * This call will be ignored if there is no secondary CO to tag with.
		 */
		void tagCOs() noexcept;

		/**
		 * Retrieves a pointer to the information on the CO currently in charge of
		 * this army.
		 * @return Information on the current CO, or \c nullptr if there is none.
		 */
		inline std::shared_ptr<const awe::commander> getCurrentCO() const noexcept
			{
			return _co_1;
		}

		/**
		 * Retrieves a pointer to the information on the tag CO of this army.
		 * @return Information on the tag CO, or \c nullptr if there is none.
		 */
		inline std::shared_ptr<const awe::commander> getTagCO() const noexcept {
			return _co_2;
		}

		/**
		 * Adds a unit to this army's unit list.
		 * @param unit The ID of the unit to add.
		 */
		void addUnit(const awe::UnitID unit) noexcept;

		/**
		 * Removes a unit from this army's unit list.
		 * @param unit The ID of the unit to remove.
		 */
		void removeUnit(const awe::UnitID unit) noexcept;

		/**
		 * Copies the internal list of all the units that belong to this army.
		 * @return The IDs of all the units that belong to this army.
		 */
		std::unordered_set<awe::UnitID> getUnits() const noexcept;

		/**
		 * Adds a tile to this army's owned tiles list.
		 * @param tile The X and Y location of the tile.
		 */
		void addTile(const sf::Vector2u tile) noexcept;

		/**
		 * Removes a tile from this army's owned tiles list.
		 * @param tile The X and Y location of the tile.
		 */
		void removeTile(const sf::Vector2u tile) noexcept;

		/**
		 * Retrieves a list of all the tiles this army owns.
		 * @return The internal list of tile locations, copied.
		 */
		std::unordered_set<sf::Vector2u> getTiles() const noexcept;
	private:
		/**
		 * The team that this army belongs to.
		 */
		awe::TeamID _team;

		/**
		 * The country of the army.
		 */
		std::shared_ptr<const awe::country> _country;

		/**
		 * The funds this army obtains.
		 */
		awe::Funds _funds = 0;

		/**
		 * The primary/current CO.
		 */
		std::shared_ptr<const awe::commander> _co_1 = nullptr;

		/**
		 * The secondary/tag CO.
		 */
		std::shared_ptr<const awe::commander> _co_2 = nullptr;

		/**
		 * The units that belong to this army.
		 */
		std::unordered_set<awe::UnitID> _units;

		/**
		 * The tiles that belong to this army.
		 */
		std::unordered_set<sf::Vector2u> _tiles;
	};
}
