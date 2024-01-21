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

#include "gui.hpp"
#include "fmtsfx.hpp"
#include "guiconstants.hpp"

#undef MessageBox

using namespace tgui;

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

void sfx::gui::setGUI(const std::string& newMenu, const bool callClose,
	const bool callOpen) {
	if (newMenu == getGUI()) return;
	const auto oldMenuItr = _menus.find(getGUI()),
		newMenuItr = _menus.find(newMenu);
	if (newMenuItr == _menus.end()) {
		_logger.error("GUI with name \"{}\" does not exist.", newMenu);
		return;
	}
	if (callClose && oldMenuItr != _menus.end()) {
		std::string nameOfMenuType(
			newMenuItr->second.object->GetObjectType()->GetName());
		if (!_scripts->callMethod(oldMenuItr->second.object,
			"void Close(Menu@ const, const string&in)", newMenuItr->second.object,
			&nameOfMenuType))
			_logger.error("Failed to call Close() method on current menu \"{}\"!",
				getGUI());
	}
	_widgetSprites.clear();
	_previousGUI = getGUI();
	_currentGUI = newMenu;
	if (callOpen) {
		asIScriptObject* const oldMenu =
			(oldMenuItr == _menus.end() ? nullptr : oldMenuItr->second.object);
		std::string nameOfMenuType;
		if (oldMenu) nameOfMenuType = oldMenu->GetObjectType()->GetName();
		if (!_scripts->callMethod(newMenuItr->second.object,
			"void Open(Menu@ const, const string&in)", oldMenu, &nameOfMenuType))
			_logger.error("Failed to call Open() method on new menu \"{}\"!",
				getGUI());
	}
	// If there is no widget currently selected, automatically select the first
	// widget.
	// Do this after invoking the Open function to allow that function to set
	// the first selected widget, if said widget is being added by the Open
	// function and is not available beforehand.
	if (_findCurrentlySelectedWidget().first == sfx::NO_WIDGET)
		_makeNewDirectionalSelection(newMenuItr->second.selectThisWidgetFirst,
			getGUI(), _upControl);
	_logger.write("Opened menu {} from menu {}.", _currentGUI, _previousGUI);
}

void sfx::gui::setSpritesheets(
	const std::shared_ptr<sfx::animated_spritesheets>& sheets) {
	_sheets = sheets;
}

void sfx::gui::setTarget(sf::RenderTarget& newTarget) {
	_gui.setTarget(newTarget);
}

bool sfx::gui::handleEvent(sf::Event e) {
	return _gui.handleEvent(e);
}

bool sfx::gui::_isWidgetFullyVisibleAndEnabled(tgui::Widget* widget,
	const bool visible, const bool enabled) const {
	assert((visible || enabled) && widget);
	if ((!visible || widget->isVisible()) && (!enabled || widget->isEnabled())) {
		if (widget = reinterpret_cast<Widget*>(widget->getParent()))
			return _isWidgetFullyVisibleAndEnabled(widget, visible, enabled);
		return true;
	}
	return false;
}

