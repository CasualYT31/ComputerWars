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

#include "userinput.hpp"
#include "fmtengine.hpp"
#include "gui.hpp"

void sfx::joystick::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if (!engine->GetTypeInfoByName("JoystickAxis")) {
		int r = engine->RegisterObjectType("JoystickAxis", sizeof(sfx::joystick),
			asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sfx::joystick>() |
			asOBJ_APP_CLASS_ALLINTS);
		engine->RegisterObjectProperty("JoystickAxis", "uint axis",
			asOFFSET(sfx::joystick, axis));
		engine->RegisterObjectProperty("JoystickAxis", "int direction",
			asOFFSET(sfx::joystick, direction));
		document->DocumentObjectType(r, "Represents a joystick axis input. This "
			"class stores the ID of the axis being input, and which direction the "
			"axis is currently being pressed.\n"
			"<tt>axis</tt> is an <tt>sf::Joystick::Axis</tt> value. If too large "
			"of an ID is given, via this object, into a function, it will be "
			"lowered down to the maximum possible value and a warning will be "
			"logged.\n"
			"A positive <tt>direction</tt> value represents the positive "
			"direction (including <tt>0</tt>), and a negative value represents "
			"the negative direction.");
	}
}

bool sfx::joystick::operator==(const sfx::joystick& rhs) const noexcept {
	return this->axis == rhs.axis && this->direction == rhs.direction;
}

bool sfx::joystick::operator!=(const sfx::joystick& rhs) const noexcept {
	return !(*this == rhs);
}

sf::Keyboard::Key sfx::convert::tokeycode(unsigned int k,
	engine::logger *const logger) noexcept {
	if (k >= sf::Keyboard::KeyCount) {
		auto old = k;
		k = sf::Keyboard::KeyCount - 1;
		if (logger) logger->warning("Given keycode {} is larger than the maximum "
			"keycode of {}: int {} will be converted to enum {}.", old, k, old, k);
	}
	return static_cast<sf::Keyboard::Key>(k);
}

sf::Mouse::Button sfx::convert::tomousebtn(unsigned int b,
	engine::logger *const logger) noexcept {
	if (b >= sf::Mouse::ButtonCount) {
		auto old = b;
		b = sf::Mouse::ButtonCount - 1;
		if (logger) logger->warning("Given mouse button ID {} is larger than the "
			"maximum mouse button ID of {}: int {} will be converted to enum {}.",
			old, b, old, b);
	}
	return static_cast<sf::Mouse::Button>(b);
}

sf::Joystick::Axis sfx::convert::toaxis(unsigned int a,
	engine::logger *const logger) noexcept {
	if (a >= sf::Joystick::AxisCount) {
		auto old = a;
		a = sf::Joystick::AxisCount - 1;
		if (logger) logger->warning("Given gamepad axis ID {} is larger than the "
			"maximum gamepad axis ID of {}: int {} will be converted to enum {}.",
			old, a, old, a);
	}
	return static_cast<sf::Joystick::Axis>(a);
}

sfx::axis_direction sfx::convert::toaxisdir(int d) noexcept {
	if (d < 0) return sfx::axis_direction::Negative;
	return sfx::axis_direction::Positive;
}

sfx::control_signal sfx::convert::tosignaltype(unsigned int s,
	engine::logger *const logger) noexcept {
	if (s >= static_cast<unsigned int>(sfx::control_signal::SignalTypeCount)) {
		auto old = s;
		s = static_cast<unsigned int>(sfx::control_signal::ButtonForm);
		if (logger) logger->warning("Given signal type ID {} is larger than the "
			"maximum signal type ID of {}: int {} will be converted to enum {}.",
			old, s, old, s);
	}
	return static_cast<sfx::control_signal>(s);
}

sfx::user_input::user_input(const engine::logger::data& data) :
	json_script({data.sink, "json_script"}), _logger(data) {}

