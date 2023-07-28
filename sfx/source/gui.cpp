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

#include "gui.hpp"
#include "fmtsfx.hpp"

#undef MessageBox

using namespace tgui;

// These values are intended to be constant.
static float NO_SPACE = -0.001f;
static sf::Color NO_COLOUR(0, 0, 0, 0);
static sfx::gui::MenuItemID NO_MENU_ITEM_ID =
	std::numeric_limits<sfx::gui::MenuItemID>::max();

////////////////////
// GUI_BACKGROUND //
////////////////////

sfx::gui::gui_background::gui_background(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
	const std::string& key) {
	set(sheet, key);
}

sfx::gui::gui_background::gui_background(const sf::Color& colour) {
	set(colour);
}

void sfx::gui::gui_background::set(
	const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
	const std::string& key) {
	_flag = sfx::gui::gui_background::type::Sprite;
	if (sheet) _bgSprite.setSpritesheet(sheet);
	_bgSprite.setSprite(key);
}

void sfx::gui::gui_background::set(const sf::Color& colour) {
	_flag = sfx::gui::gui_background::type::Colour;
	_bgColour.setFillColor(colour);
}

sfx::gui::gui_background::type sfx::gui::gui_background::getType() const noexcept {
	return _flag;
}

std::string sfx::gui::gui_background::getSprite() const {
	return _bgSprite.getSprite();
}

sf::Color sfx::gui::gui_background::getColour() const {
	return _bgColour.getFillColor();
}

