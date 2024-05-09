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

#include "../include/nextturn.hpp"

awe::next_turn::next_turn(const engine::CScriptWrapper<awe::country_view>& country,
	const std::string& nextTurnLabel,
	const std::unordered_set<std::string>& controls,
	const std::shared_ptr<sfx::user_input>& ui,
	const std::shared_ptr<engine::language_dictionary>& dict,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet,
	const std::shared_ptr<sf::Font>& font,
	const std::function<void(void)>& code) :
	_controls(controls), _ui(ui), _code(code),
	_countryIcon(sheet, country->icon()),
	_countryName((*dict)(country->longName()), *font),
	_nextTurnLabel((*dict)(nextTurnLabel), *font) {
	_circle.setFillColor(sf::Color::Black);
	_countryName.setFillColor(sf::Color::White);
	_nextTurnLabel.setFillColor(sf::Color::White);
}

bool awe::next_turn::animate(const sf::RenderTarget& target) {
	if (firstTimeAnimated())
		_transition.create(target.getSize().x, target.getSize().y);
	const auto delta = calculateDelta();
	_countryIcon.animate(target);

	// Reposition everything.
	const sf::Vector2f targetSize(target.getSize()),
		transitionSize(_transition.getSize());
	_circle.setOrigin(_circle.getLocalBounds().width * 0.5f,
		_circle.getLocalBounds().height * 0.5f);
	_circle.setPosition(transitionSize * 0.5f);
	_countryIcon.setOrigin(_countryIcon.getUnscaledSize() * 0.5f);
	_countryIcon.setPosition(targetSize * 0.5f);
	_countryIcon.setScale({ 4.f, 4.f });
	_countryName.setOrigin(_countryName.getLocalBounds().width * 0.5f, 0.f);
	_countryName.setPosition(targetSize.x * 0.5f,
		targetSize.y * 0.5f + _countryIcon.getSize().y * 0.5f + 15.f);
	_nextTurnLabel.setOrigin(_nextTurnLabel.getLocalBounds().width * 0.5f,
		_nextTurnLabel.getLocalBounds().height);
	_nextTurnLabel.setPosition(targetSize.x * 0.5f,
		targetSize.y * 0.5f - _countryIcon.getSize().y * 0.5f - 20.f);

	// State-specific changes.
	const auto CIRCLE_SPEED = std::max(transitionSize.x, transitionSize.y) /
		TRANSITION_DURATION;
	switch (_state) {
	case state::TransitionIn:
		_circle.setRadius(_circle.getRadius() + CIRCLE_SPEED * delta);
		if (_circle.getRadius() * 2.f >=
			std::max(transitionSize.x, transitionSize.y)) {
			_state = state::WaitForInput;
		} else break;
		[[fallthrough]];
	case state::WaitForInput:
		if (std::any_of(_controls.begin(), _controls.end(),
			[ui = _ui](const std::string& control) { return (*ui)[control]; })) {
			_circle.setRadius(0.f);
			_circle.setFillColor(sf::Color::Transparent);
			_state = state::TransitionOut;
			_code();
		} else break;
		[[fallthrough]];
	case state::TransitionOut:
		_circle.setRadius(_circle.getRadius() + CIRCLE_SPEED * delta);
		if (_circle.getRadius() * 2.f >=
			std::max(transitionSize.x, transitionSize.y)) return true;
	}
	
	// Render the transition.
	_transition.clear(_state == state::TransitionIn ? sf::Color::Transparent :
		sf::Color::Black);
	_transition.draw(_circle, sf::BlendNone);
	_transition.display();

	return false;
}

void awe::next_turn::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	const sf::View oldView = target.getView();
	sf::View newView(sf::FloatRect({ 0.0f, 0.0f },
		sf::Vector2f(target.getSize())));
	target.setView(newView);
	sf::Sprite transition(_transition.getTexture());
	const auto targetSize = sf::Vector2f(target.getSize()),
		transitionSize = sf::Vector2f(_transition.getSize());
	const auto factor = targetSize.x >= targetSize.y ?
		targetSize.x / transitionSize.x : targetSize.y / transitionSize.y;
	transition.setScale(factor, factor);
	transition.setOrigin(transitionSize * 0.5f);
	transition.setPosition(targetSize * 0.5f);
	target.draw(transition, states);
	if (_state == state::WaitForInput) {
		target.draw(_countryIcon, states);
		target.draw(_countryName, states);
		target.draw(_nextTurnLabel, states);
	}
	target.setView(oldView);
}