std::unordered_set<std::string> sfx::user_input::getControls() const {
	std::unordered_set<std::string> ret;
	for (auto control : _control) ret.insert(control.first);
	return ret;
}

void sfx::user_input::tieWindow(const std::shared_ptr<const sf::Window>& window) {
	if (_window && !window) {
		_logger.warning("Untying user_input object from the window at position "
			"{}!", _window->getPosition());
	}
	_window = window;
}

unsigned int sfx::user_input::getJoystickID() const noexcept {
	return _joystickid;
}

void sfx::user_input::setJoystickID(unsigned int newid) {
	auto old = getJoystickID();
	if (newid <= sf::Joystick::Count && sf::Joystick::isConnected(newid)) {
		_joystickid = newid;
	} else {
		_logger.warning("Attempted to set a joystick with ID {} that wasn't "
			"connected, resetting the ID to {}.", newid, old);
		_joystickid = old;
	}
}

float sfx::user_input::getJoystickAxisThreshold() const noexcept {
	return _joystickAxisThreshold;
}

void sfx::user_input::setJoystickAxisThreshold(float newthreshold) noexcept {
	_joystickAxisThreshold = newthreshold;
	if (_joystickAxisThreshold < 5.0) {
		_logger.warning("Attempted to set a joystick axis threshold of {}, reset "
			"to 5.0.", _joystickAxisThreshold);
		_joystickAxisThreshold = 5.0;
	}
	if (_joystickAxisThreshold > 95.0) {
		_logger.warning("Attempted to set a joystick axis threshold of {}, reset "
			"to 95.0.", _joystickAxisThreshold);
		_joystickAxisThreshold = 95.0;
	}
}

sfx::user_configuration sfx::user_input::getConfiguration(const std::string& name)
	const {
	if (_control.find(name) == _control.end()) {
		_logger.error("Could not retrieve configurations for non-existent control "
			"called \"{}\".", name);
		return sfx::user_configuration();
	}
	return _control.at(name).config;
}

void sfx::user_input::setConfiguration(const std::string& name,
	const sfx::user_configuration& uc) {
	if (_control.find(name) != _control.end()) {
		_control[name].config = uc;
	} else {
		_logger.error("Attempted to update the configurations of non-existent "
			"control called \"{}\".", name);
	}
}

sf::Vector2i sfx::user_input::mousePosition() const {
	if (_window) {
		if (_window->hasFocus()) {
			// Also ignore button presses if the mouse is currently outside of the
			// client area.
			auto pos = sf::Mouse::getPosition(*_window);
			if (pos.x >= 0 && pos.y >= 0 && pos.x <= (int)_window->getSize().x &&
				pos.y <= (int)_window->getSize().y) {
				return pos;
			}
		}
		return sfx::INVALID_MOUSE;
	} else {
		return sf::Mouse::getPosition();
	}
}

void sfx::user_input::update() {
	sf::Joystick::update();
	user_configuration scan;
	_scanInput(scan);
	for (auto& itr : _control) _updateSingle(scan, itr.first);
}

bool sfx::user_input::operator[](const std::string& name) const {
	if (_control.find(name) == _control.end()) {
		_logger.error("Attempted to test if non-existent control called \"{}\" "
			"was being triggered.", name);
		return false;
	}
	return _control.at(name).signal.signal;
}

bool sfx::user_input::isMouseButtonTriggeringControl(
	const std::string& name) const {
	if (_control.find(name) == _control.end()) {
		_logger.error("Attempted to test if non-existent control called \"{}\" "
			"was being triggered by a mouse button.", name);
		return false;
	}
	return _control.at(name).signal.triggeredByMouse;
}