void sfx::gui::_showWidgetInScrollablePanel(const Widget::Ptr& widget,
	const unsigned int panelAncestryDepth) {
	static const std::function<ScrollablePanel*(Widget*, const unsigned int)>
		findScrollablePanelAncestor =
		[&](Widget* w, const unsigned int depth) -> ScrollablePanel* {
		if (w = reinterpret_cast<Widget*>(w->getParent())) {
			if (w->getWidgetType() == type::ScrollablePanel) {
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
		_showWidgetInScrollablePanel(widget, panelAncestryDepth + 1);
		return;
	}

	// Figure out portion of ScrollablePanel that is being shown.
	const auto scrollbarWidth = panel->getScrollbarWidth();
	const auto panelAbsolutePosition =
		panel->getAbsolutePosition(panel->getContentOffset());
	const sf::FloatRect panelRect(
		panelAbsolutePosition.x,
		panelAbsolutePosition.y,
		// Gotta exclude the scrollbars from the visible portion.
		panel->getSize().x - (vertShown ? scrollbarWidth : 0.0f),
		panel->getSize().y - (horiShown ? scrollbarWidth : 0.0f)
	);

	// Figure out bounding rectangle of given widget.
	const auto widgetAbsolutePosition = widget->getAbsolutePosition();
	const sf::FloatRect widgetRect(
		widgetAbsolutePosition.x,
		widgetAbsolutePosition.y,
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
	_showWidgetInScrollablePanel(widget, panelAncestryDepth + 1);
}

void sfx::gui::handleInput(const std::shared_ptr<sfx::user_input>& ui) {
	_ui = ui;
	if (getGUI().empty()) return;
	if (!ui && !_handleInputErrorLogged) {
		_logger.error("Called handleInput() with nullptr user_input object for "
			"menu \"{}\"!", getGUI());
		_handleInputErrorLogged = true;
		return;
	}
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
		// If select is issued, and there is currently a widget selected that isn't
		// disabled, then trigger an appropriate signal.
		if ((*ui)[_selectControl.control] && cursel != sfx::NO_WIDGET) {
			const auto widget = _findWidget(cursel);
			if (_isWidgetFullyVisibleAndEnabled(widget->ptr.get(), true, true)) {
				const auto& widgetType = widget->ptr->getWidgetType();
				if (widgetType == type::ButtonBase ||
					widgetType == type::Button ||
					widgetType == type::BitmapButton ||
					widgetType == type::ToggleButton) {
					selectedSignalHandler(widget->ptr, signal::Clicked);
					signalHandlerTriggered = true;
				} else if (widgetType == type::ListBox) {
					selectedSignalHandler(widget->ptr, signal::MouseReleased);
					signalHandlerTriggered = true;
				}
			}
		}
	} else if (_previousMousePosition == _currentMousePosition) {
		// Only re-enable directional flow if a directional input is made,
		// whilst the mouse isn't moving.
		_enableDirectionalFlow = (*ui)[_upControl.control] ||
			(*ui)[_downControl.control] || (*ui)[_leftControl.control] ||
			(*ui)[_rightControl.control];
		// If there wasn't a selection made previously, go straight to making
		// the selection.
		const auto cursel = _findCurrentlySelectedWidget();
		if (cursel.first == sfx::NO_WIDGET)
			_moveDirectionalFlow(ui);
		// Otherwise, make sure what was selected is now visible to the user.
		// Make sure to play a sound, too.
		else if (_enableDirectionalFlow && cursel.second) {
			_showWidgetInScrollablePanel(cursel.second);
			const auto w = _findWidget(cursel.first);
			if ((*ui)[_upControl.control])
				_play(w->moveUpSoundObject, w->moveUpSound);
			if ((*ui)[_downControl.control])
				_play(w->moveDownSoundObject, w->moveDownSound);
			if ((*ui)[_leftControl.control])
				_play(w->moveLeftSoundObject, w->moveLeftSound);
			if ((*ui)[_rightControl.control])
				_play(w->moveRightSoundObject, w->moveRightSound);
		}
	}
	// Invoke the current menu's bespoke input handling method.
	// If the signal handler was invoked, do not invoke any bespoke input
	// handler. If we do, it can cause multiple inputs that are typically
	// carried out separately to be processed in a single iteration.
	if (!signalHandlerTriggered) {
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
		if (!_scripts->callMethod(_menus.at(getGUI()).object, "void Periodic("
			"const dictionary, const dictionary, const MousePosition&in, "
			"const MousePosition&in)", controls, triggeredByMouse,
			&_previousMousePosition, &_currentMousePosition)) {
			_logger.error("Failed to call Periodic() method on current menu "
				"\"{}\"!", getGUI());
		}
		controls->Release();
		triggeredByMouse->Release();
	}
}

void sfx::gui::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& lang) {
	_lastlang.clear();
	_langdict = lang;
}

void sfx::gui::setFonts(const std::shared_ptr<sfx::fonts>& fonts) noexcept {
	_fonts = fonts;
}

void sfx::gui::setAudios(const std::shared_ptr<sfx::audios>& audios) noexcept {
	_audios = audios;
}

void sfx::gui::setScalingFactor(const float factor) {
	_gui.setRelativeView({0.0f, 0.0f, 1.0f / factor, 1.0f / factor});
	_scalingFactor = factor;
}

float sfx::gui::getScalingFactor() const noexcept {
	return _scalingFactor;
}

std::string sfx::gui::getSelectControl() const {
	return _selectControl.control;
}

bool sfx::gui::isWidgetUnderMouse() const {
	if (!_ui) {
		_logger.error("Called isWidgetUnderMouse() when no user input object has "
			"been given to this gui object!");
		return false;
	}
	return _gui.getWidgetBelowMouseCursor(_ui->mousePosition()).operator bool();
}

sfx::gui::widget_data::operator std::string() const {
	static const char* SEP = "\t\t\t";
	return fmt::format("Widget #{}{}type=\"{}\"{}name=\"{}\"{}enabled={}.",
		( ptr ? _getWidgetID(ptr) : sfx::NO_WIDGET ), SEP,
		( ptr ? ptr->getWidgetType() : "Unknown" ), SEP,
		( ptr ? ptr->getWidgetName() : "" ), SEP,
		( ptr ? ptr->isEnabled() : "N/A" ), SEP
	);
}

bool sfx::gui::animate(const sf::RenderTarget& target) {
	if (!getGUI().empty()) {
		if (_langdict && _langdict->getLanguage() != _lastlang) {
			_lastlang = _langdict->getLanguage();
			auto& widgetList = _gui.getWidgets();
			for (auto& widget : widgetList) _translateWidget(widget);
		}
		std::size_t animatedSprite = 0;
		_animate(target, _gui.getContainer());
	}

	// Whenever there isn't a widget currently selected via directional controls,
	// or the currently selected widget is not currently visible, always reset the
	// animation.
	const auto cursel = _findCurrentlySelectedWidget();
	if (cursel.first == sfx::NO_WIDGET || !_enableDirectionalFlow ||
		!cursel.second || !_isWidgetFullyVisibleAndEnabled(cursel.second.get(),
			true, false)) {
		_angleBracketUL.setCurrentFrame(0);
		_angleBracketUR.setCurrentFrame(0);
		_angleBracketLL.setCurrentFrame(0);
		_angleBracketLR.setCurrentFrame(0);
	} else if (cursel.first != sfx::NO_WIDGET) {
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
	static auto allocImage = [](const tgui::String& type, Widget::Ptr widget,
		unsigned int w, unsigned int h,
		const bool dontOverridePictureSizeWithSpriteSize) -> void {
		// We cannot allow an empty image to be allocated, so always ensure the
		// image is at least 1x1 pixels.
		if (!w) w = 1; if (!h) h = 1;

		// Create an empty texture.
		sf::Texture blank;
		sf::Uint8* pixels = (sf::Uint8*)::calloc(
			static_cast<std::size_t>(w) * static_cast<std::size_t>(h), 4);
		try {
			blank.create(w, h);
			blank.update(pixels);
		} catch (...) {
			// Make absolutely sure nothing leaks.
			if (pixels) ::free(pixels);
			throw;
		}
		::free(pixels);

		// Assign the empty texture.
		if (type == type::BitmapButton) {
			std::dynamic_pointer_cast<BitmapButton>(widget)->
				setImage(blank);
		} else if (type == type::Picture) {
			auto picture = std::dynamic_pointer_cast<tgui::Picture>(widget);
			picture->getRenderer()->setTexture(blank);
			if (!dontOverridePictureSizeWithSpriteSize) {
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

		const auto widgetID = _getWidgetID(widget);
		if (widgetID == sfx::NO_WIDGET) continue; // Placeholder widget, ignore.
		const auto& widgetData = *_findWidget(widgetID);
		const String type = widget->getWidgetType();

		// Only BitmapButtons and Pictures can have animated sprites.
		if (type == type::BitmapButton || type == type::Picture && (
			!widgetData.spritesheetKey.empty() && !widgetData.spriteKey.empty())) {
			// If the widget doesn't have a sprite, or if it doesn't have a valid
			// spritesheet, then don't animate the widget's sprite.
			if (_sheets->exists(widgetData.spritesheetKey)) {
				std::shared_ptr<sfx::animated_spritesheet> sheet =
					(*_sheets)[widgetData.spritesheetKey];
				const std::string& sprite = widgetData.spriteKey;

				if (_widgetSprites.find(widget) == _widgetSprites.end()) {
					// Animated sprite for this widget doesn't exist yet, so
					// allocate it.
					_widgetSprites.insert({ widget, sfx::animated_sprite() });
				}
				auto& animatedSprite = _widgetSprites.at(widget);

				if (sprite.empty() && !animatedSprite.getSprite().empty()) {
					// If the sprite has been removed, then we also need to remove
					// the image from the widget, see else if case at the bottom.
					allocImage(type, widget, 0, 0,
						widgetData.doNotOverridePictureSizeWithSpriteSize);
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
					sf::Vector2u spriteSize(
						static_cast<unsigned int>(spriteSizeF.x),
						static_cast<unsigned int>(spriteSizeF.y)
					);
					allocImage(type, widget, spriteSize.x, spriteSize.y,
						widgetData.doNotOverridePictureSizeWithSpriteSize);
				} else {
					// If the widget's sprite hasn't changed, then simply animate
					// it.
					animatedSprite.animate(target);
				}
			} else if (_widgetSprites.find(widget) != _widgetSprites.end() &&
				_widgetSprites.at(widget).getSpritesheet() != nullptr &&
				!_sheets->exists(widgetData.spritesheetKey)) {
				// Else if the widget DID have a valid spritesheet, then we're
				// going to have to remove the image from the widget to ensure that
				// sizing works out.
				allocImage(type, widget, 0, 0,
					widgetData.doNotOverridePictureSizeWithSpriteSize);
			}
		}

		if (widget->isContainer())
			_animate(target, std::dynamic_pointer_cast<Container>(widget));
	}
}

void sfx::gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_gui.draw();
	// Draw angle brackets, if there is currently a widget selected via the
	// directional controls, and it is visible.
	if (getGUI().empty()) return;
	const auto cursel = _menus.at(getGUI()).currentlySelectedWidget;
	if (_enableDirectionalFlow && cursel != sfx::NO_WIDGET &&
		_isWidgetFullyVisibleAndEnabled(_findWidget(cursel)->ptr.get(), true,
			false)) {
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
		// tgui::Transform::combine(). Bruno, you're a legend.
		sf::Transform original;
		const auto a = original.getMatrix();
		const auto& b = states.transform.getMatrix();
		original = sf::Transform(
			a[0] *  b[0] + a[4] *  b[1] + a[12] *  b[3],
			a[0] *  b[4] + a[4] *  b[5] + a[12] *  b[7],
			a[0] * b[12] + a[4] * b[13] + a[12] * b[15],
			a[1] *  b[0] + a[5] *  b[1] + a[13] *  b[3],
			a[1] *  b[4] + a[5] *  b[5] + a[13] *  b[7],
			a[1] * b[12] + a[5] * b[13] + a[13] * b[15],
			a[3] *  b[0] + a[7] *  b[1] + a[15] *  b[3],
			a[3] *  b[4] + a[7] *  b[5] + a[15] *  b[7],
			a[3] * b[12] + a[7] * b[13] + a[15] * b[15]
		);
		dynamic_cast<BackendRenderTargetSFML&>(target).getTarget()->
			draw(widgetSprite->second, original);
	}
}

sfx::WidgetID sfx::gui::_moveDirectionalFlow(
	const std::shared_ptr<sfx::user_input>& ui) {
	const auto cursel = _findCurrentlySelectedWidget();
	const auto& widgetType = !cursel.second ? "" : cursel.second->getWidgetType();
	const auto widgetData = _findWidget(cursel.first);
	if ((*ui)[_upControl.control]) {
		if (cursel.first == sfx::NO_WIDGET) {
			_makeNewDirectionalSelection(_menus.at(getGUI()).selectThisWidgetFirst,
				getGUI(), _upControl);
		} else if (widgetType == type::ListBox) {
			const auto listbox = std::dynamic_pointer_cast<ListBox>(cursel.second);
			const auto i = listbox->getSelectedItemIndex();
			if (i == -1) {
				if (listbox->setSelectedItemByIndex(0))
					_play(widgetData->moveUpSoundObject, widgetData->moveUpSound);
			} else if (i > 0) {
				if (listbox->setSelectedItemByIndex(
					static_cast<std::size_t>(i) - 1))
					_play(widgetData->moveUpSoundObject, widgetData->moveUpSound);
			} else if (widgetData->directionalFlow.up == sfx::NO_WIDGET) {
				// If there is nowhere to go from the top, loop through to the
				// the bottom of the list.
				if (listbox->setSelectedItemByIndex(listbox->getItemCount() - 1))
					_play(widgetData->moveUpSoundObject, widgetData->moveUpSound);
			} else {
				_makeNewDirectionalSelection(widgetData->directionalFlow.up,
					getGUI(), _upControl);
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
				_makeNewDirectionalSelection(widgetData->directionalFlow.up,
					getGUI(), _upControl);
			}
		} else {
			_makeNewDirectionalSelection(widgetData->directionalFlow.up, getGUI(),
				_upControl);
		}
	}
	if ((*ui)[_downControl.control]) {
		if (cursel.first == sfx::NO_WIDGET) {
			_makeNewDirectionalSelection(_menus.at(getGUI()).selectThisWidgetFirst,
				getGUI(), _downControl);
		} else if (widgetType == type::ListBox) {
			const auto listbox = std::dynamic_pointer_cast<ListBox>(cursel.second);
			const auto i = listbox->getSelectedItemIndex();
			if (i == -1) {
				if (listbox->setSelectedItemByIndex(0))
					_play(widgetData->moveDownSoundObject,
						widgetData->moveDownSound);
			} else if (i < listbox->getItemCount() - 1) {
				if (listbox->setSelectedItemByIndex(
					static_cast<std::size_t>(i) + 1))
					_play(widgetData->moveDownSoundObject,
						widgetData->moveDownSound);
			} else if (widgetData->directionalFlow.down == sfx::NO_WIDGET) {
				// If there is nowhere to go from the bottom, loop through to the
				// the top of the list.
				if (listbox->setSelectedItemByIndex(0))
					_play(widgetData->moveDownSoundObject,
						widgetData->moveDownSound);
			} else {
				_makeNewDirectionalSelection(widgetData->directionalFlow.down,
					getGUI(), _downControl);
			}
		} else if (widgetType == type::ScrollablePanel) {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getVerticalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getVerticalScrollAmount() > 0 &&
				panel->isVerticalScrollbarShown() &&
				value < static_cast<unsigned int>(panel->getContentSize().y) -
					(static_cast<unsigned int>(panel->getSize().y) +
					static_cast<unsigned int>(panel->getScrollbarWidth()))) {
				panel->setVerticalScrollbarValue(
					value + panel->getVerticalScrollAmount());
			} else {
				_makeNewDirectionalSelection(widgetData->directionalFlow.down,
					getGUI(), _downControl);
			}
		} else {
			_makeNewDirectionalSelection(widgetData->directionalFlow.down,
				getGUI(), _downControl);
		}
	}
	if ((*ui)[_leftControl.control]) {
		if (cursel.first == sfx::NO_WIDGET) {
			_makeNewDirectionalSelection(_menus.at(getGUI()).selectThisWidgetFirst,
				getGUI(), _leftControl);
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
				_makeNewDirectionalSelection(widgetData->directionalFlow.left,
					getGUI(), _leftControl);
			}
		} else {
			_makeNewDirectionalSelection(widgetData->directionalFlow.left,
				getGUI(), _leftControl);
		}
	}
	if ((*ui)[_rightControl.control]) {
		if (cursel.first == sfx::NO_WIDGET) {
			_makeNewDirectionalSelection(_menus.at(getGUI()).selectThisWidgetFirst,
				getGUI(), _rightControl);
		} else if (widgetType == type::ScrollablePanel) {
			const auto panel =
				std::dynamic_pointer_cast<ScrollablePanel>(cursel.second);
			const auto value = panel->getHorizontalScrollbarValue();
			// If this scrollbar has no amount, don't let the directional control
			// set the scroll value. Otherwise the input will be swallowed!
			if (panel->getHorizontalScrollAmount() > 0 &&
				panel->isHorizontalScrollbarShown() &&
				value < static_cast<unsigned int>(panel->getContentSize().x) -
					(static_cast<unsigned int>(panel->getSize().x) +
					static_cast<unsigned int>(panel->getScrollbarWidth()))) {
				panel->setHorizontalScrollbarValue(
					value + panel->getHorizontalScrollAmount());
			} else {
				_makeNewDirectionalSelection(widgetData->directionalFlow.right,
					getGUI(), _rightControl);
			}
		} else {
			_makeNewDirectionalSelection(widgetData->directionalFlow.right,
				getGUI(), _rightControl);
		}
	}
	return _menus.at(getGUI()).currentlySelectedWidget;
}

void sfx::gui::_makeNewDirectionalSelection(const sfx::WidgetIDRef newsel,
	const std::string& menu, const control_settings& sound) {
	if (newsel == sfx::NO_WIDGET) return;
	auto& prev = _menus.at(menu).previouslySelectedWidget;
	auto& current = _menus.at(menu).currentlySelectedWidget;
	const auto currentSel = _findWidget(current);
	if (newsel == GOTO_PREVIOUS_WIDGET) {
		// Do not allow selection to go ahead if the previous widget is now not
		// visible!
		if (_isWidgetFullyVisibleAndEnabled(_findWidget(prev)->ptr.get(), true,
			false)) std::swap(prev, current);
		else return;
	} else {
		// Do not allow selection to go ahead if the given widget is not visible!
		if (_isWidgetFullyVisibleAndEnabled(_findWidget(newsel)->ptr.get(), true,
			false)) {
			prev = current;
			current = newsel;
		} else return;
	}
	// Selection is moving, so play sound now.
	if (_audios && currentSel != _widgets.end()) {
		if (sound.name == "up") {
			if (const auto audio = (*_audios)[currentSel->moveUpSoundObject])
				audio->play(currentSel->moveUpSound);
		} else if (sound.name == "down") {
			if (const auto audio = (*_audios)[currentSel->moveDownSoundObject])
				audio->play(currentSel->moveDownSound);
		} else if (sound.name == "left") {
			if (const auto audio = (*_audios)[currentSel->moveLeftSoundObject])
				audio->play(currentSel->moveLeftSound);
		} else if (sound.name == "right") {
			if (const auto audio = (*_audios)[currentSel->moveRightSoundObject])
				audio->play(currentSel->moveRightSound);
		}
	}
	// Make a copy of the pointer instead of holding a reference to the original.
	// This is because the signal handler may invalidate the iterator returned by
	// _findWidget(). See the documentation on signalHandler().
	const Widget::Ptr widget = _findWidget(current)->ptr;
	signalHandler(widget, "MouseEntered");
	_showWidgetInScrollablePanel(widget);
};

void sfx::gui::_translateWidget(tgui::Widget::Ptr widget) {
	const auto widgetID = _getWidgetID(widget);
	if (widgetID == sfx::NO_WIDGET) return; // Placeholder widget, won't translate.
	String type = widget->getWidgetType();
	const auto widgetData = _findWidget(widgetID);
	if (widgetData->originalCaption.index() != 0) {
		if (type == type::ButtonBase) { // SingleCaption
			widgetData->castPtr<ButtonBase>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::Button) { // SingleCaption
			widgetData->castPtr<Button>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::BitmapButton) { // SingleCaption
			widgetData->castPtr<BitmapButton>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::CheckBox) { // SingleCaption
			widgetData->castPtr<CheckBox>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::ChildWindow) { // SingleCaption
			widgetData->castPtr<ChildWindow>()->setTitle(
				_getTranslatedText(*widgetData));
		} else if (type == type::ColorPicker) { // SingleCaption
			widgetData->castPtr<ColorPicker>()->setTitle(
				_getTranslatedText(*widgetData));
		} else if (type == type::ComboBox) { // ListOfCaptions
			auto w = widgetData->castPtr<ComboBox>();
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(*widgetData, i));
		} else if (type == type::EditBox) { // SingleCaption
			widgetData->castPtr<EditBox>()->setDefaultText(
				_getTranslatedText(*widgetData));
		} else if (type == type::FileDialog) { // ListOfCaptions
			auto w = widgetData->castPtr<FileDialog>();
			const auto n = std::get<sfx::gui::ListOfCaptions>(
				widgetData->originalCaption).size();
			if (n >= 1)
				w->setTitle(_getTranslatedText(*widgetData, 0));
			if (n >= 2)
				w->setConfirmButtonText(_getTranslatedText(*widgetData, 1));
			if (n >= 3)
				w->setCancelButtonText(_getTranslatedText(*widgetData, 2));
			if (n >= 4)
				w->setCreateFolderButtonText(_getTranslatedText(*widgetData, 3));
			if (n >= 5)
				w->setFilenameLabelText(_getTranslatedText(*widgetData, 4));
			if (n >= 8) {
				w->setListViewColumnCaptions(
					_getTranslatedText(*widgetData, 5),
					_getTranslatedText(*widgetData, 6),
					_getTranslatedText(*widgetData, 7)
				);
			}
			std::vector<std::pair<String, std::vector<String>>> filters =
				w->getFileTypeFilters();
			for (std::size_t i = 0, len = filters.size(); i < len; ++i)
				filters[i].first = _getTranslatedText(*widgetData, i + 8);
			w->setFileTypeFilters(filters);
		} else if (type == type::Label) { // SingleCaption
			widgetData->castPtr<Label>()->setText(_getTranslatedText(*widgetData));
		} else if (type == type::ListBox) { // ListOfCaptions
			auto w = widgetData->castPtr<ListBox>();
			for (std::size_t i = 0; i < w->getItemCount(); ++i)
				w->changeItemByIndex(i, _getTranslatedText(*widgetData, i));
		} else if (type == type::ListView) { // ListOfCaptions
			auto w = widgetData->castPtr<ListView>();
			std::size_t colCount = w->getColumnCount();
			for (std::size_t i = 0; i < colCount; ++i) {
				w->setColumnText(i, _getTranslatedText(*widgetData, i));
				for (std::size_t j = 0; j <= w->getItemCount(); ++j) {
					w->changeSubItem(i, j,
						_getTranslatedText(*widgetData, colCount * (i + 1) + j)
					);
				}
			}
		} else if (type == type::MenuBar) { // ListOfCaptions
			auto w = widgetData->castPtr<MenuBar>();
			std::vector<String> hierarchy;
			std::size_t index = 0;
			_translateMenuItems(w, *widgetData, w->getMenus(), hierarchy, index);
		} else if (type == type::MessageBox) { // ListOfCaptions
			auto w = widgetData->castPtr<MessageBox>();
			w->setTitle(_getTranslatedText(*widgetData, 0));
			w->setText(_getTranslatedText(*widgetData, 1));
			const auto& buttonCaptions = std::get<sfx::gui::ListOfCaptions>(
				widgetData->originalCaption);
			std::vector<String> newCaptions;
			for (std::size_t i = 2, len = buttonCaptions.size(); i < len; ++i)
				newCaptions.emplace_back(_getTranslatedText(*widgetData, i));
			w->changeButtons(newCaptions);
		} else if (type == type::ProgressBar) { // SingleCaption
			widgetData->castPtr<ProgressBar>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::RadioButton) { // SingleCaption
			widgetData->castPtr<RadioButton>()->setText(
				_getTranslatedText(*widgetData));
		} else if (type == type::TabContainer) { // ListOfCaptions
			auto w = widgetData->castPtr<TabContainer>();
			for (std::size_t i = 0; i < w->getTabs()->getTabsCount(); ++i)
				w->changeTabText(i, _getTranslatedText(*widgetData, i));
		} else if (type == type::Tabs) { // ListOfCaptions
			auto w = widgetData->castPtr<Tabs>();
			for (std::size_t i = 0; i < w->getTabsCount(); ++i)
				w->changeText(i, _getTranslatedText(*widgetData, i));
		} else if (type == type::TextArea) { // SingleCaption
			widgetData->castPtr<TextArea>()->setDefaultText(
				_getTranslatedText(*widgetData));
		} else if (type == type::ToggleButton) { // SingleCaption
			widgetData->castPtr<ToggleButton>()->setText(
				_getTranslatedText(*widgetData));
		}
	}
	if (widget->isContainer()) {
		const auto& widgetList = widgetData->castPtr<Container>()->getWidgets();
		for (auto& w : widgetList) _translateWidget(w);
	}
}

