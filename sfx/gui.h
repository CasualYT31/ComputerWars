/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file gui.h
 * Defines code related to managing GUIs.
 * The backend used with this class is TGUI.
 */

#pragma once

#include "tgui/tgui.hpp"
#include "texture.h"
#include "script.h"
#include "language.h"

namespace sfx {
	/**
	 * Represents a collection of GUI menus.
	 * This class can only display one menu at a time, however it loads all menus
	 * it is given via \c load() so that they can be switched quickly. Each menu is
	 * given their own TGUI group to achieve this.
	 */
	class gui : public sfx::animated_drawable, public engine::json_script {
	public:
		/**
		 * Initialises the GUI object with a pointer to a \c scripts object and a
		 * name for the internal logger object.
		 * @param scripts Pointer to a scripts object containing functions to
		 *                handle GUI signals. An error is logged if \c nullptr is
		 *                given. <b>It is important to note that the given scripts
		 *                object should <em>not</em> have any scripts loaded, as
		 *                this constructor adds to the scripts' interface.</b>
		 *                Scripts must be loaded \em after this class is
		 *                instantiated but \em before it is <tt>load()</tt>ed.
		 *                <b>It is also important to note that there should only be
		 *                <em>one</em> instance of \c scripts for every instance of
		 *                \c gui,</b> as the different \c gui functions belonging
		 *                to the different instances will conflict with each other.
		 * @param name    The name to give this particular instantiation within the
		 *                log file. Defaults to "gui."
		 * @sa    \c engine::logger
		 */
		gui(const std::shared_ptr<engine::scripts>& scripts,
			const std::string& name = "gui") noexcept;

		/**
		 * Sets the GUI menu to display.
		 * No widgets are actually destroyed: they are only made visible/invisible.
		 * @param newPanel The name of the menu to display. If a menu with the
		 *                 given name does not exist, an error will be logged and
		 *                 no other changes will occur.
		 */
		void setGUI(const std::string& newPanel) noexcept;

		/**
		 * Gets the name of the menu currently showing.
		 * @return The menu currently being drawn.
		 */
		std::string getGUI() const noexcept;

		/**
		 * Adds a spritesheet which can be uses with the GUI menus.
		 * If a spritesheet with the given name already exists, a warning will be
		 * logged and the spritesheet will be updated.
		 * @param name  The name which scripts use to reference the spritesheet.
		 * @param sheet A pointer to the \c animated_spritesheet to use with these
		 *              GUI menus.
		 */
		void addSpritesheet(const std::string& name,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet) noexcept;

		/**
		 * Sets the target to draw the GUI menus to.
		 * @remark This method was introduced because of the limitations of the
		 *         TGUI. Ideally, this method should be removed altogether if at
		 *         all possible in the future. Please see \c engine::gui::draw()
		 *         for more details.
		 * @param  newTarget Reference to the new target to set.
		 */
		void setTarget(sf::RenderTarget& newTarget) noexcept;

		/**
		 * Allows the TGUI to handle SFML window events.
		 * Should be called within the event handling portion of the drawing loop.
		 * @param  e The event to handle. All polled events should be given
		 *           eventually.
		 * @return \c FALSE if all widgets ignored the given event, \c TRUE
		 *         otherwise.
		 */
		bool handleEvent(sf::Event e) noexcept;

		/**
		 * Handles all widget signals by attempting to invoke the necessary script
		 * function.
		 * In order to react to widget signals (such as button presses), nothing
		 * has to be changed within any JSON or TXT script. Instead, within one of
		 * your \c engine::scripts, the following function should be defined:
		 * \code
		 * void GUIName_WidgetName_SignalName() {
		 *     // code
		 * }
		 * \endcode
		 * Where \c GUIName is the menu containing the widget to handle,
		 * \c WidgetName is the name of the widget in question, and \c SignalName
		 * is the name of the signal sent. If the function of a particular signal
		 * was not defined, the signal will simply be ignored.\n
		 * For a list of available signals, you can view the definition of
		 * \c _connectSignals() in \c gui.cpp.
		 * @param widget     The widget sending the signal.
		 * @param signalName The name of the signal being sent.
		 */
		void signalHandler(tgui::Widget::Ptr widget,
			const tgui::String& signalName) noexcept;

