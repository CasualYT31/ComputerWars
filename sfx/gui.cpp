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

	if (getGUI() != "") {
		// lang == true if the language has been changed
		const bool lang = (_langdict && _langdict->getLanguage() != _lastlang);
		if (lang) _lastlang = _langdict->getLanguage();
		std::size_t animatedSprite = 0;
		_animate(target, scaling, _gui.get<Container>(getGUI()), getGUI(), lang,
			animatedSprite);
	}

	return false;
}

void sfx::gui::_animate(const sf::RenderTarget& target, const double scaling,
	tgui::Container::Ptr container, std::string baseName, const bool lang,
	std::size_t& animatedSprite) noexcept {
	auto& widgetList = container->getWidgets();
	baseName += ".";
	for (auto& widget : widgetList) {
		std::string widgetName = baseName + widget->getWidgetName().toStdString();
		String type = widget->getWidgetType();
		// if the widget deals with animated sprites then handle them
		bool updateTexture = true;
		if (type == "BitmapButton" || type == "Picture") {
			if (_guiSpriteKeys.find(widgetName) == _guiSpriteKeys.end() ||
				_sheet.find(_guiSpriteKeys.at(widgetName).first) == _sheet.end()) {
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
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "BitmapButton") {
			auto w = _findWidget<BitmapButton>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
			if (updateTexture) w->setImage(_widgetPictures[animatedSprite]);
		} else if (type == "CheckBox") {
			auto w = _findWidget<CheckBox>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "ChildWindow") {
			auto w = _findWidget<ChildWindow>(widgetName);
			if (lang) {
				w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "ColorPicker") {
			auto w = _findWidget<ColorPicker>(widgetName);
			if (lang) {
				w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "ComboBox") {
			auto w = _findWidget<ComboBox>(widgetName);
			if (lang) {
				for (std::size_t i = 0; i < w->getItemCount(); i++) {
					w->changeItemByIndex(i,
						(*_langdict)(_originalStrings[widgetName][i]));
				}
			}
		} else if (type == "FileDialog") {
			auto w = _findWidget<FileDialog>(widgetName);
			if (lang) {
				w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "Label") {
			auto w = _findWidget<Label>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "ListBox") {
			auto w = _findWidget<ListBox>(widgetName);
			if (lang) {
				for (std::size_t i = 0; i < w->getItemCount(); i++) {
					w->changeItemByIndex(i,
						(*_langdict)(_originalStrings[widgetName][i]));
				}
			}
		} else if (type == "ListView") {
			auto w = _findWidget<ListView>(widgetName);
			if (lang) {
				std::size_t colCount = w->getColumnCount();
				for (std::size_t i = 0; i < colCount; i++) {
					w->setColumnText(i,
						(*_langdict)(_originalStrings[widgetName][i]));
					for (std::size_t j = 0; j <= w->getItemCount(); j++) {
						w->changeSubItem(i, j, (*_langdict)
							(_originalStrings[widgetName][colCount * (i + 1) + j])
						);
					}
				}
			}
		} else if (type == "Picture") {
			auto w = _findWidget<Picture>(widgetName);
			if (updateTexture) {
				auto newRenderer = tgui::PictureRenderer();
				newRenderer.setTexture(_widgetPictures[animatedSprite]);
				w->setRenderer(newRenderer.getData());
			}
		} else if (type == "MenuBar") {
			auto w = _findWidget<MenuBar>(widgetName);
			if (lang) {
				// it's possible, but we would somehow need to store the menu
				// hierarchy separately to keep this as simple as possible.
				// potentially multiple menu hierarchies would have to be stored,
				// though...
			}
		} else if (type == "MessageBox") {
			auto w = _findWidget<MessageBox>(widgetName);
			if (lang) {
				w->setTitle((*_langdict)(_originalStrings[widgetName][0]));
				w->setText((*_langdict)(_originalStrings[widgetName][1]));
			}
			// don't know how I'm going to translate buttons
		} else if (type == "ProgressBar") {
			auto w = _findWidget<ProgressBar>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "RadioButton") {
			auto w = _findWidget<RadioButton>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		} else if (type == "TabContainer") {
			auto w = _findWidget<TabContainer>(widgetName);
			if (lang) {
				for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); i++) {
					w->changeTabText(i,
						(*_langdict)(_originalStrings[widgetName][i]));
				}
			}
		} else if (type == "Tabs") {
			auto w = _findWidget<Tabs>(widgetName);
			if (lang) {
				for (std::size_t i = 0; i < w->getTabsCount(); i++) {
					w->changeText(i,
						(*_langdict)(_originalStrings[widgetName][i]));
				}
			}
		} else if (type == "ToggleButton") {
			auto w = _findWidget<ToggleButton>(widgetName);
			if (lang) {
				w->setText((*_langdict)(_originalStrings[widgetName][0]));
			}
		}
		// container types - not all of them are here for future reference
		if (type == "ChildWindow" || type == "Grid" || type == "Group" ||
			type == "RadioButtonGroup") {
			auto w = _findWidget<Container>(widgetName);
			_animate(target, scaling, w, widgetName, lang, animatedSprite);
		}
		animatedSprite++;
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
		// clear state
		_gui.removeAllWidgets();
		_guiBackground.clear();
		_widgetPictures.clear();
		_widgetSprites.clear();
		_guiSpriteKeys.clear();
		_originalStrings.clear();
		// create the main menu that always exists
		tgui::Group::Ptr menu = tgui::Group::create();
		menu->setVisible(false);
		_gui.add(menu, "MainMenu");
		setGUI("MainMenu");
		if (_scripts) _scripts->callFunction("MainMenuSetUp");
		// create each menu
		for (auto& m : names) {
			menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, m);
			// temporarily set the current GUI to this one to make _findWidget()
			// work with relative widget names in SetUp() functions
			setGUI(m);
			if (_scripts) _scripts->callFunction(m + "SetUp");
		}
		// leave with the current menu being MainMenu
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
	// register non-widget global functions
	engine->RegisterGlobalFunction("void setGUI(const string& in)",
		asMETHODPR(sfx::gui, setGUI, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBackground(const string& in)",
		asMETHOD(sfx::gui, _noBackground), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBackground(const string& in, const "
		"string& in, const string& in)",
		asMETHOD(sfx::gui, _spriteBackground), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBackground(const uint, const uint, "
		"const uint, const uint, const string& in)",
		asMETHOD(sfx::gui, _colourBackground), asCALL_THISCALL_ASGLOBAL, this);
	// register bitmap button global functions
	engine->RegisterGlobalFunction("void addBitmapButton(const string& in, "
		"const float x, const float y, const float w, const float h)",
		asMETHOD(sfx::gui, _addBitmapButton), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBitmapButtonText(const string& in, "
		"const string& in)",
		asMETHOD(sfx::gui, _setBitmapButtonText), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void setBitmapButtonSprite(const string& in, "
		"const string& in)", asMETHOD(sfx::gui, _setBitmapButtonSprite),
		asCALL_THISCALL_ASGLOBAL, this);
	// register vertical layout container global functions
	engine->RegisterGlobalFunction("void addVerticalLayout(const string& in, "
		"const float x, const float y, const float w, const float h)",
		asMETHOD(sfx::gui, _addVerticalLayout), asCALL_THISCALL_ASGLOBAL, this);
	// register listbox global functions
	engine->RegisterGlobalFunction("void addListBox(const string& in, const float "
		"x, const float y, const float w, const float h)",
		asMETHOD(sfx::gui, _addListbox), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void addListBoxItem(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _addListboxItem), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("string getListBoxSelectedItem(const string& "
		"in)", asMETHOD(sfx::gui, _getListboxSelectedItem),
		asCALL_THISCALL_ASGLOBAL, this);
}

void sfx::gui::_noBackground(const std::string& menu) noexcept {
	_guiBackground.erase(menu);
}

void sfx::gui::_spriteBackground(const std::string& sheet,
	const std::string& sprite, const std::string& menu) noexcept {
	try {
		_guiBackground[menu].set(_sheet.at(sheet), sprite);
	} catch (std::out_of_range&) {
		_logger.error("Attempted to set sprite \"{}\" from sheet \"{}\" to the "
			"background of menu \"{}\". The sheet does not exist!", sprite, sheet,
			menu);
	}
}

void sfx::gui::_colourBackground(const unsigned int r, const unsigned int g,
	const unsigned int b, const unsigned int a, const std::string& menu) noexcept {
	_guiBackground[menu].set(sf::Color(r, g, b, a));
}

void sfx::gui::_addBitmapButton(const std::string& name, const float x,
	const float y, const float w, const float h) noexcept {
	std::vector<std::string> fullname;
	if (_findWidget<Widget>(name, &fullname)) {
		_logger.error("Attempted to create a new bitmap button with name \"{}\": "
			"a widget with that name already exists!", name);
	} else {
		auto widget = tgui::BitmapButton::create();
		widget->setPosition(tgui::Vector2f(x, y));
		widget->setSize(tgui::Vector2f(w, h));
		auto container = _gui.get<Container>(fullname[0]);
		if (!container) {
			_logger.error("Attempted to add a bitmap button \"{}\" to the menu "
				"\"{}\". This menu does not exist.", name, fullname[0]);
			return;
		}
		if (fullname.size() > 2) {
			for (std::size_t i = 1; i < fullname.size() - 1; i++) {
				if (!container) {
					_logger.error("Attempted to add a bitmap button \"{}\" to the "
						"container \"{}\" within menu \"{}\". This container does "
						"not exist.", name, fullname[i - 1], fullname[0]);
					return;
				}
				container = container->get<Container>(fullname[i]);
			}
		}
		container->add(widget, fullname.back());
		_connectSignals(widget);
	}
}

void sfx::gui::_setBitmapButtonText(const std::string& name,
	const std::string& text) noexcept {
	std::vector<std::string> fullname;
	BitmapButton::Ptr button = _findWidget<BitmapButton>(name, &fullname);
	if (button) {
		button->setText(text);
		std::string widgetFullname = "";
		for (auto& n : fullname) {
			widgetFullname += n + ".";
		}
		if (widgetFullname.size() > 0) widgetFullname.pop_back();
		if (_originalStrings[widgetFullname].size() == 0) {
			_originalStrings[widgetFullname].push_back(text);
		} else {
			_originalStrings[widgetFullname][0] = text;
		}
	} else {
		_logger.error("Attempted to set the text \"{}\" to a bitmap button "
			"\"{}\" within menu \"{}\". This bitmap button does not exist.", text,
			name, fullname[0]);
	}
}

void sfx::gui::_setBitmapButtonSprite(const std::string& name,
	const std::string& sprite) noexcept {
	std::vector<std::string> fullname;
	if (_findWidget<BitmapButton>(name, &fullname)) {
		std::string widgetFullname = "";
		for (auto& n : fullname) {
			widgetFullname += n + ".";
		}
		if (widgetFullname.size() > 0) widgetFullname.pop_back();
		_guiSpriteKeys[widgetFullname] = std::make_pair("icon", sprite);
	} else {
		_logger.error("Attempted to set the sprite \"{}\" to a bitmap button "
			"\"{}\" within menu \"{}\". This bitmap button does not exist.",
			sprite, name, fullname[0]);
	}
}

void sfx::gui::_addVerticalLayout(const std::string& name, const float x,
	const float y, const float w, const float h) noexcept {
	std::vector<std::string> fullname;
	if (_findWidget<Widget>(name, &fullname)) {
		_logger.error("Attempted to create a new vertical layout container with "
			"name \"{}\": a widget with that name already exists!", name);
	} else {
		auto widget = tgui::VerticalLayout::create();
		widget->setPosition(tgui::Vector2f(x, y));
		widget->setSize(tgui::Vector2f(w, h));
		auto container = _gui.get<Container>(fullname[0]);
		if (!container) {
			_logger.error("Attempted to add a vertical layout container \"{}\" to "
				"the menu \"{}\". This menu does not exist.", name, fullname[0]);
			return;
		}
		if (fullname.size() > 2) {
			for (std::size_t i = 1; i < fullname.size() - 1; i++) {
				if (!container) {
					_logger.error("Attempted to add a vertical layout container "
						"\"{}\" to the container \"{}\" within menu \"{}\". This "
						"container does not exist.", name, fullname[i - 1],
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

void sfx::gui::_addListbox(const std::string& name, const float x, const float y,
	const float w, const float h) noexcept {
	std::vector<std::string> fullname;
	if (_findWidget<Widget>(name, &fullname)) {
		_logger.error("Attempted to create a new listbox with name \"{}\": a "
			"widget with that name already exists!", name);
	} else {
		auto widget = tgui::ListBox::create();
		widget->setPosition(tgui::Vector2f(x, y));
		widget->setSize(tgui::Vector2f(w, h));
		auto container = _gui.get<Container>(fullname[0]);
		if (!container) {
			_logger.error("Attempted to add a listbox \"{}\" to the menu \"{}\". "
				"This menu does not exist.", name, fullname[0]);
			return;
		}
		if (fullname.size() > 2) {
			for (std::size_t i = 1; i < fullname.size() - 1; i++) {
				if (!container) {
					_logger.error("Attempted to add a listbox \"{}\" to the "
						"container \"{}\" within menu \"{}\". This container does "
						"not exist.", name, fullname[i - 1], fullname[0]);
					return;
				}
				container = container->get<Container>(fullname[i]);
			}
		}
		container->add(widget, fullname.back());
		_connectSignals(widget);
	}
}

void sfx::gui::_addListboxItem(const std::string& name, const std::string& item)
	noexcept {
	std::vector<std::string> fullname;
	ListBox::Ptr listbox = _findWidget<ListBox>(name, &fullname);
	if (listbox) {
		listbox->addItem(item);
		std::string widgetFullname = "";
		for (auto& n : fullname) {
			widgetFullname += n + ".";
		}
		if (widgetFullname.size() > 0) widgetFullname.pop_back();
		_originalStrings[widgetFullname].push_back(item);
	} else {
		_logger.error("Attempted to add a new listbox item \"{}\" to a listbox "
			"\"{}\" within menu \"{}\". This listbox does not exist.", item,
			name, fullname[0]);
	}
}

std::string sfx::gui::_getListboxSelectedItem(const std::string& name) noexcept {
	std::vector<std::string> fullname;
	ListBox::Ptr listbox = _findWidget<ListBox>(name, &fullname);
	if (listbox) {
		return listbox->getSelectedItem().toStdString();
	} else {
		_logger.error("Attempted to get a listbox \"{}\"'s currently selected "
			"item, within menu \"{}\". This listbox does not exist.", name,
			fullname[0]);
	}
	return "";
}