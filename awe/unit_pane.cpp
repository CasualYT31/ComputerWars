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

#include "unit_pane.h"

awe::unit_pane::unit_pane() noexcept {
	_unitName.setCharacterSize(16);
	_unitName.setFillColor(sf::Color::White);
	_unitName.setOutlineColor(sf::Color::Black);
	_unitName.setOutlineThickness(1.5f);
	_unitHP.setCharacterSize(12);
	_unitHP.setFillColor(sf::Color::White);
	_unitHP.setOutlineColor(sf::Color::Black);
	_unitHP.setOutlineThickness(1.5f);
	_unitFuel.setCharacterSize(12);
	_unitFuel.setFillColor(sf::Color::White);
	_unitFuel.setOutlineColor(sf::Color::Black);
	_unitFuel.setOutlineThickness(1.5f);
	_unitAmmo.setCharacterSize(12);
	_unitAmmo.setFillColor(sf::Color::White);
	_unitAmmo.setOutlineColor(sf::Color::Black);
	_unitAmmo.setOutlineThickness(1.5f);
}

void awe::unit_pane::setUnit(const awe::unit& unit) noexcept {
	_unit = std::make_shared<const awe::unit>(unit);
}

void awe::unit_pane::setRect(const sf::FloatRect& rect) noexcept {
	_rect = rect;
}

void awe::unit_pane::setSpritesheet(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet) noexcept {
	_unitHPIcon.setSpritesheet(sheet);
	_unitHPIcon.setSprite("hp");
	_unitFuelIcon.setSpritesheet(sheet);
	_unitFuelIcon.setSprite("fuel");
	_unitAmmoIcon.setSpritesheet(sheet);
	_unitAmmoIcon.setSprite("ammo");
}

void awe::unit_pane::setFont(const std::shared_ptr<const sf::Font>& font) noexcept
	{
	if (font) {
		_unitName.setFont(*font);
		_unitHP.setFont(*font);
		_unitFuel.setFont(*font);
		_unitAmmo.setFont(*font);
	}
}

void awe::unit_pane::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) noexcept {
	if (dict) _dict = dict;
}

bool awe::unit_pane::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	const float tileCentre = _rect.left + _rect.width / 2.0f;
	// icon
	if (_unitIcon.getSpritesheet() != _unit->getSpritesheet()) {
		_unitIcon.setSpritesheet(_unit->getSpritesheet());
	}
	if (_unitIcon.getSprite() != _unit->getSprite()) {
		_unitIcon.setSprite(_unit->getSprite());
	}
	_unitIcon.animate(target, scaling);
	_unitIcon.setPosition(
		sf::Vector2f(tileCentre - _unitIcon.getSize().x / 2.0f, _rect.top + 10.0f)
	);
	// name
	_unitName.setString(_unit->getType()->getShortName());
	_unitName.setPosition(
		sf::Vector2f(tileCentre - _unitName.getLocalBounds().width / 2.0f,
			_unitIcon.getPosition().y + _unitIcon.getSize().y)
	);
	// ammo
	_unitAmmoIcon.animate(target, scaling);
	_unitAmmoIcon.setPosition(sf::Vector2f(_rect.left + 10.0f,
		_rect.top + _rect.height - 10.0f - _unitAmmoIcon.getSize().y));
	_unitAmmo.setString(std::to_string(_unit->getAmmo()));
	_unitAmmo.setPosition(
		sf::Vector2f(_rect.left + _rect.width -
			_unitAmmo.getLocalBounds().width - 10.0f,
			_unitAmmoIcon.getPosition().y - 3.0f)
	);
	// fuel
	_unitFuelIcon.animate(target, scaling);
	_unitFuelIcon.setPosition(sf::Vector2f(_rect.left + 10.0f,
		_unitAmmoIcon.getPosition().y - 5.0f - _unitFuelIcon.getSize().y));
	_unitFuel.setString(std::to_string(_unit->getFuel()));
	_unitFuel.setPosition(
		sf::Vector2f(_rect.left + _rect.width -
			_unitFuel.getLocalBounds().width - 10.0f,
			_unitFuelIcon.getPosition().y - 3.0f)
	);
	// HP
	_unitHPIcon.animate(target, scaling);
	_unitHPIcon.setPosition(sf::Vector2f(_rect.left + 10.0f,
		_unitFuelIcon.getPosition().y - 5.0f - _unitHPIcon.getSize().y));
	_unitHP.setString(std::to_string(_unit->getDisplayedHP()));
	_unitHP.setPosition(
		sf::Vector2f(_rect.left + _rect.width -
			_unitHP.getLocalBounds().width - 10.0f,
			_unitHPIcon.getPosition().y - 3.0f)
	);
	return true;
}

void awe::unit_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	target.draw(_unitIcon, states);
	target.draw(_unitName, states);
	target.draw(_unitHPIcon, states);
	target.draw(_unitFuelIcon, states);
	target.draw(_unitAmmoIcon, states);
	target.draw(_unitHP, states);
	target.draw(_unitFuel, states);
	target.draw(_unitAmmo, states);
}