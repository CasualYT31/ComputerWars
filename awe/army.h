/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file army.h
 * Declares the class which represents an in-game army.
 */

#include "bank.h"
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
	 */
	class army {
	public:
		/**
		 * Reserved value representing no army.
		 * Assigned the maximum value of the unsigned type of \c awe::ArmyID.
		 */
		static const awe::ArmyID NO_ARMY = ~((awe::ArmyID)0);

		/**
		 * Constructs a new army.
		 * @param country The country the army belongs to, which can't be changed.
		 */
		army(const std::shared_ptr<const awe::country>& country = nullptr)
			noexcept;

		/**
		 * Gets the army's country information.
		 * @return The army's country information.
		 */
		std::shared_ptr<const awe::country> getCountry() const noexcept;

		/**
		 * Updates the army's fund count.
		 * If a value below \c 0 is given, \c 0 will be stored.
		 * @param funds The fund count to replace this army's old fund count with.
		 */
		void setFunds(const awe::Funds funds) noexcept;

		/**
		 * Retrieves the army's fund count.
		 * By default, it is \c 0.
		 * @return The funds this army obtains.
		 */
		awe::Funds getFunds() const noexcept;

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
		 * The country of the army.
		 */
		const std::shared_ptr<const awe::country> _country;

		/**
		 * The funds this army obtains.
		 */
		awe::Funds _funds = 0;

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
