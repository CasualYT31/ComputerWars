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
#include "fmtformatter.hpp"

#undef MessageBox

using namespace tgui;

// These values are intended to be constant.
float NO_SPACE = -0.001f;
sf::Color NO_COLOUR(0, 0, 0, 0);

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

bool sfx::gui::gui_background::animate(const sf::RenderTarget& target,
	const double scaling) {
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
	engine::RegisterVectorTypes(engine, document);

	auto r = engine->RegisterEnum("WidgetAlignment");
	document->DocumentObjectEnum(r, "Values representing widget alignments in a "
		"grid.");
	engine->RegisterEnumValue("WidgetAlignment", "Centre",
		(int)Grid::Alignment::Center);
	engine->RegisterEnumValue("WidgetAlignment", "UpperLeft",
		(int)Grid::Alignment::UpperLeft);
	engine->RegisterEnumValue("WidgetAlignment", "Up",
		(int)Grid::Alignment::Up);
	engine->RegisterEnumValue("WidgetAlignment", "UpperRight",
		(int)Grid::Alignment::UpperRight);
	engine->RegisterEnumValue("WidgetAlignment", "Right",
		(int)Grid::Alignment::Right);
	engine->RegisterEnumValue("WidgetAlignment", "BottomRight",
		(int)Grid::Alignment::BottomRight);
	engine->RegisterEnumValue("WidgetAlignment", "Bottom",
		(int)Grid::Alignment::Bottom);
	engine->RegisterEnumValue("WidgetAlignment", "BottomLeft",
		(int)Grid::Alignment::BottomLeft);
	engine->RegisterEnumValue("WidgetAlignment", "Left",
		(int)Grid::Alignment::Left);

	r = engine->RegisterEnum("ScrollbarPolicy");
	document->DocumentObjectEnum(r, "Values representing scollbar policies.");
	engine->RegisterEnumValue("ScrollbarPolicy", "Automatic",
		(int)Scrollbar::Policy::Automatic);
	engine->RegisterEnumValue("ScrollbarPolicy", "Always",
		(int)Scrollbar::Policy::Always);
	engine->RegisterEnumValue("ScrollbarPolicy", "Never",
		(int)Scrollbar::Policy::Never);

	r = engine->RegisterEnum("HorizontalAlignment");
	document->DocumentObjectEnum(r, "Values representing horizontal alignment.");
	engine->RegisterEnumValue("HorizontalAlignment", "Left",
		(int)Label::HorizontalAlignment::Left);
	engine->RegisterEnumValue("HorizontalAlignment", "Centre",
		(int)Label::HorizontalAlignment::Center);
	engine->RegisterEnumValue("HorizontalAlignment", "Right",
		(int)Label::HorizontalAlignment::Right);

	r = engine->RegisterEnum("VerticalAlignment");
	document->DocumentObjectEnum(r, "Values representing vertical alignment.");
	engine->RegisterEnumValue("VerticalAlignment", "Top",
		(int)Label::VerticalAlignment::Top);
	engine->RegisterEnumValue("VerticalAlignment", "Centre",
		(int)Label::VerticalAlignment::Center);
	engine->RegisterEnumValue("VerticalAlignment", "Bottom",
		(int)Label::VerticalAlignment::Bottom);

	// Register global constants.
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
		"that was emitted.");

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
	document->DocumentGlobalFunction(r, "Sets the widgets that should be selected "
		"if directional controls are input when the given widget is currently "
		"selected. The \"given widget\" should be given first, followed by the "
		"widgets that should be selected, when up, down, left, and right are "
		"input, respectively. All given widgets must be in the same menu!");

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
			closeFuncDecl = "void " + _currentGUI + "Close(const string&in)",
			newMenu = newPanel;
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
	} catch (tgui::Exception& e) {
		_logger.error("{}", e.what());
		if (_gui.get(old)) _gui.get(old)->setVisible(true);
	}
}

void sfx::gui::addSpritesheet(const std::string& name,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	if (_sheet.find(name) != _sheet.end()) {
		_logger.warning("Updated the spritesheet named {}!", name);
	}
	_sheet[name] = sheet;
}

void sfx::gui::setTarget(sf::RenderTarget& newTarget) {
	_gui.setTarget(newTarget);
}

bool sfx::gui::handleEvent(sf::Event e) {
	return _gui.handleEvent(e);
}

void sfx::gui::handleInput(const std::shared_ptr<sfx::user_input>& ui) {
	if (ui) {
		// Invoke the current menu's bespoke input handling function.
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
		// Keep track of mouse movement. If the mouse has moved, then we disregard
		// directional flow (and select inputs) until a new directional input has
		// been made.
		_previousMousePosition = _currentMousePosition;
		_currentMousePosition = ui->mousePosition();
		if (_previousMousePosition != _currentMousePosition)
			_enableDirectionalFlow = false;
		// Handle directional input.
		if (_enableDirectionalFlow) {
			const auto cursel = _moveDirectionalFlow(ui);
			// If select is issued, and there is currently a widget selected, then
			// trigger an appropriate signal handler.
			if ((*ui)[_selectControl] && !cursel.empty()) {
				const auto widget = _findWidget<Widget>(cursel);
				const auto widgetType = widget->getWidgetType();
				if (widgetType == "Button" || widgetType == "BitmapButton") {
					signalHandler(widget, "MouseReleased");
				}
			}
		} else if (_previousMousePosition == _currentMousePosition) {
			// Only re-enable directional flow if a directional input is made,
			// whilst the mouse isn't moving.
			_enableDirectionalFlow = (*ui)[_upControl] || (*ui)[_downControl] ||
				(*ui)[_leftControl] || (*ui)[_rightControl];
			// If there wasn't a selection made previously, go straight to making
			// the selection.
			if (_currentlySelectedWidget[_currentGUI].empty())
				_moveDirectionalFlow(ui);
		}
	} else if (!_handleInputErrorLogged) {
		_logger.error("Called handleInput() with nullptr user_input object for "
			"menu \"{}\"!", getGUI());
		_handleInputErrorLogged = true;
	}
}

void sfx::gui::signalHandler(tgui::Widget::Ptr widget,
	const tgui::String& signalName) {
	if (_scripts && getGUI() != "") {
		std::string fullname = widget->getWidgetName().toStdString();
		std::string signalNameStd = signalName.toStdString();
		auto customHandler = _customSignalHandlers.find(fullname);
		if (customHandler != _customSignalHandlers.end()) {
			std::string decl = "void " + customHandler->second +
				"(const string&in, const string&in)";
			if (_scripts->functionDeclExists(decl)) {
				_scripts->callFunction(customHandler->second, &fullname,
					&signalNameStd);
				return;
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
			_scripts->callFunction(functionName);
		}
	}
}

void sfx::gui::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& lang) {
	_lastlang = "";
	_langdict = lang;
}