void sfx::gui::_translateMenuItems(const tgui::MenuBar::Ptr& w,
	const sfx::gui::widget_data& widgetData,
	const std::vector<MenuBar::GetMenusElement>& items,
	std::vector<String>& hierarchy, std::size_t& index) {
	// Each and every menu item is stored in _originalCaptions depth-first.
	// See the documentation on menuItemClickedSignalHandler() for more
	// info.
	for (const auto& item : items) {
		hierarchy.push_back(item.text);
		const tgui::String translatedCaption =
			_getTranslatedText(widgetData, index);
		w->changeMenuItem(hierarchy, translatedCaption);
		hierarchy.back() = translatedCaption;
		// I know, it's really ugly. Not much choice.
		w->connectMenuItem(hierarchy, &sfx::gui::menuItemClickedSignalHandler,
			this, _getWidgetID(w), static_cast<sfx::MenuItemID>(index++));
		_translateMenuItems(w, widgetData, item.menuItems, hierarchy, index);
		hierarchy.pop_back();
	}
}

bool sfx::gui::_load(engine::json& j) {
	_isLoading = true;
	// Disable all sounds during the process.
	const std::shared_ptr<sfx::audios> audiosCopy = _audios;
	_audios = nullptr;
	try {
		// First, reserve widgets within the internal vector.
		std::size_t widgetCount = 256;
		j.apply(widgetCount, { "reserve" }, true);
		_clearState(widgetCount);
		// Load game control settings.
		_upControl.load(j);
		_downControl.load(j);
		_leftControl.load(j);
		_rightControl.load(j);
		_selectControl.load(j);
		// Now, load the menus.
		const auto menuTypes = _scripts->getConcreteClassNames("Menu");
		_menus.reserve(menuTypes.size());
		for (const auto& menuType : menuTypes) _menus.insert({ menuType, {} });
		for (const auto& menuType : menuTypes) {
			const auto menu = _scripts->createObject(menuType);
			if (menu) _menus.at(menuType).object = menu;
			else _logger.error("Could not create menu \"{}\"!", menuType);
		}
		// Finally, load the name of the first menu to open.
		j.apply(_firstMenu, { "main" }, true);
		_isLoading = false;
		if (_menus.count(_firstMenu)) setGUI(_firstMenu); else {
			_logger.error("Could not locate main menu with name \"{}\".",
				_firstMenu);
			_audios = audiosCopy;
			return false;
		}
		_audios = audiosCopy;
		return true;
	} catch (...) {
		_isLoading = false;
		_audios = audiosCopy;
		throw;
	}
	_isLoading = false;
	_audios = audiosCopy;
	return false;
}

