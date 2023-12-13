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
static std::string formatMenuItemID(const sfx::MenuItemID id) {
	return std::to_string(id);
}

void sfx::gui::_documentGUILibrary(
	const std::shared_ptr<DocumentationGenerator>& document) {
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
		"into this topic, see https://tgui.eu/tutorials/latest-stable/layouts/. "
		"To specify pixel values, you can issue \"5px\", or \"5\", for example.");
	document->DocumentExpectedFunction("WidgetTypes",
		"Widget types are essentially the GUI backend's class names but in string "
		"form. https://tgui.eu/documentation/latest-stable/annotated.html is the "
		"backend's reference documentation, which lists all of the widgets "
		"available. Note that support might be patchy, though. If you find a "
		"widget or an operation that is not supported, you will have to add "
		"support yourself.");
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

	engine->RegisterTypedef("WidgetID", "uint64");
	document->DocumentExpectedFunction("typedef uint64 WidgetID",
		"Used to uniquely identify widgets created in the engine.");

	r = engine->RegisterGlobalFunction("string formatMenuItemID(const MenuItemID)",
		asFUNCTION(formatMenuItemID), asCALL_CDECL);
	document->DocumentGlobalFunction(r, "Formats a MenuItemID value as a string.");

	r = engine->RegisterFuncdef("void SingleSignalHandler()");
	document->DocumentObjectFuncDef(r, "The signature of a callback that is "
		"invoked when a specific widget emits a specific signal.");

	r = engine->RegisterFuncdef("void MultiSignalHandler(" WIDGET_ID_PARAM ", "
		"const string&in)");
	document->DocumentObjectFuncDef(r, "The signature of a callback that is "
		"invoked when a widget emits any signal. The ID of the widget emitting "
		"the signal will be passed, as well as the name of the signal being "
		"emitted.");

	r = engine->RegisterFuncdef("void ChildWindowClosingSignalHandler("
		WIDGET_ID_PARAM ", bool&out)");
	document->DocumentObjectFuncDef(r, "The signature of a callback that is "
		"invoked when a <tt>ChildWindow</tt> emits the <tt>Closing</tt> signal. "
		"The ID of the widget emitting the signal will be passed, as well as a "
		"reference to a bool that's used to determine if the <tt>ChildWindow</tt> "
		"should actually close or not. It defaults to <tt>TRUE</tt>, and can be "
		"set to <tt>FALSE</tt> to prevent the <tt>ChildWindow</tt> from closing.");
}

void sfx::gui::_registerConstants(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalProperty("const WidgetID NO_WIDGET",
		&sfx::NO_WIDGET);
	document->DocumentExpectedFunction("const WidgetID NO_WIDGET", "Constant "
		"which represents \"no widget.\"");

	r = engine->RegisterGlobalProperty("const WidgetID ROOT_WIDGET",
		&sfx::ROOT_WIDGET);
	document->DocumentExpectedFunction("const WidgetID ROOT_WIDGET", "Constant "
		"which represents the root widget that all widgets must be children of in "
		"some way if they are to be displayed to the user.");

	r = engine->RegisterGlobalProperty("const float NO_SPACE", &NO_SPACE);
	document->DocumentExpectedFunction("const float NO_SPACE", "Constant which "
		"represents \"no space between widgets in a vertical or horizontal "
		"layout.\" Due to rounding errors, however, this likely won't be "
		"perfect, especially when scaling is applied.");

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

	r = engine->RegisterGlobalProperty("const WidgetID GOTO_PREVIOUS_WIDGET",
		&GOTO_PREVIOUS_WIDGET);
	document->DocumentExpectedFunction("const WidgetID GOTO_PREVIOUS_WIDGET",
		"Used with directional flow functions to represent \"going to the widget "
		"that was previously selected using directional flow.\"");

	r = engine->RegisterGlobalProperty("const string VALIDATOR_ALL",
		&VALIDATOR_ALL);
	document->DocumentExpectedFunction("const string VALIDATOR_ALL",
		"<tt>EditBox</tt> validator that accepts any character.");

	r = engine->RegisterGlobalProperty("const string VALIDATOR_INT",
		&VALIDATOR_INT);
	document->DocumentExpectedFunction("const string VALIDATOR_INT",
		"<tt>EditBox</tt> validator that accepts signed or unsigned integers.");

	r = engine->RegisterGlobalProperty("const string VALIDATOR_UINT",
		&VALIDATOR_UINT);
	document->DocumentExpectedFunction("const string VALIDATOR_UINT",
		"<tt>EditBox</tt> validator that accepts only unsigned integers.");

	r = engine->RegisterGlobalProperty("const string VALIDATOR_FLOAT",
		&VALIDATOR_FLOAT);
	document->DocumentExpectedFunction("const string VALIDATOR_FLOAT",
		"<tt>EditBox</tt> validator that accepts floating point numbers.");

	REGISTER_WIDGET_TYPE_NAME(engine, document, BitmapButton)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ListBox)
	REGISTER_WIDGET_TYPE_NAME(engine, document, VerticalLayout)
	REGISTER_WIDGET_TYPE_NAME(engine, document, HorizontalLayout)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Picture)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Label)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ScrollablePanel)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Panel)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Group)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Grid)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Button)
	REGISTER_WIDGET_TYPE_NAME(engine, document, EditBox)
	REGISTER_WIDGET_TYPE_NAME(engine, document, MenuBar)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ChildWindow)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ComboBox)
	REGISTER_WIDGET_TYPE_NAME(engine, document, FileDialog)
	REGISTER_WIDGET_TYPE_NAME(engine, document, MessageBox)
	REGISTER_WIDGET_TYPE_NAME(engine, document, HorizontalWrap)
	REGISTER_WIDGET_TYPE_NAME(engine, document, Tabs)
	REGISTER_WIDGET_TYPE_NAME(engine, document, TreeView)
	REGISTER_WIDGET_TYPE_NAME(engine, document, CheckBox)
	REGISTER_WIDGET_TYPE_NAME(engine, document, RadioButton)
	REGISTER_WIDGET_TYPE_NAME(engine, document, TabContainer)
	REGISTER_WIDGET_TYPE_NAME(engine, document, TextArea)
	REGISTER_WIDGET_TYPE_NAME(engine, document, SpinControl)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ClickableWidget)
	REGISTER_WIDGET_TYPE_NAME(engine, document, ButtonBase)
	REGISTER_WIDGET_TYPE_NAME(engine, document, BoxLayout)
	REGISTER_WIDGET_TYPE_NAME(engine, document, BoxLayoutRatios)

	REGISTER_SIGNAL_TYPE_NAME(engine, document, PositionChanged);
	REGISTER_SIGNAL_TYPE_NAME(engine, document, SizeChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Focused)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Unfocused)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, MouseEntered)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, MouseLeft)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ShowEffectFinished)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, AnimationFinished)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, MousePressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, MouseReleased)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Clicked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, DoubleClicked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, RightMousePressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, RightMouseReleased)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, RightClicked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Pressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ItemSelected)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, HeaderClicked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, MenuItemClicked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Expanded)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Collapsed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Closed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, EscapeKeyPressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Minimized)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Maximized)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Closing)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ColorChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, OkPressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, TextChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ReturnKeyPressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, CaretPositionChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, FileSelected)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ButtonPressed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, ValueChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, RangeChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Full)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Checked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Unchecked)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, Changed)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, SelectionChanging)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, SelectionChanged)
	REGISTER_SIGNAL_TYPE_NAME(engine, document, TabSelected)
}