void sfx::gui::setFonts(const std::shared_ptr<sfx::fonts>& fonts) noexcept {
	_fonts = fonts;
}

bool sfx::gui::animate(const sf::RenderTarget& target, const double scaling) {
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		_guiBackground.at(getGUI()).animate(target, scaling);
	}

	if (getGUI() != "") {
		if (_langdict && _langdict->getLanguage() != _lastlang) {
			_lastlang = _langdict->getLanguage();
			auto& widgetList = _gui.getWidgets();
			for (auto& widget : widgetList) _translateWidget(widget);
		}
		std::size_t animatedSprite = 0;
		// Update the menu's scaling factor.
		// We also need to update the size of the group container, as percentage
		// calculations made within the script setWidgetSize() calls will be off
		// otherwise.
		auto menu = _gui.get<Container>(getGUI());
		menu->setScale((float)scaling);
		tgui::String percentage(100.0f / (float)scaling);
		percentage += "%";
		menu->setSize(percentage);
		_animate(target, scaling, menu);
	}

	// Whenever there isn't a widget currently selected via directional controls,
	// always reset the animation.
	const auto& cursel = _currentlySelectedWidget[getGUI()];
	if (cursel.empty() || !_enableDirectionalFlow) {
		_angleBracketUL.setCurrentFrame(0);
		_angleBracketUR.setCurrentFrame(0);
		_angleBracketLL.setCurrentFrame(0);
		_angleBracketLR.setCurrentFrame(0);
	} else if (!cursel.empty()) {
		// Ensure the angle brackets are at the correct locations.
		std::string fullname;
		const auto widget = _findWidget<Widget>(cursel, nullptr, &fullname);
		if (widget) {
			const auto pos = widget->getAbsolutePosition(),
				size = widget->getSize();
			_angleBracketUL.setPosition(pos);
			_angleBracketUL.animate(target, scaling);
			_angleBracketUR.setPosition(pos + tgui::Vector2f(
				size.x - _angleBracketUR.getSize().x, 0.0f));
			_angleBracketUR.animate(target, scaling);
			_angleBracketLL.setPosition(pos + tgui::Vector2f(0.0f,
				size.y - _angleBracketLL.getSize().y));
			_angleBracketLL.animate(target, scaling);
			_angleBracketLR.setPosition(pos + size -
				tgui::Vector2f(_angleBracketLR.getSize()));
			_angleBracketLR.animate(target, scaling);
		} else {
			_logger.error("Currently selected widget \"{}\" couldn't be found! "
				"Current menu is \"{}\". Deselecting...", fullname, getGUI());
			_currentlySelectedWidget.erase(getGUI());
		}
	}

	return false;
}

void sfx::gui::_animate(const sf::RenderTarget& target, const double scaling,
	tgui::Container::Ptr container) {
	static auto allocImage = [&](const tgui::String& type, Widget::Ptr widget,
		const std::string& widgetName, const unsigned int w, const unsigned int h)
		-> void {
		// Create an empty texture
		sf::Uint8* pixels = (sf::Uint8*)calloc(w * h, 4);
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

				if (_widgetSprites.find(widgetName) == _widgetSprites.end()) {
					// Animated sprite for this widget doesn't exist yet, so
					// allocate it.
					_widgetSprites.insert({ widgetName, sfx::animated_sprite() });
				}
				auto& animatedSprite = _widgetSprites.at(widgetName);

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
					animatedSprite.animate(target, scaling);
					sf::Vector2f spriteSizeF = animatedSprite.getSize();
					sf::Vector2u spriteSize((unsigned int)spriteSizeF.x,
						(unsigned int)spriteSizeF.y);
					allocImage(type, widget, widgetName, spriteSize.x,
						spriteSize.y);
				} else {
					// If the widget's sprite hasn't changed, then simply animate
					// it.
					animatedSprite.animate(target, scaling);
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
				_widgetSprites.at(widgetName).getSpritesheet() != nullptr &&
				_sheet.find(_guiSpriteKeys.at(widgetName).first) == _sheet.end()) {
				// Else if the widget DID have a valid spritesheet, then we're
				// going to have to remove the image from the widget to ensure that
				// sizing works out.
				allocImage(type, widget, widgetName, 0, 0);
			}
		}

		if (_isContainerWidget(type)) {
			_animate(target, scaling,
				std::dynamic_pointer_cast<Container>(widget));
		}
	}
}

std::string sfx::gui::_moveDirectionalFlow(
	const std::shared_ptr<sfx::user_input>& ui) {
	auto& cursel = _currentlySelectedWidget[_currentGUI];
	if ((*ui)[_upControl]) {
		if (cursel.empty() && _selectThisWidgetFirst.find(_currentGUI) !=
			_selectThisWidgetFirst.end()) {
			cursel = _selectThisWidgetFirst[_currentGUI];
		} else if (_directionalFlow.find(cursel) != _directionalFlow.end()
			&& !_directionalFlow[cursel].up.empty()) {
			cursel = _directionalFlow[cursel].up;
		}
	}
	if ((*ui)[_downControl]) {
		_enableDirectionalFlow = true;
		if (cursel.empty() && _selectThisWidgetFirst.find(_currentGUI) !=
			_selectThisWidgetFirst.end()) {
			cursel = _selectThisWidgetFirst[_currentGUI];
		} else if (_directionalFlow.find(cursel) != _directionalFlow.end()
			&& !_directionalFlow[cursel].down.empty()) {
			cursel = _directionalFlow[cursel].down;
		}
	}
	if ((*ui)[_leftControl]) {
		_enableDirectionalFlow = true;
		if (cursel.empty() && _selectThisWidgetFirst.find(_currentGUI) !=
			_selectThisWidgetFirst.end()) {
			cursel = _selectThisWidgetFirst[_currentGUI];
		} else if (_directionalFlow.find(cursel) != _directionalFlow.end()
			&& !_directionalFlow[cursel].left.empty()) {
			cursel = _directionalFlow[cursel].left;
		}
	}
	if ((*ui)[_rightControl]) {
		_enableDirectionalFlow = true;
		if (cursel.empty() && _selectThisWidgetFirst.find(_currentGUI) !=
			_selectThisWidgetFirst.end()) {
			cursel = _selectThisWidgetFirst[_currentGUI];
		} else if (_directionalFlow.find(cursel) != _directionalFlow.end()
			&& !_directionalFlow[cursel].right.empty()) {
			cursel = _directionalFlow[cursel].right;
		}
	}
	return cursel;
}

