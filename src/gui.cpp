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

#include "gui.h"
#include <iostream>

awe::gui_background::gui_background() noexcept {}

awe::gui_background::gui_background(unsigned int key) noexcept {
	set(key);
}

awe::gui_background::gui_background(sf::Color colour) noexcept {
	set(colour);
}

void awe::gui_background::set(unsigned int key) noexcept {
	_flag = awe::gui_background::type::Sprite;
	_key = key;
}

void awe::gui_background::set(sf::Color colour) noexcept {
	_flag = awe::gui_background::type::Colour;
	_colour = colour;
}

awe::gui_background::type awe::gui_background::getType() const noexcept {
	return _flag;
}

unsigned int awe::gui_background::getSprite() const noexcept {
	return _key;
}

sf::Color awe::gui_background::getColour() const noexcept {
	return _colour;
}

awe::gui::gui(awe::scripts* scripts, const std::string& name) noexcept : _scripts(scripts), _logger(name) {
	if (!scripts) _logger.error("No scripts object has been provided to this GUI object: no signal handling will occur.");
}

std::string awe::gui::setGUI(const std::string& newPanel) noexcept {
	auto old = getGUI();
	if (_gui.get(old)) _gui.get(old)->setVisible(false);
	try {
		if (!_gui.get(newPanel)) throw tgui::Exception("GUI with name \"" + newPanel + "\" does not exist.");
		_gui.get(newPanel)->setVisible(true);
	} catch (tgui::Exception& e) {
		_logger.error("{}", e.what());
		return old;
	}
	_currentGUI = newPanel;
	return old;
}

std::string awe::gui::getGUI() noexcept {
	return _currentGUI;
}

void awe::gui::setTarget(sf::RenderTarget& newTarget) noexcept {
	_gui.setTarget(newTarget);
}

bool awe::gui::handleEvent(sf::Event e) noexcept {
	return _gui.handleEvent(e);
}

void awe::gui::drawBackground(sfx::spritesheet* sprites) noexcept {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		if (_guiBackground[getGUI()].getType() == awe::gui_background::type::Colour) {
			_gui.getTarget()->clear(_guiBackground[getGUI()].getColour());
		} else if (sprites) {
			_gui.getTarget()->clear();
			sf::Texture sprite = (*sprites)[_guiBackground[getGUI()].getSprite()];
			_bgsprite.setTexture(sprite);
			_bgsprite.setScale((float) _gui.getTarget()->getSize().x / (float) sprite.getSize().x, (float) _gui.getTarget()->getSize().y / (float) sprite.getSize().y);
			_gui.getTarget()->draw(_bgsprite);
		} else {
			_gui.getTarget()->clear();
		}
	} else {
		_gui.getTarget()->clear();
	}
}

void awe::gui::drawForeground(sfx::spritesheet* sprites) noexcept {
	if (sprites && getGUI() != "") {
		//update bitmapbutton and picture sprites
		auto widgetList = _gui.get<tgui::Group>(getGUI())->getWidgets();
		for (auto itr = widgetList.begin(), enditr = widgetList.end(); itr != enditr; itr++) {
			auto widget = *itr;
			if (widget->getWidgetType() == "BitmapButton" || widget->getWidgetType() == "Picture") {
				//apply new texture
				if (widget->getWidgetType() == "BitmapButton") {
					auto bitmapbutton = _gui.get<tgui::BitmapButton>(widget->getWidgetName());
					bitmapbutton->setImage((*sprites)[_guiSpriteKeys[getGUI()][widget->getWidgetName()]]);
				} else {
					auto newRenderer = tgui::PictureRenderer();
					newRenderer.setTexture((*sprites)[_guiSpriteKeys[getGUI()][widget->getWidgetName()]]);
					_gui.get<tgui::Picture>(widget->getWidgetName())->setRenderer(newRenderer.getData());
				}
			}
		}
	}
	//draw
	_gui.draw();
}

void awe::gui::signalHandler(tgui::Widget::Ptr widget, const std::string& signalName) noexcept {
	std::string functionName = getGUI() + "_" + widget->getWidgetName() + "_" + signalName;
	if (_scripts && getGUI() != "" && _scripts->functionExists(functionName)) _scripts->callFunction(functionName);
}

bool awe::gui::_load(safe::json& j) noexcept {
	bool ret = true;
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		if (i.value().find("path") != i.value().end() && i.value()["path"].is_string()) {
			if (!_loadGUI(i.key(), i.value()["path"])) {
				ret = false;
				std::cout << "invalid!\n";
			}
		} else {
			_logger.write("Warning - GUI with the name \"{}\" did not refer to a valid string path within its required object value.", i.key());
		}
		if (i.value().find("background") != i.value().end()) {
			if (i.value()["background"].is_array()) {
				sf::Color newcolour = _guiBackground[i.key()].getColour();
				j.applyColour(newcolour, { i.key(), "background" }, &newcolour, true);
				_guiBackground[i.key()].set(newcolour);
			} else {
				unsigned int newkey = _guiBackground[i.key()].getSprite();
				j.apply(newkey, { i.key(), "background" }, &newkey, true);
				_guiBackground[i.key()].set(newkey);
			}
		} else {
			_logger.write("Warning - no background given for the GUI \"{}\": black background provided.", i.key());
			_guiBackground[i.key()].set(sf::Color(0,0,0,255));
		}

		nlohmann::json jjj = i.value();
		for (auto& ii : jjj.items()) {
			if (ii.key() == "path" || ii.key() == "background") continue;
			if (ii.value().find("sprite") != ii.value().end()) {
				j.apply(_guiSpriteKeys[i.key()][ii.key()], { i.key(), ii.key(), "sprite" }, &_guiSpriteKeys[i.key()][ii.key()], true);
			}
		}
	}
	return ret;
}

