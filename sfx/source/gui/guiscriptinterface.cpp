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

// NON-WIDGET //

void sfx::gui::_setGUI(const std::string& name) {
	setGUI(name, true, true);
}

bool sfx::gui::_menuExists(const std::string& menu) {
	// More efficient implementation would just cache the menu list, as menus can
	// only be added or removed via load().
	const auto& menus = _gui.getWidgets();
	for (const auto& widget : menus)
		if (widget->getWidgetName() == menu) return true;
	return false;
}

void sfx::gui::_noBackground(std::string menu) {
	if (menu == "") menu = getGUI();
	_guiBackground.erase(menu);
}

void sfx::gui::_spriteBackground(std::string menu, const std::string& sheet,
	const std::string& sprite) {
	START();
		if (menu == "") menu = getGUI();
		try {
			_guiBackground[menu].set(_sheet.at(sheet), sprite);
		} catch (const std::out_of_range&) {
			ERROR("This sheet does not exist!");
		}
	END("Attempted to set sprite \"{}\" from sheet \"{}\" to the background of "
		"menu \"{}\".", sprite, sheet, menu);
}

void sfx::gui::_colourBackground(std::string menu, const sf::Color& colour) {
	if (menu == "") menu = getGUI();
	_guiBackground[menu].set(colour);
}

void sfx::gui::_setGlobalFont(const std::string& fontName) {
	START()
		if (!_fonts) ERROR("No fonts object has been given to this gui object.")
		auto fontPath = _fonts->getFontPath(fontName);
		// Invalid font name will be logged by fonts class.
		if (!fontPath.empty()) {
			auto font = tgui::Font(fontPath);
			font.setSmooth(false);
			_gui.setFont(font);
		}
	END("Attempted to set the font \"{}\" as the global font.", fontName)
}

// WIDGETS //

bool sfx::gui::_widgetExists(const std::string& name) const {
	return _findWidget<Widget>(name).operator bool();
}

std::string sfx::gui::_getWidgetFocused(const std::string& parent) const {
	if (parent.empty() && _gui.getFocusedChild()) {
		return _gui.getFocusedChild()->getWidgetName().toStdString();
	} else {
		START_WITH_WIDGET(parent)
			Container::ConstPtr c = nullptr;
			if (widget->isContainer()) {
				c = std::dynamic_pointer_cast<Container>(widget);
			} else if (auto subwidgetContainer = _getSubwidgetContainer(widget)) {
				c = subwidgetContainer;
			} else UNSUPPORTED_WIDGET_TYPE()
			if (c->getFocusedChild())
				return c->getFocusedChild()->getWidgetName().toStdString();
		END("Attempted to find the widget with setfocus that is within widget "
			"\"{}\".", parent)
	}
	return "";
}

void sfx::gui::_addWidget(const std::string& newWidgetType,
	const std::string& name, const std::string& signalHandler) {
	START_WITH_NONEXISTENT_WIDGET(name)
	if (widget = _createWidget(newWidgetType, fullnameAsString, fullname[0])) {
		container->add(widget, fullnameAsString);
		_connectSignals(widget, signalHandler);
		// If the widget is a ChildWindow, don't forget to turn on automatic
		// handling of minimise and maximise, apply all titlebar buttons by
		// default, and make it resizable.
		if (widget->getWidgetType() == type::ChildWindow) {
			_childWindowData[fullnameAsString];
			const auto win = std::dynamic_pointer_cast<ChildWindow>(widget);
			win->setTitleButtons(ChildWindow::TitleButton::Close |
				ChildWindow::TitleButton::Minimize |
				ChildWindow::TitleButton::Maximize);
			win->setResizable();
		}
	} else {
		ERROR("Could not create the new widget.")
	}
	END("Attempted to create a new \"{}\" widget with name \"{}\".", newWidgetType,
		name)
}

void sfx::gui::_connectSignalHandler(const std::string& name,
	asIScriptFunction* const handler) {
	START_WITH_WIDGET(name)
		_additionalSignalHandlers.erase(fullnameAsString);
		if (handler) _additionalSignalHandlers.emplace(fullnameAsString, handler);
	END("Attempted to connect a signal handler to a widget with name \"{}\", in "
		"menu \"{}\".", name, fullname[0])
	if (handler) handler->Release();
}

void sfx::gui::_disconnectSignalHandlers(const CScriptArray* const names) {
	if (!names) {
		_logger.warning("Null array given to disconnectSignalHandlers(): doing "
			"nothing.");
		return;
	}
	for (asUINT i = 0, len = names->GetSize(); i < len; ++i) {
		const std::string name =
			*static_cast<const std::string* const>(names->At(i));
		START_WITH_WIDGET(name)
			_additionalSignalHandlers.erase(fullnameAsString);
		END("Attempted to disconnect signal handler from a widget with name "
			"\"{}\", in menu \"{}\".", name, fullname[0])
	}
	names->Release();
}

std::string sfx::gui::_getParent(const std::string& name) {
	START_WITH_WIDGET(name)
		if (fullname.size() < 2)
			ERROR("This operation is not supported on menus themselves.")
		return _findParent(widget.get())->getWidgetName().toStdString();
	END("Attempted to get the name of a widget \"{}\"'s parent, in menu \"{}\".",
		name, fullname[0])
	return "";
}

void sfx::gui::_removeWidget(const std::string& name) {
	START_WITH_WIDGET(name)
	if (fullname.size() < 2) ERROR("Removing entire menus is not supported.")
	_removeWidgets(widget, container, true);
	END("Attempted to remove the widget \"{}\" within menu \"{}\".", name,
		fullname[0])
}

void sfx::gui::_setWidgetFocus(const std::string& name) {
	START_WITH_WIDGET(name)
	widget->setFocused(true);
	END("Attempted to set the focus to a widget \"{}\" within menu \"{}\".", name,
		fullname[0])
}

void sfx::gui::_setWidgetFont(const std::string& name,
	const std::string& fontName) {
	START_WITH_WIDGET(name)
	if (!_fonts) ERROR("No fonts object has been given to this gui object.")
	auto fontPath = _fonts->getFontPath(fontName);
	// Invalid font name will be logged by fonts class.
	if (!fontPath.empty()) {
		auto font = tgui::Font(fontPath);
		font.setSmooth(false);
		widget->getRenderer()->setFont(font);
	}
	END("Attempted to set the font \"{}\" to a widget \"{}\" within menu \"{}\".",
		fontName, name, fullname[0])
}