void sfx::gui::_translateWidget(tgui::Widget::Ptr widget) {
	std::string widgetName = widget->getWidgetName().toStdString();
	String type = widget->getWidgetType();
	if (!_originalStrings[widgetName].empty()) {
		if (type == "Button") {
			auto w = _findWidget<Button>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "BitmapButton") {
			auto w = _findWidget<BitmapButton>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "CheckBox") {
			auto w = _findWidget<CheckBox>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "ChildWindow") {
			auto w = _findWidget<ChildWindow>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
		} else if (type == "ColorPicker") {
			auto w = _findWidget<ColorPicker>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
		} else if (type == "ComboBox") {
			auto w = _findWidget<ComboBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); i++) {
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
			}
		} else if (type == "EditBox") {
			auto w = _findWidget<EditBox>(widgetName);
			w->setDefaultText(_getTranslatedText(widgetName, 0));
		} else if (type == "FileDialog") {
			auto w = _findWidget<FileDialog>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
		} else if (type == "Label") {
			auto w = _findWidget<Label>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "ListBox") {
			auto w = _findWidget<ListBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); i++) {
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
			}
		} else if (type == "ListView") {
			auto w = _findWidget<ListView>(widgetName);
			std::size_t colCount = w->getColumnCount();
			for (std::size_t i = 0; i < colCount; i++) {
				w->setColumnText(i, _getTranslatedText(widgetName, i));
				for (std::size_t j = 0; j <= w->getItemCount(); j++) {
					w->changeSubItem(i, j,
						_getTranslatedText(widgetName, colCount * (i + 1) + j)
					);
				}
			}
		} else if (type == "MenuBar") {
			auto w = _findWidget<MenuBar>(widgetName);
			// It's possible, but we would somehow need to store the menu hierarchy
			// separately to keep this as simple as possible. Potentially multiple
			// menu hierarchies would have to be stored, though...
		} else if (type == "MessageBox") {
			auto w = _findWidget<tgui::MessageBox>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
			w->setText(_getTranslatedText(widgetName, 1));
			// Don't know how I'm going to translate buttons.
		} else if (type == "ProgressBar") {
			auto w = _findWidget<ProgressBar>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "RadioButton") {
			auto w = _findWidget<RadioButton>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		} else if (type == "TabContainer") {
			auto w = _findWidget<TabContainer>(widgetName);
			for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); i++) {
				w->changeTabText(i, _getTranslatedText(widgetName, i));
			}
		} else if (type == "Tabs") {
			auto w = _findWidget<Tabs>(widgetName);
			for (std::size_t i = 0; i < w->getTabsCount(); i++) {
				w->changeText(i, _getTranslatedText(widgetName, i));
			}
		} else if (type == "ToggleButton") {
			auto w = _findWidget<ToggleButton>(widgetName);
			w->setText(_getTranslatedText(widgetName, 0));
		}
	}
	if (_isContainerWidget(type)) {
		auto w = _findWidget<Container>(widgetName);
		auto& widgetList = w->getWidgets();
		for (auto& w : widgetList) _translateWidget(w);
	}
}

std::string sfx::gui::_getTranslatedText(const std::string& name,
	const std::size_t index) const {
	std::string ret = (*_langdict)(_originalStrings.at(name).at(index));
	// If there are any variables, insert them manually.
	if (_originalStringsVariables.find(name) != _originalStringsVariables.end()) {
		// Code will crash if you don't set up _originalStrings and
		// _originalStringsVariables entries together!
		for (auto& var : _originalStringsVariables.at(name).at(index)) {
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
				_logger.warning("Unsupported type \"{}\" given when translating "
					"widget \"{}\"'s #{} string: inserting blank string instead.",
					_scripts->getTypeName(type), name, index);
				ret = engine::expand_string::insert(ret, "");
			}
		}
	}
	return ret;
}

void sfx::gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draw background.
	if (_guiBackground.find(getGUI()) != _guiBackground.end()) {
		// This GUI has a background to animate.
		target.draw(_guiBackground.at(getGUI()), states);
	}
	// Draw foreground.
	_gui.draw();
	// Draw each widget's sprite if the widget isn't hidden.
	sf::View oldView = target.getView();
	target.setView(_gui.calculateGUIView());
	for (auto& sprite : _widgetSprites) {
		Widget::Ptr widget = _findWidget<Widget>(sprite.first);
		static const std::function<bool(const Widget* const)> isWidgetVisible =
			[](const Widget* const widget) -> bool {
			if (widget->isVisible()) {
				const auto parent = widget->getParent();
				if (parent) {
					return isWidgetVisible(parent);
				} else {
					return true;
				}
			}
			return false;
		};
		if (widget && isWidgetVisible(widget.get())) {
			// Pictures that don't match with their sprite's size will stretch the
			// sprite. This should be emulated here in the future using scaling.
			target.draw(sprite.second, states);
		}
	}
	// Draw angle brackets, if there is currently a widget selected via the
	// directional controls.
	if (_enableDirectionalFlow && _currentlySelectedWidget.find(getGUI()) !=
		_currentlySelectedWidget.end() &&
		!_currentlySelectedWidget.at(getGUI()).empty()) {
		target.draw(_angleBracketUL, states);
		target.draw(_angleBracketUR, states);
		target.draw(_angleBracketLL, states);
		target.draw(_angleBracketLR, states);
	}
	target.setView(oldView);
}

