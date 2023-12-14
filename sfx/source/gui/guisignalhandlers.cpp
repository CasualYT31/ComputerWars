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
	if (!_scripts) return false;
	const auto id = _getWidgetID(widget);
	// Placeholder widgets will not ever be connected here.
	const auto dataItr = _findWidget(id);
	// If the widget can no longer be found, it means at least two signals for it
	// fired off, and the first signal deleted the widget from storage before the
	// second one was handled. In such cases, just silently drop the signal.
	if (dataItr == _widgets.end()) return false;
	const auto& data = *dataItr;
	std::string signalNameStd = signalName.toStdString();
	bool calledAny = false, allSuccessful = true;
	// Invoke the single signal handler first.
	if (data.singleSignalHandlers.find(signalNameStd) !=
		data.singleSignalHandlers.end()) {
		calledAny = true;
		allSuccessful &= _scripts->callFunction(
			data.singleSignalHandlers.at(signalNameStd).operator->());
	}
	// Then invoke the multi signal handler.
	if (data.multiSignalHandler) {
		calledAny = true;
		allSuccessful &= _scripts->callFunction(
			data.multiSignalHandler->operator->(), id, &signalNameStd);
	}
	return calledAny && allSuccessful;
}

void sfx::gui::menuItemClickedSignalHandler(const sfx::WidgetID menuBarID,
	const sfx::MenuItemID index) {
	auto& data = *_findWidget(menuBarID);
	data.lastMenuItemClicked = index;
	signalHandler(data.ptr, signal::MenuItemClicked);
}

void sfx::gui::messageBoxButtonPressedSignalHandler(const sfx::WidgetID id,
	const tgui::String& caption) {
	auto& data = *_findWidget(id);
	const auto btns = data.castPtr<MessageBox>()->getButtons();
	data.lastMessageBoxButtonClicked = 0;
	for (const auto len = btns.size(); data.lastMessageBoxButtonClicked < len;
		++data.lastMessageBoxButtonClicked)
		if (btns[data.lastMessageBoxButtonClicked] == caption) break;
	signalHandler(data.ptr, signal::ButtonPressed);
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
	const sfx::WidgetIDRef id) {
	std::size_t x = MINIMISED_CHILD_WINDOW_PADDING;
	for (auto& window : _windows) {
		if (window == sfx::NO_WIDGET) {
			window = id;
			return std::to_string(x).append("px");
		}
		x += MINIMISED_CHILD_WINDOW_WIDTH + MINIMISED_CHILD_WINDOW_PADDING;
	}
	_windows.push_back(id);
	return std::to_string(x).append("px");
}

void sfx::gui::minimised_child_window_list::restore(const WidgetIDRef id) {
	for (auto& window : _windows) if (window == id) window = sfx::NO_WIDGET;
	while (_windows.size() > 0 && _windows.back() == sfx::NO_WIDGET)
		_windows.pop_back();
}

void sfx::gui::closingSignalHandler(const tgui::ChildWindow::Ptr& window,
	bool* abort) {
	const auto id = _getWidgetID(window);
	auto& data = *_findWidget(id);
	// Firstly, invoke the signal handler, if it exists. If it doesn't, always
	// "close" the window.
	bool close = true;
	if (data.childWindowClosingHandler) {
		_scripts->callFunction(data.childWindowClosingHandler->operator->(), id,
			&close);
	}
	if (close) {
		// If the window was minimised when it was closed, we need to restore it.
		if (data.childWindowData && data.childWindowData->isMinimised)
			_restoreChildWindowImpl(id, data);
		// Instead of removing the window from its parent, we make it go invisible
		// instead.
		window->setVisible(false);
	}
	// We always abort the closing process to prevent TGUI from removing the window
	// from its parent.
	*abort = true;
}

void sfx::gui::basicClosingSignalHandler(const tgui::Widget::Ptr& window,
	bool* abort) {
	const auto id = _getWidgetID(window);
	const auto& data = *_findWidget(id);
	if (data.childWindowClosingHandler) _scripts->callFunction(
		data.childWindowClosingHandler->operator->(), id, abort);
}

void sfx::gui::minimizedSignalHandler(const tgui::ChildWindow::Ptr& window) {
	const auto id = _getWidgetID(window);
	auto& data = *_findWidget(id);
	if (data.childWindowData) {
		if (!data.childWindowData->isMinimised) {
			if (!data.childWindowData->isMaximised)
				data.childWindowData->cache(window);
			data.childWindowData->isMinimised = true;
			data.childWindowData->isMaximised = false;
			tgui::String x("0");
			if (window->getParent()) x = _findWidget(
				_getWidgetID(window->getParent()))->
				minimisedChildWindowList.minimise(id);
			window->setSize(MINIMISED_CHILD_WINDOW_WIDTH, tgui::String(
				std::to_string(window->getRenderer()->getTitleBarHeight())));
			window->setPosition(x, "99%");
			window->setOrigin(0.0f, 1.0f);
			window->setResizable(false);
			window->setPositionLocked(true);
			window->moveToFront();
		}
	}
	signalHandler(window, signal::Minimized);
}

