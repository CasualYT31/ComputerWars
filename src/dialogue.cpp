/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

// dialogue_sequence

engine::dialogue_sequence::dialogue_sequence(const std::string& name) noexcept : _logger(name) {}

void engine::dialogue_sequence::setLanguageDictionary(std::shared_ptr<i18n::language_dictionary> dict) noexcept {
	_langDic = dict;
}

void engine::dialogue_sequence::setUserInput(std::shared_ptr<sfx::user_input> ui) noexcept {
	_userInput = ui;
}

bool engine::dialogue_sequence::animate(const sf::RenderTarget& target) noexcept {
	if (!_currentBox) {
		// if no box has ever been allocated yet, attempt to allocate the first one
		_currentBox = _allocateDialogueBox(_currentBoxID = 0);
		// if it couldn't be allocated, return TRUE and ignore call
		if (!_currentBox) return true;
	}
	if (_currentBox) {
		// handle user input
		if (_userInput) {
			if ((*_userInput)[_moveRightControlKey]) {
				_currentBox->selectNextOption();
			} else if ((*_userInput)[_moveLeftControlKey]) {
				_currentBox->selectPreviousOption();
			} else if ((*_userInput)[_selectControlKey]) {
				if (_currentBox->optionCount() > 0) {
					// this also conveniently makes it so that users can't skip any part of an option dialogue
					// it allows users to become aware that they should stop spamming the "A" button since they could inadvertedly select something which they don't want to
					_currentBox->selectCurrentOption();
				} else {
					_currentBox->skipCurrentState();
				}
			} else if ((*_userInput)[_skipControlKey]) {
				// emulate a failed dialogue box allocation
				_currentBox = nullptr;
				return true;
			}
		}
		// animate the dialogue box
		if (_currentBox->animate(target)) {
			// current box has ended, allocate next one
			_currentBox = _allocateDialogueBox(++_currentBoxID);
			// if nullptr is returned, the end of the sequence has been reached (hopefully it is not an allocation error, maybe move exception checking outside of that method?), return TRUE
			if (!_currentBox) return true;
		}
	}
	return false;
}

std::unique_ptr<engine::dialogue_box> engine::dialogue_sequence::_allocateDialogueBox(const std::size_t i) noexcept {
	if (i >= _boxes.size()) return nullptr;
	std::unique_ptr<engine::dialogue_box> ret;
	try {
		ret = std::make_unique<engine::dialogue_box>();
	} catch (std::bad_alloc& e) {
		_logger.error("Fatal allocation error of dialogue box {}: {}", i, e.what());
		return nullptr;
	}
	engine::dialogue_sequence::dialogue_box_data& data = _boxes[i];
	ret->setPosition(data.position);
	ret->setSizeRatio(data.size);
	ret->flip(data.flipped);
	ret->setMainText(data.mainText);
	ret->skipTransitioningIn(data.skipTransIn);
	ret->skipTransitioningOut(data.skipTransOut);
	ret->setSprite(nullptr, data.spriteID);
	ret->setTransitionLength(data.transLength);
	ret->setTypingDelay(data.typingDelay);
	ret->setSounds(nullptr, data.typingSoundKey, data.moveSelSoundKey, data.selectSoundKey);
	ret->setThemeColour(data.themeColour);
	ret->setNameText(data.nameText);
	ret->setFont(nullptr);
	ret->setOptions(data.options[0], data.options[1], data.options[2]);
	return ret;
}

void engine::dialogue_sequence::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_currentBox) target.draw(*_currentBox, states);
}

