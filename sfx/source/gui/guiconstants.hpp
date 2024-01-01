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
static sfx::WidgetID GOTO_PREVIOUS_WIDGET =
	std::numeric_limits<sfx::WidgetID>::max();

// Regex validators. Mostly copied from TGUI EditBox directly.

static std::string VALIDATOR_ALL = ".*";
static std::string VALIDATOR_INT = "[+-]?[0-9]*";
static std::string VALIDATOR_UINT = "[0-9]*";
static std::string VALIDATOR_FLOAT = "[+-]?[0-9]*\\.?[0-9]*";

/// Supported widget types.
namespace type {
	// Widgets with full or partial support in the engine.
	STRING_CONSTANT(BitmapButton)
	STRING_CONSTANT(ListBox)
	STRING_CONSTANT(VerticalLayout)
	STRING_CONSTANT(HorizontalLayout)
	STRING_CONSTANT(Picture)
	STRING_CONSTANT(Label)
	STRING_CONSTANT(ScrollablePanel)
	STRING_CONSTANT(Panel)
	STRING_CONSTANT(Group)
	STRING_CONSTANT(Grid)
	STRING_CONSTANT(Button)
	STRING_CONSTANT(EditBox)
	STRING_CONSTANT(MenuBar)
	STRING_CONSTANT(ChildWindow)
	STRING_CONSTANT(ComboBox)
	STRING_CONSTANT(FileDialog)
	STRING_CONSTANT(MessageBox)
	STRING_CONSTANT(HorizontalWrap)
	STRING_CONSTANT(Tabs)
	STRING_CONSTANT(TreeView)
	STRING_CONSTANT(CheckBox)
	STRING_CONSTANT(RadioButton)
	STRING_CONSTANT(TabContainer)
	STRING_CONSTANT(TextArea)
	STRING_CONSTANT(SpinControl)
	STRING_CONSTANT(ClickableWidget)
	STRING_CONSTANT(ButtonBase)
	STRING_CONSTANT(BoxLayout)
	STRING_CONSTANT(BoxLayoutRatios)
	STRING_CONSTANT(Slider)

	// Widgets that can't be created by the scripts yet.
	STRING_CONSTANT(ProgressBar)
	STRING_CONSTANT(SpinButton)
	STRING_CONSTANT(ColorPicker)
	STRING_CONSTANT(Knob)
	STRING_CONSTANT(Scrollbar)
	STRING_CONSTANT(ListView)
	STRING_CONSTANT(RangeSlider)
	STRING_CONSTANT(ToggleButton)
}

/// Supported signals.
namespace signal {
	STRING_CONSTANT(PositionChanged)
	STRING_CONSTANT(SizeChanged)
	STRING_CONSTANT(Focused)
	STRING_CONSTANT(Unfocused)
	STRING_CONSTANT(MouseEntered)
	STRING_CONSTANT(MouseLeft)
	STRING_CONSTANT(ShowEffectFinished)
	STRING_CONSTANT(AnimationFinished)

	STRING_CONSTANT(MousePressed)
	STRING_CONSTANT(MouseReleased)
	STRING_CONSTANT(Clicked)
	STRING_CONSTANT(DoubleClicked)
	STRING_CONSTANT(RightMousePressed)
	STRING_CONSTANT(RightMouseReleased)
	STRING_CONSTANT(RightClicked)
	STRING_CONSTANT(Pressed)

	STRING_CONSTANT(ItemSelected)
	STRING_CONSTANT(HeaderClicked)
	STRING_CONSTANT(MenuItemClicked)
	STRING_CONSTANT(Expanded)
	STRING_CONSTANT(Collapsed)
	
	STRING_CONSTANT(Closed)
	STRING_CONSTANT(EscapeKeyPressed)
	STRING_CONSTANT(Minimized)
	STRING_CONSTANT(Maximized)
	STRING_CONSTANT(Closing)

	STRING_CONSTANT(ColorChanged)
	STRING_CONSTANT(OkPressed)

	STRING_CONSTANT(TextChanged)
	STRING_CONSTANT(ReturnKeyPressed)
	STRING_CONSTANT(ReturnOrUnfocused)
	STRING_CONSTANT(CaretPositionChanged)

	STRING_CONSTANT(FileSelected)

	STRING_CONSTANT(ButtonPressed)

	STRING_CONSTANT(ValueChanged)
	STRING_CONSTANT(RangeChanged)
	STRING_CONSTANT(Full)

	STRING_CONSTANT(Checked)
	STRING_CONSTANT(Unchecked)
	STRING_CONSTANT(Changed)

	STRING_CONSTANT(SelectionChanging)
	STRING_CONSTANT(SelectionChanged)
	STRING_CONSTANT(TabSelected)
}

/// All widget types that support "clickable" signals.
static const std::unordered_set<std::string> CLICKABLE_WIDGETS = {
	type::ClickableWidget,
	type::ButtonBase,
	type::Button,
	type::EditBox,
	type::Label,
	type::Picture,
	type::ProgressBar,
	type::RadioButton,
	type::SpinButton,
	type::Panel,
	type::BitmapButton,
	type::CheckBox
};