bool sfx::gui::_save(nlohmann::ordered_json& j) noexcept {
	j["reserve"] = _widgets.capacity();
	j["main"] = _firstMenu;
	_upControl.save(j);
	_downControl.save(j);
	_leftControl.save(j);
	_rightControl.save(j);
	_selectControl.save(j);
	return true;
}

void sfx::gui::_deleteWidget(const sfx::WidgetIDRef widget) {
	const auto widgetData = _findWidget(widget);
	const tgui::Widget::Ptr widgetPtr = widgetData->ptr;
	// 1. If this widget is a TabContainer, delete all of its Panels, too. Since
	//    the TabContainer is being deleted anyway, we don't care that we are
	//    removing the Panels directly (which is bad).
	if (widgetPtr->getWidgetType() == type::TabContainer) {
		std::vector<Widget::Ptr> children =
			widgetData->castPtr<TabContainer>()->getWidgets();
		for (const auto& child : children) {
			const auto childID = _getWidgetID(child);
			// If we can't find the child's ID, it's not a widget managed by us.
			if (childID == sfx::NO_WIDGET) continue;
			_deleteWidget(childID);
		}
	}
	// 2 & 6. If this widget is a ChildWindow, remove it from its parent's
	//        ChildWindowList. Also, remove the widget from its parent.
	const auto parentPtr = widgetPtr->getParent();
	if (parentPtr) {
		const auto parentID = _getWidgetID(parentPtr);
		const auto parentData = _findWidget(parentID);
		if (parentData == _widgets.end())
			_logger.warning("Child widget \"{}\" still had pointer to parent "
				"\"{}\" that has already been deleted!", widget, parentID);
		else
			_removeWidgetFromParent(*parentData, *widgetData);
	}
	// 3a. Go through every widget and if the widget to remove is referenced,
	//     remove it. To speed this up we could just leave the references and
	//     remove them if we come across them later.
	for (auto& data : _widgets) {
		if (data.directionalFlow.up == widget)
			data.directionalFlow.up = sfx::NO_WIDGET;
		if (data.directionalFlow.down == widget)
			data.directionalFlow.down = sfx::NO_WIDGET;
		if (data.directionalFlow.left == widget)
			data.directionalFlow.left = sfx::NO_WIDGET;
		if (data.directionalFlow.right == widget)
			data.directionalFlow.right = sfx::NO_WIDGET;
	}
	// 3b. Do the same for the menus.
	for (auto& menu : _menus) {
		auto& menuData = menu.second;
		if (menuData.selectThisWidgetFirst == widget)
			menuData.selectThisWidgetFirst = sfx::NO_WIDGET;
		// If the removed widget was previously selected at all, then remove it
		// from the history. If the removed widget is currently selected, then
		// deselect it and erase the history, as well.
		if (menuData.previouslySelectedWidget == widget ||
			menuData.currentlySelectedWidget == widget)
			menuData.previouslySelectedWidget = sfx::NO_WIDGET;
		if (menuData.currentlySelectedWidget == widget)
			menuData.currentlySelectedWidget = sfx::NO_WIDGET;
	}
	// 4. Delete the widget's sprite, if it has one.
	_widgetSprites.erase(widgetPtr);
	// 5. Clear data and free up the ID.
	*widgetData = {};
	_availableCells.insert(widget);
}