void sfx::gui::_registerMenuInterface(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	static const auto Menu = "Menu";
	auto r = engine->RegisterInterface(Menu);
	document->DocumentInterface(r, "Represents a menu in a GUI. The constructor "
		"of each subclass is called when the GUI JSON script is loaded. It should "
		"be used to setup widgets \"within\" the menu that will always exist for "
		"the entire lifetime of the menu. The order of subclass construction "
		"cannot be guaranteed.");

	r = engine->RegisterInterfaceMethod(Menu,
		std::string("void Open(").append(Menu).append("@ const)").c_str());
	document->DocumentInterfaceMethod(r, "When switching to the menu, this method "
		"will be called. The given handle points to the menu that was open before "
		"this one. If it's <tt>null</tt>, there was no menu open before this "
		"one.");

	r = engine->RegisterInterfaceMethod(Menu,
		std::string("void Close(").append(Menu).append("@ const)").c_str());
	document->DocumentInterfaceMethod(r, "When switching from the menu, this "
		"method will be called. The given handle points to the menu that will be "
		"opened next. Will never be <tt>null</tt>.");

	r = engine->RegisterInterfaceMethod(Menu, "void Periodic(const dictionary, "
		"const dictionary, const MousePosition&in, const MousePosition&in)");
	document->DocumentInterfaceMethod(r, "This method is called as part of the "
		"C++ engine's main loop, allowing the menu to react to any controls that "
		"the user is pressing.\n\n"
		"The dictionary parameters map control names (strings defined by the UI "
		"JSON configuration script) to bools, where <tt>TRUE</tt> means that the "
		"control is being pressed and should be reacted to, and <tt>FALSE</tt> "
		"means the control should be ignored for that iteration of the game loop. "
		"The first dictionary stores <tt>TRUE</tt> or <tt>FALSE</tt> for all "
		"controls and every type of input (e.g. keyboard, mouse, gamepad), and "
		"the second dictionary stores <tt>TRUE</tt> or <tt>FALSE</tt> for all "
		"controls and only the mouse input. So if a control is being pressed by "
		"the keyboard, but not the mouse, the first dictionary will store "
		"<tt>TRUE</tt> but the second dictionary will store <tt>FALSE</tt>. And "
		"if a control is being pressed by the mouse, then both dictionaries will "
		"store <tt>TRUE</tt>.\n\n"
		"The first <tt>MousePosition</tt> parameter stores the position of the "
		"mouse during the previous iteration of the game loop, and the second "
		"<tt>MousePosition</tt> parameter stores the position of the mouse during "
		"the current iteration of the game loop.");
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

	r = engine->RegisterGlobalFunction("Menu@ getMenu(const string&in)",
		asMETHOD(sfx::gui, _getMenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns a handle to the <tt>Menu</tt> "
		"object corresponding to the specified menu. <tt>null</tt> is returned if "
		"the given menu doesn't exist.");
}

void sfx::gui::_registerWidgetGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("WidgetID getWidgetUnderMouse()",
		asMETHOD(sfx::gui, getWidgetUnderMouse), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the ID of the widget that is "
		"under the current mouse position. <tt>NO_WIDGET</tt> if there isn't "
		"one.");

	r = engine->RegisterGlobalFunction("bool widgetExists(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _widgetExists), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns <tt>TRUE</tt> if a widget with "
		"the given ID exists, <tt>FALSE</tt> otherwise.");

	r = engine->RegisterGlobalFunction(
		"WidgetID getWidgetFocused(" WIDGET_ID_PARAM " = NO_WIDGET)",
		asMETHOD(sfx::gui, _getWidgetFocused), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the ID of the widget that "
		"currently has setfocus within the given container widget. If there is no "
		"widget with setfocus in the given container, or an error occurred, "
		"<tt>NO_WIDGET</tt> will be returned. If <tt>NO_WIDGET</tt> is given, the "
		"root GUI will be queried. This function does not search recursively.");

	r = engine->RegisterGlobalFunction("WidgetID createWidget(const string&in)",
		asMETHOD(sfx::gui, _createWidgetScriptInterface),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new widget. The type of widget "
		"is given. An error will be logged if the widget type was invalid, and "
		"<tt>NO_WIDGET</tt> will be returned.");

	r = engine->RegisterGlobalFunction("void connectSignal(" WIDGET_ID_PARAM ", "
		"const string&in, SingleSignalHandler@ const)",
		asMETHODPR(sfx::gui, _connectSignal, (const WidgetIDRef,
			const std::string&, asIScriptFunction* const), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Connects a handler to a given signal for "
		"a given widget. <tt>null</tt> can be given to disconnect single signal "
		"handlers.");

	r = engine->RegisterGlobalFunction("void connectSignal(" WIDGET_ID_PARAM ", "
		"MultiSignalHandler@ const)",
		asMETHODPR(sfx::gui, _connectSignal, (const WidgetIDRef,
			asIScriptFunction* const), void), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Connects a handler for all signals for a "
		"a given widget. This handler is invoked after any specific signal "
		"handlers have been invoked. <tt>null</tt> can be given to disconnect the "
		"multi signal handler.");

	r = engine->RegisterGlobalFunction(
		"void disconnectSignals(const array<WidgetID>@ const)",
		asMETHOD(sfx::gui, _disconnectSignals), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "This method disconnects all signal "
		"handlers that have been assigned to each of the widgets given in the "
		"array. If <tt>null</tt> is given, a warning will be logged and nothing "
		"will be changed.");

	r = engine->RegisterGlobalFunction("WidgetID getParent(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getParent), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "This method returns the ID of the parent "
		"of the given widget. <tt>NO_WIDGET</tt> if there is no parent.");

	r = engine->RegisterGlobalFunction("void deleteWidget(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _deleteWidget), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes a given widget. If the given "
		"widget is a container, then all of its widgets will be removed "
		"recursively. The <tt>ROOT_WIDGET</tt> cannot be deleted!");

	r = engine->RegisterGlobalFunction("void setWidgetName(" WIDGET_ID_PARAM ", "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetName), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's name. The ID of the "
		"widget is given, then the new name.");

	r = engine->RegisterGlobalFunction("string getWidgetName(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getWidgetName), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's name.");

	r = engine->RegisterGlobalFunction("void setWidgetFocus(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _setWidgetFocus), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Updates the setfocus to point to a given "
		"widget.");

	r = engine->RegisterGlobalFunction("void setWidgetFont(" WIDGET_ID_PARAM ", "
		"const string&in)",
		asMETHOD(sfx::gui, _setWidgetFont), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's font. The ID of the "
		"widget is given, then the name of the font, as defined in the fonts.json "
		"script.");

	r = engine->RegisterGlobalFunction("void setWidgetInheritedFont("
		WIDGET_ID_PARAM ", const string&in)",
		asMETHOD(sfx::gui, _setWidgetInheritedFont),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's inherited font, meaning "
		"this font will be used by all of the children of this widget unless they "
		"override it. The ID of the widget is given, then the name of the font, "
		"as defined in the fonts.json script.");

	r = engine->RegisterGlobalFunction("void setWidgetPosition(" WIDGET_ID_PARAM
		", const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetPosition), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's position. The ID of the "
		"widget is given, then the X position, then the Y position.");

	r = engine->RegisterGlobalFunction("Vector2f getWidgetAbsolutePosition("
		WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getWidgetAbsolutePosition),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's absolute position. The "
		"ID of the widget is given, then the position of the top-left point of "
		"the widget is returned. Returns (0.0f,0.0f) upon an error.");

	r = engine->RegisterGlobalFunction("void setWidgetOrigin(" WIDGET_ID_PARAM ", "
		"const float, const float)",
		asMETHOD(sfx::gui, _setWidgetOrigin), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's origin. The ID of the "
		"widget is given first. Then the new origin is given: along the X axis, "
		"and then along the Y axis. Each origin is a value between 0 and 1, and "
		"represents a percentage, from left/top to right/bottom.");

	r = engine->RegisterGlobalFunction("void setWidgetSize(" WIDGET_ID_PARAM ", "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's size. The ID of the "
		"widget is given, then the width, then the height. If either the width or "
		"height is a blank string, then the engine will retain the layout that is "
		"currently set to that width or height.");

	r = engine->RegisterGlobalFunction(
		"Vector2f getWidgetFullSize(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getWidgetFullSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's full size, which "
		"includes any borders it may have, etc.");

	r = engine->RegisterGlobalFunction("void setWidgetEnabled(" WIDGET_ID_PARAM
		", const bool)",
		asMETHOD(sfx::gui, _setWidgetEnabled), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's enabled state. The ID of "
		"the widget is given, then if it should be enabled or not.");

	r = engine->RegisterGlobalFunction("bool getWidgetEnabled(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _getWidgetEnabled), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's enabled state. Returns "
		"<tt>FALSE</tt> on error.");

	r = engine->RegisterGlobalFunction("void setWidgetVisibility(" WIDGET_ID_PARAM
		", const bool)",
		asMETHOD(sfx::gui, _setWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's visibility. The ID of "
		"the widget is given, then if it should be visible or not.");

	r = engine->RegisterGlobalFunction("bool getWidgetVisibility(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _getWidgetVisibility), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's visibility. Returns "
		"<tt>FALSE</tt> on error.");

	r = engine->RegisterGlobalFunction("void moveWidgetToFront(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _moveWidgetToFront), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Moves a given widget to the front of its "
		"parent container.");

	r = engine->RegisterGlobalFunction("void moveWidgetToBack(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _moveWidgetToBack), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Moves a given widget to the back of its "
		"parent container.");

	r = engine->RegisterGlobalFunction("void setWidgetText(" WIDGET_ID_PARAM ", "
		"const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _setWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, std::string("Sets a widget's text. The "
		"ID of the widget is given, then its new text. An optional list of "
		"variables can also be given. These variables will be inserted into the "
		"text wherever a '" + std::to_string(engine::expand_string::getVarChar()) +
		"' is found.\n"
		"This function has special behaviour for <tt>EditBox</tt> and "
		"<tt>TextArea</tt>widgets. No variables are inserted into the given "
		"string, and it is not translated. The <tt>EditBox</tt>'s or "
		"<tt>TextArea</tt>'s current text is simply replaced with whatever string "
		"is given.").c_str());

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextSize(" WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setWidgetTextSize), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's character size. The ID "
		"of the widget is given, then its new character size.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetIndex(" WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setWidgetIndex),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a "
		"container.");
}

