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
#include "guiconstants.hpp"
#include "fmtsfx.hpp"
#include <algorithm>

using namespace tgui;

// NON-WIDGET //

void sfx::gui::_setGUI(const std::string& name) {
	setGUI(name, true, true);
}

bool sfx::gui::_menuExists(const std::string& menu) {
	return _menus.find(menu) != _menus.end();
}

asIScriptObject* sfx::gui::_getMenu(const std::string& menu) {
	if (!_menuExists(menu)) {
		_logger.error("Tried to get the object of a non-existent menu \"{}\".",
			menu);
		return nullptr;
	}
	_menus.at(menu).object->AddRef();
	return _menus.at(menu).object;
}

void sfx::gui::_dumpWidgetsToString(std::string& str,
	const sfx::gui::widget_data& data, const std::size_t numberOfTabs) const {
	str.append(std::string(numberOfTabs, '\t')).append(data).append("\n");
	if (!data.ptr->isContainer()) return;
	const auto& children = data.castPtr<Container>()->getWidgets();
	for (const auto& child : children) {
		const auto id = _getWidgetID(child);
		if (id == sfx::NO_WIDGET)
			str.append(std::string(numberOfTabs + 1, '\t')).
				append("PLACEHOLDER \"").
				append(child->getWidgetType().toStdString()).
				append("\": \"").
				append(child->getWidgetName().toStdString()).
				append("\"\n");
		else _dumpWidgetsToString(str, *_findWidget(id), numberOfTabs + 1);
	}
}

void sfx::gui::_dumpWidgetsToLog() const {
	std::string str("\n~~~ WIDGET DATA ~~~\n");
	_dumpWidgetsToString(str, _widgets.at(sfx::ROOT_WIDGET));
	str.erase(str.size() - 1);
	_logger.write(str);
}

std::size_t sfx::gui::_getHeightOfTallestFrame(const std::string& sheet) const {
	if (_sheets->exists(sheet)) return (*_sheets)[sheet]->heightOfTallestFrame();
	_logger.error("Attempted to find the tallest frame from a spritesheet \"{}\": "
		"this spritesheet does not exist!", sheet);
	return 0;
}

// WIDGETS //

bool sfx::gui::_widgetExists(const sfx::WidgetIDRef id) const {
	if (id == sfx::NO_WIDGET) return false;
	// If it's at or above the counter, then we know it can't exist yet.
	if (id >= _widgetCounter) return false;
	// If this widget has been marked for replacement, it doesn't exist.
	if (_availableCells.find(id) != _availableCells.end()) return false;
	// Otherwise, it exists.
	return true;
}

sfx::WidgetID sfx::gui::_getWidgetFocused(const sfx::WidgetIDRef parent) const {
	if (parent == sfx::NO_WIDGET && _gui.getFocusedChild()) {
		return _getWidgetID(_gui.getFocusedChild());
	} else {
		START_WITH_WIDGET(parent)
			if (!widget->ptr->isContainer()) UNSUPPORTED_WIDGET_TYPE()
			auto c = widget->castPtr<Container>()->getFocusedChild();
			if (c) return _getWidgetID(c);
		END("Attempted to find the widget with setfocus that is within widget "
			"\"{}\".", parent)
	}
	return sfx::NO_WIDGET;
}

sfx::WidgetID sfx::gui::_createWidgetScriptInterface(
	const std::string& newWidgetType) {
	const auto id = _createWidget(newWidgetType);
	START()
		if (id == sfx::NO_WIDGET) ERROR("Could not create the new widget.")
	END("Attempted to create a new \"{}\" widget.", newWidgetType)
	return id;
}

void sfx::gui::_connectSignal(const sfx::WidgetIDRef id, const std::string& signal,
	asIScriptFunction* const handler) {
	START_WITH_WIDGET(id)
		if (!SIGNALS.count(signal)) ERROR("This is not a signal!")
		if (signal == signal::Closing) ERROR("You cannot use this method to "
			"connect a handler to this signal!");
		const auto supportedTypes = SIGNALS.equal_range(signal);
		bool supported = false;
		std::for_each(supportedTypes.first, supportedTypes.second,
			[&supported, &widgetType](auto& types) {
			if (supported) return;
			supported = types.second.count(widgetType.toStdString()) ||
				types.second.empty();
		});
		if (!supported)
			ERROR("This signal is not supported for this type of widget!");
		if (widget->singleSignalHandlers.count(signal))
			widget->singleSignalHandlers.erase(signal);
		if (handler) widget->singleSignalHandlers.emplace(signal, handler);
	END("Attempted to connect a handler to the \"{}\" signal for widget with ID "
		"\"{}\".", signal, id)
	if (handler) handler->Release();
}

void sfx::gui::_connectSignal(const sfx::WidgetIDRef id,
	asIScriptFunction* const handler) {
	START_WITH_WIDGET(id)
		widget->multiSignalHandler = nullptr;
		if (handler) widget->multiSignalHandler =
			std::make_unique<engine::CScriptWrapper<asIScriptFunction>>(handler);
	END("Attempted to connect a multi signal handler to widget with ID \"{}\".",
		id)
	if (handler) handler->Release();
}

void sfx::gui::_disconnectSignals(const CScriptArray* const ids) {
	if (!ids) {
		_logger.warning("Null array given to disconnectSignals(): doing "
			"nothing.");
		return;
	}
	for (asUINT i = 0, len = ids->GetSize(); i < len; ++i) {
		const WidgetID id = *static_cast<const WidgetID* const>(ids->At(i));
		START_WITH_WIDGET(id)
			widget->singleSignalHandlers.clear();
			widget->multiSignalHandler = nullptr;
			widget->childWindowClosingHandler = nullptr;
		END("Attempted to disconnect signal handlers from a widget with ID "
			"\"{}\".", id)
	}
	ids->Release();
}

sfx::WidgetID sfx::gui::_getParent(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		return containerID;
	END("Attempted to get the ID of a widget \"{}\"'s parent.", id)
	return sfx::NO_WIDGET;
}

void sfx::gui::_deleteWidgetScriptInterface(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		if (id == sfx::ROOT_WIDGET) ERROR("You cannot delete the root widget!");
		if (widgetType == type::Panel && containerID != sfx::NO_WIDGET &&
			container->ptr->getWidgetType() == type::TabContainer) {
			_logger.warning("The manual deletion of a Panel (\"{}\") from a "
				"TabContainer (\"{}\") is forbidden! Using removeTabAndPanel() "
				"instead...", id, containerID);
			_removeTabAndPanel(id);
		} else _deleteWidget(id);
	END("Attempted to delete the widget \"{}\".", id)
}

void sfx::gui::_setWidgetName(const sfx::WidgetIDRef id, const std::string& name) {
	START_WITH_WIDGET(id)
		return widget->ptr->setWidgetName(name);
	END("Attempted to set widget \"{}\"'s name to \"{}\". The widget is of type "
		"\"{}\".", id, name, widgetType);
}

std::string sfx::gui::_getWidgetName(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		return widget->ptr->getWidgetName().toStdString();
	END("Attempted to get widget \"{}\"'s name. The widget is of type \"{}\".", id,
		widgetType);
	return {};
}

void sfx::gui::_setWidgetFocus(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		widget->ptr->setFocused(true);
	END("Attempted to set the focus to a widget \"{}\".", id)
}

void sfx::gui::_setWidgetFont(const sfx::WidgetIDRef id,
	const std::string& fontName) {
	START_WITH_WIDGET(id)
		if (!_fonts) ERROR("No fonts object has been given to this gui object.")
		auto fontPath = _fonts->getFontPath(fontName);
		// Invalid font name will be logged by fonts class.
		if (!fontPath.empty()) {
			auto font = tgui::Font(fontPath);
			font.setSmooth(false);
			widget->ptr->getRenderer()->setFont(font);
		}
	END("Attempted to set the font \"{}\" to a widget \"{}\".", fontName, id)
}

void sfx::gui::_setWidgetInheritedFont(const WidgetIDRef id,
	const std::string& font) {
	START_WITH_WIDGET(id)
		if (!_fonts) ERROR("No fonts object has been given to this gui object.")
		// Invalid font name will be logged by fonts class.
		const auto fontPath = _fonts->getFontPath(font);
		if (!fontPath.empty()) {
			auto fontObject = tgui::Font(fontPath);
			fontObject.setSmooth(false);
			widget->ptr->setInheritedFont(fontObject);
		}
	END("Attempted to set the inherited font of widget \"{}\", which is of type "
		"\"{}\", to \"{}\".", id, widgetType, font)
}