bool engine::dialogue_sequence::_load(safe::json& j) noexcept {
	_boxes.clear();
	_currentBox = nullptr;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		// special keys
		if (i.key() == "right") {
			j.apply(_moveRightControlKey, { "right" }, &_moveRightControlKey, true);
		} else if (i.key() == "left") {
			j.apply(_moveLeftControlKey, { "left" }, &_moveLeftControlKey, true);
		} else if (i.key() == "select") {
			j.apply(_selectControlKey, { "select" }, &_selectControlKey, true);
		} else if (i.key() == "skip") {
			j.apply(_skipControlKey, { "skip" }, &_skipControlKey, true);
		} else { // other keys store dialogue boxes
			_boxes.push_back(dialogue_box_data()); // in VS this line appears as an error sometimes but it compiles fine...
			std::size_t k = _boxes.size() - 1;

			unsigned short pos = 0;
			j.apply(pos, { i.key(), "position" }, &pos, true);
			if (pos >= (unsigned short)engine::dialogue_box_position::NumberOfPositions) {
				pos = 0;
			}
			_boxes[k].position = static_cast<engine::dialogue_box_position>(pos);

			j.apply(_boxes[k].size, { i.key(), "size" }, &_boxes[k].size, true);
			j.apply(_boxes[k].flipped, { i.key(), "flipped" }, &_boxes[k].flipped, true);
			j.apply(_boxes[k].mainText, { i.key(), "text" }, &_boxes[k].mainText, true);
			j.apply(_boxes[k].skipTransIn, { i.key(), "skiptransin" }, &_boxes[k].skipTransIn, true);
			j.apply(_boxes[k].skipTransOut, { i.key(), "skiptransout" }, &_boxes[k].skipTransOut, true);
			// in order to achieve spritesheet key, we're going to need some kind of request system whereby a class requests a pointer to a spritesheet from awe::game...
			// look at the Mediator design pattern
			j.apply(_boxes[k].spriteID, { i.key(), "sprite" }, &_boxes[k].spriteID, true);
			j.apply(_boxes[k].transLength, { i.key(), "translength" }, &_boxes[k].transLength, true);
			j.apply(_boxes[k].typingDelay, { i.key(), "typingdelay" }, &_boxes[k].typingDelay, true);
			// see above, also applies to audio key
			j.apply(_boxes[k].typingSoundKey, { i.key(), "typingsound" }, &_boxes[k].typingSoundKey, true);
			j.apply(_boxes[k].moveSelSoundKey, { i.key(), "moveselsound" }, &_boxes[k].moveSelSoundKey, true);
			j.apply(_boxes[k].selectSoundKey, { i.key(), "selectsound" }, &_boxes[k].selectSoundKey, true);
			j.applyColour(_boxes[k].themeColour, { i.key(), "theme" }, &_boxes[k].themeColour, true);
			j.apply(_boxes[k].nameText, { i.key(), "name" }, &_boxes[k].nameText, true);
			// see above, also applies to font key
			j.applyArray(_boxes[k].options, { i.key(), "options" });
			j.resetState();
		}
	}
	return true;
}

bool engine::dialogue_sequence::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

// dialogue_box

const float engine::dialogue_box::_smallPadding = 10.0f;
const float engine::dialogue_box::_largePadding = 50.0f;

engine::dialogue_box::dialogue_box() noexcept {
	setOutlineThickness(5.0f);
}

void engine::dialogue_box::setSounds(std::shared_ptr<sfx::audio> audioLibrary, const std::string& typing, const std::string& moveSelection, const std::string& select) noexcept {
	_audioLibrary = audioLibrary;
	_typingKey = typing;
	_moveSelectionKey = moveSelection;
	_selectKey = select;
}

std::string engine::dialogue_box::getTypingSound() const noexcept {
	return _typingKey;
}

std::string engine::dialogue_box::getMoveSelectionSound() const noexcept {
	return _moveSelectionKey;
}

std::string engine::dialogue_box::getSelectSound() const noexcept {
	return _selectKey;
}

void engine::dialogue_box::setTransitionLength(const float seconds) noexcept {
	_transitionLength = seconds;
}

void engine::dialogue_box::setTypingDelay(const float seconds) noexcept {
	_typingDelay = seconds;
}

void engine::dialogue_box::setPosition(const engine::dialogue_box_position position) noexcept {
	_position = position;
	if (_position == engine::dialogue_box_position::NumberOfPositions) {
		_position = engine::dialogue_box_position::Bottom;
	}
}

void engine::dialogue_box::setBackgroundColour(const sf::Color& colour) noexcept {
	_background.setFillColor(colour);
	_nameBackground.setFillColor(colour);
}

void engine::dialogue_box::setThemeColour(const sf::Color& colour) noexcept {
	_background.setOutlineColor(colour);
	_nameBackground.setOutlineColor(colour);
	_indicator.setFillColor(colour);
}

