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

// CONSTRUCTION ZONE

bool awe::dialogue_sequence::dialogue::animate(const sf::RenderTarget& target) noexcept {
	float delta = calculateDelta();
	// _sprite.animate(target);

	// state machine
	if (_state == awe::dialogue_state::TransitioningIn) {
		if (_skipTransitioningIn) {
			_state = awe::dialogue_state::Typing;
		} else {

		}
	} else if (_state == awe::dialogue_state::TransitioningOut) {
		if (_skipTransitioningOut) {
			_state = awe::dialogue_state::Closed;
		} else {

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

void awe::dialogue::_offsetPosition() noexcept {
	if (_status == dialogue_status::TransitioningIn) {
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
}