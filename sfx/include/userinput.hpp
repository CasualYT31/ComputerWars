/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file userinput.hpp
 * Classes used for user input handling.
 * These classes build on top of the SFML backend to manage keyboard, mouse, and
 * joystick inputs. Another layer of abstraction is added on top of these devices
 * so that multiple input devices and multiple buttons or keys can be assigned to
 * one control, and the client need only query if a configured game control is
 * being triggered, such as "jump," "move right," or "shoot."
 */

#pragma once

#include "safejson.hpp"
#include "script.hpp"
#include "SFML/Graphics.hpp"
#include <memory>
#include <unordered_set>

namespace sfx {
	/**
	 * Value representing an invalid mouse position.
	 * @warning If you need to change this value, make sure to also update
	 *          \c INVALID_MOUSE_SCRIPT in <tt>engine/source/script.cpp</tt>!
	 * @sa user_input::mousePosition()
	 */
	const sf::Vector2i INVALID_MOUSE = sf::Vector2i(INT_MIN, INT_MIN);

	/**
	 * The different types of control signal.
	 * \c FreeForm (\c 0) triggers game controls so long as a configured key/button
	 * is pressed or held.\n
	 * \c ButtonForm (\c 1) triggers game controls only when a configured
	 * key/button is initially pressed, and not when held.\n
	 * \c DelayedForm (\c 2) triggers game controls at configured intervals if a
	 * configured key/button is being held.\n
	 * \c SignalTypeCount stores the number of control signal types and must
	 * \b remain as the last member of \c control_signal.
	 * @sa user_input::operator[]()
	 */
	enum control_signal {
		FreeForm,
		ButtonForm,
		DelayedForm,
		SignalTypeCount
	};

	/**
	 * The two directions a joystick axis can take.
	 * Joystick axes always have one of two directions: either positive or
	 * negative. This enum is used to differentiate between those directions for
	 * the client.
	 */
	enum axis_direction {
		Negative = -1,
		Positive = 1
	};

	/**
	 * Represents a joystick axis.
	 * The axis ID and the axis direction are both stored.
	 * @sa sfx::axis_direction
	 */
	struct joystick {
		/**
		 * Registers the joystick type with the script interface.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Tests if a given \c sfx::joystick object is equivalent to this one.
		 * Two \c sfx::joystick objects are equivalent if both \c axis and
		 * \c direction are equal.
		 * @param  rhs Right-hand side argument. The \c sfx::joystick object to
		 *             test against.
		 * @return \c TRUE if both objects are equivalent, \c FALSE if not.
		 * @sa     operator!=()
		 */
		bool operator==(const sfx::joystick& rhs) const noexcept;

		/**
		 * Tests if a given \c sfx::joystick object is not equivalent to this one.
		 * Two \c sfx::joystick objects are not equivalent if either \c axis or
		 * \c direction or both are not equal with the other's.
		 * @param  rhs Right-hand side argument. The \c sfx::joystick object to
		 *             test against.
		 * @return \c TRUE if both objects are not equivalent, \c FALSE if they
		 *         are.
		 * @sa     operator==()
		 */
		bool operator!=(const sfx::joystick& rhs) const noexcept;

		/**
		 * The joystick axis ID.
		 */
		sf::Joystick::Axis axis;

		/**
		 * The intended direction of the axis.
		 */
		sfx::axis_direction direction;
	};

	/**
	 * Typedef representing a list of keyboard key IDs.
	 */
	typedef std::vector<sf::Keyboard::Key> KeyboardKeyList;

	/**
	 * Typedef representing a list of mouse button IDs.
	 */
	typedef std::vector<sf::Mouse::Button> MouseButtonList;

	/**
	 * Typedef representing a list of joystick button IDs.
	 */
	typedef std::vector<unsigned int> JoystickButtonList;

	/**
	 * Typedef representing a list of joystick axes.
	 */
	typedef std::vector<sfx::joystick> JoystickAxisList;

