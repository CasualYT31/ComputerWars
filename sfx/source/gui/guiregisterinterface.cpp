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
#include "guiconstants.hpp"
#include "TGUI/Widgets/Grid.hpp"
#include "TGUI/Widgets/Scrollbar.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#undef MessageBox

using namespace tgui;

/**
 * Formats a \c MenuItemID into a string.
 * @param  id The \c MenuItemID to format.
 * @return The \c MenuItemID in string form.
 */
static std::string formatMenuItemID(const sfx::gui::MenuItemID id) {
	return std::to_string(id);
}

void sfx::gui::_documentGUILibrary(
	const std::shared_ptr<DocumentationGenerator>& document) {
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
		"you to set a textbox's typable contents.");
	document->DocumentExpectedFunction("GUISizeAndPosition",
		"The GUI backend used by the game engine allows you to specify "
		"expressions for the size and position of widgets. For an introduction "
		"into this topic, see https://tgui.eu/tutorials/1.0/layouts/. To specify "
		"pixel values, you can issue \"5px\", or \"5\", for example.");
	document->DocumentExpectedFunction("WidgetTypes",
		"Widget types are essentially the GUI backend's class names but in string "
		"form. https://tgui.eu/documentation/1.0/annotated.html is the backend's "
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
		"void MainMenuHandleInput(const dictionary[, const dictionary, "
		"const MousePosition&in, const MousePosition&in])",
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
		"The second dictionary, and <tt>MousePosition</tt> parameters, are "
		"optional and do not have to be defined. If they are, the first "
		"<tt>MousePosition</tt> holds the position of the mouse during the "
		"previous iteration of the game loop, and the second "
		"<tt>MousePosition</tt> holds the current position of the mouse.\n\n"
		"The second dictionary also maps to bools, and it has the same keys as "
		"the first dictionary. If <tt>TRUE</tt> is stored at the key, it means "
		"that control is being triggered by one of its configured mouse "
		"buttons. If <tt>FALSE</tt> is stored, it means that either the control "
		"isn't being triggered, or if it is, not by a mouse button.\n\n"
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
}

void sfx::gui::_registerTypes(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
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

	engine->RegisterTypedef("MenuItemID", "uint64");
	document->DocumentExpectedFunction("typedef uint64 MenuItemID",
		"Index used to identify a menu item in a <tt>MenuBar</tt> widget.");

	r = engine->RegisterGlobalFunction("string formatMenuItemID(const MenuItemID)",
		asFUNCTION(formatMenuItemID), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Formats a MenuItemID value as a string.");

	r = engine->RegisterFuncdef(
		"void SignalHandler(const string&in, const string&in)");
	document->DocumentObjectFuncDef(r, "The signature of a callback that is "
		"invoked when a widget emits a signal. The first string is the full name "
		"of the widget emitting the signal, and the second string is the name of "
		"the signal that was emitted.");
}

void sfx::gui::_registerConstants(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalProperty("const float NO_SPACE", &NO_SPACE);
	document->DocumentExpectedFunction("const float NO_SPACE", "Constant which "
		"represents \"no space between widgets in a vertical or horizontal "
		"layout.\" Due to rounding errors, however, this likely won't be "
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

	r = engine->RegisterGlobalProperty("const string GOTO_PREVIOUS_WIDGET",
		&GOTO_PREVIOUS_WIDGET);
	document->DocumentExpectedFunction("const string GOTO_PREVIOUS_WIDGET",
		"Used with directional flow functions to represent \"going to the widget "
		"that was previously selected using directional flow.\"");

	REGISTER_WIDGET_TYPE_NAME(engine, document, BitmapButton);
	REGISTER_WIDGET_TYPE_NAME(engine, document, ListBox);
	REGISTER_WIDGET_TYPE_NAME(engine, document, VerticalLayout);
	REGISTER_WIDGET_TYPE_NAME(engine, document, HorizontalLayout);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Picture);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Label);
	REGISTER_WIDGET_TYPE_NAME(engine, document, ScrollablePanel);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Panel);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Group);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Grid);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Button);
	REGISTER_WIDGET_TYPE_NAME(engine, document, EditBox);
	REGISTER_WIDGET_TYPE_NAME(engine, document, MenuBar);
	REGISTER_WIDGET_TYPE_NAME(engine, document, ChildWindow);
	REGISTER_WIDGET_TYPE_NAME(engine, document, ComboBox);
	REGISTER_WIDGET_TYPE_NAME(engine, document, FileDialog);
	REGISTER_WIDGET_TYPE_NAME(engine, document, MessageBox);
	REGISTER_WIDGET_TYPE_NAME(engine, document, HorizontalWrap);
	REGISTER_WIDGET_TYPE_NAME(engine, document, Tabs);
	REGISTER_WIDGET_TYPE_NAME(engine, document, TreeView);
	REGISTER_WIDGET_TYPE_NAME(engine, document, CheckBox);
	REGISTER_WIDGET_TYPE_NAME(engine, document, RadioButton);
	REGISTER_WIDGET_TYPE_NAME(engine, document, TabContainer);
	REGISTER_WIDGET_TYPE_NAME(engine, document, TextArea);
}