bool sfx::gui::_load(engine::json& j) {
	std::vector<std::string> names;
	j.applyVector(names, { "menus" });
	if (j.inGoodState()) {
		// Clear state.
		_gui.removeAllWidgets();
		_guiBackground.clear();
		_widgetSprites.clear();
		_guiSpriteKeys.clear();
		_dontOverridePictureSizeWithSpriteSize.clear();
		_originalStrings.clear();
		_originalStringsVariables.clear();
		_customSignalHandlers.clear();
		_upControl.clear();
		_downControl.clear();
		_leftControl.clear();
		_rightControl.clear();
		_selectControl.clear();
		_directionalFlow.clear();
		_selectThisWidgetFirst.clear();
		_currentlySelectedWidget.clear();
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
			// Temporarily set the current GUI to this one to make _findWidget()
			// work with relative widget names in SetUp() functions.
			setGUI(m, false, false);
			if (_scripts) _scripts->callFunction(m + "SetUp");
		}
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
	} else {
		return false;
	}
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
		// Okay... So... When I try to set ValueChanged on a SpinControl here, the
		// program crashes without reporting any errors whatsoever, not even in
		// debug mode... But the TGUI documentation says that it should have this
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
		if (_widgetSprites.find(name) != _widgetSprites.end())
			_widgetSprites.erase(name);
		if (_guiSpriteKeys.find(name) != _guiSpriteKeys.end())
			_guiSpriteKeys.erase(name);
		if (_dontOverridePictureSizeWithSpriteSize.find(name) !=
			_dontOverridePictureSizeWithSpriteSize.end())
			_dontOverridePictureSizeWithSpriteSize.erase(name);
		if (_originalStrings.find(name) != _originalStrings.end())
			_originalStrings.erase(name);
		if (_originalStringsVariables.find(name) !=
			_originalStringsVariables.end()) _originalStringsVariables.erase(name);
		if (_customSignalHandlers.find(name) != _customSignalHandlers.end())
			_customSignalHandlers.erase(name);
		if (_directionalFlow.find(name) != _directionalFlow.end())
			_directionalFlow.erase(name);
		if (removeIt) container->remove(widget);
	} else {
		_logger.error("Attempted to remove a widget \"{}\", which did not have a "
			"container!", name);
	}
}

void sfx::gui::_setTranslatedString(const std::string& text,
	const std::string& fullname, const std::size_t index,
	CScriptArray* variables) {
	// Resize both containers to fit.
	if (_originalStrings[fullname].size() <= index) {
		_originalStrings[fullname].resize(index + 1);
		_originalStringsVariables[fullname].resize(index + 1);
	}
	// Update _originalStrings.
	_originalStrings[fullname][index] = text;
	// Update _originalStringsVariables.
	_originalStringsVariables[fullname][index].clear();
	if (variables) {
		for (asUINT i = 0; i < variables->GetSize(); ++i) {
			_originalStringsVariables[fullname][index].emplace_back(
				(CScriptAny*)variables->At(i));
		}
	}
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
		return tgui::BitmapButton::create();
	} else if (type == "listbox") {
		return tgui::ListBox::create();
	} else if (type == "verticallayout") {
		return tgui::VerticalLayout::create();
	} else if (type == "horizontallayout") {
		return tgui::HorizontalLayout::create();
	} else if (type == "picture") {
		return tgui::Picture::create();
	} else if (type == "label") {
		return tgui::Label::create();
	} else if (type == "scrollablepanel") {
		return tgui::ScrollablePanel::create();
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
	} else {
		_logger.error("Attempted to create a widget of type \"{}\" with name "
			"\"{}\" for menu \"{}\": that widget type is not supported.", wType,
			name, menu);
		return nullptr;
	}
}

//////////////////////
// SCRIPT INTERFACE //
//////////////////////

void sfx::gui::_setGUI(const std::string& name) {
	setGUI(name, true, true);
}

void sfx::gui::_noBackground(std::string menu) {
	if (menu == "") menu = getGUI();
	_guiBackground.erase(menu);
}

void sfx::gui::_spriteBackground(std::string menu, const std::string& sheet,
	const std::string& sprite) {
	if (menu == "") menu = getGUI();
	try {
		_guiBackground[menu].set(_sheet.at(sheet), sprite);
	} catch (const std::out_of_range&) {
		_logger.error("Attempted to set sprite \"{}\" from sheet \"{}\" to the "
			"background of menu \"{}\". The sheet does not exist!", sprite, sheet,
			menu);
	}
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
	const auto menus = _gui.getWidgets();
	for (const auto& widget : menus)
		if (widget->getWidgetName() == menu) return true;
	return false;
}

void sfx::gui::_addWidget(const std::string& widgetType, const std::string& name,
	const std::string& signalHandler) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	if (_findWidget<Widget>(name, &fullname, &fullnameAsString)) {
		_logger.error("Attempted to create a new \"{}\" widget with name \"{}\": "
			"a widget with that name already exists!", widgetType, name);
	} else {
		tgui::Widget::Ptr widget = _createWidget(widgetType, name, fullname[0]);
		if (widget) {
			const auto containerName =
				fullnameAsString.substr(0, fullnameAsString.rfind('.'));
			auto container = _findWidget<Container>(containerName);
			if (!container) {
				_logger.error("Attempted to add a \"{}\" widget called \"{}\" to "
					"the container \"{}\". This container does not exist.",
					widgetType, name, containerName);
				return;
			}
			container->add(widget, fullnameAsString);
			_connectSignals(widget, signalHandler);
		}
	}
}

void sfx::gui::_addWidgetToGrid(const std::string& widgetType,
	const std::string& name, const std::size_t row, const std::size_t col,
	const std::string& signalHandler) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	if (_findWidget<Widget>(name, &fullname, &fullnameAsString)) {
		_logger.error("Attempted to create a new \"{}\" widget with name \"{}\": "
			"a widget with that name already exists!", widgetType, name);
	} else {
		tgui::Widget::Ptr widget = _createWidget(widgetType, name, fullname[0]);
		if (widget) {
			const auto gridName =
				fullnameAsString.substr(0, fullnameAsString.rfind('.'));
			auto grid = _findWidget<Grid>(gridName);
			if (!grid) {
				_logger.error("Attempted to add a \"{}\" widget called \"{}\" to "
					"the grid \"{}\". This grid does not exist.",
					widgetType, name, gridName);
			} else if (grid->getWidgetType() != "Grid") {
				_logger.error("Attempted to add a \"{}\" widget called \"{}\" to "
					"the grid \"{}\". This widget is not a grid.",
					widgetType, name, gridName);
			} else {
				widget->setWidgetName(fullnameAsString);
				_connectSignals(widget, signalHandler);
				grid->addWidget(widget, row, col);
			}
		}
	}
}

void sfx::gui::_removeWidget(const std::string& name) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		if (fullname.size() < 2) {
			_logger.error("Attempted to remove the \"{}\" menu using "
				"removeWidget(), which is not supported.", fullname[0]);
		} else {
			auto container = _findWidget<Container>(
				fullnameAsString.substr(0, fullnameAsString.rfind('.')));
			_removeWidgets(widget, container, true);
		}
	} else {
		_logger.error("Attempted to remove a widget \"{}\" within menu \"{}\". "
			"This widget does not exist.", name, fullname[0]);
	}
}

