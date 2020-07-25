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

#include "dialogue.h"
#include <iostream>

const float awe::dialogue_box::_namePadding = 5.0f;
const float awe::dialogue_box::_mainPadding = 50.0f;
const float awe::dialogue_box::_indicatorSize = 5.0f;

awe::dialogue_box::dialogue_box() noexcept {
	setOutlineThickness(5.0f);
	_indicator.setPointCount(3);
	_indicator.setPoint(0, sf::Vector2f(0.0f, 0.0f));
	_indicator.setPoint(1, sf::Vector2f(_indicatorSize, _indicatorSize / 2.0f));
	_indicator.setPoint(2, sf::Vector2f(0.0f, _indicatorSize));
	_indicator.setFillColor(sf::Color::Red);
}

void awe::dialogue_box::setTransitionSpeed(const float pixelsPerSecond) noexcept {
	_transitionSpeed = pixelsPerSecond;
}

void awe::dialogue_box::setTypingDelay(const float seconds) noexcept {
	_typingDelay = seconds;
}

void awe::dialogue_box::setPosition(const awe::dialogue_box_position position) noexcept {
	_position = position;
}

void awe::dialogue_box::setBackgroundColour(const sf::Color& colour) noexcept {
	_background.setFillColor(colour);
	_nameBackground.setFillColor(colour);
}

void awe::dialogue_box::setThemeColour(const sf::Color& colour) noexcept {
	_background.setOutlineColor(colour);
	_nameBackground.setOutlineColor(colour);
}

void awe::dialogue_box::setOutlineThickness(const float thickness) noexcept {
	_background.setOutlineThickness(thickness);
	_nameBackground.setOutlineThickness(thickness);
}

void awe::dialogue_box::setMainText(const std::string& text) noexcept {
	_fullText = text;
}

void awe::dialogue_box::setNameText(const std::string& text) noexcept {
	_nameText.setString(text);
}

void awe::dialogue_box::setFont(std::shared_ptr<sf::Font> font) noexcept {
	_mainText.setFont(*font);
	_option1Text.setFont(*font);
	_option2Text.setFont(*font);
	_option3Text.setFont(*font);
	_nameText.setFont(*font);
}

void awe::dialogue_box::setOptions(std::string option1, std::string option2, std::string option3) noexcept {
	if (option1 == "" && option2 != "") {
		option1 = option2;
		option2 = "";
	} else if (option1 == "" && option3 != "") {
		option1 = option3;
		option3 = "";
	}
	if (option2 == "" && option3 != "") {
		option2 = option3;
		option3 = "";
	}
	_option1Text.setString(option1);
	_option2Text.setString(option2);
	_option3Text.setString(option3);
}

void awe::dialogue_box::setSizeRatio(const float ratio) noexcept {
	_sizeRatio = ratio;
}

void awe::dialogue_box::setSprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite) noexcept {
	_sheet = sheet;
	_spriteID = sprite;
	_spriteInfoChanged = true;
}

void awe::dialogue_box::skipTransitioningIn(const bool skip) noexcept {
	_skipTransitioningIn = skip;
}

void awe::dialogue_box::skipTransitioningOut(const bool skip) noexcept {
	_skipTransitioningOut = skip;
}

void awe::dialogue_box::selectNextOption() noexcept {
	if (++_currentOption == 4) _currentOption = 1;
}

void awe::dialogue_box::selectPreviousOption() noexcept {
	if (--_currentOption == 0) _currentOption = 3;
}

void awe::dialogue_box::selectCurrentOption() noexcept {
	if (_currentOption == 1) {
		_state = awe::dialogue_box_state::Option1;
	} else if (_currentOption == 2) {
		_state = awe::dialogue_box_state::Option2;
	} else {
		_state = awe::dialogue_box_state::Option3;
	}
}

void awe::dialogue_box::flip(const bool isFlipped) noexcept {
	_flipped = isFlipped;
}

bool awe::dialogue_box::thereAreOptions() const noexcept {
	return _option1Text.getString() != "";
}

bool awe::dialogue_box::thereIsAName() const noexcept {
	return _nameText.getString() != "";
}