void sfx::gui::_registerNonWidgetGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void setGUI(const string&in)",
		asMETHOD(sfx::gui, _setGUI), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Hides the current menu and shows the "
		"menu given.");

	r = engine->RegisterGlobalFunction("bool menuExists(const string&in)",
		asMETHOD(sfx::gui, _menuExists), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if the given "
		"menu exists, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction("void setBackground(string)",
		asMETHOD(sfx::gui, _noBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes the background from the given "
		"menu. If a blank string is given, removes the background from the menu "
		"that is currently open.");

	r = engine->RegisterGlobalFunction("void setBackground(string, const "
		"string&in, const string&in)",
		asMETHOD(sfx::gui, _spriteBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given menu's background to be "
		"an animated sprite from the given sprite sheet.\n<ol><li>The name of the "
		"menu to set the background of. If blank, the current menu is chosen."
		"</li><li>The name of the spritesheet which contains the sprite to apply."
		"</li><li>The name of the sprite to apply.</li></ol>");

	r = engine->RegisterGlobalFunction(
		"void setBackground(string, const Colour&in)",
		asMETHOD(sfx::gui, _colourBackground), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given menu's background to a "
		"solid colour. If a blank string is given, the current menu will be "
		"changed.");

	r = engine->RegisterGlobalFunction("void setGlobalFont(const string&in)",
		asMETHOD(sfx::gui, _setGlobalFont), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the global font. The name of the "
		"font is given, as defined in the fonts.json script.");
}

void sfx::gui::_registerWidgetGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("string getWidgetUnderMouse()",
		asMETHOD(sfx::gui, getWidgetUnderMouse), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the full name of the widget that "
		"is under the current mouse position. An empty string if there isn't "
		"one.");

	r = engine->RegisterGlobalFunction("bool widgetExists(const string&in)",
		asMETHOD(sfx::gui, _widgetExists), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if the named "
		"widget exists, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction(
		"string getWidgetFocused(const string&in = \"\")",
		asMETHOD(sfx::gui, _getWidgetFocused), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the full name of the widget that "
		"currently has setfocus within the given container widget. If there is no "
		"widget with setfocus in the given container, or an error occurred, an "
		"empty string will be returned. If an empty string is given, the root GUI "
		"will be queried. This function does not search recursively.");

	r = engine->RegisterGlobalFunction("void addWidget(const string&in, const "
		"string&in, const string&in = \"\")",
		asMETHOD(sfx::gui, _addWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget and adds it to a "
		"menu. The type of widget is given, then the name of the new widget. If "
		"the name of the new widget is a full name, it will be added to the "
		"specified menu. If it is not a full name, it will be added to the "
		"current menu.\n"
		"The final parameter is the name of the script function to call when this "
		"widget emits a signal. If a blank string is given, then the default "
		"handlers will be assumed. The custom signal handler must have two "
		"<tt>const string&in</tt> parameters. The first is the full name of the "
		"widget that triggered the handler. The second is the name of the signal "
		"that was emitted. <b>Note that the following signals will never invoke "
		"the custom signal handler!</b>\n"
		"<ul><li><tt>MenuItemClicked</tt>.</li>"
		"<li><tt>Closing</tt>.</li></ul>\n"
		"An error will be logged if the widget type was invalid or if a widget "
		"with the given name already exists.");

	r = engine->RegisterGlobalFunction("void connectSignalHandler(const string&in,"
		"SignalHandler@ const)",
		asMETHOD(sfx::gui, _connectSignalHandler), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "This method is used to provide an "
		"additional signal handler that is called before any others. One can pass "
		"in <tt>null</tt> to remove it.");

	r = engine->RegisterGlobalFunction(
		"void disconnectSignalHandlers(const array<string>@ const)",
		asMETHOD(sfx::gui, _disconnectSignalHandlers),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "This method disconnects all additional "
		"signal handlers that have been assigned to each of the widgets given in "
		"the array. If null is given, a warning will be logged and nothing will "
		"be changed.");

	r = engine->RegisterGlobalFunction("string getParent(const string&in)",
		asMETHOD(sfx::gui, _getParent), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "This method returns the full name of the "
		"parent of the given widget.");

	r = engine->RegisterGlobalFunction("void removeWidget(const string&in)",
		asMETHOD(sfx::gui, _removeWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes a given widget. If the given "
		"widget is a container, then all of its widgets will be removed "
		"recursively.");

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
		"widget is given, then the width, then the height. If either the width or "
		"height is a blank string, then the engine will retain the layout that is "
		"currently set to that width or height.");

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
	document->DocumentGlobalFunction(r, "Gets a widget's enabled state. Returns "
		"<tt>FALSE</tt> on error.");

	r = engine->RegisterGlobalFunction("void setWidgetVisibility(const string&in, "
		"const bool)",
		asMETHOD(sfx::gui, _setWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's visibility. The name of "
		"the widget is given, then if it should be visible or not.");

	r = engine->RegisterGlobalFunction("bool getWidgetVisibility(const string&in)",
		asMETHOD(sfx::gui, _getWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's visibility. Returns "
		"<tt>FALSE</tt> on error.");

	r = engine->RegisterGlobalFunction("void moveWidgetToFront(const string&in)",
		asMETHOD(sfx::gui, _moveWidgetToFront), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Moves a given widget to the front of its "
		"parent container.");

	r = engine->RegisterGlobalFunction("void moveWidgetToBack(const string&in)",
		asMETHOD(sfx::gui, _moveWidgetToBack), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Moves a given widget to the back of its "
		"parent container.");

	r = engine->RegisterGlobalFunction("void setWidgetText(const string&in, "
		"const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _setWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets a widget's text. The "
		"name of the widget is given, then its new text. An optional list of "
		"variables can also be given. These variables will be inserted into the "
		"text wherever a '" + std::to_string(engine::expand_string::getVarChar()) +
		"' is found.\n"
		"This function has special behaviour for <tt>EditBox</tt> and "
		"<tt>TextArea</tt>widgets. No variables are inserted into the given "
		"string, and it is not translated. The <tt>EditBox</tt>'s or "
		"<tt>TextArea</tt>'s current text is simply replaced with whatever string "
		"is given.").c_str());

	r = engine->RegisterGlobalFunction(
		"void setWidgetIndex(const string&in, const uint)",
		asMETHOD(sfx::gui, _setWidgetIndex),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a "
		"container.");
}

void sfx::gui::_registerDirectionalFlowGlobalFunctions(
	asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlow("
		"const string&in, const string&in, const string&in, const string&in, "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlow),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the widgets that should be selected "
		"if directional controls are input when the given widget is currently "
		"selected. The \"given widget\" should be given first, followed by the "
		"widgets that should be selected, when up, down, left, and right are "
		"input, respectively. All given widgets must be in the same menu! A blank "
		"string means that the input won't change the selected widget. A value of "
		"<tt>GOTO_PREVIOUS_WIDGET</tt> means \"navigate back to the previously "
		"selected widget.\"");

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
		"controls from selecting a widget for the given menu. This is the default "
		"behaviour for all menus.");

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
		"name of the sprite. The first string is trimmed and case-insensitive.");
}