	/**
	 * Represents signal information pertaining to a single game control.
	 * @sa user_control
	 */
	struct signal_properties {
		/**
		 * Stores the type of signal this control has.
		 * @sa control_signal
		 */
		sfx::control_signal type = sfx::control_signal::ButtonForm;

		/**
		 * Stores the signal that was current at the last call to \c operator[]().
		 * @sa current
		 * @sa user_input::operator[]()
		 */
		bool previous = false;

		/**
		 * Stores the current signal.
		 * \c TRUE if a key/button is being pressed, \c FALSE if not.
		 * @sa previous
		 */
		bool current = false;

		/**
		 * Stores the signal that should be read by the client, based on the
		 * \c type.
		 */
		bool signal = false;

		/**
		 * Internal clock used for the timing of this individual signal.
		 * Used only if \c type is \c DelayedForm.
		 */
		sf::Clock clock;

		/**
		 * The ID of the \c delayLength delay to take into account.
		 * Used only if \c type is \c DelayedForm.
		 */
		std::size_t delayIndex = 0;

		/**
		 * The delays which define the intervals at which \c signal should be
		 * \c TRUE if \c current is \c TRUE.
		 * If the list is exhausted, the last delay length in the list will be used
		 * consistently so long as \c current is \c TRUE. If \c current becomes
		 * \c FALSE and then becomes \c TRUE again, the list will start from the
		 * beginning again. Used only if \c type is \c DelayedForm.
		 */
		std::vector<sf::Time> delayLength = {};
	};

	/**
	 * Stores the keys, buttons, and axes mapped to a single game control.
	 * @sa user_control
	 */
	struct user_configuration {
		/**
		 * The list of keys assigned to this game control.
		 */
		sfx::KeyboardKeyList keyboard = {};

		/**
		 * The list of mouse buttons assigned to this game control.
		 */
		sfx::MouseButtonList mouse = {};

		/**
		 * The list of joystick buttons assigned to this game control.
		 */
		sfx::JoystickButtonList joystickButton = {};

		/**
		 * The list of joystick axes assigned to this game control.
		 */
		sfx::JoystickAxisList joystickAxis = {};
	};

	/**
	 * Stores all the information associated with a single game control.
	 */
	struct user_control {
		/**
		 * The inputs mapped to this game control.
		 */
		sfx::user_configuration config;

		/**
		 * The signalling information attached to this game control.
		 */
		sfx::signal_properties signal;
	};

	/**
	 * Class used to convert from integers to enum values.
	 * For all the methods except \c toaxisdir(), if an unsigned value is beyond
	 * the enum's limits, it will be automatically brought back into the enum's
	 * limits by removing one from the associated \c Count enum member and
	 * returning this value. If this occurs, a warning will be logged if a logger
	 * object is given.
	 */
	class convert {
	public:
		/**
		 * Converts an unsigned integer to a keyboard key ID.
		 * @param  k      The unsigned integer holding the key ID.
		 * @param  logger Optional pointer to the logger object.
		 * @return The enum equivalent of the given key ID.
		 * @sa     sfx::convert
		 */
		static sf::Keyboard::Key tokeycode(unsigned int k,
			engine::logger *const logger = nullptr) noexcept;

		/**
		 * Converts an unsigned integer to a mouse button ID.
		 * @param  b      The unsigned integer holding the button ID.
		 * @param  logger Optional pointer to the logger object.
		 * @return The enum equivalent of the given button ID.
		 * @sa     sfx::convert
		 */
		static sf::Mouse::Button tomousebtn(unsigned int b,
			engine::logger *const logger = nullptr) noexcept;

		/**
		 * Converts an unsigned integer to a joystick axis ID.
		 * @param  a      The unsigned integer holding the axis ID.
		 * @param  logger Optional pointer to the logger object.
		 * @return The enum equivalent of the given axis ID.
		 * @sa     sfx::convert
		 */
		static sf::Joystick::Axis toaxis(unsigned int a,
			engine::logger *const logger = nullptr) noexcept;