void sfx::gui::_registerDirectionalFlowGlobalFunctions(
	asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlow("
		WIDGET_ID_PARAM ", " WIDGET_ID_PARAM ", " WIDGET_ID_PARAM ", "
		WIDGET_ID_PARAM ", " WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlow),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the widgets that should be selected "
		"if directional controls are input when the given widget is currently "
		"selected. The \"given widget\" should be given first, followed by the "
		"widgets that should be selected, when up, down, left, and right are "
		"input, respectively. All given widgets should be in the same menu. "
		"<tt>NO_WIDGET</tt> means that the input won't change the selected "
		"widget. <tt>GOTO_PREVIOUS_WIDGET</tt> means \"navigate back to the "
		"previously selected widget.\"");

	r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlowStart("
		"const string&in, " WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlowStart),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets which widget should be selected "
		"first when a directional control is first input on the given menu. Best "
		"practice is to give a widget that is \"within\" the menu. You can give "
		"<tt>NO_WIDGET</tt> to explicitly prevent directional controls from "
		"selecting a widget for the given menu (which is the default behaviour "
		"for all menus).");

	r = engine->RegisterGlobalFunction("void setWidgetDirectionalFlowSelection("
		"const string&in, " WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _setWidgetDirectionalFlowSelection),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Manually select a widget within the "
		"given menu which the user can move away from using the directional "
		"controls.");

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
	auto r = engine->RegisterGlobalFunction("void setWidgetSprite("
		WIDGET_ID_PARAM ", const string&in, const string&in)",
		asMETHOD(sfx::gui, _setWidgetSprite), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's sprite. The ID of the "
		"widget is given, then the name of the sprite sheet, then the name of the "
		"sprite. You can provide an empty spritesheet and sprite to clear the "
		"widget's sprite.");

	r = engine->RegisterGlobalFunction("void matchWidgetSizeToSprite("
		WIDGET_ID_PARAM ", const bool)",
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
	auto r = engine->RegisterGlobalFunction("void setWidgetTextStyles("
		WIDGET_ID_PARAM ", const string&in)",
		asMETHOD(sfx::gui, _setWidgetTextStyles), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text styles. The ID of "
		"the widget is given, then its new styles. It can be a combination of "
		"<a href=\"https://tgui.eu/documentation/latest-stable/namespacetgui.html#aafa478ba31ef52a263be37506428943bac6084aa83d23b9324edfdcf20d1901e0\" target=\"_blank\">"
		"these enums in string form</a>, joined together using \" | \".");

	r = engine->RegisterGlobalFunction("void setWidgetTextMaximumWidth("
		WIDGET_ID_PARAM ", const float)",
		asMETHOD(sfx::gui, _setWidgetTextMaximumWidth),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's maximum text width. The "
		"ID of the widget is given, then its new maximum text width.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextColour(" WIDGET_ID_PARAM ", const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetTextColour), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextOutlineColour(" WIDGET_ID_PARAM ", const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetTextOutlineColour),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text outline colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetTextOutlineThickness(" WIDGET_ID_PARAM ", const float)",
		asMETHOD(sfx::gui, _setWidgetTextOutlineThickness),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text outline thickness.");

	r = engine->RegisterGlobalFunction("void setWidgetTextAlignment("
		WIDGET_ID_PARAM ", const HorizontalAlignment, const VerticalAlignment)",
		asMETHOD(sfx::gui, _setWidgetTextAlignment),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's text horizontal and "
		"vertical alignment.");
}

void sfx::gui::_registerEditBoxGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"string getWidgetText(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getWidgetText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets an <tt>EditBox</tt>'s or "
		"<tt>TextArea</tt>'s text.");

	r = engine->RegisterGlobalFunction(
		"void setEditBoxRegexValidator(" WIDGET_ID_PARAM ", const string&in)",
		asMETHOD(sfx::gui, _setEditBoxRegexValidator),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Configures an <tt>EditBox</tt> to "
		"validate its input using a regex expression. If the text within an "
		"<tt>EditBox</tt> does not match the regex, it will be rejected.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetDefaultText(" WIDGET_ID_PARAM ", const string&in, "
		"array<any>@ const = null)",
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

	r = engine->RegisterGlobalFunction(
		"void optimiseTextAreaForMonospaceFont(" WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _optimiseTextAreaForMonospaceFont),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Enables or disables monospace font "
		"optimisations for a given <tt>TextArea</tt>. By default it is off.");

	r = engine->RegisterGlobalFunction(
		"void getCaretLineAndColumn(" WIDGET_ID_PARAM ", uint64&out, uint64&out)",
		asMETHOD(sfx::gui, _getCaretLineAndColumn),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Retrieves the caret's 1-based position "
		"within a <tt>TextArea</tt> or <tt>EditBox</tt>. <tt>EditBox</tt> will "
		"always have a line number of <tt>1</tt>. If an error occurred, neither "
		"of the given parameters are changed.");
}