sfx::KeyboardKeyList sfx::user_input::keyboardKeysBeingPressed() const {
	sfx::KeyboardKeyList ret;
	if (_window && !_window->hasFocus()) return ret;
	for (unsigned int k = 0; k < sf::Keyboard::KeyCount; ++k) {
		if (sf::Keyboard::isKeyPressed(sfx::convert::tokeycode(k, &_logger))) {
			ret.push_back(sfx::convert::tokeycode(k, &_logger));
		}
	}
	return ret;
}

sfx::MouseButtonList sfx::user_input::mouseButtonsBeingPressed() const {
	sfx::MouseButtonList ret;
	if (mousePosition() == INVALID_MOUSE) return ret;
	for (unsigned int b = 0; b < sf::Mouse::ButtonCount; ++b) {
		if (sf::Mouse::isButtonPressed(sfx::convert::tomousebtn(b, &_logger))) {
			ret.push_back(sfx::convert::tomousebtn(b, &_logger));
		}
	}
	return ret;
}

sfx::JoystickButtonList sfx::user_input::joystickButtonsBeingPressed() const {
	sfx::JoystickButtonList ret;
	if (_window && !_window->hasFocus()) return ret;
	for (unsigned int b = 0;
		b < sf::Joystick::getButtonCount(getJoystickID()); ++b) {
		if (sf::Joystick::isButtonPressed(getJoystickID(), b)) {
			ret.push_back(b);
		}
	}
	return ret;
}

sfx::JoystickAxisList sfx::user_input::joystickAxesBeingPressed() const {
	sfx::JoystickAxisList ret;
	if (_window && !_window->hasFocus()) return ret;
	for (unsigned int a = 0; a < sf::Joystick::AxisCount; ++a) {
		if (sf::Joystick::hasAxis(getJoystickID(),
			sfx::convert::toaxis(a, &_logger))) {
			if (sf::Joystick::getAxisPosition(getJoystickID(),
				sfx::convert::toaxis(a, &_logger)) >= getJoystickAxisThreshold()) {
				sfx::joystick item;
				item.axis = sfx::convert::toaxis(a, &_logger);
				item.direction = sfx::axis_direction::Positive;
				ret.push_back(item);
			} else if (sf::Joystick::getAxisPosition(getJoystickID(),
				sfx::convert::toaxis(a, &_logger)) <= -getJoystickAxisThreshold())
			{
				sfx::joystick item;
				item.axis = sfx::convert::toaxis(a, &_logger);
				item.direction = sfx::axis_direction::Negative;
				ret.push_back(item);
			}
		}
	}
	return ret;
}


void sfx::user_input::setGUI(const std::shared_ptr<const sfx::gui>& gui) {
	_gui = gui;
}