		/**
		 * Converts an integer to a joystick axis direction.
		 * @param  d The integer to convert.
		 * @return \c sfx::axis_direction::Negative if <tt>d < 0</tt>,
		 *         \c afx::axis_direction::Positive otherwise.
		 */
		static sfx::axis_direction toaxisdir(int d) noexcept;

		/**
		 * Converts an unsigned integer to a control signal type ID.
		 * If an out-or-range \c s parameter is given, \c sfx::ButtonForm will be
		 * returned.
		 * @param  s      The unsigned integer holding the type ID.
		 * @param  logger Optional pointer to the logger object.
		 * @return The enum equivalent of the given type ID.
		 * @sa     sfx::convert
		 */
		static sfx::control_signal tosignaltype(unsigned int s,
			engine::logger *const logger = nullptr) noexcept;
	protected:
		/**
		 * This class cannot be instantiated by the client.
		 */
		convert() noexcept;
	};

	/**
	 * Represents a single user's controls.
	 */
	class user_input : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "user_input."
		 * @sa    \c engine::logger
		 */
		user_input(const std::string& name = "user_input") noexcept;

		/**
		 * Returns a set of controls that have been registered with this object.
		 */
		std::unordered_set<std::string> getControls() const noexcept;

		/**
		 * Sets the window that this \c user_input object is tied to.
		 * Tying user input to a window will have the following effects:
		 * <ol><li>\c mousePosition() will return the mouse position relative to
		 *         the window instead of relative to the upper left corner of the
		 *         screen, and will return \c sfx::INVALID_MOUSE in case the window
		 *         does not have focus.</li>
		 *     <li>\c keyboardKeysBeingPressed(), \c mouseButtonsBeingPressed(),
		 *         \c joystickButtonsBeingPressed(), and
		 *         \c joystickAxesBeingPressed() will return empty lists if the
		 *         window does not have focus. This can be used to automatically
		 *         ignore input that is not meant for the window or the process as
		 *         a whole.</li></ol>
		 * It is recommended to tie a window to a \c user_input object.
		 * @param window Pointer to the window to tie to this object. If it is
		 *               \c nullptr, this object will be untied from any window.
		 */
		void tieWindow(const std::shared_ptr<const sf::Window>& window) noexcept;

		/**
		 * Retrieves the joystick ID associated with this user.
		 * @return The joystick ID.
		 */
		unsigned int getJoystickID() const noexcept;

		/**
		 * Sets the joystick to associate with this user.
		 * If the ID of a non-existent joystick was given, it will be reset to the
		 * old joystick ID. This will also happen if a joystick ID with an ID
		 * greater than \c sf::Joystick::Count is given, as undefined behaviour
		 * occurs if those larger values are processed within the SFML.
		 * @param newid The new ID of the joystick to associate with this
		 *              user/object.
		 */
		void setJoystickID(unsigned int newid) noexcept;

		/**
		 * Retrieves the joystick axis threshold.
		 * @return The joystick axis threshold.
		 * @sa     setJoystickAxisThreshold()
		 */
		float getJoystickAxisThreshold() const noexcept;

		/**
		 * Sets the joystick axis threshold.
		 * If the axis is pressed harder than the given threshold, it is above (or
		 * below, if going in the negative direction) the threshold value and
		 * considered "pressed." If the given value was below \c 5.0, it will be
		 * readjusted to \c 5.0 and a warning will be logged. If the given value
		 * was above \c 95.0, it will be readjusted to \c 95.0 and a warning will
		 * be logged.
		 * @param newthreshold A value between \c 5.0 and \c 95.0.
		 * @sa    getJoystickAxisThreshold()
		 */
		void setJoystickAxisThreshold(float newthreshold) noexcept;