void sfx::gui::_setTranslatedString(sfx::gui::widget_data& data,
	const std::string& text, CScriptArray* variables) {
	data.originalCaption = sfx::gui::SingleCaption(text, variables);
}

void sfx::gui::_setTranslatedString(sfx::gui::widget_data& data,
	const std::string& text, CScriptArray* variables, const std::size_t index) {
	if (!std::holds_alternative<sfx::gui::ListOfCaptions>(data.originalCaption))
		data.originalCaption = sfx::gui::ListOfCaptions();
	auto& list = std::get<sfx::gui::ListOfCaptions>(data.originalCaption);
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

std::string sfx::gui::_getTranslatedText(const sfx::gui::widget_data& data) const {
	return _getTranslatedText(std::get<sfx::gui::SingleCaption>(
		data.originalCaption), [&](engine::logger* const logger,
			const std::string& typeName) {
				logger->warning("Unsupported type \"{}\" given when translating "
					"widget \"{}\"'s caption: inserting blank string instead.",
					typeName, _getWidgetID(data.ptr));
		});
}

std::string sfx::gui::_getTranslatedText(const sfx::gui::widget_data& data,
	const std::size_t index) const {
	return _getTranslatedText(std::get<sfx::gui::ListOfCaptions>(
		data.originalCaption).at(index),
			[&](engine::logger* const logger, const std::string& typeName) {
				logger->warning("Unsupported type \"{}\" given when translating "
					"widget \"{}\"'s #{} caption: inserting blank string instead.",
					typeName, _getWidgetID(data.ptr), index);
		});
}

void sfx::gui::_applySprite(widget_data& widget, const std::string& sheet,
	const std::string& key) {
	// Prevent deleting sprite objects if there won't be any change.
	if (widget.spritesheetKey != sheet || widget.spriteKey != key) {
		widget.spritesheetKey = sheet;
		widget.spriteKey = key;
		_widgetSprites.erase(widget.ptr);
	}
}

Widget::Ptr sfx::gui::_widgetFactory(const std::string& wType) const {
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
	} else if (type == type::ButtonBase) {
		return tgui::ButtonBase::create();
	} else if (type == type::EditBox) {
		return tgui::EditBox::create();
	} else if (type == type::MenuBar) {
		return tgui::MenuBar::create();
	} else if (type == type::ChildWindow) {
		const auto win = tgui::ChildWindow::create();
		win->setTitleButtons(ChildWindow::TitleButton::Close |
			ChildWindow::TitleButton::Minimize |
			ChildWindow::TitleButton::Maximize);
		win->setResizable();
		return win;
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
		return tgui::TabContainer::create();
	} else if (type == type::TextArea) {
		const auto textArea = tgui::TextArea::create();
		textArea->setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
		textArea->setTabString("    ");
		return textArea;
	} else if (type == type::SpinControl) {
		return tgui::SpinControl::create();
	} else if (type == type::ClickableWidget) {
		return tgui::ClickableWidget::create();
	} else if (type == type::BoxLayout) {
		return tgui::BoxLayout::create();
	} else if (type == type::BoxLayoutRatios) {
		return tgui::BoxLayoutRatios::create();
	} else if (type == type::Slider) {
		return tgui::Slider::create();
	} else {
		_logger.error("Attempted to create a widget of type \"{}\": that widget "
			"type is not supported.", wType);
		return nullptr;
	}
}

