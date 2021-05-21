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

awe::army::army(const std::shared_ptr<const awe::country>& country, awe::TeamID team) noexcept : _country(country), _team(team) {}

void awe::army::setCountry(const std::shared_ptr<const awe::country>& country) noexcept {
	if (country) _country = country;
}

std::shared_ptr<const awe::country> awe::army::getCountry() const noexcept {
	return _country;
}

void awe::army::setTeam(const awe::TeamID team) noexcept {
	_team = team;
}

awe::TeamID awe::army::getTeam() const noexcept {
	return _team;
}

void awe::army::setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		(*itr)->setUnitSpritesheet(ptr);
	}
}

void awe::army::createUnit(const std::shared_ptr<const awe::unit_type>& unitType) noexcept {
	_units.push_back(std::make_shared<awe::unit>(unitType));
}

bool awe::army::deleteUnit(engine::uuid<awe::unit> uuid) noexcept {
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		if ((*itr)->UUID == uuid) {
			// should automatically handle deletion of loaded units,
			// as when a unit is loaded onto another one, it should be
			// removed entirely from the army list and put into the
			// holder unit's internal list, meaning that army doesn't
			// hold the reference anymore and so it will be "garbage
			// collected"
			_units.erase(itr);
			return true;
		} else {
			// check to see if the desired unit is loaded onto this one
			// if so, it will be deleted and true will return,
			// and so the loop should be broken out of
			if ((*itr)->deleteUnit(uuid)) return true;
		}
	}
	return false;
}

std::shared_ptr<awe::unit> awe::army::_findUnit(engine::uuid<awe::unit> uuid) const noexcept {
	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
		if ((*itr)->UUID == uuid) return *itr;
	}
	return nullptr;
}