void sfx::gui::_setWidgetPosition(const std::string& name, const std::string& x,
	const std::string& y) {
	START_WITH_WIDGET(name)
	widget->setPosition(x.c_str(), y.c_str());
	END("Attempted to set the position (\"{}\",\"{}\") to a widget \"{}\" within "
		"menu \"{}\".", x, y, name, fullname[0])
}

sf::Vector2f sfx::gui::_getWidgetAbsolutePosition(const std::string& name) {
	START_WITH_WIDGET(name)
	return _findWidgetAbsolutePosition(widget.get());
	END("Attempted to get the absolute position of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return {};
}

void sfx::gui::_setWidgetOrigin(const std::string& name, const float x,
	const float y) {
	START_WITH_WIDGET(name)
	widget->setOrigin(x, y);
	END("Attempted to set the origin ({},{}) to a widget \"{}\" within menu "
		"\"{}\".", x, y, name, fullname[0])
}

void sfx::gui::_setWidgetSize(const std::string& name, const std::string& w,
	const std::string& h) {
	START_WITH_WIDGET(name)
	if (w.empty() && h.empty())
		ERROR("Did you mean to provide an empty width and height?");
	if (w.empty()) widget->setHeight(h.c_str());
	else if (h.empty()) widget->setWidth(w.c_str());
	else widget->setSize(w.c_str(), h.c_str());
	END("Attempted to set the size (\"{}\",\"{}\") to a widget \"{}\" within menu "
		"\"{}\".", w, h, name, fullname[0])
}

sf::Vector2f sfx::gui::_getWidgetFullSize(const std::string& name) {
	START_WITH_WIDGET(name)
	return widget->getFullSize();
	END("Attempted to get the full size of a widget \"{}\" within menu \"{}\".",
		name, fullname[0])
	return {};
}

void sfx::gui::_setWidgetEnabled(const std::string& name, const bool enable) {
	START_WITH_WIDGET(name)
	widget->setEnabled(enable);
	END("Attempted to update widget \"{}\"'s enabled state, within menu \"{}\"",
		name, fullname[0])
}

bool sfx::gui::_getWidgetEnabled(const std::string& name) const {
	START_WITH_WIDGET(name)
	return widget->isEnabled();
	END("Attempted to get the enabled property of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return false;
}

void sfx::gui::_setWidgetVisibility(const std::string& name, const bool visible) {
	START_WITH_WIDGET(name)
	widget->setVisible(visible);
	END("Attempted to update widget \"{}\"'s visibility, within menu \"{}\".",
		name, fullname[0])
}

bool sfx::gui::_getWidgetVisibility(const std::string& name) const {
	START_WITH_WIDGET(name)
	return widget->isVisible();
	END("Attempted to get the visibility property of a widget \"{}\" within menu "
		"\"{}\".", name, fullname[0])
	return false;
}

void sfx::gui::_moveWidgetToFront(const std::string& name) {
	START_WITH_WIDGET(name)
	widget->moveToFront();
	END("Attempted to move the widget \"{}\" within menu \"{}\" to the front.",
		name, fullname[0])
}

void sfx::gui::_moveWidgetToBack(const std::string& name) {
	START_WITH_WIDGET(name)
	widget->moveToBack();
	END("Attempted to move the widget \"{}\" within menu \"{}\" to the back.",
		name, fullname[0])
}

void sfx::gui::_setWidgetText(const std::string& name, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(name)
		// For EditBoxes and TextAreas, don't translate the text, as this is text
		// that the user can edit.
		if (widgetType == type::EditBox) {
			std::dynamic_pointer_cast<EditBox>(widget)->setText(text);
		} else if (widgetType == type::TextArea) {
			std::dynamic_pointer_cast<TextArea>(widget)->setText(text);
		} else {
			if (widgetType != type::BitmapButton && widgetType != type::Label &&
				widgetType != type::Button && widgetType != type::ChildWindow &&
				widgetType != type::CheckBox && widgetType != type::RadioButton)
					UNSUPPORTED_WIDGET_TYPE()
			_setTranslatedString(fullnameAsString, text, variables);
			_translateWidget(widget);
		}
	END("Attempted to set the caption \"{}\" to a widget \"{}\" of type \"{}\" "
		"within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

void sfx::gui::_setWidgetIndex(const std::string& name, const std::size_t index) {
	START_WITH_WIDGET(name)
	if (fullname.size() < 2)
		ERROR("This is operation is unsupported for entire menus.")
	if (!container->setWidgetIndex(widget, index)) {
		// The size() should never be 0 here...
		ERROR(std::string("The index cannot be higher than ").append(
			std::to_string(container->getWidgets().size() - 1)).append("."))
	}
	END("Attempted to set a widget \"{}\"'s index to {}.", name, index)
}

// DIRECTIONAL FLOW //

void sfx::gui::_setWidgetDirectionalFlow(const std::string& name,
	const std::string& upName, const std::string& downName,
	const std::string& leftName, const std::string& rightName) {
	std::vector<std::string> fullname, fullnameUp, fullnameDown, fullnameLeft,
		fullnameRight;
	std::string fullnameAsString, fullnameAsStringUp, fullnameAsStringDown,
		fullnameAsStringLeft, fullnameAsStringRight;
	static const auto widgetDoesNotExist = [&](const std::string& doesNotExist) {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"within menu \"{}\", to the widgets up=\"{}\", down=\"{}\", "
			"left=\"{}\", right=\"{}\". The widget \"{}\" does not exist.", name,
			fullname[0], upName, downName, leftName, rightName, doesNotExist);
	};
	if (!_findWidget<Widget>(name, &fullname, &fullnameAsString)) {
		widgetDoesNotExist(name);
		return;
	}
	if (!upName.empty() && upName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(upName, &fullnameUp, &fullnameAsStringUp)) {
		widgetDoesNotExist(upName);
		return;
	}
	if (!downName.empty() && downName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(downName, &fullnameDown, &fullnameAsStringDown)) {
		widgetDoesNotExist(downName);
		return;
	}
	if (!leftName.empty() && leftName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(leftName, &fullnameLeft, &fullnameAsStringLeft)) {
		widgetDoesNotExist(leftName);
		return;
	}
	if (!rightName.empty() && rightName != GOTO_PREVIOUS_WIDGET &&
		!_findWidget<Widget>(rightName, &fullnameRight, &fullnameAsStringRight)) {
		widgetDoesNotExist(rightName);
		return;
	}
	if ((fullnameUp.empty() || fullname[0] == fullnameUp[0]) &&
		(fullnameDown.empty() || fullname[0] == fullnameDown[0]) &&
		(fullnameLeft.empty() || fullname[0] == fullnameLeft[0]) &&
		(fullnameRight.empty() || fullname[0] == fullnameRight[0])) {
		_directionalFlow[fullnameAsString].up =
			upName == GOTO_PREVIOUS_WIDGET ? upName : fullnameAsStringUp;
		_directionalFlow[fullnameAsString].down =
			downName == GOTO_PREVIOUS_WIDGET ? downName : fullnameAsStringDown;
		_directionalFlow[fullnameAsString].left =
			leftName == GOTO_PREVIOUS_WIDGET ? leftName : fullnameAsStringLeft;
		_directionalFlow[fullnameAsString].right =
			rightName == GOTO_PREVIOUS_WIDGET ? rightName : fullnameAsStringRight;
	} else {
		_logger.error("Attempted to set the directional flow of a widget \"{}\", "
			"within menu \"{}\", to the widgets up=\"{}\", down=\"{}\", "
			"left=\"{}\", right=\"{}\". Not all of these widgets are in the same "
			"menu!", name, fullname[0], fullnameAsStringUp, fullnameAsStringDown,
			fullnameAsStringLeft, fullnameAsStringRight);
	}
}

void sfx::gui::_setWidgetDirectionalFlowStart(const std::string& name) {
	START_WITH_WIDGET(name)
		_selectThisWidgetFirst[fullname[0]] = fullnameAsString;
	END("Attempted to set the widget \"{}\" as the first to be selected upon "
		"initial directional input, for the menu \"{}\".", name, fullname[0])
}

void sfx::gui::_clearWidgetDirectionalFlowStart(const std::string& menu) {
	if (_menuExists(menu)) {
		_selectThisWidgetFirst.erase(menu);
	} else {
		_logger.error("Attempted to disable directional input for the menu "
			"\"{}\". Menu does not exist.", menu);
	}
}

void sfx::gui::_setWidgetDirectionalFlowSelection(const std::string& name) {
	START_WITH_WIDGET(name)
		_makeNewDirectionalSelection(fullnameAsString, fullname[0]);
	END("Attempted to manually directionally select the widget \"{}\", in the "
		"menu \"{}\".", name, fullname[0])
}

void sfx::gui::_setDirectionalFlowAngleBracketSprite(const std::string& corner,
	const std::string& sheet, const std::string& key) {
	START()
		const auto spritesheet = _sheet.find(sheet);
		if (spritesheet == _sheet.end()) ERROR("This spritesheet does not exist.")
		if (!spritesheet->second->doesSpriteExist(key))
			ERROR("This sprite does not exist.")
		const auto cornerFormatted = tgui::String(corner).trim().toLower();
		if (cornerFormatted == "ul") {
			_angleBracketUL.setSpritesheet(spritesheet->second);
			_angleBracketUL.setSprite(key);
		} else if (cornerFormatted == "ur") {
			_angleBracketUR.setSpritesheet(spritesheet->second);
			_angleBracketUR.setSprite(key);
		} else if (cornerFormatted == "ll") {
			_angleBracketLL.setSpritesheet(spritesheet->second);
			_angleBracketLL.setSprite(key);
		} else if (cornerFormatted == "lr") {
			_angleBracketLR.setSpritesheet(spritesheet->second);
			_angleBracketLR.setSprite(key);
		} else {
			ERROR("Unrecognised corner, must be \"UL\", \"UR\", \"LL\", or "
				"\"LR\".")
		}
	END("Attempted to set the sprite \"{}\" from spritesheet \"{}\" as the "
		"directional flow angle bracket for the \"{}\" corner.", key, sheet,
		corner)
}

// SPRITES //

void sfx::gui::_setWidgetSprite(const std::string& name, const std::string& sheet,
	const std::string& key) {
	START_WITH_WIDGET(name)
		if (widgetType != type::BitmapButton && widgetType != type::Picture)
			UNSUPPORTED_WIDGET_TYPE()
		_applySprite(widget, sheet, key);
	END("Attempted to set the sprite \"{}\" from sheet \"{}\" to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", key, sheet, name,
		widgetType, fullname[0])
}

void sfx::gui::_clearWidgetSprite(const std::string& name) {
	START_WITH_WIDGET(name)
		if (widgetType != type::BitmapButton && widgetType != type::Picture)
			UNSUPPORTED_WIDGET_TYPE()
		_guiSpriteKeys.erase(fullnameAsString);
		_widgetSprites.erase(widget);
	END("Attempted to clear the sprite from widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_matchWidgetSizeToSprite(const std::string& name,
	const bool overrideSetSize) {
	START_WITH_WIDGET(name)
		if (widgetType != type::Picture) UNSUPPORTED_WIDGET_TYPE()
		if (overrideSetSize)
			_dontOverridePictureSizeWithSpriteSize.erase(fullnameAsString);
		else
			_dontOverridePictureSizeWithSpriteSize.insert(fullnameAsString);
	END("Attempted to match widget \"{}\"'s size to its set sprite. The widget is "
		"of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

// LABEL //

void sfx::gui::_setWidgetTextSize(const std::string& name,
	const unsigned int size) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(BitmapButton, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(Button, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(EditBox, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(TextArea, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(MenuBar, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(Tabs, castWidget->setTextSize(size);)
		ELSE_IF_WIDGET_IS(TextArea, castWidget->setTextSize(size);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the character size {} to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", size, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextStyles(const std::string& name,
	const std::string& styles) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->getRenderer()->setTextStyle({ styles });)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text styles \"{}\" to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", styles, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextMaximumWidth(const std::string& name, const float w) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->setMaximumTextWidth(w);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text max width {} to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", w, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->getRenderer()->setTextColor(colour);)
		ELSE_IF_WIDGET_IS(EditBox,
			castWidget->getRenderer()->setTextColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text colour \"{}\" to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", colour, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetTextOutlineColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline colour \"{}\" to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", colour, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetTextOutlineThickness(const std::string& name,
	const float thickness) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label,
			castWidget->getRenderer()->setTextOutlineThickness(thickness);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text outline thickness {} to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", thickness, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetTextAlignment(const std::string& name,
	const tgui::Label::HorizontalAlignment h,
	const tgui::Label::VerticalAlignment v) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Label, castWidget->setHorizontalAlignment(h);
			castWidget->setVerticalAlignment(v);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the text horizontal alignment {} and vertical alignment "
		"{} to widget \"{}\", which is of type \"{}\", within menu \"{}\".", h, v,
		name, widgetType, fullname[0])
}

// EDITBOX AND TEXTAREA //

std::string sfx::gui::_getWidgetText(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(EditBox, return castWidget->getText().toStdString();)
		IF_WIDGET_IS(TextArea, return castWidget->getText().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of a widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", name, widgetType, fullname[0])
	return "";
}

void sfx::gui::_setEditBoxRegexValidator(const std::string& name,
	const std::string& regex) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(EditBox,
			if (!castWidget->setInputValidator(regex)) ERROR("Invalid regex!");
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\", to validate its input with the regex:  {}  .", name,
		widgetType, fullname[0], regex);
}

void sfx::gui::_setWidgetDefaultText(const std::string& name,
	const std::string& text, CScriptArray* variables) {
	START_WITH_WIDGET(name)
		if (widgetType != type::EditBox && widgetType != type::TextArea)
			UNSUPPORTED_WIDGET_TYPE()
		_setTranslatedString(fullnameAsString, text, variables);
		_translateWidget(widget);
	END("Attempted to set the default text \"{}\" to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

bool sfx::gui::_editBoxOrTextAreaHasFocus() const {
	return _editBoxOrTextAreaHasSetFocus;
}

void sfx::gui::_optimiseTextAreaForMonospaceFont(const std::string& name,
	const bool optimise) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(TextArea,
			castWidget->enableMonospacedFontOptimization(optimise);)
		ELSE_UNSUPPORTED()
	END("Attempted to turn optimisation for monospace fonts {} for widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", optimise ? "on" : "off",
		name, widgetType, fullname[0])
}

void sfx::gui::_getCaretLineAndColumn(const std::string& name,
	std::size_t& line, std::size_t& column) {
	START_WITH_WIDGET(name)
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
		"is of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

// RADIOBUTTON & CHECKBOX //

void sfx::gui::_setWidgetChecked(const std::string& name, const bool checked) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(RadioButton, castWidget->setChecked(checked);)
		ELSE_IF_WIDGET_IS(CheckBox, castWidget->setChecked(checked);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the check status to {} for widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", checked, name, widgetType,
		fullname[0]);
}

bool sfx::gui::_isWidgetChecked(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(RadioButton, return castWidget->isChecked();)
		ELSE_IF_WIDGET_IS(CheckBox, return castWidget->isChecked();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the check status of a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return false;
}

// LIST //

void sfx::gui::_addItem(const std::string& name, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(name)
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
	_setTranslatedString(fullnameAsString, text, variables, index);
	_translateWidget(widget);
	END("Attempted to add an item \"{}\" to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

void sfx::gui::_clearItems(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, castWidget->removeAllItems();)
		ELSE_IF_WIDGET_IS(ComboBox, castWidget->removeAllItems();)
		ELSE_IF_WIDGET_IS(TreeView, castWidget->removeAllItems();)
		ELSE_UNSUPPORTED()
	_originalCaptions.erase(fullnameAsString);
	END("Attempted to clear all items from widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_setSelectedItem(const std::string& name, const std::size_t index) {
	START_WITH_WIDGET(name)
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
	END("Attempted to select item {} from widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", index, name, widgetType, fullname[0])
}

void sfx::gui::_deselectItem(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, castWidget->deselectItem();)
		ELSE_IF_WIDGET_IS(ComboBox, castWidget->deselectItem();)
		ELSE_IF_WIDGET_IS(TreeView, castWidget->deselectItem();)
		ELSE_UNSUPPORTED()
	END("Attempted to deselect the selected item of a widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

int sfx::gui::_getSelectedItem(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItemIndex();)
		ELSE_IF_WIDGET_IS(ComboBox, return castWidget->getSelectedItemIndex();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the index of the selected item of a widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", name, widgetType,
		fullname[0])
	return -1;
}

std::string sfx::gui::_getSelectedItemText(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ListBox, return castWidget->getSelectedItem().toStdString();)
		ELSE_IF_WIDGET_IS(ComboBox,
			return castWidget->getSelectedItem().toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the text of the selected item of a widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return "";
}

void sfx::gui::_setItemsToDisplay(const std::string& name,
	const std::size_t items) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ComboBox, castWidget->setItemsToDisplay(items);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the number of items to display to {} for widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", items, name, widgetType,
		fullname[0])
}

// TREEVIEW //

CScriptArray* sfx::gui::_getSelectedItemTextHierarchy(const std::string& name) {
	auto const arr = _scripts->createArray("string");
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(TreeView,
			const auto item = castWidget->getSelectedItem();
			arr->Resize(static_cast<asUINT>(item.size()));
			asUINT i = 0;
			for (const auto& parent : item)
				arr->SetValue(i++, &parent.toStdString());
		)
		ELSE_UNSUPPORTED()
	END("Attempted to get the hierarchy of the selected item of a widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", name, widgetType,
		fullname[0])
	return arr;
}

void sfx::gui::_addTreeViewItem(const std::string& name,
	const CScriptArray* const hierarchy) {
	START_WITH_WIDGET(name)
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
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	if (hierarchy) hierarchy->Release();
}

// TABS //

void sfx::gui::_addTab(const std::string& name, const std::string& text,
	CScriptArray* const variables) {
	START_WITH_WIDGET(name)
		std::size_t index = 0;
		IF_WIDGET_IS(Tabs, index = castWidget->add(text, false);)
		ELSE_UNSUPPORTED()
		_setTranslatedString(fullnameAsString, text, variables, index);
		_translateWidget(widget);
	END("Attempted to add a tab \"{}\" to widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

void sfx::gui::_setSelectedTab(const std::string& name, const std::size_t index) {
	START_WITH_WIDGET(name)
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
	END("Attempted to select tab {} from widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", index, name, widgetType, fullname[0])
}

int sfx::gui::_getSelectedTab(const std::string& name) const {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Tabs, return castWidget->getSelectedIndex();)
		ELSE_IF_WIDGET_IS(TabContainer, return castWidget->getSelectedIndex();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the index of the selected tab of a widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return -1;
}

std::size_t sfx::gui::_getTabCount(const std::string& name) const {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Tabs, return castWidget->getTabsCount();)
		ELSE_IF_WIDGET_IS(TabContainer,
			return castWidget->getTabs()->getTabsCount();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the tab count of a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return 0;
}

std::string sfx::gui::_getTabText(const std::string& name,
	const std::size_t index) const {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Tabs, return castWidget->getText(index).toStdString();)
		ELSE_IF_WIDGET_IS(TabContainer,
			return castWidget->getTabText(index).toStdString();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the translated text of tab number {} of a widget "
		"\"{}\", which is of type \"{}\", within menu \"{}\".", index, name,
		widgetType, fullname[0])
	return "";
}

// CONTAINER //

void sfx::gui::_removeWidgetsFromContainer(const std::string& name) {
	START_WITH_WIDGET(name)
		if (fullname.size() < 2) {
			_removeWidgets(widget, nullptr, false);
		} else {
			// Using isContainer() conveniently prevents deleting all of a
			// SubwidgetContainer's widgets without removing the SubwidgetContainer
			// itself.
			if (widget->isContainer()) _removeWidgets(widget, container, false);
			else UNSUPPORTED_WIDGET_TYPE()
		}
	END("Attempted to remove the widgets from a widget \"{}\", of type \"{}\", "
		"within menu \"{}\".", name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetIndexInContainer(const std::string& name,
	const std::size_t oldIndex, const std::size_t newIndex) {
	START_WITH_WIDGET(name)
	if (!widget->isContainer()) UNSUPPORTED_WIDGET_TYPE()
		container = std::dynamic_pointer_cast<Container>(widget);
	try {
		widget = container->getWidgets().at(oldIndex);
	} catch (const std::out_of_range&) {
		ERROR("This container does not have a widget with that number.")
	}
	if (!container->setWidgetIndex(widget, newIndex)) {
		const auto count = container->getWidgets().size();
		if (count) {
			ERROR(std::string("The new index cannot be higher than ").append(
				std::to_string(count - 1)).append("."))
		} else {
			ERROR("This container has no widgets.")
		}
	}
	END("Attempted to set the widget \"{}\"'s number {} widget to an index of {}, "
		"within menu \"{}\". The widget is of type \"{}\".", name, oldIndex,
		newIndex, fullname[0], widgetType)
}

std::size_t sfx::gui::_getWidgetCount(const std::string& name) {
	START_WITH_WIDGET(name)
		if (widget->isContainer()) {
			return std::dynamic_pointer_cast<Container>(
				widget)->getWidgets().size();
		} else UNSUPPORTED_WIDGET_TYPE()
	END("Attempted to get the widget count of a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return 0;
}

void sfx::gui::_setGroupPadding(const std::string& name,
	const std::string& padding) {
	START_WITH_WIDGET(name)
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
	END("Attempted to set a padding {} to widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", padding, name, widgetType, fullname[0])
}

void sfx::gui::_setGroupPadding(const std::string& name, const std::string& left,
	const std::string& top, const std::string& right, const std::string& bottom) {
	const auto padding = Padding(AbsoluteOrRelativeValue(left),
		AbsoluteOrRelativeValue(top), AbsoluteOrRelativeValue(right),
		AbsoluteOrRelativeValue(bottom));
	START_WITH_WIDGET(name)
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
		"\"{}\", which is of type \"{}\", within menu \"{}\".", left, top, right,
		bottom, name, widgetType, fullname[0])
}

void sfx::gui::_applySpritesToWidgetsInContainer(const std::string& name,
	const std::string& spritesheet, const CScriptArray* const sprites) {
	std::size_t spritesCount = 0;
	START_WITH_WIDGET(name)
		if (!sprites) ERROR("No sprites given!")
		if (!widget->isContainer()) UNSUPPORTED_WIDGET_TYPE()
		const auto& widgets =
			std::dynamic_pointer_cast<Container>(widget)->getWidgets();
		spritesCount = sprites->GetSize();
		asUINT counter = 0;
		for (const auto& widget : widgets) {
			if (widget->getWidgetType() == type::BitmapButton ||
				widget->getWidgetType() == type::Picture) {
				_applySprite(widget, spritesheet,
					*static_cast<const std::string*>(sprites->At(counter++)));
				if (counter >= spritesCount) break;
			}
		}
	END("Attempted to apply {} sprites from spritesheet \"{}\", to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", spritesCount, spritesheet,
		name, widgetType, fullname[0])
	if (sprites) sprites->Release();
}

// PANEL //

void sfx::gui::_setWidgetBgColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_IF_WIDGET_IS(ScrollablePanel,
			castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_IF_WIDGET_IS(BitmapButton,
			castWidget->getRenderer()->setBackgroundColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the background colour \"{}\" to widget \"{}\", which is "
		"of type \"{}\", within menu \"{}\".", colour, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetBorderSize(const std::string& name, const float size) {
	START_WITH_WIDGET(name);
		IF_WIDGET_IS(Panel, castWidget->getRenderer()->setBorders(size);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border size of {} to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", size, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetBorderColour(const std::string& name,
	const sf::Color& colour) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setBorderColor(colour);)
		ELSE_UNSUPPORTED()
	END("Attempted to set a border colour of {} to widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", colour, name, widgetType, fullname[0])
}

void sfx::gui::_setWidgetBorderRadius(const std::string& name,
	const float radius) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Panel,
			castWidget->getRenderer()->setRoundedBorderRadius(radius);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the border radius {} to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", radius, name, widgetType, fullname[0])
}

void sfx::gui::_setHorizontalScrollbarPolicy(const std::string& name,
	const tgui::Scrollbar::Policy policy) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollbarPolicy(policy);)
		ELSE_IF_WIDGET_IS(TextArea,
			castWidget->setHorizontalScrollbarPolicy(policy);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar policy {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", policy, name, widgetType,
		fullname[0])
}

void sfx::gui::_setHorizontalScrollbarAmount(const std::string& name,
	const unsigned int amount) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setHorizontalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the horizontal scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", amount, name, widgetType,
		fullname[0])
}

void sfx::gui::_setVerticalScrollbarAmount(const std::string& name,
	const unsigned int amount) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			castWidget->setVerticalScrollAmount(amount);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the vertical scrollbar amount {} to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", amount, name, widgetType,
		fullname[0])
}

void sfx::gui::_setVerticalScrollbarValue(const std::string& name,
	const unsigned int value) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel,
			const auto max =
				static_cast<unsigned int>(castWidget->getContentSize().y);
			if (value > max) castWidget->setVerticalScrollbarValue(max);
			else castWidget->setVerticalScrollbarValue(value);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the vertical scrollbar value {} to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", value, name, widgetType,
		fullname[0])
}

float sfx::gui::_getScrollbarWidth(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ScrollablePanel, return castWidget->getScrollbarWidth();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the scrollbar width of widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
	return 0.0f;
}

// LAYOUT //

void sfx::gui::_setWidgetRatioInLayout(const std::string& name,
	const std::size_t index, const float ratio) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(HorizontalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_IF_WIDGET_IS(VerticalLayout, if (!castWidget->setRatio(index, ratio))
			ERROR("The widget index was too high.");)
		ELSE_UNSUPPORTED()
	END("Attempted to set the widget ratio {} to widget {} in widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", ratio, index, name,
		widgetType, fullname[0])
}

void sfx::gui::_setSpaceBetweenWidgets(const std::string& name,
	const float space) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(HorizontalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_IF_WIDGET_IS(VerticalLayout, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_IF_WIDGET_IS(HorizontalWrap, castWidget->getRenderer()->
			setSpaceBetweenWidgets(space);)
		ELSE_UNSUPPORTED()
	END("Attempted to set {} to a widget \"{}\"'s space between widgets property. "
		"The widget is of type \"{}\", within menu \"{}\".", space, name,
		widgetType, fullname[0])
}

// GRID //

void sfx::gui::_addWidgetToGrid(const std::string& newWidgetType,
	const std::string& name, const std::size_t row, const std::size_t col,
	const std::string& signalHandler) {
	START_WITH_NONEXISTENT_WIDGET(name)
		if (widget = _createWidget(newWidgetType, fullnameAsString, fullname[0])) {
			if (container->getWidgetType() != type::Grid) {
				ERROR(std::string("The widget \"").append(containerName).append(
					"\" is of type \"").append(container->getWidgetType().
						toStdString()).append("\", not type \"").
					append(type::Grid).append("\"."))
			} else {
				widget->setWidgetName(fullnameAsString);
				_connectSignals(widget, signalHandler);
				std::dynamic_pointer_cast<Grid>(container)->addWidget(widget, row,
					col);
			}
		}
	END("Attempted to create a new \"{}\" widget with name \"{}\" and add it to a "
		"grid at row {}, column {}.", newWidgetType, name, row, col)
}

void sfx::gui::_setWidgetAlignmentInGrid(const std::string& name,
	const std::size_t row, const std::size_t col,
	const tgui::Grid::Alignment alignment) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Grid,
			auto& table = castWidget->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size()) {
					castWidget->setWidgetAlignment(row, col, alignment);
				} else {
					ERROR("The column index is out of range.")
				}
			} else {
				ERROR("The row index is out of range.")
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set an alignment {} to a widget \"{}\", which is of type "
		"\"{}\", @ ({}, {}), within menu \"{}\".", alignment, name, widgetType,
		row, col, fullname[0])
}

void sfx::gui::_setWidgetPaddingInGrid(const std::string& name,
	const std::size_t row, const std::size_t col, const std::string& padding) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(Grid,
			auto& table = castWidget->getGridWidgets();
			if (row < table.size()) {
				if (col < table[row].size()) {
					castWidget->setWidgetPadding(row, col,
						AbsoluteOrRelativeValue(padding));
				} else {
					ERROR("The column index is out of range.")
				}
			} else {
				ERROR("The row index is out of range.")
			}
			)
		ELSE_UNSUPPORTED()
	END("Attempted to set a padding {} to a widget \"{}\", which is of type "
		"\"{}\", @ ({}, {}), within menu \"{}\".", padding, name, widgetType,
		row, col, fullname[0])
}

// MENUS //

sfx::gui::MenuItemID sfx::gui::_addMenu(const std::string& name,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			if (_hierarchyOfLastMenuItem[fullnameAsString].size() == 1) {
				_logger.warning("Menu \"{}\" in MenuBar \"{}\" is empty!",
					_hierarchyOfLastMenuItem[fullnameAsString][0],
						fullnameAsString);
			}
			castWidget->addMenu(text);
			_hierarchyOfLastMenuItem[fullnameAsString] = { text };
			if (_menuCounter.find(fullnameAsString) == _menuCounter.end())
				_menuCounter.emplace(fullnameAsString, 0);
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu \"{}\" to a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
	return ret;
}

sfx::gui::MenuItemID sfx::gui::_addMenuItem(const std::string& name,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
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
				for (sfx::gui::MenuItemID i = 0, len = hierarchy.size(); i < len;
					++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy = copy;
				ERROR(error);
			}
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, name,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a new menu item \"{}\" to a widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
	return ret;
}

sfx::gui::MenuItemID sfx::gui::_addMenuItemIntoLastItem(const std::string& name,
	const std::string& text, CScriptArray* const variables) {
	auto ret = NO_MENU_ITEM_ID;
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() == 1) {
				_logger.warning("Calling addMenuItemIntoLastItem() when "
					"addMenuItem() was likely intended.");
			}
			hierarchy.push_back(text);
			if (!castWidget->addMenuItem(hierarchy)) {
				std::string error = "Could not add item with hierarchy: ";
				for (sfx::gui::MenuItemID i = 0, len = hierarchy.size(); i < len;
					++i) {
					error += hierarchy[i].toStdString() +
						(i < len - 1 ? ", " : ". ");
				}
				hierarchy.pop_back();
				ERROR(error);
			}
			_setTranslatedString(fullnameAsString, text, variables,
				_menuCounter[fullnameAsString]);
			// NOTE: we also must reconnect the signal handler after translating
			// the menu item!
			castWidget->connectMenuItem(hierarchy,
				&sfx::gui::menuItemClickedSignalHandler, this, name,
				_menuCounter[fullnameAsString]);
			ret = _menuCounter[fullnameAsString]++;
		)
		ELSE_UNSUPPORTED()
	END("Attempted to create a new submenu with item \"{}\" in a widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", text, name, widgetType,
		fullname[0])
	if (variables) variables->Release();
	return ret;
}

void sfx::gui::_exitSubmenu(const std::string& name) {
	START_WITH_WIDGET(name)
		if (!_isLoading) {
			ERROR("This function cannot be called outside of a menu's SetUp() "
				"function!");
		}
		IF_WIDGET_IS(MenuBar,
			auto& hierarchy = _hierarchyOfLastMenuItem[fullnameAsString];
			if (hierarchy.size() == 0) {
				ERROR("No menu has been added yet!");
			} else if (hierarchy.size() < 3) {
				ERROR("Not currently in a submenu!");
			}
			hierarchy.pop_back();
		)
		ELSE_UNSUPPORTED()
	END("Attempted to exit the current submenu of widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0])
}

// CHILDWINDOW //

void sfx::gui::_autoHandleMinMax(const std::string& name, const bool handle) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			if (handle)
				_childWindowData.erase(fullnameAsString);
			else
				_childWindowData[fullnameAsString];
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the autoHandleMinMax property to {}, for the widget "
		"\"{}\", which is of type \"{}\", within menu \"{}\".", handle, name,
		widgetType, fullname[0]);
}

