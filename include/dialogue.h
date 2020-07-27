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

/**@file dialogue.h
 * Allows the client to produce in-game dialogue boxes.
 * @todo COMPLETE DOC WHEN ALL CLASSES HAVE BEEN WRITTEN.
 */

#pragma once

#include "texture.h"
#include "audio.h"
#include "language.h"
#include "userinput.h"

/**
 * The \c engine namespace defines game code that isn't specific to Computer Wars.
 * Code within this namespace utilises dependencies defined within non-<tt>cwe</tt> namespaces to define game-based classes that aren't specific to Computer Wars.
 * These could include script, GUI, and dialogue management classes.
 */
namespace engine {
	/**
	 * Defines the different positions a dialogue box can have.
	 * <ul><li>\c Bottom - Anchors the dialogue box to the bottom of the target. The dialogue box moves up from the bottom edge of the target and moves down past the bottom edge of the target.</li>
	 * <li>\c Top - Anchors the dialogue box to the top of the target. The dialogue box moves down from the top edge of the target and moves up past the top edge of the target.</li>
	 * <li>\c Middle - Keeps the dialogue box in the very middle of the target. The dialogue box expands from the centre of the target and collapses back into the centre of the target.</li>
	 * <li>\c NumberOfPositions can be used to count the number of possible positions a dialogue box can have.</li></ul>
	 */
	enum class dialogue_box_position {
		Bottom,
		Top,
		Middle,
		NumberOfPositions
	};

	/**
	 * Defines the different states a dialogue box can be in.
	 * They are defined in the order the dialogue box states are assigned when animating.
	 */
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

	/**
	 * Represents a single dialogue box.
	 * Here is how this class is used in code:
	 * \code{.cpp}
	 * engine::dialogue_box box; // construct the object
	 * // setup the object using the public methods available
	 * while (true) { // this is the drawing loop
	 *     // code...
	 *     
	 *     // handle user input
	 *     // this includes calling methods such as the selectXOption() methods and skipCurrentState()
	 *     
	 *     // additional methods you can call to help keep the dialogue box responsive, to changes in language, for example
	 *     // changing most if not all properties of the dialogue box during animation should be entirely safe and will be handled correctly
	 *     box.setMainText(languageDictionaryObject("key"));
	 *     box.setOptions(etc);
	 *     box.setNameText(etc);
	 *     
	 *     sfxTarget.clear();
	 *     sfxTarget.animate(box); // animate the dialogue box, returns TRUE once it is over
	 *     sfxTarget.draw(box); // draw the dialogue box
	 *     sfxTarget.display();
	 *     
	 *     // code...
	 * }
	 * \endcode
	 * This class is responsible for most of the management of the dialogue box.
	 * The client is only responsible for the following:
	 * <ul><li>Calling a few additional methods \em during the draw loop as shown above. This isn't mandatory but it helps keep the dialogue box responsive.</li>
	 * <li>Handling user input: when users press certain buttons, the dialogue box should do certain things, like skip the text.</li>
	 * <li>Getting rid of the dialogue box once \c animate() returns \c TRUE. If kept alive in the loop, it will be animated-drawn over and over again until the loop is broken out of.</li></ul>
	 * Alternatively, you could use a \c dialogue_sequence object to handle these problems, as well as animate a series of dialogue boxes in order without having to manually manage them.
	 * @sa dialogue_sequence
	 */
	class dialogue_box : public sfx::animated_drawable {
	public:
		/**
		 * The constructor.
		 * The constructor simply sets the default outline thickness, which is \c 5.0.
		 */
		dialogue_box() noexcept;

