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

#include "gui.h"

sfx::gui_background::gui_background() noexcept {}

sfx::gui_background::gui_background(unsigned int key) noexcept {
	set(key);
}

sfx::gui_background::gui_background(sf::Color colour) noexcept {
	set(colour);
}

void sfx::gui_background::set(unsigned int key) noexcept {
	_flag = sfx::gui_background::type::Sprite;
	_key = key;
}

void sfx::gui_background::set(sf::Color colour) noexcept {
	_flag = sfx::gui_background::type::Colour;
	_colour = colour;
}

sfx::gui_background::type sfx::gui_background::getType() const noexcept {
	return _flag;
}

unsigned int sfx::gui_background::getSprite() const noexcept {
	return _key;
}

sf::Color sfx::gui_background::getColour() const noexcept {
	return _colour;
}

sfx::gui::gui(const std::shared_ptr<engine::scripts>& scripts,
	const std::string& name) noexcept : _scripts(scripts), _logger(name) {
	if (!scripts) _logger.error("No scripts object has been provided to this GUI "
		"object: no signals will be handled.");
}

void sfx::gui::setGUI(const std::string& newPanel) noexcept {
	auto old = getGUI();
	if (_gui.get(old)) _gui.get(old)->setVisible(false);
	try {
		if (!_gui.get(newPanel)) throw tgui::Exception("GUI with name \"" +
			newPanel + "\" does not exist.");
		_gui.get(newPanel)->setVisible(true);
		// clear widget sprites
		_widgetSprites.clear();
		_currentGUI = newPanel;
	} catch (tgui::Exception& e) {
		_logger.error("{}", e.what());
		_gui.get(old)->setVisible(true);
	}
}

std::string sfx::gui::getGUI() const noexcept {
	return _currentGUI;
}

void sfx::gui::setSpritesheet(std::shared_ptr<sfx::animated_spritesheet> sheet)
	noexcept {
	_sheet = sheet;
}

void sfx::gui::setTarget(sf::RenderTarget& newTarget) noexcept {
	_gui.setTarget(newTarget);
}

bool sfx::gui::handleEvent(sf::Event e) noexcept {
	return _gui.handleEvent(e);
}

void sfx::gui::signalHandler(tgui::Widget::Ptr widget,
	const std::string& signalName) noexcept {
	std::string functionName = getGUI() + "_" + widget->getWidgetName() + "_" +
		signalName;
	if (_scripts && getGUI() != "" && _scripts->functionExists(functionName))
		_scripts->callFunction(functionName);
}

bool sfx::gui::animate(const sf::RenderTarget& target) noexcept {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// this GUI has a background to animate
		if (_guiBackground[getGUI()].getType() ==
			sfx::gui_background::type::Colour) {
			_bgColour.setSize(sf::Vector2f(target.getSize().x,
				target.getSize().y));
			_bgColour.setFillColor(_guiBackground[getGUI()].getColour());
		} else {
			_bgSprite.setSpritesheet(_sheet);
			_bgSprite.setSprite(_guiBackground[getGUI()].getSprite());
			_bgSprite.animate(target);
		}
	}

	_widgetPictures.clear();
	std::size_t i = 0;
	if (_sheet && getGUI() != "") {
		// update bitmapbutton and picture sprites
		auto& widgetList = _gui.get<tgui::Group>(getGUI())->getWidgets();
		for (auto& widget : widgetList) {
			if (widget->getWidgetType() == "BitmapButton" ||
				widget->getWidgetType() == "Picture") {
				if (i == _widgetSprites.size()) {
					// animated sprite doesn't yet exist, allocate it
					_widgetSprites.push_back(sfx::animated_sprite(_sheet,
						_guiSpriteKeys[getGUI()][widget->getWidgetName()]));
				}
				_widgetSprites[i].animate(target);
				try {
					_widgetPictures.push_back(tgui::Texture(_sheet->accessTexture(
						_widgetSprites[i].getCurrentFrame()),
						_sheet->accessSprite(_widgetSprites[i].getSprite())));
				} catch (std::out_of_range& e) {
					i++;
					continue;
				}

				// apply new texture
				if (widget->getWidgetType() == "BitmapButton") {
					auto bitmapbutton = _gui.get<tgui::BitmapButton>(
						widget->getWidgetName()
					);
					bitmapbutton->setImage(_widgetPictures[i]);
				} else {
					auto newRenderer = tgui::PictureRenderer();
					newRenderer.setTexture(_widgetPictures[i]);
					_gui.get<tgui::Picture>(widget->getWidgetName())->setRenderer(
						newRenderer.getData()
					);
				}

				i++;
			}
		}
	}

	return false;
}

void sfx::gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// draw background
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// this GUI has a background to animate
		if (_guiBackground.at(getGUI()).getType() ==
			sfx::gui_background::type::Colour) {
			target.draw(_bgColour);
		} else {
			target.draw(_bgSprite);
		}
	}
	// draw foreground
	_gui.draw();
}

