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

#pragma once

#include "unit.h"
#include "terrain.h"

namespace awe {
	typedef unsigned int TeamID;

	class army {
	public:
		army(TeamID team = 0, const awe::country* country = nullptr, unsigned int funds = 0,
			const awe::commander* firstCO = nullptr, const awe::commander* secondCO = nullptr) noexcept;

		TeamID setTeam(TeamID newTeam) noexcept;
		TeamID getTeam() const noexcept;
		const awe::country* setCountry(const awe::country* newCountry) noexcept;
		const awe::country* getCountry() const noexcept;
		int setFunds(int newFunds) noexcept;
		int getFunds() const noexcept;
		void setCommanders(const awe::commander* firstCO, const awe::commander* secondCO = nullptr) noexcept;
		const awe::commander* getFirstCommander() const noexcept;
		const awe::commander* getSecondCommander() const noexcept;
		bool isTagTeam() const noexcept;

		void addOwnedTile(awe::tile const* ptr) noexcept;
		void removeOwnedTile(awe::tile const* ptr) noexcept;
		bool isOwnedTile(awe::tile const* ptr) const noexcept;
		void clearOwnedTiles() noexcept;
		std::size_t ownedTilesCount(std::vector<const awe::terrain const*> filter = {}, const bool inverted = false) const noexcept;

		void addUnit(const awe::unit_type const* typeInfo) noexcept;
		void removeUnit(const awe::unit const* unitToDelete) noexcept;
		bool armysUnit(const awe::unit const* unitToSearch) const noexcept;
		void clearUnits() noexcept;
		std::size_t unitCount(std::vector<const awe::terrain const*> filter = {}, const bool inverted = false) const noexcept;
	private:
		TeamID _team = 0;
		const awe::country* _country = 0;
		std::vector<awe::unit> _units;
		std::vector<awe::tile const*> _ownedTiles;
		int _funds = 0;
		const awe::commander* _firstCO = nullptr;
		const awe::commander* _secondCO = nullptr;
	};
}