		/**
		 * Assigns the sounds to play with the dialogue box.
		 * By default, no sounds are played. You can turn off individual sounds, as well as turn off sounds altogether, by passing \c nullptr and blank strings.
		 * Sounds \em can be played, however, in the following circumstances:
		 * <ol><li>When new letter/s added to the main text during animation (typing sounds).</li>
		 * <li>When \c selectNextOption() and \c selectPreviousOption() are called successfully (moving option selection).</li>
		 * <li>When \c selectCurrentOption() is called successfully (selecting an option).</li></ol>
		 * @param audioLibrary  A shared point to an \c sfx::audio object.
		 * @param typing        The key of the audio to play when letters are typed out.
		 * @param moveSelection The key of the audio to play when the option selection is moved.
		 * @param select        The Key of the audio to play when an option is selected.
		 */
		void setSounds(std::shared_ptr<sfx::audio> audioLibrary, const std::string& typing = "", const std::string& moveSelection = "", const std::string& select = "") noexcept;

		/**
		 * Retrieves the typing sound that's been set.
		 * @return The key of the audio which plays when letters are typed out.
		 */
		std::string getTypingSound() const noexcept;

		/**
		 * Retrieves the sound that plays when the current option selection is changed.
		 * @return The key of the audio which plays when the option selection is moved.
		 */
		std::string getMoveSelectionSound() const noexcept;

		/**
		 * Retrieves the option selection sound that's been set.
		 * @return The key of the audio which plays when an option is selected.
		 */
		std::string getSelectSound() const noexcept;

		/**
		 * Sets the length of transitioning in and out.
		 * @param seconds The length of time, in seconds, that transitions should go on for.
		 */
		void setTransitionLength(const float seconds) noexcept;

		/**
		 * Sets the length of time in between letters being typed out.
		 * @param seconds The length of time, in seconds, between one letter being typed out and the next.
		 */
		void setTypingDelay(const float seconds) noexcept;

		/**
		 * Sets the position of the dialogue box.
		 * If \c NumberOfPositions is given, the position will instead be set to \c Bottom.
		 * @param position The position to set.
		 * @sa    dialogue_box_position
		 */
		void setPosition(const dialogue_box_position position) noexcept;

		/**
		 * Sets the background colour of the dialogue box.
		 * Both the background of the main body of the dialogue box as well as the name rectangle above it is drawn with the same background colour.
		 * @param colour The new background colour.
		 */
		void setBackgroundColour(const sf::Color& colour) noexcept;

		/**
		 * Sets the outline colour of he dialogue box.
		 * It also sets the fill colour of the indicator.
		 * @param colour The new theme colour.
		 */
		void setThemeColour(const sf::Color& colour) noexcept;

		/**
		 * Sets the outline thickness of the dialogue box.
		 * @param thickness The thickness in pixels.
		 */
		void setOutlineThickness(const float thickness) noexcept;

		/**
		 * Updates the main text of the dialogue box.
		 * New lines can be inserted with \c \\n. All other escape sequences should work as expected.
		 * @remark The default sizing settings of the dialogue box should mean that three lines of text in the Computer Wars font can fit comfortably within the dialogue box plus the options.
		 *         However, this will not be consistent across all fonts. It is safer to assume two lines only can fit comfortably when outside of Computer Wars.
		 * @param  text The text to display in the main body of the dialogue box.
		 */
		void setMainText(const std::string& text) noexcept;

		/**
		 * Updates the name text of the dialogue box.
		 * You can set this to a blank string (which is the default). In which case, the name background will not be rendered.
		 * @param text The text to display on the name background which is attached to the dialogue box.
		 */
		void setNameText(const std::string& text) noexcept;

		/**
		 * Sets the font to use with all text.
		 * If \c nullptr is given, the call will be ignored.
		 * @param font A pointer to the font object to set to each text object.
		 */
		void setFont(std::shared_ptr<sf::Font> font) noexcept;

		/**
		 * Sets the available options.
		 * This method ensures that each option given will be set to the first option first, then to the second, then to the third
		 * For example, if a blank string is given for \c option1 but \c option2 is not blank, \c option2 will instead be assigned to the first option text.
		 * \c _currentOption will also be adjusted to ensure it keeps within range. For example, if there were three options previously and the user had their selection on
		 * option three, and then the third option was removed, the selection would then move to the second option.\n
		 * By default, there are no options. No options will render, and neither will the indicator. All calls to option-related methods except this one will be ignored.
		 * Instead, \c skipCurrentState() should be used to advance forward from the \c StoppedTyping state if this is the case.
		 * @param option1 The text of the first option.
		 * @param option2 The text of the second option.
		 * @param option3 The text of the third option.
		 */
		void setOptions(const std::string& option1, const std::string& option2 = "", const std::string& option3 = "") noexcept;