		/**
		 * Sets the \c language_dictionary object to use with these GUI menus.
		 * If a language dictionary is given, all GUI captions will be feeded into
		 * it for translation purposes, during the call to \c animate(). If one is
		 * not given, which is the default, GUI text will not be translated or
		 * amended.
		 * @param lang Pointer to the \c language_dictionary to use. \c nullptr can
		 *             be given to disable translation.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& lang) noexcept;

		/**
		 * Animates the current GUI menu.
		 * Any sprites are animated, and the colour background (if there is one) is
		 * resized to match the size of the target. In addition, if a langauge
		 * dictionary has been given, all captions will be translated.
		 * @return Always returns \c FALSE.
		 * @sa     sfx::gui::setLanguageDictionary()
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * Represents a GUI background.
		 * The background of a GUI can be either a solid colour or a sprite from a
		 * separate \c sfx::animated_spritesheet. This class manages switching from
		 * one to another so that the \c gui class knows which type of background
		 * to draw.
		 */
		class gui_background : public sfx::animated_drawable {
		public:
			/**
			 * Used to identify which type of background this instantiation
			 * represents.
			 */
			enum class type {
				Sprite,
				Colour
			};

			/**
			 * Default constructor.
			 * By default, a GUI background is a solid colour of black.
			 */
			gui_background() noexcept;

			/**
			 * Initialises the GUI background with a sprite.
			 * @param sheet The sheet containing the sprite.
			 * @param key   The name of the sprite to set.
			 */
			gui_background(
				const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
				const std::string& key) noexcept;

			/**
			 * Initialises the GUI background with a solid colour.
			 * @param colour The colour to set.
			 */
			gui_background(sf::Color colour) noexcept;

			/**
			 * Sets this GUI background to be a sprite.
			 * The stored colour value is then ignored.
			 * @param sheet Pointer to the spritesheet containing the sprite. If
			 *              \c nullptr is given, a new spritesheet won't be
			 *              applied. This can be used to set a new sprite on the
			 *              same sheet.
			 * @param key   The sprite name to set.
			 */
			void set(const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
				const std::string& key) noexcept;

			/**
			 * Sets this GUI background to be a solid colour.
			 * The stored sprite ID is then ignored.
			 * @param colour The colour to set.
			 */
			void set(sf::Color colour) noexcept;

			/**
			 * Retrieves the type of this GUI background.
			 * @return The type.
			 */
			type getType() const noexcept;

			/**
			 * Retrieves the assigned sprite ID.
			 * This method will simply return the name that was last assigned.
			 * Therefore, you should use \c getType() to first test if this GUI
			 * background is actually meant to represent a sprite background.
			 * @return The sprite name that was last assigned.
			 */
			std::string getSprite() const noexcept;

			/**
			 * Retrieves the assigned colour value.
			 * This method will simply return the colour that was last assigned.
			 * Therefore, you should use \c getType() to first test if this GUI
			 * background is actually meant to represent a solid colour background.
			 * @return The colour that was last assigned.
			 */
			sf::Color getColour() const noexcept;

			/**
			 * Animates this background.
			 * Any sprites are animated, and the colour background (if there is
			 * one) is resized to match the size of the target.
			 * @return Always returns \c FALSE if a colour background. Returns the
			 *         result of the call to the sprite's \c animate() method if a
			 *         sprite background.
			 */
			virtual bool animate(const sf::RenderTarget& target,
				const double scaling = 1.0) noexcept;
		private:
			/**
			 * Draws the GUI background.
			 */
			virtual void draw(sf::RenderTarget& target, sf::RenderStates states)
				const;

			/**
			 * Tracks the type of background this instantiation represents.
			 */
			type _flag = type::Colour;

			/**
			 * Used to paint the solid colour background.
			 */
			sf::RectangleShape _bgColour;

			/**
			 * Used to paint the animated sprite background.
			 */
			sfx::animated_sprite _bgSprite;
		};

