/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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
 * <ul><li><tt>tgui::String widgetType;</tt> Holds the type of the widget.
 *     "Unknown" if the widget does not exist.</li>
 *     <li><tt>WidgetCollection::iterator widget;</tt> Points to the widget's data.
 *     If it was not found, points to <tt>_widgets.end()</tt>.</li>
 *     <li><tt>const bool widgetExists;</tt> \c TRUE if the widget exists, \c FALSE
 *     if not.</li>
 *     <li><tt>WidgetID containerID;</tt> The ID of the container which contains
 *     (or which would contain) the given widget. If there isn't one, \c NO_WIDGET
 *     is stored.</li>
 *     <li><tt>WidgetCollection::iterator container;</tt> Will point to the
 *     container's (identified by \c containerID) data.</li></ul>
 * @param n The ID of the widget to search for.
 */
#define START_WITH_WIDGET(n) tgui::String widgetType("Unknown"); \
	try { \
		auto widget = _findWidget(n); \
		const bool widgetExists = widget != _widgets.end(); \
		if (!widgetExists) ERROR("This widget does not exist!"); \
		widgetType = widget->ptr->getWidgetType(); \
		sfx::WidgetID containerID = widget->ptr->getParent() ? \
			_getWidgetID(widget->ptr->getParent()) : sfx::NO_WIDGET; \
		auto container = _findWidget(containerID);

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
		auto castWidget = widget->castPtr<type>(); \
		code \
	}

/**
 * Execute code on \c widget if it is of a given type.
 * @sa \c IF_WIDGET_IS()
 */
#define ELSE_IF_WIDGET_IS(type, code) else if (widgetType == #type) { \
		auto castWidget = widget->castPtr<type>(); \
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
 * Defines a widget/signal type static variable which holds the string name of a
 * given widget/signal type.
 * @param t The case-sensitive name of the widget/signal type.
 */
#define STRING_CONSTANT(t) static std::string t = #t;

/**
 * Registers a widget type constant with a script interface and documents it.
 * @param e The engine to register the constant with.
 * @param d The documentation generator to send documentation to.
 * @param t The widget type.
 */
#define REGISTER_WIDGET_TYPE_NAME(e, d, t) \
	e->RegisterGlobalProperty("const string " #t "EngineName", &type::t); \
	d->DocumentExpectedFunction("const string " #t, "The name of the <tt>" #t \
		"</tt> widget type, should be given to functions such as " \
		"<tt>addWidget()</tt>.");

 /**
  * Registers a signal type constant with a script interface and documents it.
  * @param e The engine to register the constant with.
  * @param d The documentation generator to send documentation to.
  * @param t The signal type.
  */
#define REGISTER_SIGNAL_TYPE_NAME(e, d, t) \
	e->RegisterGlobalProperty("const string " #t, &signal::t); \
	d->DocumentExpectedFunction("const string " #t, "The name of the <tt>" #t \
		"</tt> signal type, should be given to functions such as " \
		"<tt>connectSignal()</tt>.");

/**
 * The data type used by the interface to accept widget IDs from the scripts.
 */
#define WIDGET_ID_PARAM "const WidgetID"
