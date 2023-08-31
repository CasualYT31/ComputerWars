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
#include "fmtsfx.hpp"

using namespace tgui;

bool sfx::gui::signalHandler(tgui::Widget::Ptr widget,
	const tgui::String& signalName) {
	if (_scripts && getGUI() != "") {
		std::string fullname = widget->getWidgetName().toStdString();
		std::string signalNameStd = signalName.toStdString();
		// Call additional signal handler before the main one.
		if (_additionalSignalHandlers.find(fullname) !=
			_additionalSignalHandlers.end()) {
			_scripts->callFunction(
				_additionalSignalHandlers[fullname].operator->(), &fullname,
				&signalNameStd);
		}
		// Call basic or extended signal handler.
		auto customHandler = _customSignalHandlers.find(fullname);
		if (customHandler != _customSignalHandlers.end()) {
			std::string decl = "void " + customHandler->second +
				"(const string&in, const string&in)";
			if (_scripts->functionDeclExists(_scripts->MAIN_MODULE, decl)) {
				return _scripts->callFunction(_scripts->MAIN_MODULE,
					customHandler->second, &fullname, &signalNameStd);
			} else {
				_logger.warning("Widget \"{}\" was configured with a custom "
					"signal handler \"{}\", but a function of declaration \"{}\" "
					"does not exist. Falling back on the default signal handler.",
					fullname, customHandler->second, decl);
			}
		}
		std::string functionName = getGUI() + "_" + _extractWidgetName(fullname) +
			"_" + signalNameStd;
		if (_scripts->functionExists(_scripts->MAIN_MODULE, functionName)) {
			return _scripts->callFunction(_scripts->MAIN_MODULE, functionName);
		}
	}
	return false;
}

void sfx::gui::menuItemClickedSignalHandler(const std::string& menuBarName,
	const sfx::gui::MenuItemID index) {
	const auto funcName = getGUI() + "_" + menuBarName +
		"_MenuItemClicked";
	const auto funcDecl = "void " + funcName + "(const MenuItemID)";
	if (_scripts->functionDeclExists(_scripts->MAIN_MODULE, funcDecl)) {
		_scripts->callFunction(_scripts->MAIN_MODULE, funcName, index);
	}
}

