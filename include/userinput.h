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

#include "safejson.h"
#include "sfml/Graphics.hpp"

namespace sfx {
	const sf::Vector2i INVALID_MOUSE = sf::Vector2i(-1, -1);

	enum control_signal {
		FreeForm,
		ButtonForm,
		DelayedForm,
		SignalTypeCount
	};

	enum axis_direction {
		Negative = -1,
		Positive = 1
	};

	struct joystick {
		bool operator==(const sfx::joystick& rhs) const noexcept;
		bool operator!=(const sfx::joystick& rhs) const noexcept;
		sf::Joystick::Axis axis;
		sfx::axis_direction direction;
	};

	typedef std::vector<sf::Keyboard::Key> KeyboardKeyList;
	typedef std::vector<sf::Mouse::Button> MouseButtonList;
	typedef std::vector<unsigned int> JoystickButtonList;
	typedef std::vector<sfx::joystick> JoystickAxisList;

	struct signal_properties {
		sfx::control_signal type = sfx::control_signal::ButtonForm;
		bool previous = false;
		bool current = false;
		bool signal = false;
		sf::Clock clock;
		std::size_t delayIndex = 0;
		std::vector<sf::Time> delayLength = {};
	};

	struct user_configuration {
		sfx::KeyboardKeyList keyboard = {};
		sfx::MouseButtonList mouse = {};
		sfx::JoystickButtonList joystickButton = {};
		sfx::JoystickAxisList joystickAxis = {};
	};

	struct user_control {
		sfx::user_configuration config;
		sfx::signal_properties signal;
	};

	class convert {
	public:
		static sf::Keyboard::Key tokeycode(unsigned int k) noexcept;
		static sf::Mouse::Button tomousebtn(unsigned int b) noexcept;
		static sf::Joystick::Axis toaxis(unsigned int a) noexcept;
		static sfx::axis_direction toaxisdir(int d) noexcept;
		static sfx::control_signal tosignaltype(unsigned int s) noexcept;
	protected:
		convert() noexcept;
	};

	class user_input : public safe::json_script {
	public:
		user_input(sf::RenderWindow& window, const std::string& name = "user_input") noexcept;

		unsigned int getJoystickID() const noexcept;
		unsigned int setJoystickID(unsigned int newid) noexcept;
		float getJoystickAxisThreshold() const noexcept;
		float setJoystickAxisThreshold(float newthreshold) noexcept;
		sfx::user_configuration getConfiguration(const std::string& name) const noexcept;
		sfx::user_configuration setConfiguration(const std::string& name, const sfx::user_configuration& uc) noexcept;

		sf::Vector2i mousePosition() const noexcept;

		void update() noexcept;

		bool operator[](const std::string& name) noexcept;

		bool listenForInput(sf::Keyboard::Key& input) const noexcept;
		bool listenForInput(sf::Mouse::Button& input) const noexcept;
		bool listenForInput(unsigned int& input) const noexcept;
		bool listenForInput(sfx::joystick& input) const noexcept;

		sfx::KeyboardKeyList keyboardKeysBeingPressed() const noexcept;
		sfx::MouseButtonList mouseButtonsBeingPressed() const noexcept;
		sfx::JoystickButtonList joystickButtonsBeingPressed() const noexcept;
		sfx::JoystickAxisList joystickAxesBeingPressed() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		bool _scanInput(sfx::user_configuration& ref) const noexcept;
		template<typename T>
		bool _isBeingTriggered(const T& configured, const T& list) const noexcept;
		void _updateSingle(const sfx::user_configuration& scan, const std::string& name) noexcept;

		unsigned int _joystickid = 0;
		float _joystickAxisThreshold = 25.0;
		global::logger _logger;
		std::unordered_map<std::string, sfx::user_control> _control;
		const sf::RenderWindow& _window;
	};
}

template<typename T>
bool sfx::user_input::_isBeingTriggered(const T& configured, const T& list) const noexcept {
	for (auto c = configured.begin(), ec = configured.end(); c != ec; c++) {
		for (auto l = list.begin(), el = list.end(); l != el; l++) {
			if (*c == *l) return true;
		}
	}
	return false;
}