void engine::dialogue_box::setOutlineThickness(const float thickness) noexcept {
	_background.setOutlineThickness(thickness);
	_nameBackground.setOutlineThickness(thickness);
}

void engine::dialogue_box::setMainText(const std::string& text) noexcept {
	_fullText = text;
}

void engine::dialogue_box::setNameText(const std::string& text) noexcept {
	_nameText.setString(text);
}

void engine::dialogue_box::setFont(std::shared_ptr<sf::Font> font) noexcept {
	if (font) {
		_mainText.setFont(*font);
		_option1Text.setFont(*font);
		_option2Text.setFont(*font);
		_option3Text.setFont(*font);
		_nameText.setFont(*font);
	}
}

void engine::dialogue_box::setOptions(const std::string& option1, const std::string& option2, const std::string& option3) noexcept {
	if (_option1Text.getString() == "" && (option1 != "" || option2 != "" || option3 != "")) {
		// select first option if there were no options before and now we are adding options
		_currentOption = 1;
	}
	_option1Text.setString(option1);
	_option2Text.setString(option2);
	_option3Text.setString(option3);
	if (_option1Text.getString() == "" && _option2Text.getString() != "") {
		_option1Text.setString(_option2Text.getString());
		_option2Text.setString("");
	} else if (_option1Text.getString() == "" && _option3Text.getString() != "") {
		_option1Text.setString(_option2Text.getString());
		_option3Text.setString("");
	}
	if (_option2Text.getString() == "" && _option3Text.getString() != "") {
		_option2Text.setString(_option3Text.getString());
		_option3Text.setString("");
	}
	if (optionCount() == 0) {
		_currentOption = 0;
	} else if (_currentOption > optionCount()) {
		_currentOption = optionCount();
	}
}

void engine::dialogue_box::setSizeRatio(const float ratio) noexcept {
	_sizeRatio = ratio;
}

void engine::dialogue_box::setSprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite) noexcept {
	_sheet = sheet;
	_spriteID = sprite;
	_spriteInfoChanged = true;
}

void engine::dialogue_box::skipTransitioningIn(const bool skip) noexcept {
	_skipTransitioningIn = skip;
}

void engine::dialogue_box::skipTransitioningOut(const bool skip) noexcept {
	_skipTransitioningOut = skip;
}

void engine::dialogue_box::selectNextOption() noexcept {
	if (optionCount() > 0 && _state == engine::dialogue_box_state::StoppedTyping) {
		if (++_currentOption == optionCount() + 1) _currentOption = 1;
		_playSound(_moveSelectionKey);
	}
}

void engine::dialogue_box::selectPreviousOption() noexcept {
	if (optionCount() > 0 && _state == engine::dialogue_box_state::StoppedTyping) {
		if (--_currentOption == 0) _currentOption = optionCount();
		_playSound(_moveSelectionKey);
	}
}

unsigned short engine::dialogue_box::selectCurrentOption() noexcept {
	if (optionCount() > 0 && _state == engine::dialogue_box_state::StoppedTyping) {
		if (_currentOption == 1) {
			_state = engine::dialogue_box_state::Option1;
		} else if (_currentOption == 2) {
			_state = engine::dialogue_box_state::Option2;
		} else {
			_state = engine::dialogue_box_state::Option3;
		}
		_playSound(_selectKey);
		return _currentOption;
	} else {
		return 0;
	}
}

void engine::dialogue_box::flip(const bool isFlipped) noexcept {
	_flipped = isFlipped;
}

unsigned short engine::dialogue_box::optionCount() const noexcept {
	unsigned short count = 0;
	if (_option1Text.getString() != "") count++;
	if (_option2Text.getString() != "") count++;
	if (_option3Text.getString() != "") count++;
	return count;
}

bool engine::dialogue_box::thereIsAName() const noexcept {
	return _nameText.getString() != "";
}

std::string engine::dialogue_box::getOption1Text() const noexcept {
	return _option1Text.getString().toAnsiString();
}

std::string engine::dialogue_box::getOption2Text() const noexcept {
	return _option2Text.getString().toAnsiString();
}

std::string engine::dialogue_box::getOption3Text() const noexcept {
	return _option3Text.getString().toAnsiString();
}

void engine::dialogue_box::skipCurrentState() noexcept {
	_skipCurrentState = true;
}