bool awe::dialogue_box::animate(const sf::RenderTarget& target) noexcept {
	_stateMachine();

	sf::Vector2f size = _calculateBackgroundSize(target),
		position = _calculateOrigin(size, target);
	_background.setSize(size);
	_background.setPosition(position);

	sf::Vector2f nameSize = _calculateNameSize(),
		namePosition = _calculateNameOrigin(position, size, nameSize);
	_nameBackground.setSize(nameSize);
	_nameBackground.setPosition(namePosition);

	if (_state == awe::dialogue_box_state::Typing || _spriteInfoChanged) {
		if (_spriteInfoChanged) {
			_characterSprite.setSpritesheet(_sheet);
			_characterSprite.setSprite(_spriteID);
			_spriteInfoChanged = false;
			// call animate once immediately so that even if the sprite isn't animated,
			// it can still be setup
		}
		_characterSprite.animate(target);
	}
	_calculateSpriteOrigin(position, size);

	// weird position offsets...
	// could be something to do with the font, or a property of the Text object...
	_nameText.setPosition(namePosition + sf::Vector2f(_namePadding, 0.0f));

	if (_flipped) {
		_mainText.setPosition(sf::Vector2f(size.x - _mainText.getLocalBounds().width - _mainPadding, _mainPadding));
	} else {
		_mainText.setPosition(sf::Vector2f(_mainPadding, _mainPadding));
	}

	_option1Text.setPosition(_mainText.getPosition().x + _indicatorSize * 1.5f, size.y - _option1Text.getLocalBounds().height - _mainPadding);

	_option2Text.setPosition(_option1Text.getPosition().x + _option1Text.getLocalBounds().width + _indicatorSize * 2.5f, _option1Text.getPosition().y);

	_option3Text.setPosition(_option2Text.getPosition().x + _option2Text.getLocalBounds().width + _indicatorSize * 2.5f, _option2Text.getPosition().y);

	if (_currentOption == 1) {
		_indicator.setPosition(_option1Text.getPosition() - sf::Vector2f(_indicatorSize * 1.5f, 0.0f));
	} else if (_currentOption == 2) {
		_indicator.setPosition(_option2Text.getPosition() - sf::Vector2f(_indicatorSize * 1.5f, 0.0f));
	} else if (_currentOption == 3) {
		_indicator.setPosition(_option3Text.getPosition() - sf::Vector2f(_indicatorSize * 1.5f, 0.0f));
	}

	return _state == awe::dialogue_box_state::Closed;
}

sf::Vector2f awe::dialogue_box::_calculateBackgroundSize(const sf::RenderTarget& target) const noexcept {
	return sf::Vector2f((float)target.getSize().x, (float)target.getSize().y * _sizeRatio);
}

sf::Vector2f awe::dialogue_box::_calculateOrigin(const sf::Vector2f& size, const sf::RenderTarget& target) const noexcept {
	if (_position == awe::dialogue_box_position::Top) {
		return sf::Vector2f(0.0f, (0.0f - size.y - _background.getOutlineThickness()) + (size.y + _background.getOutlineThickness() * 2.0f) * _positionRatio);
	} else if (_position == awe::dialogue_box_position::Bottom) {
		return sf::Vector2f(0.0f, ((float)target.getSize().y + _background.getOutlineThickness()) - ((size.y + _background.getOutlineThickness() * 2.0f) * _positionRatio));
	} else if (_position == awe::dialogue_box_position::Middle) {
		return sf::Vector2f(0.0f, ((float)target.getSize().y / 2.0f) - ((size.y + _background.getOutlineThickness() * 2.0f) / 2.0f) * _positionRatio);
	}
	return sf::Vector2f(0.0f, 0.0f);
}

sf::Vector2f awe::dialogue_box::_calculateNameSize() const noexcept {
	if (thereIsAName()) {
		return sf::Vector2f(_nameText.getLocalBounds().width + _namePadding * 2.0f, _nameText.getLocalBounds().height + _namePadding * 2.0f);
	} else {
		return sf::Vector2f(0.0f, 0.0f);
	}
}

sf::Vector2f awe::dialogue_box::_calculateNameOrigin(sf::Vector2f origin, const sf::Vector2f& bgSize, const sf::Vector2f& nameSize) const noexcept {
	origin.x += _nameBackground.getOutlineThickness();
	origin.y -= _nameBackground.getOutlineThickness();
	if (_flipped) origin.x += bgSize.x - nameSize.x;
	if (_position == awe::dialogue_box_position::Top) {
		origin.y += bgSize.y;
	} else {
		origin.y -= nameSize.y;
	}
	return origin;
}

