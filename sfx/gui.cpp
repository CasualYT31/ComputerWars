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
#include "fmtformatter.h"

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
	// Document GUI behaviours.
	document->DocumentExpectedFunction("WidgetNames",
		"Widgets have both a short name and a long name. The long name describes "
		"the full path of the widget from its menu down to the widget itself. For "
		"example, a widget named \"WidgetName\" within a layout container "
		"\"Layout\", which is within a child window \"Child\", which is in the "
		"menu \"ComplexMenu\", will have a long/full name of <tt>ComplexMenu."
		"Child.Layout.WidgetName</tt>. A widget's short name doesn't include the "
		"menu it is within. The short name of the previously mentioned widget "
		"would be <tt>Child.Layout.WidgetName</tt>.\n"
		"All widget name parameters, unless specified otherwise, can accept both "
		"full and short names for widgets. If a short name is given, it will be "
		"mapped to its full name by the engine by prefixing it with the name of "
		"the menu currently open. In order to add a widget to a container, it "
		"becomes necessary to explicitly include the container at all times "
		"within the widget's name. <b>This only isn't the case with signal "
		"handler function names, which always exclude container names, unless the "
		"signal handler is connected to a container itself.</b>\n"
		"It's worth noting, however, that if a container has the same name as a "
		"menu, then the engine will assume that the first name before the "
		"<tt>.</tt> is a menu name, and you will encounter unexpected results.");
	document->DocumentExpectedFunction("GUIErrorBehaviour",
		"Whenever an error is logged by GUI global functions, no changes will "
		"occur. If an error was logged during a query call, a blank object will "
		"be returned, unless specified otherwise.");
	document->DocumentExpectedFunction("GUITextTranslation",
		"Unless specified otherwise, string parameters given to the engine that "
		"represent captions, labels, or text that is displayed to the user, are "
		"language dictionary keys that are first translated before being "
		"displayed. More obvious exceptions to this rule are functions that allow "
		"you to set a textbox' typable contents.");
	document->DocumentExpectedFunction("GUISizeAndPosition",
		"The GUI backend used by the game engine allows you to specify "
		"expressions for the size and position of widgets. For an introduction "
		"into this topic, see https://tgui.eu/tutorials/0.9/layouts/. To specify "
		"pixel values, you can issue \"5px\", for example.");
	document->DocumentExpectedFunction("WidgetTypes",
		"Widget types are essentially the GUI backend's class names but in string "
		"form. https://tgui.eu/documentation/0.9/annotated.html is the backend's "
		"reference documentation, which lists all of the widgets available. Note "
		"that support might be patchy, though. If you find a widget or an "
		"operation that is not supported, you will have to add support yourself.");

	// Document expected functions.
	document->DocumentExpectedFunction("void MainMenuSetUp()",
		"Regardless of how the game is modded, there will <b>always</b> be a menu "
		"called \"MainMenu\". Therefore, this function must be defined somewhere "
		"within the GUI scripts.\n\n"
		"All menus have a <tt>SetUp()</tt> function which has the same "
		"declaration as this one, except it is called MenuName<tt>SetUp()</tt>. "
		"They are called when the game's GUI JSON configuration script is loaded, "
		"as and when each menu name is read from the script. This means that a "
		"menu defined later in the <tt>menus</tt> list won't exist when an "
		"earlier menu's <tt>SetUp()</tt> function is called.");
	document->DocumentExpectedFunction("void MainMenuOpen(const string&in)",
		"When switching to the \"MainMenu\", its \"Open()\" function will be "
		"called, if it has been defined. The parameter holds the name of the "
		"previous menu. This will be blank when the main menu is opened for the "
		"very first time.\n\n"
		"All menus have an <tt>Open()</tt> function which has the same "
		"declaration and behaviour as this one, except it is called "
		"MenuName<tt>Open()</tt>.");
	document->DocumentExpectedFunction(
		"void MainMenuHandleInput(const dictionary)",
		"Regardless of how the game is modded, there will <b>always</b> be a menu "
		"called \"MainMenu\". Therefore, this function must be defined somewhere "
		"within the GUI scripts.\n\n"
		"All menus have a <tt>HandleInput()</tt> function which has the same "
		"declaration as this one, except it is called "
		"MenuName<tt>HandleInput()</tt>. They are called as part of the game "
		"engine's main loop, with the aim of allowing the menu to react to any "
		"controls that the user may be inputting. Note that all the typical GUI "
		"input management (e.g. handling clicking of buttons, typing in "
		"textboxes) is carried out by the game engine and does not need to be "
		"handled by the scripts.\n\n"
		"The given <tt>dictionary</tt> maps control names—which are strings "
		"defined by the UI JSON configuration script—to booleans, where "
		"<tt>TRUE</tt> means that the control should be reacted to (if you are "
		"interested in it), and <tt>FALSE</tt> means that you should ignore the "
		"control for that iteration of the game loop.");
	document->DocumentExpectedFunction("void MenuName_WidgetName_SignalName()",
		"All GUI scripts can react to widget events by writing functions for "
		"any signals they are interested in. If a signal handler isn't defined, "
		"the signal will be silently ignored.\n\n"
		"For example, to see if a bitmap button called <tt>ButtonName</tt>, "
		"within a vertical layout container called <tt>MenuLayout</tt>, which is "
		"in a menu called <tt>GameMenu</tt>, has been clicked, the following "
		"function can be defined:\n"
		"<pre><code>void GameMenu_ButtonName_Pressed() {\n"
"\tinfo(\"I have been pressed!\");\n"
"}</code></pre>\n"
		"A list of supported signals can be found be in the "
		"<tt>sfx::gui::_connectSignals()</tt> method in the game engine's code.");

	// Register types.
	engine::RegisterColourType(engine, document);

	// Register non-widget global functions.
	auto r = engine->RegisterGlobalFunction("void setGUI(const string& in)",
		asMETHOD(sfx::gui, _setGUI), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Hides the current menu and shows the "
		"menu given.");
	r = engine->RegisterGlobalFunction("void setBackground(string)",
		asMETHOD(sfx::gui, _noBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes the background from the given "
		"menu.");
	r = engine->RegisterGlobalFunction("void setBackground(string, const "
		"string& in, const string& in)",
		asMETHOD(sfx::gui, _spriteBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given menu's background to be "
		"an animated sprite from the given sprite sheet.\n<ol><li>The name of the "
		"menu to set the background of.</li><li>The name of the spritesheet which "
		"contains the sprite to apply.</li><li>The name of the sprite to apply."
		"</li></ol>");
	r = engine->RegisterGlobalFunction("void setBackground(string, const uint, "
		"const uint, const uint, const uint)",
		asMETHOD(sfx::gui, _colourBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given menu's background to a "
		"solid colour. The name of the menu is given, then the R, G, B and A "
		"components of the colour, respectively.");
	// Register widget global functions.
	r = engine->RegisterGlobalFunction("void addWidget(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _addWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget and adds it to a "
		"menu. The type of widget is given, then the name of the new widget. If "
		"the name of the new widget is a full name, it will be added in the "
		"specified container. If it is not a full name, it will be added to the "
		"current menu.");
	r = engine->RegisterGlobalFunction("void setWidgetPosition(const string& in, "
		"const string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetPosition), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's position. The name of "
		"the widget is given, then the X position, then the Y position.");
	r = engine->RegisterGlobalFunction("void setWidgetSize(const string& in, "
		"const string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's size. The name of the "
		"widget is given, then the width, then the height.");
	r = engine->RegisterGlobalFunction("void setWidgetVisibility(const string&in, "
		"const bool)",
		asMETHOD(sfx::gui, _setWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's visibility. The name of "
		"the widget is given, then if it should be visible or not.");
	r = engine->RegisterGlobalFunction("void setWidgetText(const string& in, "
		"const string& in)",
		asMETHOD(sfx::gui, _setWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text. The name of the "
		"widget is given, then its new text.");
	r = engine->RegisterGlobalFunction("void setWidgetSprite(const string& in, "
		"const string& in, const string& in)",
		asMETHOD(sfx::gui, _setWidgetSprite), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's sprite. The name of "
		"the widget is given, then the name of the sprite sheet, then the name of "
		"the sprite.");
	r = engine->RegisterGlobalFunction(
		"void setWidgetBackgroundColour(const string&in, const Colour)",
		asMETHOD(sfx::gui, _setWidgetBgColour), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's background colour.");
	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderRadius(const string&in, const float)",
		asMETHOD(sfx::gui, _setWidgetBorderRadius),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's rounded border radius.");
	r = engine->RegisterGlobalFunction("void addItem(const string& in, const "
		"string& in)",
		asMETHOD(sfx::gui, _addItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new item to a widget. The name "
		"of the widget is given, then the text of the new item.");
	r = engine->RegisterGlobalFunction("string getSelectedItemText(const string& "
		"in)",
		asMETHOD(sfx::gui, _getSelectedItemText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's text.");
}

void sfx::gui::setGUI(const std::string& newPanel, const bool callOpen) noexcept {
	auto old = getGUI();
	if (_gui.get(old)) {
		_gui.get(old)->setVisible(false);
	}
	try {
		if (!_gui.get(newPanel)) throw tgui::Exception("GUI with name \"" +
			newPanel + "\" does not exist.");
		_gui.get(newPanel)->setVisible(true);
		// Clear widget sprites.
		_widgetSprites.clear();
		_currentGUI = newPanel;
		// Call NewPanelOpen() script function, if it has been defined.
		auto funcName = newPanel + "Open";
		if (callOpen && _scripts->functionExists(funcName))
			_scripts->callFunction(funcName, &old);
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
		setGUI("MainMenu", false);
		if (_scripts) _scripts->callFunction("MainMenuSetUp");
		// Create each menu.
		for (auto& m : names) {
			menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, m);
			// Temporarily set the current GUI to this one to make _findWidget()
			// work with relative widget names in SetUp() functions.
			setGUI(m, false);
			if (_scripts) _scripts->callFunction(m + "SetUp");
		}
		// Leave with the current menu being MainMenu.
		setGUI("MainMenu", false);
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

void sfx::gui::_setGUI(const std::string& name) noexcept {
	setGUI(name, true);
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
		} else if (type == "horizontallayout") {
			widget = tgui::HorizontalLayout::create();
		} else if (type == "picture") {
			widget = tgui::Picture::create();
		} else if (type == "label") {
			widget = tgui::Label::create();
		} else if (type == "scrollablepanel") {
			widget = tgui::ScrollablePanel::create();
		} else if (type == "panel") {
			widget = tgui::Panel::create();
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

void sfx::gui::_setWidgetVisibility(const std::string& name, const bool visible)
	noexcept {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setVisible(visible);
	} else {
		_logger.error("Attempted to update widget \"{}\"'s visibility, within "
			"menu \"{}\". This widget does not exist.", name, fullname[0]);
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
		} else if (type == "label") {
			_findWidget<Label>(name)->setText(text);
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
		if (type != "bitmapbutton" && type != "picture") {
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

void sfx::gui::_setWidgetBgColour(const std::string& name, const sf::Color& colour)
	noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			auto panel = _findWidget<Panel>(name);
			panel->getRenderer()->setBackgroundColor(colour);
		} else {
			_logger.error("Attempted to set the background colour \"{}\" to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", colour,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the background colour \"{}\" to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", colour, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetBorderRadius(const std::string& name, const float radius)
	noexcept {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			auto panel = _findWidget<Panel>(name);
			// panel->getRenderer()->setRoundedBorderRadius(radius);
		} else {
			_logger.error("Attempted to set the border radius {} to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", radius,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the border radius {} to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", radius, name,
			fullname[0]);
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