bool engine::dialogue_box::animate(const sf::RenderTarget& target) noexcept {
	// manage the state of the dialogue box first
	_stateMachine();
	// quite a few measurements are based on the bounding box of the text,
	// so update that before anything else
	_updateMainText();
	_updateCharacterSize(target);
	_resizeIndicator(_option1Text.getCharacterSize() * 0.5f);
	// practically all measurements are based on the main graphic of the dialogue box,
	// so resize and reposition that next
	_updateBackground(target);
	_updateNameBackground();
	_updateCharacterSprite(target);
	_updateTextPositions();
	_repositionIndicator();
	_drawToCanvas(target);
	// if the dialogue box has finished, return TRUE
	return _state == engine::dialogue_box_state::Closed;
}

void engine::dialogue_box::_repositionIndicator() noexcept {
	if (_currentOption == 1) {
		_indicator.setPosition(_option1Text.getPosition() + sf::Vector2f(-_indicatorSize * 1.5f, _option1Text.getLocalBounds().height / 2.0f - _indicatorSize / 2.0f));
	} else if (_currentOption == 2) {
		_indicator.setPosition(_option2Text.getPosition() + sf::Vector2f(-_indicatorSize * 1.5f, _option2Text.getLocalBounds().height / 2.0f - _indicatorSize / 2.0f));
	} else if (_currentOption == 3) {
		_indicator.setPosition(_option3Text.getPosition() + sf::Vector2f(-_indicatorSize * 1.5f, _option3Text.getLocalBounds().height / 2.0f - _indicatorSize / 2.0f));
	}
}

void engine::dialogue_box::_updateTextPositions() noexcept {
	_nameText.setPosition(_nameBackground.getPosition() + sf::Vector2f(_smallPadding, _smallPadding));
	
	if (_flipped) {
		_mainText.setPosition(_background.getPosition() + sf::Vector2f(_largePadding, _smallPadding));
	} else {
		_mainText.setPosition(_background.getPosition() + sf::Vector2f(_characterSprite.getPosition().x + _characterSprite.getSize().x + _largePadding, _smallPadding));
	}

	// I probably could've avoided having to take into account the position in some cases
	// if I instead changed the origin and not the position of objects
	_option1Text.setPosition(_mainText.getPosition().x + _indicatorSize * 1.5f, _background.getPosition().y + _background.getSize().y - _option1Text.getLocalBounds().height - _smallPadding);

	_option2Text.setPosition(_option1Text.getPosition().x + _option1Text.getLocalBounds().width + _indicatorSize * 2.5f, _option1Text.getPosition().y);

	_option3Text.setPosition(_option2Text.getPosition().x + _option2Text.getLocalBounds().width + _indicatorSize * 2.5f, _option2Text.getPosition().y);
}

void engine::dialogue_box::_updateCharacterSprite(const sf::RenderTarget& target) noexcept {
	if (_state == engine::dialogue_box_state::Typing || _spriteInfoChanged) {
		if (_spriteInfoChanged) {
			_characterSprite.setSpritesheet(_sheet);
			_characterSprite.setSprite(_spriteID);
			_spriteInfoChanged = false;
			// call animate once immediately so that even if the sprite isn't animated,
			// it can still be setup
		}
		_characterSprite.animate(target);
	}

	float y = _background.getPosition().y + (_background.getSize().y / 2.0f) - (_characterSprite.getSize().y / 2.0f);
	if (_flipped) {
		_characterSprite.setPosition(sf::Vector2f(_background.getPosition().x + _background.getSize().x - _characterSprite.getSize().x - _largePadding, y));
	} else {
		_characterSprite.setPosition(sf::Vector2f(_background.getPosition().x + _largePadding, y));
	}
}