bool sfx::user_input::_load(engine::json& j) {
	std::unordered_map<std::string, sfx::user_control> control;
	nlohmann::ordered_json jj = j.nlohmannJSON();

	for (auto& i : jj.items()) {
		if (i.key() == "joystickid") {
			j.apply(_joystickid, { "joystickid" }, true);
			setJoystickID(getJoystickID());
		} else if (i.key() == "joystickaxis") {
			j.apply(_joystickAxisThreshold, { "joystickaxis" }, true);
			setJoystickAxisThreshold(getJoystickAxisThreshold());
		} else {
			try {
				if (i.value().find("keys") != i.value().end()) {
					for (auto k = i.value()["keys"].begin(),
						ek = i.value()["keys"].end(); k != ek; ++k) {
						control[i.key()].config.keyboard.push_back(
							sfx::convert::tokeycode(*k, &_logger)
						);
					}
				}
			} catch (const std::exception & e) {
				_logger.error("An error occurred when attempting to load keyboard "
					"controls for \"{}\": {}.", i.key(), e);
			}
			try {
				if (i.value().find("mouse") != i.value().end()) {
					for (auto m = i.value()["mouse"].begin(),
						em = i.value()["mouse"].end(); m != em; ++m) {
						control[i.key()].config.mouse.push_back(
							sfx::convert::tomousebtn(*m, &_logger)
						);
					}
				}
			} catch (const std::exception & e) {
				_logger.error("An error occurred when attempting to load mouse "
					"button controls for \"{}\": {}.", i.key(), e);
			}
			try {
				if (i.value().find("buttons") != i.value().end()) {
					for (auto b = i.value()["buttons"].begin(),
						eb = i.value()["buttons"].end(); b != eb; ++b) {
						control[i.key()].config.joystickButton.push_back(*b);
					}
				}
			} catch (const std::exception & e) {
				_logger.error("An error occurred when attempting to load joystick "
					"button controls for \"{}\": {}.", i.key(), e);
			}
			try {
				if (i.value().find("axes") != i.value().end()) {
					for (auto a = i.value()["axes"].begin(),
						ea = i.value()["axes"].end(); a != ea; ++a) {
						sfx::joystick item;
						item.axis = sfx::convert::toaxis((*a)[0], &_logger);
						item.direction = sfx::convert::toaxisdir((*a)[1]);
						control[i.key()].config.joystickAxis.push_back(item);
					}
				}
			} catch (const std::exception & e) {
				_logger.error("An error occurred when attempting to load joystick "
					"axis controls for \"{}\": {}.", i.key(), e);
			}

			auto sigtype = static_cast<unsigned int>(control[i.key()].signal.type);
			j.apply(sigtype, { i.key(), "type" }, true);
			control[i.key()].signal.type =
				sfx::convert::tosignaltype(sigtype, &_logger);
			if (control[i.key()].signal.type == sfx::control_signal::DelayedForm &&
				i.value().find("delays") != i.value().end()) {
				// If delays aren't given for a delayed-form control, they will be
				// generated in operator[].
				try {
					for (auto d = i.value()["delays"].begin(),
						ed = i.value()["delays"].end(); d != ed; ++d) {
						control[i.key()].signal.delayLength.push_back(
							sf::milliseconds(*d)
						);
					}
				} catch (const std::exception & e) {
					_logger.error("An error occurred when attempting to load "
						"delays for the delayed-form control \"{}\": {}.",
						i.key(), e);
				}
			}

			if (i.value().find("triggerviamouseevenifmouseisoverwidget") !=
				i.value().end()) {
				j.apply(
					control[i.key()].config.triggerViaMouseEvenIfMouseIsOverWidget,
					{ i.key(), "triggerviamouseevenifmouseisoverwidget" }, true);
			}
		}
	}

	_control = std::move(control);
	return true;
}

bool sfx::user_input::_save(nlohmann::ordered_json& j) {
	j["joystickid"] = getJoystickID();
	j["joystickaxis"] = getJoystickAxisThreshold();
	for (auto c = _control.begin(), ec = _control.end(); c != ec; ++c) {
		for (auto k = c->second.config.keyboard.begin(),
			ek = c->second.config.keyboard.end(); k != ek; ++k) {
			j[c->first]["keys"].push_back(*k);
		}
		for (auto m = c->second.config.mouse.begin(),
			em = c->second.config.mouse.end(); m != em; ++m) {
			j[c->first]["mouse"].push_back(*m);
		}
		for (auto b = c->second.config.joystickButton.begin(),
			eb = c->second.config.joystickButton.end(); b != eb; ++b) {
			j[c->first]["buttons"].push_back(*b);
		}
		for (auto a = c->second.config.joystickAxis.begin(),
			ea = c->second.config.joystickAxis.end(); a != ea; ++a) {
			std::array<int, 2> axisItem;
			axisItem[0] = a->axis;
			axisItem[1] = static_cast<int>(a->direction);
			j[c->first]["axes"].push_back(axisItem);
		}
		j[c->first]["type"] = c->second.signal.type;
		if (c->second.signal.delayLength.size()) {
			for (auto d = c->second.signal.delayLength.begin(),
				ed = c->second.signal.delayLength.end(); d != ed; ++d) {
				j[c->first]["delays"].push_back(d->asMilliseconds());
			}
		}
		j[c->first]["triggerviamouseevenifmouseisoverwidget"] =
			c->second.config.triggerViaMouseEvenIfMouseIsOverWidget;
	}
	return true;
}