		/**
		 * Retrieves a copy of the user's control configurations for a given game
		 * control.
		 * This does not include signalling information.
		 * @param  name The name identifying the game control.
		 * @return The user's configured profile for the given game control, or a
		 *         blank object if \c name couldn't uniquely identify a game
		 *         control (an error will be logged).
		 */
		sfx::user_configuration getConfiguration(const std::string& name) const
			noexcept;

		/**
		 * Updates the user's control configurations for a given game control.
		 * This does not reset signalling information.\n
		 * This \b cannot be used to create new game controls on the fly. If a
		 * non-existent game control is referenced via \c name, an error will be
		 * logged.
		 * @param name The name identifying the game control.
		 * @param uc   The user configurations to set.
		 */
		void setConfiguration(const std::string& name,
			const sfx::user_configuration& uc) noexcept;

		/**
		 * Retrieves the mouse position in pixels.
		 * If a window has been tied to this object, the returned position will be
		 * relative to that window. If the window does not have focus,
		 * \c sfx::INVALID_MOUSE will be returned.\n
		 * If no window has been tied to this object, then the mouse position
		 * relative to the upper left corner of the primary monitor will be
		 * returned.
		 * @return The mouse position as described above.
		 * @sa     tieWindow()
		 */
		sf::Vector2i mousePosition() const noexcept;

		/**
		 * Updates the signalling information for all controls for this user.
		 * Should be called only once every iteration of the game loop. If not
		 * called, the class will not work and no controls will be registered.
		 */
		void update() noexcept;

		/**
		 * Tests whether a game control is being triggered or not.
		 * @param  name The name of the game control to test.
		 * @return \c TRUE if the control is being input, \c FALSE if not, or if
		 *         the \c name given didn't identify a game control.
		 */
		bool operator[](const std::string& name) noexcept;

		/**
		 * Returns a list of keys being pressed.
		 * @return A list of all the keys being pressed at the time of calling. An
		 *         empty list is returned if this object is tied to a window and
		 *         that window is not in focus.
		 * @sa     tieWindow()
		 */
		sfx::KeyboardKeyList keyboardKeysBeingPressed() const noexcept;

		/**
		 * Returns a list of mouse buttons being pressed.
		 * @return A list of all the mouse buttons being pressed at the time of
		 *         calling. If \c mousePosition() returns \c sfx::INVALID_MOUSE,
		 *         then an empty list is returned.
		 * @sa     mousePosition()
		 */
		sfx::MouseButtonList mouseButtonsBeingPressed() const noexcept;

		/**
		 * Returns a list of joystick buttons being pressed.
		 * @return A list of all the joystick buttons being pressed at the time of
		 *         calling. An empty list is returned if this object is tied to a
		 *         window and that window is not in focus.
		 */
		sfx::JoystickButtonList joystickButtonsBeingPressed() const noexcept;