void sfx::gui::_setWidgetPosition(const sfx::WidgetIDRef id, const std::string& x,
	const std::string& y) {
	START_WITH_WIDGET(id)
		widget->ptr->setPosition(x.c_str(), y.c_str());
	END("Attempted to set the position (\"{}\",\"{}\") to a widget \"{}\".", x, y,
		id)
}

sf::Vector2f sfx::gui::_getWidgetAbsolutePosition(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		return widget->ptr->getAbsolutePosition();
	END("Attempted to get the absolute position of a widget \"{}\".", id)
	return {};
}

void sfx::gui::_setWidgetOrigin(const sfx::WidgetIDRef id, const float x,
	const float y) {
	START_WITH_WIDGET(id)
		widget->ptr->setOrigin(x, y);
	END("Attempted to set the origin ({},{}) to a widget \"{}\".", x, y, id)
}

void sfx::gui::_setWidgetSize(const sfx::WidgetIDRef id, const std::string& w,
	const std::string& h) {
	START_WITH_WIDGET(id)
		if (w.empty() && h.empty())
			ERROR("Did you mean to provide an empty width and height?");
		if (w.empty()) widget->ptr->setHeight(h.c_str());
		else if (h.empty()) widget->ptr->setWidth(w.c_str());
		else widget->ptr->setSize(w.c_str(), h.c_str());
	END("Attempted to set the size (\"{}\",\"{}\") to a widget \"{}\".", w, h, id)
}

sf::Vector2f sfx::gui::_getWidgetFullSize(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		return widget->ptr->getFullSize();
	END("Attempted to get the full size of a widget \"{}\".", id)
	return {};
}

sf::Vector2f sfx::gui::_getWidgetScaledFullSize(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		return widget->ptr->getFullSize() * _scalingFactor;
	END("Attempted to get the scaled full size of a widget \"{}\".", id)
	return {};
}

void sfx::gui::_setWidgetScale(const sfx::WidgetIDRef id, const float x,
	const float y) {
	START_WITH_WIDGET(id)
		widget->ptr->setScale({ x, y });
	END("Attempted to set the scale ({},{}) to a widget \"{}\".", x, y, id)
}

void sfx::gui::_setWidgetEnabled(const sfx::WidgetIDRef id, const bool enable) {
	START_WITH_WIDGET(id)
		widget->ptr->setEnabled(enable);
	END("Attempted to update widget \"{}\"'s enabled state.", id)
}

bool sfx::gui::_getWidgetEnabled(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		return widget->ptr->isEnabled();
	END("Attempted to get the enabled property of a widget \"{}\".", id)
	return false;
}

void sfx::gui::_setWidgetVisibility(const sfx::WidgetIDRef id, const bool visible) {
	START_WITH_WIDGET(id)
		widget->ptr->setVisible(visible);
	END("Attempted to update widget \"{}\"'s visibility.", id)
}

bool sfx::gui::_getWidgetVisibility(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		return widget->ptr->isVisible();
	END("Attempted to get the visibility property of a widget \"{}\".", id)
	return false;
}

void sfx::gui::_moveWidgetToFront(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		widget->ptr->moveToFront();
	END("Attempted to move the widget \"{}\" to the front.", id)
}

void sfx::gui::_moveWidgetToBack(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		widget->ptr->moveToBack();
	END("Attempted to move the widget \"{}\" to the back.", id)
}

void sfx::gui::_setWidgetText(const sfx::WidgetIDRef id, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(id)
		// For EditBoxes and TextAreas, don't translate the text, as this is text
		// that the user can edit.
		if (widgetType == type::EditBox) {
			widget->castPtr<EditBox>()->setText(text);
		} else if (widgetType == type::TextArea) {
			widget->castPtr<TextArea>()->setText(text);
		} else {
			if (widgetType != type::BitmapButton && widgetType != type::Label &&
				widgetType != type::Button && widgetType != type::ChildWindow &&
				widgetType != type::CheckBox && widgetType != type::RadioButton &&
				widgetType != type::ButtonBase)
					UNSUPPORTED_WIDGET_TYPE()
			_setTranslatedString(*widget, text, variables);
			_translateWidget(widget->ptr);
		}
	END("Attempted to set the caption \"{}\" to a widget \"{}\" of type \"{}\".",
		text, id, widgetType)
	if (variables) variables->Release();
}

void sfx::gui::_setWidgetTextSize(const sfx::WidgetIDRef id,
	const unsigned int size) {
	START_WITH_WIDGET(id)
		widget->ptr->setTextSize(size);
	END("Attempted to set the character size {} to widget \"{}\", which is of "
		"type \"{}\".", size, id, widgetType)
}

void sfx::gui::_setWidgetIndex(const sfx::WidgetIDRef id,
	const std::size_t index) {
	START_WITH_WIDGET(id)
		// If the widget has no parent, assume it's in the root GUI container.
		const bool result = containerID == sfx::NO_WIDGET ?
			_gui.setWidgetIndex(widget->ptr, index) :
			container->castPtr<Container>()->setWidgetIndex(widget->ptr, index);
		if (!result) ERROR("Either the parent of the widget could not be found or "
			"the given index was too high!")
	END("Attempted to set a widget \"{}\"'s index to {}.", id, index)
}

void sfx::gui::_setWidgetAutoLayout(const sfx::WidgetIDRef id,
	const tgui::AutoLayout layout) {
	START_WITH_WIDGET(id)
		widget->ptr->setAutoLayout(layout);
	END("Attempted to set the auto layout value {} to widget \"{}\", which is of "
		"type \"{}\".", layout, id, widgetType)
}

// DIRECTIONAL FLOW //

void sfx::gui::_setWidgetDirectionalFlow(const WidgetIDRef id,
	const WidgetIDRef upID, const WidgetIDRef downID, const WidgetIDRef leftID,
	const WidgetIDRef rightID) {
	static const auto widgetDoesNotExist = [&](const WidgetIDRef doesNotExist) {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"to the widgets up=\"{}\", down=\"{}\", left=\"{}\", right=\"{}\". "
			"The widget \"{}\" does not exist.", id, upID, downID, leftID, rightID,
			doesNotExist);
	};
	const auto widget = _findWidget(id);
	if (widget == _widgets.end()) {
		widgetDoesNotExist(id);
		return;
	}
	static const auto checkWidget = [&](const WidgetIDRef checkID) -> bool {
		if (checkID != sfx::NO_WIDGET && checkID != GOTO_PREVIOUS_WIDGET &&
			_findWidget(checkID) == _widgets.end()) {
			widgetDoesNotExist(checkID);
			return false;
		}
		return true;
	};
	if (!checkWidget(upID)) return;
	if (!checkWidget(downID)) return;
	if (!checkWidget(leftID)) return;
	if (!checkWidget(rightID)) return;
	// Directional flow should not traverse across menus ideally...
	// But no easy way to check for that after the rewrite.
	widget->directionalFlow.up = upID;
	widget->directionalFlow.down = downID;
	widget->directionalFlow.left = leftID;
	widget->directionalFlow.right = rightID;
}

void sfx::gui::_setWidgetDirectionalFlowStart(const std::string& menu,
	const WidgetIDRef id) {
	START_WITH_WIDGET(id)
		if (!_menuExists(menu)) ERROR("This menu does not exist!");
		_menus.at(menu).selectThisWidgetFirst = id;
	END("Attempted to set the widget \"{}\" as the first to be selected upon "
		"initial directional input, for the menu \"{}\".", id, menu)
}

void sfx::gui::_setWidgetDirectionalFlowSelection(const std::string& menu,
	const WidgetIDRef id) {
	START_WITH_WIDGET(id)
		if (!_menuExists(menu)) ERROR("This menu does not exist!");
		_makeNewDirectionalSelection(id, menu, _upControl);
	END("Attempted to manually directionally select the widget \"{}\", in the "
		"menu \"{}\".", id, menu)
}

void sfx::gui::_setDirectionalFlowAngleBracketSprite(const std::string& corner,
	const std::string& sheet, const std::string& key) {
	START()
		if (!_sheets->exists(sheet)) ERROR("This spritesheet does not exist.")
		const auto spritesheet = (*_sheets)[sheet];
		if (!spritesheet->doesSpriteExist(key))
			ERROR("This sprite does not exist.")
		const auto cornerFormatted = tgui::String(corner).trim().toLower();
		if (cornerFormatted == "ul") {
			_angleBracketUL.setSpritesheet(spritesheet);
			_angleBracketUL.setSprite(key);
		} else if (cornerFormatted == "ur") {
			_angleBracketUR.setSpritesheet(spritesheet);
			_angleBracketUR.setSprite(key);
		} else if (cornerFormatted == "ll") {
			_angleBracketLL.setSpritesheet(spritesheet);
			_angleBracketLL.setSprite(key);
		} else if (cornerFormatted == "lr") {
			_angleBracketLR.setSpritesheet(spritesheet);
			_angleBracketLR.setSprite(key);
		} else {
			ERROR("Unrecognised corner, must be \"UL\", \"UR\", \"LL\", or "
				"\"LR\".")
		}
	END("Attempted to set the sprite \"{}\" from spritesheet \"{}\" as the "
		"directional flow angle bracket for the \"{}\" corner.", key, sheet,
		corner)
}