void sfx::gui::_registerSpriteGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetSprite(const string&in, const string&in, const string&in)",
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
		"size of its sprite, even if a size is manually given. Only supported for "
		"<tt>Picture</tt> widgets. <b>TRUE is the default behaviour for all "
		"<tt>Picture</tt>s!</b>");
}

void sfx::gui::_registerLabelGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetTextSize(const string&in, const uint)",
		asMETHOD(sfx::gui, _setWidgetTextSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's character size. The name "
		"of the widget is given, then its new character size.");

	r = engine->RegisterGlobalFunction("void setWidgetTextStyles(const string&in, "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetTextStyles), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text styles. The name of "
		"the widget is given, then its new styles. It can be a combination of "
		"<a href=\"https://tgui.eu/documentation/1.0/namespacetgui.html#aafa478ba31ef52a263be37506428943b\" target=\"_blank\">"
		"these enums in string form</a>, joined together using \" | \".");

	r = engine->RegisterGlobalFunction("void setWidgetTextMaximumWidth("
		"const string&in, const float)",
		asMETHOD(sfx::gui, _setWidgetTextMaximumWidth),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's maximum text width. The "
		"name of the widget is given, then its new maximum text width.");

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
}

void sfx::gui::_registerEditBoxGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"string getWidgetText(const string&in)",
		asMETHOD(sfx::gui, _getWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets an <tt>EditBox</tt>'s or "
		"<tt>TextArea</tt>'s text.");

	r = engine->RegisterGlobalFunction(
		"void onlyAcceptUIntsInEditBox(const string&in)",
		asMETHOD(sfx::gui, _onlyAcceptUIntsInEditBox),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Configures an <tt>EditBox</tt> to only "
		"accept unsigned integers.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetDefaultText(const string&in, const string&in, "
		"array<any>@ = null)",
		asMETHOD(sfx::gui, _setWidgetDefaultText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets an <tt>EditBox</tt>'s "
		"or <tt>TextArea</tt>'s default text, which is shown when there isn't any "
		"text in the widget. The name of the widget is given, then its new "
		"default text. An optional list of variables can also be given. These "
		"variables will be inserted into the text wherever a '" +
		std::to_string(engine::expand_string::getVarChar()) + "' is "
		"found.").c_str());

	r = engine->RegisterGlobalFunction(
		"bool editBoxOrTextAreaHasFocus()",
		asMETHOD(sfx::gui, _editBoxOrTextAreaHasFocus),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if an "
		"<tt>EditBox</tt> or a <tt>TextArea</tt> currently has set focus, "
		"<tt>FALSE</tt> otherwise.");
}

