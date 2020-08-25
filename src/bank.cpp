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

#include "bank.h"
#include <algorithm>

void awe::updateAllTerrains(awe::bank<awe::tile_type>& tileBank, const awe::bank<awe::terrain>& terrainBank) noexcept {
	for (std::size_t i = 0; i < tileBank.size(); i++) {
		tileBank[(awe::bank<awe::tile_type>::index)i]->updateTerrain(terrainBank);
	}
}

//*******************
//*COMMON PROPERTIES*
//*******************
awe::common_properties::common_properties(safe::json& j) noexcept {
	j.apply(_name, { "longname" }, &_name, true);
	j.apply(_shortName, { "shortname" }, &_shortName, true);
	j.apply(_iconKey, { "icon" }, &_iconKey, true);
	j.apply(_description, { "description" }, &_description, true);
}
awe::common_properties::~common_properties() noexcept {}
const std::string& awe::common_properties::getName() const noexcept {
	return _name;
}
const std::string& awe::common_properties::getShortName() const noexcept {
	return _shortName;
}
unsigned int awe::common_properties::getIconKey() const noexcept {
	return _iconKey;
}
const std::string& awe::common_properties::getDescription() const noexcept {
	return _description;
}

//*********
//*COUNTRY*
//*********
awe::country::country(safe::json& j) noexcept : common_properties(j) {
	j.applyColour(_colour, { "colour" }, &_colour, true);
}
const sf::Color& awe::country::getColour() const noexcept {
	return _colour;
}

//*********
//*WEATHER*
//*********
awe::weather::weather(safe::json& j) noexcept : common_properties(j) {}

//*************
//*ENVIRONMENT*
//*************
awe::environment::environment(safe::json& j) noexcept : common_properties(j) {}

//***************
//*MOVEMENT TYPE*
//***************
awe::movement_type::movement_type(safe::json& j) noexcept : common_properties(j) {}

//*********
//*TERRAIN*
//*********
awe::terrain::terrain(safe::json& j) noexcept : common_properties(j) {
	j.apply(_maxHP, { "hp" }, &_maxHP, true);
	if (_maxHP > INT_MAX) _maxHP = INT_MAX;
	j.apply(_defence, { "defence" }, &_defence, true);
	j.apply(_isCapturable, { "capturable" }, &_isCapturable, true);
	j.applyVector(_movecosts, { "movecosts" });
	j.resetState();
	j.applyVector(_pictures, { "pictures" });
}
unsigned int awe::terrain::getMaxHP() const noexcept {
	return _maxHP;
}
unsigned int awe::terrain::getDefence() const noexcept {
	return _defence;
}
int awe::terrain::getMoveCost(const awe::bank<awe::movement_type>::index movecostID) const noexcept {
	if (movecostID >= _movecosts.size()) return -1;
	return _movecosts[movecostID];
}
unsigned int awe::terrain::getPicture(const awe::bank<awe::country>::index countryID) const noexcept {
	if (countryID >= _pictures.size()) return UINT_MAX;
	return _pictures[countryID];
}
bool awe::terrain::isCapturable() const noexcept {
	return _isCapturable;
}

//******
//*TILE*
//******
awe::tile_type::tile_type(safe::json& j) noexcept {
	j.apply(_terrainType, { "type" }, &_terrainType, true);
	j.applyVector(_tiles, { "tiles" });
}
awe::bank<awe::terrain>::index awe::tile_type::getTypeIndex() const noexcept {
	return _terrainType;
}
unsigned int awe::tile_type::getTile(awe::bank<awe::country>::index countryID) const noexcept {
	if (countryID >= _tiles.size()) return UINT_MAX;
	return _tiles[countryID];
}
const awe::terrain* awe::tile_type::getType() const noexcept {
	return _terrain;
}
void awe::tile_type::updateTerrain(const awe::bank<awe::terrain>& terrainBank) const noexcept {
	_terrain = terrainBank[_terrainType];
}

//******
//*UNIT*
//******
awe::unit_type::unit_type(safe::json& j) noexcept : common_properties(j) {
	j.apply(_movementTypeID, { "movetype" }, &_movementTypeID, true);
	j.apply(_cost, { "price" }, &_cost, true);
	j.apply(_maxFuel, { "fuel" }, &_maxFuel, true);
	j.apply(_maxAmmo, { "ammo" }, &_maxAmmo, true);
	j.apply(_maxHP, { "hp" }, &_maxHP, true);
	if (_maxHP > INT_MAX) _maxHP = INT_MAX;
	j.apply(_movementPoints, { "mp" }, &_movementPoints, true);
	j.apply(_vision, { "vision" }, &_vision, true);
	j.apply(_lowerRange, { "lowrange" }, &_lowerRange, true);
	j.apply(_higherRange, { "highrange" }, &_higherRange, true);
	if (_lowerRange > _higherRange) std::swap(_lowerRange, _higherRange);
	j.applyVector(_pictures, { "pictures" });
	j.resetState();
	j.applyVector(_units, { "sprites" });
	j.resetState();
	j.applyVector(_canLoadThese, { "canload" });
}
awe::bank<awe::movement_type>::index awe::unit_type::getMovementType() const noexcept {
	return _movementTypeID;
}
unsigned int awe::unit_type::getPicture(awe::bank<awe::country>::index countryID) const noexcept {
	if (countryID >= _pictures.size()) return UINT_MAX;
	return _pictures[countryID];
}
unsigned int awe::unit_type::getUnit(awe::bank<awe::country>::index countryID) const noexcept {
	if (countryID >= _units.size()) return UINT_MAX;
	return _units[countryID];
}
unsigned int awe::unit_type::getCost() const noexcept {
	return _cost;
}
int awe::unit_type::getMaxFuel() const noexcept {
	return _maxFuel;
}
int awe::unit_type::getMaxAmmo() const noexcept {
	return _maxAmmo;
}
unsigned int awe::unit_type::getMaxHP() const noexcept {
	return _maxHP;
}
unsigned int awe::unit_type::getMovementPoints() const noexcept {
	return _movementPoints;
}
unsigned int awe::unit_type::getVision() const noexcept {
	return _vision;
}
unsigned int awe::unit_type::getLowerRange() const noexcept {
	return _lowerRange;
}
unsigned int awe::unit_type::getHigherRange() const noexcept {
	return _higherRange;
}
bool awe::unit_type::isInfiniteFuel() const noexcept {
	return _maxFuel < 0;
}
bool awe::unit_type::isInfiniteAmmo() const noexcept {
	return _maxAmmo < 0;
}
bool awe::unit_type::canLoad(const awe::bank<unit_type>::index typeID) const noexcept {
	return std::find(_canLoadThese.begin(), _canLoadThese.end(), typeID) != _canLoadThese.end();
}

//***********
//*COMMANDER*
//***********
awe::commander::commander(safe::json& j) noexcept : common_properties(j) {
	j.apply(_portrait, { "portrait" }, &_portrait, true);
}
unsigned int awe::commander::getPortrait() const noexcept {
	return _portrait;
}