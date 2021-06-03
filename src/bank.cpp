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

#include "bank.h"
#include <algorithm>

void awe::updateAllTerrains(awe::bank<awe::tile_type>& tileBank,
	const awe::bank<awe::terrain>& terrainBank) noexcept {
	for (awe::BankID i = 0; i < tileBank.size(); i++) {
		tileBank[(awe::bank<awe::tile_type>::index)i]->updateTerrain(terrainBank);
	}
}

void awe::updateAllMovementsAndLoadedUnits(awe::bank<awe::unit_type>& unitBank,
	const awe::bank<awe::movement_type>& movementBank) noexcept {
	for (awe::BankID i = 0; i < unitBank.size(); i++) {
		unitBank[(awe::bank<awe::unit_type>::index)i]->updateMovementType(
			movementBank
		);
		unitBank[(awe::bank<awe::unit_type>::index)i]->updateUnitTypes(
			unitBank
		);
	}
}

//*********
//*BANK ID*
//*********
awe::bank_id::bank_id(const awe::BankID id) noexcept : _id(id) {}
awe::bank_id::~bank_id() noexcept {}
awe::BankID awe::bank_id::getID() const noexcept { return _id; }

//*******************
//*COMMON PROPERTIES*
//*******************
awe::common_properties::common_properties(const awe::BankID id, safe::json& j)
	noexcept : bank_id(id) {
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
awe::country::country(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {
	j.applyColour(_colour, { "colour" }, &_colour, true);
}
const sf::Color& awe::country::getColour() const noexcept {
	return _colour;
}
bool awe::country::operator==(const awe::country& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::country::operator!=(const awe::country& rhs) const noexcept {
	return !(*this == rhs);
}

//*********
//*WEATHER*
//*********
awe::weather::weather(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {}
bool awe::weather::operator==(const awe::weather& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::weather::operator!=(const awe::weather& rhs) const noexcept {
	return !(*this == rhs);
}

//*************
//*ENVIRONMENT*
//*************
awe::environment::environment(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {}
bool awe::environment::operator==(const awe::environment& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::environment::operator!=(const awe::environment& rhs) const noexcept {
	return !(*this == rhs);
}

//***************
//*MOVEMENT TYPE*
//***************
awe::movement_type::movement_type(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {}
bool awe::movement_type::operator==(const awe::movement_type& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::movement_type::operator!=(const awe::movement_type& rhs) const noexcept {
	return !(*this == rhs);
}

//*********
//*TERRAIN*
//*********
awe::terrain::terrain(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {
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
int awe::terrain::getMoveCost(const awe::bank<awe::movement_type>::index
	movecostID) const noexcept {
	if (movecostID >= _movecosts.size()) return -1;
	return _movecosts[movecostID];
}
unsigned int awe::terrain::getPicture(const awe::bank<awe::country>::index
	countryID) const noexcept {
	if (countryID >= _pictures.size()) return UINT_MAX;
	return _pictures[countryID];
}
bool awe::terrain::isCapturable() const noexcept {
	return _isCapturable;
}
std::vector<int> awe::terrain::copyMoveCosts() const noexcept {
	return _movecosts;
}
std::vector<unsigned int> awe::terrain::copyPictures() const noexcept {
	return _pictures;
}
bool awe::terrain::operator==(const awe::terrain& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::terrain::operator!=(const awe::terrain& rhs) const noexcept {
	return !(*this == rhs);
}

//******
//*TILE*
//******
awe::tile_type::tile_type(const awe::BankID id, safe::json& j) noexcept :
	bank_id(id) {
	j.apply(_terrainType, { "type" }, &_terrainType, true);
	j.applyVector(_tiles, { "tiles" });
	j.apply(_neutralTile, { "neutral" }, &_neutralTile, true);
}
awe::bank<awe::terrain>::index awe::tile_type::getTypeIndex() const noexcept {
	return _terrainType;
}
unsigned int awe::tile_type::getOwnedTile(awe::bank<awe::country>::index countryID)
	const noexcept {
	if (countryID >= _tiles.size()) return _neutralTile;
	return _tiles[countryID];
}
unsigned int awe::tile_type::getNeutralTile() const noexcept {
	return _neutralTile;
}
std::shared_ptr<const awe::terrain> awe::tile_type::getType() const noexcept {
	return _terrain;
}
void awe::tile_type::updateTerrain(const awe::bank<awe::terrain>& terrainBank)
	const noexcept {
	_terrain = terrainBank[_terrainType];
}
bool awe::tile_type::operator==(const awe::tile_type& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::tile_type::operator!=(const awe::tile_type& rhs) const noexcept {
	return !(*this == rhs);
}

//******
//*UNIT*
//******
awe::unit_type::unit_type(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {
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
	j.apply(_loadLimit, { "loadlimit" }, &_loadLimit, true);
	j.apply(_fuelPerTurn, { "fuelperturn" }, &_fuelPerTurn, true);
}
awe::bank<awe::movement_type>::index awe::unit_type::getMovementTypeIndex() const
	noexcept {
	return _movementTypeID;
}
std::shared_ptr<const awe::movement_type> awe::unit_type::getMovementType() const
	noexcept {
	return _movementType;
}
void awe::unit_type::updateMovementType(const awe::bank<awe::movement_type>&
	movementBank) const noexcept {
	_movementType = movementBank[_movementTypeID];
}
unsigned int awe::unit_type::getPicture(awe::bank<awe::country>::index countryID)
	const noexcept {
	if (countryID >= _pictures.size()) return UINT_MAX;
	return _pictures[countryID];
}
unsigned int awe::unit_type::getUnit(awe::bank<awe::country>::index countryID)
	const noexcept {
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
bool awe::unit_type::hasInfiniteFuel() const noexcept {
	return _maxFuel < 0;
}
bool awe::unit_type::hasInfiniteAmmo() const noexcept {
	return _maxAmmo < 0;
}
bool awe::unit_type::canLoad(const awe::bank<unit_type>::index typeID) const
	noexcept {
	return std::find(_canLoadThese.begin(), _canLoadThese.end(), typeID) !=
		_canLoadThese.end();
}
bool awe::unit_type::canLoad(const std::shared_ptr<const awe::unit_type>& type)
	const noexcept {
	if (!type) return false;
	for (auto& u : _canLoadTheseUnitTypes) {
		if (u && *u == *type) return true;
	}
	return false;
}
unsigned int awe::unit_type::loadLimit() const noexcept {
	return _loadLimit;
}
signed int awe::unit_type::fuelPerTurn() const noexcept {
	return _fuelPerTurn;
}
void awe::unit_type::updateUnitTypes(const awe::bank<awe::unit_type>& unitBank)
	const noexcept {
	_canLoadTheseUnitTypes.clear();
	for (awe::BankID i = 0; i < unitBank.size(); i++) {
		for (auto& u : _canLoadThese) {
			if (i == u) {
				_canLoadTheseUnitTypes.push_back(unitBank[
					(awe::bank<awe::unit_type>::index)i
				]);
				break;
			}
		}
	}
}
std::vector<unsigned int> awe::unit_type::copyPictures() const noexcept {
	return _pictures;
}
std::vector<unsigned int> awe::unit_type::copyUnits() const noexcept {
	return _units;
}
std::vector<awe::bank<awe::unit_type>::index> awe::unit_type::copyLoadableUnitIDs()
	const noexcept {
	return _canLoadThese;
}
std::vector<std::shared_ptr<const awe::unit_type>>
	awe::unit_type::copyLoadableUnits() const noexcept {
	return _canLoadTheseUnitTypes;
}
bool awe::unit_type::operator==(const awe::unit_type& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::unit_type::operator!=(const awe::unit_type& rhs) const noexcept {
	return !(*this == rhs);
}

//***********
//*COMMANDER*
//***********
awe::commander::commander(const awe::BankID id, safe::json& j) noexcept :
	common_properties(id, j) {
	j.apply(_portrait, { "portrait" }, &_portrait, true);
}
unsigned int awe::commander::getPortrait() const noexcept {
	return _portrait;
}
bool awe::commander::operator==(const awe::commander& rhs) const noexcept {
	return UUID == rhs.UUID;
}
bool awe::commander::operator!=(const awe::commander& rhs) const noexcept {
	return !(*this == rhs);
}