/// All widget types that support \c ChildWindow signals.
static const std::unordered_set<std::string> CHILDWINDOW_WIDGETS = {
	type::ChildWindow,
	type::FileDialog,
	type::ColorPicker,
	type::MessageBox
};

/**
 * Signals, and the widget types that support them.
 * If no widget types are listed for a signal, even once, it means that it's
 * supported on all types.
 * @warning MAKE SURE THAT THE SAME WIDGET TYPE IS NOT STORED ACROSS MORE THAN ONE
 *          SET WITH THE SAME KEY! This container really needs to be changed.
 */
static std::unordered_multimap<std::string, std::unordered_set<std::string>>
	SIGNALS = {
	// COMMON WIDGET SIGNALS //
	{ signal::PositionChanged, {} },
	{ signal::SizeChanged, {} },
	{ signal::Focused, {} },
	{ signal::Unfocused, {} },
	{ signal::MouseEntered, {} },
	{ signal::MouseLeft, {} },
	{ signal::ShowEffectFinished, {} },
	{ signal::AnimationFinished, {} },

	// CLICKABLE WIDGET SIGNALS //
	{ signal::MousePressed, CLICKABLE_WIDGETS },
	{ signal::MouseReleased, CLICKABLE_WIDGETS },
	{ signal::Clicked, CLICKABLE_WIDGETS },
	{ signal::RightMousePressed, CLICKABLE_WIDGETS },
	{ signal::RightMouseReleased, CLICKABLE_WIDGETS },
	{ signal::RightClicked, CLICKABLE_WIDGETS },

	// BUTTON SIGNALS //
	{ signal::Pressed, { type::Button, type::BitmapButton } },

	// COMBOBOX SIGNALS //
	{ signal::ItemSelected, { type::ComboBox } },

	// CHILDWINDOW SIGNALS //
	{ signal::MousePressed, CHILDWINDOW_WIDGETS },
	{ signal::Closed, CHILDWINDOW_WIDGETS },
	{ signal::EscapeKeyPressed, CHILDWINDOW_WIDGETS },
	{ signal::Minimized, CHILDWINDOW_WIDGETS },
	{ signal::Maximized, CHILDWINDOW_WIDGETS },
	{ signal::Closing, CHILDWINDOW_WIDGETS },

	// COLOURPICKER SIGNALS //
	{ signal::ColorChanged, { type::ColorPicker } },
	{ signal::OkPressed, { type::ColorPicker } },

	// EDITBOX SIGNALS //
	{ signal::TextChanged, { type::EditBox } },
	{ signal::ReturnKeyPressed, { type::EditBox } },
	{ signal::ReturnOrUnfocused, { type::EditBox } },
	{ signal::CaretPositionChanged, { type::EditBox } },

	// FILEDIALOG SIGNALS //
	{ signal::FileSelected, { type::FileDialog } },

	// VALUECHANGED SIGNAL //
	{ signal::ValueChanged, { type::Knob, type::Scrollbar, type::Slider,
		type::SpinButton, type::SpinControl } },

	// DOUBLECLICKED SIGNAL //
	{ signal::DoubleClicked, { type::Label, type::Picture, type::Panel } },

	// LISTBOX SIGNALS //
	{ signal::ItemSelected, { type::ListBox } },
	{ signal::MousePressed, { type::ListBox } },
	{ signal::MouseReleased, { type::ListBox } },
	{ signal::DoubleClicked, { type::ListBox } },

	// LISTVIEW SIGNALS //
	{ signal::ItemSelected, { type::ListView } },
	{ signal::HeaderClicked, { type::ListView } },
	{ signal::RightClicked, { type::ListView } },
	{ signal::DoubleClicked, { type::ListView } },

	// MENUBAR SIGNALS //
	{ signal::MenuItemClicked, { type::MenuBar } },

	// MESSAGEBOX SIGNALS //
	{ signal::ButtonPressed, { type::MessageBox } },

	// PROGRESSBAR SIGNALS //
	{ signal::ValueChanged, { type::ProgressBar } },
	{ signal::Full, { type::ProgressBar } },

	// RADIOBUTTON/CHECKBOX SIGNALS //
	{ signal::Checked, { type::RadioButton, type::CheckBox, type::ToggleButton } },
	{ signal::Unchecked, { type::RadioButton, type::CheckBox } },
	{ signal::Changed, { type::RadioButton, type::CheckBox } },

	// RANGESLIDER SIGNALS //
	{ signal::RangeChanged, { type::RangeSlider } },

	// TABCONTAINER SIGNALS //
	{ signal::SelectionChanging, { type::TabContainer } },
	{ signal::SelectionChanged, { type::TabContainer } },

	// TABS SIGNALS //
	{ signal::TabSelected, { type::Tabs } },

	// TEXTAREA SIGNALS //
	{ signal::TextChanged, { type::TextArea } },
	{ signal::SelectionChanged, { type::TextArea } },
	{ signal::CaretPositionChanged, { type::TextArea } },

	// TREEVIEW SIGNALS //
	{ signal::ItemSelected, { type::TreeView } },
	{ signal::DoubleClicked, { type::TreeView } },
	{ signal::Expanded, { type::TreeView } },
	{ signal::Collapsed, { type::TreeView } },
};