bool sfx::user_input::_scanInput(sfx::user_configuration& ref) const {
	ref.keyboard = keyboardKeysBeingPressed();
	ref.mouse = mouseButtonsBeingPressed();
	ref.joystickButton = joystickButtonsBeingPressed();
	ref.joystickAxis = joystickAxesBeingPressed();
	return (ref.keyboard.size() || ref.mouse.size() || ref.joystickButton.size() ||
		ref.joystickAxis.size());
}

void sfx::user_input::_updateSingle(const sfx::user_configuration& scan,
	const std::string& name) {
	auto& signal = _control.at(name).signal;

	signal.previousTriggeredByMouse = signal.currentTriggeredByMouse;
	signal.currentTriggeredByMouse = false;
	signal.previous = signal.current;
	signal.current = false;

	if (_isBeingTriggered(_control.at(name).config.keyboard, scan.keyboard)) {
		signal.current = true;
	} else if (_isBeingTriggered(_control.at(name).config.joystickAxis,
		scan.joystickAxis)) {
		signal.current = true;
	} else if (_isBeingTriggered(_control.at(name).config.joystickButton,
		scan.joystickButton)) {
		signal.current = true;
	}
	if (_isBeingTriggered(_control.at(name).config.mouse, scan.mouse)) {
		signal.current = true;
		signal.currentTriggeredByMouse = true;
	}

	// Is a control being triggered by a mouse whilst it is over a widget?
	if (_gui && _gui->isWidgetUnderMouse() &&
		signal.currentTriggeredByMouse && !signal.previousTriggeredByMouse)
			signal.startedWhenMouseOverWidget = true;

	switch (signal.type) {
	case sfx::control_signal::FreeForm:
		signal.signal = signal.current;
		signal.triggeredByMouse = signal.signal && signal.currentTriggeredByMouse;
		break;
	case sfx::control_signal::ButtonForm:
		signal.signal = signal.previous && !signal.current;
		signal.triggeredByMouse = signal.signal && signal.previousTriggeredByMouse;
		break;
	case sfx::control_signal::DelayedForm:
		signal.signal = false;
		signal.triggeredByMouse = false;
		if (!signal.delayLength.size()) {
			signal.delayLength.push_back(sf::seconds(1.0f));
			signal.delayLength.push_back(sf::seconds(0.1f));
			_logger.warning("Attempted to use a delayed-form control \"{}\" "
				"without providing any delay lengths, [1000,100] provided.", name);
		}
		if (!signal.previous && signal.current) {
			signal.clock.restart();
			signal.delayIndex = 0;
			signal.signal = true;
		} else if (signal.current) {
			if (signal.clock.getElapsedTime() >=
				signal.delayLength[signal.delayIndex]) {
				signal.clock.restart();
				if (++signal.delayIndex >= signal.delayLength.size())
					signal.delayIndex = signal.delayLength.size() - 1;
				signal.signal = true;
			}
		}
		signal.triggeredByMouse = signal.signal && signal.currentTriggeredByMouse;
		break;
	}

	// If the input started when the mouse was over a widget, cancel the signal.
	// BUT, only perform this logic if the control's config says so.
	if (signal.startedWhenMouseOverWidget &&
		!_control.at(name).config.triggerViaMouseEvenIfMouseIsOverWidget) {
		signal.signal = false;
		signal.triggeredByMouse = false;
	}

	// Only cancel the flag once the control has been let go. Do it here since we
	// won't be able to cancel ButtonForm inputs if done before the switch
	// statement.
	if (signal.previousTriggeredByMouse && !signal.currentTriggeredByMouse)
		signal.startedWhenMouseOverWidget = false;
}