		/**
		 * Updates the ratio of dialogue box to target which defines the height of the main background.
		 * For example, if \c ratio is \c 0.5, the main body of the dialogue box will be half the height of the target.
		 * Must be a value between \c 0.0 and \c 1.0 - higher values will cause the dialogue box to be larger than the target.
		 * @param ratio The ratio of dialogue box : target.
		 */
		void setSizeRatio(const float ratio) noexcept;

		/**
		 * Sets the character sprite which is drawn alongside the main text within the main body.
		 * If a \c nullptr is given, the character sprite will be removed and sizing calculations will not take it into account.
		 * <b>Note: the character sprite is \em only animated during the \c Typing state. Afterwards its current frame will be reset to 0.</b>
		 * @param sheet  The \c sfx::animated_spritesheet which contains the character sprites.
		 * @param sprite The ID of the character sprite to draw with this dialogue box.
		 */
		void setSprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite) noexcept;

		/**
		 * Dictates whether the dialogue box skips transitioning in or not.
		 * @param skip If \c TRUE, the dialogue box will not transition in and will instead appear immediately.
		 */
		void skipTransitioningIn(const bool skip) noexcept;

		/**
		 * Dictates whether the dialogue box skips transitioning out or not.
		 * @param skip If \c TRUE, the dialogue box will not transition out and will instead disappear immediately after typing, option selection, etc.
		 */
		void skipTransitioningOut(const bool skip) noexcept;

		/**
		 * Selects the next option in the list.
		 * Moves the current option selection to the option on the right.
		 * If there aren't any more options on the right, the first option will be selected.
		 * @remark This method only works if the state of the dialogue box is \c StoppedTyping and if there are options to select.
		 *         If both of these conditions aren't satisified, the call is ignored/unsuccessful.
		 * @sa     \c setSounds()
		 */
		void selectNextOption() noexcept;

		/**
		 * Selects the previous option in the list.
		 * Moves the current option selection to the option on the left.
		 * If there aren't any more options on the left, the last option will be selected.
		 * @remark This method only works if the state of the dialogue box is \c StoppedTyping and if there are options to select.
		 *         If both of these conditions aren't satisified, the call is ignored/unsuccessful.
		 * @sa     \c setSounds()
		 */
		void selectPreviousOption() noexcept;

		/**
		 * Selects the current option in the list.
		 * @remark This method only works if the state of the dialogue box is \c StoppedTyping and if there are options to select.
		 *         If neither of these conditions are satisified, the call is ignored/unsuccessful.
		 * @return The ID of the selected option, from \c 1 to \c 3. \c 0 is returned if the call is ignored as described above.
		 * @sa     \c setSounds()
		 */
		unsigned short selectCurrentOption() noexcept;

		/**
		 * Dictates whether the dialogue box is flipped or not.
		 * Normally, the character sprite is rendered on the left while the main text is rendered next to the character sprite.
		 * The name and its background are drawn above the dialogue box, along the top left side (or bottom if the box' position is \c Top).
		 * If the dialogue box is flipped, the main text is instead rendered on the left and the character sprite on the right.
		 * The name and its background are drawn above/below the dialogue box along the top \em right side instead.
		 * @param isFlipped \c TRUE if the dialogue box should be flipped.
		 */
		void flip(const bool isFlipped) noexcept;

		/**
		 * Returns the number of options this dialogue box has.
		 * @return The number of options available.
		 */
		unsigned short optionCount() const noexcept;

		/**
		 * Used to test if the dialogue box has any name text.
		 * @return \c TRUE if the name text isn't a blank string, \c FALSE if so.
		 */
		bool thereIsAName() const noexcept;

		/**
		 * Retrieves the first option's text directly from the text object.
		 * @return The previously assigned text.
		 */
		std::string getOption1Text() const noexcept;