void sfx::gui::_removeWidgetsFromContainer(const std::string& name) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		if (fullname.size() < 2) {
			_removeWidgets(widget, nullptr, false);
		} else {
			const std::string type =
				widget->getWidgetType().toLower().toStdString();
			if (_isContainerWidget(type)) {
				auto container = _findWidget<Container>(
					fullnameAsString.substr(0, fullnameAsString.rfind('.')));
				_removeWidgets(widget, container, false);
			} else {
				_logger.error("Attempted to remove the widgets from a widget "
					"\"{}\" which is of type \"{}\", within menu \"{}\". This "
					"operation is not supported for this type of widget.", name,
					type, fullname[0]);
			}
		}
	} else {
		_logger.error("Attempted to remove the widgets from a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", name, fullname[0]);
	}

}

void sfx::gui::_setWidgetFocus(const std::string& name) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setFocused(true);
	} else {
		_logger.error("Attempted to set the focus to a widget \"{}\" within menu "
			"\"{}\". This widget does not exist.", name, fullname[0]);
	}
}

void sfx::gui::_setWidgetFont(const std::string& name,
	const std::string& fontName) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		if (_fonts) {
			auto fontPath = _fonts->getFontPath(fontName);
			// Invalid font name will be logged by fonts class.
			if (!fontPath.empty())
				widget->getRenderer()->setFont(tgui::Font(fontPath));
		} else {
			_logger.error("Attempted to set the font \"{}\" to a widget \"{}\" "
				"within menu \"{}\". No fonts object has been given to this gui "
				"object.", fontName, name, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the font \"{}\" to a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", fontName, name,
			fullname[0]);
	}
}

void sfx::gui::_setGlobalFont(const std::string& fontName) {
	if (_fonts) {
		auto fontPath = _fonts->getFontPath(fontName);
		// Invalid font name will be logged by fonts class.
		if (!fontPath.empty()) _gui.setFont(tgui::Font(fontPath));
	} else {
		_logger.error("Attempted to update the global font without this gui "
			"object having a fonts object!");
	}
}

void sfx::gui::_setWidgetPosition(const std::string& name, const std::string& x,
	const std::string& y) {
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

void sfx::gui::_setWidgetOrigin(const std::string& name, const float x,
	const float y) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setOrigin(x, y);
	} else {
		_logger.error("Attempted to set the origin (\"{}\",\"{}\") to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", x, y, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetSize(const std::string& name, const std::string& w,
	const std::string& h) {
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

sf::Vector2f sfx::gui::_getWidgetFullSize(const std::string& name) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		return widget->getFullSize();
	} else {
		_logger.error("Attempted to get the full size of a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
	return {};
}

void sfx::gui::_setWidgetEnabled(const std::string& name, const bool enable) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setEnabled(enable);
	} else {
		_logger.error("Attempted to update widget \"{}\"'s enabled state, within "
			"menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
}

bool sfx::gui::_getWidgetEnabled(const std::string& name) const {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		return widget->isEnabled();
	} else {
		_logger.error("Attempted to get the enabled property of a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
	return false;
}

void sfx::gui::_setWidgetVisibility(const std::string& name, const bool visible) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		widget->setVisible(visible);
	} else {
		_logger.error("Attempted to update widget \"{}\"'s visibility, within "
			"menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
}

bool sfx::gui::_getWidgetVisibility(const std::string& name) const {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		return widget->isVisible();
	} else {
		_logger.error("Attempted to get the visibility property of a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", name,
			fullname[0]);
	}
	return false;
}

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
	if (!upName.empty() &&
		!_findWidget<Widget>(upName, &fullnameUp, &fullnameAsStringUp)) {
		widgetDoesNotExist(upName);
		return;
	}
	if (!downName.empty() &&
		!_findWidget<Widget>(downName, &fullnameDown, &fullnameAsStringDown)) {
		widgetDoesNotExist(downName);
		return;
	}
	if (!leftName.empty() &&
		!_findWidget<Widget>(leftName, &fullnameLeft, &fullnameAsStringLeft)) {
		widgetDoesNotExist(leftName);
		return;
	}
	if (!rightName.empty() &&
		!_findWidget<Widget>(rightName, &fullnameRight, &fullnameAsStringRight)) {
		widgetDoesNotExist(rightName);
		return;
	}
	if ((fullnameUp.empty() || fullname[0] == fullnameUp[0]) &&
		(fullnameDown.empty() || fullname[0] == fullnameDown[0]) &&
		(fullnameLeft.empty() || fullname[0] == fullnameLeft[0]) &&
		(fullnameRight.empty() || fullname[0] == fullnameRight[0])) {
		_directionalFlow[fullnameAsString].up = fullnameAsStringUp;
		_directionalFlow[fullnameAsString].down = fullnameAsStringDown;
		_directionalFlow[fullnameAsString].left = fullnameAsStringLeft;
		_directionalFlow[fullnameAsString].right = fullnameAsStringRight;
	} else {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"within menu \"{}\", to the widgets up=\"{}\", down=\"{}\", "
			"left=\"{}\", right=\"{}\". Not all of these widgets are in the same "
			"menu!", name, fullname[0], fullnameAsStringUp, fullnameAsStringDown,
			fullnameAsStringLeft, fullnameAsStringRight);
	}
}

void sfx::gui::_setWidgetDirectionalFlowStart(const std::string& name) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		_selectThisWidgetFirst[fullname[0]] = fullnameAsString;
	} else {
		_logger.error("Attempted to set the widget \"{}\" as the first to be "
			"selected upon initial directional input, for the menu \"{}\". This "
			"widget does not exist.", name, fullname[0]);
	}
}

void sfx::gui::_clearWidgetDirectionalFlowStart(const std::string& menu) {
	if (_menuExists(menu)) {
		_selectThisWidgetFirst.erase(menu);
	} else {
		_logger.error("Attempted to disable directional input for the menu "
			"\"{}\". Menu does not exist.", menu);
	}
}

void sfx::gui::_setDirectionalFlowAngleBracketSprite(const std::string& corner,
	const std::string& sheet, const std::string& key) {
	const auto spritesheet = _sheet.find(sheet);
	if (spritesheet == _sheet.end()) {
		_logger.error("Attempted to set the sprite \"{}\" from spritesheet \"{}\" "
			"as the directional flow angle bracket for the \"{}\" corner. This "
			"spritesheet does not exist.", key, sheet, corner);
		return;
	}
	if (!spritesheet->second->doesSpriteExist(key)) {
		_logger.error("Attempted to set the sprite \"{}\" from spritesheet \"{}\" "
			"as the directional flow angle bracket for the \"{}\" corner. This "
			"sprite does not exist.", key, sheet, corner);
		return;
	}
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
		_logger.error("Attempted to set the sprite \"{}\" from spritesheet \"{}\" "
			"as the directional flow angle bracket for the \"{}\" corner. "
			"Unrecognised corner, must be \"UL\", \"UR\", \"LL\", or \"LR\".", key,
			sheet, corner);
	}
}

