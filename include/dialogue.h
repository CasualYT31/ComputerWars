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

#pragma once

#include "texture.h"

namespace awe {
	enum class dialogue_box_position {
		Bottom,
		Top,
		Middle,
		NumberOfPositions
	};

	enum class dialogue_box_state {
		Closed,
		TransitioningIn,
		Typing,
		StoppedTyping,
		Option1,
		Option2,
		Option3,
		TransitioningOut
	};

	class dialogue_box : public sfx::animated_drawable {
	public:
		void setPosition(const awe::dialogue_box_position position) noexcept;
		void setBackgroundColour(const sf::Color& colour) noexcept;
		void setThemeColour(const sf::Color& colour) noexcept;
		void setOutlineThickness(const float thickness) noexcept;
		void setMainText(const std::string& text) noexcept;
		void setNameText(const std::string& text) noexcept;
		void setFont(std::shared_ptr<sf::Font> font) noexcept;
		void setOptions(std::string option1, std::string option2 = "", std::string option3 = "") noexcept;
		void setSizeRatio(const float ratio) noexcept;
		void setSprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite) noexcept;
		void skipTransitioningIn(const bool skip) noexcept;
		void skipTransitioningOut(const bool skip) noexcept;
		void selectNextOption() noexcept;
		void selectPreviousOption() noexcept;
		void selectCurrentOption() noexcept;
		void flip(const bool isFlipped) noexcept;
		bool thereAreOptions() const noexcept;
		bool thereIsAName() const noexcept;
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		// drawing
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		// sf::RenderTexture _canvas;
		sf::RectangleShape _background;
		sf::RectangleShape _nameBackground;
		sfx::animated_sprite _characterSprite;
		sf::Text _nameText;
		sf::Text _mainText;
		sf::Text _option1Text;
		sf::Text _option2Text;
		sf::Text _option3Text;
		sf::ConvexShape _indicator;
		// base properties that define the transforms of the dialogue box
		sf::Vector2f _calculateBackgroundSize(const sf::RenderTarget& target) const noexcept;
		sf::Vector2f _calculateOrigin(const sf::Vector2f& size, const sf::RenderTarget& target) const noexcept;
		sf::Vector2f _calculateNameSize() const noexcept;
		sf::Vector2f _calculateNameOrigin(sf::Vector2f bgOrigin, const sf::Vector2f& bgSize, const sf::Vector2f& nameSize) const noexcept;
		void _calculateSpriteOrigin(const sf::Vector2f& bgOrigin, const sf::Vector2f& bgSzie) noexcept;
		awe::dialogue_box_position _position = awe::dialogue_box_position::Bottom;
		float _sizeRatio = 0.16f;
		float _positionRatio = 0.0f;
		bool _flipped = false;
		// tracks the state of the dialogue box
		void _stateMachine() noexcept;
		awe::dialogue_box_state _state = awe::dialogue_box_state::Closed;
		std::string _fullText = "";
		bool _skipTransitioningIn = false;
		bool _skipTransitioningOut = false;
		unsigned short _currentOption = 1;
		// animated_sprite data
		std::shared_ptr<const sfx::animated_spritesheet> _sheet = nullptr;
		unsigned int _spriteID = 0;
		bool _spriteInfoChanged = true; // only if the above two fields are updated
		sf::Vector2f _spriteTranslation = sf::Vector2f(0.0f, 0.0f);
	};
}