void sfx::gui::_setChildWindowTitleButtons(const std::string& name,
	const unsigned int buttons) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setTitleButtons(buttons);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the titlebar button mask {} to the widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", buttons, name, widgetType,
		fullname[0])
}

void sfx::gui::_setWidgetResizable(const std::string& name, const bool resizable) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setResizable(resizable);)
		ELSE_IF_WIDGET_IS(FileDialog, castWidget->setResizable(resizable);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the resizability property of widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\", to {}.", name, widgetType, fullname[0],
		resizable);
}

void sfx::gui::_setWidgetPositionLocked(const std::string& name,
	const bool locked) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setPositionLocked(locked);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the position locked property of widget \"{}\", which is "
		"of type \"{}\", within menu \"{}\", to {}.", name, widgetType,
		fullname[0], locked);
}

float sfx::gui::_getTitleBarHeight(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_IF_WIDGET_IS(FileDialog,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_IF_WIDGET_IS(MessageBox,
			return castWidget->getRenderer()->getTitleBarHeight();)
		ELSE_UNSUPPORTED()
	END("Attempted to get the titlebar height of a widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", name, widgetType, fullname[0]);
	return 0.0f;
}

CScriptArray* sfx::gui::_getBorderWidths(const std::string& name) {
	auto arr = _scripts->createArray("float");
	arr->Resize(4);
	static const auto fromBorders = [arr](const Borders& borders) {
		auto temp = borders.getLeft(); arr->SetValue(0, &temp);
		      temp = borders.getTop(); arr->SetValue(1, &temp);
		    temp = borders.getRight(); arr->SetValue(2, &temp);
		   temp = borders.getBottom(); arr->SetValue(3, &temp);
	};
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_IF_WIDGET_IS(FileDialog,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_IF_WIDGET_IS(MessageBox,
			fromBorders(castWidget->getRenderer()->getBorders());)
		ELSE_UNSUPPORTED()
	END("Attempted to get the border widths of a widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0]);
	return arr;
}

void sfx::gui::_openChildWindow(const std::string& name, const std::string& x,
	const std::string& y) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			if (_childWindowData.find(fullnameAsString) !=
				_childWindowData.end()) {
				_restoreChildWindowImpl(castWidget,
					_childWindowData[fullnameAsString]);
			}
			castWidget->setPosition(x.c_str(), y.c_str());
			castWidget->moveToFront();
			castWidget->setVisible(true);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to open the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\".", name, widgetType, fullname[0]);
}

void sfx::gui::_closeChildWindow(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->setVisible(false);)
		ELSE_UNSUPPORTED()
	END("Attempted to close the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\".", name, widgetType, fullname[0]);
}