		/**
		 * Performs animation calculations on a container of widgets.
		 * @param target         The target which the GUI will be drawn to later.
		 * @param scaling        Scaling factor which will be applied when drawing.
		 * @param container      Pointer to the container widget.
		 * @param baseName       The name of the container widget.
		 * @param lang           \c TRUE if widget strings need to be translated,
		 *                       \c FALSE if they should not be renamed.
		 * @param animatedSprite Counter which keeps track of which animated sprite
		 *                       from the internal vector to work with.
		 */
		void _animate(const sf::RenderTarget& target, const double scaling,
			tgui::Container::Ptr container, std::string baseName, const bool lang,
			std::size_t& animatedSprite) noexcept;

		/**
		 * Draws the current GUI menu.
		 * @warning It is to be noted that this implementation of \c draw()
		 *          \b ignores any given render states. This was done to remain
		 *          consistent with TGUI (which does not seem to allow
		 *          \c sf::RenderStates). The internal \c tgui::Gui object also
		 *          ignores the given \c target when drawing: it must be previously
		 *          set with \c setTarget().
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * The JSON load method for this class.
		 * There should be only one key-value pair in this script called "menus".
		 * The value should be an array of string names, each one naming a menu.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if all existing menus before the call were deleted, \c
		 *         FALSE if not.
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Connects all signals of a widget to this class' \c signalHandler()
		 * method.
		 * @param widget The widget whose signals are to be connected.
		 */
		void _connectSignals(tgui::Widget::Ptr widget) noexcept;

		/**
		 * Converts a generic widget pointer to a pointer of the correct type for
		 * the given widget.
		 * @warning This method assumes that a GUI menu has already been set!
		 *          Behaviour is undefined if one has not already been set and this
		 *          method is called.
		 * @tparam  T      The type of widget pointer to retrieve.
		 * @param   widget The generic widget pointer to convert.
		 * @return  The correctly-typed widget pointer.
		 */
		template<typename T>
		typename T::Ptr _getPtr(const tgui::Widget::Ptr& widget) const noexcept;

		/**
		 * Finds a widget in the root GUI object and returns it.
		 * The name of the widget should be in the format
		 * "[MenuName[.ContainerNames].]WidgetName". If MenuName is not given,
		 * whatever is in \c _currentGUI will be inserted.
		 * @tparam T         The type of widget to find.
		 * @param  name      The name of the widget to find.
		 * @param  namesList The full name of the widget, split up. This is
		 *                   generated by the method and it can be dropped by
		 *                   passing \c nullptr.
		 * @return The pointer to the widget.
		 */
		template<typename T>
		typename T::Ptr _findWidget(std::string name,
			std::vector<std::string>* namesList = nullptr) const noexcept;

		//////////////////////
		// SCRIPT INTERFACE //
		//////////////////////

		/**
		 * Callback given to \c engine::scripts::registerInterface().
		 * @warning Do not allow scripts to destroy widgets! At least not widgets
		 *          from the current GUI. This is because my animated sprite code
		 *          for BitmapButtons and Pictures relies on the order of widget
		 *          retrieval to remain the same.
		 * @param   engine Pointer to the engine to register the interface with.
		 * @sa      \c engine::scripts::registerInterface()
		 */
		void _registerInterface(asIScriptEngine* engine) noexcept;

		// BACKGROUND //

		/**
		 * Removes a menu's background.
		 * @param menu The name of the menu whose background is to be removed. If a
		 *             blank string is given, the current menu is chosen.
		 */
		void _noBackground(const std::string& menu = "") noexcept;

		/**
		 * Makes a menu have an animated sprite for a background.
		 * @param sheet  The name of the spritesheet which contains the sprite to
		 *               draw.
		 * @param sprite The name of the sprite to draw.
		 * @param menu   The name of the menu to apply the animated sprite to the
		 *               background of. If a blank string is given, the current
		 *               menu is chosen.
		 */
		void _spriteBackground(const std::string& sheet, const std::string& sprite,
			std::string menu = "") noexcept;

		/**
		 * Makes a menu have a solid colour background.
		 */
		void _colourBackground(const unsigned int r, const unsigned int g,
			const unsigned int b, const unsigned int a,
			std::string menu = "") noexcept;

		// LISTBOX //