/*
/*#include "audio.h"
#include "fonts.h"
#include "userinput.h"
#include "texture.h"
#include "language.h"

#include "language.h"
#include "texture.h"

namespace awe {
	// CONSTRUCTION ZONE

	enum class dialogue_state {
		TransitioningIn,
		Typing,
		StoppedTyping,
		TransitioningOut,
		Closed,
		Option1,
		Option2,
		Option3
	};

	class dialogue_sequence : public sfx::animated_drawable, public safe::json_script {
	public:
		virtual bool animate(const sf::RenderTarget& target) noexcept;
		template<typename... Ts>
		void updateText(const i18n::language_dictionary& dict, Ts... values) noexcept;
	private:
		class dialogue : public sfx::animated_drawable {
		public:
			virtual bool animate(const sf::RenderTarget& target) noexcept;
			template <typename... Ts>
			void updateText(const i18n::language_dictionary& dict, Ts... values) noexcept;
		private:
			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

			void _transitionIn(bool shouldEnd) noexcept;
			void _transitionOut(bool shouldEnd) noexcept;

			void _offsetPosition() noexcept;

			bool _skipTransitioningIn = false;
			bool _skipTransitioningOut = false;

			dialogue_state _state = dialogue_state::TransitioningIn;
			sf::Text _text;
			sf::Text _nameText;
			sfx::animated_sprite _sprite;
			sf::RectangleShape _bg;
			sf::RectangleShape _indicator;
			sf::RectangleShape _nameBg;
		};
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		std::unique_ptr<dialogue> _dialogue = nullptr;
	};

	// END OF CONSTRUCTION ZONE




	enum class dialogue_location {
		Bottom,
		Top,
		Middle, // OLD -> PLEASE KEEP AS THE LAST ONE! This is because it is used in a check in dialogue_sequence to test if a read JSON uint is within the safe range of the enum
		LocationCount
	};

	

	struct dialogue_data {
		// CONSTRUCTION ZONE

		std::shared_ptr<sfx::animated_spritesheet> spritesheetObject = nullptr;
		unsigned int characterSpriteID = 0;

		std::string translatedStringToDisplay = "";
		std::shared_ptr<sf::Font> font = nullptr;
		sf::Time typingDelay = sf::milliseconds(50);
		sf::Time transitionOutDelay = sf::milliseconds(1000);
		float transitionSpeed = 400.0f; // pixels per second
		sf::Color borderColour = sf::Color();
		sf::Color backgroundColour = sf::Color(100, 100, 100, 255);
		std::array<std::string, 3> option = { "", "", "" };
		bool flip = false;
		bool skipable = true;
		bool skipTransitioningIn = false;
		bool skipTransitioningOut = false;
		dialogue_location location = dialogue_location::Bottom;
		float lineSpacingFactor = 1.0f;
		unsigned int fontSize = 30;
		float height = 165.0f;

		std::shared_ptr<sfx::audio> audioObject = nullptr;
		std::string typingSoundKey = "typing";
		std::string selectionSoundKey = "moveselection";

		std::shared_ptr<sfx::user_input> userInputObject = nullptr;
		std::string selectControlKey = "select";
		std::string leftControlKey = "left";
		std::string rightControlKey = "right";
		
		// END OF CONSTRUCTION ZONE
	};

	// CONSTRUCTION ZONE

	class dialogue : public sfx::animated_drawable {
	public:
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};

	// END OF CONSTRUCTION ZONE

	class dialogue {
	public:
		dialogue(const dialogue_data& data, const std::string& name = "dialogue") noexcept;
		template<typename... Ts>
		dialogue_status animate(Ts... values) noexcept;
		bool thereAreOptions() const noexcept;
		unsigned int choice() const noexcept;
	private:
		void _setInitialPosition() noexcept;
		void _offsetPosition() noexcept;
		bool _positionIsReady() noexcept;
		void _resetSelection() noexcept;
		template<typename... Ts>
		void _setTextToDisplay(Ts... values) noexcept;

		template<typename... Ts>
		void _stateMachine(Ts... values) noexcept;
		void _render() noexcept;

		sfx::delta_timer _delta;

		global::logger _logger;
		dialogue_data _data;
		dialogue_status _status = dialogue_status::Closed;
		sf::Clock _clock;
		std::size_t _charid = 0;
		std::string _textToDisplay = "";
		unsigned int _selection = 1;

		sf::RenderTexture _canvas;
		sf::RectangleShape _bg;
		sf::Text _text;
		sf::Sprite _sprite;
		sf::Texture _spriteTexture;
		sf::Sprite _tophalf, _bottomhalf;
		sf::Vector2f _position;
		std::size_t _o1pos = 0, _o2pos = 0, _o3pos = 0;
		sf::RectangleShape _indicator;
	};

	class dialogue_sequence : public safe::json_script {
	public:
		dialogue_sequence(sfx::renderer* r, sfx::fonts* f, sfx::user_input* u = nullptr, sfx::spritesheet* s = nullptr, i18n::language_dictionary* l = nullptr, sfx::audio* a = nullptr, const std::string& name = "dialogue_sequence") noexcept;
		~dialogue_sequence() noexcept;
		template<typename... Ts>
		bool animate(Ts... values) noexcept;
		unsigned int choice() const noexcept;
		bool thereAreOptions() const noexcept;
		dialogue_status status() const noexcept;
		std::string current() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		global::logger _logger;
		awe::dialogue* _diag = nullptr;
		std::map<std::string, awe::dialogue_data> _diagdata;
		std::map<std::string, awe::dialogue_data>::iterator _currentObject;
		unsigned int _lastChoice = 3;
		dialogue_status _lastStatus = dialogue_status::Closed;

		//mandatory
		sfx::renderer* _renderer = nullptr;
		sfx::fonts* _font = nullptr;
		//optional
		sfx::audio* _audio = nullptr;
		i18n::language_dictionary* _language = nullptr;
		sfx::spritesheet* _spritesheet = nullptr;
		sfx::user_input* _userinput = nullptr;
	};
}

template<typename... Ts>
bool awe::dialogue_sequence::animate(Ts... values) noexcept {
	bool ret = false;
	if (!_diag) {
		//allocate new dialogue object
		_diag = new awe::dialogue(_currentObject->second);
	}
	if (_diag) {
		_lastStatus = _diag->animate(values...);
		_lastChoice = _diag->choice();
		if (_lastStatus == awe::dialogue_status::Closed) {
			//next dialogue object
			//if we've reached the end, then reset the counter and return true
			if (++_currentObject == _diagdata.end()) {
				ret = true;
				_currentObject = _diagdata.begin();
			}
			//deallocate dialogue object
			delete _diag;
			_diag = nullptr;
			//problem? if we've deleted this drawable before display(), are there issues?
			//theoretically would be fine as it is out of sight anyway,
			//but if errors start cropping up it might be because of this
		}
	} else {
		_logger.error("Fatal error: failed to allocate a dialogue object.");
	}
	return ret;
}

template<typename... Ts>
awe::dialogue_status awe::dialogue::animate(Ts... values) noexcept {
	_delta.calculate();
	_stateMachine(values...);
	_render();
	return _status;
}

template<typename... Ts>
void awe::dialogue::_stateMachine(Ts... values) noexcept {
	if (_status == dialogue_status::Option1 || _status == dialogue_status::Option2 || _status == dialogue_status::Option3) {
		_status = dialogue_status::TransitioningOut;
	}

	if (_status == dialogue_status::Closed) {
		_delta.calculate(); //restarts timer so that it doesn't skip transitioning in
		_status = dialogue_status::TransitioningIn;
	}

	if (_status == dialogue_status::TransitioningIn || _status == dialogue_status::TransitioningOut) {
		_offsetPosition();
		if (_positionIsReady()) {
			if (_status == dialogue_status::TransitioningIn) {
				_status = dialogue_status::Typing;
				_setTextToDisplay(values...);
				_clock.restart();
			} else {
				_charid = 0;
				_text.setString("");
				_status = dialogue_status::Closed;
			}
		}
	}

	if (_status == dialogue_status::Typing) {
		if (_data.skipable && _data.userinputObject && (*_data.userinputObject)[_data.selectControlKey]) {
			_charid = _textToDisplay.size();
			_text.setString(_textToDisplay);
		}
		if (_clock.getElapsedTime() >= _data.typingDelay) {
			if (_charid < _textToDisplay.size()) _text.setString(_text.getString() + _textToDisplay.at(_charid++));
			if (thereAreOptions() && (_charid == _o1pos || _charid == _o2pos || _charid == _o3pos)) {
				_charid = _textToDisplay.size();
				_text.setString(_textToDisplay);
			}
			if (_data.audioObject) {
				_data.audioObject->play(_data.typingSoundKey);
			}
			_clock.restart();
		}
		if (_charid == _textToDisplay.size()) {
			_status = dialogue_status::StoppedTyping;
			_resetSelection();
		}
	} else if (_status == dialogue_status::StoppedTyping) {
		if (_data.userinputObject) {
			if (thereAreOptions()) {
				bool l = (*_data.userinputObject)[_data.leftControlKey], r = (*_data.userinputObject)[_data.rightControlKey];
				do {
					if (l) {
						_data.audioObject->play(_data.selectionSoundKey);
						_selection--;
					}
					if (r) {
						_data.audioObject->play(_data.selectionSoundKey);
						_selection++;
					}
					if (_selection < 1) _selection = 3;
					if (_selection > 3) _selection = 1;
				} while (_data.option[_selection - 1] == "");
				if ((*_data.userinputObject)[_data.selectControlKey]) {
					switch (_selection) {
					case 1:
						_status = dialogue_status::Option1;
						break;
					case 2:
						_status = dialogue_status::Option2;
						break;
					case 3:
						_status = dialogue_status::Option3;
						break;
					}
				}
			} else {
				if ((*_data.userinputObject)[_data.selectControlKey]) {
					_status = dialogue_status::TransitioningOut;
				}
			}
		} else {
			if (_clock.getElapsedTime() >= _data.transitionOutDelay) {
				_status = dialogue_status::TransitioningOut;
			}
		}
	}
}

//updates text to display
template<typename... Ts>
void awe::dialogue::_setTextToDisplay(Ts... values) noexcept {
	if (_data.languageObject) {
		_textToDisplay = (*_data.languageObject)(_data.nativeString, values...);
		if (thereAreOptions()) {
			if (_textToDisplay.find('\n') == std::string::npos) {
				_textToDisplay += "\n\n";
			} else {
				_textToDisplay += "\n";
			}
			if (_data.option[0] != "") {
				_o1pos = _textToDisplay.size();
				_textToDisplay += (*_data.languageObject)(_data.option[0]) + '\t';
			}
			if (_data.option[1] != "") {
				_o2pos = _textToDisplay.size();
				_textToDisplay += (*_data.languageObject)(_data.option[1]) + '\t';
			}
			if (_data.option[2] != "") {
				_o3pos = _textToDisplay.size();
				_textToDisplay += (*_data.languageObject)(_data.option[2]) + '\t';
			}
		}
	} else {
		_textToDisplay = _data.nativeString;
		if (thereAreOptions()) {
			if (_textToDisplay.find('\n') == std::string::npos) {
				_textToDisplay += "\n\n";
			} else {
				_textToDisplay += "\n";
			}
			if (_data.option[0] != "") {
				_o1pos = _textToDisplay.size();
				_textToDisplay += _data.option[0] + '\t';
			}
			if (_data.option[1] != "") {
				_o2pos = _textToDisplay.size();
				_textToDisplay += _data.option[1] + '\t';
			}
			if (_data.option[2] != "") {
				_o3pos = _textToDisplay.size();
				_textToDisplay += _data.option[2] + '\t';
			}
		}
	}
}*/