void awe::dialogue_box::_calculateSpriteOrigin(const sf::Vector2f& bgOrigin, const sf::Vector2f& bgSize) noexcept {
	sf::Vector2f spriteTranslation(bgOrigin.x + 50.0f, bgOrigin.y + 50.0f);
	if (_flipped) spriteTranslation.x += bgSize.x - _characterSprite.getSize().x - 50.0f;
	_characterSprite.setPosition(spriteTranslation);
}

float awe::dialogue_box::_calculatePositionRatioOffset(const float secondsElapsed) const noexcept {
	return secondsElapsed / (_background.getSize().y / _transitionSpeed);
}

void awe::dialogue_box::_stateMachine() noexcept {
	float delta = calculateDelta();
	if (_state == awe::dialogue_box_state::Closed) {
		_mainText.setString("");
		if (_skipTransitioningIn) {
			_positionRatio = 1.0f;
			_state = awe::dialogue_box_state::Typing;
		} else {
			_state = awe::dialogue_box_state::TransitioningIn;
		}
	} else if (_state == awe::dialogue_box_state::TransitioningIn) {
		// must be else-if so that Closed doesn't immediately switch to TransitioningIn.
		// this allows animate() to set the initial size of the dialogue box so that
		// _calculatePositionRatioOffset() doesn't consider the dialogue box to be 0,
		// thus skipping this transition when first drawing
		// this also fixes any potential delta-related problems:
		// i.e. dialogue_box created, then animated seconds later resulting in a "skipped" first transition in
		_positionRatio += _calculatePositionRatioOffset(delta);
		if (_positionRatio >= 1.0f) {
			_state = awe::dialogue_box_state::Typing;
			_positionRatio = 1.0f;
		}
	}
	if (_state == awe::dialogue_box_state::Typing) {
		if (delta >= _typingDelay) {
			if (_mainText.getString().getSize() < _fullText.size()) {
				_mainText.setString(_mainText.getString() + _fullText.substr(_mainText.getString().getSize(), (std::size_t)(delta / _typingDelay)));
			}
		}
		if (_mainText.getString() == _fullText) {
			_state = awe::dialogue_box_state::StoppedTyping;
		}
	}
	// see selectCurrentOption()
	if (_state == awe::dialogue_box_state::Option1 || _state == awe::dialogue_box_state::Option2 || _state == awe::dialogue_box_state::Option3) {
		if (_skipTransitioningOut) {
			_positionRatio = 0.0f;
			_state = awe::dialogue_box_state::Closed;
		} else {
			_state = awe::dialogue_box_state::TransitioningOut;
		}
	}
	if (_state == awe::dialogue_box_state::TransitioningOut) {
		_positionRatio -= _calculatePositionRatioOffset(delta);
		if (_positionRatio <= 0.0f) {
			_state = awe::dialogue_box_state::Closed;
			_positionRatio = 0.0f;
		}
	}
}

void awe::dialogue_box::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_background, states);
	if (thereIsAName()) {
		target.draw(_nameBackground, states);
		target.draw(_nameText, states);
	}
	target.draw(_characterSprite, states);
	target.draw(_mainText, states);
	if (thereAreOptions()) {
		target.draw(_option1Text, states);
		if (_option2Text.getString() != "") target.draw(_option2Text, states);
		if (_option3Text.getString() != "") target.draw(_option3Text, states);
		target.draw(_indicator, states);
	}
}