bool awe::gui::_save(nlohmann::json& j) noexcept {
	return false;
}

bool awe::gui::_loadGUI(const std::string& name, const std::string& filepath) noexcept {
	tgui::Gui temp;
	try {
		temp.loadWidgetsFromFile(filepath);
	} catch (tgui::Exception& e) {
		_logger.error("Error while loading GUI from {}: {}", filepath, e.what());
		return false;
	}
	auto group = tgui::Group::create();
	auto widgetList = temp.getWidgets();
	auto itr = widgetList.begin();
	try {
		for (auto enditr = widgetList.end(); itr != enditr; itr++) {
			auto copy = (*itr)->clone();
			_connectSignals(copy);
			group->add(copy, (*itr)->getWidgetName());
		}
	} catch (tgui::Exception& e) {
		std::string widgetName = (*itr)->getWidgetName();
		_logger.error("Error while copying GUI widget with the name \"{}\" from GUI \"{}\": {}", widgetName, name, e.what());
		return false;
	}
	temp.removeAllWidgets();
	group->setVisible(false);
	_gui.add(group, name);
	return true;
}

//ALL SIGNALS NEED TO BE TESTED IDEALLY
void awe::gui::_connectSignals(tgui::Widget::Ptr widget) noexcept {
	//connect common Widget signals
	widget->connect({"PositionChanged", "SizeChanged", "Focused", "Unfocused", "MouseEntered", "MouseLeft", "AnimationFinished"}, &awe::gui::signalHandler, this);
	//connect clickable widget signals
	tgui::String type = widget->getWidgetType(); type = type.toLower();
	if (type == "button" || type == "editbox" || type == "label" || type == "picture" || type == "progressbar" || type == "radiobutton" || type == "spinbutton" || type == "panel") {
		widget->connect({"MousePressed", "MouseReleased", "Clicked", "RightMousePressed", "RightMouseReleased", "RightClicked"}, &awe::gui::signalHandler, this);
	}
	//connect bespoke signals
	if (type == "button") {
		widget->connect({ "Pressed" }, &awe::gui::signalHandler, this);
	} else if (type == "childwindow") {
		widget->connect({ "MousePressed", "Closed", "Minimized", "Maximized", "EscapeKeyPressed" }, &awe::gui::signalHandler, this);
	} else if (type == "combobox") {
		widget->connect({ "ItemSelected" }, &awe::gui::signalHandler, this);
	} else if (type == "editbox") {
		widget->connect({ "TextChanged", "ReturnKeyPressed" }, &awe::gui::signalHandler, this);
	} else if (type == "knob" || type == "scrollbar" || type == "slider" || type == "spinbutton") {
		widget->connect({ "ValueChanged" }, &awe::gui::signalHandler, this);
	} else if (type == "label" || type == "picture") {
		widget->connect({ "DoubleClicked" }, &awe::gui::signalHandler, this);
	} else if (type == "listbox") {
		widget->connect({ "ItemSelected", "MousePressed", "MouseReleased", "DoubleClicked" }, &awe::gui::signalHandler, this);
	} else if (type == "listview") {
		widget->connect({ "ItemSelected", "HeaderClicked", "RightClicked", "DoubleClicked" }, &awe::gui::signalHandler, this);
	} else if (type == "menubar") {
		widget->connect({ "MenuItemClicked" }, &awe::gui::signalHandler, this);
	} else if (type == "messagebox") {
		widget->connect({ "ButtonPressed" }, &awe::gui::signalHandler, this);
	} else if (type == "progressbar") {
		widget->connect({ "ValueChanged", "Full" }, &awe::gui::signalHandler, this);
	} else if (type == "radiobutton") {
		widget->connect({ "Checked", "Unchecked", "Changed" }, &awe::gui::signalHandler, this);
	} else if (type == "rangeslider") {
		widget->connect({ "RangeChanged" }, &awe::gui::signalHandler, this);
	} else if (type == "tabs") {
		widget->connect({ "TabSelected" }, &awe::gui::signalHandler, this);
	} else if (type == "textbox") {
		widget->connect({ "TextChanged", "SelectionChanged" }, &awe::gui::signalHandler, this);
	} else if (type == "treeview") {
		widget->connect({ "ItemSelected", "DoubleClicked", "Expanded", "Collapsed" }, &awe::gui::signalHandler, this);
	}
}