void sfx::gui::_setWidgetMoveUpFromSound(const sfx::WidgetIDRef id,
	const std::string& object, const std::string& sound) {
	START_WITH_WIDGET(id)
		widget->moveUpSoundObject = object;
		widget->moveUpSound = sound;
	END("Attempted to set the widget \"{}\"'s move up sound to \"{}\", with "
		"audio object \"{}\".", id, sound, object)
}

void sfx::gui::_setWidgetMoveDownFromSound(const sfx::WidgetIDRef id,
	const std::string& object, const std::string& sound) {
	START_WITH_WIDGET(id)
		widget->moveDownSoundObject = object;
		widget->moveDownSound = sound;
	END("Attempted to set the widget \"{}\"'s move down sound to \"{}\", with "
		"audio object \"{}\".", id, sound, object)
}

void sfx::gui::_setWidgetMoveLeftFromSound(const sfx::WidgetIDRef id,
	const std::string& object, const std::string& sound) {
	START_WITH_WIDGET(id)
		widget->moveLeftSoundObject = object;
		widget->moveLeftSound = sound;
	END("Attempted to set the widget \"{}\"'s move left sound to \"{}\", with "
		"audio object \"{}\".", id, sound, object)
}

void sfx::gui::_setWidgetMoveRightFromSound(const sfx::WidgetIDRef id,
	const std::string& object, const std::string& sound) {
	START_WITH_WIDGET(id)
		widget->moveRightSoundObject = object;
		widget->moveRightSound = sound;
	END("Attempted to set the widget \"{}\"'s move right sound to \"{}\", with "
		"audio object \"{}\".", id, sound, object)
}

void sfx::gui::_setWidgetSelectionSound(const sfx::WidgetIDRef id,
	const std::string& object, const std::string& sound) {
	START_WITH_WIDGET(id)
		widget->selectionSoundObject = object;
		widget->selectionSound = sound;
	END("Attempted to set the widget \"{}\"'s selection sound to \"{}\", with "
		"audio object \"{}\".", id, sound, object)
}

// SPRITES //

void sfx::gui::_setWidgetSprite(const sfx::WidgetIDRef id,
	const std::string& sheet, const std::string& key) {
	START_WITH_WIDGET(id)
		if (widgetType != type::BitmapButton && widgetType != type::Picture)
			UNSUPPORTED_WIDGET_TYPE()
		_applySprite(*widget, sheet, key);
	END("Attempted to set the sprite \"{}\" from sheet \"{}\" to widget \"{}\", "
		"which is of type \"{}\".", key, sheet, id, widgetType)
}

void sfx::gui::_matchWidgetSizeToSprite(const sfx::WidgetIDRef id,
	const bool overrideSetSize) {
	START_WITH_WIDGET(id)
		if (widgetType != type::Picture) UNSUPPORTED_WIDGET_TYPE()
		widget->doNotOverridePictureSizeWithSpriteSize = overrideSetSize;
	END("Attempted to match widget \"{}\"'s size to its set sprite. The widget is "
		"of type \"{}\".", id, widgetType)
}

// LABEL //

void sfx::gui::_setWidgetTextStyles(const sfx::WidgetIDRef id,
	const std::string& styles) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label, castWidget->getRenderer()->setTextStyle({ styles });)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text styles \"{}\" to widget \"{}\", which is of "
		"type \"{}\".", styles, id, widgetType)
}

void sfx::gui::_setWidgetTextMaximumWidth(const sfx::WidgetIDRef id,
	const float w) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label, castWidget->setMaximumTextWidth(w);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text max width {} to widget \"{}\", which is of "
		"type \"{}\".", w, id, widgetType)
}

void sfx::gui::_setWidgetTextColour(const sfx::WidgetIDRef id,
	const sf::Color& colour) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label, castWidget->getRenderer()->setTextColor(colour);)
		ELSE_IF_WIDGET_IS(EditBox,
			castWidget->getRenderer()->setTextColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text colour \"{}\" to widget \"{}\", which is of "
		"type \"{}\".", colour, id, widgetType)
}

void sfx::gui::_setWidgetTextOutlineColour(const sfx::WidgetIDRef id,
	const sf::Color& colour) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline colour \"{}\" to widget \"{}\", which "
		"is of type \"{}\".", colour, id, widgetType)
}

void sfx::gui::_setWidgetTextOutlineThickness(const sfx::WidgetIDRef id,
	const float thickness) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineThickness(thickness);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline thickness {} to widget \"{}\", which "
		"is of type \"{}\".", thickness, id, widgetType)
}

void sfx::gui::_setWidgetTextAlignment(const sfx::WidgetIDRef id,
	const tgui::Label::HorizontalAlignment h,
	const tgui::Label::VerticalAlignment v) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Label, castWidget->setHorizontalAlignment(h);
			castWidget->setVerticalAlignment(v);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text horizontal alignment {} and vertical alignment "
		"{} to widget \"{}\", which is of type \"{}\".", h, v, id, widgetType)
}

// EDITBOX AND TEXTAREA //

std::string sfx::gui::_getWidgetText(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(EditBox, return castWidget->getText().toStdString();)
		IF_WIDGET_IS(TextArea, return castWidget->getText().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of a widget \"{}\", which is of type \"{}\".",
		id, widgetType)
	return "";
}

void sfx::gui::_setEditBoxRegexValidator(const sfx::WidgetIDRef id,
	const std::string& regex) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(EditBox,
			if (!castWidget->setInputValidator(regex)) ERROR("Invalid regex!");
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the widget \"{}\", which is of type \"{}\", to validate "
		"its input with the regex:  {}  .", id, widgetType, regex);
}

void sfx::gui::_setWidgetDefaultText(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const variables) {
	START_WITH_WIDGET(id)
		if (widgetType != type::EditBox && widgetType != type::TextArea)
			UNSUPPORTED_WIDGET_TYPE()
		_setTranslatedString(*widget, text, variables);
		_translateWidget(widget->ptr);
	END("Attempted to set the default text \"{}\" to widget \"{}\", which is of "
		"type \"{}\".", text, id, widgetType)
	if (variables) variables->Release();
}

bool sfx::gui::_editBoxOrTextAreaHasFocus() const {
	return _editBoxOrTextAreaHasSetFocus;
}

void sfx::gui::_optimiseTextAreaForMonospaceFont(const sfx::WidgetIDRef id,
	const bool optimise) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(TextArea,
			castWidget->enableMonospacedFontOptimization(optimise);)
		ELSE_UNSUPPORTED()
	END("Attempted to turn optimisation for monospace fonts {} for widget \"{}\", "
		"which is of type \"{}\".", optimise ? "on" : "off", id, widgetType)
}

void sfx::gui::_getCaretLineAndColumn(const sfx::WidgetIDRef id,
	std::size_t& line, std::size_t& column) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(TextArea,
			line = castWidget->getCaretLine();
			column = castWidget->getCaretColumn();
		)
		ELSE_IF_WIDGET_IS(EditBox,
			line = 1;
			column = castWidget->getCaretPosition() + 1;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to retrieve the caret line and column of widget \"{}\", which "
		"is of type \"{}\".", id, widgetType)
}

void sfx::gui::_setEditBoxTextAlignment(const WidgetIDRef id,
	const tgui::EditBox::Alignment alignment) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(EditBox, castWidget->setAlignment(alignment);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text alignment to {} within widget \"{}\", which is "
		"of type \"{}\".", alignment, id, widgetType);
}

// BUTTON //

void sfx::gui::_setWidgetDisabledBgColour(const sfx::WidgetIDRef id,
	const sf::Color& colour) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(BitmapButton,
			castWidget->getRenderer()->setBackgroundColorDisabled(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the disabled background colour \"{}\" to widget \"{}\", "
		"which is of type \"{}\".", colour, id, widgetType)
}

// RADIOBUTTON & CHECKBOX //

void sfx::gui::_setWidgetChecked(const sfx::WidgetIDRef id, const bool checked) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(RadioButton, castWidget->setChecked(checked);)
		ELSE_IF_WIDGET_IS(CheckBox, castWidget->setChecked(checked);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the check status to {} for widget \"{}\", which is of "
		"type \"{}\".", checked, id, widgetType);
}

