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

#include "army.hpp"
#include <algorithm>

awe::ArmyID awe::army::NO_ARMY_SCRIPT = ~((awe::ArmyID)0);

awe::army::army(const std::shared_ptr<const awe::country>& country) noexcept :
	_country(country) {}

std::shared_ptr<const awe::country> awe::army::getCountry() const noexcept {
	return _country;
}

void awe::army::setFunds(const awe::Funds funds) noexcept {
	if (funds <= 0) {
		_funds = 0;
	} else {
		_funds = funds;
	}
}

awe::Funds awe::army::getFunds() const noexcept {
	return _funds;
}

void awe::army::setCOs(const std::shared_ptr<const awe::commander>& current,
	const std::shared_ptr<const awe::commander>& tag) noexcept {
	if (!current && tag) {
		_co_1 = tag;
		_co_2 = nullptr;
	} else {
		_co_1 = current;
		_co_2 = tag;
	}
}

void awe::army::tagCOs() noexcept {
	if (_co_2) std::swap(_co_1, _co_2);
}

std::shared_ptr<const awe::commander> awe::army::getCurrentCO() const noexcept {
	return _co_1;
}

std::shared_ptr<const awe::commander> awe::army::getTagCO() const noexcept {
	return _co_2;
}

void awe::army::addUnit(const awe::UnitID unit) noexcept {
	_units.insert(unit);
}

void awe::army::removeUnit(const awe::UnitID unit) noexcept {
	_units.erase(unit);
}

std::unordered_set<awe::UnitID> awe::army::getUnits() const noexcept {
	return _units;
}

void awe::army::addTile(const sf::Vector2u tile) noexcept {
	_tiles.insert(tile);
}

void awe::army::removeTile(const sf::Vector2u tile) noexcept {
	_tiles.erase(tile);
}

std::unordered_set<sf::Vector2u> awe::army::getTiles() const noexcept {
	return _tiles;
}