void sfx::gui::_setWidgetText(const std::string& name, const std::string& text,
	CScriptArray* variables) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		// For EditBoxes, don't translate the text, as this is text that the user
		// can edit.
		if (type == "editbox") {
			std::dynamic_pointer_cast<EditBox>(widget)->setText(text);
			if (variables) variables->Release();
			return;
		}
		if (type != "bitmapbutton" && type != "label" && type != "button") {
			_logger.error("Attempted to set the caption \"{}\" to widget \"{}\" "
				"which is of type \"{}\", within menu \"{}\". This operation is "
				"not supported for this type of widget.", text, name, type,
				fullname[0]);
			if (variables) variables->Release();
			return;
		}
		_setTranslatedString(text, fullnameAsString, 0, variables);
		// Set it by translating it.
		_translateWidget(widget);
	} else {
		_logger.error("Attempted to set the caption \"{}\" to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", text, name,
			fullname[0]);
	}
	if (variables) variables->Release();
}

std::string sfx::gui::_getWidgetText(const std::string& name) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		// Get the text differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "editbox") {
			return std::dynamic_pointer_cast<EditBox>(widget)->
				getText().toStdString();
		} else {
			_logger.error("Attempted to get the text of a widget \"{}\" which is "
				"of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to get the text of a widget \"{}\" within menu "
			"\"{}\". This widget does not exist.", name, fullname[0]);
	}
	return "";
}

void sfx::gui::_setWidgetTextSize(const std::string& name,
	const unsigned int size) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "label") {
			std::dynamic_pointer_cast<Label>(widget)->setTextSize(size);
		} else if (type == "bitmapbutton") {
			std::dynamic_pointer_cast<BitmapButton>(widget)->setTextSize(size);
		} else if (type == "button") {
			std::dynamic_pointer_cast<Button>(widget)->setTextSize(size);
		} else if (type == "editbox") {
			std::dynamic_pointer_cast<EditBox>(widget)->setTextSize(size);
		} else {
			_logger.error("Attempted to set the character size {} to widget "
				"\"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", size,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the character size {} to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", size, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetTextColour(const std::string& name,
	const sf::Color& colour) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "label") {
			std::dynamic_pointer_cast<Label>(widget)->
				getRenderer()->setTextColor(colour);
		} else if (type == "editbox") {
			std::dynamic_pointer_cast<EditBox>(widget)->
				getRenderer()->setTextColor(colour);
		} else {
			_logger.error("Attempted to set the text colour \"{}\" to widget "
				"\"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", colour,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the text colour \"{}\" to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", colour, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetTextOutlineColour(const std::string& name,
	const sf::Color& colour) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "label") {
			std::dynamic_pointer_cast<Label>(widget)->
				getRenderer()->setTextOutlineColor(colour);
		} else {
			_logger.error("Attempted to set the text outline colour \"{}\" to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", colour,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the text outline colour \"{}\" to a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.",
			colour, name, fullname[0]);
	}
}

void sfx::gui::_setWidgetTextOutlineThickness(const std::string& name,
	const float thickness) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "label") {
			auto label = std::dynamic_pointer_cast<Label>(widget);
			label->getRenderer()->setTextOutlineThickness(thickness);
		} else {
			_logger.error("Attempted to set the text outline thickness {} to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", thickness,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the text outline thickness {} to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", thickness,
			name, fullname[0]);
	}
}

void sfx::gui::_setWidgetTextAlignment(const std::string& name,
	const tgui::Label::HorizontalAlignment h,
	const tgui::Label::VerticalAlignment v) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "label") {
			auto label = std::dynamic_pointer_cast<Label>(widget);
			label->setHorizontalAlignment(h);
			label->setVerticalAlignment(v);
		} else {
			_logger.error("Attempted to set the text horizontal alignment {} and "
				"vertical alignment {} to widget \"{}\" which is of type \"{}\", "
				"within menu \"{}\". This operation is not supported for this "
				"type of widget.", h, v, name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the text horizontal alignment {} and "
			"vertical alignment {} to a widget \"{}\" within menu \"{}\". This "
			"widget does not exist.", h, v, name, fullname[0]);
	}
}

void sfx::gui::_setWidgetSprite(const std::string& name, const std::string& sheet,
	const std::string& key) {
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
		// Prevent deleting sprite objects if there won't be any change.
		if (_guiSpriteKeys[fullnameAsString].first != sheet ||
			_guiSpriteKeys[fullnameAsString].second != key) {
			_guiSpriteKeys[fullnameAsString] = std::make_pair(sheet, key);
			_widgetSprites.erase(fullnameAsString);
		}
	} else {
		_logger.error("Attempted to set the sprite \"{}\" from sheet \"{}\" to a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.", key,
			sheet, name, fullname[0]);
	}
}