bool sfx::gui::_isWidgetChecked(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(RadioButton, return castWidget->isChecked();)
		ELSE_IF_WIDGET_IS(CheckBox, return castWidget->isChecked();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the check status of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return false;
}

// LIST //

void sfx::gui::_addItem(const sfx::WidgetIDRef id, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(id)
		std::size_t index = 0;
		IF_WIDGET_IS(ListBox,
			const auto limit = castWidget->getMaximumItems();
			index = castWidget->addItem(text);
			if (limit > 0 && index == limit) {
				ERROR(std::string("This widget has reached its configured maximum "
					"number of items, which is ").append(std::to_string(limit)).
					append("."))
			}
		)
		ELSE_IF_WIDGET_IS(ComboBox,
			const auto limit = castWidget->getMaximumItems();
			index = castWidget->addItem(text);
			if (limit > 0 && index == limit) {
				ERROR(std::string("This widget has reached its configured maximum "
					"number of items, which is ").append(std::to_string(limit)).
					append("."))
			}
		)
		ELSE_UNSUPPORTED()
		_setTranslatedString(*widget, text, variables, index);
		_translateWidget(widget->ptr);
	END("Attempted to add an item \"{}\" to widget \"{}\", which is of type "
		"\"{}\".", text, id, widgetType)
	if (variables) variables->Release();
}

void sfx::gui::_clearItems(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox, castWidget->removeAllItems();)
		ELSE_IF_WIDGET_IS(ComboBox, castWidget->removeAllItems();)
		ELSE_IF_WIDGET_IS(TreeView, castWidget->removeAllItems();)
		ELSE_UNSUPPORTED()
		widget->originalCaption = {};
	END("Attempted to clear all items from widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
}

void sfx::gui::_setSelectedItem(const sfx::WidgetIDRef id,
	const std::size_t index) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox,
			if (!castWidget->setSelectedItemByIndex(index)) {
				const auto count = castWidget->getItemCount();
				if (count) {
					ERROR(std::string("The item index cannot be higher than ").
						append(std::to_string(count - 1)).append("."))
				} else {
					ERROR("This widget has no items.")
				}
			}
		)
		ELSE_IF_WIDGET_IS(ComboBox,
			if (!castWidget->setSelectedItemByIndex(index)) {
				const auto count = castWidget->getItemCount();
				if (count) {
					ERROR(std::string("The item index cannot be higher than ").
						append(std::to_string(count - 1)).append("."))
				} else {
					ERROR("This widget has no items.")
				}
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to select item {} from widget \"{}\", which is of type \"{}\".",
		index, id, widgetType)
}

void sfx::gui::_deselectItem(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox, castWidget->deselectItem();)
		ELSE_IF_WIDGET_IS(ComboBox, castWidget->deselectItem();)
		ELSE_IF_WIDGET_IS(TreeView, castWidget->deselectItem();)
		ELSE_UNSUPPORTED()
	END("Attempted to deselect the selected item of a widget \"{}\", which is of "
		"type \"{}\".", id, widgetType)
}

std::size_t sfx::gui::_getItemCount(const WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox, return castWidget->getItemCount();)
		ELSE_IF_WIDGET_IS(ComboBox, return castWidget->getItemCount();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the item count of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0;
}

int sfx::gui::_getSelectedItem(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItemIndex();)
		ELSE_IF_WIDGET_IS(ComboBox, return castWidget->getSelectedItemIndex();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the index of the selected item of a widget \"{}\", "
		"which is of type \"{}\".", id, widgetType)
	return -1;
}

std::string sfx::gui::_getSelectedItemText(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItem().toStdString();)
		ELSE_IF_WIDGET_IS(ComboBox,
			return castWidget->getSelectedItem().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of the selected item of a widget \"{}\", which "
		"is of type \"{}\".", id, widgetType)
	return "";
}

void sfx::gui::_setItemsToDisplay(const sfx::WidgetIDRef id,
	const std::size_t items) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ComboBox, castWidget->setItemsToDisplay(items);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the number of items to display to {} for widget \"{}\", "
		"which is of type \"{}\".", items, id, widgetType)
}

// TREEVIEW //

void sfx::gui::_setSelectedItemTextHierarchy(const sfx::WidgetIDRef id,
	const CScriptArray* const item) {
	const auto itemStl = // Frees item.
		engine::ConvertCScriptArray<std::vector<std::string>, std::string>(item);
	// Then, convert the vector to the correct type.
	std::vector<String> itemTgui;
	std::copy(itemStl.begin(), itemStl.end(), std::back_inserter(itemTgui));
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(TreeView, castWidget->selectItem(itemTgui);)
		ELSE_UNSUPPORTED()
	END("Attempted to select a TreeView item from widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
}

CScriptArray* sfx::gui::_getSelectedItemTextHierarchy(
	const sfx::WidgetIDRef id) const {
	auto const arr = _scripts->createArray("string");
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(TreeView,
			const auto item = castWidget->getSelectedItem();
			arr->Resize(static_cast<asUINT>(item.size()));
			asUINT i = 0;
			for (const auto& parent : item)
				arr->SetValue(i++, &parent.toStdString());
		)
		ELSE_UNSUPPORTED()
	END("Attempted to get the hierarchy of the selected item of a widget \"{}\", "
		"which is of type \"{}\".", id, widgetType)
	return arr;
}

void sfx::gui::_addTreeViewItem(const sfx::WidgetIDRef id,
	const CScriptArray* const hierarchy) {
	START_WITH_WIDGET(id)
		if (!hierarchy) ERROR("No item hierarchy was given!")
		IF_WIDGET_IS(TreeView,
			std::vector<String> newItem;
			for (asUINT i = 0, len = hierarchy->GetSize(); i < len; ++i) {
				newItem.emplace_back(*static_cast<const std::string* const>(
					hierarchy->At(i)));
			}
			castWidget->addItem(newItem, true);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a TreeView item to widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	if (hierarchy) hierarchy->Release();
}

// TABS //

void sfx::gui::_addTab(const sfx::WidgetIDRef id, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(id)
		std::size_t index = 0;
		IF_WIDGET_IS(Tabs, index = castWidget->add(text, false);)
		ELSE_UNSUPPORTED()
		_setTranslatedString(*widget, text, variables, index);
		_translateWidget(widget->ptr);
	END("Attempted to add a tab \"{}\" to widget \"{}\", which is of type \"{}\".",
		text, id, widgetType)
	if (variables) variables->Release();
}

void sfx::gui::_setSelectedTab(const sfx::WidgetIDRef id,
	const std::size_t index) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs,
			const auto prevSelected = castWidget->getSelectedIndex();
			if (!castWidget->select(index)) {
				if (prevSelected >= 0) castWidget->select(prevSelected);
				const auto count = castWidget->getTabsCount();
				if (count == 0) {
					ERROR("This widget has no items.")
				} else if (count <= index) {
					ERROR(std::string("The item index cannot be higher than ").
						append(std::to_string(count - 1)).append("."))
				}
				const auto disabled = !castWidget->getTabEnabled(index);
				const auto invisible = !castWidget->getTabVisible(index);
				if (invisible && disabled) {
					ERROR("This tab is invisible and disabled.")
				} else if (invisible) {
					ERROR("This tab is invisible.")
				} else if (disabled) {
					ERROR("This tab is disabled.")
				}
			}
		)
		ELSE_IF_WIDGET_IS(TabContainer,
			const auto panelCount = castWidget->getPanelCount();
			if (panelCount <= index) ERROR("Given tab index was too high! Max is "
				+ std::to_string(panelCount) + "!")
			castWidget->select(index);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to select tab {} from widget \"{}\", which is of type \"{}\".",
		index, id, widgetType)
}

void sfx::gui::_deselectTab(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs, castWidget->deselect();)
		ELSE_IF_WIDGET_IS(TabContainer, castWidget->deselect();)
		ELSE_UNSUPPORTED()
	END("Attempted to deselect the selected tab of a widget \"{}\", which is of "
		"type \"{}\".", id, widgetType)
}

int sfx::gui::_getSelectedTab(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs, return castWidget->getSelectedIndex();)
		ELSE_IF_WIDGET_IS(TabContainer, return castWidget->getSelectedIndex();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the index of the selected tab of a widget \"{}\", which "
		"is of type \"{}\".", id, widgetType)
	return -1;
}

void sfx::gui::_setTabEnabled(const sfx::WidgetIDRef id, const std::size_t i,
	const bool enabled) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs,
			if (i >= castWidget->getTabsCount()) ERROR("This tab does not exist!")
			castWidget->setTabEnabled(i, enabled);)
		ELSE_IF_WIDGET_IS(TabContainer,
			if (i >= castWidget->getTabs()->getTabsCount())
				ERROR("This tab does not exist!")
			castWidget->getTabs()->setTabEnabled(i, enabled);)
		ELSE_UNSUPPORTED()
	END("Attempted to set tab {}'s enabled state to {} for a widget \"{}\", which "
		"is of type \"{}\".", i, enabled, id, widgetType)
}