void sfx::gui::_closeChildWindowAndEmitSignal(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow, castWidget->close();)
		ELSE_UNSUPPORTED()
	END("Attempted to close the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\", and emit the onClosing signal.", name, widgetType,
		fullname[0]);
}

void sfx::gui::_restoreChildWindow(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			if (_childWindowData.find(fullnameAsString) !=
				_childWindowData.end()) {
				_restoreChildWindowImpl(castWidget,
					_childWindowData[fullnameAsString]);
			}
		)
		ELSE_UNSUPPORTED()
	END("Attempted to restore the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\".", name, widgetType, fullname[0]);
}

void sfx::gui::_restoreChildWindowImpl(const tgui::ChildWindow::Ptr& window,
	child_window_properties& data) {
	if (data.isMinimised || data.isMaximised) {
		if (data.isMinimised) {
			_minimisedChildWindowList[
				window->getParent()->getWidgetName().toStdString()].
				restore(window->getWidgetName().toStdString());
		}
		data.restore(window);
		data.isMinimised = false;
		data.isMaximised = false;
	}
}

bool sfx::gui::_isChildWindowOpen(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(ChildWindow,
			return castWidget->isVisible();)
		ELSE_UNSUPPORTED()
	END("Attempted to query if a widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\", is open.", name, widgetType, fullname[0]);
	return false;
}

