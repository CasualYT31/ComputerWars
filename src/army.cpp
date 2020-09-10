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

awe::army::army(TeamID team, const std::shared_ptr<const awe::country>& country, unsigned int funds,
				const std::shared_ptr<const awe::commander>& firstCO,
				const std::shared_ptr<const awe::commander>& secondCO) noexcept :
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
				auto& terrainToTest = **filterItr;
				if (*spOwnedTile->getTile()->getType() == terrainToTest) {
					if (!inverted) count++;
					break;
				}
			}
			if (inverted && filterItr == filterEnd) count++;
		}
	}
	return count;
}

std::shared_ptr<awe::unit> awe::army::addUnit(const std::shared_ptr<const awe::unit_type>& typeInfo) noexcept {
	std::shared_ptr<awe::unit> temp = std::make_shared<awe::unit>(awe::unit(typeInfo));
	_units.push_back(temp);
	return temp;
}

void awe::army::removeUnit(const std::shared_ptr<awe::unit>& unitToDelete) noexcept {
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		if (*itr && unitToDelete && **itr == *unitToDelete) {
			_units.erase(itr);
			return;
		}
	}
}

bool awe::army::isArmysUnit(const std::shared_ptr<awe::unit>& unitToSearch) const noexcept {
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		if (*itr && unitToSearch && **itr == *unitToSearch) return true;
	}
	return false;
}

void awe::army::clearUnits() noexcept {
	_units.clear();
}

std::size_t awe::army::unitCount(std::vector<std::shared_ptr<const awe::unit_type>> filter, const bool inverted) const noexcept {
	if (filter.size() == 0 && !inverted) return 0;
	if (filter.size() == 0 && inverted) return _units.size();
	std::size_t count = 0;
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		if (*itr) {
			auto filterItr = filter.begin(), filterEnd = filter.end();
			for (; filterItr != filterEnd; filterItr++) {
				if (!*filterItr) continue;
				auto& unitTypeToTest = **filterItr;
				if (*(*itr)->getType() == unitTypeToTest) {
					if (!inverted) count++;
					break;
				}
			}
			if (inverted && filterItr == filterEnd) count++;
		}
	}
	return count;
}

void awe::army::setUnitSpritesheet(const std::shared_ptr<awe::spritesheets::units>& ptr) noexcept {
	_unitSprites = ptr;
}

void awe::army::setPictureSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
	_pictureSprites = ptr;
}