bool sfx::gui::_getTabEnabled(const sfx::WidgetIDRef id, const std::size_t i) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs,
			if (i >= castWidget->getTabsCount()) ERROR("This tab does not exist!")
			return castWidget->getTabEnabled(i);)
		ELSE_IF_WIDGET_IS(TabContainer,
			if (i >= castWidget->getTabs()->getTabsCount())
				ERROR("This tab does not exist!")
			return castWidget->getTabs()->getTabEnabled(i);)
		ELSE_UNSUPPORTED()
	END("Attempted to get tab {}'s enabled state for a widget \"{}\", which is of "
		"type \"{}\".", i, id, widgetType)
	return false;
}

std::size_t sfx::gui::_getTabCount(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs, return castWidget->getTabsCount();)
		ELSE_IF_WIDGET_IS(TabContainer,
			return castWidget->getTabs()->getTabsCount();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the tab count of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0;
}

std::string sfx::gui::_getTabText(const sfx::WidgetIDRef id,
	const std::size_t index) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Tabs, return castWidget->getText(index).toStdString();)
		ELSE_IF_WIDGET_IS(TabContainer,
			return castWidget->getTabText(index).toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the translated text of tab number {} of a widget "
		"\"{}\", which is of type \"{}\".", index, id, widgetType)
	return "";
}

// CONTAINER //

void sfx::gui::_add(const sfx::WidgetIDRef p, const sfx::WidgetIDRef c) {
	START_WITH_WIDGET(c)
		if (_getWidgetID(widget->ptr) == sfx::ROOT_WIDGET)
			ERROR("You cannot add the root widget to a container!");
		const auto containerData = _findWidget(p);
		if (containerData == _widgets.end())
			ERROR("The given container does not exist!")
		if (!containerData->ptr->isContainer())
			ERROR("The given container widget is not a container!");
		// Do not allow the scripts to add to a TabContainer directly, as this will
		// play havoc with resource management if a TabContainer is deleted.
		if (containerData->ptr->getWidgetType() == type::TabContainer)
			ERROR("Adding a widget to a TabContainer is forbidden!");
		// If the given widget is already attached to a parent, remove it
		// explicitly first.
		if (containerID != sfx::NO_WIDGET)
			_removeWidgetFromParent(*container, *widget);
		_addWidgetToParent(*containerData, *widget);
		// If the widget was added to the root container directly, make it
		// invisible.
		if (_getWidgetID(containerData->ptr) == sfx::ROOT_WIDGET)
			widget->ptr->setVisible(false);
	END("Attempted to add widget \"{}\", which is of type \"{}\", to container "
		"\"{}\".", c, widgetType, p);
}

void sfx::gui::_remove(const sfx::WidgetIDRef c) {
	START_WITH_WIDGET(c)
		if (containerID == sfx::NO_WIDGET)
			ERROR("This widget does not have a parent!");
		if (container->ptr->getWidgetType() == type::TabContainer &&
			widgetType == type::Panel) {
			_logger.warning("Panel \"{}\" cannot be directly removed from "
				"TabContainer \"{}\"! Using removeTabAndPanel() instead...", c,
				containerID);
			_removeTabAndPanel(c);
		} else _removeWidgetFromParent(*container, *widget);
	END("Attempted to remove widget \"{}\", which is of type \"{}\", from its "
		"parent.", c, widgetType);
}

void sfx::gui::_removeAll(const sfx::WidgetIDRef p) {
	START_WITH_WIDGET(p)
		if (!widget->ptr->isContainer() || widgetType == type::TabContainer)
			UNSUPPORTED_WIDGET_TYPE()
		const std::vector<Widget::Ptr> children =
			widget->castPtr<Container>()->getWidgets();
		for (const auto& child : children) _removeWidgetFromParent(*widget,
			*_findWidget(_getWidgetID(child)));
	END("Attempted to remove all widgets from the container \"{}\", which is of "
		"type \"{}\".", p, widgetType);
}

void sfx::gui::_setWidgetIndexInContainer(const sfx::WidgetIDRef id,
	const std::size_t oldIndex, const std::size_t newIndex) {
	START_WITH_WIDGET(id)
		if (!widget->ptr->isContainer()) UNSUPPORTED_WIDGET_TYPE()
		Widget::Ptr w;
		const auto containerPtr = widget->castPtr<Container>();
		try {
			w = containerPtr->getWidgets().at(oldIndex);
		} catch (const std::out_of_range&) {
			ERROR("This container does not have a widget with that number.")
		}
		if (!containerPtr->setWidgetIndex(w, newIndex)) {
			const auto count = containerPtr->getWidgets().size();
			if (count) {
				ERROR(std::string("The new index cannot be higher than ").append(
					std::to_string(count - 1)).append("."))
			} else {
				ERROR("This container has no widgets.")
			}
		}
	END("Attempted to set the widget \"{}\"'s number {} widget to an index of {}. "
		"The widget is of type \"{}\".", id, oldIndex, newIndex, widgetType)
}

std::size_t sfx::gui::_getWidgetCount(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		if (widget->ptr->isContainer()) {
			return widget->castPtr<Container>()->getWidgets().size();
		} else UNSUPPORTED_WIDGET_TYPE()
	END("Attempted to get the widget count of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0;
}

void sfx::gui::_setGroupPadding(const sfx::WidgetIDRef id,
	const std::string& padding) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(Panel, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(HorizontalLayout, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(VerticalLayout, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(Group, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(HorizontalWrap, castWidget->getRenderer()->setPadding(
			AbsoluteOrRelativeValue(padding));)
		ELSE_IF_WIDGET_IS(Grid,
			const auto& widgets = castWidget->getWidgets();
			for (const auto& widget : widgets) {
				castWidget->setWidgetPadding(widget,
					AbsoluteOrRelativeValue(padding));
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set a padding {} to widget \"{}\", which is of type \"{}\".",
		padding, id, widgetType)
}

void sfx::gui::_setGroupPadding(const sfx::WidgetIDRef id, const std::string& left,
	const std::string& top, const std::string& right, const std::string& bottom) {
	const auto padding = Padding(AbsoluteOrRelativeValue(left),
		AbsoluteOrRelativeValue(top), AbsoluteOrRelativeValue(right),
		AbsoluteOrRelativeValue(bottom));
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(HorizontalLayout,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(VerticalLayout,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(Group,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(HorizontalWrap,
			castWidget->getRenderer()->setPadding(padding);)
		ELSE_IF_WIDGET_IS(Grid,
			const auto & widgets = castWidget->getWidgets();
			for (const auto& widget : widgets)
				castWidget->setWidgetPadding(widget, padding);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set padding left:{}, top:{}, right:{}, bottom:{}, to widget "
		"\"{}\", which is of type \"{}\".", left, top, right, bottom, id,
		widgetType)
}

void sfx::gui::_applySpritesToWidgetsInContainer(const sfx::WidgetIDRef id,
	const std::string& spritesheet, const CScriptArray* const sprites) {
	std::size_t spritesCount = 0;
	START_WITH_WIDGET(id)
		if (!sprites) ERROR("No sprites given!")
		if (!widget->ptr->isContainer()) UNSUPPORTED_WIDGET_TYPE()
		const auto& widgets = widget->castPtr<Container>()->getWidgets();
		spritesCount = sprites->GetSize();
		asUINT counter = 0;
		for (const auto& w : widgets) {
			if (w->getWidgetType() == type::BitmapButton ||
				w->getWidgetType() == type::Picture) {
				_applySprite(*_findWidget(w->getUserData<sfx::WidgetID>()),
					spritesheet,
					*static_cast<const std::string*>(sprites->At(counter++)));
				if (counter >= spritesCount) break;
			}
		}
	END("Attempted to apply {} sprites from spritesheet \"{}\", to widget \"{}\", "
		"which is of type \"{}\".", spritesCount, spritesheet, id, widgetType)
	if (sprites) sprites->Release();
}

// PANEL //

void sfx::gui::_setWidgetBgColour(const sfx::WidgetIDRef id,
	const sf::Color& colour) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_IF_WIDGET_IS(ScrollablePanel,
			castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_IF_WIDGET_IS(BitmapButton,
			castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the background colour \"{}\" to widget \"{}\", which is "
		"of type \"{}\".", colour, id, widgetType)
}

void sfx::gui::_setWidgetBorderSize(const sfx::WidgetIDRef id, const float size) {
	START_WITH_WIDGET(id);
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBorders(size);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border size of {} to widget \"{}\", which is of type "
		"\"{}\".", size, id, widgetType)
}

void sfx::gui::_setWidgetBorderSizes(const sfx::WidgetIDRef id,
	const std::string& left, const std::string& top, const std::string& right,
	const std::string& bottom) {
	const auto borders = Borders(AbsoluteOrRelativeValue(left),
		AbsoluteOrRelativeValue(top), AbsoluteOrRelativeValue(right),
		AbsoluteOrRelativeValue(bottom));
	START_WITH_WIDGET(id);
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBorders(borders);)
		ELSE_UNSUPPORTED()
	END("Attempted to set border sizes left:{}, top:{}, right:{}, bottom:{}, to "
		"widget \"{}\", which is of type \"{}\".", left, top, right, bottom, id,
		widgetType)
}

void sfx::gui::_setWidgetBorderColour(const sfx::WidgetIDRef id,
	const sf::Color& colour) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setBorderColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border colour of {} to widget \"{}\", which is of "
		"type \"{}\".", colour, id, widgetType)
}

void sfx::gui::_setWidgetBorderRadius(const sfx::WidgetIDRef id,
	const float radius) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setRoundedBorderRadius(radius);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the border radius {} to widget \"{}\", which is of type "
		"\"{}\".", radius, id, widgetType)
}

void sfx::gui::_setHorizontalScrollbarPolicy(const sfx::WidgetIDRef id,
	const tgui::Scrollbar::Policy policy) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollbarPolicy(policy);)
		ELSE_IF_WIDGET_IS(TextArea,
			castWidget->setHorizontalScrollbarPolicy(policy);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar policy {} to widget \"{}\", "
		"which is of type \"{}\".", policy, id, widgetType)
}

