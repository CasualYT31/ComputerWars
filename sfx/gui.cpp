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

using namespace tgui;

sfx::gui::gui_background::gui_background() noexcept {}

sfx::gui::gui_background::gui_background(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
	const std::string& key) noexcept {
	set(sheet, key);
}

sfx::gui::gui_background::gui_background(sf::Color colour) noexcept {
	set(colour);
}

void sfx::gui::gui_background::set(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
	const std::string& key) noexcept {
	_flag = sfx::gui::gui_background::type::Sprite;
	if (sheet) _bgSprite.setSpritesheet(sheet);
	_bgSprite.setSprite(key);
}

void sfx::gui::gui_background::set(sf::Color colour) noexcept {
	_flag = sfx::gui::gui_background::type::Colour;
	_bgColour.setFillColor(colour);
}

sfx::gui::gui_background::type sfx::gui::gui_background::getType() const noexcept {
	return _flag;
}

std::string sfx::gui::gui_background::getSprite() const noexcept {
	return _bgSprite.getSprite();
}

sf::Color sfx::gui::gui_background::getColour() const noexcept {
	return _bgColour.getFillColor();
}

bool sfx::gui::gui_background::animate(const sf::RenderTarget& target,
	const double scaling = 1.0) noexcept {
	if (_flag == sfx::gui::gui_background::type::Sprite) {
		return _bgSprite.animate(target, scaling);
	} else if (_flag == sfx::gui::gui_background::type::Colour) {
		_bgColour.setSize(
			sf::Vector2f(static_cast<float>(target.getSize().x),
				static_cast<float>(target.getSize().y))
		);
	}
	return false;
}

void sfx::gui::gui_background::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	switch (_flag) {
	case sfx::gui::gui_background::type::Sprite:
		target.draw(_bgSprite, states);
		break;
	case sfx::gui::gui_background::type::Colour:
		target.draw(_bgColour, states);
		break;
	}
}

sfx::gui::gui(const std::shared_ptr<engine::scripts>& scripts,
	const std::string& name) noexcept : _scripts(scripts), _logger(name) {
	if (!scripts) {
		_logger.error("No scripts object has been provided to this GUI object: no "
			"menus will be loaded.");
	} else {
		_scripts->registerInterface(
			std::bind(&sfx::gui::_registerInterface, this, std::placeholders::_1)
		);
	}
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
		if (_gui.get(old)) _gui.get(old)->setVisible(true);
	}
}

std::string sfx::gui::getGUI() const noexcept {
	return _currentGUI;
}