sfx::WidgetID sfx::gui::_storeWidget(const tgui::Widget::Ptr& w) {
	assert(w);
	const auto widgetID = _findNextWidgetID();
	w->setUserData(widgetID);
	const auto widget = _findWidget(widgetID);
	widget->ptr = w;
	// If this widget can be "selected," apply the default sounds.
	widget->moveUpSoundObject = _upControl.defaultSoundObject;
	widget->moveUpSound = _upControl.defaultSoundName;
	widget->moveDownSoundObject = _downControl.defaultSoundObject;
	widget->moveDownSound = _downControl.defaultSoundName;
	widget->moveLeftSoundObject = _leftControl.defaultSoundObject;
	widget->moveLeftSound = _leftControl.defaultSoundName;
	widget->moveRightSoundObject = _rightControl.defaultSoundObject;
	widget->moveRightSound = _rightControl.defaultSoundName;
	widget->selectionSoundObject = _selectControl.defaultSoundObject;
	widget->selectionSound = _selectControl.defaultSoundName;
	if (w->getWidgetType() == type::ChildWindow) {
		// If the widget is a ChildWindow, don't forget to turn on automatic
		// handling of minimise and maximise.
		widget->childWindowData = sfx::gui::child_window_properties();
	}
	return widgetID;
}