void sfx::gui::_setHorizontalScrollbarAmount(const sfx::WidgetIDRef id,
	const unsigned int amount) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\".", amount, id, widgetType)
}

bool sfx::gui::_isHorizontalScrollbarVisible(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			return castWidget->isHorizontalScrollbarShown();)
		ELSE_UNSUPPORTED()
	END("Attempted to get widget \"{}\"'s horizontal scrollbar visibility state. "
		"The widget is of type \"{}\".", id, widgetType)
	return false;
}

void sfx::gui::_setVerticalScrollbarAmount(const sfx::WidgetIDRef id,
	const unsigned int amount) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setVerticalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the vertical scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\".", amount, id, widgetType)
}

void sfx::gui::_setVerticalScrollbarValue(const sfx::WidgetIDRef id,
	const unsigned int value) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			const auto max =
				static_cast<unsigned int>(castWidget->getContentSize().y);
			if (value > max) castWidget->setVerticalScrollbarValue(max);
			else castWidget->setVerticalScrollbarValue(value);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the vertical scrollbar value {} to widget \"{}\", which "
		"is of type \"{}\".", value, id, widgetType)
}

bool sfx::gui::_isVerticalScrollbarVisible(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel,
			return castWidget->isVerticalScrollbarShown();)
		ELSE_UNSUPPORTED()
	END("Attempted to get widget \"{}\"'s vertical scrollbar visibility state. "
		"The widget is of type \"{}\".", id, widgetType)
	return false;
}

float sfx::gui::_getScrollbarWidth(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ScrollablePanel, return castWidget->getScrollbarWidth();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the scrollbar width of widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0.0f;
}

// LAYOUT //

void sfx::gui::_setWidgetRatioInLayout(const sfx::WidgetIDRef id,
	const std::size_t index, const float ratio) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(HorizontalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_IF_WIDGET_IS(VerticalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_UNSUPPORTED()
	END("Attempted to set the widget ratio {} to widget {} in widget \"{}\", "
		"which is of type \"{}\".", ratio, index, id, widgetType)
}

void sfx::gui::_setSpaceBetweenWidgets(const sfx::WidgetIDRef id,
	const float space) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(HorizontalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_IF_WIDGET_IS(VerticalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_IF_WIDGET_IS(HorizontalWrap, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_UNSUPPORTED()
	END("Attempted to set {} to a widget \"{}\"'s space between widgets property. "
		"The widget is of type \"{}\".", space, id, widgetType)
}

// GRID //

void sfx::gui::_addWidgetToGrid(const sfx::WidgetIDRef id,
	const sfx::WidgetIDRef childId, const std::size_t row,
	const std::size_t col) {
	START_WITH_WIDGET(childId)
		if (_getWidgetID(widget->ptr) == sfx::ROOT_WIDGET)
			ERROR("You cannot add the root widget to a grid!");
		const auto grid = _findWidget(id);
		if (grid == _widgets.end()) ERROR("The given grid does not exist!")
		if (grid->ptr->getWidgetType() != type::Grid)
			ERROR(std::string("The given grid is of type \"").append(
				grid->ptr->getWidgetType().toStdString()).append("\"."));
		// If the given widget is already attached to a parent, remove it
		// explicitly first.
		if (containerID != sfx::NO_WIDGET)
			_removeWidgetFromParent(*container, *widget);
		_addWidgetToGrid(*grid, *widget, row, col);
	END("Attempted to add widget \"{}\", which is of type \"{}\", to grid \"{}\".",
		childId, widgetType, id);
}

void sfx::gui::_setWidgetAlignmentInGrid(const sfx::WidgetIDRef id,
	const std::size_t row, const std::size_t col,
	const tgui::Grid::Alignment alignment) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Grid,
			auto& table = castWidget->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size())
					castWidget->setWidgetAlignment(row, col, alignment);
				else ERROR("The column index is out of range.")
			} else ERROR("The row index is out of range.")
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set an alignment {} to a widget \"{}\", which is of type "
		"\"{}\", @ ({}, {}).", alignment, id, widgetType, row, col)
}

void sfx::gui::_setWidgetPaddingInGrid(const sfx::WidgetIDRef id,
	const std::size_t row, const std::size_t col, const std::string& padding) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Grid,
			auto& table = castWidget->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size()) castWidget->setWidgetPadding(row, col,
					AbsoluteOrRelativeValue(padding));
				else ERROR("The column index is out of range.")
			} else ERROR("The row index is out of range.")
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set a padding {} to a widget \"{}\", which is of type "
		"\"{}\", @ ({}, {}).", padding, id, widgetType, row, col)
}

std::size_t sfx::gui::_getWidgetColumnCount(const WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Grid,
			// There has to be a fancy ranges implmentation for this.
			std::size_t maxColumns = 0;
			for (const auto& row : castWidget->getGridWidgets())
				if (row.size() > maxColumns) maxColumns = row.size();
			return maxColumns;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to get the column count of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0;
}

std::size_t sfx::gui::_getWidgetRowCount(const WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Grid, return castWidget->getGridWidgets().size();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the row count of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
	return 0;
}

// MENUS //

sfx::MenuItemID sfx::gui::_addMenu(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(id)
		if (!_isLoading) ERROR("This function cannot be called outside of a "
			"Menu's constructor!");
		IF_WIDGET_IS(MenuBar,
			if (widget->hierarchyOfLastMenuItem.size() == 1) {
				_logger.warning("Menu \"{}\" in MenuBar \"{}\" is empty!",
					widget->hierarchyOfLastMenuItem[0], id);
			}
			castWidget->addMenu(text);
			widget->hierarchyOfLastMenuItem = { text };
			_setTranslatedString(*widget, text, variables, widget->menuCounter);
			ret = widget->menuCounter++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu \"{}\" to a widget \"{}\", which is of type "
		"\"{}\".", text, id, widgetType)
	if (variables) variables->Release();
	return ret;
}

sfx::MenuItemID sfx::gui::_addMenuItem(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(id)
		if (!_isLoading) ERROR("This function cannot be called outside of a "
			"Menu's constructor!");
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = widget->hierarchyOfLastMenuItem;
			auto copy(hierarchy);
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() == 1) {
				hierarchy.push_back(text);
			} else {
				hierarchy.back() = text;
			}
			if (!castWidget->addMenuItem(hierarchy)) {
				std::string error = "Could not add item with hierarchy: ";
				for (sfx::MenuItemID i = 0, len = hierarchy.size(); i < len; ++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy = copy;
				ERROR(error);
			}
			_setTranslatedString(*widget, text, variables, widget->menuCounter);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, id,
				widget->menuCounter);
			ret = widget->menuCounter++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu item \"{}\" to a widget \"{}\", which is of "
		"type \"{}\".", text, id, widgetType)
	if (variables) variables->Release();
	return ret;
}