bool sfx::gui::_load(engine::json& j) noexcept {
	bool ret = true;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		if (i.value().find("path") != i.value().end() &&
			i.value()["path"].is_string()) {
			if (!_loadGUI(i.key(), i.value()["path"])) ret = false;
		} else {
			_logger.write("Warning - GUI with the name \"{}\" did not refer to a "
				"valid string path within its required object value.", i.key());
		}
		if (i.value().find("background") != i.value().end()) {
			if (i.value()["background"].is_array()) {
				sf::Color newcolour = _guiBackground[i.key()].getColour();
				j.applyColour(newcolour, { i.key(), "background" }, &newcolour,
					true);
				_guiBackground[i.key()].set(newcolour);
			} else {
				unsigned int newkey = _guiBackground[i.key()].getSprite();
				j.apply(newkey, { i.key(), "background" }, &newkey, true);
				_guiBackground[i.key()].set(newkey);
			}
		} else {
			_logger.write("Warning - no background given for the GUI \"{}\": "
				"black background provided.", i.key());
			_guiBackground[i.key()].set(sf::Color(0,0,0,255));
		}

		nlohmann::ordered_json jjj = i.value();
		for (auto& ii : jjj.items()) {
			if (ii.key() == "path" || ii.key() == "background") continue;
			if (ii.value().find("sprite") != ii.value().end()) {
				j.apply(_guiSpriteKeys[i.key()][ii.key()], { i.key(), ii.key(),
					"sprite" }, &_guiSpriteKeys[i.key()][ii.key()], true);
			}
		}
	}
	return ret;
}

bool sfx::gui::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

bool sfx::gui::_loadGUI(const std::string& name, const std::string& filepath)
	noexcept {
	tgui::Gui temp;
	try {
		temp.loadWidgetsFromFile(filepath);
	} catch (tgui::Exception& e) {
		_logger.error("Error while loading GUI from {}: {}", filepath, e.what());
		return false;
	}
	auto group = tgui::Group::create();
	auto& widgetList = temp.getWidgets();
	auto itr = widgetList.begin();
	try {
		for (auto enditr = widgetList.end(); itr != enditr; itr++) {
			auto copy = (*itr)->clone();
			_connectSignals(copy);
			group->add(copy, (*itr)->getWidgetName());
		}
	} catch (tgui::Exception& e) {
		std::string widgetName = (*itr)->getWidgetName();
		_logger.error("Error while copying GUI widget with the name \"{}\" from "
			"GUI \"{}\": {}", widgetName, name, e.what());
		return false;
	}
	temp.removeAllWidgets();
	group->setVisible(false);
	_gui.add(group, name);
	return true;
}

// ALL SIGNALS NEED TO BE TESTED IDEALLY
void sfx::gui::_connectSignals(tgui::Widget::Ptr widget) noexcept {
	// connect common Widget signals
	widget->connect({"PositionChanged", "SizeChanged", "Focused", "Unfocused",
		"MouseEntered", "MouseLeft", "AnimationFinished"},
		&sfx::gui::signalHandler, this);
	// connect clickable widget signals
	tgui::String type = widget->getWidgetType(); type = type.toLower();
	if (type == "button" || type == "editbox" || type == "label" ||
		type == "picture" || type == "progressbar" || type == "radiobutton" ||
		type == "spinbutton" || type == "panel") {
		widget->connect({"MousePressed", "MouseReleased", "Clicked",
			"RightMousePressed", "RightMouseReleased", "RightClicked"},
			&sfx::gui::signalHandler, this);
	}
	// connect bespoke signals
	if (type == "button") {
		widget->connect({ "Pressed" }, &sfx::gui::signalHandler, this);
	} else if (type == "childwindow") {
		widget->connect({ "MousePressed", "Closed", "Minimized", "Maximized",
			"EscapeKeyPressed" }, &sfx::gui::signalHandler, this);
	} else if (type == "combobox") {
		widget->connect({ "ItemSelected" }, &sfx::gui::signalHandler, this);
	} else if (type == "editbox") {
		widget->connect({ "TextChanged", "ReturnKeyPressed" },
			&sfx::gui::signalHandler, this);
	} else if (type == "knob" || type == "scrollbar" || type == "slider" ||
		type == "spinbutton") {
		widget->connect({ "ValueChanged" }, &sfx::gui::signalHandler, this);
	} else if (type == "label" || type == "picture") {
		widget->connect({ "DoubleClicked" }, &sfx::gui::signalHandler, this);
	} else if (type == "listbox") {
		widget->connect({ "ItemSelected", "MousePressed", "MouseReleased",
			"DoubleClicked" }, &sfx::gui::signalHandler, this);
	} else if (type == "listview") {
		widget->connect({ "ItemSelected", "HeaderClicked", "RightClicked",
			"DoubleClicked" }, &sfx::gui::signalHandler, this);
	} else if (type == "menubar") {
		widget->connect({ "MenuItemClicked" }, &sfx::gui::signalHandler, this);
	} else if (type == "messagebox") {
		widget->connect({ "ButtonPressed" }, &sfx::gui::signalHandler, this);
	} else if (type == "progressbar") {
		widget->connect({ "ValueChanged", "Full" }, &sfx::gui::signalHandler,
			this);
	} else if (type == "radiobutton") {
		widget->connect({ "Checked", "Unchecked", "Changed" },
			&sfx::gui::signalHandler, this);
	} else if (type == "rangeslider") {
		widget->connect({ "RangeChanged" }, &sfx::gui::signalHandler, this);
	} else if (type == "tabs") {
		widget->connect({ "TabSelected" }, &sfx::gui::signalHandler, this);
	} else if (type == "textbox") {
		widget->connect({ "TextChanged", "SelectionChanged" },
			&sfx::gui::signalHandler, this);
	} else if (type == "treeview") {
		widget->connect({ "ItemSelected", "DoubleClicked", "Expanded",
			"Collapsed" }, &sfx::gui::signalHandler, this);
	}
}