		/**
		 * Creates a listbox.
		 * This method writes an error to the log file if a widget with the given
		 * name already exists. When an error is logged, no listbox is created.
		 * @param name The name of the listbox.
		 * @param x    The X location of the listbox.
		 * @param y    The Y location of the listbox.
		 * @param w    The width of the listbox.
		 * @param h    The height of the listbox.
		 */
		void _addListbox(const std::string& name, const float x, const float y,
			const float w, const float h) noexcept;

		/**
		 * Appends a new item to a listbox.
		 * This method writes an error to the log file if a listbox with the given
		 * name could not be found. When an error is logged, no item is added.
		 * @param name The name of the listbox to amend.
		 * @param item The text of the new item.
		 */
		void _addListboxItem(const std::string& name, const std::string& item)
			noexcept;

		//////////
		// DATA //
		//////////

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Stores all the GUI menus panels.
		 * @remark Unfortunately, we have to make this mutable so that the
		 *         \c tgui::Gui::draw() method can be called in this class'
		 *         \c draw() implementation.
		 */
		mutable tgui::Gui _gui;

		/**
		 * Stores the name of the GUI currently being displayed.
		 */
		std::string _currentGUI;

		/**
		 * Stores the background information for each GUI menu.
		 */
		std::unordered_map<std::string, gui::gui_background> _guiBackground;

		/**
		 * Pointer to the scripts object containing the signal handler functions.
		 */
		std::shared_ptr<engine::scripts> _scripts = nullptr;

		/**
		 * Pointer to the animated spritesheets that can be used with the GUI
		 * menus.
		 */
		std::unordered_map<std::string, std::shared_ptr<sfx::animated_spritesheet>>
			_sheet;

		/**
		 * Stores a list of TGUI textures to draw with the animated widgets.
		 * Cleared and refilled upon every call to \c animate().
		 */
		std::vector<tgui::Texture> _widgetPictures;

		/**
		 * Stores a list of animated sprites associated with each animated widget.
		 * Cleared every call to \c setGUI(). Refilled once upon the first call to
		 * \c animate() since \c setGUI() was last called.
		 */
		std::vector<sfx::animated_sprite> _widgetSprites;

		/**
		 * Stores the sprite names of all the widgets containing pictures that are
		 * configured to be animated.
		 * The value holds the name of the spritesheet and then the name of the
		 * sprite.
		 * @remark Only \c BitmapButtons and \c Pictures can animate!
		 */
		std::unordered_map<std::string, std::pair<std::string, std::string>>
			_guiSpriteKeys;

		/**
		 * Stores the original captions assigned to each widget.
		 */
		std::unordered_map<std::string, std::vector<std::string>> _originalStrings;

		/**
		 * Pointer to the language dictionary used to translate all captions.
		 */
		std::shared_ptr<engine::language_dictionary> _langdict = nullptr;

		/**
		 * The last known language set in \c _langdict.
		 */
		std::string _lastlang = "";
	};
}

template<typename T>
typename T::Ptr sfx::gui::_getPtr(const tgui::Widget::Ptr& widget) const noexcept {
	return _gui.get<tgui::Group>(getGUI())->get<T>(widget->getWidgetName());
}

template<typename T>
typename T::Ptr sfx::gui::_findWidget(std::string name,
	std::vector<std::string>* namesList) const noexcept {
	// split string
	std::vector<std::string> names;
	std::size_t pos = 0;
	do {
		pos = name.find('.');
		names.push_back(name.substr(0, pos));
		if (pos != std::string::npos) name = name.substr(pos + 1);
	} while (pos != std::string::npos);
	// if group name was not given, insert it
	if (!_gui.get<tgui::Group>(names[0])) names.insert(names.begin(), _currentGUI);
	if (namesList) {
		namesList->clear();
		*namesList = names;
	}
	// find it
	tgui::Container::Ptr container = _gui.get<tgui::Container>(names[0]);
	if (names.size() > 2) {
		for (std::size_t w = 1; w < names.size() - 1; w++) {
			if (!container) return nullptr;
			container = container->get<tgui::Container>(names[w]);
		}
	}
	if (container)
		return container->get<T>(names[names.size() - 1]);
	else
		return nullptr;
}