sfx::WidgetID sfx::gui::_storeWidgetAndConnectSignals(const tgui::Widget::Ptr& w) {
	const auto id = _storeWidget(w);
	_connectSignals(w);
	return id;
}

sfx::WidgetID sfx::gui::_createWidget(const std::string& wType) {
	if (const auto w = _widgetFactory(wType))
		return _storeWidgetAndConnectSignals(w);
	else return sfx::NO_WIDGET;
}

void sfx::gui::_addWidgetToParent(sfx::gui::widget_data& parent,
	const sfx::gui::widget_data& child) {
	// If the child is a ChildWindow, we'll need to add it to the parent's
	// minimisedChildWindowList if the ChildWindow is minimised.
	if (child.ptr->getWidgetType() == type::ChildWindow && child.childWindowData &&
		child.childWindowData->isMinimised) {
		parent.minimisedChildWindowList.minimise(_getWidgetID(child.ptr));
	}
	parent.castPtr<Container>()->add(child.ptr);
}

void sfx::gui::_addWidgetToGrid(widget_data& parent, const widget_data& child,
	const std::size_t row, const std::size_t col) {
	// If the child is a ChildWindow, we'll need to add it to the parent's
	// minimisedChildWindowList if the ChildWindow is minimised.
	if (child.ptr->getWidgetType() == type::ChildWindow && child.childWindowData &&
		child.childWindowData->isMinimised) {
		parent.minimisedChildWindowList.minimise(_getWidgetID(child.ptr));
	}
	parent.castPtr<Grid>()->addWidget(child.ptr, row, col);
}

