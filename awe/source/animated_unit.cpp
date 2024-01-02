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

awe::animated_unit::animated_unit(const engine::logger::data& data) :
	_sprite(data), _hpIcon({ data.sink, data.name + "_hp_icon" }),
	_fuelAmmoIcon({ data.sink, data.name + "_fuel_ammo_icon" }),
	_loadedIcon({ data.sink, data.name + "_loaded_icon" }),
	_capturingHidingIcon({ data.sink, data.name + "_status_icon" }) {}

void awe::animated_unit::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_hpIcon.setSpritesheet(sheet);
	_fuelAmmoIcon.setSpritesheet(sheet);
	_loadedIcon.setSpritesheet(sheet);
	_capturingHidingIcon.setSpritesheet(sheet);
}

bool awe::animated_unit::animate(const sf::RenderTarget& target) {
	// Animate sprites.
	_hpIcon.animate(target);
	_fuelAmmoIcon.animate(target);
	_loadedIcon.animate(target);
	_capturingHidingIcon.animate(target);
	bool ret = _sprite.animate(target);
	// Calculate icon positions.
	sf::Vector2f pos = _sprite.getPosition();
	sf::Vector2f size = _sprite.getSize();
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
	target.draw(_loadedIcon, states);
	target.draw(_capturingHidingIcon, states);
	target.draw(_fuelAmmoIcon, states);
	target.draw(_hpIcon, states);
}