		/**
		 * Retrieves the second option's text directly from the text object.
		 * @return The previously assigned text.
		 */
		std::string getOption2Text() const noexcept;

		/**
		 * Retrieves the third option's text directly from the text object.
		 * @return The previously assigned text.
		 */
		std::string getOption3Text() const noexcept;

		/**
		 * Skips the current state.
		 * This is used to skip transitions, typing, and option selection midway through.
		 * It is the primary way of causing the dialogue box to transition out after all text has been displayed.
		 * \c selectCurrentOption() also achieves this.
		 */
		void skipCurrentState() noexcept;

		/**
		 * Animates the dialogue box.
		 * All sizes and positions are calculated within here.
		 * The dialogue box is also rendered to an internal \c RenderTexture at this time - this was done in order to achieve the \c Middle position transition.
		 * @return \c TRUE once the dialogue box has closed after being fully animated and rendered, \c FALSE if the dialogue box' animation is still in progress.
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * Draws the dialogue box.
		 * Usually, \c _canvas' texture is usually just exported to \c _portion1 and rendered as normal.
		 * However, if the box' position is \c Middle and it's transitioning, both \c _portion1 and \c _portion2 will be used to carry out the effect.
		 * @param target The target to render the dialogue box to.
		 * @param states The render states to apply to the dialogue box. Applying transforms is perfectly valid and will not alter the internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Updates the main text's string.
		 */
		void _updateMainText() noexcept;

		/**
		 * Updates the text object's character size in proportion to the height of the target.
		 * @param target The target to measure against.
		 */
		void _updateCharacterSize(const sf::RenderTarget& target) noexcept;

		/**
		 * Updates the main background's size and position.
		 * The dialogue box occupies the entire width of the target by default (if no addition states are applied when drawing).
		 * @param target The target to measure against.
		 * @sa    setSizeRatio()
		 */
		void _updateBackground(const sf::RenderTarget& target) noexcept;

		/**
		 * Updates the name background's size and position to ensure it comfortably surrounds the name text.
		 */
		void _updateNameBackground() noexcept;

		/**
		 * Animates the character sprite and repositions it.
		 * @param target The target to animate the sprite on (same as the dialogue box).
		 */
		void _updateCharacterSprite(const sf::RenderTarget& target) noexcept;

		/**
		 * Updates all text positions.
		 */
		void _updateTextPositions() noexcept;

		/**
		 * Resizes the indicator.
		 * @param size The height and width of the indicator (an equilateral triangle pointing to the right), in pixels.
		 */
		void _resizeIndicator(const float size) noexcept;

		/**
		 * Repositions the indicator.
		 */
		void _repositionIndicator() noexcept;

		/**
		 * Renders the dialogue box to the internal canvas.
		 * @param target The target which the canvas will eventually be rendered to in \c draw().
		 */
		void _drawToCanvas(const sf::RenderTarget& target) noexcept;

		/**
		 * Prepares \c _portion1 and \c _portion2 for rendering.
		 */
		void _prepareHalfSprites() noexcept;

		/**
		 * Calculates how much the position ratio should be offset by during a transition.
		 * @param secondsElapsed The seconds elapsed since the last call to \c animate().
		 */
		float _calculatePositionRatioOffset(const float secondsElapsed) const noexcept;

		/**
		 * The state machine of the dialogue box.
		 * Determines which state to switch to next and performs \em some of the things to do in each state, such as offsetting the position ratio field and maintaining _characterPosition.
		 */
		void _stateMachine() noexcept;

		/**
		 * Code performed when switching from \c Closed state to \c TransitioningIn state.
		 */
		void _fromClosedToTransitioning() noexcept;

		/**
		 * Code performed whilst in the \c TransitioningIn state.
		 * @param delta The number of seconds elapsed since the last call to \c animate().
		 */
		void _fromTransitioningToTyping(const float delta) noexcept;

		/**
		 * Code performed whilst in the \c Typing state.
		 */
		void _fromTypingToStoppedTyping() noexcept;