void engine::dialogue_box::_updateNameBackground() noexcept {
	if (thereIsAName()) {
		_nameBackground.setSize(sf::Vector2f(_nameText.getLocalBounds().width + _smallPadding * 2.0f, _nameText.getLocalBounds().height + _smallPadding * 2.0f));
	} else {
		_nameBackground.setSize(sf::Vector2f(0.0f, 0.0f));
	}

	sf::Vector2f origin = _background.getPosition();
	origin.x += _nameBackground.getOutlineThickness();
	origin.y -= _nameBackground.getOutlineThickness();
	if (_flipped) origin.x += _background.getSize().x - _nameBackground.getSize().x - _nameBackground.getOutlineThickness() * 2.0f;
	if (_position == engine::dialogue_box_position::Top) {
		origin.y += _background.getSize().y + _nameBackground.getOutlineThickness() * 2.0f;
	} else {
		origin.y -= _nameBackground.getSize().y;
	}
	_nameBackground.setPosition(origin);
}

void engine::dialogue_box::_updateBackground(const sf::RenderTarget& target) noexcept {
	_background.setSize(sf::Vector2f((float)target.getSize().x, (float)target.getSize().y * _sizeRatio));
	if (_position == engine::dialogue_box_position::Top) {
		_background.setPosition(0.0f, (0.0f - _background.getSize().y - _background.getOutlineThickness()) + (_background.getSize().y + _background.getOutlineThickness() * 2.0f) * _positionRatio);
	} else if (_position == engine::dialogue_box_position::Bottom) {
		_background.setPosition(0.0f, ((float)target.getSize().y + _background.getOutlineThickness()) - ((_background.getSize().y + _background.getOutlineThickness() * 2.0f) * _positionRatio));
	} else if (_position == engine::dialogue_box_position::Middle) {
		_background.setPosition(0.0f, ((float)target.getSize().y / 2.0f) - ((_background.getSize().y + _background.getOutlineThickness() * 2.0f) / 2.0f) * _positionRatio);
	} else {
		_background.setPosition(0.0f, 0.0f);
	}
}

float engine::dialogue_box::_calculatePositionRatioOffset(const float secondsElapsed) const noexcept {
	return secondsElapsed / _transitionLength;
}

void engine::dialogue_box::_resizeIndicator(const float size) noexcept {
	_indicatorSize = size;
	_indicator.setPoint(0, sf::Vector2f(0.0f, 0.0f));
	_indicator.setPoint(1, sf::Vector2f(size, size / 2.0f));
	_indicator.setPoint(2, sf::Vector2f(0.0f, size));
}

void engine::dialogue_box::_fromClosedToTransitioning() noexcept {
	_characterPosition = 0;
	if (_skipTransitioningIn) {
		_positionRatio = 1.0f;
		_state = engine::dialogue_box_state::Typing;
		_typingTimer.restart();
	} else {
		_state = engine::dialogue_box_state::TransitioningIn;
	}
}

void engine::dialogue_box::_fromTransitioningToTyping(const float delta) noexcept {
	_positionRatio += _calculatePositionRatioOffset(delta);
	if (_skipCurrentState || _positionRatio >= 1.0f) {
		_state = engine::dialogue_box_state::Typing;
		_positionRatio = 1.0f;
		_typingTimer.restart();
		_skipCurrentState = false;
	}
}

void engine::dialogue_box::_fromTypingToStoppedTyping() noexcept {
	if (_typingTimer.getElapsedTime().asSeconds() >= _typingDelay) {
		_playSound(_typingKey);
		if (_characterPosition < _fullText.size()) {
			_characterPosition += (std::size_t)(_typingTimer.getElapsedTime().asSeconds() / _typingDelay);
		}
		_typingTimer.restart();
	}
	if (_skipCurrentState) {
		_characterPosition = _fullText.size();
		_skipCurrentState = false;
	}
	if (_characterPosition == _fullText.size()) {
		_characterSprite.setCurrentFrame(0);
		_state = engine::dialogue_box_state::StoppedTyping;
	}
}

void engine::dialogue_box::_fromOptionToTransitioning() noexcept {
	if (_skipTransitioningOut) {
		_positionRatio = 0.0f;
		_state = engine::dialogue_box_state::Closed;
	} else {
		_state = engine::dialogue_box_state::TransitioningOut;
	}
}

void engine::dialogue_box::_fromTransitioningToClosed(const float delta) noexcept {
	_positionRatio -= _calculatePositionRatioOffset(delta);
	if (_skipCurrentState || _positionRatio <= 0.0f) {
		_state = engine::dialogue_box_state::Closed;
		_positionRatio = 0.0f;
		_skipCurrentState = false;
	}
}