// FILEDIALOG //

void sfx::gui::_setFileDialogStrings(const std::string& name,
	const std::string& title, CScriptArray* const v0, const std::string& confirm,
	CScriptArray* const v1, const std::string& cancel, CScriptArray* const v2,
	const std::string& createFolder, CScriptArray* const v3,
	const std::string& filenameLabel, CScriptArray* const v4,
	const std::string& nameColumn, CScriptArray* const v5,
	const std::string& sizeColumn, CScriptArray* const v6,
	const std::string& modifyColumn, CScriptArray* const v7,
	const std::string& allFiles, CScriptArray* const v8) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog,
			_setTranslatedString(fullnameAsString, title, v0, 0);
			_setTranslatedString(fullnameAsString, confirm, v1, 1);
			_setTranslatedString(fullnameAsString, cancel, v2, 2);
			_setTranslatedString(fullnameAsString, createFolder, v3, 3);
			_setTranslatedString(fullnameAsString, filenameLabel, v4, 4);
			_setTranslatedString(fullnameAsString, nameColumn, v5, 5);
			_setTranslatedString(fullnameAsString, sizeColumn, v6, 6);
			_setTranslatedString(fullnameAsString, modifyColumn, v7, 7);
			_setTranslatedString(fullnameAsString, allFiles, v8, 8);
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to restore the widget \"{}\", which is of type \"{}\", within "
		"menu \"{}\".", name, widgetType, fullname[0]);
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