bool sfx::gui::gui_background::animate(const sf::RenderTarget& target) {
	if (_flag == sfx::gui::gui_background::type::Sprite) {
		return _bgSprite.animate(target);
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

///////////////////////
// CSCRIPTANYWRAPPER //
///////////////////////

sfx::gui::CScriptAnyWrapper::CScriptAnyWrapper(CScriptAny* const obj) : _any(obj) {
	if (_any) _any->AddRef();
}

sfx::gui::CScriptAnyWrapper::CScriptAnyWrapper(
	const sfx::gui::CScriptAnyWrapper& obj) : _any(obj.operator->()) {
	if (_any) _any->AddRef();
}

sfx::gui::CScriptAnyWrapper::CScriptAnyWrapper(sfx::gui::CScriptAnyWrapper&& obj)
	noexcept : _any(std::move(obj.operator->())) {
	if (_any) _any->AddRef();
}

sfx::gui::CScriptAnyWrapper::~CScriptAnyWrapper() noexcept {
	if (_any) _any->Release();
}

CScriptAny* sfx::gui::CScriptAnyWrapper::operator->() const noexcept {
	return _any;
}

//////////////////////
// ORIGINAL_CAPTION //
//////////////////////

sfx::gui::original_caption::original_caption(const std::string& text,
	CScriptArray* vars) : caption(text) {
	if (!vars) return;
	for (asUINT i = 0; i < vars->GetSize(); ++i)
		variables.emplace_back((CScriptAny*)vars->At(i));
}

/////////
// GUI //
/////////

sfx::gui::gui(const std::shared_ptr<engine::scripts>& scripts,
	const engine::logger::data& data) : json_script({data.sink, "json_script"}),
	_scripts(scripts), _logger(data) {
	if (!scripts) {
		_logger.critical("No scripts object has been provided to this GUI object: "
			"no menus will be loaded.");
	} else {
		_scripts->addRegistrant(this);
	}
}

void sfx::gui::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
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
	document->DocumentExpectedFunction("void MainMenuOpen([const string&in])",
		"When switching to the \"MainMenu\", its \"Open()\" function will be "
		"called, if it has been defined. The parameter holds the name of the "
		"previous menu. This will be blank when the main menu is opened for the "
		"very first time. It is optional, i.e. you don't have to declare it as a "
		"parameter and the function will still be called.\n\n"
		"All menus have an <tt>Open()</tt> function which has the same "
		"declaration and behaviour as this one, except it is called "
		"MenuName<tt>Open()</tt>.");
	document->DocumentExpectedFunction("void MainMenuClose([const string&in])",
		"When switching from the \"MainMenu\", its \"Close()\" function will be "
		"called, if it has been defined. The parameter holds the name of the menu "
		"being opened next. It is optional, i.e. you don't have to declare it as "
		"a parameter and the function will still be called.\n\n"
		"All menus have a <tt>Close()</tt> function which has the same "
		"declaration and behaviour as this one, except it is called "
		"MenuName<tt>Close()</tt>.");
	document->DocumentExpectedFunction(
		"void MainMenuHandleInput(const dictionary[, const MousePosition&in, "
		"const MousePosition&in])",
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
		"control for that iteration of the game loop.\n\n"
		"The <tt>MousePosition</tt> parameters are optional and do not have to be "
		"defined. If they are, the first <tt>MousePosition</tt> holds the "
		"position of the mouse during the previous iteration of the game loop, "
		"and the second <tt>MousePosition</tt> holds the current position of the "
		"mouse.\n\n"
		"There must only be one <tt>HandleInput()</tt> function per menu.");
	document->DocumentExpectedFunction("void MenuName_WidgetName_SignalName()",
		"All GUI scripts can react to widget events by writing functions for "
		"any signals they are interested in. If a signal handler isn't defined, "
		"the signal will be silently ignored.\n\n"
		"For example, to see if a bitmap button called <tt>ButtonName</tt>, "
		"within a vertical layout container called <tt>MenuLayout</tt>, which is "
		"in a menu called <tt>GameMenu</tt>, has been clicked, the following "
		"function can be defined:\n"
		"<pre><code>void GameMenu_ButtonName_MouseReleased() {\n"
"\tinfo(\"I have been pressed!\");\n"
"}</code></pre>\n"
		"A list of supported signals can be found be in the "
		"<tt>sfx::gui::_connectSignals()</tt> method in the game engine's code.");
	document->DocumentExpectedFunction(
		"void MenuName_MenuBarName_MenuItemClicked(const MenuItemID)",
		"The <tt>MenuItemClicked</tt> signal handler is a special case. It "
		"accepts the ID of the menu item that was clicked. See the documentation "
		"on the C++ method <tt>sfx::gui::menuItemClickedSignalHandler()</tt> for "
		"more information.");

	// Register types.
	engine::RegisterColourType(engine, document);
	engine::RegisterVectorTypes(engine, document);

	auto r = engine->RegisterEnum("WidgetAlignment");
	document->DocumentObjectEnum(r, "Values representing widget alignments in a "
		"grid.");
	engine->RegisterEnumValue("WidgetAlignment", "Centre",
		static_cast<int>(Grid::Alignment::Center));
	engine->RegisterEnumValue("WidgetAlignment", "UpperLeft",
		static_cast<int>(Grid::Alignment::UpperLeft));
	engine->RegisterEnumValue("WidgetAlignment", "Up",
		static_cast<int>(Grid::Alignment::Up));
	engine->RegisterEnumValue("WidgetAlignment", "UpperRight",
		static_cast<int>(Grid::Alignment::UpperRight));
	engine->RegisterEnumValue("WidgetAlignment", "Right",
		static_cast<int>(Grid::Alignment::Right));
	engine->RegisterEnumValue("WidgetAlignment", "BottomRight",
		static_cast<int>(Grid::Alignment::BottomRight));
	engine->RegisterEnumValue("WidgetAlignment", "Bottom",
		static_cast<int>(Grid::Alignment::Bottom));
	engine->RegisterEnumValue("WidgetAlignment", "BottomLeft",
		static_cast<int>(Grid::Alignment::BottomLeft));
	engine->RegisterEnumValue("WidgetAlignment", "Left",
		static_cast<int>(Grid::Alignment::Left));

	r = engine->RegisterEnum("ScrollbarPolicy");
	document->DocumentObjectEnum(r, "Values representing scollbar policies.");
	engine->RegisterEnumValue("ScrollbarPolicy", "Automatic",
		static_cast<int>(Scrollbar::Policy::Automatic));
	engine->RegisterEnumValue("ScrollbarPolicy", "Always",
		static_cast<int>(Scrollbar::Policy::Always));
	engine->RegisterEnumValue("ScrollbarPolicy", "Never",
		static_cast<int>(Scrollbar::Policy::Never));

	r = engine->RegisterEnum("HorizontalAlignment");
	document->DocumentObjectEnum(r, "Values representing horizontal alignment.");
	engine->RegisterEnumValue("HorizontalAlignment", "Left",
		static_cast<int>(Label::HorizontalAlignment::Left));
	engine->RegisterEnumValue("HorizontalAlignment", "Centre",
		static_cast<int>(Label::HorizontalAlignment::Center));
	engine->RegisterEnumValue("HorizontalAlignment", "Right",
		static_cast<int>(Label::HorizontalAlignment::Right));

	r = engine->RegisterEnum("VerticalAlignment");
	document->DocumentObjectEnum(r, "Values representing vertical alignment.");
	engine->RegisterEnumValue("VerticalAlignment", "Top",
		static_cast<int>(Label::VerticalAlignment::Top));
	engine->RegisterEnumValue("VerticalAlignment", "Centre",
		static_cast<int>(Label::VerticalAlignment::Center));
	engine->RegisterEnumValue("VerticalAlignment", "Bottom",
		static_cast<int>(Label::VerticalAlignment::Bottom));

	r = engine->RegisterEnum("TitleButton");
	document->DocumentObjectEnum(r, "Values representing different "
		"<tt>ChildWindow</tt> titlebar buttons.");
	engine->RegisterEnumValue("TitleButton", "None",
		static_cast<int>(ChildWindow::TitleButton::None));
	engine->RegisterEnumValue("TitleButton", "Close",
		static_cast<int>(ChildWindow::TitleButton::Close));
	engine->RegisterEnumValue("TitleButton", "Maximise",
		static_cast<int>(ChildWindow::TitleButton::Maximize));
	engine->RegisterEnumValue("TitleButton", "Minimise",
		static_cast<int>(ChildWindow::TitleButton::Minimize));
	engine->RegisterEnumValue("TitleButton", "All",
		static_cast<int>(ChildWindow::TitleButton::Close |
			ChildWindow::TitleButton::Maximize |
			ChildWindow::TitleButton::Minimize));

	// Register global constants and typdefs.
	engine->RegisterTypedef("MenuItemID", "uint64");
	document->DocumentExpectedFunction("typedef uint64 MenuItemID",
		"Index used to identify a menu item in a <tt>MenuBar</tt> widget.");
	r = engine->RegisterGlobalProperty("const float NO_SPACE", &NO_SPACE);
	document->DocumentExpectedFunction("const float NO_SPACE", "Constant which "
		"represents \"no space between widgets in a vertical or horizontal "
		"layout\". Due to rounding errors, however, this likely won't be "
		"perfect, especially when scaling is applied.");
	r = engine->RegisterGlobalProperty("const Colour NO_COLOUR", &NO_COLOUR);
	document->DocumentExpectedFunction("const Colour NO_COLOUR", "Constant which "
		"holds a colour value of (0, 0, 0, 0).");
	r = engine->RegisterGlobalProperty("const string PREVIOUS_MENU",
		&_previousGUI);
	document->DocumentExpectedFunction("const string PREVIOUS_MENU", "Holds the "
		"name of the menu that was open before the current one. Scripts cannot "
		"change this value, but the engine does update it when switching menus.");
	r = engine->RegisterGlobalProperty("const MenuItemID NO_MENU_ITEM_ID",
		&NO_MENU_ITEM_ID);
	document->DocumentExpectedFunction("const MenuItemID NO_MENU_ITEM_ID",
		"Constant which is returned when creating a menu or menu item in a "
		"<tt>MenuBar</tt> failed.");

	// Register non-widget global functions.
	r = engine->RegisterGlobalFunction("void setGUI(const string& in)",
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
	r = engine->RegisterGlobalFunction("bool widgetExists(const string&in)",
		asMETHOD(sfx::gui, _widgetExists), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if the named "
		"widget exists, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction("bool menuExists(const string&in)",
		asMETHOD(sfx::gui, _menuExists), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if the named "
		"menu exists, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction("void addWidget(const string&in, const "
		"string&in, const string&in = \"\")",
		asMETHOD(sfx::gui, _addWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget and adds it to a "
		"menu. The type of widget is given, then the name of the new widget. If "
		"the name of the new widget is a full name, it will be added in the "
		"specified container. If it is not a full name, it will be added to the "
		"current menu.\n"
		"The final parameter is the name of the script function to call when this "
		"widget emits a signal. If a blank string is given, then the default "
		"handlers will be assumed. The custom signal handler must have two "
		"<tt>const string&in</tt> parameters. The first is the full name of the "
		"widget that triggered the handler. The second is the name of the signal "
		"that was emitted. <b>Note that the following signals will never invoke "
		"the custom signal handler!</b>\n"
		"<ul><li><tt>MenuItemClicked</tt>.</li>"
		"<li><tt>Closing</tt>.</li></ul>");

	r = engine->RegisterGlobalFunction("void addWidgetToGrid(const string&in,"
		"const string&in, const uint, const uint, const string&in = \"\")",
		asMETHOD(sfx::gui, _addWidgetToGrid), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget and adds it to a "
		"grid. The type of widget is given, then the name of the new widget "
		"(which must include the grid's name before the new widget's name!). The "
		"widget's row and column index are then specified, in that order.\n"
		"See <tt>addWidget()</tt> for information on the final parameter to this "
		"function.");

	r = engine->RegisterGlobalFunction("void removeWidget(const string&in)",
		asMETHOD(sfx::gui, _removeWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes a given widget. If the given "
		"widget is a container, then all of its widgets will be removed "
		"recursively.");

	r = engine->RegisterGlobalFunction("void removeWidgetsFromContainer("
		"const string&in)", asMETHOD(sfx::gui, _removeWidgetsFromContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all the widgets from a given "
		"container/menu, but does not remove the container/menu itself.");

	r = engine->RegisterGlobalFunction("void setWidgetFocus(const string&in)",
		asMETHOD(sfx::gui, _setWidgetFocus), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Updates the setfocus to point to a given "
		"widget.");

	r = engine->RegisterGlobalFunction("void setWidgetFont(const string&in, "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetFont), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's font. The name of "
		"the widget is given, then the name of the font, as defined in the "
		"fonts.json script.");

	r = engine->RegisterGlobalFunction("void setGlobalFont(const string&in)",
		asMETHOD(sfx::gui, _setGlobalFont), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the global font. The name of the "
		"font is given, as defined in the fonts.json script.");

	r = engine->RegisterGlobalFunction("void setWidgetPosition(const string&in, "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetPosition), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's position. The name of "
		"the widget is given, then the X position, then the Y position.");

	r = engine->RegisterGlobalFunction("Vector2f getWidgetAbsolutePosition("
		"const string&in)",
		asMETHOD(sfx::gui, _getWidgetAbsolutePosition),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's absolute position. The "
		"name of the widget is given, then the position of the top-left point of "
		"the widget is returned. Returns (0.0f,0.0f) upon an error.");

	r = engine->RegisterGlobalFunction("void setWidgetOrigin(const string&in, "
		"const float, const float)",
		asMETHOD(sfx::gui, _setWidgetOrigin), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's origin. The name of "
		"the widget is given first. Then the new origin is given: along the X "
		"axis, and then along the Y axis. Each origin is a value between 0 and 1, "
		"and represents a percentage, from left/top to right/bottom.");

	r = engine->RegisterGlobalFunction("void setWidgetSize(const string&in, "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's size. The name of the "
		"widget is given, then the width, then the height.");

	r = engine->RegisterGlobalFunction(
		"Vector2f getWidgetFullSize(const string&in)",
		asMETHOD(sfx::gui, _getWidgetFullSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's full size, which "
		"includes any borders it may have, etc.");

	r = engine->RegisterGlobalFunction("void setWidgetEnabled(const string&in, "
		"const bool)",
		asMETHOD(sfx::gui, _setWidgetEnabled), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's enabled state. The name "
		"of the widget is given, then if it should be enabled or not.");

	r = engine->RegisterGlobalFunction("bool getWidgetEnabled(const string&in)",
		asMETHOD(sfx::gui, _getWidgetEnabled), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's enabled state.");

	r = engine->RegisterGlobalFunction("void setWidgetVisibility(const string&in, "
		"const bool)",
		asMETHOD(sfx::gui, _setWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's visibility. The name of "
		"the widget is given, then if it should be visible or not.");

	r = engine->RegisterGlobalFunction("bool getWidgetVisibility(const string&in)",
		asMETHOD(sfx::gui, _getWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's visibility.");

	r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlow("
		"const string&in, const string&in, const string&in, const string&in, "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlow),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets the widgets that should "
		"be selected if directional controls are input when the given widget is "
		"currently selected. The \"given widget\" should be given first, followed "
		"by the widgets that should be selected, when up, down, left, and right "
		"are input, respectively. All given widgets must be in the same menu! A "
		"blank string means that the input won't change the selected widget. A "
		"value of \"").append(GOTO_PREVIOUS_WIDGET).append("\" means \"navigate "
		"back to the previously selected widget.\"").c_str());

	r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlowStart("
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlowStart),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets which widget should be selected "
		"first when a directional control is first input on that widget's menu.");

	r = engine->RegisterGlobalFunction("void clearWidgetDirectionalFlowStart("
		"const string&in)",
		asMETHOD(sfx::gui, _clearWidgetDirectionalFlowStart),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Used to explicitly prevent directional "
		"controls from selecting a widget for the given menu.");

	r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlowSelection("
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlowSelection),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Manually select a widget which the user "
		"can move away from using the directional controls.");

	r = engine->RegisterGlobalFunction("void setDirectionalFlowAngleBracketSprite("
		"const string&in, const string&in, const string&in)",
		asMETHOD(sfx::gui, _setDirectionalFlowAngleBracketSprite),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Used to set the sprite used as a given "
		"angle bracket, which surrounds the widget currently selected using the "
		"directional controls. The first string denotes the corner (either "
		"\"UL\", \"UR\", \"LL\", or \"LR\"), the second string denotes the "
		"spritesheet to retrieve the sprite from, and the third string stores the "
		"name of the sprite.");

	r = engine->RegisterGlobalFunction("void setWidgetText(const string&in, "
		"const string&in, array<any>@ = null)",
		asMETHOD(sfx::gui, _setWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets a widget's text. The "
		"name of the widget is given, then its new text. An optional list of "
		"variables can also be given. These variables will be inserted into the "
		"text wherever a '" + std::to_string(engine::expand_string::getVarChar()) +
		"' is found.").c_str());

	r = engine->RegisterGlobalFunction("string getWidgetText(const string&in)",
		asMETHOD(sfx::gui, _getWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's caption/text.");

	r = engine->RegisterGlobalFunction("void setWidgetTextSize(const string&in, "
		"const uint)",
		asMETHOD(sfx::gui, _setWidgetTextSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's character size. The name "
		"of the widget is given, then its new character size.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextColour(const string&in, const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetTextColour), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextOutlineColour(const string&in, const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetTextOutlineColour),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text outline colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextOutlineThickness(const string&in, const float)",
		asMETHOD(sfx::gui, _setWidgetTextOutlineThickness),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text outline thickness.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextAlignment(const string&in, const HorizontalAlignment, "
		"const VerticalAlignment)",
		asMETHOD(sfx::gui, _setWidgetTextAlignment),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text horizontal and "
		"vertical alignment.");

	r = engine->RegisterGlobalFunction("void setWidgetSprite(const string&in, "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetSprite), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's sprite. The name of "
		"the widget is given, then the name of the sprite sheet, then the name of "
		"the sprite.");

	r = engine->RegisterGlobalFunction("void clearWidgetSprite(const string&in)",
		asMETHOD(sfx::gui, _clearWidgetSprite), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Clears/removes a widget's sprite.");

	r = engine->RegisterGlobalFunction("void matchWidgetSizeToSprite("
		"const string & in, const bool)",
		asMETHOD(sfx::gui, _matchWidgetSizeToSprite),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt> is given for a widget, "
		"it informs the engine that the widget should <b>always</b> match the "
		"size of its sprite. Only supported by the Picture widget type. <b>TRUE "
		"is the default behaviour for all picture widgets!</b>");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBackgroundColour(const string&in, const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetBgColour), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's background colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderSize(const string&in, const float)",
		asMETHOD(sfx::gui, _setWidgetBorderSize),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's border size. Applies the "
		"same size to each side of the widget.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderColour(const string&in, const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetBorderColour),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's border colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderRadius(const string&in, const float)",
		asMETHOD(sfx::gui, _setWidgetBorderRadius),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's rounded border radius.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetIndex(const string&in, const uint)",
		asMETHOD(sfx::gui, _setWidgetIndex),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a "
		"container.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetIndexInContainer(const string&in, const uint, const uint)",
		asMETHOD(sfx::gui, _setWidgetIndexInContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a given "
		"container. The name of the container is given, then the index of the "
		"widget to edit, with its new index given as the last parameter.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetRatioInLayout(const string&in, const uint, const float)",
		asMETHOD(sfx::gui, _setWidgetRatioInLayout),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Within a VerticalLayout or "
		"HorizontalLayout; sets the ratio of a specified widget's size compared "
		"to others in the layout. The unsigned integer is a 0-based index of the "
		"widget in the layout to amend.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetDefaultText(const string&in, const string&in, "
		"array<any>@ = null)",
		asMETHOD(sfx::gui, _setWidgetDefaultText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets a widget's default "
		"text. The name of the widget is given, then its new default text. An "
		"optional list of variables can also be given. These variables will be "
		"inserted into the text wherever a '" +
		std::to_string(engine::expand_string::getVarChar()) + "' is "
		"found.").c_str());

	r = engine->RegisterGlobalFunction("void addItem(const string&in, const "
		"string&in, array<any>@ = null)",
		asMETHOD(sfx::gui, _addItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new item to a widget. The name "
		"of the widget is given, then the text of the new item. An optional list "
		"variables can also be given: see setWidgetText() for more information.");

	r = engine->RegisterGlobalFunction("void clearItems(const string&in)",
		asMETHOD(sfx::gui, _clearItems), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all items from a given widget. "
		"The name of the widget should be given.");

	r = engine->RegisterGlobalFunction("void setSelectedItem(const string&in, "
		"const uint)", asMETHODPR(sfx::gui, _setSelectedItem,
		(const std::string&, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects an item from a widget. The name "
		"of the widget is given, then the 0-based index of the item to select.");

	r = engine->RegisterGlobalFunction("int getSelectedItem(const string&in)",
		asMETHOD(sfx::gui, _getSelectedItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's index.");

	r = engine->RegisterGlobalFunction("string getSelectedItemText("
		"const string&in)",
		asMETHOD(sfx::gui, _getSelectedItemText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's text.");

	r = engine->RegisterGlobalFunction("uint getWidgetCount(const string&in)",
		asMETHOD(sfx::gui, _getWidgetCount), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets the number of widgets that are in "
		"the specified container. Does not count recursively.");

	r = engine->RegisterGlobalFunction("void setHorizontalScrollbarPolicy("
		"const string&in, const ScrollbarPolicy)",
		asMETHOD(sfx::gui, _setHorizontalScrollbarPolicy),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's horizontal "
		"scrollbar policy.");

	r = engine->RegisterGlobalFunction("void setHorizontalScrollbarAmount("
		"const string&in, const uint)",
		asMETHOD(sfx::gui, _setHorizontalScrollbarAmount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's horizontal "
		"scroll amount.");

	r = engine->RegisterGlobalFunction("void setVerticalScrollbarAmount("
		"const string&in, const uint)",
		asMETHOD(sfx::gui, _setVerticalScrollbarAmount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's vertical scroll "
		"amount.");

	r = engine->RegisterGlobalFunction("void setGroupPadding("
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _setGroupPadding), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a group's padding.");

	r = engine->RegisterGlobalFunction("void setWidgetAlignmentInGrid("
		"const string&in, const uint, const uint, const WidgetAlignment)",
		asMETHOD(sfx::gui, _setWidgetAlignmentInGrid),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's alignment within its "
		"grid cell.");

	r = engine->RegisterGlobalFunction("void setSpaceBetweenWidgets("
		"const string&in, const float)",
		asMETHOD(sfx::gui, _setSpaceBetweenWidgets),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the space between widgets in a "
		"vertical or horizontal layout.");

	// MENUS //
	
	r = engine->RegisterGlobalFunction("MenuItemID addMenu(const string&in, "
		"const string&in, array<any>@ = null)",
		asMETHOD(sfx::gui, _addMenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a new menu. The name of the "
		"<tt>MenuBar</tt> is given, then the text of the new menu. An optional "
		"list of variables can also be given. These variables will be inserted "
		"into the text in the same way as setWidgetText().\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>...SetUp()</tt> function!\n"
		"The ID of the newly created menu is returned. If the function failed, "
		"<tt>NO_MENU_ITEM_ID</tt> is returned.");

	r = engine->RegisterGlobalFunction("MenuItemID addMenuItem(const string&in, "
		"const string&in, array<any>@ = null)",
		asMETHOD(sfx::gui, _addMenuItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a new menu item. The name of the "
		"<tt>MenuBar</tt> is given, then the text of the new menu item. See "
		"addMenu() for an explanation of the array parameter. The menu item will "
		"be added to the most recently created menu (addMenu()) or submenu "
		"(addMenuItemIntoLastItem()). If there are no menus, the function will "
		"fail.\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>...SetUp()</tt> function!\n"
		"The ID of the newly created menu item is returned. If the function "
		"failed, <tt>NO_MENU_ITEM_ID</tt> is returned.");

	r = engine->RegisterGlobalFunction("MenuItemID addMenuItemIntoLastItem(const "
		"string&in, const string&in, array<any>@ = null)",
		asMETHOD(sfx::gui, _addMenuItemIntoLastItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new submenu in the most "
		"recently created menu or submenu, and adds a new menu item into it. The "
		"name of the <tt>MenuBar</tt> is given, then the text of the new menu "
		"item. See addMenu() for an explanation of the array parameter. If there "
		"are no menus, the function will fail. If the most recently created menu "
		"is empty, then this call will have the same effect as addMenuItem().\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>...SetUp()</tt> function!\n"
		"The ID of the newly created menu item is returned. If the function "
		"failed, <tt>NO_MENU_ITEM_ID</tt> is returned.");

	r = engine->RegisterGlobalFunction("void exitSubmenu(const string&in)",
		asMETHOD(sfx::gui, _exitSubmenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Exits the current submenu. The name of "
		"the <tt>MenuBar</tt> is given. This function can be used to go up one "
		"level in the menu hierarchy at a time. If the current hierarchy level is "
		"less than three (i.e. the given menu bar is not in a submenu), then the "
		"function will fail.\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>...SetUp()</tt> function!");

	// CHILDWINDOWS //

	r = engine->RegisterGlobalFunction("void autoHandleMinMax("
		"const string&in, const bool)",
		asMETHOD(sfx::gui, _autoHandleMinMax),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt>, instructs the engine "
		"to handle the minimise and maximise functionality of the given "
		"<tt>ChildWindow</tt>, before invoking the <tt>Minimized</tt> and "
		"<tt>Maximised</tt> signal handlers. This is the default. Use "
		"<tt>FALSE</tt> to disable this functionality for the given "
		"<tt>ChildWindow</tt>.");

	r = engine->RegisterGlobalFunction("void setChildWindowTitleButtons("
		"const string&in, const uint)",
		asMETHOD(sfx::gui, _setChildWindowTitleButtons),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given <tt>ChildWindow</tt>'s "
		"titlebar buttons. The given integer should be a bitwise-ORed list of "
		"<tt>TitleButton</tt> enum values.");

	r = engine->RegisterGlobalFunction("void setWidgetResizable("
		"const string&in, const bool)",
		asMETHOD(sfx::gui, _setWidgetResizable),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt>, the given widget can "
		"be resized by the user, if the widget supports it. If <tt>FALSE</tt>, "
		"only the engine or scripts can resize the given widget.");

	r = engine->RegisterGlobalFunction("void restoreChildWindow(const string&in)",
		asMETHOD(sfx::gui, _restoreChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Restores a <tt>ChildWindow</tt> if it "
		"was maximised or minimised. If the given <tt>ChildWindow</tt> was "
		"neither, then this function will have no effect.");
}

void sfx::gui::setGUI(const std::string& newPanel, const bool callClose,
	const bool callOpen) {
	auto old = getGUI();
	if (_gui.get(old)) {
		_gui.get(old)->setVisible(false);
	}
	try {
		if (!_gui.get(newPanel)) throw tgui::Exception("GUI with name \"" +
			newPanel + "\" does not exist.");
		_gui.get(newPanel)->setVisible(true);
		// Call CurrentPanelClose() script function, if it has been defined.
		auto closeFuncName = _currentGUI + "Close",
			closeFuncEmptyDecl = "void " + _currentGUI + "Close()",
			closeFuncDecl = "void " + _currentGUI + "Close(const string&in)";
		std::string newMenu = newPanel; // Can't pass in pointer to const string :(
		if (callClose && !_currentGUI.empty()) {
			if (_scripts->functionDeclExists(closeFuncDecl)) {
				_scripts->callFunction(closeFuncName, &newMenu);
			} else if (_scripts->functionDeclExists(closeFuncEmptyDecl)) {
				_scripts->callFunction(closeFuncName);
			}
		}
		// Clear widget sprites.
		_widgetSprites.clear();
		_previousGUI = old;
		_currentGUI = newPanel;
		// Call NewPanelOpen() script function, if it has been defined.
		auto openFuncName = newPanel + "Open",
			openFuncEmptyDecl = "void " + _currentGUI + "Open()";
		if (callOpen) {
			if (_scripts->functionDeclExists(openFuncEmptyDecl)) {
				_scripts->callFunction(openFuncName);
			}
		}
		// If there is no widget currently selected, automatically select the first
		// widget.
		// Do this after invoking the Open function to allow that function to set
		// the first selected widget, if said widget is being added by the Open
		// function and is not available beforehand.
		if (_findCurrentlySelectedWidget().first.empty()) {
			_makeNewDirectionalSelection(
				_selectThisWidgetFirst[_currentGUI], _currentGUI);
		}
	} catch (const tgui::Exception& e) {
		_logger.error("{}", e);
		if (_gui.get(old)) _gui.get(old)->setVisible(true);
	}
}

void sfx::gui::addSpritesheet(const std::string& name,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	if (_sheet.find(name) != _sheet.end()) {
		_logger.warning("Updated the spritesheet named \"{}\"!", name);
	}
	_sheet[name] = sheet;
}

void sfx::gui::setTarget(sf::RenderTarget& newTarget) {
	_gui.setTarget(newTarget);
}

bool sfx::gui::handleEvent(sf::Event e) {
	return _gui.handleEvent(e);
}

/**
 * Checks if a given widget is visible and/or enabled, and that the same can be
 * said for all of its parents.
 * @warning An assertion is made that at least one of the boolean parameters is
 *          \c TRUE!
 * @param   widget  Pointer to the widget to test.
 * @param   visible If \c TRUE, will test each widget's visibility.
 * @param   enabled If \c TRUE, will test each widget's enabled state.
 * @return  \c TRUE if the given widget and all of its parents pass the specified
 *          tests, \c FALSE if at least one of the widgets in the chain does not
 *          pass a single test.
 */
static bool isWidgetFullyVisibleAndEnabled(const Widget* widget,
	const bool visible, const bool enabled) {
	assert((visible || enabled) && widget);
	if ((!visible || widget->isVisible()) && (!enabled || widget->isEnabled())) {
		if (widget = reinterpret_cast<Widget*>(widget->getParent()))
			return isWidgetFullyVisibleAndEnabled(widget, visible, enabled);
		return true;
	}
	return false;
}

/**
 * Makes a widget in a \c ScrollablePanel visible by scrolling the
 * scrollbars to make the widget fully visible.
 * If the given widget does not have a \c ScrollablePanel ancestor, then no
 * changes will be made.
 * @param widget             Pointer to the widget to show.
 * @param panelAncestryDepth Recursion parameter. Leave to default.
 */
static void showWidgetInScrollablePanel(const Widget::Ptr& widget,
	const unsigned int panelAncestryDepth = 0) {
	static const std::function<ScrollablePanel*(Widget*,const unsigned int)>
		findScrollablePanelAncestor =
		[](Widget* w, const unsigned int depth) -> ScrollablePanel* {
		if (w = reinterpret_cast<Widget*>(w->getParent())) {
			if (w->getWidgetType() == "ScrollablePanel") {
				if (depth == 0)
					return dynamic_cast<ScrollablePanel*>(w);
				else
					return findScrollablePanelAncestor(w, depth - 1);
			}
			return findScrollablePanelAncestor(w, depth);
		} else return nullptr;
	};

	auto panel = findScrollablePanelAncestor(widget.get(), panelAncestryDepth);
	if (!panel) return; // Exit condition.
	// If there are no scrollbars, then don't do anything with this panel.
	const auto horiShown = panel->isHorizontalScrollbarShown(),
		vertShown = panel->isVerticalScrollbarShown();
	if (!horiShown && !vertShown) {
		showWidgetInScrollablePanel(widget, panelAncestryDepth + 1);
		return;
	}
	// Figure out portion of ScrollablePanel that is being shown.
	const auto scrollbarWidth = panel->getScrollbarWidth();

	const sf::FloatRect panelRect(
		panel->getAbsolutePosition().x + panel->getContentOffset().x,
		panel->getAbsolutePosition().y + panel->getContentOffset().y,
		// Gotta exclude the scrollbars from the visible portion.
		panel->getSize().x - (vertShown ? scrollbarWidth : 0.0f),
		panel->getSize().y - (horiShown ? scrollbarWidth : 0.0f)
	);
	// Figure out bounding rectangle of given widget.
	const sf::FloatRect widgetRect(
		widget->getAbsolutePosition().x,
		widget->getAbsolutePosition().y,
		widget->getSize().x,
		widget->getSize().y
	);
	// Figure out if given widget is fully visible inside that portion.
	// If not, scroll scrollbars by required amount, if possible.
	if (horiShown) {
		const int oldHori = static_cast<int>(panel->getHorizontalScrollbarValue());
		int newHori = oldHori;
		if (widgetRect.left >
			panelRect.left + panelRect.width - widgetRect.width) {
			// Too far right.
			if (widgetRect.width < panelRect.width) {
				newHori = oldHori + static_cast<int>(
					::abs((widgetRect.left + widgetRect.width) -
						(panelRect.left + panelRect.width)));
			} else {
				// If the widget is too wide for the panel, always favour the left
				// side.
				newHori = oldHori + static_cast<int>(
					::abs(widgetRect.left - panelRect.left));
			}
		}
		if (widgetRect.left < panelRect.left) {
			// Too far left.
			newHori = oldHori - static_cast<int>(
				::abs(panelRect.left - widgetRect.left));
		}
		if (newHori <= 0)
			panel->setHorizontalScrollbarValue(0);
		else
			panel->setHorizontalScrollbarValue(static_cast<unsigned int>(newHori));
	}
	if (vertShown) {
		const auto oldVert = static_cast<int>(panel->getVerticalScrollbarValue());
		int newVert = oldVert;
		if (widgetRect.top >
			panelRect.top + panelRect.height - widgetRect.height) {
			// Too far down.
			if (widgetRect.height < panelRect.height) {
				newVert = oldVert + static_cast<int>(
					::abs((widgetRect.top + widgetRect.height) -
						(panelRect.top + panelRect.height)));
			} else {
				// If the widget is too high for the panel, always favour the top
				// side.
				newVert = oldVert + static_cast<int>(
					::abs(widgetRect.top - panelRect.top));
			}
		}
		if (widgetRect.top < panelRect.top) {
			// Too far up.
			newVert = oldVert - static_cast<int>(
				::abs(panelRect.top - widgetRect.top));
		}
		if (newVert <= 0)
			panel->setVerticalScrollbarValue(0);
		else
			panel->setVerticalScrollbarValue(static_cast<unsigned int>(newVert));
	}
	// The ScrollablePanel itself may be in more ScrollablePanels, so we need to
	// make sure they're scrolled properly, too.
	showWidgetInScrollablePanel(widget, panelAncestryDepth + 1);
}

void sfx::gui::handleInput(const std::shared_ptr<sfx::user_input>& ui) {
	if (ui) {
		_handleInputErrorLogged = false;
		// Keep track of mouse movement. If the mouse has moved, then we disregard
		// directional flow (and select inputs) until a new directional input has
		// been made.
		_previousMousePosition = _currentMousePosition;
		_currentMousePosition = ui->mousePosition();
		if (_previousMousePosition != _currentMousePosition)
			_enableDirectionalFlow = false;
		// Handle directional input.
		bool signalHandlerTriggered = false;
		if (_enableDirectionalFlow) {
			const auto cursel = _moveDirectionalFlow(ui);
			// If select is issued, and there is currently a widget selected that
			// isn't disabled, then trigger an appropriate signal.
			if ((*ui)[_selectControl] && !cursel.empty()) {
				const auto widget = _findWidget<Widget>(cursel);
				if (isWidgetFullyVisibleAndEnabled(widget.get(), true, true)) {
					const auto& widgetType = widget->getWidgetType();
					if (widgetType == "Button" || widgetType == "BitmapButton" ||
						widgetType == "ListBox") {
						signalHandler(widget, "MouseReleased");
						signalHandlerTriggered = true;
					}
				}
			}
		} else if (_previousMousePosition == _currentMousePosition) {
			// Only re-enable directional flow if a directional input is made,
			// whilst the mouse isn't moving.
			_enableDirectionalFlow = (*ui)[_upControl] || (*ui)[_downControl] ||
				(*ui)[_leftControl] || (*ui)[_rightControl];
			// If there wasn't a selection made previously, go straight to making
			// the selection.
			const auto cursel = _findCurrentlySelectedWidget();
			if (cursel.first.empty())
				_moveDirectionalFlow(ui);
			// Otherwise, make sure what was selected is now visible to the user.
			else if (_enableDirectionalFlow && cursel.second)
				showWidgetInScrollablePanel(cursel.second);
		}
		// Invoke the current menu's bespoke input handling function.
		// If the signal handler was invoked, do not invoke any bespoke input
		// handler. If we do, it can cause multiple inputs that are typically
		// carried out separately to be processed in a single iteration.
		if (!signalHandlerTriggered) {
			const auto funcName = getGUI() + "HandleInput";
			const auto basicHandleInputDecl = "void " + funcName +
				"(const dictionary)",
				extendedHandleInputDecl = "void " + funcName +
				"(const dictionary, const MousePosition&in, "
					"const MousePosition&in)";
			if (_scripts->functionExists(funcName)) {
				// Construct the dictionary.
				CScriptDictionary* controls = _scripts->createDictionary();
				auto controlKeys = ui->getControls();
				for (auto& key : controlKeys)
					controls->Set(key, (asINT64)ui->operator[](key));
				// Invoke the function.
				if (_scripts->functionDeclExists(basicHandleInputDecl)) {
					_scripts->callFunction(funcName, controls);
				} else if (_scripts->functionDeclExists(extendedHandleInputDecl)) {
					_scripts->callFunction(funcName, controls,
						&_previousMousePosition, &_currentMousePosition);
				}
				controls->Release();
			}
		}
	} else if (!_handleInputErrorLogged) {
		_logger.error("Called handleInput() with nullptr user_input object for "
			"menu \"{}\"!", getGUI());
		_handleInputErrorLogged = true;
	}
}

bool sfx::gui::signalHandler(tgui::Widget::Ptr widget,
	const tgui::String& signalName) {
	if (_scripts && getGUI() != "") {
		std::string fullname = widget->getWidgetName().toStdString();
		std::string signalNameStd = signalName.toStdString();
		auto customHandler = _customSignalHandlers.find(fullname);
		if (customHandler != _customSignalHandlers.end()) {
			std::string decl = "void " + customHandler->second +
				"(const string&in, const string&in)";
			if (_scripts->functionDeclExists(decl)) {
				return _scripts->callFunction(customHandler->second, &fullname,
					&signalNameStd);
			} else {
				_logger.warning("Widget \"{}\" was configured with a custom "
					"signal handler \"{}\", but a function of declaration \"{}\" "
					"does not exist. Falling back on the default signal handler.",
					fullname, customHandler->second, decl);
			}
		}
		std::string functionName = getGUI() + "_" + _extractWidgetName(fullname) +
			"_" + signalNameStd;
		if (_scripts->functionExists(functionName)) {
			return _scripts->callFunction(functionName);
		}
	}
	return false;
}

void sfx::gui::menuItemClickedSignalHandler(const std::string& menuBarName,
	const sfx::gui::MenuItemID index) {
	const auto funcName = getGUI() + "_" + menuBarName +
		"_MenuItemClicked";
	const auto funcDecl = "void " + funcName + "(const MenuItemID)";
	if (_scripts->functionDeclExists(funcDecl)) {
		_scripts->callFunction(funcName, index);
	}
}

void sfx::gui::child_window_properties::cache(
	const tgui::ChildWindow::Ptr& window) {
	size = window->getSizeLayout();
	position = window->getPositionLayout();
	origin = window->getOrigin();
	isResizeable = window->isResizable();
	isPositionLocked = window->isPositionLocked();
}

void sfx::gui::child_window_properties::restore(
	const tgui::ChildWindow::Ptr& window) {
	window->setSize(size);
	window->setPosition(position);
	window->setOrigin(origin);
	window->setResizable(isResizeable);
	window->setPositionLocked(isPositionLocked);
}

static const std::size_t PADDING = 5, WIDTH = 100;

tgui::String sfx::gui::minimised_child_window_list::minimise(
	const std::string& name) {
	std::size_t x = PADDING;
	for (auto& window : _windows) {
		if (window.empty()) {
			window = name;
			return std::to_string(x).append("px");
		}
		x += WIDTH + PADDING;
	}
	_windows.push_back(name);
	return std::to_string(x).append("px");
}

void sfx::gui::minimised_child_window_list::restore(const std::string& name) {
	for (auto& window : _windows) if (window == name) window = "";
	while (_windows.size() > 0 && _windows.back().empty()) _windows.pop_back();
}

void sfx::gui::closingSignalHandler(const tgui::ChildWindow::Ptr& window,
	bool* abort) {
	const auto widgetName = window->getWidgetName().toStdString();
	// Firstly, invoke the signal handler, if it exists. If it doesn't, always
	// "close" the window.
	const auto funcName = getGUI() + "_" + _extractWidgetName(widgetName) +
		"_Closing";
	const auto funcDecl = "void " + funcName + "(bool&out)";
	bool close = true;
	if (_scripts->functionDeclExists(funcDecl)) {
		_scripts->callFunction(funcName, &close);
	}
	if (close) {
		// If the window was minimised when it was closed, we need to restore it.
		if (_childWindowData.find(widgetName) != _childWindowData.end()) {
			auto& data = _childWindowData[widgetName];
			if (data.isMinimised) _restoreChildWindowImpl(window, data);
		}
		// Instead of removing the window from its parent, we make it go invisible
		// instead.
		window->setVisible(false);
	}
	// We always abort the closing process to prevent TGUI from removing the window
	// from its parent.
	*abort = true;
}

void sfx::gui::minimizedSignalHandler(const tgui::ChildWindow::Ptr& window) {
	const auto widgetName = window->getWidgetName().toStdString();
	if (_childWindowData.find(widgetName) != _childWindowData.end()) {
		auto& data = _childWindowData[widgetName];
		if (!data.isMinimised) {
			if (!data.isMaximised) data.cache(window);
			data.isMinimised = true;
			data.isMaximised = false;
			const auto x = _minimisedChildWindowList[
				window->getParent()->getWidgetName().toStdString()].
				minimise(widgetName);
			window->setSize(WIDTH, tgui::String(std::to_string(
				window->getRenderer()->getTitleBarHeight()).append("px")));
			window->setPosition(x, "99%");
			window->setOrigin(0.0f, 1.0f);
			window->setResizable(false);
			window->setPositionLocked(true);
			window->moveToBack();
		}
	}
	signalHandler(window, "Minimized");
}

void sfx::gui::maximizedSignalHandler(const tgui::ChildWindow::Ptr& window) {
	const auto widgetName = window->getWidgetName().toStdString();
	if (_childWindowData.find(widgetName) != _childWindowData.end()) {
		auto& data = _childWindowData[widgetName];
		if (data.isMinimised || data.isMaximised) {
			_restoreChildWindowImpl(window, data);
		} else {
			data.cache(window);
			data.isMinimised = false;
			data.isMaximised = true;
			window->setSize("100%", "100%");
			window->setPosition("50%", "50%");
			window->setOrigin(0.5f, 0.5f);
			window->setResizable(false);
			window->setPositionLocked(true);
		}
		window->moveToFront();
	}
	signalHandler(window, "Maximized");
}

void sfx::gui::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& lang) {
	_lastlang = "";
	_langdict = lang;
}

void sfx::gui::setFonts(const std::shared_ptr<sfx::fonts>& fonts) noexcept {
	_fonts = fonts;
}

void sfx::gui::setScalingFactor(const float factor) {
	_gui.setRelativeView({0.0f, 0.0f, 1.0f / factor, 1.0f / factor});
}

bool sfx::gui::animate(const sf::RenderTarget& target) {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		_guiBackground.at(getGUI()).animate(target);
	}

	if (getGUI() != "") {
		if (_langdict && _langdict->getLanguage() != _lastlang) {
			_lastlang = _langdict->getLanguage();
			auto& widgetList = _gui.getWidgets();
			for (auto& widget : widgetList) _translateWidget(widget);
		}
		std::size_t animatedSprite = 0;
		_animate(target, _gui.get<Container>(getGUI()));
	}

	// Whenever there isn't a widget currently selected via directional controls,
	// or the currently selected widget is not currently visible, always reset the
	// animation.
	const auto& cursel = _findCurrentlySelectedWidget();
	if (cursel.first.empty() || !_enableDirectionalFlow || !cursel.second ||
		!isWidgetFullyVisibleAndEnabled(cursel.second.get(), true, false)) {
		_angleBracketUL.setCurrentFrame(0);
		_angleBracketUR.setCurrentFrame(0);
		_angleBracketLL.setCurrentFrame(0);
		_angleBracketLR.setCurrentFrame(0);
	} else if (!cursel.first.empty()) {
		// Ensure the angle brackets are at the correct locations.
		auto pos = cursel.second->getAbsolutePosition(),
			size = cursel.second->getSize();
		if (cursel.second->getWidgetType() == "ScrollablePanel") {
			pos += std::dynamic_pointer_cast<ScrollablePanel>(cursel.second)->
				getContentOffset();
		}
		_angleBracketUL.setPosition(pos);
		_angleBracketUL.animate(target);
		_angleBracketUR.setPosition(pos + tgui::Vector2f(
			size.x - _angleBracketUR.getSize().x, 0.0f));
		_angleBracketUR.animate(target);
		_angleBracketLL.setPosition(pos + tgui::Vector2f(0.0f,
			size.y - _angleBracketLL.getSize().y));
		_angleBracketLL.animate(target);
		_angleBracketLR.setPosition(pos + size -
			tgui::Vector2f(_angleBracketLR.getSize()));
		_angleBracketLR.animate(target);
	}

	return false;
}

void sfx::gui::_animate(const sf::RenderTarget& target,
	tgui::Container::Ptr container) {
	static auto allocImage = [&](const tgui::String& type, Widget::Ptr widget,
		const std::string& widgetName, unsigned int w, unsigned int h) -> void {
		// We cannot allow an empty image to be allocated, so always ensure the
		// image is at least 1x1 pixels.
		if (!w) w = 1; if (!h) h = 1;

		// Create an empty texture
		sf::Uint8* pixels = (sf::Uint8*)calloc(
			static_cast<std::size_t>(w) * static_cast<std::size_t>(h), 4);
		sf::Texture blank;
		blank.create(w, h);
		blank.update(pixels);
		free(pixels);

		// Assign the empty texture
		if (type == "BitmapButton") {
			std::dynamic_pointer_cast<BitmapButton>(widget)->
				setImage(blank);
		} else if (type == "Picture") {
			auto picture = std::dynamic_pointer_cast<tgui::Picture>(widget);
			picture->getRenderer()->setTexture(blank);
			if (_dontOverridePictureSizeWithSpriteSize.find(widgetName)
				== _dontOverridePictureSizeWithSpriteSize.end()) {
				// Resize the Picture to match with the sprite's size.
				picture->setSize(w, h);
			}
		}
	};

	// Animate each widget.
	auto& widgetList = container->getWidgets();
	for (auto& widget : widgetList) {
		// Ignore the widget if it is hidden.
		if (!widget->isVisible()) continue;

		std::string widgetName = widget->getWidgetName().toStdString();
		String type = widget->getWidgetType();

		// Only BitmapButtons and Pictures can have animated sprites.
		if (type == "BitmapButton" || type == "Picture") {
			// If the widget doesn't have a sprite, or if it doesn't have a valid
			// spritesheet, then don't animate the widget's sprite.
			if (_guiSpriteKeys.find(widgetName) != _guiSpriteKeys.end() &&
				_sheet.find(_guiSpriteKeys.at(widgetName).first) != _sheet.end()) {
				std::shared_ptr<sfx::animated_spritesheet> sheet =
					_sheet[_guiSpriteKeys[widgetName].first];
				const std::string& sprite = _guiSpriteKeys[widgetName].second;

				if (_widgetSprites.find(widget) == _widgetSprites.end()) {
					// Animated sprite for this widget doesn't exist yet, so
					// allocate it.
					_widgetSprites.insert({ widget, sfx::animated_sprite() });
				}
				auto& animatedSprite = _widgetSprites.at(widget);

				if (sprite == "" && animatedSprite.getSprite() != "") {
					// If the sprite has been removed, then we also need to remove
					// the image from the widget, see else if case at the bottom.
					allocImage(type, widget, widgetName, 0, 0);
					continue;
				}

				if (animatedSprite.getSpritesheet() != sheet ||
					animatedSprite.getSprite() != sprite) {
					// If the widget's animated sprite hasn't been given its sprite
					// yet, or if it has changed, then we need to update the
					// widget's texture so that all the positioning and sizing
					// matches up.
					animatedSprite.setSpritesheet(sheet);
					animatedSprite.setSprite(sprite);
					animatedSprite.animate(target);
					sf::Vector2f spriteSizeF = animatedSprite.getSize();
					sf::Vector2u spriteSize((unsigned int)spriteSizeF.x,
						(unsigned int)spriteSizeF.y);
					allocImage(type, widget, widgetName, spriteSize.x,
						spriteSize.y);
				} else {
					// If the widget's sprite hasn't changed, then simply animate
					// it.
					animatedSprite.animate(target);
				}

				// Now reposition the animated sprites based on the locations and
				// sizes of the widget's image.
				sf::Vector2f newPosition;
				if (type == "BitmapButton") {
					newPosition = std::dynamic_pointer_cast<BitmapButton>(widget)->
						getAbsolutePositionOfImage();
				} else if (type == "Picture") {
					newPosition = std::dynamic_pointer_cast<tgui::Picture>(widget)
						->getAbsolutePosition();
				}
				animatedSprite.setPosition(newPosition);
			} else if (_guiSpriteKeys.find(widgetName) != _guiSpriteKeys.end() &&
				_widgetSprites.at(widget).getSpritesheet() != nullptr &&
				_sheet.find(_guiSpriteKeys.at(widgetName).first) == _sheet.end()) {
				// Else if the widget DID have a valid spritesheet, then we're
				// going to have to remove the image from the widget to ensure that
				// sizing works out.
				allocImage(type, widget, widgetName, 0, 0);
			}
		}

		if (_isContainerWidget(type))
			_animate(target, std::dynamic_pointer_cast<Container>(widget));
	}
}

void sfx::gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draw background.
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// This GUI has a background to animate.
		target.draw(_guiBackground.at(getGUI()), states);
	}
	// Draw foreground.
	_gui.draw();
	// Draw angle brackets, if there is currently a widget selected via the
	// directional controls, and it is visible.
	if (_enableDirectionalFlow && _currentlySelectedWidget.find(getGUI()) !=
		_currentlySelectedWidget.end() &&
		!_currentlySelectedWidget.at(getGUI()).second.empty() &&
		isWidgetFullyVisibleAndEnabled(_findWidget<Widget>(
			_currentlySelectedWidget.at(getGUI()).second).get(), true, false)) {
		sf::View oldView = target.getView();
		target.setView(sf::View(
			_gui.getView().getRect().operator sf::Rect<float>()));
		target.draw(_angleBracketUL, states);
		target.draw(_angleBracketUR, states);
		target.draw(_angleBracketLL, states);
		target.draw(_angleBracketLR, states);
		target.setView(oldView);
	}
}

// Maybe need to add RenderStates to this callback in the future.
void sfx::gui::_drawCallback(BackendRenderTarget& target,
	tgui::Widget::ConstPtr widget) const {
	const auto widgetSprite = _widgetSprites.find(widget);
	if (widgetSprite != _widgetSprites.end()) {
		dynamic_cast<BackendRenderTargetSFML&>(target).getTarget()->
			draw(widgetSprite->second);
	}
}

std::string sfx::gui::_moveDirectionalFlow(
	const std::shared_ptr<sfx::user_input>& ui) {
	const auto cursel = _findCurrentlySelectedWidget();
	const auto& widgetType = !cursel.second ? "" : cursel.second->getWidgetType();
	if ((*ui)[_upControl]) {
		if (cursel.first.empty()) {
			_makeNewDirectionalSelection(_selectThisWidgetFirst[getGUI()],
				getGUI());
		} else if (widgetType == "ListBox") {
			const auto listbox = std::dynamic_pointer_cast<ListBox>(cursel.second);
			const auto i = listbox->getSelectedItemIndex();
			if (i == -1) {
				listbox->setSelectedItemByIndex(0);
			} else if (i > 0) {
				listbox->setSelectedItemByIndex(static_cast<std::size_t>(i) - 1);
			} else if (_directionalFlow[cursel.first].up.empty()) {
				// If there is nowhere to go from the top, loop through to the
				// the bottom of the list.
				listbox->setSelectedItemByIndex(listbox->getItemCount() - 1);
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].up,
					getGUI());
			}
		} else if (widgetType == "ScrollablePanel") {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getVerticalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getVerticalScrollAmount() > 0 &&
				panel->isVerticalScrollbarShown() && value > 0) {
				if (static_cast<int>(value) -
					static_cast<int>(panel->getVerticalScrollAmount()) < 0) {
					panel->setVerticalScrollbarValue(0);
				} else {
					panel->setVerticalScrollbarValue(
						value - panel->getVerticalScrollAmount());
				}
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].up,
					getGUI());
			}
		} else {
			_makeNewDirectionalSelection(_directionalFlow[cursel.first].up,
				getGUI());
		}
	}
	if ((*ui)[_downControl]) {
		if (cursel.first.empty()) {
			_makeNewDirectionalSelection(_selectThisWidgetFirst[getGUI()],
				getGUI());
		} else if (widgetType == "ListBox") {
			const auto listbox = std::dynamic_pointer_cast<ListBox>(cursel.second);
			const auto i = listbox->getSelectedItemIndex();
			if (i == -1) {
				listbox->setSelectedItemByIndex(0);
			} else if (i < listbox->getItemCount() - 1) {
				listbox->setSelectedItemByIndex(static_cast<std::size_t>(i) + 1);
			} else if (_directionalFlow[cursel.first].down.empty()) {
				// If there is nowhere to go from the bottom, loop through to the
				// the top of the list.
				listbox->setSelectedItemByIndex(0);
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].down,
					getGUI());
			}
		} else if (widgetType == "ScrollablePanel") {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getVerticalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getVerticalScrollAmount() > 0 &&
				panel->isVerticalScrollbarShown() &&
				value < panel->getVerticalScrollbarMaximum() -
					static_cast<unsigned int>(panel->getSize().y) +
					static_cast<unsigned int>(panel->getScrollbarWidth())) {
				panel->setVerticalScrollbarValue(
					value + panel->getVerticalScrollAmount());
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].down,
					getGUI());
			}
		} else {
			_makeNewDirectionalSelection(_directionalFlow[cursel.first].down,
				getGUI());
		}
	}
	if ((*ui)[_leftControl]) {
		if (cursel.first.empty()) {
			_makeNewDirectionalSelection(_selectThisWidgetFirst[getGUI()],
				getGUI());
		} else if (widgetType == "ScrollablePanel") {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getHorizontalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getHorizontalScrollAmount() > 0 &&
				panel->isHorizontalScrollbarShown() && value > 0) {
				if (static_cast<int>(value) -
					static_cast<int>(panel->getHorizontalScrollAmount()) < 0) {
					panel->setHorizontalScrollbarValue(0);
				} else {
					panel->setHorizontalScrollbarValue(
						value - panel->getHorizontalScrollAmount());
				}
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].left,
					getGUI());
			}
		} else {
			_makeNewDirectionalSelection(_directionalFlow[cursel.first].left,
				getGUI());
		}
	}
	if ((*ui)[_rightControl]) {
		if (cursel.first.empty()) {
			_makeNewDirectionalSelection(_selectThisWidgetFirst[getGUI()],
				getGUI());
		} else if (widgetType == "ScrollablePanel") {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getHorizontalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getHorizontalScrollAmount() > 0 &&
				panel->isHorizontalScrollbarShown() &&
				value < panel->getHorizontalScrollbarMaximum() -
					static_cast<unsigned int>(panel->getSize().x) +
					static_cast<unsigned int>(panel->getScrollbarWidth())) {
				panel->setHorizontalScrollbarValue(
					value + panel->getHorizontalScrollAmount());
			} else {
				_makeNewDirectionalSelection(_directionalFlow[cursel.first].right,
					getGUI());
			}
		} else {
			_makeNewDirectionalSelection(_directionalFlow[cursel.first].right,
				getGUI());
		}
	}
	return _currentlySelectedWidget[_currentGUI].second;
}

void sfx::gui::_makeNewDirectionalSelection(const std::string& newsel,
	const std::string& menu) {
	if (newsel.empty()) return;
	if (newsel == GOTO_PREVIOUS_WIDGET) {
		// Do not allow selection to go ahead if the previous widget is now not
		// visible!
		if (isWidgetFullyVisibleAndEnabled(_findWidget<Widget>(
			_currentlySelectedWidget[menu].first).get(), true, false))
			std::swap(_currentlySelectedWidget[menu].first,
				_currentlySelectedWidget[menu].second);
		else
			return;
	} else {
		// Do not allow selection to go ahead if the given widget is not visible!
		if (isWidgetFullyVisibleAndEnabled(_findWidget<Widget>(
			newsel).get(), true, false)) {
			_currentlySelectedWidget[menu].first =
				_currentlySelectedWidget[menu].second;
			_currentlySelectedWidget[menu].second = newsel;
		} else return;
	}
	const auto widget = _findWidget<Widget>(_currentlySelectedWidget[menu].second);
	signalHandler(widget, "MouseEntered");
	showWidgetInScrollablePanel(widget);
};

void sfx::gui::_translateWidget(tgui::Widget::Ptr widget) {
	std::string widgetName = widget->getWidgetName().toStdString();
	String type = widget->getWidgetType();
	if (_originalCaptions.find(widgetName) != _originalCaptions.end()) {
		if (type == "Button") { // SingleCaption
			auto w = _findWidget<Button>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "BitmapButton") { // SingleCaption
			auto w = _findWidget<BitmapButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "CheckBox") { // SingleCaption
			auto w = _findWidget<CheckBox>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "ChildWindow") { // SingleCaption
			auto w = _findWidget<ChildWindow>(widgetName);
			w->setTitle(_getTranslatedText(widgetName));
		} else if (type == "ColorPicker") { // SingleCaption
			auto w = _findWidget<ColorPicker>(widgetName);
			w->setTitle(_getTranslatedText(widgetName));
		} else if (type == "ComboBox") { // ListOfCaptions
			auto w = _findWidget<ComboBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
		} else if (type == "EditBox") { // SingleCaption
			auto w = _findWidget<EditBox>(widgetName);
			w->setDefaultText(_getTranslatedText(widgetName));
		} else if (type == "FileDialog") { // SingleCaption
			auto w = _findWidget<FileDialog>(widgetName);
			w->setTitle(_getTranslatedText(widgetName));
		} else if (type == "Label") { // SingleCaption
			auto w = _findWidget<Label>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "ListBox") { // ListOfCaptions
			auto w = _findWidget<ListBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
		} else if (type == "ListView") { // ListOfCaptions
			auto w = _findWidget<ListView>(widgetName);
			std::size_t colCount = w->getColumnCount();
			for (std::size_t i = 0; i < colCount; ++i) {
				w->setColumnText(i, _getTranslatedText(widgetName, i));
				for (std::size_t j = 0; j <= w->getItemCount(); ++j) {
					w->changeSubItem(i, j,
						_getTranslatedText(widgetName, colCount * (i + 1) + j)
					);
				}
			}
		} else if (type == "MenuBar") { // ListOfCaptions
			auto w = _findWidget<MenuBar>(widgetName);
			// Each and every menu item is stored in _originalCaptions depth-first.
			// See the documentation on menuItemClickedSignalHandler() for more
			// info.
			std::vector<tgui::String> hierarchy;
			std::size_t index = 0;
			static const std::function<void(
				const std::vector<MenuBar::GetMenusElement>&)> translateItems =
					[&](const std::vector<MenuBar::GetMenusElement>& items) {
				for (const auto& item : items) {
					hierarchy.push_back(item.text);
					const tgui::String translatedCaption =
						_getTranslatedText(widgetName, index);
					w->changeMenuItem(hierarchy, translatedCaption);
					hierarchy.back() = translatedCaption;
					// I know, it's really ugly. Not much choice.
					w->connectMenuItem(hierarchy,
						&sfx::gui::menuItemClickedSignalHandler, this,
						_extractWidgetName(widgetName),
						static_cast<sfx::gui::MenuItemID>(index++));
					translateItems(item.menuItems);
					hierarchy.pop_back();
				}
			};
			translateItems(w->getMenus());
		} else if (type == "MessageBox") { // ListOfCaptions
			auto w = _findWidget<tgui::MessageBox>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
			w->setText(_getTranslatedText(widgetName, 1));
			// Don't know how I'm going to translate buttons.
		} else if (type == "ProgressBar") { // SingleCaption
			auto w = _findWidget<ProgressBar>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "RadioButton") { // SingleCaption
			auto w = _findWidget<RadioButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == "TabContainer") { // ListOfCaptions
			auto w = _findWidget<TabContainer>(widgetName);
			for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); ++i)
				w->changeTabText(i, _getTranslatedText(widgetName, i));
		} else if (type == "Tabs") { // ListOfCaptions
			auto w = _findWidget<Tabs>(widgetName);
			for (std::size_t i = 0; i < w->getTabsCount(); ++i)
				w->changeText(i, _getTranslatedText(widgetName, i));
		} else if (type == "ToggleButton") { // SingleCaption
			auto w = _findWidget<ToggleButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		}
	}
	if (_isContainerWidget(type)) {
		auto w = _findWidget<Container>(widgetName);
		auto& widgetList = w->getWidgets();
		for (auto& w : widgetList) _translateWidget(w);
	}
}

bool sfx::gui::_load(engine::json& j) {
	_isLoading = true;
	try {
		std::vector<std::string> names;
		j.applyVector(names, { "menus" });
		if (j.inGoodState()) {
			// Clear state.
			_gui.removeAllWidgets();
			_guiBackground.clear();
			_widgetSprites.clear();
			_guiSpriteKeys.clear();
			_dontOverridePictureSizeWithSpriteSize.clear();
			_originalCaptions.clear();
			_customSignalHandlers.clear();
			_upControl.clear();
			_downControl.clear();
			_leftControl.clear();
			_rightControl.clear();
			_selectControl.clear();
			_directionalFlow.clear();
			_selectThisWidgetFirst.clear();
			_currentlySelectedWidget.clear();
			_hierarchyOfLastMenuItem.clear();
			_menuCounter.clear();
			_childWindowData.clear();
			_minimisedChildWindowList.clear();
			// Create the main menu that always exists.
			tgui::Group::Ptr menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, "MainMenu");
			setGUI("MainMenu", false, false);
			if (_scripts) _scripts->callFunction("MainMenuSetUp");
			// Create each menu.
			for (const auto& m : names) {
				menu = tgui::Group::create();
				menu->setVisible(false);
				_gui.add(menu, m);
				// Temporarily set the current GUI to this one to make
				// _findWidget() work with relative widget names in SetUp()
				// functions.
				setGUI(m, false, false);
				if (_scripts) _scripts->callFunction(m + "SetUp");
			}
			_isLoading = false;
			// Leave with the current menu being MainMenu.
			// _previousGUI will hold the name of the last menu in the JSON array.
			setGUI("MainMenu", false, true);
			// Load game control settings.
			j.apply(_upControl, { "up" }, true);
			j.apply(_downControl, { "down" }, true);
			j.apply(_leftControl, { "left" }, true);
			j.apply(_rightControl, { "right" }, true);
			j.apply(_selectControl, { "select" }, true);
			return true;
		}
	} catch (...) {
		_isLoading = false;
		throw;
	}
	_isLoading = false;
	return false;
}

bool sfx::gui::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

// ALL SIGNALS NEED TO BE TESTED IDEALLY
void sfx::gui::_connectSignals(tgui::Widget::Ptr widget,
	const std::string& customSignalHandler) {
	// Register the custom signal handler, if one is provided.
	if (!customSignalHandler.empty()) {
		_customSignalHandlers[widget->getWidgetName().toStdString()] =
			customSignalHandler;
	}
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
	// Connect clickable widget signals.
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
	// Connect bespoke signals.
	if (type == "button" || type == "bitmapbutton") {
		widget->getSignal("Pressed").connectEx(&sfx::gui::signalHandler, this);
	} else if (type == "childwindow") {
		auto childWindow = std::dynamic_pointer_cast<ChildWindow>(widget);
		widget->getSignal("MousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Closed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("EscapeKeyPressed").
			connectEx(&sfx::gui::signalHandler, this);
		// The engine can perform additional tasks upon receiving the Minimized,
		// Maximized, and Closing signals. Eventually the signalHandler is called,
		// though.
		childWindow->onMinimize(&sfx::gui::minimizedSignalHandler, this,
			childWindow);
		childWindow->onMaximize(&sfx::gui::maximizedSignalHandler, this,
			childWindow);
		childWindow->onClosing(&sfx::gui::closingSignalHandler, this, childWindow);
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
		// Each item is connected individually, when created and when translated.
		/*widget->getSignal("MenuItemClicked").
			connectEx(&sfx::gui::signalHandler, this);*/
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

void sfx::gui::_removeWidgets(const tgui::Widget::Ptr& widget,
	const tgui::Container::Ptr& container, const bool removeIt) {
	if (_isContainerWidget(widget->getWidgetType())) {
		auto container = _findWidget<Container>(
			widget->getWidgetName().toStdString());
		auto& widgetsInContainer = container->getWidgets();
		for (auto& widgetInContainer : widgetsInContainer) {
			// Remove each child widget's internal data entries only.
			_removeWidgets(widgetInContainer, container, false);
		}
		// Now remove each child widget fr.
		container->removeAllWidgets();
		if (!removeIt) return;
	}
	// Remove widget.
	const std::string name = widget->getWidgetName().toStdString();
	if (container) {
		_widgetSprites.erase(widget);
		_guiSpriteKeys.erase(name);
		_dontOverridePictureSizeWithSpriteSize.erase(name);
		_originalCaptions.erase(name);
		_customSignalHandlers.erase(name);
		_directionalFlow.erase(name);
		_hierarchyOfLastMenuItem.erase(name);
		_menuCounter.erase(name);
		_childWindowData.erase(name);
		_minimisedChildWindowList.erase(name);
		// If a ChildWindow, remove it from its parent's ChildWindowList.
		_minimisedChildWindowList[container->getWidgetName().toStdString()].
			restore(name);
		// Also delete references to the removed sprite from other widgets.
		for (auto& flowInfo : _directionalFlow) {
			if (flowInfo.second.up == name) flowInfo.second.up.clear();
			if (flowInfo.second.down == name) flowInfo.second.down.clear();
			if (flowInfo.second.left == name) flowInfo.second.left.clear();
			if (flowInfo.second.right == name) flowInfo.second.right.clear();
		}
		// If the removed widget was configured to be selected first at all, remove
		// it.
		for (auto& menu : _selectThisWidgetFirst)
			if (menu.second == name) menu.second = "";
		// If the removed widget was previously selected at all, then remove it
		// from the history. If the removed widget is currently selected, then
		// deselect it and erase the history, as well.
		for (auto& selectedWidgetData : _currentlySelectedWidget) {
			if (selectedWidgetData.second.second == name) {
				selectedWidgetData.second = {};
			} else if (selectedWidgetData.second.first == name) {
				selectedWidgetData.second.first = "";
			}
		}
		if (removeIt) container->remove(widget);
	} else {
		_logger.error("Attempted to remove a widget \"{}\", which did not have a "
			"container!", name);
	}
}

void sfx::gui::_setTranslatedString(const std::string& fullname,
	const std::string& text, CScriptArray* variables) {
	_originalCaptions[fullname] = sfx::gui::SingleCaption(text, variables);
}

void sfx::gui::_setTranslatedString(const std::string& fullname,
	const std::string& text, CScriptArray* variables, const std::size_t index) {
	if (!std::holds_alternative<sfx::gui::ListOfCaptions>(
		_originalCaptions[fullname]))
		_originalCaptions[fullname] = sfx::gui::ListOfCaptions();
	auto& list = std::get<sfx::gui::ListOfCaptions>(_originalCaptions[fullname]);
	if (list.size() <= index) list.resize(index + 1);
	list[index] = sfx::gui::SingleCaption(text, variables);
}

std::string sfx::gui::_getTranslatedText(const sfx::gui::original_caption& caption,
	const std::function<void(engine::logger *const, const std::string&)>&
	warningCallback) const {
	std::string ret = (*_langdict)(caption.caption);
	for (auto& var : caption.variables) {
		int type = var->GetTypeId();
		if (type == _scripts->getTypeID("int64")) {
			asINT64 val = 0;
			var->Retrieve(val);
			ret = engine::expand_string::insert(ret, val);
		} else if (type == _scripts->getTypeID("double")) {
			double val = 0.0;
			var->Retrieve(val);
			ret = engine::expand_string::insert(ret, val);
		} else if (type == _scripts->getTypeID("string")) {
			std::string val;
			var->Retrieve(&val, type);
			ret = engine::expand_string::insert(ret, val);
		} else {
			warningCallback(&_logger, _scripts->getTypeName(type));
			ret = engine::expand_string::insert(ret, "");
		}
	}
	return ret;
}

std::string sfx::gui::_getTranslatedText(const std::string& fullname) const {
	return _getTranslatedText(std::get<sfx::gui::SingleCaption>(
		_originalCaptions.at(fullname)), [&](engine::logger *const logger,
			const std::string& typeName) {
				logger->warning("Unsupported type \"{}\" given when translating "
					"widget \"{}\"'s caption: inserting blank string instead.",
					typeName, fullname);
		});
}

std::string sfx::gui::_getTranslatedText(const std::string& fullname,
	const std::size_t index) const {
	return _getTranslatedText(std::get<sfx::gui::ListOfCaptions>(
		_originalCaptions.at(fullname)).at(index),
			[&](engine::logger* const logger, const std::string& typeName) {
				logger->warning("Unsupported type \"{}\" given when translating "
					"widget \"{}\"'s #{} caption: inserting blank string instead.",
					typeName, fullname, index);
		});
}

std::string sfx::gui::_extractWidgetName(const std::string& fullname) {
	if (fullname.rfind('.') == String::npos) {
		return fullname;
	} else {
		return fullname.substr(fullname.rfind('.') + 1);
	}
}

Widget::Ptr sfx::gui::_createWidget(const std::string& wType,
	const std::string& name, const std::string& menu) const {
	tgui::String type = tgui::String(wType).trim().toLower();
	if (type == "bitmapbutton") {
		auto button = tgui::BitmapButton::create();
		button->setCallback(std::bind(&sfx::gui::_drawCallback, this,
			std::placeholders::_1, std::placeholders::_2));
		return button;
	} else if (type == "listbox") {
		return tgui::ListBox::create();
	} else if (type == "verticallayout") {
		return tgui::VerticalLayout::create();
	} else if (type == "horizontallayout") {
		return tgui::HorizontalLayout::create();
	} else if (type == "picture") {
		const auto picture = tgui::Picture::create();
		picture->setDrawCallback(std::bind(&sfx::gui::_drawCallback, this,
			std::placeholders::_1, std::placeholders::_2));
		return picture;
	} else if (type == "label") {
		return tgui::Label::create();
	} else if (type == "scrollablepanel") {
		auto panel = tgui::ScrollablePanel::create();
		// Always set default scrollbar amounts to allow directional flow to work.
		panel->setHorizontalScrollAmount(5);
		panel->setVerticalScrollAmount(5);
		return panel;
	} else if (type == "panel") {
		return tgui::Panel::create();
	} else if (type == "group") {
		return tgui::Group::create();
	} else if (type == "grid") {
		return tgui::Grid::create();
	} else if (type == "button") {
		return tgui::Button::create();
	} else if (type == "editbox") {
		return tgui::EditBox::create();
	} else if (type == "menubar") {
		return tgui::MenuBar::create();
	} else if (type == "childwindow") {
		return tgui::ChildWindow::create();
	} else if (type == "combobox") {
		const auto combobox = tgui::ComboBox::create();
		combobox->setItemsToDisplay(5);
		return combobox;
	} else {
		_logger.error("Attempted to create a widget of type \"{}\" with name "
			"\"{}\" for menu \"{}\": that widget type is not supported.", wType,
			name, menu);
		return nullptr;
	}
}

std::pair<std::string, tgui::Widget::Ptr>
	sfx::gui::_findCurrentlySelectedWidget() {
	const auto& cursel = _currentlySelectedWidget[getGUI()].second;
	if (cursel.empty()) return {};
	const auto widget = _findWidget<Widget>(cursel);
	if (!widget) {
		_logger.error("Currently selected widget \"{}\" couldn't be found! "
			"Current menu is \"{}\". Deselecting...", cursel, getGUI());
		_currentlySelectedWidget.erase(getGUI());
		return {};
	}
	return { cursel, widget };
}

//////////////////////
// SCRIPT INTERFACE //
//////////////////////

#include "guimacros.hpp"

void sfx::gui::_setGUI(const std::string& name) {
	setGUI(name, true, true);
}

void sfx::gui::_noBackground(std::string menu) {
	if (menu == "") menu = getGUI();
	_guiBackground.erase(menu);
}

void sfx::gui::_spriteBackground(std::string menu, const std::string& sheet,
	const std::string& sprite) {
	START();
	if (menu == "") menu = getGUI();
	try {
		_guiBackground[menu].set(_sheet.at(sheet), sprite);
	} catch (const std::out_of_range&) {
		ERROR("This sheet does not exist!");
	}
	END("Attempted to set sprite \"{}\" from sheet \"{}\" to the background of "
		"menu \"{}\".", sprite, sheet, menu);
}

void sfx::gui::_colourBackground(std::string menu, const unsigned int r,
	const unsigned int g, const unsigned int b, const unsigned int a) {
	if (menu == "") menu = getGUI();
	_guiBackground[menu].set(sf::Color(r, g, b, a));
}

bool sfx::gui::_widgetExists(const std::string& name) {
	return _findWidget<Widget>(name).operator bool();
}

bool sfx::gui::_menuExists(const std::string& menu) {
	// More efficient implementation would just cache the menu list, as menus can
	// only be added or removed via load().
	const auto& menus = _gui.getWidgets();
	for (const auto& widget : menus)
		if (widget->getWidgetName() == menu) return true;
	return false;
}

void sfx::gui::_addWidget(const std::string& newWidgetType,
	const std::string& name, const std::string& signalHandler) {
	START_WITH_NONEXISTENT_WIDGET(name)
	if (widget = _createWidget(newWidgetType, name, fullname[0])) {
		container->add(widget, fullnameAsString);
		_connectSignals(widget, signalHandler);
		// If the widget is a ChildWindow, don't forget to turn on automatic
		// handling of minimise and maximise, apply all titlebar buttons by
		// default, and make it resizable.
		if (widget->getWidgetType() == "ChildWindow") {
			_childWindowData[fullnameAsString];
			const auto win = std::dynamic_pointer_cast<ChildWindow>(widget);
			win->setTitleButtons(ChildWindow::TitleButton::Close |
				ChildWindow::TitleButton::Minimize |
				ChildWindow::TitleButton::Maximize);
			win->setResizable();
		}
	} else {
		ERROR("Could not create the new widget.")
	}
	END("Attempted to create a new \"{}\" widget with name \"{}\".", newWidgetType,
		name)
}

void sfx::gui::_addWidgetToGrid(const std::string& newWidgetType,
	const std::string& name, const std::size_t row, const std::size_t col,
	const std::string& signalHandler) {
	START_WITH_NONEXISTENT_WIDGET(name)
	if (widget = _createWidget(newWidgetType, name, fullname[0])) {
		if (container->getWidgetType() != "Grid") {
			ERROR(std::string("The widget \"").append(containerName).append("\" "
				"is of type \"").append(container->getWidgetType().toStdString()).
				append("\", not Grid."))
		} else {
			widget->setWidgetName(fullnameAsString);
			_connectSignals(widget, signalHandler);
			std::dynamic_pointer_cast<Grid>(container)->addWidget(widget, row,
				col);
		}
	}
	END("Attempted to create a new \"{}\" widget with name \"{}\" and add it to a "
		"grid at row {}, column {}.", newWidgetType, name, row, col)
}

void sfx::gui::_removeWidget(const std::string& name) {
	START_WITH_WIDGET(name)
	if (fullname.size() < 2) ERROR("Removing entire menus is not supported.")
	_removeWidgets(widget, container, true);
	END("Attempted to remove the widget \"{}\" within menu \"{}\".", name,
		fullname[0])
}

void sfx::gui::_removeWidgetsFromContainer(const std::string& name) {
	START_WITH_WIDGET(name)
	if (fullname.size() < 2) {
		_removeWidgets(widget, nullptr, false);
	} else {
		if (_isContainerWidget(widgetType.toLower().toStdString()))
			_removeWidgets(widget, container, false);
		else
			UNSUPPORTED_WIDGET_TYPE()
	}
	END("Attempted to remove the widgets from a widget \"{}\", of type \"{}\", "
		"within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetFocus(const std::string& name) {
	START_WITH_WIDGET(name)
	widget->setFocused(true);
	END("Attempted to set the focus to a widget \"{}\" within menu \"{}\".", name,
		fullname[0])
}

void sfx::gui::_setWidgetFont(const std::string& name,
	const std::string& fontName) {
	START_WITH_WIDGET(name)
	if (!_fonts) ERROR("No fonts object has been given to this gui object.")
	auto fontPath = _fonts->getFontPath(fontName);
	// Invalid font name will be logged by fonts class.
	if (!fontPath.empty()) {
		auto font = tgui::Font(fontPath);
		font.setSmooth(false);
		widget->getRenderer()->setFont(font);
	}
	END("Attempted to set the font \"{}\" to a widget \"{}\" within menu \"{}\".",
		fontName, name, fullname[0])
}

void sfx::gui::_setGlobalFont(const std::string& fontName) {
	START()
	if (!_fonts) ERROR("No fonts object has been given to this gui object.")
	auto fontPath = _fonts->getFontPath(fontName);
	// Invalid font name will be logged by fonts class.
	if (!fontPath.empty()) {
		auto font = tgui::Font(fontPath);
		font.setSmooth(false);
		_gui.setFont(font);
	}
	END("Attempted to set the font \"{}\" as the global font.", fontName)
}

void sfx::gui::_setWidgetPosition(const std::string& name, const std::string& x,
	const std::string& y) {
	START_WITH_WIDGET(name)
	widget->setPosition(x.c_str(), y.c_str());
	END("Attempted to set the position (\"{}\",\"{}\") to a widget \"{}\" within "
		"menu \"{}\".", x, y, name, fullname[0])
}

sf::Vector2f sfx::gui::_getWidgetAbsolutePosition(const std::string& name) {
	START_WITH_WIDGET(name)
	return widget->getAbsolutePosition();
	END("Attempted to get the absolute position of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return {};
}

void sfx::gui::_setWidgetOrigin(const std::string& name, const float x,
	const float y) {
	START_WITH_WIDGET(name)
	widget->setOrigin(x, y);
	END("Attempted to set the origin ({},{}) to a widget \"{}\" within menu "
		"\"{}\".", x, y, name, fullname[0])
}

void sfx::gui::_setWidgetSize(const std::string& name, const std::string& w,
	const std::string& h) {
	START_WITH_WIDGET(name)
	widget->setSize(w.c_str(), h.c_str());
	END("Attempted to set the size (\"{}\",\"{}\") to a widget \"{}\" within menu "
		"\"{}\".", w, h, name, fullname[0])
}

sf::Vector2f sfx::gui::_getWidgetFullSize(const std::string& name) {
	START_WITH_WIDGET(name)
	return widget->getFullSize();
	END("Attempted to get the full size of a widget \"{}\" within menu \"{}\".",
		name, fullname[0])
	return {};
}

void sfx::gui::_setWidgetEnabled(const std::string& name, const bool enable) {
	START_WITH_WIDGET(name)
	widget->setEnabled(enable);
	END("Attempted to update widget \"{}\"'s enabled state, within menu \"{}\"",
		name, fullname[0])
}

bool sfx::gui::_getWidgetEnabled(const std::string& name) const {
	START_WITH_WIDGET(name)
	return widget->isEnabled();
	END("Attempted to get the enabled property of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return false;
}

void sfx::gui::_setWidgetVisibility(const std::string& name, const bool visible) {
	START_WITH_WIDGET(name)
	widget->setVisible(visible);
	END("Attempted to update widget \"{}\"'s visibility, within menu \"{}\".",
		name, fullname[0])
}

bool sfx::gui::_getWidgetVisibility(const std::string& name) const {
	START_WITH_WIDGET(name)
	return widget->isVisible();
	END("Attempted to get the visibility property of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return false;
}

const std::string sfx::gui::GOTO_PREVIOUS_WIDGET = "~";

void sfx::gui::_setWidgetDirectionalFlow(const std::string& name,
	const std::string& upName, const std::string& downName,
	const std::string& leftName, const std::string& rightName) {
	std::vector<std::string> fullname, fullnameUp, fullnameDown, fullnameLeft,
		fullnameRight;
	std::string fullnameAsString, fullnameAsStringUp, fullnameAsStringDown,
		fullnameAsStringLeft, fullnameAsStringRight;
	static const auto widgetDoesNotExist = [&](const std::string& doesNotExist) {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"within menu \"{}\", to the widgets up=\"{}\", down=\"{}\", "
			"left=\"{}\", right=\"{}\". The widget \"{}\" does not exist.", name,
			fullname[0], upName, downName, leftName, rightName, doesNotExist);
	};
	if (!_findWidget<Widget>(name, &fullname, &fullnameAsString)) {
		widgetDoesNotExist(name);
		return;
	}
	if (!upName.empty() && upName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(upName, &fullnameUp, &fullnameAsStringUp)) {
		widgetDoesNotExist(upName);
		return;
	}
	if (!downName.empty() && downName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(downName, &fullnameDown, &fullnameAsStringDown)) {
		widgetDoesNotExist(downName);
		return;
	}
	if (!leftName.empty() && leftName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(leftName, &fullnameLeft, &fullnameAsStringLeft)) {
		widgetDoesNotExist(leftName);
		return;
	}
	if (!rightName.empty() && rightName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(rightName, &fullnameRight, &fullnameAsStringRight)) {
		widgetDoesNotExist(rightName);
		return;
	}
	if ((fullnameUp.empty() || fullname[0] == fullnameUp[0]) &&
		(fullnameDown.empty() || fullname[0] == fullnameDown[0]) &&
		(fullnameLeft.empty() || fullname[0] == fullnameLeft[0]) &&
		(fullnameRight.empty() || fullname[0] == fullnameRight[0])) {
		_directionalFlow[fullnameAsString].up =
			upName == GOTO_PREVIOUS_WIDGET ? upName : fullnameAsStringUp;
		_directionalFlow[fullnameAsString].down =
			downName == GOTO_PREVIOUS_WIDGET ? downName : fullnameAsStringDown;
		_directionalFlow[fullnameAsString].left =
			leftName == GOTO_PREVIOUS_WIDGET ? leftName : fullnameAsStringLeft;
		_directionalFlow[fullnameAsString].right =
			rightName == GOTO_PREVIOUS_WIDGET ? rightName : fullnameAsStringRight;
	} else {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"within menu \"{}\", to the widgets up=\"{}\", down=\"{}\", "
			"left=\"{}\", right=\"{}\". Not all of these widgets are in the same "
			"menu!", name, fullname[0], fullnameAsStringUp, fullnameAsStringDown,
			fullnameAsStringLeft, fullnameAsStringRight);
	}
}

void sfx::gui::_setWidgetDirectionalFlowStart(const std::string& name) {
	START_WITH_WIDGET(name)
	_selectThisWidgetFirst[fullname[0]] = fullnameAsString;
	END("Attempted to set the widget \"{}\" as the first to be selected upon "
		"initial directional input, for the menu \"{}\".", name, fullname[0])
}

void sfx::gui::_clearWidgetDirectionalFlowStart(const std::string& menu) {
	if (_menuExists(menu)) {
		_selectThisWidgetFirst.erase(menu);
	} else {
		_logger.error("Attempted to disable directional input for the menu "
			"\"{}\". Menu does not exist.", menu);
	}
}

void sfx::gui::_setWidgetDirectionalFlowSelection(const std::string& name) {
	START_WITH_WIDGET(name)
	_makeNewDirectionalSelection(fullnameAsString, fullname[0]);
	END("Attempted to manually directionally select the widget \"{}\", in the "
		"menu \"{}\".", name, fullname[0])
}

void sfx::gui::_setDirectionalFlowAngleBracketSprite(const std::string& corner,
	const std::string& sheet, const std::string& key) {
	START()
	const auto spritesheet = _sheet.find(sheet);
	if (spritesheet == _sheet.end()) ERROR("This spritesheet does not exist.")
	if (!spritesheet->second->doesSpriteExist(key))
		ERROR("This sprite does not exist.")
	const auto cornerFormatted = tgui::String(corner).trim().toLower();
	if (cornerFormatted == "ul") {
		_angleBracketUL.setSpritesheet(spritesheet->second);
		_angleBracketUL.setSprite(key);
	} else if (cornerFormatted == "ur") {
		_angleBracketUR.setSpritesheet(spritesheet->second);
		_angleBracketUR.setSprite(key);
	} else if (cornerFormatted == "ll") {
		_angleBracketLL.setSpritesheet(spritesheet->second);
		_angleBracketLL.setSprite(key);
	} else if (cornerFormatted == "lr") {
		_angleBracketLR.setSpritesheet(spritesheet->second);
		_angleBracketLR.setSprite(key);
	} else {
		ERROR("Unrecognised corner, must be \"UL\", \"UR\", \"LL\", or \"LR\".")
	}
	END("Attempted to set the sprite \"{}\" from spritesheet \"{}\" as the "
		"directional flow angle bracket for the \"{}\" corner.", key, sheet,
		corner)
}

void sfx::gui::_setWidgetText(const std::string& name, const std::string& text,
	CScriptArray* variables) {
	START_WITH_WIDGET(name)
	// For EditBoxes, don't translate the text, as this is text that the user can
	// edit.
	if (widgetType == "EditBox") {
		std::dynamic_pointer_cast<EditBox>(widget)->setText(text);
	} else {
		if (widgetType != "BitmapButton" && widgetType != "Label" &&
			widgetType != "Button" && widgetType != "ChildWindow")
				UNSUPPORTED_WIDGET_TYPE()
		_setTranslatedString(fullnameAsString, text, variables);
		_translateWidget(widget);
	}
	END("Attempted to set the caption \"{}\" to a widget \"{}\" of type \"{}\" "
		"within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

std::string sfx::gui::_getWidgetText(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(EditBox, return castWidget->getText().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of a widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", name, widgetType, fullname[0])
	return "";
}

void sfx::gui::_setWidgetTextSize(const std::string& name,
	const unsigned int size) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(BitmapButton, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(Button, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(EditBox, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(MenuBar, castWidget->setTextSize(size);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the character size {} to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", size, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->getRenderer()->setTextColor(colour);)
		ELSE_IF_WIDGET_IS(EditBox,
			castWidget->getRenderer()->setTextColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text colour \"{}\" to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", colour, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextOutlineColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline colour \"{}\" to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", colour, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetTextOutlineThickness(const std::string& name,
	const float thickness) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineThickness(thickness);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline thickness {} to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", thickness, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetTextAlignment(const std::string& name,
	const tgui::Label::HorizontalAlignment h,
	const tgui::Label::VerticalAlignment v) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->setHorizontalAlignment(h);
			castWidget->setVerticalAlignment(v);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text horizontal alignment {} and vertical alignment "
		"{} to widget \"{}\", which is of type \"{}\", within menu \"{}\".", h, v,
		name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetSprite(const std::string& name, const std::string& sheet,
	const std::string& key) {
	START_WITH_WIDGET(name)
	if (widgetType != "BitmapButton" && widgetType != "Picture")
		UNSUPPORTED_WIDGET_TYPE()
	// Prevent deleting sprite objects if there won't be any change.
	if (_guiSpriteKeys[fullnameAsString].first != sheet ||
		_guiSpriteKeys[fullnameAsString].second != key) {
		_guiSpriteKeys[fullnameAsString] = std::make_pair(sheet, key);
		_widgetSprites.erase(widget);
	}
	END("Attempted to set the sprite \"{}\" from sheet \"{}\" to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", key, sheet, name,
		widgetType, fullname[0])
}

void sfx::gui::_clearWidgetSprite(const std::string& name) {
	START_WITH_WIDGET(name)
	if (widgetType != "BitmapButton" && widgetType != "Picture")
		UNSUPPORTED_WIDGET_TYPE()
	_guiSpriteKeys.erase(fullnameAsString);
	_widgetSprites.erase(widget);
	END("Attempted to clear the sprite from widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_matchWidgetSizeToSprite(const std::string& name,
	const bool overrideSetSize) {
	START_WITH_WIDGET(name)
	if (widgetType != "Picture") UNSUPPORTED_WIDGET_TYPE()
	if (overrideSetSize)
		_dontOverridePictureSizeWithSpriteSize.erase(fullnameAsString);
	else
		_dontOverridePictureSizeWithSpriteSize.insert(fullnameAsString);
	END("Attempted to match widget \"{}\"'s size to its set sprite. The widget is "
		"of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetBgColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_IF_WIDGET_IS(ScrollablePanel,
			castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the background colour \"{}\" to widget \"{}\", which is "
		"of type \"{}\", within menu \"{}\".", colour, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetBorderSize(const std::string& name, const float size) {
	START_WITH_WIDGET(name);
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBorders(size);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border size of {} to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", size, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetBorderColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setBorderColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border colour of {} to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", colour, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetBorderRadius(const std::string& name,
	const float radius) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setRoundedBorderRadius(radius);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the border radius {} to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", radius, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetIndex(const std::string& name, const std::size_t index) {
	START_WITH_WIDGET(name)
	if (fullname.size() < 2)
		ERROR("This is operation is unsupported for entire menus.")
	if (!container->setWidgetIndex(widget, index)) {
		// The size() should never be 0 here...
		ERROR(std::string("The index cannot be higher than ").append(
			std::to_string(container->getWidgets().size() - 1)).append("."))
	}
	END("Attempted to set a widget \"{}\"'s index to {}.", name, index)
}

void sfx::gui::_setWidgetIndexInContainer(const std::string& name,
	const std::size_t oldIndex, const std::size_t newIndex) {
	START_WITH_WIDGET(name)
	if (!_isContainerWidget(widgetType)) UNSUPPORTED_WIDGET_TYPE()
	container = std::dynamic_pointer_cast<Container>(widget);
	try {
		widget = container->getWidgets().at(oldIndex);
	} catch (const std::out_of_range&) {
		ERROR("This container does not have a widget with that number.")
	}
	if (!container->setWidgetIndex(widget, newIndex)) {
		const auto count = container->getWidgets().size();
		if (count) {
			ERROR(std::string("The new index cannot be higher than ").append(
				std::to_string(count - 1)).append("."))
		} else {
			ERROR("This container has no widgets.")
		}
	}
	END("Attempted to set the widget \"{}\"'s number {} widget to an index of {}, "
		"within menu \"{}\". The widget is of type \"{}\".", name, oldIndex,
		newIndex, fullname[0], widgetType)
}

void sfx::gui::_setWidgetRatioInLayout(const std::string& name,
	const std::size_t index, const float ratio) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(HorizontalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_IF_WIDGET_IS(VerticalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_UNSUPPORTED()
	END("Attempted to set the widget ratio {} to widget {} in widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", ratio, index, name,
		widgetType, fullname[0])
}

void sfx::gui::_setWidgetDefaultText(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	START_WITH_WIDGET(name)
	if (widgetType != "EditBox") UNSUPPORTED_WIDGET_TYPE()
	_setTranslatedString(fullnameAsString, text, variables);
	_translateWidget(widget);
	END("Attempted to set the default text \"{}\" to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

void sfx::gui::_addItem(const std::string& name, const std::string& text,
	CScriptArray* variables) {
	START_WITH_WIDGET(name)
	std::size_t index = 0;
		IF_WIDGET_IS(ListBox,
			const auto limit = castWidget->getMaximumItems();
			index = castWidget->addItem(text);
			if (limit > 0 && index == limit) {
				ERROR(std::string("This widget has reached its configured maximum "
					"number of items, which is ").append(std::to_string(limit)).
					append("."))
			}
		)
		ELSE_IF_WIDGET_IS(ComboBox,
			const auto limit = castWidget->getMaximumItems();
			index = castWidget->addItem(text);
			if (limit > 0 && index == limit) {
				ERROR(std::string("This widget has reached its configured maximum "
					"number of items, which is ").append(std::to_string(limit)).
					append("."))
			}
		)
		ELSE_UNSUPPORTED()
	_setTranslatedString(fullnameAsString, text, variables, index);
	_translateWidget(widget);
	END("Attempted to add an item \"{}\" to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

void sfx::gui::_clearItems(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, castWidget->removeAllItems();)
		ELSE_IF_WIDGET_IS(ComboBox, castWidget->removeAllItems();)
		ELSE_UNSUPPORTED()
	_originalCaptions.erase(fullnameAsString);
	END("Attempted to clear all items from widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_setSelectedItem(const std::string& name, const std::size_t index) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox,
			if (!castWidget->setSelectedItemByIndex(index)) {
				const auto count = castWidget->getItemCount();
				if (count) {
					ERROR(std::string("The item index cannot be higher than ").
						append(std::to_string(count - 1)).append("."))
				} else {
					ERROR("This widget has no items.")
				}
			}
		)
		ELSE_IF_WIDGET_IS(ComboBox,
			if (!castWidget->setSelectedItemByIndex(index)) {
				const auto count = castWidget->getItemCount();
				if (count) {
					ERROR(std::string("The item index cannot be higher than ").
						append(std::to_string(count - 1)).append("."))
				} else {
					ERROR("This widget has no items.")
				}
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to select item {} from widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", index, name, widgetType, fullname[0])
}

int sfx::gui::_getSelectedItem(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItemIndex();)
		ELSE_IF_WIDGET_IS(ComboBox, return castWidget->getSelectedItemIndex();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the index of the selected item of a widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", name, widgetType,
		fullname[0])
	return -1;
}

std::string sfx::gui::_getSelectedItemText(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItem().toStdString();)
		ELSE_IF_WIDGET_IS(ComboBox,
			return castWidget->getSelectedItem().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of the selected item of a widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return "";
}

std::size_t sfx::gui::_getWidgetCount(const std::string& name) {
	START_WITH_WIDGET(name)
	if (_isContainerWidget(widgetType))
		return std::dynamic_pointer_cast<Container>(widget)->getWidgets().size();
	else
		UNSUPPORTED_WIDGET_TYPE()
	END("Attempted to get the widget count of a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return 0;
}

void sfx::gui::_setHorizontalScrollbarPolicy(const std::string& name,
	const tgui::Scrollbar::Policy policy) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollbarPolicy(policy);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar policy {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", policy, name, widgetType,
		fullname[0])
}

void sfx::gui::_setHorizontalScrollbarAmount(const std::string& name,
	const unsigned int amount) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", amount, name, widgetType,
		fullname[0])
}

void sfx::gui::_setVerticalScrollbarAmount(const std::string& name,
	const unsigned int amount) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setVerticalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the vertical scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", amount, name, widgetType,
		fullname[0])
}

void sfx::gui::_setGroupPadding(const std::string& name,
	const std::string& padding) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(Panel, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(HorizontalLayout, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(VerticalLayout, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(Group, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_UNSUPPORTED()
	END("Attempted to set a padding {} to widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", padding, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetAlignmentInGrid(const std::string& name,
	const std::size_t row, const std::size_t col,
	const tgui::Grid::Alignment alignment) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Grid,
			auto & table = castWidget->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size()) {
					castWidget->setWidgetAlignment(row, col, alignment);
				} else {
					ERROR("The column index is out of range.")
				}
			} else {
				ERROR("The row index is out of range.")
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set an alignment {} to a widget \"{}\", which is of type "
		"\"{}\", @ ({}, {}), within menu \"{}\".", alignment, name, widgetType,
		row, col, fullname[0])
}

void sfx::gui::_setSpaceBetweenWidgets(const std::string& name,
	const float space) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(HorizontalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_IF_WIDGET_IS(VerticalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_UNSUPPORTED()
	END("Attempted to set {} to a widget \"{}\"'s space between widgets property. "
		"The widget is of type \"{}\", within menu \"{}\".", space, name,
		widgetType, fullname[0])
}

// MENUS //

sfx::gui::MenuItemID sfx::gui::_addMenu(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			if (_hierarchyOfLastMenuItem[fullnameAsString].size() == 1) {
				_logger.warning("Menu \"{}\" in MenuBar \"{}\" is empty!",
					_hierarchyOfLastMenuItem[fullnameAsString][0],
						fullnameAsString);
			}
			castWidget->addMenu(text);
			_hierarchyOfLastMenuItem[fullnameAsString] = { text };
			if (_menuCounter.find(fullnameAsString) == _menuCounter.end())
				_menuCounter.emplace(fullnameAsString, 0);
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu \"{}\" to a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
	return ret;
}

sfx::gui::MenuItemID sfx::gui::_addMenuItem(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
			auto copy(hierarchy);
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() == 1) {
				hierarchy.push_back(text);
			} else {
				hierarchy.back() = text;
			}
			if (!castWidget->addMenuItem(hierarchy)) {
				std::string error = "Could not add item with hierarchy: ";
				for (sfx::gui::MenuItemID i = 0, len = hierarchy.size(); i < len;
					++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy = copy;
				ERROR(error);
			}
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, name,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu item \"{}\" to a widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
	return ret;
}

sfx::gui::MenuItemID sfx::gui::_addMenuItemIntoLastItem(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() == 1) {
				_logger.warning("Calling addMenuItemIntoLastItem() when "
					"addMenuItem() was likely intended.");
			}
			hierarchy.push_back(text);
			if (!castWidget->addMenuItem(hierarchy)) {
				std::string error = "Could not add item with hierarchy: ";
				for (sfx::gui::MenuItemID i = 0, len = hierarchy.size(); i < len;
					++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy.pop_back();
				ERROR(error);
			}
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, name,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to create a new submenu with item \"{}\" in a widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", text, name, widgetType,
		fullname[0])
	if (variables) variables->Release();
	return ret;
}

void sfx::gui::_exitSubmenu(const std::string& name) {
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() < 3) {
				ERROR("Not currently in a submenu!");
			}
			hierarchy.pop_back();
		)
		ELSE_UNSUPPORTED()
	END("Attempted to exit the current submenu of widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_autoHandleMinMax(const std::string& name, const bool handle) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			if (handle)
				_childWindowData.erase(fullnameAsString);
			else
				_childWindowData[fullnameAsString];
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the autoHandleMinMax property to {}, for the widget "
		"\"{}\", which is of type \"{}\", within menu \"{}\".", handle, name,
		widgetType, fullname[0]);
}

void sfx::gui::_setChildWindowTitleButtons(const std::string& name,
	const unsigned int buttons) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setTitleButtons(buttons);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the titlebar button mask {} to the widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", buttons, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetResizable(const std::string& name, const bool resizable) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setResizable(resizable);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the resizability property of widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\", to {}.", name, widgetType, fullname[0],
		resizable);
}

void sfx::gui::_restoreChildWindow(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			if (_childWindowData.find(fullnameAsString) != _childWindowData.end()) {
				_restoreChildWindowImpl(castWidget,
					_childWindowData[fullnameAsString]);
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to restore the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\".", name, widgetType, fullname[0]);
}

void sfx::gui::_restoreChildWindowImpl(const tgui::ChildWindow::Ptr& window,
	child_window_properties& data) {
	if (data.isMinimised || data.isMaximised) {
		if (data.isMinimised) {
			_minimisedChildWindowList[
				window->getParent()->getWidgetName().toStdString()].
				restore(window->getWidgetName().toStdString());
		}
		data.restore(window);
		data.isMinimised = false;
		data.isMaximised = false;
	}
}