void sfx::gui::_registerRadioButtonAndCheckBoxGlobalFunctions(
	asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetChecked(" WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _setWidgetChecked), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's checked state. The ID of "
		"the widget is given, then if it should be checked or not.");

	r = engine->RegisterGlobalFunction("bool isWidgetChecked(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _isWidgetChecked), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's checked state. The ID of "
		"the widget is given. Returns <tt>FALSE</tt> if the checked status could "
		"not be retrieved.");
}

void sfx::gui::_registerListGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void addItem(" WIDGET_ID_PARAM ", "
		"const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new item to a widget. The ID "
		"of the widget is given, then the text of the new item. An optional list "
		"of variables can also be given: see <tt>setWidgetText()</tt> for more "
		"information.");

	r = engine->RegisterGlobalFunction("void clearItems(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _clearItems), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all items from a given widget.");

	r = engine->RegisterGlobalFunction("void setSelectedItem(" WIDGET_ID_PARAM ", "
		"const uint64)", asMETHODPR(sfx::gui, _setSelectedItem,
			(const sfx::WidgetIDRef, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects an item from a widget. The ID of "
		"the widget is given, then the 0-based index of the item to select.");

	r = engine->RegisterGlobalFunction("void deselectItem(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _deselectItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Deselects an item from a widget.");

	r = engine->RegisterGlobalFunction("int getSelectedItem(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getSelectedItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's index. "
		"If no item is selected, or the index of the selected item couldn't be "
		"retrieved, <tt>-1</tt> is returned.");

	r = engine->RegisterGlobalFunction("string getSelectedItemText("
		WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getSelectedItemText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected item's text.");

	r = engine->RegisterGlobalFunction(
		"void setItemsToDisplay(" WIDGET_ID_PARAM ", const uint64)",
		asMETHOD(sfx::gui, _setItemsToDisplay), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the number of items to display in a "
		"given <tt>ComboBox</tt> when open, at one time. 0 means always show all "
		"items.");
}

void sfx::gui::_registerTreeViewGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"array<string>@ getSelectedItemTextHierarchy(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getSelectedItemTextHierarchy),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a <tt>TreeView</tt>'s selected "
		"item's text, including the text of its parents. If it couldn't be "
		"retrieved, or if there isn't a selected item, an empty array will be "
		"returned.");

	r = engine->RegisterGlobalFunction(
		"void addTreeViewItem(" WIDGET_ID_PARAM ", const array<string>@ const)",
		asMETHOD(sfx::gui, _addTreeViewItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds an item to a <tt>TreeView</tt> "
		"widget. The array describes the hierarchy of the new item. If parent "
		"items do not exist, then they will be created. Note that "
		"<tt>TreeView</tt> items are not translated!");
}

void sfx::gui::_registerTabsGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void addTab(" WIDGET_ID_PARAM ", "
		"const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addTab), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Appends a new tab to a <tt>Tabs</tt> "
		"widget. The ID of the widget is given, then the text of the new tab. An "
		"optional list variables can also be given: see <tt>setWidgetText()</tt> "
		"for more information. The new tab will not be selected automatically.");

	r = engine->RegisterGlobalFunction("void setSelectedTab(" WIDGET_ID_PARAM ", "
		"const uint64)", asMETHODPR(sfx::gui, _setSelectedTab,
			(const sfx::WidgetIDRef, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects a tab from a widget. The ID of "
		"the widget is given, then the 0-based index of the tab to select.\n"
		"For <tt>Tabs</tt> widgets: if the tab is invisible or disabled, the "
		"selection operation will fail. If the operation fails for any reason, an "
		"attempt will be made to select the previously selected tab, if there was "
		"one.\n"
		"For <tt>TabContainer</tt> widgets: the SelectionChanging signal will "
		"emit if the given index is valid and not the same as the index of the "
		"tab that is currently selected. This signal has the opportunity to veto "
		"the tab selection.");

	r = engine->RegisterGlobalFunction("int getSelectedTab(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getSelectedTab), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's selected tab's index. If "
		"and error ocurred, or there wasn't a selected tab, <tt>-1</tt> will be "
		"returned.");

	r = engine->RegisterGlobalFunction("uint64 getTabCount(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getTabCount), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's tab count.");

	r = engine->RegisterGlobalFunction(
		"string getTabText(" WIDGET_ID_PARAM ", const uint64)",
		asMETHOD(sfx::gui, _getTabText), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's tab's translated text. "
		"Returns an empty string on error.");
}

void sfx::gui::_registerContainerGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void add("
		WIDGET_ID_PARAM ", " WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _add),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a widget (second parameter) to a "
		"container widget (first parameter). If the first parameter is "
		"<tt>NO_WIDGET</tt>, or the second parameter is <tt>NO_WIDGET</tt> or "
		"<tt>ROOT_WIDGET</tt>, an error will be logged. If a widget is added to "
		"the root container directly, the widget will be made invisible.");

	r = engine->RegisterGlobalFunction("void remove("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _remove),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes a widget from its container. "
		"Logs an error if the given widget didn't have a parent.");

	r = engine->RegisterGlobalFunction("void removeAll("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _removeAll),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes all widgets from a given "
		"container widget.");

	r = engine->RegisterGlobalFunction("void deleteWidgetsFromContainer("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _deleteWidgetsFromContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Deletes all the widgets from a given "
		"container, but does not delete the container itself.");

	r = engine->RegisterGlobalFunction("void setWidgetIndexInContainer("
		WIDGET_ID_PARAM ", const uint, const uint)",
		asMETHOD(sfx::gui, _setWidgetIndexInContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's index within a given "
		"container. The ID of the container is given, then the index of the "
		"widget to edit, with its new index given as the last parameter.");

	r = engine->RegisterGlobalFunction("uint getWidgetCount(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getWidgetCount), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets the number of widgets that are in "
		"the specified container. Does not count recursively.");

	r = engine->RegisterGlobalFunction("void setGroupPadding("
		WIDGET_ID_PARAM ", const string&in)",
			asMETHODPR(sfx::gui, _setGroupPadding,
			(const sfx::WidgetIDRef, const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a group's padding. If a "
		"<tt>Grid</tt> is given, each of its widgets will be assigned the given "
		"padding, but note that it will only work once all of its widgets have "
		"been added and newly added widgets won't have the padding applied "
		"automatically!");

	r = engine->RegisterGlobalFunction("void setGroupPadding(" WIDGET_ID_PARAM ", "
		"const string&in, const string&in, const string&in, const string&in)",
		asMETHODPR(sfx::gui, _setGroupPadding, (const sfx::WidgetIDRef,
			const std::string&, const std::string&, const std::string&,
			const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a group's padding. The group's ID "
		"is given, then the padding applied to the left, top, right, then bottom "
		"sides. If a <tt>Grid</tt> is given, each of its widgets will be assigned "
		"the given padding, but note that it will only work once all of its "
		"widgets have been added and newly added widgets won't have the padding "
		"applied automatically!");

	r = engine->RegisterGlobalFunction("void applySpritesToWidgetsInContainer("
		WIDGET_ID_PARAM ", const string&in, const array<string>@ const)",
		asMETHOD(sfx::gui, _applySpritesToWidgetsInContainer),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Loops through a given container (whose "
		"ID is given as the first parameter), and applies sprites from a given "
		"spritesheet (second parameter) to each <tt>BitmapButton</tt> and "
		"<tt>Picture</tt> that is found. The first applicable widget will receive "
		"the first sprite in the array, the second applicable widget the second "
		"sprite, and so on.");
}

void sfx::gui::_registerPanelGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetBackgroundColour(" WIDGET_ID_PARAM ", const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetBgColour), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's background colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderSize(" WIDGET_ID_PARAM ", const float)",
		asMETHOD(sfx::gui, _setWidgetBorderSize),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's border size. Applies the "
		"same size to each side of the widget.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderColour(" WIDGET_ID_PARAM ", const Colour&in)",
		asMETHOD(sfx::gui, _setWidgetBorderColour),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's border colour.");

	r = engine->RegisterGlobalFunction(
		"void setWidgetBorderRadius(" WIDGET_ID_PARAM ", const float)",
		asMETHOD(sfx::gui, _setWidgetBorderRadius),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's rounded border radius.");

	r = engine->RegisterGlobalFunction("void setHorizontalScrollbarPolicy("
		WIDGET_ID_PARAM ", const ScrollbarPolicy)",
		asMETHOD(sfx::gui, _setHorizontalScrollbarPolicy),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's or TextArea's "
		"horizontal scrollbar policy.");

	r = engine->RegisterGlobalFunction("void setHorizontalScrollbarAmount("
		WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setHorizontalScrollbarAmount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's horizontal "
		"scroll amount.");

	r = engine->RegisterGlobalFunction("void setVerticalScrollbarAmount("
		WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setVerticalScrollbarAmount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's vertical scroll "
		"amount.");

	r = engine->RegisterGlobalFunction("void setVerticalScrollbarValue("
		WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setVerticalScrollbarValue),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a ScrollablePanel's vertical scroll "
		"value.");

	r = engine->RegisterGlobalFunction("float getScrollbarWidth(" WIDGET_ID_PARAM
		")", asMETHOD(sfx::gui, _getScrollbarWidth),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a ScrollablePanel's scrollbar "
		"width.");
}

void sfx::gui::_registerLayoutGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void "
		"setWidgetRatioInLayout(" WIDGET_ID_PARAM ", const uint, const float)",
		asMETHOD(sfx::gui, _setWidgetRatioInLayout),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Within a <tt>VerticalLayout</tt> or "
		"<tt>HorizontalLayout</tt>; sets the ratio of a specified widget's size "
		"compared to others in the layout. The unsigned integer is a 0-based "
		"index of the widget in the layout to amend.");

	r = engine->RegisterGlobalFunction("void setSpaceBetweenWidgets("
		WIDGET_ID_PARAM ", const float)",
		asMETHOD(sfx::gui, _setSpaceBetweenWidgets),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the space between widgets in a "
		"vertical or horizontal layout.");
}

void sfx::gui::_registerGridGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("WidgetID addWidgetToGrid("
		WIDGET_ID_PARAM ", " WIDGET_ID_PARAM ", const uint, const uint)",
		asMETHODPR(sfx::gui, _addWidgetToGrid, (const WidgetIDRef,
			const WidgetIDRef, const std::size_t, const std::size_t), void),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a widget to a grid. The ID of the "
		"grid widget is given, then the ID of the widget to add. The widget's row "
		"and column index are then specified, in that order.\n"
		"An error will be logged if the second widget was the root widget, or the "
		"first widget did not identify a <tt>Grid</tt>.");

	r = engine->RegisterGlobalFunction("void setWidgetAlignmentInGrid("
		WIDGET_ID_PARAM ", const uint, const uint, const WidgetAlignment)",
		asMETHOD(sfx::gui, _setWidgetAlignmentInGrid),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's alignment within its "
		"grid cell.");

	r = engine->RegisterGlobalFunction("void setWidgetPaddingInGrid("
		WIDGET_ID_PARAM ", const uint, const uint, const string&in)",
		asMETHOD(sfx::gui, _setWidgetPaddingInGrid),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets a widget's padding within its grid "
		"cell.");

	r = engine->RegisterGlobalFunction("uint64 getWidgetColumnCount("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _getWidgetColumnCount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's column count.");

	r = engine->RegisterGlobalFunction("uint64 getWidgetRowCount("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _getWidgetRowCount),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Gets a widget's row count.");
}

void sfx::gui::_registerMenuBarGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("MenuItemID addMenu(" WIDGET_ID_PARAM
		", const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a new menu. The ID of the "
		"<tt>MenuBar</tt> is given, then the text of the new menu. An optional "
		"list of variables can also be given. These variables will be inserted "
		"into the text in the same way as setWidgetText().\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>Menu</tt> constructor!\n"
		"The ID of the newly created menu is returned. If the function failed, "
		"<tt>NO_MENU_ITEM_ID</tt> is returned.\n"
		"A warning will be logged if no items were added to the previously added "
		"menu.");

	r = engine->RegisterGlobalFunction("MenuItemID addMenuItem(" WIDGET_ID_PARAM
		", const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMenuItem), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a new menu item. The ID of the "
		"<tt>MenuBar</tt> is given, then the text of the new menu item. See "
		"addMenu() for an explanation of the array parameter. The menu item will "
		"be added to the most recently created menu (addMenu()) or submenu "
		"(addMenuItemIntoLastItem()). If there are no menus, the function will "
		"fail.\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>Menu</tt> constructor!\n"
		"The ID of the newly created menu item is returned. If the function "
		"failed, <tt>NO_MENU_ITEM_ID</tt> is returned.");

	r = engine->RegisterGlobalFunction("MenuItemID addMenuItemIntoLastItem("
		WIDGET_ID_PARAM ", const string&in, array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMenuItemIntoLastItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Creates a new submenu in the most "
		"recently created menu or submenu, and adds a new menu item into it. The "
		"ID of the <tt>MenuBar</tt> is given, then the text of the new menu item. "
		"See addMenu() for an explanation of the array parameter. If there are no "
		"menus, the function will fail. If the most recently created menu is "
		"empty, then this call will have the same effect as addMenuItem(), but a "
		"warning will be logged.\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>Menu</tt> constructor!\n"
		"The ID of the newly created menu item is returned. If the function "
		"failed, <tt>NO_MENU_ITEM_ID</tt> is returned.");

	r = engine->RegisterGlobalFunction("void exitSubmenu(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _exitSubmenu), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Exits the current submenu. The ID of the "
		"<tt>MenuBar</tt> is given. This function can be used to go up one level "
		"in the menu hierarchy at a time. If the current hierarchy level is less "
		"than three (i.e. the given menu bar is not in a submenu), then the "
		"function will fail.\n"
		"<b><u>WARNING:</u></b> this function will fail if called outside of a "
		"<tt>Menu</tt> constructor!");

	r = engine->RegisterGlobalFunction("MenuItemID getLastSelectedMenuItem("
		WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getLastSelectedMenuItem),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the 0-based ID of the last "
		"selected menu item of the given <tt>MenuBar</tt>. In the event that no "
		"item has been selected yet, or if an error occurred, "
		"<tt>NO_MENU_ITEM_ID</tt> will be returned.");
}