		/**
		 * Code performed when switching from the StoppedTyping/Option1/Option2/Option3 states to the \c TransitioningOut state.
		 */
		void _fromOptionToTransitioning() noexcept;

		/**
		 * Code performed whilst in the \c TransitioningOut state.
		 * @param delta The number of seconds elapsed since the last call to \c animate().
		 */
		void _fromTransitioningToClosed(const float delta) noexcept;

		/**
		 * Plays a sound.
		 * If \c _audioLibrary is \c nullptr, calls will be ignored.
		 * @param key The key of the audio from \c _audioLibrary to play.
		 */
		void _playSound(const std::string& key) noexcept;

		/**
		 * Small padding constant, the padding applied to name text as well as the top and bottom of the main text.
		 */
		static const float _smallPadding;

		/**
		 * Large padding constant, the padding applied to the left and right of content within the main background.
		 */
		static const float _largePadding;

		/**
		 * The internal canvas to render the dialogue box to.
		 */
		sf::RenderTexture _canvas;

		/**
		 * The main background object.
		 */
		sf::RectangleShape _background;

		/**
		 * The name background object.
		 */
		sf::RectangleShape _nameBackground;

		/**
		 * The animated character sprite object.
		 */
		sfx::animated_sprite _characterSprite;

		/**
		 * The name text object.
		 */
		sf::Text _nameText;

		/**
		 * The main text object.
		 */
		sf::Text _mainText;

		/**
		 * The first option text object.
		 */
		sf::Text _option1Text;

		/**
		 * The second option text object.
		 */
		sf::Text _option2Text;

		/**
		 * The third option text object.
		 */
		sf::Text _option3Text;

		/**
		 * The indicator object.
		 */
		sf::ConvexShape _indicator = sf::ConvexShape(3);

		/**
		 * The first portion which is intended to draw the top half of the dialogue box (mostly all of the dialogue box).
		 */
		sf::Sprite _portion1;
		
		/**
		 * The second portion which is intended to draw the bottom half of the dialogue box (mostly not drawn at all).
		 */
		sf::Sprite _portion2;

		/**
		 * Defines the length of \c _fullText to render.
		 */
		std::size_t _characterPosition = 0;

		/**
		 * The position of the dialogue box.
		 */
		dialogue_box_position _position = dialogue_box_position::Bottom;

		/**
		 * The size ratio defining how much dialogue box to the render target they should be along the Y axis.
		 */
		float _sizeRatio = 0.15f;

		/**
		 * This ratio defines how much of the dialogue is visible. \c 0.0 means none of it, \c 0.5 means half of it, \c 1.0 means all of it.
		 */
		float _positionRatio = 0.0f;

		/**
		 * Stores the flipped property of the dialogue box.
		 */
		bool _flipped = false;

		/**
		 * Tracks the state of the dialogue box.
		 */
		dialogue_box_state _state = dialogue_box_state::Closed;

		/**
		 * Stores the main text of the dialogue box.
		 */
		std::string _fullText = "";

		/**
		 * Stores the transition in skipping property of the dialogue box.
		 */
		bool _skipTransitioningIn = false;

		/**
		 * Stores the transition out skipping property of the dialogue box.
		 */
		bool _skipTransitioningOut = false;

		/**
		 * Stores the ID of the currently selected option.
		 */
		unsigned short _currentOption = 0;

		/**
		 * Flag which is set to \c TRUE when a state is to be skipped in \c _stateMachine().
		 */
		bool _skipCurrentState = false;

		/**
		 * The pointer to the animated spritesheet object.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> _sheet = nullptr;

		/**
		 * The ID of the sprite from \c _sheet to assign to \c _characterSprite.
		 */
		unsigned int _spriteID = 0;

		/**
		 * To be set to \c TRUE when \c _sheet and/or \c _spriteID are changed.
		 */
		bool _spriteInfoChanged = true;

		/**
		 * The size of the indicator in pixels.
		 */
		float _indicatorSize = 0.0f;

		/**
		 * The length of transitioning in and out in seconds.
		 */
		float _transitionLength = 1.0f;

