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

/**@file guiconstants.hpp
 * Defines constants used throughout the GUI code, to be registered with the script
 * interface.
 */

#pragma once

#include <limits>
#include "SFML/Graphics/Color.hpp"
#include "gui.hpp"
#include "guimacros.hpp"

#undef MessageBox

/// The padding between minimised child windows.
static const std::size_t MINIMISED_CHILD_WINDOW_PADDING = 5;

/// The width of a minimised child window.
static const std::size_t MINIMISED_CHILD_WINDOW_WIDTH = 100;

 // All of these values are intended to be constant.

static float NO_SPACE = -0.001f;
static sf::Color NO_COLOUR(0, 0, 0, 0);
static sfx::gui::MenuItemID NO_MENU_ITEM_ID =
	std::numeric_limits<sfx::gui::MenuItemID>::max();
static std::string GOTO_PREVIOUS_WIDGET = "~";

/// Supported widget types.
namespace type {
	// Widgets with full or partial support in the engine.
	WIDGET_TYPE(BitmapButton);
	WIDGET_TYPE(ListBox);
	WIDGET_TYPE(VerticalLayout);
	WIDGET_TYPE(HorizontalLayout);
	WIDGET_TYPE(Picture);
	WIDGET_TYPE(Label);
	WIDGET_TYPE(ScrollablePanel);
	WIDGET_TYPE(Panel);
	WIDGET_TYPE(Group);
	WIDGET_TYPE(Grid);
	WIDGET_TYPE(Button);
	WIDGET_TYPE(EditBox);
	WIDGET_TYPE(MenuBar);
	WIDGET_TYPE(ChildWindow);
	WIDGET_TYPE(ComboBox);
	WIDGET_TYPE(FileDialog);
	WIDGET_TYPE(MessageBox);
	WIDGET_TYPE(HorizontalWrap);
	WIDGET_TYPE(Tabs);
	WIDGET_TYPE(TreeView);
	WIDGET_TYPE(CheckBox);
	WIDGET_TYPE(RadioButton);
	WIDGET_TYPE(TabContainer);
	WIDGET_TYPE(TextArea);

	// Widgets that can't be created by the scripts yet.
	WIDGET_TYPE(ProgressBar);
	WIDGET_TYPE(SpinButton);
	WIDGET_TYPE(ColourPicker);
	WIDGET_TYPE(Knob);
	WIDGET_TYPE(Scrollbar);
	WIDGET_TYPE(Slider);
	WIDGET_TYPE(SpinControl);
	WIDGET_TYPE(ListView);
	WIDGET_TYPE(RangeSlider);
	WIDGET_TYPE(ToggleButton);

	// ~~~ ALERT ~~~
	// When adding support for widgets that derive from SubwidgetContainer, you
	// MUST fix the names of the widgets that the SubwidgetContainer creates to
	// allow the engine to access additional widgets within the SubwidgetContainer
	// that YOU can create. For example, a TabContainer is able to create Panel
	// widgets, and if these Panel's names are not fixed, the engine will not be
	// able to find any widgets that you add to them! SubwidgetContainers should
	// work just like orindary containers. For example, a panel "Panel1" within
	// TabContainer "Menu.TabContainer" should have a full name of
	// "Menu.TabContainer.Panel1", or at least "TabContainer.Panel1". Be careful
	// not to leave any '.' characters in the short name of any of the
	// SubwidgetContainer's widgets!
}