void sfx::gui::_registerChildWindowGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void autoHandleMinMax("
		WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _autoHandleMinMax),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt>, instructs the engine "
		"to handle the minimise and maximise functionality of the given "
		"<tt>ChildWindow</tt>, before invoking the <tt>Minimized</tt> and "
		"<tt>Maximised</tt> signal handlers. This is the default. Use "
		"<tt>FALSE</tt> to disable this functionality for the given "
		"<tt>ChildWindow</tt>.");

	r = engine->RegisterGlobalFunction("void setChildWindowTitleButtons("
		WIDGET_ID_PARAM ", const uint)",
		asMETHOD(sfx::gui, _setChildWindowTitleButtons),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the given <tt>ChildWindow</tt>'s "
		"titlebar buttons. The given integer should be a bitwise-ORed list of "
		"<tt>TitleButton</tt> enum values.");

	r = engine->RegisterGlobalFunction("void setWidgetResizable("
		WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _setWidgetResizable),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt>, the given widget can "
		"be resized by the user, if the widget supports it. If <tt>FALSE</tt>, "
		"only the engine or scripts can resize the given widget.");

	r = engine->RegisterGlobalFunction("void setWidgetPositionLocked("
		WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _setWidgetPositionLocked),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "If <tt>TRUE</tt>, the given widget can't "
		"be moved by the user, if the widget supports it. If <tt>FALSE</tt>, the "
		"user can freely move the widget.");

	r = engine->RegisterGlobalFunction("float getTitleBarHeight(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _getTitleBarHeight),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the given widget's titlebar "
		"height. Returns <tt>0.0f</tt> on error.");

	r = engine->RegisterGlobalFunction(
		"array<float>@ getBorderWidths(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getBorderWidths), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the given widget's border "
		"widths. Always returns an array of size four, even if the function "
		"fails. If the function succeeds, the border widths will be stored in the "
		"following order: left, top, right, bottom.");

	r = engine->RegisterGlobalFunction("void openChildWindow(" WIDGET_ID_PARAM ", "
		"const string&in, const string&in)",
		asMETHOD(sfx::gui, _openChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Opens a <tt>ChildWindow</tt> to a given "
		"location. It is also made visible, brought to the front, and restored if "
		"it was maximised or minimised. If the <tt>ChildWindow</tt> was already "
		"open, it will still carry out the aforementioned tasks.");

	r = engine->RegisterGlobalFunction("void closeChildWindow(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _closeChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes a <tt>ChildWindow</tt> by making "
		"it invisible. Note that this will force a window to close, and will not "
		"invoke the onClosing signal handler!");

	r = engine->RegisterGlobalFunction(
		"void closeChildWindowAndEmitSignal(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _closeChildWindowAndEmitSignal),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Closes a <tt>ChildWindow</tt> by "
		"invoking the onClosing/Closing signal. This gives the scripts a chance "
		"to accept the signal and cancel the close attempt. Otherwise, the window "
		"will be closed via setting its visibility to <tt>FALSE</tt>.");

	r = engine->RegisterGlobalFunction("void restoreChildWindow(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _restoreChildWindow), asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Restores a <tt>ChildWindow</tt> if it "
		"was maximised or minimised. If the given <tt>ChildWindow</tt> was "
		"neither, then this function will have no effect.");

	r = engine->RegisterGlobalFunction("bool isChildWindowOpen(" WIDGET_ID_PARAM
		")",
		asMETHOD(sfx::gui, _isChildWindowOpen),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns if a given <tt>ChildWindow</tt> "
		"is open or closed.");
}