void sfx::gui::_removeWidgetFromParent(sfx::gui::widget_data& parent,
	const sfx::gui::widget_data& child) {
	// If the child is a ChildWindow, we'll need to remove it from the parent's
	// minimisedChildWindowList.
	if (child.ptr->getWidgetType() == type::ChildWindow) {
		parent.minimisedChildWindowList.restore(_getWidgetID(child.ptr));
	}
	parent.castPtr<Container>()->remove(child.ptr);
}

sfx::WidgetID sfx::gui::_findNextWidgetID() {
	// If there is a free cell, use it.
	if (!_availableCells.empty()) {
		const auto id = *_availableCells.begin();
		_availableCells.erase(id);
		return id;
	}
	// Otherwise, use the next empty cell.
	const auto newID = _widgetCounter++;
	// Allocate more cells if we've run out of IDs.
	if (_widgetCounter == _widgets.capacity()) {
		const auto newCapacity = static_cast<std::size_t>(
			std::ceil(_widgets.capacity() * 1.5));
		_logger.warning("Ran out of widget cells, allocating {} more...",
			newCapacity - _widgets.capacity());
		_widgets.reserve(newCapacity);
	}
	// Increase the actual size of the vector by one.
	_widgets.emplace_back();
	return newID;
}

std::pair<sfx::WidgetID, tgui::Widget::Ptr>
	sfx::gui::_findCurrentlySelectedWidget() {
	if (getGUI().empty()) return { sfx::NO_WIDGET, nullptr };
	const auto& cursel = _menus.at(getGUI()).currentlySelectedWidget;
	if (cursel == sfx::NO_WIDGET) return { sfx::NO_WIDGET, nullptr };
	const auto widget = _findWidget(cursel);
	if (widget == _widgets.end()) {
		_logger.error("Currently selected widget \"{}\" couldn't be found! "
			"Current menu is \"{}\". Deselecting...", cursel, getGUI());
		_menus.at(getGUI()).currentlySelectedWidget = sfx::NO_WIDGET;
		return { sfx::NO_WIDGET, nullptr };
	}
	return { cursel, widget->ptr };
}

void sfx::gui::_clearState(const std::size_t widgetCount) {
	_gui.removeAllWidgets();
	_currentGUI.clear();
	_previousGUI.clear();
	_menus.clear();
	_widgets.clear();
	_widgets.shrink_to_fit();
	_availableCells.clear();
	_widgetSprites.clear();
	_firstMenu.clear();
	// Reserve the widget cells.
	_logger.write("Reserving {} widget cell{}...", widgetCount,
		widgetCount == 1 ? "" : "s");
	_widgets.reserve(widgetCount);
	// Create empty first N cells. Since IDs are based on ROOT_WIDGET and not 0.
	// To improve memory efficiency you could add offset to _findWidget(), but it's
	// only one empty cell right now so let's keep it simple.
	_widgets.resize(sfx::ROOT_WIDGET);
	// Always store the first widget as the root widget.
	_widgetCounter = sfx::ROOT_WIDGET;
	_storeWidgetAndConnectSignals(_gui.getContainer());
}

sfx::gui::menu_data::~menu_data() noexcept {
	if (object) object->Release();
}

void sfx::gui::control_settings::load(engine::json& j) {
	control.clear();
	j.apply(control, { name, "control" }, true);
	defaultSoundObject.clear();
	j.apply(defaultSoundObject, { name, "soundobject" }, true);
	defaultSoundName.clear();
	j.apply(defaultSoundName, { name, "soundname" }, true);
}

void sfx::gui::control_settings::save(nlohmann::ordered_json& j) {
	j[name]["control"] = control;
	j[name]["soundobject"] = defaultSoundObject;
	j[name]["soundname"] = defaultSoundName;
}
