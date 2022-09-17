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

#include "army_pane.hpp"

const float awe::army_pane::_outlineThickness = 2.0f;

awe::army_pane::army_pane() noexcept {
	_funds.setCharacterSize(22);
	_funds.setFillColor(sf::Color::Black);
	_bg_border.setFillColor(sf::Color(65, 65, 65, 128));
	_rounded_bg.setOutlineColor(sf::Color(65,65,65,128));
	_rounded_bg.setOutlineThickness(_outlineThickness);
	_rounded_bg_texture.create(25.0f + _outlineThickness * 2.0f,
		50.0f + _outlineThickness * 2.0f);
}

void awe::army_pane::setArmy(const awe::army& army) noexcept {
	_army = std::make_shared<const awe::army>(army);
}

void awe::army_pane::setGeneralLocation(const awe::army_pane::location& location)
	noexcept {
	_location = location;
}

void awe::army_pane::setSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	_co.setSpritesheet(sheet);
}

void awe::army_pane::setFont(const std::shared_ptr<const sf::Font>& font) noexcept
	{
	if (font) _funds.setFont(*font);
}

bool awe::army_pane::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	sf::Color bgColour = sf::Color::White;
	if (_army) bgColour = _army->getCountry()->getColour();
	bgColour.a = 128;
	_bg.setFillColor(bgColour);
	_rounded_bg.setFillColor(bgColour);
	_outlineCover.setFillColor(bgColour);
	// CO (minus positioning)
	if (_army && _army->getCurrentCO() &&
		_army->getCurrentCO()->getID() != _oldCoSprite) {
		_oldCoSprite = _army->getCurrentCO()->getID();
		_co.setSprite(_army->getCurrentCO()->getIconName());
	}
	_co.animate(target, scaling);
	// funds (minus positioning)
	if (_army)
		_funds.setString("G. " + std::to_string(_army->getFunds()));
	else
		_funds.setString("G. ");
	// ensure original transform has been cleared
	_position = sf::Transform();
	if (_location == awe::army_pane::location::Left) {
		_animateLeft();
	} else if (_location == awe::army_pane::location::Right) {
		_animateRight(target, scaling);
	}
	return true;
}

void awe::army_pane::_animateLeft() noexcept {
	// _rounded_bg_texture.create() relies on size!
	sf::Vector2f size = sf::Vector2f(200.0f, 50.0f);
	sf::Vector2f origin = sf::Vector2f(0.0f, 0.0f);
	// step 1: pane background
	_bg.setPosition(origin);
	_bg.setSize(size);
	_bg_border.setPosition(sf::Vector2f(origin.x, origin.y + size.y));
	_bg_border.setSize(
		sf::Vector2f(size.x + _outlineThickness, _outlineThickness)
	);
	_rounded_bg.setPosition(
		sf::Vector2f(0.0f - size.y / 2.0f, 0.0f)
	);
	_rounded_bg.setRadius(size.y / 2.0f);
	_rounded_bg_texture.clear(sf::Color(0, 0, 0, 0));
	_rounded_bg_texture.draw(_rounded_bg, sf::BlendNone);
	_rounded_bg_texture.display();
	_rounded_bg_sprite.setPosition(
		sf::Vector2f(origin.x + size.x + _outlineThickness, origin.y)
	);
	_rounded_bg_sprite.setTexture(_rounded_bg_texture.getTexture());
	_outlineCover.setPosition(sf::Vector2f(origin.x + size.x, origin.y));
	_outlineCover.setSize(sf::Vector2f(_outlineThickness, size.y));
	// step 2: CO face
	_co.setPosition(sf::Vector2f(origin.x + 5.0f, origin.y + 5.0f));
	// step 3: funds
	_funds.setPosition(
		sf::Vector2f(origin.x + _co.getSize().x + 10.0f, origin.y)
	);
	// step 4: power meter
}

void awe::army_pane::_animateRight(const sf::RenderTarget& target,
	const double scaling) noexcept {
	// _rounded_bg_texture.create() relies on size!
	sf::Vector2f size = sf::Vector2f(200.0f, 50.0f);
	sf::Vector2f origin = sf::Vector2f(size.x + size.y / 2.0f, 0.0f);
	_position.translate(
		sf::Vector2f(target.getSize().x / (float)scaling - origin.x, 0.0f)
	);
	// step 1: pane background
	_bg.setPosition(sf::Vector2f(origin.x - size.x, origin.y));
	_bg.setSize(size);
	_bg_border.setPosition(sf::Vector2f(origin.x - size.x - _outlineThickness,
		origin.y + size.y));
	_bg_border.setSize(
		sf::Vector2f(size.x + _outlineThickness, _outlineThickness)
	);
	_rounded_bg.setPosition(
		sf::Vector2f(0.0f + _outlineThickness, 0.0f)
	);
	_rounded_bg.setRadius(size.y / 2.0f);
	_rounded_bg_texture.clear(sf::Color(0, 0, 0, 0));
	_rounded_bg_texture.draw(_rounded_bg, sf::BlendNone);
	_rounded_bg_texture.display();
	_rounded_bg_sprite.setPosition(
		sf::Vector2f(origin.x - size.x - size.y / 2.0f - _outlineThickness * 3.0f,
			origin.y)
	);
	_rounded_bg_sprite.setTexture(_rounded_bg_texture.getTexture());
	_outlineCover.setPosition(
		sf::Vector2f(origin.x - size.x - _outlineThickness, origin.y)
	);
	_outlineCover.setSize(sf::Vector2f(_outlineThickness, size.y));
	// step 2: CO face
	_co.setPosition(
		sf::Vector2f(origin.x - _co.getSize().x - 5.0f, origin.y + 5.0f)
	);
	// step 3: funds
	_funds.setPosition(sf::Vector2f(origin.x - size.x + 5.0f, origin.y));
	// step 4: power meter
}

void awe::army_pane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	// combine translation with given states
	states.transform.combine(_position);
	// draw
	target.draw(_rounded_bg_sprite, states);
	target.draw(_bg_border, states);
	target.draw(_bg, states);
	target.draw(_outlineCover, states);
	target.draw(_co, states);
	target.draw(_funds, states);
}