void sfx::gui::_registerFileDialogGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction("void setFileDialogStrings("
		WIDGET_ID_PARAM ", "
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
		"<tt>FileDialog</tt>. The ID of the widget is given, then each caption, "
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
		"getFileDialogSelectedPaths(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _getFileDialogSelectedPaths),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Retrieves a list of a "
		"<tt>FileDialog</tt>'s selected paths. If no paths were selected, or an "
		"error occurred, an empty array will be returned.");

	r = engine->RegisterGlobalFunction("void addFileDialogFileTypeFilter("
		WIDGET_ID_PARAM ", const string&in, array<any>@, array<string>@)",
		asMETHOD(sfx::gui, _addFileDialogFileTypeFilter),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a file type filter to the given "
		"<tt>FileDialog</tt>. The ID of the dialog is given first, and then the "
		"caption that describes the filter. Variables to be inserted into the "
		"caption come next (pass <tt>null</tt> to insert none). Then, the "
		"expressions that make up the filter come next.");

	r = engine->RegisterGlobalFunction(
		"void clearFileDialogFileTypeFilters(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _clearFileDialogFileTypeFilters),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Clears a <tt>FileDialog</tt>'s file type "
		"filters.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogFileMustExist(" WIDGET_ID_PARAM ", const bool)",
		asMETHOD(sfx::gui, _setFileDialogFileMustExist),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets whether a <tt>FileDialog</tt>'s "
		"selected path must exist or not.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogDefaultFileFilter(" WIDGET_ID_PARAM ", const uint64)",
		asMETHOD(sfx::gui, _setFileDialogDefaultFileFilter),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Selects a <tt>FileDialog</tt>'s file "
		"filter, given a 0-based index of the filter to select.");

	r = engine->RegisterGlobalFunction(
		"void setFileDialogPath(" WIDGET_ID_PARAM ", const string&in)",
		asMETHOD(sfx::gui, _setFileDialogPath),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Set a <tt>FileDialog</tt>'s current "
		"path.");
}