sfx::MenuItemID sfx::gui::_addMenuItemIntoLastItem(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(id)
		if (!_isLoading) ERROR("This function cannot be called outside of a "
			"Menu's constructor!");
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = widget->hierarchyOfLastMenuItem;
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() == 1) {
				_logger.warning("Calling addMenuItemIntoLastItem() when "
					"addMenuItem() was likely intended.");
			}
			hierarchy.push_back(text);
			if (!castWidget->addMenuItem(hierarchy)) {
				std::string error = "Could not add item with hierarchy: ";
				for (sfx::MenuItemID i = 0, len = hierarchy.size(); i < len; ++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy.pop_back();
				ERROR(error);
			}
			_setTranslatedString(*widget, text, variables, widget->menuCounter);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, id,
				widget->menuCounter);
			ret = widget->menuCounter++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to create a new submenu with item \"{}\" in a widget \"{}\", "
		"which is of type \"{}\".", text, id, widgetType)
	if (variables) variables->Release();
	return ret;
}

void sfx::gui::_exitSubmenu(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		if (!_isLoading) ERROR("This function cannot be called outside of a "
			"Menu's constructor!");
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = widget->hierarchyOfLastMenuItem;
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() < 3) {
				ERROR("Not currently in a submenu!");
			}
			hierarchy.pop_back();
		)
		ELSE_UNSUPPORTED()
	END("Attempted to exit the current submenu of widget \"{}\", which is of type "
		"\"{}\".", id, widgetType)
}

sfx::MenuItemID sfx::gui::_getLastSelectedMenuItem(
	const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(MenuBar, return widget->lastMenuItemClicked;)
		ELSE_UNSUPPORTED()
	END("Attempted to get the ID of the last selected menu item of widget \"{}\", "
		"which is of type \"{}\".", id, widgetType)
	return sfx::NO_MENU_ITEM_ID;
}

// CHILDWINDOW //

void sfx::gui::_autoHandleMinMax(const sfx::WidgetIDRef id, const bool handle) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			if (handle) widget->childWindowData =
				sfx::gui::child_window_properties{};
			else widget->childWindowData = std::nullopt;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the autoHandleMinMax property to {}, for the widget "
		"\"{}\", which is of type \"{}\".", handle, id, widgetType);
}

void sfx::gui::_setChildWindowTitleButtons(const sfx::WidgetIDRef id,
	const unsigned int buttons) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, castWidget->setTitleButtons(buttons);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the titlebar button mask {} to the widget \"{}\", which "
		"is of type \"{}\".", buttons, id, widgetType)
}

void sfx::gui::_setWidgetResizable(const sfx::WidgetIDRef id,
	const bool resizable) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, castWidget->setResizable(resizable);)
		ELSE_IF_WIDGET_IS(FileDialog, castWidget->setResizable(resizable);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the resizability property of widget \"{}\", which is of "
		"type \"{}\", to {}.", id, widgetType, resizable);
}

void sfx::gui::_setWidgetPositionLocked(const sfx::WidgetIDRef id,
	const bool locked) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, castWidget->setPositionLocked(locked);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the position locked property of widget \"{}\", which is "
		"of type \"{}\", to {}.", id, widgetType, locked);
}

float sfx::gui::_getTitleBarHeight(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_IF_WIDGET_IS(FileDialog,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_IF_WIDGET_IS(MessageBox,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the titlebar height of a widget \"{}\", which is of "
		"type \"{}\".", id, widgetType);
	return 0.0f;
}

CScriptArray* sfx::gui::_getBorderWidths(const sfx::WidgetIDRef id) const {
	auto arr = _scripts->createArray("float");
	arr->Resize(4);
	static const auto fromBorders = [arr](const Borders& borders) {
		auto temp = borders.getLeft(); arr->SetValue(0, &temp);
		      temp = borders.getTop(); arr->SetValue(1, &temp);
		    temp = borders.getRight(); arr->SetValue(2, &temp);
		   temp = borders.getBottom(); arr->SetValue(3, &temp);
	};
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_IF_WIDGET_IS(FileDialog,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_IF_WIDGET_IS(MessageBox,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_UNSUPPORTED()
	END("Attempted to get the border widths of a widget \"{}\", which is of type "
		"\"{}\".", id, widgetType);
	return arr;
}

void sfx::gui::_openChildWindow(const sfx::WidgetIDRef id, const std::string& x,
	const std::string& y) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			if (widget->childWindowData) _restoreChildWindowImpl(id, *widget);
			castWidget->setPosition(x.c_str(), y.c_str());
			castWidget->moveToFront();
			castWidget->setVisible(true);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to open the widget \"{}\", which is of type \"{}\".", id,
		widgetType);
}

void sfx::gui::_closeChildWindow(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, castWidget->setVisible(false);)
		ELSE_UNSUPPORTED()
	END("Attempted to close the widget \"{}\", which is of type \"{}\".", id,
		widgetType);
}

void sfx::gui::_closeChildWindowAndEmitSignal(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, castWidget->close();)
		ELSE_UNSUPPORTED()
	END("Attempted to close the widget \"{}\", which is of type \"{}\", and emit "
		"the onClosing signal.", id, widgetType);
}

void sfx::gui::_restoreChildWindow(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			if (widget->childWindowData) _restoreChildWindowImpl(id, *widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to restore the widget \"{}\", which is of type \"{}\".", id,
		widgetType);
}

void sfx::gui::_restoreChildWindowImpl(const WidgetIDRef widgetID,
	sfx::gui::widget_data& widgetData) {
	auto& data = *widgetData.childWindowData;
	const sfx::gui::WidgetCollection::iterator parentData =
		(widgetData.ptr->getParent() ?
		_findWidget(_getWidgetID(widgetData.ptr->getParent())) : _widgets.end());
	if (data.isMinimised || data.isMaximised) {
		if (data.isMinimised && parentData != _widgets.end())
			parentData->minimisedChildWindowList.restore(widgetID);
		data.restore(widgetData.castPtr<ChildWindow>());
		data.isMinimised = false;
		data.isMaximised = false;
	}
}

void sfx::gui::_maximiseChildWindow(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow,
			if (widget->childWindowData) {
				_restoreChildWindowImpl(id, *widget);
				_maximiseChildWindowImpl(castWidget, *widget);
				widget->ptr->moveToFront();
				widget->ptr->setVisible(true);
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to maximise the widget \"{}\", which is of type \"{}\".", id,
		widgetType);
}

void sfx::gui::_maximiseChildWindowImpl(const tgui::ChildWindow::Ptr& window,
	sfx::gui::widget_data& widgetData) {
	// NOTE: this assumes a restored child window!
	auto& data = *widgetData.childWindowData;
	data.cache(window);
	data.isMinimised = false;
	data.isMaximised = true;
	window->setSize("100%", "100%");
	window->setPosition("50%", "50%");
	window->setOrigin(0.5f, 0.5f);
	window->setResizable(false);
	window->setPositionLocked(true);
}

bool sfx::gui::_isChildWindowOpen(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(ChildWindow, return castWidget->isVisible();)
		ELSE_UNSUPPORTED()
	END("Attempted to query if a widget \"{}\", which is of type \"{}\", is open.",
		id, widgetType);
	return false;
}

void sfx::gui::_connectChildWindowClosingSignalHandler(const sfx::WidgetIDRef id,
	asIScriptFunction* const handler) {
	START_WITH_WIDGET(id)
		if (widgetType != type::ChildWindow && widgetType != type::MessageBox &&
			widgetType != type::FileDialog && widgetType != type::ColorPicker)
			UNSUPPORTED_WIDGET_TYPE()
		if (handler) widget->childWindowClosingHandler =
			std::make_unique<engine::CScriptWrapper<asIScriptFunction>>(handler);
		else widget->childWindowClosingHandler = nullptr;
	END("Attempted to connect a handler to the \"{}\" signal for widget with ID "
		"\"{}\", which is of type \"{}\".", signal::Closing, id, widgetType);
	if (handler) handler->Release();
}

// FILEDIALOG //

void sfx::gui::_setFileDialogStrings(const sfx::WidgetIDRef id,
	const std::string& title, CScriptArray* const v0,
	const std::string& confirm, CScriptArray* const v1,
	const std::string& cancel, CScriptArray* const v2,
	const std::string& createFolder, CScriptArray* const v3,
	const std::string& filenameLabel, CScriptArray* const v4,
	const std::string& nameColumn, CScriptArray* const v5,
	const std::string& sizeColumn, CScriptArray* const v6,
	const std::string& modifyColumn, CScriptArray* const v7,
	const std::string& allFiles, CScriptArray* const v8) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog,
			_setTranslatedString(*widget, title, v0, 0);
			_setTranslatedString(*widget, confirm, v1, 1);
			_setTranslatedString(*widget, cancel, v2, 2);
			_setTranslatedString(*widget, createFolder, v3, 3);
			_setTranslatedString(*widget, filenameLabel, v4, 4);
			_setTranslatedString(*widget, nameColumn, v5, 5);
			_setTranslatedString(*widget, sizeColumn, v6, 6);
			_setTranslatedString(*widget, modifyColumn, v7, 7);
			_setTranslatedString(*widget, allFiles, v8, 8);
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file dialog strings for widget \"{}\", which is of "
		"type \"{}\".", id, widgetType);
	if (v0) v0->Release();
	if (v1) v1->Release();
	if (v2) v2->Release();
	if (v3) v3->Release();
	if (v4) v4->Release();
	if (v5) v5->Release();
	if (v6) v6->Release();
	if (v7) v7->Release();
	if (v8) v8->Release();
}