void sfx::gui::_matchWidgetSizeToSprite(const std::string& name,
	const bool overrideSetSize) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type != "picture") {
			_logger.error("Attempted to match widget \"{}\"'s size to its set "
				"sprite. The widget is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", name, type,
				fullname[0]);
			return;
		}
		if (overrideSetSize)
			_dontOverridePictureSizeWithSpriteSize.erase(fullnameAsString);
		else
			_dontOverridePictureSizeWithSpriteSize.insert(fullnameAsString);
	} else {
		_logger.error("Attempted to match widget \"{}\"'s size to its set sprite. "
			"The widget is within menu \"{}\". This widget does not exist.", name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetBgColour(const std::string& name,
	const sf::Color& colour) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			std::dynamic_pointer_cast<Panel>(widget)->getRenderer()->
				setBackgroundColor(colour);
		} else if (type == "scrollablepanel") {
			std::dynamic_pointer_cast<ScrollablePanel>(widget)->getRenderer()->
				setBackgroundColor(colour);
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

void sfx::gui::_setWidgetBorderSize(const std::string& name, const float size) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			std::dynamic_pointer_cast<Panel>(widget)->getRenderer()->
				setBorders(size);
		} else {
			_logger.error("Attempted to set a border size of {} to widget \"{}\" "
				"which is of type \"{}\", within menu \"{}\". This operation is "
				"not supported for this type of widget.", size, name, type,
				fullname[0]);
		}
	} else {
		_logger.error("Attempted to set a border size of {} to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", size, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetBorderColour(const std::string& name,
	const sf::Color& colour) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			std::dynamic_pointer_cast<Panel>(widget)->getRenderer()->
				setBorderColor(colour);
		} else {
			_logger.error("Attempted to set a border colour of {} to widget "
				"\"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", colour,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set a border colour of {} to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", colour, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetBorderRadius(const std::string& name,
	const float radius) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "panel") {
			std::dynamic_pointer_cast<Panel>(widget)->getRenderer()->
				setRoundedBorderRadius(radius);
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

void sfx::gui::_setWidgetIndex(const std::string& name, const std::size_t index) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		if (fullname.size() >= 2) {
			std::string containerName =
				fullnameAsString.substr(0, fullnameAsString.rfind('.'));
			Container::Ptr container = _findWidget<Container>(containerName);
			if (container) {
				if (!container->setWidgetIndex(widget, index)) {
					// The size() should never be 0 here...
					_logger.error("Could not set index {} to widget \"{}\" within "
						"menu \"{}\". The index cannot be higher than {}.", index,
						name, fullname[0], container->getWidgets().size() - 1);
				}
			} else {
				_logger.error("Could not find container \"{}\" whilst setting "
					"widget \"{}\"'s index to {}, within menu \"{}\".",
					containerName, name, index, fullname[0]);
			}
		} else {
			_logger.error("Attempted to set a menu \"{}\"'s widget index to {}. "
				"This is unsupported for menu groups.", name, index);
		}
	} else {
		_logger.error("Attempted to set the index {} to a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", index, name, fullname[0]);
	}
}

void sfx::gui::_setWidgetIndexInContainer(const std::string& name,
	const std::size_t oldIndex, const std::size_t newIndex) {
	std::vector<std::string> fullname;
	Container::Ptr container = _findWidget<Container>(name, &fullname);
	if (container) {
		if (_isContainerWidget(container->getWidgetType())) {
			Widget::Ptr widget;
			try {
				widget = container->getWidgets().at(oldIndex);
			} catch (const std::out_of_range&) {
				_logger.error("Attempted to set container \"{}\"'s number {} "
					"widget to an index of {}, within menu \"{}\". This container "
					"does not have a widget with index {}.", name, oldIndex,
					newIndex, fullname[0], oldIndex);
				return;
			}
			if (!container->setWidgetIndex(widget, newIndex)) {
				const auto count = container->getWidgets().size();
				if (count) {
					_logger.error("Attempted to set container \"{}\"'s number {} "
						"widget to an index of {}, within menu \"{}\". The new "
						"index cannot be higher than {}.", name, oldIndex,
						newIndex, fullname[0], count - 1);
				} else {
					_logger.error("Attempted to set container \"{}\"'s number {} "
						"widget to an index of {}, within menu \"{}\". There are "
						"no widgets in this container.", name, oldIndex, newIndex,
						fullname[0]);
				}
			}
		} else {
			_logger.error("Attempted to set widget \"{}\"'s number {} widget to "
				"an index of {}, within menu \"{}\". The first widget is of type "
				"\"{}\". This operation is not supported for this widget type.",
				name, oldIndex, newIndex, fullname[0],
				container->getWidgetType().toStdString());
		}
	} else {
		_logger.error("Attempted to set widget \"{}\"'s number {} widget to an "
			"index of {}, within menu \"{}\". The first widget does not exist.",
			name, oldIndex, newIndex, fullname[0]);
	}
}

void sfx::gui::_setWidgetRatioInLayout(const std::string& name,
	const std::size_t index, const float ratio) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "verticallayout" || type == "horizontallayout") {
			auto boxRatios = std::dynamic_pointer_cast<BoxLayoutRatios>(widget);
			if (!boxRatios->setRatio(index, ratio)) {
				_logger.error("Attempted to set the widget ratio {} to widget {} "
					"in layout \"{}\", within menu \"{}\". The widget index was "
					"too high.", ratio, index, name, fullname[0]);
			}
		} else {
			_logger.error("Attempted to set the widget ratio {} to widget {} in "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", ratio,
				index, name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the widget ratio {} to widget {} in "
			"widget \"{}\" within menu \"{}\". This widget does not exist.", ratio,
			index, name, fullname[0]);
	}
}

void sfx::gui::_setWidgetDefaultText(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type != "editbox") {
			_logger.error("Attempted to set the default text \"{}\" to widget "
				"\"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", text, name,
				type, fullname[0]);
			if (variables) variables->Release();
			return;
		}
		_setTranslatedString(text, fullnameAsString, 0, variables);
		// Set it by translating it.
		_translateWidget(widget);
	} else {
		_logger.error("Attempted to set the default text \"{}\" to a widget "
			"\"{}\" within menu \"{}\". This widget does not exist.", text, name,
			fullname[0]);
	}
	if (variables) variables->Release();
}

void sfx::gui::_addItem(const std::string& name, const std::string& text,
	CScriptArray* variables) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		// Add the item differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			std::dynamic_pointer_cast<ListBox>(widget)->addItem(text);
		} else {
			_logger.error("Attempted to add an item \"{}\" to widget \"{}\" which "
				"is of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", text, name, type,
				fullname[0]);
			if (variables) variables->Release();
			return;
		}
		_setTranslatedString(text, fullnameAsString,
			_originalStrings[fullnameAsString].size(), variables);
		// Translate the new item.
		// We still have to add the new item itself so keep the code above!
		_translateWidget(widget);
	} else {
		_logger.error("Attempted to add a new item \"{}\" to a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", text, name,
			fullname[0]);
	}
	if (variables) variables->Release();
}

void sfx::gui::_clearItems(const std::string& name) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		// Remove all the items differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			std::dynamic_pointer_cast<ListBox>(widget)->removeAllItems();
		} else {
			_logger.error("Attempted to clear all items from widget \"{}\" which "
				"is of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", name, type, fullname[0]);
			return;
		}
		// Clear this widget's entry in the _originalStrings container.
		_originalStrings[fullnameAsString].clear();
		_originalStringsVariables[fullnameAsString].clear();
	} else {
		_logger.error("Attempted to clear all items from a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
}

void sfx::gui::_setSelectedItem(const std::string& name, const std::size_t index) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		// Select the item differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			auto listbox = std::dynamic_pointer_cast<ListBox>(widget);
			if (!listbox->setSelectedItemByIndex(index)) {
				const auto count = listbox->getItemCount();
				if (count) {
					_logger.error("Attempted to select item {} from listbox "
						"\"{}\", within menu \"{}\". The item index cannot be "
						"higher than {}.", index, name, fullname[0], count - 1);
				} else {
					_logger.error("Attempted to select item {} from listbox "
						"\"{}\", within menu \"{}\". There are no items in this "
						"listbox.", index, name, fullname[0]);
				}
			}
		} else {
			_logger.error("Attempted to select item {} from widget \"{}\" which "
				"is of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", index, name, type,
				fullname[0]);
		}
	} else {
		_logger.error("Attempted to select item {} from a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", index, name, fullname[0]);
	}
}

