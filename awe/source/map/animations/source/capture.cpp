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

#include "../include/capture.hpp"

awe::capture::capture(const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::string& background, const std::string& oldProperty,
	const std::string& newProperty, const std::string& capturing,
	const std::string& captured, const std::string capturedText,
	const std::shared_ptr<engine::language_dictionary>& dict,
	const awe::HP oldHP, const awe::HP newHP, const unsigned int maxHP,
	const awe::animated_tile& tileSprite,
	const std::shared_ptr<sf::Font>& hpTextFont,
	const std::shared_ptr<sf::Font>& capturedTextFont) :
	_background(sheet, background),
	_property(sheet, oldProperty),
	_unit(sheet, capturing),
	_hpText("", *hpTextFont, 16),
	// Should be no need to translate mid-animation, but it would be trivial to
	// achieve.
	_capturedText((*dict)(capturedText), *capturedTextFont, 22),
	_hp(static_cast<float>(oldHP)), _oldHP(_hp),
	_newHP(std::max(0.0f, static_cast<float>(newHP))),
	_maxHP(static_cast<float>(maxHP)), _capturedProperty(newProperty),
	_capturedUnit(captured) {
	// Set position of background sprite now.
	const auto tilePos = tileSprite.getPixelPosition(),
		tileSize = tileSprite.getPixelSize();
	const auto centre = sf::Vector2f(tilePos.x + tileSize.x * 0.5f,
		tilePos.y + tileSize.y * 0.5f);
	_background.setPosition(centre);
	// Setup the HP text.
	_hpText.setFillColor(sf::Color::White);
	_hpText.setOutlineColor(sf::Color::Black);
	_hpText.setOutlineThickness(3.0f);
	// Setup the Captured text.
	_capturedText.setScale(1.0f, 0.0f);
	_capturedText.setFillColor(sf::Color::White);
	_capturedText.setOutlineColor(sf::Color::Black);
	_capturedText.setOutlineThickness(3.0f);
}

bool awe::capture::animate(const sf::RenderTarget& target) {
	const auto delta = accumulatedDelta();
	_background.animate(target);
	_property.animate(target);
	if (_state == State::Captured) {
		_unit.animate(target);
	} else if (_state == State::Intro && _unit.animate(target)) {
		setState(State::Wait);
		_unit.setCurrentFrame(0);
	}

	if (_state == State::Wait) {
		if (delta >= WAIT_DURATION) setState(State::Falling);
		return false;
	}
	if (_state == State::Falling) {
		_hp = _oldHP - (_oldHP - _newHP) * (delta / FALLING_DURATION);
		if (_hp <= _newHP) {
			_hp = _newHP;
			setState(State::Wait2);
		}
	} else if (_state == State::Wait2) {
		if (delta >= WAIT2_DURATION) {
			if (_newHP > 0.0f) return true;
			setState(State::Captured);
			_property.setSprite(_capturedProperty);
			_unit.setSprite(_capturedUnit);
			return false;
		} else return false;
	} else if (_state == State::Captured) {
		_capturedText.setScale(1.0f, delta / CAPTURED_DURATION);
		_hp = _maxHP * (delta / CAPTURED_DURATION);
		if (_hp >= _maxHP) {
			_hp = _maxHP;
			setState(State::Wait3);
		}
	} else if (_state == State::Wait3) {
		return delta >= WAIT3_DURATION;
	}

	// Background. Place it centrally over the tile sprite.
	const auto bgSize = _background.getSize();
	_background.setOrigin(sf::Vector2f(bgSize.x * 0.5f, bgSize.y * 0.5f));

	// Property sprite.
	const auto propertySize = _property.getUnscaledSize();
	_property.setOrigin(sf::Vector2f(propertySize.x * 0.5f, propertySize.y));
	_property.setPosition(sf::Vector2f(_background.getPosition().x,
		_background.getPosition().y + bgSize.y * 0.5f));
	_property.setScale(sf::Vector2f(1.0f, _hp / _maxHP));

	// Unit sprite. We have to "adjust" any offset of the unit sprites as it may
	// look strange if the tile's HP is at 0 (unit may poke out of the BG sprite).
	const auto unitYOffset = _unit.getOffset().y * (1 - (_hp / _maxHP));
	const auto unitSize = _unit.getSize();
	_unit.setOrigin(sf::Vector2f(unitSize.x * 0.5f, unitSize.y));
	_unit.setPosition(sf::Vector2f(_property.getPosition().x,
		_property.getPosition().y - _property.getSize().y - unitYOffset));

	// HP text.
	_hpText.setString(std::to_string(static_cast<awe::HP>(_hp)));
	_hpText.setOrigin(sf::Vector2f(_hpText.getLocalBounds().width * 0.5f,
		_hpText.getLocalBounds().height * 0.5f));
	_hpText.setPosition(_property.getPosition());

	// Captured text.
	_capturedText.setOrigin(sf::Vector2f(
		_capturedText.getLocalBounds().width * 0.5f,
		_capturedText.getLocalBounds().height * 0.5f
	));
	_capturedText.setPosition(_background.getPosition());

	return false;
}

void awe::capture::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_background, states);
	target.draw(_property, states);
	target.draw(_unit, states);
	target.draw(_hpText, states);
	target.draw(_capturedText, states);
}

void awe::capture::setState(const State newState) {
	_state = newState;
	resetDeltaAccumulation();
}