void sfx::gui::addSpritesheet(const std::string& name,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept {
	if (_sheet.find(name) != _sheet.end()) {
		_logger.warning("Updated the spritesheet named {}!", name);
	}
	_sheet[name] = sheet;
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
	_lastlang = "";
}

bool sfx::gui::animate(const sf::RenderTarget& target, const double scaling)
	noexcept {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		_guiBackground.at(getGUI()).animate(target, scaling);
	}

	_widgetPictures.clear();
	std::size_t animatedSprite = 0;

	if (getGUI() != "") {
		// lang == true if the language has been changed
		const bool lang = (_langdict && _langdict->getLanguage() != _lastlang);
		if (lang) _lastlang = _langdict->getLanguage();
		// go through each control and perform changes
		auto& widgetList = _gui.get<Group>(getGUI())->getWidgets();
		std::string guiName = getGUI() + ".";
		for (auto& widget : widgetList) {
			std::string widgetName =
				guiName + widget->getWidgetName().toStdString();
			String type = widget->getWidgetType();
			// if the widget deals with animated sprites then deal with it
			bool updateTexture = true;
			if (type == "BitmapButton" || type == "Picture") {
				if (_guiSpriteKeys.find(widgetName) == _guiSpriteKeys.end() ||
					_sheet.find(_guiSpriteKeys.at(widgetName).first) ==
					_sheet.end()) {
					updateTexture = false;
				} else {
					std::shared_ptr<sfx::animated_spritesheet> sheet =
						_sheet[_guiSpriteKeys[widgetName].first];
					if (animatedSprite == _widgetSprites.size()) {
						// animated sprite for this widget doesn't exist yet, so
						// allocate it
						_widgetSprites.emplace_back(
							sheet, _guiSpriteKeys[widgetName].second
						);
					}
					_widgetSprites[animatedSprite].animate(target, scaling);
					try {
						tgui::Texture tex;
						auto iRect = sheet->getFrameRect(
							_widgetSprites[animatedSprite].getSprite(),
							_widgetSprites[animatedSprite].getCurrentFrame()
						);
						tgui::UIntRect rect;
						rect.left = iRect.left;
						rect.top = iRect.top;
						rect.width = iRect.width;
						rect.height = iRect.height;
						tex.load(sheet->getTexture(), rect);
						_widgetPictures.push_back(tex);
					} catch (std::out_of_range&) {
						updateTexture = false;
					}
				}
			}
			// widget-specific code
			if (type == "Button") {
				auto w = _findWidget<Button>(widgetName);
				if (lang) {
					w->setText(
						(*_langdict)(_originalStrings[widgetName][0])
					);
				}
			} else if (type == "BitmapButton") {
				auto w = _findWidget<BitmapButton>(widgetName);
				if (lang) {
					w->setText(
						(*_langdict)(_originalStrings[widgetName][0])
					);
				}
				if (updateTexture) w->setImage(_widgetPictures[animatedSprite]);
			} else if (type == "Picture") {
				auto w = _findWidget<Picture>(widgetName);
				if (updateTexture) {
					auto newRenderer = tgui::PictureRenderer();
					newRenderer.setTexture(_widgetPictures[animatedSprite]);
					w->setRenderer(newRenderer.getData());
				}
			}
			animatedSprite++;
		}
	}
	// translate captions
	/*if (_langdict && getGUI() != "" && _langdict->getLanguage() != _lastlang) {
		_lastlang = _langdict->getLanguage();
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
			for (std::size_t i = 0; i < ptr->getItemCount(); i++) {
				ptr->changeItemByIndex(i,
					(*_langdict)(ptr->getItems().at(i).toStdString()));
			}
			});
		TRANSLATION_LOOP(ListBox, {
			for (std::size_t i = 0; i < ptr->getItemCount(); i++) {
				ptr->changeItemByIndex(i,
					(*_langdict)(ptr->getItems().at(i).toStdString()));
			}
			});
		// for a listview, we need to translate all the columns as well as each
		// item
		TRANSLATION_LOOP(ListView, {
			for (std::size_t i = 0; i < ptr->getColumnCount(); i++) {
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
			for (std::size_t i = 0; i < ptr->getTabsCount(); i++) {
				ptr->changeText(i,
					(*_langdict)(ptr->getText(i).toStdString()));
			}
			});
		TRANSLATION_LOOP(TabContainer, {
			for (std::size_t i = 0; i < ptr->getTabs()->getTabsCount(); i++) {
				ptr->changeTabText(i,
					(*_langdict)(ptr->getTabs()->getText(i).toStdString()));
			}
			});
		// we *also* need to translate treeviews
		// possible, but so not worth it....
		// it doesn't look like the TXT files even fully support tree views
		// anyway...
	} */

	return false;
}

void sfx::gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// draw background
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// this GUI has a background to animate
		target.draw(_guiBackground.at(getGUI()), states);
	}
	// draw foreground
	_gui.draw();
}

bool sfx::gui::_load(engine::json& j) noexcept {
	std::vector<std::string> names;
	j.applyVector(names, { "menus" });
	if (j.inGoodState()) {
		// clear state
		_gui.removeAllWidgets();
		_currentGUI = "";
		_guiBackground.clear();
		_widgetPictures.clear();
		_widgetSprites.clear();
		_guiSpriteKeys.clear();
		_originalStrings.clear();
		// create each menu
		for (auto& m : names) {
			tgui::Group::Ptr menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, m);
			if (_scripts) _scripts->callFunction(m + "SetUp");
		}
		return true;
	} else {
		return false;
	}
}

bool sfx::gui::_save(nlohmann::ordered_json& j) noexcept {
	return false;
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
		type == "spinbutton") { // || type == "spincontrol") {
		// okay... so... when I try to set ValueChanged on a SpinControl here, the
		// program crashes without reporting any errors whatsoever, not even in
		// debug mode... but the TGUI documentation says that it should have this
		// signal...
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

void sfx::gui::_registerInterface(asIScriptEngine* engine) noexcept {
	engine->RegisterGlobalFunction("void setGUI(const string& in)",
		asMETHODPR(sfx::gui, setGUI, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
}