void sfx::gui::_registerRadioButtonAndCheckBoxGlobalFunctions(
	asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetChecked(const string&in, const bool)",
		asMETHOD(sfx::gui, _setWidgetChecked), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's checked state. The name "
		"of the widget is given, then if it should be checked or not.");

	r = engine->RegisterGlobalFunction("bool isWidgetChecked(const string&in)",
		asMETHOD(sfx::gui, _isWidgetChecked), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's checked state. The name "
		"of the widget is given. Returns <tt>FALSE</tt> if the checked status "
		"could not be retrieved.");
}

void sfx::gui::_registerListGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void addItem(const string&in, const "
		"string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new item to a widget. The name "
		"of the widget is given, then the text of the new item. An optional list "
		"variables can also be given: see setWidgetText() for more information.");

	r = engine->RegisterGlobalFunction("void clearItems(const string&in)",
		asMETHOD(sfx::gui, _clearItems), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all items from a given widget. "
		"The name of the widget should be given.");

	r = engine->RegisterGlobalFunction("void setSelectedItem(const string&in, "
		"const uint64)", asMETHODPR(sfx::gui, _setSelectedItem,
			(const std::string&, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects an item from a widget. The name "
		"of the widget is given, then the 0-based index of the item to select.");

	r = engine->RegisterGlobalFunction("void deselectItem(const string&in)",
		asMETHOD(sfx::gui, _deselectItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Deselects an item from a widget.");

	r = engine->RegisterGlobalFunction("int getSelectedItem(const string&in)",
		asMETHOD(sfx::gui, _getSelectedItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's index. "
		"If no item is selected, or the index of the selected item couldn't be "
		"retrieved, <tt>-1</tt> is returned.");

	r = engine->RegisterGlobalFunction("string getSelectedItemText("
		"const string&in)",
		asMETHOD(sfx::gui, _getSelectedItemText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's text.");

	r = engine->RegisterGlobalFunction(
		"void setItemsToDisplay(const string&in, const uint64)",
		asMETHOD(sfx::gui, _setItemsToDisplay), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the number of items to display in a "
		"given <tt>ComboBox</tt> when open, at one time. 0 means always show all "
		"items.");
}

void sfx::gui::_registerTreeViewGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"array<string>@ getSelectedItemTextHierarchy(const string&in)",
		asMETHOD(sfx::gui, _getSelectedItemTextHierarchy),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a <tt>TreeView</tt>'s selected "
		"item's text, including the text of its parents. If it couldn't be "
		"retrieved, or if there isn't a selected item, an empty array will be "
		"returned.");

	r = engine->RegisterGlobalFunction(
		"void addTreeViewItem(const string&in, const array<string>@ const)",
		asMETHOD(sfx::gui, _addTreeViewItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds an item to a <tt>TreeView</tt> "
		"widget. The array describes the hierarchy of the new item. If parent "
		"items do not exist, then they will be created. Note that "
		"<tt>TreeView</tt> items are not translated!");
}

void sfx::gui::_registerTabsGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void addTab(const string&in, const "
		"string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addTab), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new tab to a <tt>Tabs</tt> "
		"widget. The name of the widget is given, then the text of the new tab. "
		"An optional list variables can also be given: see setWidgetText() for "
		"more information. The new tab will not be selected automatically.");

	r = engine->RegisterGlobalFunction("void setSelectedTab(const string&in, "
		"const uint64)", asMETHODPR(sfx::gui, _setSelectedTab,
			(const std::string&, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects a tab from a widget. The name of "
		"the widget is given, then the 0-based index of the tab to select. If the "
		"tab is invisible or disabled, the selection operation will fail. If the "
		"operation fails for any reason, an attempt will be made to select the "
		"previously selected tab, if there was one.");

	r = engine->RegisterGlobalFunction("int getSelectedTab(const string&in)",
		asMETHOD(sfx::gui, _getSelectedTab), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected tab's index. If "
		"and error ocurred, or there wasn't a selected tab, <tt>-1</tt> will be "
		"returned.");

	r = engine->RegisterGlobalFunction("uint64 getTabCount(const string&in)",
		asMETHOD(sfx::gui, _getTabCount), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's tab count.");
}

void sfx::gui::_registerContainerGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void removeWidgetsFromContainer("
		"const string&in)", asMETHOD(sfx::gui, _removeWidgetsFromContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all the widgets from a given "
		"container/menu, but does not remove the container/menu itself.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetIndexInContainer(const string&in, const uint, const uint)",
		asMETHOD(sfx::gui, _setWidgetIndexInContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a given "
		"container. The name of the container is given, then the index of the "
		"widget to edit, with its new index given as the last parameter.");

	r = engine->RegisterGlobalFunction("uint getWidgetCount(const string&in)",
		asMETHOD(sfx::gui, _getWidgetCount), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets the number of widgets that are in "
		"the specified container. Does not count recursively.");

	r = engine->RegisterGlobalFunction("void setGroupPadding("
		"const string&in, const string&in)", asMETHODPR(sfx::gui, _setGroupPadding,
			(const std::string&, const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a group's padding. If a "
		"<tt>Grid</tt> is given, each of its widgets will be assigned the given "
		"padding, but note that it will only work once all of its widgets have "
		"been added and newly added widgets won't have the padding applied "
		"automatically!");

	r = engine->RegisterGlobalFunction("void setGroupPadding(const string&in, "
		"const string&in, const string&in, const string&in, const string&in)",
		asMETHODPR(sfx::gui, _setGroupPadding, (const std::string&,
			const std::string&, const std::string&, const std::string&,
			const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a group's padding. The group's name "
		"is given, then the padding applied to the left, top, right, then bottom "
		"sides. If a <tt>Grid</tt> is given, each of its widgets will be assigned "
		"the given padding, but note that it will only work once all of its "
		"widgets have been added and newly added widgets won't have the padding "
		"applied automatically!");

	r = engine->RegisterGlobalFunction("void applySpritesToWidgetsInContainer("
		"const string&in, const string&in, const array<string>@ const)",
		asMETHOD(sfx::gui, _applySpritesToWidgetsInContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Loops through a given container (whose "
		"name is given as the first parameter), and applies sprites from a given "
		"spritesheet (second parameter) to each <tt>BitmapButton</tt> and "
		"<tt>Picture</tt> that is found. The first applicable widget will receive "
		"the first sprite in the array, the second applicable widget the second "
		"sprite, and so on.");
}

void sfx::gui::_registerPanelGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
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

	r = engine->RegisterGlobalFunction("void setHorizontalScrollbarPolicy("
		"const string&in, const ScrollbarPolicy)",
		asMETHOD(sfx::gui, _setHorizontalScrollbarPolicy),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's or TextArea's "
		"horizontal scrollbar policy.");

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

	r = engine->RegisterGlobalFunction("void setVerticalScrollbarValue("
		"const string&in, const uint)",
		asMETHOD(sfx::gui, _setVerticalScrollbarValue),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's vertical scroll "
		"value.");

	r = engine->RegisterGlobalFunction("float getScrollbarWidth(const string&in)",
		asMETHOD(sfx::gui, _getScrollbarWidth),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a ScrollablePanel's scrollbar "
		"width.");
}

void sfx::gui::_registerLayoutGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetRatioInLayout(const string&in, const uint, const float)",
		asMETHOD(sfx::gui, _setWidgetRatioInLayout),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Within a <tt>VerticalLayout</tt> or "
		"<tt>HorizontalLayout</tt>; sets the ratio of a specified widget's size "
		"compared to others in the layout. The unsigned integer is a 0-based "
		"index of the widget in the layout to amend.");

	r = engine->RegisterGlobalFunction("void setSpaceBetweenWidgets("
		"const string&in, const float)",
		asMETHOD(sfx::gui, _setSpaceBetweenWidgets),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the space between widgets in a "
		"vertical or horizontal layout.");
}

void sfx::gui::_registerGridGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void addWidgetToGrid(const string&in,"
		"const string&in, const uint, const uint, const string&in = \"\")",
		asMETHOD(sfx::gui, _addWidgetToGrid), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget and adds it to a "
		"grid. The type of widget is given, then the name of the new widget "
		"(which must include the grid's name before the new widget's name!). The "
		"widget's row and column index are then specified, in that order.\n"
		"See <tt>addWidget()</tt> for information on the final parameter to this "
		"function.\n"
		"An error will be logged if the widget type was invalid, the immediate "
		"container specified in the given widget name did not identify a "
		"<tt>Grid</tt>, and if there was already a widget in the specified cell.");

	r = engine->RegisterGlobalFunction("void setWidgetAlignmentInGrid("
		"const string&in, const uint, const uint, const WidgetAlignment)",
		asMETHOD(sfx::gui, _setWidgetAlignmentInGrid),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's alignment within its "
		"grid cell.");

	r = engine->RegisterGlobalFunction("void setWidgetPaddingInGrid("
		"const string&in, const uint, const uint, const string&in)",
		asMETHOD(sfx::gui, _setWidgetPaddingInGrid),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's padding within its grid "
		"cell.");
}

void sfx::gui::_registerMenuBarGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("MenuItemID addMenu(const string&in, "
		"const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a new menu. The name of the "
		"<tt>MenuBar</tt> is given, then the text of the new menu. An optional "
		"list of variables can also be given. These variables will be inserted "
		"into the text in the same way as setWidgetText().\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>...SetUp()</tt> function!\n"
		"The ID of the newly created menu is returned. If the function failed, "
		"<tt>NO_MENU_ITEM_ID</tt> is returned.\n"
		"A warning will be logged if no items were added to the previously added "
		"menu.");

	r = engine->RegisterGlobalFunction("MenuItemID addMenuItem(const string&in, "
		"const string&in, array<any>@ const = null)",
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
		"string&in, const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMenuItemIntoLastItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new submenu in the most "
		"recently created menu or submenu, and adds a new menu item into it. The "
		"name of the <tt>MenuBar</tt> is given, then the text of the new menu "
		"item. See addMenu() for an explanation of the array parameter. If there "
		"are no menus, the function will fail. If the most recently created menu "
		"is empty, then this call will have the same effect as addMenuItem(), but "
		"a warning will be logged.\n"
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
}

void sfx::gui::_registerChildWindowGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void autoHandleMinMax("
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

	r = engine->RegisterGlobalFunction("float getTitleBarHeight(const string&in)",
		asMETHOD(sfx::gui, _getTitleBarHeight),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the given widget's titlebar "
		"height. Returns <tt>0.0f</tt> on error.");

	r = engine->RegisterGlobalFunction(
		"array<float>@ getBorderWidths(const string&in)",
		asMETHOD(sfx::gui, _getBorderWidths), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the given widget's border "
		"widths. Always returns an array of size four, even if the function "
		"fails. If the function succeeds, the border widths will be stored in the "
		"following order: left, top, right, bottom.");

	r = engine->RegisterGlobalFunction("void openChildWindow(const string&in, "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _openChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Opens a <tt>ChildWindow</tt> to a given "
		"location. It is also made visible, brought to the front, and restored if "
		"it was maximised or minimised. If the <tt>ChildWindow</tt> was already "
		"open, it will still carry out the aforementioned tasks.");

	r = engine->RegisterGlobalFunction("void closeChildWindow(const string&in)",
		asMETHOD(sfx::gui, _closeChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes a <tt>ChildWindow</tt> by making "
		"it invisible.");

	r = engine->RegisterGlobalFunction("void restoreChildWindow(const string&in)",
		asMETHOD(sfx::gui, _restoreChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Restores a <tt>ChildWindow</tt> if it "
		"was maximised or minimised. If the given <tt>ChildWindow</tt> was "
		"neither, then this function will have no effect.");

	r = engine->RegisterGlobalFunction("bool isChildWindowOpen(const string&in)",
		asMETHOD(sfx::gui, _isChildWindowOpen),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns if a given <tt>ChildWindow</tt> "
		"is open or closed.");
}

void sfx::gui::_registerFileDialogGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void setFileDialogStrings("
		"const string&in, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const, "
		"const string&in, array<any>@ const)",
		asMETHOD(sfx::gui, _setFileDialogStrings), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets every caption in a "
		"<tt>FileDialog</tt>. The name of the widget is given, then each caption, "
		"along with the variables to insert into each. If no variables are to be "
		"inserted, then pass in <tt>null</tt>. See setWidgetText() for more "
		"information. The captions are given in the following order:\n"
		"<ol><li>Title.</li>"
		"<li>Confirm/open button.</li>"
		"<li>Cancel button.</li>"
		"<li>Create folder button.</li>"
		"<li>Filename label.</li>"
		"<li>Name column.</li>"
		"<li>Size column.</li>"
		"<li>Modify column.</li>"
		"<li>All files filter.</li></ol>");

	r = engine->RegisterGlobalFunction("array<string>@ "
		"getFileDialogSelectedPaths(const string&in)",
		asMETHOD(sfx::gui, _getFileDialogSelectedPaths),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Retrieves a list of a "
		"<tt>FileDialog</tt>'s selected paths. If no paths were selected, or an "
		"error occurred, an empty array will be returned.");

	r = engine->RegisterGlobalFunction("void addFileDialogFileTypeFilter("
		"const string&in, const string&in, array<any>@, array<string>@)",
		asMETHOD(sfx::gui, _addFileDialogFileTypeFilter),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a file type filter to the given "
		"<tt>FileDialog</tt>. The name of the dialog is given first, and then the "
		"caption that describes the filter. Variables to be inserted into the "
		"caption come next (pass <tt>null</tt> to insert none). Then, the "
		"expressions that make up the filter come next.");

	r = engine->RegisterGlobalFunction(
		"void clearFileDialogFileTypeFilters(const string&in)",
		asMETHOD(sfx::gui, _clearFileDialogFileTypeFilters),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Clears a <tt>FileDialog</tt>'s file type "
		"filters.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogFileMustExist(const string&in, const bool)",
		asMETHOD(sfx::gui, _setFileDialogFileMustExist),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets whether a <tt>FileDialog</tt>'s "
		"selected path must exist or not.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogDefaultFileFilter(const string&in, const uint64)",
		asMETHOD(sfx::gui, _setFileDialogDefaultFileFilter),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects a <tt>FileDialog</tt>'s file "
		"filter, given a 0-based index of the filter to select.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogPath(const string&in, const string&in)",
		asMETHOD(sfx::gui, _setFileDialogPath),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Set a <tt>FileDialog</tt>'s current "
		"path.");
}

void sfx::gui::_registerMessageBoxGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setMessageBoxStrings(const string&in, const string&in, "
		"array<any>@ const, const string&in, array<any>@ const)",
		asMETHOD(sfx::gui, _setMessageBoxStrings),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Set a <tt>MessageBox</tt>'s title, then "
		"text, along with variables to be inserted into each.");

	r = engine->RegisterGlobalFunction(
		"void addMessageBoxButton(const string&in, const string&in, "
		"array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMessageBoxButton),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Add a button to a <tt>MessageBox</tt>.");
}

void sfx::gui::_registerTabContainerGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"string addTabAndPanel(const string&in, const string&in, "
		"array<any>@ const = null)",
		asMETHOD(sfx::gui, _addTabAndPanel),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a tab to a <tt>TabContainer</tt>. "
		"Returns the name of the <tt>Panel</tt> associated with the tab, or an "
		"empty string if the tab and panel could not be added. The new tab will "
		"be selected.");
}

void sfx::gui::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	_documentGUILibrary(document);
	_registerTypes(engine, document);
	_registerConstants(engine, document);
	_registerNonWidgetGlobalFunctions(engine, document);
	_registerWidgetGlobalFunctions(engine, document);
	_registerDirectionalFlowGlobalFunctions(engine, document);
	_registerSpriteGlobalFunctions(engine, document);
	_registerLabelGlobalFunctions(engine, document);
	_registerEditBoxGlobalFunctions(engine, document);
	_registerRadioButtonAndCheckBoxGlobalFunctions(engine, document);
	_registerListGlobalFunctions(engine, document);
	_registerTreeViewGlobalFunctions(engine, document);
	_registerTabsGlobalFunctions(engine, document);
	_registerContainerGlobalFunctions(engine, document);
	_registerPanelGlobalFunctions(engine, document);
	_registerLayoutGlobalFunctions(engine, document);
	_registerGridGlobalFunctions(engine, document);
	_registerMenuBarGlobalFunctions(engine, document);
	_registerChildWindowGlobalFunctions(engine, document);
	_registerFileDialogGlobalFunctions(engine, document);
	_registerMessageBoxGlobalFunctions(engine, document);
	_registerTabContainerGlobalFunctions(engine, document);
}
