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

const awe::country* awe::army::setCountry(const awe::country* newCountry) noexcept {
	auto old = getCountry();
	if (newCountry) _country = newCountry;
	return old;
}

const awe::country* awe::army::getCountry() const noexcept {
	return _country;
}

int awe::army::setFunds(int newFunds) noexcept {
	auto old = getFunds();
	_funds = newFunds;
	if (_funds < 0) _funds = 0;
	return old;
}

int awe::army::getFunds() const noexcept {
	return _funds;
}

void awe::army::setCommanders(const awe::commander* firstCO, const awe::commander* secondCO) noexcept {
	_firstCO = firstCO;
	_secondCO = secondCO;
	if (!_firstCO && _secondCO) std::swap(_firstCO, _secondCO);
}

const awe::commander* awe::army::getFirstCommander() const noexcept {
	return _firstCO;
}

const awe::commander* awe::army::getSecondCommander() const noexcept {
	return _secondCO;
}

bool awe::army::isTagTeam() const noexcept {
	return _firstCO && _secondCO;
}

void awe::army::addOwnedTile(awe::tile const* ptr) noexcept {
	if (!ptr) return;
	_ownedTiles.push_back(ptr);
}

void awe::army::removeOwnedTile(awe::tile const* ptr) noexcept {
	if (!ptr) return;
	auto itemReference = std::find(_ownedTiles.begin(), _ownedTiles.end(), ptr);
	if (itemReference != _ownedTiles.end()) _ownedTiles.erase(itemReference);
}

bool awe::army::isOwnedTile(awe::tile const* ptr) const noexcept {
	if (!ptr) return false;
	return std::find(_ownedTiles.begin(), _ownedTiles.end(), ptr) != _ownedTiles.end();
}

void awe::army::clearOwnedTiles() noexcept {
	_ownedTiles.clear();
}

std::size_t awe::army::ownedTilesCount(std::vector<const awe::terrain const*> filter, const bool inverted) const noexcept {
	if (filter.size() == 0 && !inverted) return 0;
	if (filter.size() == 0 && inverted) return _ownedTiles.size();
}