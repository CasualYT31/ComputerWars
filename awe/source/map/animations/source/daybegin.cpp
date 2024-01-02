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

#include "../include/daybegin.hpp"

awe::day_begin::day_begin(const std::shared_ptr<const awe::country>& country,
	const awe::Day day,
	const std::shared_ptr<engine::language_dictionary>& translate,
	const std::shared_ptr<sf::Font>& font) :
	// There isn't a need to re-translate the text mid-animation but it would
	// be super simple to do in animate().
	_text((*translate)("day", day), *font, 128),
	_colour(country->getColour()) {
	_text.setOutlineColor(sf::Color::Black);
	_text.setOutlineThickness(5.0f);
	_text.setStyle(sf::Text::Bold);
}

bool awe::day_begin::animate(const sf::RenderTarget& target) {
	const auto delta = accumulatedDelta();

	// Keep the text central.
	_text.setPosition(sf::Vector2f(target.getSize()) / 2.0f);
	_text.setOrigin(_text.getLocalBounds().width / 2.0f,
		_text.getLocalBounds().height / 1.5f);

	// State machine/alpha calculation.
	switch (_state) {
	case State::FadeIn:
		_alpha = _colour.a * (delta / FADE_DURATION);
		if (delta >= FADE_DURATION) {
			_alpha = _colour.a;
			_state = State::Display;
			resetDeltaAccumulation();
		}
		break;
	case State::Display:
		_alpha = _colour.a;
		if (delta >= FADE_DURATION) {
			_state = State::FadeOut;
			resetDeltaAccumulation();
		}
		break;
	case State::FadeOut:
		_alpha = _colour.a * (1 - (delta / FADE_DURATION));
		if (delta >= FADE_DURATION) {
			_alpha = 0.0f;
			finish();
		}
		break;
	}

	// Apply alpha.
	const auto a = static_cast<sf::Uint8>(_alpha);
	_text.setOutlineColor(sf::Color(0, 0, 0, a));
	_text.setFillColor(sf::Color(_colour.r, _colour.g, _colour.b, a));

	return isFinished();
}

void awe::day_begin::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	const sf::View oldView = target.getView();
	target.setView(sf::View(sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(target.getSize().x),
		static_cast<float>(target.getSize().y))));
	target.draw(_text, states);
	target.setView(oldView);
}
