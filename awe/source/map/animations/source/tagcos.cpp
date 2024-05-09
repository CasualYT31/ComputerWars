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

#include "../include/tagcos.hpp"

awe::tag_cos::tag_cos(const engine::CScriptWrapper<awe::country_view>& country,
	const engine::CScriptWrapper<awe::commander_view>& oldCurrentCO,
	const engine::CScriptWrapper<awe::commander_view>& newCurrentCO,
	const std::shared_ptr<sfx::animated_spritesheet>& coSheet,
	const std::shared_ptr<engine::language_dictionary>& translate,
	const std::shared_ptr<sf::Font>& font) :
	// There isn't a need to re-translate the text mid-animation but it would
	// be super simple to do in animate().
	_text((*translate)("tagco"), *font, 114),
	_oldCurrentCO(coSheet, oldCurrentCO->portrait()),
	_newCurrentCO(coSheet, newCurrentCO->portrait()),
	_colour(country->colour()) {
	// Setup text.
	_text.setOutlineColor(sf::Color::White);
	_text.setOutlineThickness(5.0f);
	_text.setStyle(sf::Text::Bold);
	// Setup old current CO.
	_oldCurrentCO.setSpritesheet(coSheet);
}

bool awe::tag_cos::animate(const sf::RenderTarget& target) {
	const auto firstTime = firstTimeAnimated();
	float rawDelta = 0.0f;
	const auto delta = accumulatedDelta(rawDelta);
	const auto targetSize = sf::Vector2f(target.getSize());

	// Animate sprites.
	_oldCurrentCO.animate(target);
	_newCurrentCO.animate(target);

	// Keep the text central.
	_updateDrawablePosition(_text, targetSize / 2.0f,
		_text.getLocalBounds().width / 2.0f, _text.getLocalBounds().height / 1.5f);

	// Keep the Y coordinate of each CO central.
	const auto y = targetSize.y / 2.0f;

	// Calculate the X coordinate of each CO based on the accumulated delta and the
	// speed at which the CO portraits should be traversing.
	// A cleaner implementation would also gradually increase and decrease the
	// speed instead of switching directly from one to the other.
	float oldCOX = _oldCurrentCO.getPosition().x,
		newCOX = _newCurrentCO.getPosition().x;
	if (firstTime) {
		oldCOX = 0.0f - _oldCurrentCO.getSize().x;
		newCOX = targetSize.x + _newCurrentCO.getSize().x;
	} else {
		const auto FAST =
			delta < FAST_DURATION || delta > DURATION - FAST_DURATION;
		const float SPEED = FAST ? FASTEST : SLOWEST;
		const float DISTANCE = targetSize.x * SPEED * rawDelta;
		const float TIME_LIMIT =
			FAST ? FAST_DURATION : DURATION - FAST_DURATION * 2.0f;
		oldCOX += DISTANCE / TIME_LIMIT;
		newCOX -= DISTANCE / TIME_LIMIT;
		oldCOX *= targetSize.x / _targetWidthCache;
		newCOX *= targetSize.x / _targetWidthCache;
	}
	_targetWidthCache = targetSize.x;

	// Reapplying CO portrait origins and positions.
	_updateDrawablePosition(_oldCurrentCO, sf::Vector2f(oldCOX, y),
		_oldCurrentCO.getSize().x / 2.0f, _oldCurrentCO.getSize().y / 2.0f);
	_updateDrawablePosition(_newCurrentCO, sf::Vector2f(newCOX, y),
		_newCurrentCO.getSize().x / 2.0f, _newCurrentCO.getSize().y / 2.0f);

	// Calculate the alpha based on the accumulated delta.
	if (delta > FADE_DURATION && delta < DURATION - FADE_DURATION)
		_alpha = 255.0f;
	else if (delta < FADE_DURATION)
		_alpha = 255.0f * (delta / FADE_DURATION);
	else if (delta < DURATION)
		_alpha = 255.0f * ((DURATION - delta) / FADE_DURATION);
	else
		_alpha = 0.0f;

	// Apply alpha.
	const auto a = static_cast<sf::Uint8>(_alpha);
	_text.setOutlineColor(sf::Color(255, 255, 255, a));
	_text.setFillColor(sf::Color(_colour.r, _colour.g, _colour.b, a));

	return delta >= DURATION;
}

void awe::tag_cos::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	const sf::View oldView = target.getView();
	target.setView(sf::View(sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(target.getSize().x),
		static_cast<float>(target.getSize().y))));
	target.draw(_oldCurrentCO, states);
	target.draw(_newCurrentCO, states);
	target.draw(_text, states);
	target.setView(oldView);
}
