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
#include "guiconstants.hpp"

#undef MessageBox

using namespace tgui;

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
			if (_scripts->functionDeclExists(_scripts->MAIN_MODULE,
				closeFuncDecl)) {
				_scripts->callFunction(_scripts->MAIN_MODULE, closeFuncName,
					&newMenu);
			} else if (_scripts->functionDeclExists(_scripts->MAIN_MODULE,
				closeFuncEmptyDecl)) {
				_scripts->callFunction(_scripts->MAIN_MODULE, closeFuncName);
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
			if (_scripts->functionDeclExists(_scripts->MAIN_MODULE,
				openFuncEmptyDecl)) {
				_scripts->callFunction(_scripts->MAIN_MODULE, openFuncName);
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
		_logger.write("Opened menu {} from menu {}.", _currentGUI, _previousGUI);
	} catch (const tgui::Exception& e) {
		_logger.error("{}", e);
		if (_gui.get(old)) _gui.get(old)->setVisible(true);
	}
}

void sfx::gui::addSpritesheet(const std::string& name,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	if (_sheet.find(name) != _sheet.end())
		_logger.warning("Updated the spritesheet named \"{}\"!", name);
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
		// TODO-6
		// If the given widget has no parent, it might be a container within a
		// SubwidgetContainer. Try to find it using `widget`'s name. If it still
		// can't be found, then assume the widget is visible and enabled.
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
	static const std::function<ScrollablePanel*(Widget*, const unsigned int)>
		findScrollablePanelAncestor =
		[](Widget* w, const unsigned int depth) -> ScrollablePanel* {
		auto const wCopy = w;
		if (w = reinterpret_cast<Widget*>(w->getParent())) {
			if (w->getWidgetType() == type::ScrollablePanel) {
				if (depth == 0)
					return dynamic_cast<ScrollablePanel*>(w);
				else
					return findScrollablePanelAncestor(w, depth - 1);
			}
			return findScrollablePanelAncestor(w, depth);
		} else {
			// TODO-6
			// If the given widget has no parent, it might be a container within a
			// SubwidgetContainer. Try to find it using `widget`'s name. If it
			// still can't be found, then assume no ScrollablePanel can be found.
			return nullptr;
		}
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
		panel->getAbsolutePosition(panel->getContentOffset()).x,
		panel->getAbsolutePosition(panel->getContentOffset()).y,
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
	_ui = ui;
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
					if (widgetType == type::Button ||
						widgetType == type::BitmapButton ||
						widgetType == type::ListBox) {
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
				"(const dictionary, const dictionary, const MousePosition&in, "
					"const MousePosition&in)";
			if (_scripts->functionExists(_scripts->MAIN_MODULE, funcName)) {
				// Construct the dictionaries.
				CScriptDictionary *controls = _scripts->createDictionary(),
					*triggeredByMouse = _scripts->createDictionary();
				auto controlKeys = ui->getControls();
				for (auto& key : controlKeys) {
					controls->Set(key, (asINT64)ui->operator[](key));
					triggeredByMouse->Set(key,
						(asINT64)ui->isMouseButtonTriggeringControl(key));
				}
				// Invoke the function.
				if (_scripts->functionDeclExists(_scripts->MAIN_MODULE,
					basicHandleInputDecl)) {
					_scripts->callFunction(_scripts->MAIN_MODULE, funcName,
						controls);
				} else if (_scripts->functionDeclExists(_scripts->MAIN_MODULE,
					extendedHandleInputDecl)) {
					_scripts->callFunction(_scripts->MAIN_MODULE, funcName,
						controls, triggeredByMouse, &_previousMousePosition,
						&_currentMousePosition);
				}
				controls->Release();
				triggeredByMouse->Release();
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
	_scalingFactor = factor;
}

float sfx::gui::getScalingFactor() const noexcept {
	return _scalingFactor;
}

std::string sfx::gui::getWidgetUnderMouse() const {
	if (_ui) {
		if (const auto w = _gui.getWidgetBelowMouseCursor(_ui->mousePosition()))
			return w->getWidgetName().toStdString();
	} else {
		_logger.error("Called getWidgetUnderMouse() when no user input object has "
			"been given to this gui object!");
	}
	return "";
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
		if (cursel.second->getWidgetType() == type::ScrollablePanel) {
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
		if (type == type::BitmapButton) {
			std::dynamic_pointer_cast<BitmapButton>(widget)->
				setImage(blank);
		} else if (type == type::Picture) {
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
		if (type == type::BitmapButton || type == type::Picture) {
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
				if (type == type::BitmapButton) {
					newPosition = std::dynamic_pointer_cast<BitmapButton>(widget)->
						getAbsolutePositionOfImage();
				} else if (type == type::Picture) {
					newPosition = std::dynamic_pointer_cast<tgui::Picture>(widget)
						->getAbsolutePosition();
				}
				animatedSprite.setPosition(newPosition);
			} else if (_guiSpriteKeys.find(widgetName) != _guiSpriteKeys.end() &&
				_widgetSprites.find(widget) != _widgetSprites.end() &&
				_widgetSprites.at(widget).getSpritesheet() != nullptr &&
				_sheet.find(_guiSpriteKeys.at(widgetName).first) == _sheet.end()) {
				// Else if the widget DID have a valid spritesheet, then we're
				// going to have to remove the image from the widget to ensure that
				// sizing works out.
				allocImage(type, widget, widgetName, 0, 0);
			}
		}

		if (widget->isContainer()) {
			_animate(target, std::dynamic_pointer_cast<Container>(widget));
		} else if (auto subwidgetContainer = _getSubwidgetContainer(widget)) {
			_animate(target, subwidgetContainer);
		}
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

void sfx::gui::_drawCallback(BackendRenderTarget& target,
	const tgui::RenderStates& states, tgui::Widget::ConstPtr widget) const {
	const auto widgetSprite = _widgetSprites.find(widget);
	if (widgetSprite != _widgetSprites.end()) {
		// tgui::RenderStates only has a transform, which is already (partially?)
		// applied to the sprite itself, so they're not useful for us right now.
		// See tgui::Transform::combine() for an example of how two 4x4 matrices
		// can be combined to convert a TGUI Transform to an SFML transform.
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
		} else if (widgetType == type::ListBox) {
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
		} else if (widgetType == type::ScrollablePanel) {
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
		} else if (widgetType == type::ListBox) {
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
		} else if (widgetType == type::ScrollablePanel) {
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
		} else if (widgetType == type::ScrollablePanel) {
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
		} else if (widgetType == type::ScrollablePanel) {
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
		if (type == type::Button) { // SingleCaption
			auto w = _findWidget<Button>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::BitmapButton) { // SingleCaption
			auto w = _findWidget<BitmapButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::CheckBox) { // SingleCaption
			auto w = _findWidget<CheckBox>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::ChildWindow) { // SingleCaption
			auto w = _findWidget<ChildWindow>(widgetName);
			w->setTitle(_getTranslatedText(widgetName));
		} else if (type == type::ColourPicker) { // SingleCaption
			auto w = _findWidget<ColorPicker>(widgetName);
			w->setTitle(_getTranslatedText(widgetName));
		} else if (type == type::ComboBox) { // ListOfCaptions
			auto w = _findWidget<ComboBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
		} else if (type == type::EditBox) { // SingleCaption
			auto w = _findWidget<EditBox>(widgetName);
			w->setDefaultText(_getTranslatedText(widgetName));
		} else if (type == type::FileDialog) { // ListOfCaptions
			auto w = _findWidget<FileDialog>(widgetName);
			const auto n = std::get<sfx::gui::ListOfCaptions>(
				_originalCaptions.at(widgetName)).size();
			if (n >= 1) w->setTitle(_getTranslatedText(widgetName, 0));
			if (n >= 2) w->setConfirmButtonText(_getTranslatedText(widgetName, 1));
			if (n >= 3) w->setCancelButtonText(_getTranslatedText(widgetName, 2));
			if (n >= 4)
				w->setCreateFolderButtonText(_getTranslatedText(widgetName, 3));
			if (n >= 5) w->setFilenameLabelText(_getTranslatedText(widgetName, 4));
			if (n >= 8) {
				w->setListViewColumnCaptions(
					_getTranslatedText(widgetName, 5),
					_getTranslatedText(widgetName, 6),
					_getTranslatedText(widgetName, 7)
				);
			}
			std::vector<std::pair<String, std::vector<String>>> filters =
				w->getFileTypeFilters();
			for (std::size_t i = 0, len = filters.size(); i < len; ++i)
				filters[i].first = _getTranslatedText(widgetName, i + 8);
			w->setFileTypeFilters(filters);
		} else if (type == type::Label) { // SingleCaption
			auto w = _findWidget<Label>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::ListBox) { // ListOfCaptions
			auto w = _findWidget<ListBox>(widgetName);
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(widgetName, i));
		} else if (type == type::ListView) { // ListOfCaptions
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
		} else if (type == type::MenuBar) { // ListOfCaptions
			auto w = std::dynamic_pointer_cast<MenuBar>(widget);
			std::vector<String> hierarchy;
			std::size_t index = 0;
			_translateMenuItems(w, widgetName, w->getMenus(), hierarchy, index);
		} else if (type == type::MessageBox) { // ListOfCaptions
			auto w = _findWidget<tgui::MessageBox>(widgetName);
			w->setTitle(_getTranslatedText(widgetName, 0));
			w->setText(_getTranslatedText(widgetName, 1));
			const auto& buttonCaptions = std::get<sfx::gui::ListOfCaptions>(
				_originalCaptions[widgetName]);
			std::vector<String> newCaptions;
			for (std::size_t i = 2, len = buttonCaptions.size(); i < len; ++i)
				newCaptions.emplace_back(_getTranslatedText(widgetName, i));
			w->renameButtons(newCaptions);
		} else if (type == type::ProgressBar) { // SingleCaption
			auto w = _findWidget<ProgressBar>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::RadioButton) { // SingleCaption
			auto w = _findWidget<RadioButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		} else if (type == type::TabContainer) { // ListOfCaptions
			auto w = _findWidget<TabContainer>(widgetName);
			for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); ++i)
				w->changeTabText(i, _getTranslatedText(widgetName, i));
		} else if (type == type::Tabs) { // ListOfCaptions
			auto w = _findWidget<Tabs>(widgetName);
			for (std::size_t i = 0; i < w->getTabsCount(); ++i)
				w->changeText(i, _getTranslatedText(widgetName, i));
		} else if (type == type::TextArea) { // SingleCaption
			auto w = std::dynamic_pointer_cast<TextArea>(widget);
			w->setDefaultText(_getTranslatedText(widgetName));
		} else if (type == type::ToggleButton) { // SingleCaption
			auto w = _findWidget<ToggleButton>(widgetName);
			w->setText(_getTranslatedText(widgetName));
		}
	}
	if (widget->isContainer()) {
		const auto& widgetList =
			std::dynamic_pointer_cast<Container>(widget)->getWidgets();
		for (auto& w : widgetList) _translateWidget(w);
	} else if (auto subwidgetContainer = _getSubwidgetContainer(widget)) {
		const auto& widgetList = subwidgetContainer->getWidgets();
		for (auto& w : widgetList) _translateWidget(w);
	}
}

void sfx::gui::_translateMenuItems(const tgui::MenuBar::Ptr& w,
	const std::string& widgetName,
	const std::vector<MenuBar::GetMenusElement>& items,
	std::vector<String>& hierarchy, std::size_t& index) {
	// Each and every menu item is stored in _originalCaptions depth-first.
	// See the documentation on menuItemClickedSignalHandler() for more
	// info.
	for (const auto& item : items) {
		hierarchy.push_back(item.text);
		const tgui::String translatedCaption =
			_getTranslatedText(widgetName, index);
		w->changeMenuItem(hierarchy, translatedCaption);
		hierarchy.back() = translatedCaption;
		// I know, it's really ugly. Not much choice.
		w->connectMenuItem(hierarchy, &sfx::gui::menuItemClickedSignalHandler,
			this, _extractWidgetName(widgetName),
			static_cast<sfx::gui::MenuItemID>(index++));
		_translateMenuItems(w, widgetName, item.menuItems, hierarchy, index);
		hierarchy.pop_back();
	}
}

bool sfx::gui::_load(engine::json& j) {
	_isLoading = true;
	try {
		std::vector<std::string> names;
		j.applyVector(names, { "menus" });
		if (j.inGoodState()) {
			_clearState();
			// Create the main menu that always exists.
			tgui::Group::Ptr menu = tgui::Group::create();
			menu->setVisible(false);
			_gui.add(menu, "MainMenu");
			setGUI("MainMenu", false, false);
			if (_scripts)
				_scripts->callFunction(_scripts->MAIN_MODULE, "MainMenuSetUp");
			// Create each menu.
			for (const auto& m : names) {
				menu = tgui::Group::create();
				menu->setVisible(false);
				_gui.add(menu, m);
				// Temporarily set the current GUI to this one to make
				// _findWidget() work with relative widget names in SetUp()
				// functions.
				setGUI(m, false, false);
				if (_scripts)
					_scripts->callFunction(_scripts->MAIN_MODULE, m + "SetUp");
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

void sfx::gui::_removeWidgets(const tgui::Widget::Ptr& widget,
	const tgui::Container::Ptr& container, const bool removeIt) {
	Container::Ptr widgetIsContainer;
	if (widget->isContainer()) {
		widgetIsContainer = std::dynamic_pointer_cast<Container>(widget);
	} else if (auto subwidgetContainer = _getSubwidgetContainer(widget)) {
		// We should be able to safely remove a SubwidgetContainer's widgets
		// ourselves. Might not be the best idea to leave the base
		// SubwidgetContainer active if we do, though, so this should be blocked
		// further up the call chain.
		widgetIsContainer = subwidgetContainer;
	}
	if (widgetIsContainer) {
		auto& widgetsInContainer = widgetIsContainer->getWidgets();
		for (auto& widgetInContainer : widgetsInContainer) {
			// Remove each child widget's internal data entries only.
			_removeWidgets(widgetInContainer, widgetIsContainer, false);
		}
		// Now remove each child widget fr.
		widgetIsContainer->removeAllWidgets();
		if (!removeIt) return;
	}
	// Remove widget.
	if (container) {
		_eraseWidgetState(widget, removeIt);
	} else {
		_logger.error("Attempted to remove a widget \"{}\", which did not have a "
			"container!", widget->getWidgetName());
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
	if (fullname.rfind('.') == String::npos) return fullname;
	else return fullname.substr(fullname.rfind('.') + 1);
}

Container::Ptr sfx::gui::_getSubwidgetContainer(
	const tgui::Widget::Ptr& widget) {
	if (widget->getWidgetType() == type::TabContainer ||
		widget->getWidgetType() == type::SpinControl)
		return std::dynamic_pointer_cast<SubwidgetContainer>(
			widget)->getContainerSharedPtr();
	else return nullptr;
}

Widget::Ptr sfx::gui::_createWidget(const std::string& wType,
	const std::string& name, const std::string& menu) const {
	tgui::String type = tgui::String(wType).trim();
	if (type == type::BitmapButton) {
		auto button = tgui::BitmapButton::create();
		button->setCallback(std::bind(&sfx::gui::_drawCallback, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		return button;
	} else if (type == type::ListBox) {
		return tgui::ListBox::create();
	} else if (type == type::VerticalLayout) {
		return tgui::VerticalLayout::create();
	} else if (type == type::HorizontalLayout) {
		return tgui::HorizontalLayout::create();
	} else if (type == type::Picture) {
		const auto picture = tgui::Picture::create();
		picture->setDrawCallback(std::bind(&sfx::gui::_drawCallback, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		return picture;
	} else if (type == type::Label) {
		return tgui::Label::create();
	} else if (type == type::ScrollablePanel) {
		auto panel = tgui::ScrollablePanel::create();
		// Always set default scrollbar amounts to allow directional flow to work.
		panel->setHorizontalScrollAmount(5);
		panel->setVerticalScrollAmount(5);
		return panel;
	} else if (type == type::Panel) {
		return tgui::Panel::create();
	} else if (type == type::Group) {
		return tgui::Group::create();
	} else if (type == type::Grid) {
		return tgui::Grid::create();
	} else if (type == type::Button) {
		return tgui::Button::create();
	} else if (type == type::EditBox) {
		return tgui::EditBox::create();
	} else if (type == type::MenuBar) {
		return tgui::MenuBar::create();
	} else if (type == type::ChildWindow) {
		return tgui::ChildWindow::create();
	} else if (type == type::ComboBox) {
		return tgui::ComboBox::create();
	} else if (type == type::FileDialog) {
		auto filedialog = tgui::FileDialog::create();
		filedialog->setResizable();
		// Will need to find a way to apply the default font...
		return filedialog;
	} else if (type == type::MessageBox) {
		return tgui::MessageBox::create();
	} else if (type == type::HorizontalWrap) {
		return tgui::HorizontalWrap::create();
	} else if (type == type::Tabs) {
		return tgui::Tabs::create();
	} else if (type == type::TreeView) {
		return tgui::TreeView::create();
	} else if (type == type::CheckBox) {
		return tgui::CheckBox::create();
	} else if (type == type::RadioButton) {
		return tgui::RadioButton::create();
	} else if (type == type::TabContainer) {
		auto tabContainer = tgui::TabContainer::create();
		// Fix the name of the Tabs widget within the TabContainer so that it can
		// be accessed by the engine.
		const auto& widgetList = tabContainer->getContainer()->getWidgets();
		for (auto& w : widgetList)
			w->setWidgetName(name + "." + w->getWidgetName().replace(".", ""));
		return tabContainer;
	} else if (type == type::TextArea) {
		return tgui::TextArea::create();
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

void sfx::gui::_clearState() {
	_gui.removeAllWidgets();
	_guiBackground.clear();
	_widgetSprites.clear();
	_guiSpriteKeys.clear();
	_dontOverridePictureSizeWithSpriteSize.clear();
	_originalCaptions.clear();
	_customSignalHandlers.clear();
	_additionalSignalHandlers.clear();
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
}

void sfx::gui::_eraseWidgetState(const tgui::Widget::Ptr& widget,
	const bool removeIt) {
	const auto name = widget->getWidgetName().toStdString();
	_widgetSprites.erase(widget);
	_guiSpriteKeys.erase(name);
	_dontOverridePictureSizeWithSpriteSize.erase(name);
	_originalCaptions.erase(name);
	_customSignalHandlers.erase(name);
	_additionalSignalHandlers.erase(name);
	_directionalFlow.erase(name);
	_hierarchyOfLastMenuItem.erase(name);
	_menuCounter.erase(name);
	_childWindowData.erase(name);
	_minimisedChildWindowList.erase(name);
	// If a ChildWindow, remove it from its parent's ChildWindowList.
	_minimisedChildWindowList[widget->getParent()->getWidgetName().toStdString()].
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
	if (removeIt) widget->getParent()->remove(widget);
}
