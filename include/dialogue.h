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
#include "audio.h"

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
		dialogue_box() noexcept;
		void setSounds(std::shared_ptr<sfx::audio> audioLibrary, const std::string& typing = "", const std::string& moveSelection = "", const std::string& select = "") noexcept;
		std::string getTypingSound() const noexcept;
		std::string getMoveSelectionSound() const noexcept;
		std::string getSelectSound() const noexcept;
		void setTransitionLength(const float seconds) noexcept;
		void setTypingDelay(const float seconds) noexcept;
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
		unsigned short selectCurrentOption() noexcept;
		void flip(const bool isFlipped) noexcept;
		bool thereAreOptions() const noexcept;
		bool thereIsAName() const noexcept;
		void skipCurrentState() noexcept;
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		// drawing
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void _updateMainText() noexcept;
		void _updateCharacterSize(const sf::RenderTarget& target) noexcept;
		void _updateBackground(const sf::RenderTarget& target) noexcept;
		void _updateNameBackground() noexcept;
		void _updateCharacterSprite(const sf::RenderTarget& target) noexcept;
		void _updateTextPositions() noexcept;
		void _resizeIndicator(const float size) noexcept;
		void _repositionIndicator() noexcept;
		void _drawToCanvas(const sf::RenderTarget& target) noexcept;
		void _prepareHalfSprites() noexcept;
		sf::RenderTexture _canvas;
		sf::RectangleShape _background;
		sf::RectangleShape _nameBackground;
		sfx::animated_sprite _characterSprite;
		sf::Text _nameText;
		sf::Text _mainText;
		sf::Text _option1Text;
		sf::Text _option2Text;
		sf::Text _option3Text;
		sf::ConvexShape _indicator;
		sf::Sprite _portion1, _portion2;
		std::size_t _characterPosition = 0;
		// base properties that define the transforms of the dialogue box
		float _calculatePositionRatioOffset(const float secondsElapsed) const noexcept;
		awe::dialogue_box_position _position = awe::dialogue_box_position::Bottom;
		float _sizeRatio = 0.15f;
		float _positionRatio = 0.0f;
		bool _flipped = false;
		// tracks the state of the dialogue box
		void _stateMachine() noexcept;
		void _fromClosedToTransitioning() noexcept;
		void _fromTransitioningToTyping(const float delta) noexcept;
		void _fromTypingToStoppedTyping() noexcept;
		void _fromOptionToTransitioning() noexcept;
		void _fromTransitioningToClosed(const float delta) noexcept;
		awe::dialogue_box_state _state = awe::dialogue_box_state::Closed;
		std::string _fullText = "";
		bool _skipTransitioningIn = false;
		bool _skipTransitioningOut = false;
		unsigned short _currentOption = 1;
		bool _skipCurrentState = false;
		// animated_sprite data
		std::shared_ptr<const sfx::animated_spritesheet> _sheet = nullptr;
		unsigned int _spriteID = 0;
		bool _spriteInfoChanged = true; // only if the above two fields are updated
		// padding/sizing constants
		static const float _smallPadding;
		static const float _largePadding;
		float _indicatorSize = 0.0f;
		// timing data
		float _transitionLength = 1.0f;
		float _typingDelay = 0.05f;
		sf::Clock _typingTimer;
		// audio data
		void _playSound(const std::string& key) noexcept;
		std::shared_ptr<sfx::audio> _audioLibrary = nullptr;
		std::string _typingKey = "";
		std::string _moveSelectionKey = "";
		std::string _selectKey = "";
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
*/