void sfx::gui::_registerMessageBoxGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setMessageBoxStrings(" WIDGET_ID_PARAM ", const string&in, "
		"array<any>@ const, const string&in, array<any>@ const)",
		asMETHOD(sfx::gui, _setMessageBoxStrings),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Set a <tt>MessageBox</tt>'s title, then "
		"text, along with variables to be inserted into each.");

	r = engine->RegisterGlobalFunction(
		"void addMessageBoxButton(" WIDGET_ID_PARAM ", const string&in, "
		"array<any>@ const = null)",
		asMETHOD(sfx::gui, _addMessageBoxButton),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Add a button to a <tt>MessageBox</tt>.");

	r = engine->RegisterGlobalFunction("uint64 getLastSelectedButton("
		WIDGET_ID_PARAM ")", asMETHOD(sfx::gui, _getLastSelectedButton),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the 0-based ID of the last "
		"selected button of the given <tt>MessageBox</tt>. In the event that no "
		"button has been selected yet, or if an error occurred, an invalid ID "
		"will be returned.");
}

void sfx::gui::_registerTabContainerGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"WidgetID addTabAndPanel(" WIDGET_ID_PARAM ", const string&in, "
		"array<any>@ const = null)",
		asMETHOD(sfx::gui, _addTabAndPanel),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Adds a tab to a <tt>TabContainer</tt>. "
		"Returns the ID of the <tt>Panel</tt> associated with the tab, or "
		"<tt>NO_WIDGET</tt> if the tab and panel could not be added. The new tab "
		"will not be selected.");

	r = engine->RegisterGlobalFunction(
		"void removeTabAndPanel(" WIDGET_ID_PARAM ")",
		asMETHOD(sfx::gui, _removeTabAndPanel),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Removes a tab from a "
		"<tt>TabContainer</tt>, given the tab's panel's ID. If there are multiple "
		"panels with the same name, the first one found from the left will be "
		"removed. If the given panel's parent is not a <tt>TabContainer</tt>, "
		"then an error will be logged and no widget will be removed.");
}