CScriptArray* sfx::gui::_getFileDialogSelectedPaths(const std::string& name) {
	auto ret = _scripts->createArray("string");
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog,
			const auto& paths = castWidget->getSelectedPaths();
			for (const auto& path : paths)
				ret->InsertLast(&path.asString().toStdString());
		)
		ELSE_UNSUPPORTED()
	END("Attempted to get the selected paths from widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", name, widgetType, fullname[0]);
	return ret;
}

void sfx::gui::_addFileDialogFileTypeFilter(const std::string& name,
	const std::string& caption, CScriptArray* const variables,
	CScriptArray* const filters) {
	std::vector<String> expressions;
	if (filters) {
		for (asUINT i = 0, len = filters->GetSize(); i < len; ++i)
			expressions.emplace_back(*static_cast<std::string*>(filters->At(i)));
		filters->Release();
	}
	std::vector<std::pair<String, std::vector<String>>> f;
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog,
			f = castWidget->getFileTypeFilters();
			f.emplace_back(caption, expressions);
			castWidget->setFileTypeFilters(f);
			_setTranslatedString(fullnameAsString, caption, variables,
				f.size() + 7);
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file type filters of widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", name, widgetType, fullname[0]);
	if (variables) variables->Release();
}

void sfx::gui::_clearFileDialogFileTypeFilters(const std::string& name) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog,
			castWidget->setFileTypeFilters({});
			std::get<sfx::gui::ListOfCaptions>(
				_originalCaptions[fullnameAsString]).resize(9);
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file type filters of widget \"{}\", which is of "
		"type \"{}\", within menu \"{}\".", name, widgetType, fullname[0]);
}

