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

/**@file guimacros.hpp
 * Defines macros used to help write the GUI script interface.
 */

#pragma once

/**
 * Used internally in the script interface when something goes wrong.
 */
class GuiScriptError : public std::exception {
public:
	GuiScriptError(const char* msg) : std::exception(msg) {}
	GuiScriptError(const std::string& msg) : std::exception(msg.c_str()) {}
	GuiScriptError(const tgui::String& msg) :
		std::exception(msg.toStdString().c_str()) {}
};

#undef ERROR
/**
 * Abort the function call.
 * @param m Message describing how the function call failed.
 */
#define ERROR(m) throw GuiScriptError(m);

/**
 * Abort the function call due to unsupported widget type.
 */
#define UNSUPPORTED_WIDGET_TYPE() ERROR("This operation is not supported for " \
	"this type of widget.")

/**
 * Initialise the function call.
 */
#define START() try {

/**
 * Initialise the function call with widget information.
 * This macro defines the following variables:
 * <ul><li><tt>std::vector<std::string> fullname;</tt> Stores the full name of the
 *     widget, with each element storing the name of each widget in the hierarchy.
 *     This variable is accessible in \c END().</li>
 *     <li><tt>std::string fullnameAsString;</tt> Stores the full name of the
 *     widget as a single string. This variable is accessible in \c END().</tt>
 *     <li><tt>tgui::string widgetType;</tt> Holds the type of the widget.
 *     "Unknown" if the widget does not exist.</li>
 *     <li><tt>Widget::Ptr widget;</tt> Points to the widget. If it was not found,
 *     points to \c nullptr.</li>
 *     <li><tt>std::string containerName;</tt> The full name of the container which
 *     contains (or which would contain) the given widget. This is deduced using
 *     \c fullnameAsString.</li>
 *     <li><tt>Container::Ptr container;</tt> Will point to the container
 *     identified by \c containerName. If it does not exist, an error will be
 *     invoked.</li></ul>
 * @param n          The name of the widget to search for.
 * @param must_exist \c TRUE if the given widget must exist, \c FALSE if it must
 *                   not exist. If the test does not pass, an error will be
 *                   invoked.
 */
#define START_WITH_WIDGET_BASE(n, must_exist) std::vector<std::string> fullname; \
	std::string fullnameAsString; \
	tgui::String widgetType("Unknown"); \
	try { \
		Widget::Ptr widget = \
			_findWidget<Widget>(n, &fullname, &fullnameAsString); \
		if (widget.operator bool() != must_exist) { \
			if (must_exist) { \
				ERROR("This widget does not exist!"); \
			} else { \
				ERROR("This widget already exists!"); \
			} \
		} \
		if (widget) widgetType = widget->getWidgetType(); \
		std::string containerName = \
				fullnameAsString.substr(0, fullnameAsString.rfind('.')); \
		Container::Ptr container = _findWidget<Container>(containerName); \
		if (!container) { \
			ERROR(std::string("The container \"").append(containerName). \
				append("\" does not exist!")); \
		}

/// <tt>START_WITH_WIDGET_BASE(n, true)</tt>.
#define START_WITH_WIDGET(n) START_WITH_WIDGET_BASE(n, true)

/// <tt>START_WITH_WIDGET_BASE(n, false)</tt>.
#define START_WITH_NONEXISTENT_WIDGET(n) START_WITH_WIDGET_BASE(n, false)

/**
 * Finish initialising the function call.
 * @param m   This message is always written when an \c ERROR() is reported.
 * @param ... The variables to insert into the message.
 */
#define END(m, ...) } catch (const GuiScriptError& e) { \
		_logger.error(std::string(m).append(" ").append(e.what()), __VA_ARGS__); \
	}

/**
 * Execute code on \c widget if it is of a given type.
 * The code to execute has access to a variable, \c castWidget, which is the
 * result of dynamically casting the \c widget pointer to the given type of widget.
 * @param type The widget type to test for, e.g. Container or ListBox.
 * @param code The code to execute. If \c widget is not of the given type, this
 *             code is not executed.
 */
#define IF_WIDGET_IS(type, code) if (widgetType == #type) { \
		auto castWidget = std::dynamic_pointer_cast<type>(widget); \
		code \
	}

/**
 * Execute code on \c widget if it is of a given type.
 * @sa \c IF_WIDGET_IS()
 */
#define ELSE_IF_WIDGET_IS(type, code) else if (widgetType == #type) { \
		auto castWidget = std::dynamic_pointer_cast<type>(widget); \
		code \
	}

/**
 * Used at the end of a chain of \c IF_WIDGET_IS() and \c ELSE_IF_WIDGET_IS()
 * macros, to invoke \C UNSUPPORTED_WIDGET_TYPE() if \c widget does not match any
 * of the given types.
 * @sa \c IF_WIDGET_IS()
 * @sa \c ELSE_IF_WIDGET_IS()
 */
#define ELSE_UNSUPPORTED() else { UNSUPPORTED_WIDGET_TYPE() }

/**
 * Defines a widget type static variable which holds the string name of a given
 * widget type.
 * @param t The case-sensitive name of the widget type.
 */
#define WIDGET_TYPE(t) static std::string t = #t;

/**
 * Registers a widget type constant with a script interface and documents it.
 * @param e The engine to register the constant with.
 * @param d The documentation generator to send documentation to.
 * @param t The widget type.
 */
#define REGISTER_WIDGET_TYPE_NAME(e, d, t) \
	e->RegisterGlobalProperty("const string " #t, &type::t); \
	d->DocumentExpectedFunction("const string " #t, "The name of the <tt>" #t \
		"</tt> widget type, should be given to functions such as " \
		"<tt>addWidget()</tt>.");