void sfx::gui::_registerSpinControlGlobalFunctions(asIScriptEngine* const engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	auto r = engine->RegisterGlobalFunction(
		"void setWidgetMinMaxValues(const string&in, const float, const float)",
		asMETHOD(sfx::gui, _setWidgetMinMaxValues),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the minimum and maximum values that "
		"can be selected by this widget. If <tt>min > max</tt>, they will be "
		"adjusted automatically.");

	r = engine->RegisterGlobalFunction(
		"bool setWidgetValue(const string&in, float)",
		asMETHOD(sfx::gui, _setWidgetValue),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Sets the value assigned to this widget. "
		"If it is outside of the configured range, it will be adjusted, and "
		"<tt>FALSE</tt> will be returned. Returns <tt>TRUE</tt> if the value "
		"could be assigned without adjustments or errors.");

	r = engine->RegisterGlobalFunction(
		"float getWidgetValue(const string&in)",
		asMETHOD(sfx::gui, _getWidgetValue),
		asCALL_THISCALL_ASGLOBAL, this);
	document->DocumentGlobalFunction(r, "Returns the value currently set in the "
		"given widget, or <tt>0.0f</tt> if there was an error.");
}

void sfx::gui::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	_documentGUILibrary(document);
	_registerTypes(engine, document);
	_registerConstants(engine, document);
	_registerMenuInterface(engine, document);
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
	_registerSpinControlGlobalFunctions(engine, document);
}