void sfx::gui::_setFileDialogFileMustExist(const std::string& name,
	const bool mustExist) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog, castWidget->setFileMustExist(mustExist);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the file must exist property to {}, for widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", mustExist, name,
		widgetType, fullname[0]);
}

void sfx::gui::_setFileDialogDefaultFileFilter(const std::string& name,
	const std::size_t index) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog,
			const auto copy = castWidget->getFileTypeFilters();
			castWidget->setFileTypeFilters(copy, index);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the default file filter to {}, for widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", index, name, widgetType,
		fullname[0]);
}

void sfx::gui::_setFileDialogPath(const std::string& name,
	const std::string& path) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(FileDialog, castWidget->setPath(path);)
		ELSE_UNSUPPORTED()
	END("Attempted to set the current path of \"{}\", to widget \"{}\", which "
		"is of type \"{}\", within menu \"{}\".", path, name, widgetType,
		fullname[0]);
}

// MESSAGEBOX //

void sfx::gui::_setMessageBoxStrings(const std::string& name,
	const std::string& title, CScriptArray* const titleVars,
	const std::string& text, CScriptArray* const textVars) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(MessageBox,
			_setTranslatedString(fullnameAsString, title, titleVars, 0);
			_setTranslatedString(fullnameAsString, text, textVars, 1);
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to set the title \"{}\" and text \"{}\" to widget \"{}\", "
		"which is of type \"{}\", within menu \"{}\".", title, text, name,
		widgetType, fullname[0])
	if (titleVars) titleVars->Release();
	if (textVars) textVars->Release();
}