void engine::dialogue_box::_stateMachine() noexcept {
	float delta = calculateDelta();
	if (_state == engine::dialogue_box_state::Closed) {
		_fromClosedToTransitioning();
	} else if (_state == engine::dialogue_box_state::TransitioningIn) {
		// must be else-if so that Closed doesn't immediately switch to TransitioningIn.
		// this allows animate() to set the initial size of the dialogue box so that
		// _calculatePositionRatioOffset() doesn't consider the dialogue box size to be 0,
		// thus skipping this transition when first drawing
		// this also fixes any potential delta-related problems:
		// i.e. dialogue_box created, then animated seconds later resulting in a "skipped" first transition in
		_fromTransitioningToTyping(delta);
	}
	if (_state == engine::dialogue_box_state::Typing) {
		_fromTypingToStoppedTyping();
	}
	// see selectCurrentOption() [and skipCurrentState()]
	if (_skipCurrentState && _state == engine::dialogue_box_state::StoppedTyping) {
		_state = engine::dialogue_box_state::Option1;
		_skipCurrentState = false;
	}
	if (_state == engine::dialogue_box_state::Option1 || _state == engine::dialogue_box_state::Option2 || _state == engine::dialogue_box_state::Option3) {
		_fromOptionToTransitioning();
	}
	if (_state == engine::dialogue_box_state::TransitioningOut) {
		_fromTransitioningToClosed(delta);
	}
	// this ensures changes in mainText apply to a pre-existing dialogue box
	if (_state == engine::dialogue_box_state::StoppedTyping || _state == engine::dialogue_box_state::Option1 ||
		_state == engine::dialogue_box_state::Option2 || _state == engine::dialogue_box_state::Option3 ||
		_state == engine::dialogue_box_state::TransitioningOut) {
		_characterPosition = _fullText.size();
	}
}

void engine::dialogue_box::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_portion1, states);
	if (_position == engine::dialogue_box_position::Middle) {
		target.draw(_portion2, states);
	}
}

void engine::dialogue_box::_updateMainText() noexcept {
	_mainText.setString(_fullText.substr(0, _characterPosition));
}

void engine::dialogue_box::_updateCharacterSize(const sf::RenderTarget& target) noexcept {
	_mainText.setCharacterSize((unsigned int)target.getSize().y / 27);
	_nameText.setCharacterSize(_mainText.getCharacterSize());
	_option1Text.setCharacterSize(_mainText.getCharacterSize());
	_option2Text.setCharacterSize(_mainText.getCharacterSize());
	_option3Text.setCharacterSize(_mainText.getCharacterSize());
}

void engine::dialogue_box::_drawToCanvas(const sf::RenderTarget& target) noexcept {
	// remember to offset the weird origin behaviour for all Text objects
	// thanks Hapax: https://en.sfml-dev.org/forums/index.php?topic=15951.0
	_canvas.create(target.getSize().x, target.getSize().y);
	_canvas.clear(sf::Color::Transparent);
	_canvas.draw(_background);
	if (thereIsAName()) {
		_canvas.draw(_nameBackground);
		_canvas.draw(_nameText, sf::RenderStates().transform.translate(sf::Vector2f(-_nameText.getLocalBounds().left, -_nameText.getLocalBounds().top)));
	}
	_canvas.draw(_characterSprite);
	_canvas.draw(_mainText, sf::RenderStates().transform.translate(sf::Vector2f(-_mainText.getLocalBounds().left, -_mainText.getLocalBounds().top)));
	if (optionCount() > 0 && _state == engine::dialogue_box_state::StoppedTyping || _state == engine::dialogue_box_state::TransitioningOut) {
		_canvas.draw(_option1Text, sf::RenderStates().transform.translate(sf::Vector2f(-_option1Text.getLocalBounds().left, -_option1Text.getLocalBounds().top)));
		if (_option2Text.getString() != "") _canvas.draw(_option2Text, sf::RenderStates().transform.translate(sf::Vector2f(-_option2Text.getLocalBounds().left, -_option2Text.getLocalBounds().top)));
		if (_option3Text.getString() != "") _canvas.draw(_option3Text, sf::RenderStates().transform.translate(sf::Vector2f(-_option3Text.getLocalBounds().left, -_option3Text.getLocalBounds().top)));
		_canvas.draw(_indicator);
	}
	_canvas.display();

	_prepareHalfSprites();
}