void sfx::gui::messageBoxButtonPressedSignalHandler(
	const std::string& messageBoxName, const tgui::MessageBox::Ptr& widget,
	const tgui::String& caption) {
	const auto funcName = getGUI() + "_" + messageBoxName + "_ButtonPressed";
	const auto funcDecl = "void " + funcName + "(const uint64)";
	const auto btns = widget->getButtons();
	std::size_t index = 0;
	for (const auto len = btns.size(); index < len; ++index)
		if (btns[index] == caption) break;
	if (_scripts->functionDeclExists(_scripts->MAIN_MODULE, funcDecl)) {
		_scripts->callFunction(_scripts->MAIN_MODULE, funcName, index);
	} else {
		_logger.critical("A message box button \"{}\" was pressed, but no signal "
			"handler for the MessageBox \"{}\" was defined! The signal handler "
			"must have the following declaration: \"{}\".", caption,
			messageBoxName, funcDecl);
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

tgui::String sfx::gui::minimised_child_window_list::minimise(
	const std::string& name) {
	std::size_t x = MINIMISED_CHILD_WINDOW_PADDING;
	for (auto& window : _windows) {
		if (window.empty()) {
			window = name;
			return std::to_string(x).append("px");
		}
		x += MINIMISED_CHILD_WINDOW_WIDTH + MINIMISED_CHILD_WINDOW_PADDING;
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
	if (_scripts->functionDeclExists(_scripts->MAIN_MODULE, funcDecl)) {
		_scripts->callFunction(_scripts->MAIN_MODULE, funcName, &close);
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

void sfx::gui::fileDialogClosingSignalHandler(const tgui::FileDialog::Ptr& window,
	bool* abort) {
	const auto widgetName = window->getWidgetName().toStdString();
	const auto funcName = getGUI() + "_" + _extractWidgetName(widgetName) +
		"_Closing";
	const auto funcDecl = "void " + funcName + "(bool&out)";
	if (_scripts->functionDeclExists(_scripts->MAIN_MODULE, funcDecl))
		_scripts->callFunction(_scripts->MAIN_MODULE, funcName, abort);
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
				window->setSize(MINIMISED_CHILD_WINDOW_WIDTH, tgui::String(
					std::to_string(window->getRenderer()->getTitleBarHeight())));
				window->setPosition(x, "99%");
				window->setOrigin(0.0f, 1.0f);
				window->setResizable(false);
				window->setPositionLocked(true);
				window->moveToFront();
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

void sfx::gui::textBoxFocusedSignalHandler(const tgui::Widget::Ptr& widget) {
	_editBoxOrTextAreaHasSetFocus = true;
	signalHandler(widget, "Focused");
}

void sfx::gui::textBoxUnfocusedSignalHandler(const tgui::Widget::Ptr& widget) {
	_editBoxOrTextAreaHasSetFocus = false;
	signalHandler(widget, "Unfocused");
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
	tgui::String type = widget->getWidgetType();
	if (type == type::Button || type == type::EditBox || type == type::Label ||
		type == type::Picture || type == type::ProgressBar ||
		type == type::RadioButton || type == type::SpinButton ||
		type == type::Panel || type == type::BitmapButton ||
		type == type::CheckBox) {
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
	if (type == type::Button || type == type::BitmapButton) {
		widget->getSignal("Pressed").connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::ChildWindow) {
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
	} else if (type == type::ColourPicker) {
		widget->getSignal("ColorChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("OkPress").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::ComboBox) {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::EditBox) {
		widget->getSignal("TextChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("ReturnKeyPressed").
			connectEx(&sfx::gui::signalHandler, this);
		// Replace focus and unfocus signal handlers.
		widget->getSignal("Focused").connect(
			&sfx::gui::textBoxFocusedSignalHandler, this, widget);
		widget->getSignal("Unfocused").connect(
			&sfx::gui::textBoxUnfocusedSignalHandler, this, widget);
	} else if (type == type::FileDialog) {
		widget->getSignal("FileSelected").
			connectEx(&sfx::gui::signalHandler, this);
		const auto fd = std::dynamic_pointer_cast<FileDialog>(widget);
		// FileDialogs must be cleaned up correctly when closed!
		fd->onClose(&sfx::gui::_removeWidget, this,
			widget->getWidgetName().toStdString());
		// Allow the scripts to handle FileDialog closing.
		fd->onClosing(&sfx::gui::fileDialogClosingSignalHandler, this, fd);
	} else if (type == type::Knob || type == type::Scrollbar ||
		type == type::Slider || type == type::SpinButton ||
		type == type::SpinControl) {
		widget->getSignal("ValueChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::Label || type == type::Picture) {
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::ListBox) {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("MousePressed").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("MouseReleased").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::ListView) {
		widget->getSignal("ItemSelected").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("HeaderClicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("RightClicked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::MenuBar) {
		// Each item is connected individually, when created and when translated.
		/*widget->getSignal("MenuItemClicked").
			connectEx(&sfx::gui::signalHandler, this);*/
	} else if (type == type::MessageBox) {
		const auto mb = std::dynamic_pointer_cast<MessageBox>(widget);
		mb->onButtonPress(&sfx::gui::messageBoxButtonPressedSignalHandler, this,
			_extractWidgetName(mb->getWidgetName().toStdString()), mb);
		// MessageBoxes must be cleaned up correctly when closed!
		mb->onClose(&sfx::gui::_removeWidget, this,
			widget->getWidgetName().toStdString());
	} else if (type == type::Panel) {
		widget->getSignal("DoubleClicked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::ProgressBar) {
		widget->getSignal("ValueChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Full").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::RadioButton || type == type::CheckBox) {
		widget->getSignal("Checked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Unchecked").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("Changed").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::RangeSlider) {
		widget->getSignal("RangeChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::TabContainer) {
		widget->getSignal("SelectionChanging").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("SelectionChanged").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::Tabs) {
		widget->getSignal("TabSelected").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::TextArea) {
		widget->getSignal("TextChanged").
			connectEx(&sfx::gui::signalHandler, this);
		widget->getSignal("SelectionChanged").
			connectEx(&sfx::gui::signalHandler, this);
		// Replace focus and unfocus signal handlers.
		widget->getSignal("Focused").connect(
			&sfx::gui::textBoxFocusedSignalHandler, this, widget);
		widget->getSignal("Unfocused").connect(
			&sfx::gui::textBoxUnfocusedSignalHandler, this, widget);
	} else if (type == type::ToggleButton) {
		widget->getSignal("Checked").
			connectEx(&sfx::gui::signalHandler, this);
	} else if (type == type::TreeView) {
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