/*
// CONSTRUCTION ZONE

bool awe::dialogue_sequence::dialogue::animate(const sf::RenderTarget& target) noexcept {
	float delta = calculateDelta();
	// _sprite.animate(target);

	// state machine
	if (_state == awe::dialogue_state::TransitioningIn) {
		_transitionIn(_skipTransitioningIn);
	}
	
	if (_state == awe::dialogue_state::TransitioningOut) {
		_transitionOut(_skipTransitioningOut);
	}
}

void awe::dialogue_sequence::dialogue::_transitionIn(bool shouldEnd) noexcept {
	if (shouldEnd) {
		_state = awe::dialogue_state::Typing;
	} else {

	}
}

void awe::dialogue_sequence::dialogue::_transitionOut(bool shouldEnd) noexcept {
	if (shouldEnd) {
		_state = awe::dialogue_state::Closed;
	} else {

	}
}

void awe::dialogue_sequence::dialogue::_offsetPosition() noexcept {
	if (_state == awe::dialogue_state::TransitioningIn) {
		switch (_data.location) {
		case (dialogue_location::Bottom):
			_position.y -= _data.transitionSpeed * _delta.get();
			break;
		case (dialogue_location::Top):
			_position.y += _data.transitionSpeed * _delta.get();
			break;
		case (dialogue_location::Middle):
			_position.y -= (_data.transitionSpeed / 2.0f) * _delta.get();
			break;
		}
	} else if (_status == dialogue_status::TransitioningOut) {
		switch (_data.location) {
		case (dialogue_location::Bottom):
			_position.y += _data.transitionSpeed * _delta.get();
			break;
		case (dialogue_location::Top):
			_position.y -= _data.transitionSpeed * _delta.get();
			break;
		case (dialogue_location::Middle):
			_position.y += (_data.transitionSpeed / 2.0f) * _delta.get();
			break;
		}
	}
}

void awe::dialogue_sequence::dialogue::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_bg, states);
	target.draw(_nameBg, states);
	target.draw(_sprite, states);
	target.draw(_nameText, states);
	target.draw(_text, states);
	target.draw(_indicator, states);
}

bool awe::dialogue_sequence::animate(const sf::RenderTarget& target) noexcept {
	_dialogue->animate(target);
}

void awe::dialogue_sequence::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*_dialogue);
}

// END OF CONSTRUCTION ZONE

awe::dialogue_sequence::dialogue_sequence(sfx::renderer* r, sfx::fonts* f,
	sfx::user_input* u, sfx::spritesheet* s, i18n::language_dictionary* l, sfx::audio* a, const std::string& name) noexcept : _logger(name) {
	_renderer = r;
	_font = f;
	_userinput = u;
	_spritesheet = s;
	_language = l;
	_audio = a;
	if (!_renderer) _logger.error("No renderer object given (null pointer).");
	if (!_font) _logger.error("No fonts object given (null pointer): text behaviour is undefined (likely will not display).");
}

awe::dialogue_sequence::~dialogue_sequence() noexcept {
	//remember to free up the dynamically allocated object!
	if (_diag) delete _diag;
}

unsigned int awe::dialogue_sequence::choice() const noexcept {
	return _lastChoice;
}

awe::dialogue_status awe::dialogue_sequence::status() const noexcept {
	return _lastStatus;
}

bool awe::dialogue_sequence::thereAreOptions() const noexcept {
	if (!_diag) return false;
	return _diag->thereAreOptions();
}

std::string awe::dialogue_sequence::current() const noexcept {
	return _currentObject->first;
}

bool awe::dialogue_sequence::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		std::string fontKey = "dialogue";
		awe::dialogue_data& d = _diagdata[i.key()];
		d.audioObject = _audio;
		d.rendererObject = _renderer;
		d.languageObject = _language;
		d.spritesheetObject = _spritesheet;
		d.userinputObject = _userinput;
		int transdelay = d.transitionOutDelay.asMilliseconds();
		int typingdelay = d.typingDelay.asMilliseconds();
		unsigned int location = static_cast<unsigned int>(d.location);
		j.apply(fontKey, { i.key(), "font" }, &fontKey, true);
		j.apply(d.flip, { i.key(), "flip" }, &d.flip, true);
		j.apply(d.fontSize, { i.key(), "fontsize" }, &d.fontSize, true);
		j.apply(d.height, { i.key(), "height" }, &d.height, true);
		j.apply(d.leftControlKey, { i.key(), "left" }, &d.leftControlKey, true);
		j.apply(d.lineSpacingFactor, { i.key(), "linespacing" }, &d.lineSpacingFactor, true);
		j.apply(location, { i.key(), "location" }, &location, true);
		if (location > static_cast<unsigned int>(dialogue_location::Middle)) location = static_cast<unsigned int>(dialogue_location::Middle);
		d.location = static_cast<dialogue_location>(location);
		j.apply(d.nativeString, { i.key(), "string" }, &d.nativeString, true);
		j.apply(d.rightControlKey, { i.key(), "right" }, &d.rightControlKey, true);
		j.apply(d.selectControlKey, { i.key(), "select" }, &d.selectControlKey, true);
		j.apply(d.selectionSoundKey, { i.key(), "selectsound" }, &d.selectionSoundKey, true);
		j.apply(d.skipable, { i.key(), "skipable" }, &d.skipable, true);
		j.apply(d.skipTransitioningIn, { i.key(), "skipin" }, &d.skipTransitioningIn, true);
		j.apply(d.skipTransitioningOut, { i.key(), "skipout" }, &d.skipTransitioningOut, true);
		j.apply(d.spriteKey, { i.key(), "sprite" }, &d.spriteKey, true);
		j.apply(transdelay, { i.key(), "outdelay" }, &transdelay, true);
		d.transitionOutDelay = sf::milliseconds(transdelay);
		j.apply(d.transitionSpeed, { i.key(), "speed" }, &d.transitionSpeed, true);
		j.apply(typingdelay, { i.key(), "typingdelay" }, &typingdelay, true);
		d.typingDelay = sf::milliseconds(typingdelay);
		j.apply(d.typingSoundKey, { i.key(), "typingsound" }, &d.typingSoundKey, true);
		j.applyColour(d.backgroundColour, { i.key(), "bgcolour" }, &d.backgroundColour, true);
		j.applyColour(d.borderColour, { i.key(), "bordercolour" }, &d.borderColour, true);
		j.applyArray(d.option, { i.key(), "options" });
		if (_font) d.font = &(*_font)[fontKey];
	}
	_currentObject = _diagdata.begin();
	return true;
}

bool awe::dialogue_sequence::_save(nlohmann::json& j) noexcept {
	return false;
}

awe::dialogue::dialogue(const awe::dialogue_data& data, const std::string& name) noexcept : _logger(name), _data(data) {
	//some error checking
	if (!_data.audioObject) {
		_logger.write("Warning: no audio object has been provided for this dialogue object, no sounds will play.");
	}

	if (!_data.languageObject) {
		_logger.write("Warning: no language object has been provided for this dialogue object, no strings will be translated.");
	}

	if (!_data.spritesheetObject) {
		_logger.write("Warning: no spritesheet object has been provided for this dialogue object, no accompanying sprite will be displayed.");
	}

	if (!_data.userinputObject && thereAreOptions()) {
		_logger.error("No user input object has been provided for this option-based dialogue object, user will not be able to select an option!");
	}

	_bg.setOutlineThickness(4.0f);

	if (!_data.rendererObject) {
		_logger.error("No renderer object has been provided for this dialogue object.");
	} else {
		_bg.setSize(sf::Vector2f(_data.rendererObject->getSize().x + _bg.getOutlineThickness(), _data.height));
		_bg.setPosition(0, _bg.getOutlineThickness());
		_canvas.create((unsigned int)_bg.getSize().x, (unsigned int)(_bg.getSize().y + _bg.getOutlineThickness() * 2.0f));
	}

	if (_data.flip) {
		_text.setPosition(25.0f, 25.0f);
		_sprite.setPosition(_bg.getSize().x - 140.0f, 25.0f);
	} else {
		_text.setPosition(175.0f, 25.0f);
		_sprite.setPosition(25.0f, 25.0f);
	}

	_bg.setFillColor(_data.backgroundColour);
	_bg.setOutlineColor(_data.borderColour);

	if (_data.font) {
		_text.setFont(*_data.font);
	} else {
		_logger.error("No font object has been provided for this dialogue object.");
	}

	_text.setLineSpacing(_data.lineSpacingFactor);
	_text.setCharacterSize(_data.fontSize);
	_setInitialPosition();
	_indicator.setFillColor(sf::Color::Cyan);
	_indicator.setSize(sf::Vector2f(10.0f, 10.0f));
}

unsigned int awe::dialogue::choice() const noexcept {
	return _selection - 1;
}

bool awe::dialogue::thereAreOptions() const noexcept {
	if (_data.option[0] != "" || _data.option[1] != "" || _data.option[2] != "") {
		return true;
	} else {
		return false;
	}
}

void awe::dialogue::_render() noexcept {
	if (thereAreOptions()) {
		std::size_t* pos = nullptr;
		switch (_selection) {
		case 1:
			pos = &_o1pos;
			break;
		case 2:
			pos = &_o2pos;
			break;
		case 3:
			pos = &_o3pos;
			break;
		}
		if (pos) {
			_indicator.setPosition(
				_text.findCharacterPos(*pos).x - _indicator.getSize().x - 5,
				_text.findCharacterPos(*pos).y
			);
		}
	}

	if (_data.spritesheetObject) {
		if (_status == dialogue_status::Typing) {
			_spriteTexture = (*_data.spritesheetObject)[_data.spriteKey];
		} else {
			_spriteTexture = _data.spritesheetObject->getFrame(0, _data.spriteKey);
		}
		_sprite.setTexture(_spriteTexture, true);
	}

	_canvas.clear();
	_canvas.draw(_bg);
	_canvas.draw(_sprite);
	_canvas.draw(_text);
	if (thereAreOptions() && _status == dialogue_status::StoppedTyping) _canvas.draw(_indicator);
	_canvas.display();

	if (_data.rendererObject) {
		if (_data.location == dialogue_location::Middle && (_status == dialogue_status::TransitioningIn || _status == dialogue_status::TransitioningOut)) {
			_tophalf.setTexture(_canvas.getTexture());
			_bottomhalf.setTexture(_canvas.getTexture());
			_tophalf.setTextureRect(sf::IntRect(
				0,
				0,
				(int)_canvas.getSize().x,
				(int)((float)_data.rendererObject->getSize().y / 2.0f - _position.y)
			));
			_bottomhalf.setTextureRect(sf::IntRect(
				0,
				(int)(_canvas.getSize().y - _tophalf.getTextureRect().height),
				(int)_canvas.getSize().x,
				_tophalf.getTextureRect().height
			));
			_data.rendererObject->drawToScale(_tophalf, (float)_position.x, (float)_position.y);
			_data.rendererObject->drawToScale(_bottomhalf, (float)_position.x, (float)(_position.y + _tophalf.getTextureRect().height));
		} else {
			_data.rendererObject->drawToScale(_canvas.getTexture(), _position.x, _position.y, false);
		}
	}
}

void awe::dialogue::_resetSelection() noexcept {
	if (_data.option[0] != "") {
		_selection = 1;
		return;
	}
	if (_data.option[1] != "") {
		_selection = 2;
		return;
	}
	if (_data.option[2] != "") _selection = 3;
}

bool awe::dialogue::_positionIsReady() noexcept {
	float finalY = 0.0f;
	if (_data.rendererObject) {
		if (_status == dialogue_status::TransitioningIn) {
			switch (_data.location) {
			case (dialogue_location::Bottom):
				finalY = (float)_data.rendererObject->getSize().y - (_bg.getSize().y + _bg.getOutlineThickness() * 2.0f);
				if (_data.skipTransitioningIn || _position.y <= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			case (dialogue_location::Top):
				if (_data.skipTransitioningIn || _position.y >= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			case (dialogue_location::Middle):
				finalY = (float)_data.rendererObject->getSize().y / 2.0f - (_bg.getSize().y + _bg.getOutlineThickness() * 2.0f) / 2.0f;
				if (_data.skipTransitioningIn || _position.y <= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			}
		} else if (_status == dialogue_status::TransitioningOut) {
			switch (_data.location) {
			case (dialogue_location::Bottom):
				finalY = (float)_data.rendererObject->getSize().y + _bg.getOutlineThickness();
				if (_data.skipTransitioningOut || _position.y >= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			case (dialogue_location::Top):
				finalY = -_bg.getSize().y - _bg.getOutlineThickness() * 2.0f;
				if (_data.skipTransitioningOut || _position.y <= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			case (dialogue_location::Middle):
				finalY = (float)_data.rendererObject->getSize().y / 2.0f;
				if (_data.skipTransitioningOut || _position.y >= finalY) {
					_position.y = finalY;
					return true;
				}
				break;
			}
		}
	}
	return false;
}

void awe::dialogue::_setInitialPosition() noexcept {
	_position.x = -_bg.getOutlineThickness();
	if (_data.rendererObject) {
		switch (_data.location) {
		case (dialogue_location::Bottom):
			_position.y = _data.rendererObject->getSize().y + _bg.getOutlineThickness();
			break;
		case (dialogue_location::Top):
			_position.y = -_bg.getSize().y - _bg.getOutlineThickness() * 2.0f;
			break;
		case (dialogue_location::Middle):
			_position.y = (float)_data.rendererObject->getSize().y / 2.0f;
			break;
		}
	}
}*/