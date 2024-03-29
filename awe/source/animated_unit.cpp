/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "animated_unit.hpp"

awe::animated_unit::animated_unit(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
	const engine::logger::data& data) :
	_sprite(sheet, "", data), _hpIcon({ data.sink, data.name + "_hp_icon" }),
	_fuelAmmoIcon({ data.sink, data.name + "_fuel_ammo_icon" }),
	_loadedIcon({ data.sink, data.name + "_loaded_icon" }),
	_capturingHidingIcon({ data.sink, data.name + "_status_icon" }),
	_onlyShowIconsWhenThisMainSpritesheetIsActive(sheet) {}

void awe::animated_unit::setSpritesheetOverride(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet) {
	if (!_oldSheet) _oldSheet = getSpritesheet();
	_sprite.setSpritesheet(sheet);
	if (_targetCache) animate(*_targetCache);
}

void awe::animated_unit::clearSpritesheetOverride() {
	if (!_oldSheet) return;
	const std::shared_ptr<const sfx::animated_spritesheet> oldSheet = *_oldSheet;
	_oldSheet.reset();
	setSpritesheet(oldSheet);
	if (_targetCache) animate(*_targetCache);
}

void awe::animated_unit::setIconSpritesheet(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet) {
	if (_oldIconSheet) {
		_oldIconSheet = sheet;
		return;
	}
	_hpIcon.setSpritesheet(sheet);
	_fuelAmmoIcon.setSpritesheet(sheet);
	_loadedIcon.setSpritesheet(sheet);
	_capturingHidingIcon.setSpritesheet(sheet);
}

void awe::animated_unit::setIconSpritesheetOverride(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet) {
	if (!_oldIconSheet) _oldIconSheet = getIconSpritesheet();
	_hpIcon.setSpritesheet(sheet);
	_fuelAmmoIcon.setSpritesheet(sheet);
	_loadedIcon.setSpritesheet(sheet);
	_capturingHidingIcon.setSpritesheet(sheet);
	if (_targetCache) animate(*_targetCache);
}

void awe::animated_unit::clearIconSpritesheetOverride() {
	if (!_oldIconSheet) return;
	const std::shared_ptr<const sfx::animated_spritesheet> oldSheet =
		*_oldIconSheet;
	_oldIconSheet.reset();
	setIconSpritesheet(oldSheet);
	if (_targetCache) animate(*_targetCache);
}

bool awe::animated_unit::animate(const sf::RenderTarget& target) {
	_targetCache = &target;
	// Animate sprites.
	_hpIcon.animate(target);
	_fuelAmmoIcon.animate(target);
	_loadedIcon.animate(target);
	_capturingHidingIcon.animate(target);
	bool ret = _sprite.animate(target);
	// Continusouly update the sprite's origin to be at the centre bottom.
	sf::Vector2f size = _sprite.getSize();
	_sprite.setOrigin(sf::Vector2f(size.x * 0.5f, size.y));
	// Calculate icon positions. pos needs to point to the UL corner of the sprite.
	sf::Vector2f pos = _sprite.getPosition();
	pos.x -= size.x * 0.5f;
	pos.y -= size.y;
	_hpIcon.setPosition(sf::Vector2f(pos.x + size.x - _hpIcon.getSize().x,
		pos.y + size.y - _hpIcon.getSize().y));
	_fuelAmmoIcon.setPosition(sf::Vector2f(pos.x, pos.y + size.y -
		_fuelAmmoIcon.getSize().y));
	_loadedIcon.setPosition(pos);
	_capturingHidingIcon.setPosition(sf::Vector2f(
		pos.x + size.x - _capturingHidingIcon.getSize().x, pos.y));
	// Return main unit graphic animation result.
	return ret;
}

void awe::animated_unit::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	target.draw(_sprite, states);
	if (getSpritesheet() != _onlyShowIconsWhenThisMainSpritesheetIsActive)
		return;
	target.draw(_loadedIcon, states);
	target.draw(_capturingHidingIcon, states);
	target.draw(_fuelAmmoIcon, states);
	target.draw(_hpIcon, states);
}
