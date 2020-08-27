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

#include "army.h"

awe::army::army(TeamID team, const awe::country* country, unsigned int funds,
		const awe::commander* firstCO, const awe::commander* secondCO) noexcept :
		_team(team), _country(country), _funds(funds), _firstCO(firstCO), _secondCO(secondCO) {}

awe::TeamID awe::army::setTeam(awe::TeamID newTeam) noexcept {
	auto old = getTeam();
	_team = newTeam;
	return old;
}

awe::TeamID awe::army::getTeam() const noexcept {
	return _team;
}

void awe::army::setCountry(std::shared_ptr<const awe::country> newCountry) noexcept {
	if (newCountry) _country = newCountry;
}

std::shared_ptr<const awe::country> awe::army::getCountry() const noexcept {
	return _country;
}

unsigned int awe::army::setFunds(unsigned int newFunds) noexcept {
	auto old = getFunds();
	_funds = newFunds;
	if (_funds < 0) _funds = 0;
	return old;
}

unsigned int awe::army::getFunds() const noexcept {
	return _funds;
}

void awe::army::setCommanders(std::shared_ptr<const awe::commander> firstCO, std::shared_ptr<const awe::commander> secondCO) noexcept {
	_firstCO = firstCO;
	_secondCO = secondCO;
	if (!_firstCO && _secondCO) std::swap(_firstCO, _secondCO);
}

std::shared_ptr<const awe::commander> awe::army::getFirstCommander() const noexcept {
	return _firstCO;
}

std::shared_ptr<const awe::commander> awe::army::getSecondCommander() const noexcept {
	return _secondCO;
}

bool awe::army::isTagTeam() const noexcept {
	return _firstCO && _secondCO;
}

void awe::army::addOwnedTile(const std::weak_ptr<awe::tile>& ptr) noexcept {
	if (ptr.expired()) return;
	_ownedTiles.push_back(ptr);
}

void awe::army::removeOwnedTile(const std::shared_ptr<awe::tile>& ptr) noexcept {
	if (!ptr) return;
	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
		auto spOwnedTile = itr->lock();
		if (spOwnedTile && *spOwnedTile == *ptr) {
			_ownedTiles.erase(itr);
			return;
		}
	}
}

bool awe::army::isOwnedTile(const std::shared_ptr<awe::tile>& ptr) const noexcept {
	if (!ptr) return false;
	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
		auto spOwnedTile = itr->lock();
		if (spOwnedTile && *spOwnedTile == *ptr) return true;
	}
	return false;
}

void awe::army::clearOwnedTiles() noexcept {
	_ownedTiles.clear();
}

std::size_t awe::army::ownedTilesCount(std::vector<std::shared_ptr<const awe::terrain>> filter, const bool inverted) const noexcept {
	if (filter.size() == 0 && !inverted) return 0;
	if (filter.size() == 0 && inverted) return _ownedTiles.size();
	std::size_t count = 0;
	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
		auto spOwnedTile = itr->lock();
		if (spOwnedTile) {
			auto filterItr = filter.begin(), filterEnd = filter.end();
			for (; filterItr != filterEnd; filterItr++) {
				if (!*filterItr) continue;
				auto terrainToTest = **filterItr;
				if (spOwnedTile && *spOwnedTile->getTile()->getType() == terrainToTest) {
					if (!inverted) count++;
					break;
				}
			}
			if (inverted && filterItr == filterEnd) count++;
		}
	}
	return count;
}