//awe::army::army(TeamID team, const std::shared_ptr<const awe::country>& country) noexcept : _team(team), _country(country) {}
//
//void awe::army::setTeam(awe::TeamID newTeam) noexcept {
//	_team = newTeam;
//}
//
//awe::TeamID awe::army::getTeam() const noexcept {
//	return _team;
//}
//
//void awe::army::setCountry(std::shared_ptr<const awe::country> newCountry) noexcept {
//	if (newCountry) _country = newCountry;
//}
//
//std::shared_ptr<const awe::country> awe::army::getCountry() const noexcept {
//	return _country;
//}
//
//void awe::army::setFunds(awe::Funds newFunds) noexcept {
//	_funds = newFunds;
//	if (_funds < 0) _funds = 0;
//}
//
//awe::Funds awe::army::getFunds() const noexcept {
//	return _funds;
//}
//
//void awe::army::setCommanders(std::shared_ptr<const awe::commander> firstCO, std::shared_ptr<const awe::commander> secondCO) noexcept {
//	_firstCO = firstCO;
//	_secondCO = secondCO;
//	if (!_firstCO && _secondCO) std::swap(_firstCO, _secondCO);
//}
//
//std::shared_ptr<const awe::commander> awe::army::getFirstCommander() const noexcept {
//	return _firstCO;
//}
//
//std::shared_ptr<const awe::commander> awe::army::getSecondCommander() const noexcept {
//	return _secondCO;
//}
//
//void awe::army::addOwnedTile(const std::weak_ptr<awe::tile>& ptr) noexcept {
//	if (ptr.expired()) return;
//	_ownedTiles.push_back(ptr);
//}
//
//void awe::army::removeOwnedTile(const std::shared_ptr<awe::tile>& ptr) noexcept {
//	if (!ptr) return;
//	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
//		auto spOwnedTile = itr->lock();
//		if (spOwnedTile && *spOwnedTile == *ptr) {
//			_ownedTiles.erase(itr);
//			return;
//		}
//	}
//}
//
//bool awe::army::isOwnedTile(const std::shared_ptr<awe::tile>& ptr) const noexcept {
//	if (!ptr) return false;
//	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
//		auto spOwnedTile = itr->lock();
//		if (spOwnedTile && *spOwnedTile == *ptr) return true;
//	}
//	return false;
//}
//
//void awe::army::clearOwnedTiles() noexcept {
//	_ownedTiles.clear();
//}
//
//std::size_t awe::army::ownedTilesCount(std::vector<std::shared_ptr<const awe::terrain>> filter, const bool inverted) const noexcept {
//	if (filter.size() == 0 && !inverted) return 0;
//	if (filter.size() == 0 && inverted) return _ownedTiles.size();
//	std::size_t count = 0;
//	for (auto itr = _ownedTiles.begin(), enditr = _ownedTiles.end(); itr != enditr; itr++) {
//		auto spOwnedTile = itr->lock();
//		if (spOwnedTile) {
//			auto filterItr = filter.begin(), filterEnd = filter.end();
//			for (; filterItr != filterEnd; filterItr++) {
//				if (!*filterItr) continue;
//				auto& terrainToTest = **filterItr;
//				if (*spOwnedTile->getTile()->getType() == terrainToTest) {
//					if (!inverted) count++;
//					break;
//				}
//			}
//			if (inverted && filterItr == filterEnd) count++;
//		}
//	}
//	return count;
//}
//
//std::shared_ptr<awe::unit> awe::army::addUnit(const std::shared_ptr<const awe::unit_type>& typeInfo) noexcept {
//	std::shared_ptr<awe::unit> temp = std::make_shared<awe::unit>(awe::unit(typeInfo));
//	if (temp) {
//		temp->setSpritesheet(_unitSprites);
//		_units.push_back(temp);
//	}
//	return temp;
//}
//
//void awe::army::removeUnit(const std::shared_ptr<awe::unit>& unitToDelete) noexcept {
//	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
//		if (*itr && unitToDelete && **itr == *unitToDelete) {
//			_units.erase(itr);
//			return;
//		}
//	}
//}
//
//bool awe::army::isArmysUnit(const std::shared_ptr<awe::unit>& unitToSearch) const noexcept {
//	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
//		if (*itr && unitToSearch && **itr == *unitToSearch) return true;
//	}
//	return false;
//}
//
//void awe::army::clearUnits() noexcept {
//	_units.clear();
//}
//
//std::size_t awe::army::unitCount(std::vector<std::shared_ptr<const awe::unit_type>> filter, const bool inverted) const noexcept {
//	if (filter.size() == 0 && !inverted) return 0;
//	if (filter.size() == 0 && inverted) return _units.size();
//	std::size_t count = 0;
//	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
//		if (*itr) {
//			auto filterItr = filter.begin(), filterEnd = filter.end();
//			for (; filterItr != filterEnd; filterItr++) {
//				if (!*filterItr) continue;
//				auto& unitTypeToTest = **filterItr;
//				if (*(*itr)->getType() == unitTypeToTest) {
//					if (!inverted) count++;
//					break;
//				}
//			}
//			if (inverted && filterItr == filterEnd) count++;
//		}
//	}
//	return count;
//}
//
//void awe::army::setUnitSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
//	// update all the unit's spritesheets
//	_unitSprites = ptr;
//	for (auto itr = _units.begin(), enditr = _units.end(); itr != enditr; itr++) {
//		if (*itr) (*itr)->setSpritesheet(ptr);
//	}
//}
//
//void awe::army::setPictureSpritesheet(const std::shared_ptr<sfx::animated_spritesheet>& ptr) noexcept {
//	_pictureSprites = ptr;
//}
//
//bool awe::army::operator==(const awe::army& rhs) const noexcept {
//	return UUID == rhs.UUID;
//}
//
//bool awe::army::operator!=(const awe::army& rhs) const noexcept {
//	return !(*this == rhs);
//}
//
//bool awe::army::animate(const sf::RenderTarget& target) noexcept {
//	return false;
//}
//
//void awe::army::draw(sf::RenderTarget& target, sf::RenderStates states) const {
//
//}