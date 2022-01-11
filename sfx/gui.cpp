/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**
 * Used to loop through all of a given type of widget for translation purposes.
 * The \c ptr variable is used to access each widget of the given type in a loop.
 * @param t The type of widget to effect. Must be a TGUI C++ class name.
 * @param b The statement/s to execute within the loop.
 */
#define TRANSLATION_LOOP(t, b) \
	for (tgui::t::Ptr ptr = _getNextWidget<tgui::t>(#t); ptr; \
		ptr = _getNextWidget<tgui::t>(#t)) { \
		b; \
	}

sfx::gui::gui_background::gui_background() noexcept {}

sfx::gui::gui_background::gui_background(const std::string& key) noexcept {
	set(key);
}

sfx::gui::gui_background::gui_background(sf::Color colour) noexcept {
	set(colour);
}

void sfx::gui::gui_background::set(const std::string& key) noexcept {
	_flag = sfx::gui::gui_background::type::Sprite;
	_key = key;
}

void sfx::gui::gui_background::set(sf::Color colour) noexcept {
	_flag = sfx::gui::gui_background::type::Colour;
	_colour = colour;
}

sfx::gui::gui_background::type sfx::gui::gui_background::getType() const noexcept {
	return _flag;
}

std::string sfx::gui::gui_background::getSprite() const noexcept {
	return _key;
}

sf::Color sfx::gui::gui_background::getColour() const noexcept {
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
		// reset _getNextWidget index field
		_widgetIndex = 0;
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
	const tgui::String& signalName) noexcept {
	std::string functionName = getGUI() + "_" +
		widget->getWidgetName().toStdString() + "_" + signalName.toStdString();
	if (_scripts && getGUI() != "" && _scripts->functionExists(functionName))
		_scripts->callFunction(functionName);
}

void sfx::gui::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& lang) noexcept {
	_langdict = lang;
}

bool sfx::gui::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// this GUI has a background to animate
		if (_guiBackground[getGUI()].getType() ==
			sfx::gui::gui_background::type::Colour) {
			_bgColour.setSize(sf::Vector2f(static_cast<float>(target.getSize().x),
				static_cast<float>(target.getSize().y)));
			_bgColour.setFillColor(_guiBackground[getGUI()].getColour());
		} else {
			_bgSprite.setSpritesheet(_sheet);
			_bgSprite.setSprite(_guiBackground[getGUI()].getSprite());
			_bgSprite.animate(target, scaling);
		}
	}

	_widgetPictures.clear();
	std::size_t i = 0;
	if (_sheet && getGUI() != "") {
		// translate captions
		if (_langdict) {
			// translate captions
			TRANSLATION_LOOP(Button,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(BitmapButton,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(ToggleButton,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(Label,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(ProgressBar,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(RadioButton,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(CheckBox,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			TRANSLATION_LOOP(MessageBox,
				ptr->setText((*_langdict)(ptr->getText().toStdString())));
			// for other types of controls, we need to iterate through a list
			TRANSLATION_LOOP(ComboBox, {
				for (std::size_t i = 0; i <= ptr->getItemCount(); i++) {
					ptr->changeItemByIndex(i,
						(*_langdict)(ptr->getItems().at(i).toStdString()));
				}
				});
			TRANSLATION_LOOP(ListBox, {
				for (std::size_t i = 0; i <= ptr->getItemCount(); i++) {
					ptr->changeItemByIndex(i,
						(*_langdict)(ptr->getItems().at(i).toStdString()));
				}
				});
			// for a listview, we need to translate all the columns as well as each
			// item
			TRANSLATION_LOOP(ListView, {
				for (std::size_t i = 0; i <= ptr->getColumnCount(); i++) {
					ptr->setColumnText(i,
						(*_langdict)(ptr->getColumnText(i).toStdString()));
					for (std::size_t j = 0; i <= ptr->getItemCount(); j++) {
						ptr->changeSubItem(j, i,
							(*_langdict)(ptr->getItemCell(j, i).toStdString()));
					}
				}
				});
			// for container types, we need to translate the title instead of the
			// "text" (sometimes as well as the "text")
			TRANSLATION_LOOP(ChildWindow,
				ptr->setTitle((*_langdict)(ptr->getTitle().toStdString())));
			TRANSLATION_LOOP(MessageBox,
				ptr->setTitle((*_langdict)(ptr->getTitle().toStdString())));
			TRANSLATION_LOOP(ColorPicker,
				ptr->setTitle((*_langdict)(ptr->getTitle().toStdString())));
			TRANSLATION_LOOP(FileDialog,
				ptr->setTitle((*_langdict)(ptr->getTitle().toStdString())));
			// we also need to translate menus
			// damn, that's not even possible as of TGUI 0.8.9...

			// we also need to translate tabs
			TRANSLATION_LOOP(Tabs, {
				for (std::size_t i = 0; i <= ptr->getTabsCount(); i++) {
					ptr->changeText(i,
						(*_langdict)(ptr->getText(i).toStdString()));
				}
				});
			TRANSLATION_LOOP(TabContainer, {
				for (std::size_t i = 0; i <= ptr->getTabs()->getTabsCount(); i++) {
					ptr->changeTabText(i,
						(*_langdict)(ptr->getTabs()->getText(i).toStdString()));
				}
				});
			// we *also* need to translate treeviews
			// possible, but so not worth it....
		}

		auto& widgetList = _gui.get<tgui::Group>(getGUI())->getWidgets();
		for (auto& widget : widgetList) {
			// update bitmapbutton and picture sprites
			if (widget->getWidgetType() == "BitmapButton" ||
				widget->getWidgetType() == "Picture") {
				if (i == _widgetSprites.size()) {
					// animated sprite doesn't yet exist, allocate it
					_widgetSprites.push_back(sfx::animated_sprite(_sheet,
						_guiSpriteKeys[getGUI()]
						[widget->getWidgetName().toStdString()]));
				}
				_widgetSprites[i].animate(target, scaling);
				try {
					tgui::Texture tex;
					auto iRect =
						_sheet->getFrameRect(_widgetSprites[i].getSprite(),
							_widgetSprites[i].getCurrentFrame());
					tgui::UIntRect rect;
					rect.left = iRect.left;
					rect.top = iRect.top;
					rect.width = iRect.width;
					rect.height = iRect.height;
					tex.load(_sheet->getTexture(), rect);
					_widgetPictures.push_back(tex);
				} catch (std::out_of_range&) {
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
			sfx::gui::gui_background::type::Colour) {
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
				j.applyColour(newcolour, { i.key(), "background" }, true);
				_guiBackground[i.key()].set(newcolour);
			} else {
				std::string newkey = _guiBackground[i.key()].getSprite();
				j.apply(newkey, { i.key(), "background" }, true);
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
					"sprite" }, true);
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
		std::string widgetName = (*itr)->getWidgetName().toStdString();
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
	widget->getSignal("PositionChanged").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("SizeChanged").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("Focused").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("Unfocused").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("MouseEntered").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("MouseLeft").
		connectEx(&sfx::gui::signalHandler, this);
	widget->getSignal("AnimationFinished").
		connectEx(&sfx::gui::signalHandler, this);
	// connect clickable widget signals
	tgui::String type = widget->getWidgetType().toLower();
	if (type == "button" || type == "editbox" || type == "label" ||
		type == "picture" || type == "progressbar" || type == "radiobutton" ||
		type == "spinbutton" || type == "panel") {
		widget->getSignal("MousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("MouseReleased").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Clicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("RightMousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("RightMouseReleased").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("RightClicked").
			connectEx(&sfx::gui::signalHandler, this);
	}
	// connect bespoke signals
	if (type == "button") {
		widget->getSignal("Pressed").connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "childwindow") {
		widget->getSignal("MousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Closed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Minimized").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Maximized").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("EscapeKeyPressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Closing").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "colorpicker") {
		widget->getSignal("ColorChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("OkPress").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "combobox") {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "editbox") {
		widget->getSignal("TextChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("ReturnKeyPressed").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "filedialog") {
		widget->getSignal("FileSelected").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "knob" || type == "scrollbar" || type == "slider" ||
		type == "spinbutton" || type == "spincontrol") {
		widget->getSignal("ValueChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "label" || type == "picture") {
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "listbox") {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("MousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("MouseReleased").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "listview") {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("HeaderClicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("RightClicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "menubar") {
		widget->getSignal("MenuItemClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "messagebox") {
		widget->getSignal("ButtonPressed").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "panel") {
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "progressbar") {
		widget->getSignal("ValueChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Full").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "radiobutton") {
		widget->getSignal("Checked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Unchecked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Changed").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "rangeslider") {
		widget->getSignal("RangeChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "tabcontainer") {
		widget->getSignal("SeletionChanging").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("SelectionChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "tabs") {
		widget->getSignal("TabSelected").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "textarea") {
		widget->getSignal("TextChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("SelectionChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "togglebutton") {
		widget->getSignal("Checked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "treeview") {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Expanded").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Collapsed").
			connectEx(&sfx::gui::signalHandler, this);
	}
}