void engine::dialogue_box::_prepareHalfSprites() noexcept {
	_portion1.setTexture(_canvas.getTexture(), true);
	_portion1.setPosition(0.0f, 0.0f);
	if (_position == engine::dialogue_box_position::Middle) {
		_portion2.setTexture(_canvas.getTexture());
		int heightOfHalves = (int)(_background.getSize().y / 2.0f * _positionRatio);
		_portion1.setTextureRect(sf::IntRect(
			(int)_background.getPosition().x,
			(int)(_background.getPosition().y - _nameBackground.getSize().y - _nameBackground.getOutlineThickness() - _background.getOutlineThickness()),
			(int)_background.getSize().x,
			heightOfHalves + (int)(_nameBackground.getSize().y + _nameBackground.getOutlineThickness() + _background.getOutlineThickness())
		));
		_portion2.setTextureRect(sf::IntRect(
			(int)_background.getPosition().x,
			(int)_background.getPosition().y + (int)_background.getSize().y - heightOfHalves,
			(int)_background.getSize().x,
			heightOfHalves + (int)_background.getOutlineThickness()
		));
		_portion1.setPosition(_background.getPosition().x, _background.getPosition().y - _nameBackground.getSize().y - _background.getOutlineThickness() - _nameBackground.getOutlineThickness());
		_portion2.setPosition(_background.getPosition().x, _portion1.getPosition().y + (float)heightOfHalves + _nameBackground.getSize().y + _background.getOutlineThickness() + _nameBackground.getOutlineThickness());
	}
}

void engine::dialogue_box::_playSound(const std::string& key) noexcept {
	if (_audioLibrary && key != "") _audioLibrary->play(key);
}

/*
// CONSTRUCTION ZONE

bool engine::dialogue_sequence::dialogue::animate(const sf::RenderTarget& target) noexcept {
	float delta = calculateDelta();
	// _sprite.animate(target);

	// state machine
	if (_state == engine::dialogue_state::TransitioningIn) {
		_transitionIn(_skipTransitioningIn);
	}
	
	if (_state == engine::dialogue_state::TransitioningOut) {
		_transitionOut(_skipTransitioningOut);
	}
}

void engine::dialogue_sequence::dialogue::_transitionIn(bool shouldEnd) noexcept {
	if (shouldEnd) {
		_state = engine::dialogue_state::Typing;
	} else {

	}
}

void engine::dialogue_sequence::dialogue::_transitionOut(bool shouldEnd) noexcept {
	if (shouldEnd) {
		_state = engine::dialogue_state::Closed;
	} else {

	}
}

void engine::dialogue_sequence::dialogue::_offsetPosition() noexcept {
	if (_state == engine::dialogue_state::TransitioningIn) {
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

void engine::dialogue_sequence::dialogue::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_bg, states);
	target.draw(_nameBg, states);
	target.draw(_sprite, states);
	target.draw(_nameText, states);
	target.draw(_text, states);
	target.draw(_indicator, states);
}

bool engine::dialogue_sequence::animate(const sf::RenderTarget& target) noexcept {
	_dialogue->animate(target);
}

void engine::dialogue_sequence::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*_dialogue);
}

engine::dialogue_sequence::dialogue_sequence(sfx::renderer* r, sfx::fonts* f,
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

engine::dialogue_sequence::~dialogue_sequence() noexcept {
	//remember to free up the dynamically allocated object!
	if (_diag) delete _diag;
}

unsigned int engine::dialogue_sequence::choice() const noexcept {
	return _lastChoice;
}

engine::dialogue_status engine::dialogue_sequence::status() const noexcept {
	return _lastStatus;
}

bool engine::dialogue_sequence::thereAreOptions() const noexcept {
	if (!_diag) return false;
	return _diag->thereAreOptions();
}

std::string engine::dialogue_sequence::current() const noexcept {
	return _currentObject->first;
}

bool engine::dialogue_sequence::_load(safe::json& j) noexcept {
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		std::string fontKey = "dialogue";
		engine::dialogue_data& d = _diagdata[i.key()];
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

bool engine::dialogue_sequence::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}*/