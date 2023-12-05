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

#include "gui/menumanager.hpp"

void sfx::gui::menu_manager::registerInterface(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	//////////////////
	// DEPENDENCIES //
	//////////////////
	engine::RegisterVectorTypes(engine, document);

	////////////////////
	// MENU INTERFACE //
	////////////////////
	static const auto Menu = "Menu";
	auto r = engine->RegisterInterface(Menu);
	document->DocumentInterface(r, "Represents a menu in a GUI.");

	r = engine->RegisterInterfaceMethod(Menu, "string Name() const");
	document->DocumentInterfaceMethod(r, "This method should return the name of "
		"the menu. It can be any string, but it should uniquely identify the "
		"menu.");

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
		"opened next. If it's <tt>null</tt>, there will be no menu opened after "
		"this one closes.");

	r = engine->RegisterInterfaceMethod(Menu, "void HandleInput(const dictionary, "
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