void sfx::gui::_addMessageBoxButton(const std::string& name,
	const std::string& text, CScriptArray* const variables) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(MessageBox,
			_setTranslatedString(fullnameAsString, text, variables,
				castWidget->getButtons().size() + 2);
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a button \"{}\" to widget \"{}\", which is of type "
		"\"{}\", within menu \"{}\".", text, name, widgetType, fullname[0])
	if (variables) variables->Release();
}

// TABCONTAINER //

std::string sfx::gui::_addTabAndPanel(const std::string& name,
	const std::string& text, CScriptArray* const vars) {
	std::string panelName = "";
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(TabContainer,
			const auto panel = castWidget->addTab(text, false);
			if (!panel) ERROR("Could not create panel!");
			// Fix Panel's name so that it can be accessed by the scripts/engine.
			_sanitiseWidgetName(panel);
			panelName = panel->getWidgetName().toStdString();
			_setTranslatedString(fullnameAsString, text, vars,
				static_cast<std::size_t>(castWidget->getIndex(panel)));
			_translateWidget(widget);
		)
		ELSE_UNSUPPORTED()
	END("Attempted to add a tab \"{}\" to widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", text, name, widgetType, fullname[0])
	if (vars) vars->Release();
	return panelName;
}

void sfx::gui::_removeTabAndPanel(const std::string& panelName) {
	std::size_t i = 0;
	START_WITH_WIDGET(panelName)
		IF_WIDGET_IS(Panel,
			if (const auto tabContainer = _findWidget<TabContainer>(
				container->getWidgetName().toStdString())) {
				i = tabContainer->getIndex(castWidget);
				if (i < 0)
					ERROR("Could not find given panel in the tab container!");
				_removeWidgets(castWidget, container, false);
				tabContainer->removeTab(i);
				// Remove tab's caption from the translation map.
				auto& captions = std::get<sfx::gui::ListOfCaptions>(
					_originalCaptions[tabContainer->getWidgetName().toStdString()]
				);
				captions.erase(captions.begin() + i);
			} else ERROR("The parent of the given panel is of type \"" +
				container->getWidgetType() + "\", not \"TabContainer\"!");
		)
	END("Attempted to add a tab and panel, the latter with name \"{}\", which is "
		"of type \"{}\", within menu \"{}\".", panelName, widgetType, fullname[0])
}

// SPINCONTROL //

void sfx::gui::_setWidgetMinMaxValues(const std::string& name, const float min,
	const float max) {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(SpinControl,
			castWidget->setMinimum(min);
			castWidget->setMaximum(max);
		)
	END("Attempted to set the minimum value ({}) and maximum value ({}) of a "
		"widget \"{}\", which is of type \"{}\", within menu \"{}\".", min, max,
		name, widgetType, fullname[0])
}

bool sfx::gui::_setWidgetValue(const std::string& name, float val) {
	static const std::string errorString = "Attempted to set the value {} to a "
		"widget \"{}\", which is of type \"{}\", within menu \"{}\".";
	bool ret = false;
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(SpinControl,
			const auto min = castWidget->getMinimum();
			const auto max = castWidget->getMaximum();
			if (val < min) {
				_logger.warning(errorString + " Value is smaller than the "
					"minimum, which is {}. The minimum value will be applied.",
					val, name, widgetType, fullname[0], min);
				val = min;
			} else if (val > max) {
				_logger.warning(errorString + " Value is greater than the "
					"maximum, which is {}. The maximum value will be applied.",
					val, name, widgetType, fullname[0], max);
				val = max;
			} else ret = true;
			ret = castWidget->setValue(val);
		)
	END(errorString, val, name, widgetType, fullname[0])
	return ret;
}

float sfx::gui::_getWidgetValue(const std::string& name) const {
	START_WITH_WIDGET(name)
		IF_WIDGET_IS(SpinControl, return castWidget->getValue();)
	END("Attempted to get the value of a widget \"{}\", which is of type \"{}\", "
		"within menu \"{}\".", name, widgetType, fullname[0])
	return 0.f;
}
