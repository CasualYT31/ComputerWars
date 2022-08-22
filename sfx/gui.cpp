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
	const double scaling) noexcept {
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
		_scripts->addRegistrant(this);
	}
}

void sfx::gui::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	// Register non-widget global functions.
	int r = engine->RegisterGlobalFunction("void setGUI(const string& in)",
		asMETHODPR(sfx::gui, setGUI, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Hides the current menu and shows the "
		"menu given.");
	engine->RegisterGlobalFunction("void setBackground(string)",
		asMETHOD(sfx::gui, _noBackground), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBackground(string, const "
		"string& in, const string& in)",
		asMETHOD(sfx::gui, _spriteBackground), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBackground(string, const uint, "
		"const uint, const uint, const uint)",
		asMETHOD(sfx::gui, _colourBackground), asCALL_THISCALL_ASGLOBAL, this);
	// Register widget global functions.
	engine->RegisterGlobalFunction("void addWidget(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _addWidget), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setWidgetPosition(const string& in, "
		"const string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetPosition), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setWidgetSize(const string& in, const "
		"string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetSize), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setWidgetText(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _setWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setWidgetSprite(const string& in, const "
		"string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetSprite), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void addItem(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _addItem), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("string getSelectedItemText(const string& in)",
		asMETHOD(sfx::gui, _getSelectedItemText), asCALL_THISCALL_ASGLOBAL, this);
}

void sfx::gui::setGUI(const std::string& newPanel) noexcept {
	auto old = getGUI();
	if (_gui.get(old)) {
		_gui.get(old)->setVisible(false);
	}
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

void sfx::gui::handleInput(const std::shared_ptr<sfx::user_input>& ui) noexcept {
	if (ui) {
		if (_scripts->functionExists(getGUI() + "HandleInput")) {
			_handleInputErrorLogged = false;
			// Construct the dictionary.
			CScriptDictionary* controls = _scripts->createDictionary();
			auto controlKeys = ui->getControls();
			for (auto& key : controlKeys)
				controls->Set(key, (asINT64)ui->operator[](key));
			// Invoke the function.
			_scripts->callFunction(getGUI() + "HandleInput", controls);
			controls->Release();
		}
	} else if (!_handleInputErrorLogged) {
		_logger.error("Called handleInput() with nullptr user_input object for "
			"menu \"{}\"!", getGUI());
		_handleInputErrorLogged = true;
	}
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

	if (getGUI() != "") {
		if (_langdict && _langdict->getLanguage() != _lastlang) {
			_lastlang = _langdict->getLanguage();
			auto& widgetList = _gui.getWidgets();
			for (auto& widget : widgetList) {
				auto w = _gui.get<tgui::Container>(widget->getWidgetName());
				_translateWidgets(w, widget->getWidgetName().toStdString());
			}
		}
		std::size_t animatedSprite = 0;
		_animate(target, scaling, _gui.get<Container>(getGUI()), getGUI(),
			animatedSprite);
	}

	return false;
}

void sfx::gui::_animate(const sf::RenderTarget& target, const double scaling,
	tgui::Container::Ptr container, std::string baseName,
	std::size_t& animatedSprite) noexcept {
	auto& widgetList = container->getWidgets();
	baseName += ".";
	for (auto& widget : widgetList) {
		std::string widgetName = baseName + widget->getWidgetName().toStdString();
		String type = widget->getWidgetType();
		// If the widget deals with animated sprites then handle them.
		bool updateTexture = true;
		if (type == "BitmapButton" || type == "Picture") {
			if (_guiSpriteKeys.find(widgetName) == _guiSpriteKeys.end() ||
				_sheet.find(_guiSpriteKeys.at(widgetName).first) == _sheet.end()) {
				updateTexture = false;
			} else {
				std::shared_ptr<sfx::animated_spritesheet> sheet =
					_sheet[_guiSpriteKeys[widgetName].first];
				if (animatedSprite == _widgetSprites.size()) {
					// Animated sprite for this widget doesn't exist yet, so
					// allocate it.
					_widgetSprites.emplace_back(
						sheet, _guiSpriteKeys[widgetName].second
					);
				}
				_widgetSprites[animatedSprite].animate(target, scaling);
				try {
					auto iRect = sheet->getFrameRect(
						_widgetSprites[animatedSprite].getSprite(),
						_widgetSprites[animatedSprite].getCurrentFrame()
					);
					tgui::UIntRect rect;
					rect.left = iRect.left;
					rect.top = iRect.top;
					rect.width = iRect.width;
					rect.height = iRect.height;
					_widgetPictures.emplace_back();
					_widgetPictures.back().load(sheet->getTexture(), rect);
				} catch (std::out_of_range&) {
					updateTexture = false;
					// Remove widget's sprite if its picture couldn't be allocated.
					_widgetSprites.pop_back();
				}
			}
		}
		// Widget-specific code.
		if (type == "Button") {
			// auto w = _findWidget<Button>(widgetName);
		} else if (type == "BitmapButton") {
			auto w = _findWidget<BitmapButton>(widgetName);
			if (updateTexture) w->setImage(_widgetPictures[animatedSprite]);
		} else if (type == "CheckBox") {
			// auto w = _findWidget<CheckBox>(widgetName);
		} else if (type == "ChildWindow") {
			// auto w = _findWidget<ChildWindow>(widgetName);
		} else if (type == "ColorPicker") {
			// auto w = _findWidget<ColorPicker>(widgetName);
		} else if (type == "ComboBox") {
			// auto w = _findWidget<ComboBox>(widgetName);
		} else if (type == "FileDialog") {
			// auto w = _findWidget<FileDialog>(widgetName);
		} else if (type == "Label") {
			// auto w = _findWidget<Label>(widgetName);
		} else if (type == "ListBox") {
			// auto w = _findWidget<ListBox>(widgetName);
		} else if (type == "ListView") {
			// auto w = _findWidget<ListView>(widgetName);
		} else if (type == "Picture") {
			auto w = _findWidget<Picture>(widgetName);
			if (updateTexture) {
				auto newRenderer = tgui::PictureRenderer();
				newRenderer.setTexture(_widgetPictures[animatedSprite]);
				w->setRenderer(newRenderer.getData());
			}
		} else if (type == "MenuBar") {
			// auto w = _findWidget<MenuBar>(widgetName);
		} else if (type == "MessageBox") {
			// auto w = _findWidget<MessageBox>(widgetName);
		} else if (type == "ProgressBar") {
			// auto w = _findWidget<ProgressBar>(widgetName);
		} else if (type == "RadioButton") {
			// auto w = _findWidget<RadioButton>(widgetName);
		} else if (type == "TabContainer") {
			// auto w = _findWidget<TabContainer>(widgetName);
		} else if (type == "Tabs") {
			// auto w = _findWidget<Tabs>(widgetName);
		} else if (type == "ToggleButton") {
			// auto w = _findWidget<ToggleButton>(widgetName);
		}
		// Container types - not all of them are here for future reference.
		if (type == "ChildWindow" || type == "Grid" || type == "Group" ||
			type == "RadioButtonGroup" || type == "VerticalLayout") {
			auto w = _findWidget<Container>(widgetName);
			_animate(target, scaling, w, widgetName, animatedSprite);
		}
		animatedSprite++;
	}
}

void sfx::gui::_translateWidgets(tgui::Container::Ptr container,
	std::string baseName) noexcept {
	auto& widgetList = container->getWidgets();
	baseName += ".";
	for (auto& widget : widgetList) {
		std::string widgetName = baseName + widget->getWidgetName().toStdString();
		String type = widget->getWidgetType();
		if (type == "Button") {
			auto w = _findWidget<Button>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "BitmapButton") {
			auto w = _findWidget<BitmapButton>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "CheckBox") {
			auto w = _findWidget<CheckBox>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "ChildWindow") {
			auto w = _findWidget<ChildWindow>(widgetName);
			w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "ColorPicker") {
			auto w = _findWidget<ColorPicker>(widgetName);
			w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "ComboBox") {
			auto w = _findWidget<ComboBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); i++) {
				w->changeItemByIndex(i,
					(*_langdict)(_originalStrings[widgetName][i]));
			}
		} else if (type == "FileDialog") {
			auto w = _findWidget<FileDialog>(widgetName);
			w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "Label") {
			auto w = _findWidget<Label>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "ListBox") {
			auto w = _findWidget<ListBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); i++) {
				w->changeItemByIndex(i,
					(*_langdict)(_originalStrings[widgetName][i]));
			}
		} else if (type == "ListView") {
			auto w = _findWidget<ListView>(widgetName);
			std::size_t colCount = w->getColumnCount();
			for (std::size_t i = 0; i < colCount; i++) {
				w->setColumnText(i, (*_langdict)(_originalStrings[widgetName][i]));
				for (std::size_t j = 0; j <= w->getItemCount(); j++) {
					w->changeSubItem(i, j, (*_langdict)
						(_originalStrings[widgetName][colCount * (i + 1) + j])
					);
				}
			}
		} else if (type == "MenuBar") {
			auto w = _findWidget<MenuBar>(widgetName);
			// It's possible, but we would somehow need to store the menu hierarchy
			// separately to keep this as simple as possible. Potentially multiple
			// menu hierarchies would have to be stored, though...
		} else if (type == "MessageBox") {
			auto w = _findWidget<MessageBox>(widgetName);
			w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
			w->setText((*_langdict)(_originalStrings[widgetName][1]));
			// Don't know how I'm going to translate buttons.
		} else if (type == "ProgressBar") {
			auto w = _findWidget<ProgressBar>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "RadioButton") {
			auto w = _findWidget<RadioButton>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		} else if (type == "TabContainer") {
			auto w = _findWidget<TabContainer>(widgetName);
			for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); i++) {
				w->changeTabText(i, (*_langdict)(_originalStrings[widgetName][i]));
			}
		} else if (type == "Tabs") {
			auto w = _findWidget<Tabs>(widgetName);
			for (std::size_t i = 0; i < w->getTabsCount(); i++) {
				w->changeText(i, (*_langdict)(_originalStrings[widgetName][i]));
			}
		} else if (type == "ToggleButton") {
			auto w = _findWidget<ToggleButton>(widgetName);
			w->setText((*_langdict)(_originalStrings[widgetName][0]));
		}
		// Container types - not all of them are here for future reference.
		if (type == "ChildWindow" || type == "Grid" || type == "Group" ||
			type == "RadioButtonGroup" || type == "VerticalLayout") {
			auto w = _findWidget<Container>(widgetName);
			_translateWidgets(w, widgetName);
		}
	}
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
		// Clear state.
		_gui.removeAllWidgets();
		_guiBackground.clear();
		_widgetPictures.clear();
		_widgetSprites.clear();
		_guiSpriteKeys.clear();
		_originalStrings.clear();
		// Create the main menu that always exists.
		tgui::Group::Ptr menu = tgui::Group::create();
		menu->setVisible(false);
		_gui.add(menu, "MainMenu");
		setGUI("MainMenu");
		if (_scripts) _scripts->callFunction("MainMenuSetUp");
		// Create each menu.
		for (auto& m : names) {
			menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, m);
			// Temporarily set the current GUI to this one to make _findWidget()
			// work with relative widget names in SetUp() functions.
			setGUI(m);
			if (_scripts) _scripts->callFunction(m + "SetUp");
		}
		// Leave with the current menu being MainMenu.
		setGUI("MainMenu");
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
	// Connect common widget signals.
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
		type == "spinbutton" || type == "panel" || type == "bitmapbutton") {
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
	if (type == "button" || type == "bitmapbutton") {
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

void sfx::gui::_noBackground(std::string menu) noexcept {
	if (menu == "") menu = getGUI();
	_guiBackground.erase(menu);
}

void sfx::gui::_spriteBackground(std::string menu, const std::string& sheet,
	const std::string& sprite) noexcept {
	if (menu == "") menu = getGUI();
	try {
		_guiBackground[menu].set(_sheet.at(sheet), sprite);
	} catch (std::out_of_range&) {
		_logger.error("Attempted to set sprite \"{}\" from sheet \"{}\" to the "
			"background of menu \"{}\". The sheet does not exist!", sprite, sheet,
			menu);
	}
}

void sfx::gui::_colourBackground(std::string menu, const unsigned int r,
	const unsigned int g, const unsigned int b, const unsigned int a) noexcept {
	if (menu == "") menu = getGUI();
	_guiBackground[menu].set(sf::Color(r, g, b, a));
}

void sfx::gui::_addWidget(const std::string& widgetType, const std::string& name)
	noexcept {
	std::string type = String(widgetType).toLower().toStdString();
	std::vector<std::string> fullname;
	if (_findWidget<Widget>(name, &fullname)) {
		_logger.error("Attempted to create a new \"{}\" widget with name \"{}\": "
			"a widget with that name already exists!", type, name);
	} else {
		tgui::Widget::Ptr widget;
		if (type == "bitmapbutton") {
			widget = tgui::BitmapButton::create();
		} else if (type == "listbox") {
			widget = tgui::ListBox::create();
		} else if (type == "verticallayout") {
			widget = tgui::VerticalLayout::create();
		} else {
			_logger.error("Attempted to create a widget of type \"{}\" with name "
				"\"{}\" for menu \"{}\": that widget type is not supported.", type,
				name, fullname[0]);
			return;
		}
		auto container = _gui.get<Container>(fullname[0]);
		if (!container) {
			_logger.error("Attempted to add a \"{}\" widget called \"{}\" to the "
				"menu \"{}\". This menu does not exist.", name, fullname[0]);
			return;
		}
		if (fullname.size() > 2) {
			for (std::size_t i = 1; i < fullname.size() - 1; i++) {
				if (!container) {
					_logger.error("Attempted to add a \"{}\" widget called \"{}\" "
						"to the container \"{}\" within menu \"{}\". This "
						"container does not exist.", type, name, fullname[i - 1],
						fullname[0]);
					return;
				}
				container = container->get<Container>(fullname[i]);
			}
		}
		container->add(widget, fullname.back());
		_connectSignals(widget);
	}
}

void sfx::gui::_setWidgetPosition(const std::string& name, const std::string& x,
	const std::string& y) noexcept {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setPosition(x.c_str(), y.c_str());
	} else {
		_logger.error("Attempted to set the position (\"{}\",\"{}\") to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", x, y, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetSize(const std::string& name, const std::string& w,
	const std::string& h) noexcept {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setSize(w.c_str(), h.c_str());
	} else {
		_logger.error("Attempted to set the size (\"{}\",\"{}\") to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", w, h, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetText(const std::string& name, const std::string& text)
	noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "bitmapbutton") {
			_findWidget<BitmapButton>(name)->setText(text);
		} else {
			_logger.error("Attempted to set the caption \"{}\" to widget \"{}\" "
				"which is of type \"{}\", within menu \"{}\". This operation is "
				"not supported for this type of widget.", text, name, type,
				fullname[0]);
			return;
		}
		// Store the item's text in the _originalStrings container.
		if (_originalStrings[fullnameAsString].size() == 0) {
			_originalStrings[fullnameAsString].push_back(text);
		} else {
			_originalStrings[fullnameAsString][0] = text;
		}
	} else {
		_logger.error("Attempted to set the caption \"{}\" to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", text, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetSprite(const std::string& name, const std::string& sheet,
	const std::string& key) noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type != "bitmapbutton") {
			_logger.error("Attempted to set the sprite \"{}\" from sheet \"{}\" "
				"to widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", key,
				sheet, name, type, fullname[0]);
			return;
		}
		_guiSpriteKeys[fullnameAsString] = std::make_pair(sheet, key);
	} else {
		_logger.error("Attempted to set the sprite \"{}\" from sheet \"{}\" to a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.", key,
			sheet, name, fullname[0]);
	}
}

void sfx::gui::_addItem(const std::string& name, const std::string& text)
	noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		// Add the item differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			_findWidget<ListBox>(name)->addItem(text);
		} else {
			_logger.error("Attempted to add an item \"{}\" to widget \"{}\" which "
				"is of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", text, name, type,
				fullname[0]);
			return;
		}
		// Store the item's text in the _originalStrings container.
		_originalStrings[fullnameAsString].push_back(text);
	} else {
		_logger.error("Attempted to add a new item \"{}\" to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", text, name,
			fullname[0]);
	}
}

std::string sfx::gui::_getSelectedItemText(const std::string& name) noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		// Get the item text differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			return _findWidget<ListBox>(name)->getSelectedItem().toStdString();
		} else {
			_logger.error("Attempted to get the text of the selected item of a "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", name, type,
				fullname[0]);
		}
	} else {
		_logger.error("Attempted to get the text of the selected item of a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", name,
			fullname[0]);
	}
	return "";
}