		/**
		 * The length of time in between \c _characterPosition increments in seconds.
		 */
		float _typingDelay = 0.05f;

		/**
		 * Internal clock used to measure the time elapsed between \c _characterPosition increments.
		 */
		sf::Clock _typingTimer;

		/**
		 * Pointer to the \c sfx::audio object containing the sounds to play.
		 */
		std::shared_ptr<sfx::audio> _audioLibrary = nullptr;

		/**
		 * The key of the audio to play when characters are typed out.
		 */
		std::string _typingKey = "";

		/**
		 * The key of the audio to play when the option selection is changed successfully.
		 */
		std::string _moveSelectionKey = "";

		/**
		 * The key of the audio to play when an option has been successfully selected.
		 */
		std::string _selectKey = "";
	};

	class dialogue_sequence : public safe::json_script, public sfx::animated_drawable {
	public:
		dialogue_sequence(const std::string& name = "dialogue_sequence") noexcept;
		void setLanguageDictionary(std::shared_ptr<i18n::language_dictionary> dict) noexcept;
		void setUserInput(std::shared_ptr<sfx::user_input> ui) noexcept;
		template<typename... Ts>
		void updateMainText(Ts... values) noexcept;
		template<typename... Ts>
		void updateNameText(Ts... values) noexcept;
		template<typename... Ts>
		void updateOption1Text(Ts... values) noexcept;
		template<typename... Ts>
		void updateOption2Text(Ts... values) noexcept;
		template<typename... Ts>
		void updateOption3Text(Ts... values) noexcept;
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		std::unique_ptr<engine::dialogue_box> _allocateDialogueBox(const std::size_t i) noexcept;

		// may need to combine this with dialogue_box class
		struct dialogue_box_data {
			engine::dialogue_box_position position = engine::dialogue_box_position::Bottom;
			float size = 0.15f;
			bool flipped = false;
			std::string mainText = "";
			bool skipTransIn = false;
			bool skipTransOut = false;
			std::shared_ptr<sfx::animated_spritesheet> sheet = nullptr;
			unsigned int spriteID = 0;
			float transLength = 1.0f;
			float typingDelay = 0.05f;
			std::shared_ptr<sfx::audio> audio = nullptr;
			std::string typingSoundKey = "typing";
			std::string moveSelSoundKey = "movesel";
			std::string selectSoundKey = "select";
			sf::Color themeColour = sf::Color::Black;
			std::string nameText = "";
			std::shared_ptr<sf::Font> font = nullptr;
			std::array<std::string, 3> options = { "", "", "" };
		};

		std::shared_ptr<i18n::language_dictionary> _langDic = nullptr;
		std::shared_ptr<sfx::user_input> _userInput = nullptr;
		std::string _moveRightControlKey = "right";
		std::string _moveLeftControlKey = "left";
		std::string _selectControlKey = "select";
		std::string _skipControlKey = "pause";

		std::vector<dialogue_box_data> _boxes;
		std::size_t _currentBoxID = 0;
		std::unique_ptr<dialogue_box> _currentBox;

		global::logger _logger;
	};
}

template<typename... Ts>
void engine::dialogue_sequence::updateMainText(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setMainText((*_langDic)(_boxes[_currentBoxID].mainText, values...));
}

template<typename... Ts>
void engine::dialogue_sequence::updateNameText(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setNameText((*_langDic)(_boxes[_currentBoxID].nameText, values...));
}

template<typename... Ts>
void engine::dialogue_sequence::updateOption1Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions((*_langDic)(_boxes[_currentBoxID].options[0], values...), _currentBox->getOption2Text(), _currentBox->getOption3Text());
}

template<typename... Ts>
void engine::dialogue_sequence::updateOption2Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions(_currentBox->getOption1Text(), (*_langDic)(_boxes[_currentBoxID].options[1], values...), _currentBox->getOption3Text());
}

template<typename... Ts>
void engine::dialogue_sequence::updateOption3Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions(_currentBox->getOption1Text(), _currentBox->getOption2Text(), (*_langDic)(_boxes[_currentBoxID].options[2], values...));
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