int sfx::gui::_getSelectedItem(const std::string& name) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		// Get the item index differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			return std::dynamic_pointer_cast<ListBox>(widget)->
				getSelectedItemIndex();
		} else {
			_logger.error("Attempted to get the index of the selected item of a "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", name, type,
				fullname[0]);
		}
	} else {
		_logger.error("Attempted to get the index of the selected item of a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.", name,
			fullname[0]);
	}
	return -1;
}

std::string sfx::gui::_getSelectedItemText(const std::string& name) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		// Get the item text differently depending on the type the widget is.
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "listbox") {
			return std::dynamic_pointer_cast<ListBox>(widget)->
				getSelectedItem().toStdString();
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

std::size_t sfx::gui::_getWidgetCount(const std::string& name) {
	std::vector<std::string> fullname;
	std::string fullnameAsString;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname, &fullnameAsString);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (_isContainerWidget(type)) {
			return
				std::dynamic_pointer_cast<Container>(widget)->getWidgets().size();
		} else {
			_logger.error("Attempted to get the widget count of a widget \"{}\" "
				"which is of type \"{}\", within menu \"{}\". This operation is "
				"not supported for this type of widget.", name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to get the widget count of a widget \"{}\" "
			"within menu \"{}\". This widget does not exist.", name, fullname[0]);
	}
	return 0;
}

void sfx::gui::_setHorizontalScrollbarPolicy(const std::string& name,
	const tgui::Scrollbar::Policy policy) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "scrollablepanel") {
			std::dynamic_pointer_cast<ScrollablePanel>(widget)->
				setHorizontalScrollbarPolicy(policy);
		} else {
			_logger.error("Attempted to set the horizontal scrollbar policy {} to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", policy,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the horizontal scrollbar policy {} to a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.",
			policy, name, fullname[0]);
	}
}

void sfx::gui::_setHorizontalScrollbarAmount(const std::string& name,
	const unsigned int amount) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "scrollablepanel") {
			std::dynamic_pointer_cast<ScrollablePanel>(widget)->
				setHorizontalScrollAmount(amount);
		} else {
			_logger.error("Attempted to set the horizontal scrollbar amount {} to "
				"widget \"{}\" which is of type \"{}\", within menu \"{}\". "
				"This operation is not supported for this type of widget.", amount,
				name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set the horizontal scrollbar amount {} to a "
			"widget \"{}\" within menu \"{}\". This widget does not exist.",
			amount, name, fullname[0]);
	}
}

void sfx::gui::_setGroupPadding(const std::string& name,
	const std::string& padding) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "scrollablepanel") {
			std::dynamic_pointer_cast<ScrollablePanel>(widget)->
				getRenderer()->setPadding(AbsoluteOrRelativeValue(padding));
		} else if (type == "panel") {
			std::dynamic_pointer_cast<Panel>(widget)->
				getRenderer()->setPadding(AbsoluteOrRelativeValue(padding));
		} else if (type == "verticallayout") {
			std::dynamic_pointer_cast<VerticalLayout>(widget)->
				getRenderer()->setPadding(AbsoluteOrRelativeValue(padding));
		} else if (type == "horizontallayout") {
			std::dynamic_pointer_cast<HorizontalLayout>(widget)->
				getRenderer()->setPadding(AbsoluteOrRelativeValue(padding));
		} else {
			_logger.error("Attempted to set a padding {} to widget \"{}\" which "
				"is of type \"{}\", within menu \"{}\". This operation is not "
				"supported for this type of widget.", padding, name, type,
				fullname[0]);
		}
	} else {
		_logger.error("Attempted to set a padding {} to a widget \"{}\" within "
			"menu \"{}\". This widget does not exist.", padding, name,
			fullname[0]);
	}
}

void sfx::gui::_setWidgetAlignmentInGrid(const std::string& name,
	const std::size_t row, const std::size_t col,
	const tgui::Grid::Alignment alignment) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "grid") {
			auto grid = std::dynamic_pointer_cast<Grid>(widget);
			auto& table = grid->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size()) {
					grid->setWidgetAlignment(row, col, alignment);
				} else {
					_logger.error("Attempted to set an alignment {} to a grid "
						"\"{}\" @ ({}, {}), within menu \"{}\". The column index "
						"is out of range.", alignment, name, row, col,
						fullname[0]);
				}
			} else {
				_logger.error("Attempted to set an alignment {} to a grid \"{}\" "
					"@ ({}, {}), within menu \"{}\". The row index is out of "
					"range.", alignment, name, row, col, fullname[0]);
			}
		} else {
			_logger.error("Attempted to set an alignment {} to a widget \"{}\" @ "
				"({}, {}) which is of type \"{}\", within menu \"{}\". This "
				"operation is not supported for this type of widget.", alignment,
				name, row, col, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set an alignment {} to a widget \"{}\" @ ({}, "
			"{}) within menu \"{}\". This widget does not exist.", alignment, name,
			row, col, fullname[0]);
	}
}

void sfx::gui::_setSpaceBetweenWidgets(const std::string& name,
	const float space) {
	std::vector<std::string> fullname;
	Widget::Ptr widget = _findWidget<Widget>(name, &fullname);
	if (widget) {
		const std::string type = widget->getWidgetType().toLower().toStdString();
		if (type == "verticallayout" || type == "horizontallayout") {
			auto layout = std::dynamic_pointer_cast<BoxLayout>(widget);
			layout->getRenderer()->setSpaceBetweenWidgets(space);
		} else {
			_logger.error("Attempted to set {} to a widget \"{}\"'s space between "
				"widgets property. The widget is of type \"{}\", within menu "
				"\"{}\". This operation is not supported for this type of widget.",
				space, name, type, fullname[0]);
		}
	} else {
		_logger.error("Attempted to set {} to a widget \"{}\"'s space between "
			"widgets property, within menu \"{}\". This widget does not exist.",
			space, name, fullname[0]);
	}
}