		/**
		 * Returns a list of joystick axes being pressed.
		 * @return A list of all the joystick axes being pressed at the time of
		 *         calling. An empty list is returned if this object is tied to a
		 *         window and that window is not in focus.
		 */
		sfx::JoystickAxisList joystickAxesBeingPressed() const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * In the root object, there are two special key-value pairs:
		 * <ul><li>\c "joystickaxis" stores a floating point value, representing
		 *         the axis threshold.</li>
		 *     <li>\c "joystickid" stores an unsigned int value, representing the
		 *         joystick ID.</li></ul>
		 * All other key-value pairs in the root object must define different game
		 * controls and their properties. The keys define the names of the game
		 * controls. All of the values must be objects. Within each object, the
		 * following key-value pairs can be given:
		 * <table><tr><th>Key</th><th>Data Type</th><th>Description</th></tr>
		 *        <tr><td>type</td>
		 *        <td>unsigned int</td>
		 *        <td><b><em>Required</em></b> Represents the type of signal this
		 *            control will have.</td></tr>
		 *        <tr><td>delays</td>
		 *        <td>array of ints</td>
		 *        <td><b><em>Required if <tt>type == 2</tt></em></b> Stores the
		 *            delays, in milliseconds.</td></tr>
		 *        <tr><td>keys</td>
		 *        <td>array of unsigned ints</td>
		 *        <td><em>Optional</em> Stores all the key IDs which can trigger
		 *            this game control.</td></tr>
		 *        <tr><td>mouse</td>
		 *        <td>array of unsigned ints</td>
		 *        <td><em>Optional</em> Stores all the mouse button IDs which can
		 *            trigger this game control.</td></tr>
		 *        <tr><td>buttons</td>
		 *        <td>array of unsigned ints</td>
		 *        <td><em>Optional</em> Stores all the joystick button IDs which
		 *            can trigger this game control.</td></tr>
		 *        <tr><td>axes</td>
		 *        <td>array of 2-element arrays</td>
		 *        <td><em>Optional</em> Stores all the joystick axes which can
		 *            trigger this game control. The first element stores an
		 *            unsigned int representing the axis ID. The second element
		 *            stores an int representing the direction of the axis.</td>
		 *        </tr></table>
		 * All other keys within each game control object will be ignored.
		 * @warning The internal user profile (control and signal configurations)
		 *          is cleared upon calling this method.
		 * @param   j The \c engine::json object representing the contents of the
		 *            loaded script which this method reads.
		 * @return  Always returns \c TRUE.
		 * @sa      setJoystickAxisThreshold()
		 * @sa      setJoystickID()
		 * @sa      sfx::control_signal
		 * @sa      sfx::convert
		 * @sa      sfx::signal_properties
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON
		 * script that this method produces.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Updates a given collection of control lists.
		 * This method scans every input device (keyboard, mouse, and joystick) and
		 * updates the given lists with all the keys, buttons, and axes being
		 * pressed at the time of calling.
		 * @param  ref The control lists to update.
		 * @return \c TRUE if at least one key/button/axis was being pressed,
		 *         \c FALSE if nothing was registered.
		 * @sa     keyboardKeysBeingPressed()
		 * @sa     mouseButtonsBeingPressed()
		 * @sa     joystickButtonsBeingPressed()
		 * @sa     joystickAxesBeingPressed()
		 */
		bool _scanInput(sfx::user_configuration& ref) const noexcept;

		/**
		 * Checks a user's mapped inputs of a specific device for a given game
		 * control against another list.
		 * Is paired with \c _scanInput() to see if any inputs being sent by the
		 * user match up with a pre-configured game control's input mapping. For
		 * example, if the key 'G' was mapped to a game control "open_menu," then
		 * this method would return \c TRUE if 'G' was also found within the list
		 * of keys being currently pressed.
		 * @tparam T          The type of lists to cross-check, such as an
		 *                    \c sfx::KeyboardKeyList.
		 * @param  configured Intended to reference a game control's input mappings
		 *                    for a specific device.
		 * @param  list       Intended to reference a list of device inputs being
		 *                    sent at the time of calling.
		 * @return \c TRUE if at least one element of \c configured was also found
		 *         within \c list, \c FALSE otherwise.
		 */
		template<typename T>
		bool _isBeingTriggered(const T& configured, const T& list) const noexcept;

		/**
		 * Updates a single control's signalling data.
		 * Called for every configured control in \c update().
		 * @param scan The input mappings of a single game control.
		 * @param name The name of the game control to update.
		 */
		void _updateSingle(const sfx::user_configuration& scan,
			const std::string& name) noexcept;

		/**
		 * The ID of the joystick associated with this user.
		 */
		unsigned int _joystickid = 0;

		/**
		 * The joystick axix threshold associated with this user.
		 */
		float _joystickAxisThreshold = 25.0;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Stores the user's game controls (mappings and signalling data) by name.
		 */
		std::unordered_map<std::string, sfx::user_control> _control;

		/**
		 * A pointer to the window tied to this \c user_input object.
		 */
		std::shared_ptr<const sf::Window> _window;
	};
}

#include "tpp/userinput.tpp"