void sfx::gui::maximizedSignalHandler(const tgui::ChildWindow::Ptr& window) {
	const auto id = _getWidgetID(window);
	auto& data = *_findWidget(id);
	if (data.childWindowData) {
		if (data.childWindowData->isMinimised ||
			data.childWindowData->isMaximised) {
			_restoreChildWindowImpl(id, data);
		} else {
			data.childWindowData->cache(window);
			data.childWindowData->isMinimised = false;
			data.childWindowData->isMaximised = true;
			window->setSize("100%", "100%");
			window->setPosition("50%", "50%");
			window->setOrigin(0.5f, 0.5f);
			window->setResizable(false);
			window->setPositionLocked(true);
		}
		window->moveToFront();
	}
	signalHandler(window, signal::Maximized);
}

void sfx::gui::textBoxFocusedSignalHandler(const tgui::Widget::Ptr& widget) {
	_editBoxOrTextAreaHasSetFocus = true;
	if (widget->getWidgetType() == type::TextArea)
		_gui.setTabKeyUsageEnabled(false);
	signalHandler(widget, signal::Focused);
}

void sfx::gui::textBoxUnfocusedSignalHandler(const tgui::Widget::Ptr& widget) {
	_editBoxOrTextAreaHasSetFocus = false;
	if (widget->getWidgetType() == type::TextArea)
		_gui.setTabKeyUsageEnabled(true);
	signalHandler(widget, signal::Unfocused);
}

void sfx::gui::_connectSignals(tgui::Widget::Ptr widget) {
	const tgui::String type = widget->getWidgetType();
	const auto id = _getWidgetID(widget);
	for (const auto& signal : SIGNALS) {
		// If this widget doesn't support this signal, don't try to connect it.
		if (!signal.second.count(type.toStdString()) && !signal.second.empty())
			continue;
		// Connect special signal handlers...
		if (type == type::ChildWindow || type == type::ColorPicker) {
			auto childWindow = std::dynamic_pointer_cast<ChildWindow>(widget);
			if (signal.first == signal::Minimized) {
				childWindow->onMinimize(&sfx::gui::minimizedSignalHandler, this,
					childWindow);
				continue;
			} else if (signal.first == signal::Maximized) {
				childWindow->onMaximize(&sfx::gui::maximizedSignalHandler, this,
					childWindow);
				continue;
			} else if (signal.first == signal::Closing) {
				childWindow->onClosing(&sfx::gui::closingSignalHandler, this,
					childWindow);
				continue;
			}
		} else if (type == type::FileDialog) {
			auto fd = std::dynamic_pointer_cast<FileDialog>(widget);
			if (signal.first == signal::Closing) {
				fd->onClosing(&sfx::gui::basicClosingSignalHandler, this, fd);
				continue;
			}
		} else if (type == type::MessageBox) {
			auto mb = std::dynamic_pointer_cast<MessageBox>(widget);
			if (signal.first == signal::ButtonPressed) {
				mb->onButtonPress(&sfx::gui::messageBoxButtonPressedSignalHandler,
					this, id);
				continue;
			} else if (signal.first == signal::Closing) {
				mb->onClosing(&sfx::gui::basicClosingSignalHandler, this, mb);
				continue;
			}
		} else if (type == type::ColorPicker) {
			auto c = std::dynamic_pointer_cast<ColorPicker>(widget);
			if (signal.first == signal::Closing) {
				c->onClosing(&sfx::gui::basicClosingSignalHandler, this, c);
				continue;
			}
		} else if (type == type::EditBox || type == type::TextArea) {
			if (signal.first == signal::Focused) {
				widget->getSignal(signal.first).connect(
					&sfx::gui::textBoxFocusedSignalHandler, this, widget);
				continue;
			} else if (signal.first == signal::Unfocused) {
				widget->getSignal(signal.first).connect(
					&sfx::gui::textBoxUnfocusedSignalHandler, this, widget);
				continue;
			}
		} else if (type == type::MenuBar) {
			// Each item is connected individually, when created and when
			// translated.
			if (signal.first == signal::MenuItemClicked) continue;
		}
		// ... Otherwise, just connect the base signal handler.
		// Would connecting Closing here work for MessageBox, etc.?
		widget->getSignal(signal.first).connectEx(&sfx::gui::signalHandler, this);
	}
}
