/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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
	 * given their own TGUI panel to achieve this.
	 */
	class gui : public sfx::animated_drawable, public engine::json_script {
	public:
		/**
		 * Initialises the GUI object with a pointer to a collection of scripts and
		 * a name for the internal logger object.
		 * @param scripts Pointer to a scripts object containing functions to
		 *                handle GUI signals. An error is logged if \c nullptr is
		 *                given. Note that the scripts only have to be loaded by
		 *                the time signals are emitted; the given object does not
		 *                have to be fully initialised (i.e. interface registered
		 *                and scripts loaded) upon \c gui construction.
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
		 * Sets the spritesheet to use with the GUI menus.
		 * @param sheet A pointer to the \c animated_spritesheet to use with these
		 *              GUI menus.
		 */
		void setSpritesheet(std::shared_ptr<sfx::animated_spritesheet> sheet)
			noexcept;

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
		void signalHandler(tgui::Widget::Ptr widget, const std::string& signalName)
			noexcept;

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
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * Represents a GUI background.
		 * The background of a GUI can be either a solid colour or a sprite from a
		 * separate \c sfx::animated_spritesheet. This class manages switching from
		 * one to another so that the \c gui class knows which type of background
		 * to draw.
		 */
		class gui_background {
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
			 * Initialises the GUI background with a sprite ID.
			 * @param key The ID to set.
			 */
			gui_background(unsigned int key) noexcept;

			/**
			 * Initialises the GUI background with a solid colour.
			 * @param colour The colour to set.
			 */
			gui_background(sf::Color colour) noexcept;

			/**
			 * Sets this GUI background to be a sprite.
			 * The stored colour value is then ignored.
			 * @param key The sprite ID to set.
			 */
			void set(unsigned int key) noexcept;

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
			 * This method will simply return the ID that was last assigned.
			 * Therefore, you should use \c getType() to first test if this GUI
			 * background is actually meant to represent a sprite background.
			 * @return The sprite ID that was last assigned.
			 */
			unsigned int getSprite() const noexcept;

			/**
			 * Retrieves the assigned colour value.
			 * This method will simply return the colour that was last assigned.
			 * Therefore, you should use \c getType() to first test if this GUI
			 * background is actually meant to represent a solid colour background.
			 * @return The colour that was last assigned.
			 */
			sf::Color getColour() const noexcept;
		private:
			/**
			 * Tracks the type of background this instantiation represents.
			 */
			type _flag = type::Colour;

			/**
			 * The sprite ID.
			 */
			unsigned int _key = 0;

			/**
			 * The solid colour value.
			 */
			sf::Color _colour;
		};

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
		 * All key-value pairs within the root object store information related to
		 * each GUI menu. The key holds the name of the GUI, and the value, an
		 * object, lists details of the menu.\n
		 * 
		 * The following key-value pairs should be specified in this object value:
		 * <ul><li>\c path - The path of the TXT script containing the details of
		 *         the menu's widgets. If none is given, a warning will be logged,
		 *         and the GUI will not be loaded (does \b not mean \c _load()
		 *         returns \c FALSE, however).</li>
		 *     <li>\c background - Must either be a colour value ([r,g,b,a]), or an
		 *         unsigned integer representing the ID of a sprite. Defaults to
		 *         solid black.</li>
		 *     <li>Additional keys must be valid animated widget names having an
		 *         object value containing a \c "sprite" key and value pair
		 *         specifying the ID of the sprite to animate and draw with that
		 *         widget (see \c _guiSpriteKeys).</li></ul>
		 * 
		 * The TXT scripts can be written by hand, but it is a lot easier to create
		 * and edit them using the bundled \c gui-builder application (developed by
		 * the sole maintainer of TGUI, Bruno Van de Velde), and then make a few
		 * small changes in a text editor as necessary.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if all GUIs could be loaded, \c FALSE if at least one
		 *         couldn't (\c _loadGUI() returned \c FALSE at least once).
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
		 * Loads a GUI menu.
		 * @param  name     The name of the GUI menu to load.
		 * @param  filepath The path of the TXT script which contains the details
		 *                  of the menu.
		 * @return \c TRUE if loading was successful, \c FALSE if not.
		 */
		bool _loadGUI(const std::string& name, const std::string& filepath)
			noexcept;

		/**
		 * Connects all signals of a widget to this class' \c signalHandler()
		 * method.
		 * @param widget The widget whose signals are to be connected.
		 */
		void _connectSignals(tgui::Widget::Ptr widget) noexcept;

		/**
		 * Retrieves the next widget within the current GUI that is of a specified
		 * type.
		 * If the type given was different from the type given in the last call to
		 * this method, searching will start from the beginning of the widget list.
		 * This is also the case if it's the first time that this method is being
		 * called, or if the last returned value was \c nullptr.
		 * @tparam T    The type of TGUI widget to search for.
		 * @param  type The TGUI's string name for the type of widget to search
		 *              for. This needs to match with \c T.
		 * @return Pointer to the next widget in the current GUI's list that
		 *         matches the given type, or \c nullptr if the end of the list has
		 *         been reached and a matching widget was not found.
		 */
		template<typename T>
		std::shared_ptr<T> _getNextWidget(const std::string& type) noexcept;

		/**
		 * The TGUI widget type last provided to \c _getNextWidget().
		 */
		std::string _lastWidgetType = "";

		/**
		 * The vector index keeping track of \c _getNextWidget() progress.
		 * Is reset whenever a new type is given, or whenever a new GUI is set.
		 */
		std::size_t _widgetIndex = 0;

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
		 * Pointer to the animated spritesheet to use with the GUI menus.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet = nullptr;

		/**
		 * Used to paint the solid colour background of a GUI menu.
		 */
		sf::RectangleShape _bgColour;

		/**
		 * Used to paint the animated sprite background of a GUI menu.
		 */
		sfx::animated_sprite _bgSprite;

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
		 * Stores the sprite IDs of all the widgets containing pictures that are
		 * configured to be animated.
		 * It stores them by panel, then by widget. The name of the GUI menu is to
		 * be specified first, then the name of the animated widget.
		 * @remark Only \c BitmapButtons and \c Pictures can animate!
		 */
		std::unordered_map<std::string,
			std::unordered_map<std::string, unsigned int>> _guiSpriteKeys;

		/**
		 * Pointer to the language dictionary used to translate all captions.
		 */
		std::shared_ptr<engine::language_dictionary> _langdict = nullptr;
	};
}

template<typename T>
std::shared_ptr<T> sfx::gui::_getNextWidget(const std::string& type) noexcept {
	if (_lastWidgetType != type) {
		_widgetIndex = 0;
		_lastWidgetType = type;
	}
	auto& widgetList = _gui.get<tgui::Group>(getGUI())->getWidgets();
	while (_widgetIndex < widgetList.size()) {
		auto& widget = widgetList.at(_widgetIndex++);
		if (widget->getWidgetType() == _lastWidgetType) {
			return std::dynamic_pointer_cast<T>(widget);
		}
	}
	// no match found
	_widgetIndex = 0;
	return nullptr;
}