CScriptArray* sfx::gui::_getFileDialogSelectedPaths(
	const sfx::WidgetIDRef id) const {
	auto ret = _scripts->createArray("string");
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog,
			const auto& paths = castWidget->getSelectedPaths();
			for (const auto& path : paths)
				ret->InsertLast(&path.asString().toStdString());
		)
		ELSE_UNSUPPORTED()
	END("Attempted to get the selected paths from widget \"{}\", which is of type "
		"\"{}\".", id, widgetType);
	return ret;
}

void sfx::gui::_addFileDialogFileTypeFilter(const sfx::WidgetIDRef id,
	const std::string& caption, CScriptArray* const variables,
	CScriptArray* const filters) {
	std::vector<String> expressions;
	if (filters) {
		for (asUINT i = 0, len = filters->GetSize(); i < len; ++i)
			expressions.emplace_back(*static_cast<std::string*>(filters->At(i)));
		filters->Release();
	}
	std::vector<std::pair<String, std::vector<String>>> f;
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog,
			f = castWidget->getFileTypeFilters();
			f.emplace_back(caption, expressions);
			castWidget->setFileTypeFilters(f);
			_setTranslatedString(*widget, caption, variables, f.size() + 7);
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file type filters of widget \"{}\", which is of "
		"type \"{}\".", id, widgetType);
	if (variables) variables->Release();
}

void sfx::gui::_clearFileDialogFileTypeFilters(const sfx::WidgetIDRef id) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog,
			castWidget->setFileTypeFilters({});
			std::get<sfx::gui::ListOfCaptions>(widget->originalCaption).resize(9);
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to clear the file type filters of widget \"{}\", which is of "
		"type \"{}\".", id, widgetType);
}

void sfx::gui::_setFileDialogFileMustExist(const sfx::WidgetIDRef id,
	const bool mustExist) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog, castWidget->setFileMustExist(mustExist);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file must exist property to {}, for widget \"{}\", "
		"which is of type \"{}\".", mustExist, id, widgetType);
}

void sfx::gui::_setFileDialogDefaultFileFilter(const sfx::WidgetIDRef id,
	const std::size_t index) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog,
			const auto copy = castWidget->getFileTypeFilters();
			castWidget->setFileTypeFilters(copy, index);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the default file filter to {}, for widget \"{}\", which "
		"is of type \"{}\".", index, id, widgetType);
}

void sfx::gui::_setFileDialogPath(const sfx::WidgetIDRef id,
	const std::string& path) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(FileDialog, castWidget->setPath(path);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the current path of \"{}\", to widget \"{}\", which "
		"is of type \"{}\".", path, id, widgetType);
}

// MESSAGEBOX //

void sfx::gui::_setMessageBoxStrings(const sfx::WidgetIDRef id,
	const std::string& title, CScriptArray* const titleVars,
	const std::string& text, CScriptArray* const textVars) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(MessageBox,
			_setTranslatedString(*widget, title, titleVars, 0);
			_setTranslatedString(*widget, text, textVars, 1);
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the title \"{}\" and text \"{}\" to widget \"{}\", "
		"which is of type \"{}\".", title, text, id, widgetType)
	if (titleVars) titleVars->Release();
	if (textVars) textVars->Release();
}

void sfx::gui::_addMessageBoxButton(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const variables) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(MessageBox,
			_setTranslatedString(*widget, text, variables,
				castWidget->getButtons().size() + 2);
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a button \"{}\" to widget \"{}\", which is of type "
		"\"{}\".", text, id, widgetType)
	if (variables) variables->Release();
}

std::size_t sfx::gui::_getLastSelectedButton(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(MessageBox, return widget->lastMessageBoxButtonClicked;)
		ELSE_UNSUPPORTED()
	END("Attempted to get the ID of the last selected button of widget \"{}\", "
		"which is of type \"{}\".", id, widgetType)
	return sfx::NO_MENU_ITEM_ID;
}

// TABCONTAINER //

sfx::WidgetID sfx::gui::_addTabAndPanel(const sfx::WidgetIDRef id,
	const std::string& text, CScriptArray* const vars) {
	auto panelID = sfx::NO_WIDGET;
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(TabContainer,
			const auto panel = castWidget->addTab(text, false);
			if (!panel) ERROR("Could not create panel!");
			panelID = _storeWidget(panel);
			_setTranslatedString(*widget, text, vars,
				static_cast<std::size_t>(castWidget->getIndex(panel)));
			_translateWidget(widget->ptr);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a tab \"{}\" to widget \"{}\", which is of type \"{}\".",
		text, id, widgetType)
	if (vars) vars->Release();
	return panelID;
}

bool sfx::gui::_removeTabAndPanel(const sfx::WidgetIDRef id) {
	std::size_t i = 0;
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(Panel,
			if (container->ptr->getWidgetType() != type::TabContainer)
				ERROR("The parent of the given panel is of type \"" +
					container->ptr->getWidgetType() + "\", not \"" +
					type::TabContainer + "\"!");
			const auto tabContainer = container->castPtr<TabContainer>();
			i = tabContainer->getIndex(castWidget);
			if (i < 0) ERROR("Could not find given panel in the tab container!");
			// Since we want to keep this TabContainer around, do not invalidate
			// its state by invoking _deleteWidget() before removeTab()!
			tabContainer->removeTab(i);
			_deleteWidget(id);
			// Remove tab's caption from the translation map.
			auto& captions = std::get<sfx::gui::ListOfCaptions>(
				container->originalCaption);
			captions.erase(captions.begin() + i);
			return true;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to remove a tab and panel, the latter with ID \"{}\", which is "
		"of type \"{}\".", id, widgetType)
	return false;
}

// SPINCONTROL & SLIDER //

void sfx::gui::_setWidgetMinMaxValues(const sfx::WidgetIDRef id, const float min,
	const float max) {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(SpinControl,
			castWidget->setMinimum(min);
			castWidget->setMaximum(max);
		)
		ELSE_IF_WIDGET_IS(Slider,
			castWidget->setMinimum(min);
			castWidget->setMaximum(max);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the minimum value ({}) and maximum value ({}) of a "
		"widget \"{}\", which is of type \"{}\".", min, max, id, widgetType)
}

bool sfx::gui::_setWidgetValue(const sfx::WidgetIDRef id, float val) {
	static const std::string errorString = "Attempted to set the value {} to a "
		"widget \"{}\", which is of type \"{}\".";
	bool ret = false;
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(SpinControl,
			const auto min = castWidget->getMinimum();
			const auto max = castWidget->getMaximum();
			if (val < min) {
				_logger.warning(errorString + " Value is smaller than the "
					"minimum, which is {}. The minimum value will be applied.",
					val, id, widgetType, min);
				val = min;
			} else if (val > max) {
				_logger.warning(errorString + " Value is greater than the "
					"maximum, which is {}. The maximum value will be applied.",
					val, id, widgetType, max);
				val = max;
			} else ret = true;
			const auto result = castWidget->setValue(val);
			if (ret && !result) ret = false;
		)
		ELSE_IF_WIDGET_IS(Slider,
			const auto min = castWidget->getMinimum();
			const auto max = castWidget->getMaximum();
			if (val < min) {
				_logger.warning(errorString + " Value is smaller than the "
					"minimum, which is {}. The minimum value will be applied.",
					val, id, widgetType, min);
				val = min;
			} else if (val > max) {
				_logger.warning(errorString + " Value is greater than the "
					"maximum, which is {}. The maximum value will be applied.",
					val, id, widgetType, max);
				val = max;
			} else ret = true;
			castWidget->setValue(val);
		)
		ELSE_UNSUPPORTED()
	END(errorString, val, id, widgetType)
	return ret;
}

float sfx::gui::_getWidgetValue(const sfx::WidgetIDRef id) const {
	START_WITH_WIDGET(id)
		IF_WIDGET_IS(SpinControl, return castWidget->getValue();)
		IF_WIDGET_IS(Slider, return castWidget->getValue();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the value of a widget \"{}\", which is of type \"{}\".",
		id, widgetType)
	return 0.f;
}
