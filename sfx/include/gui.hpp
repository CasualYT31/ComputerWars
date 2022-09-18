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

/**@file gui.hpp
 * Defines code related to managing GUIs.
 * The backend used with this class is TGUI.
 */

#pragma once

#include "tgui/tgui.hpp"
#include "texture.hpp"
#include "script.hpp"
#include "language.hpp"
#include "userinput.hpp"
#include "fonts.hpp"

namespace sfx {
	/**
	 * Represents a collection of GUI menus.
	 * This class can only display one menu at a time, however it loads all menus
	 * it is given via \c load() so that they can be switched quickly. Each menu is
	 * given their own TGUI group to achieve this.\n
	 * @b Important! There is always @b one menu defined, called \c MainMenu. This
	 * will be the menu that is displayed by the @c gui instance once
	 * <tt>load()</tt>ed.
	 */
	class gui : public sfx::animated_drawable, public engine::script_registrant,
		public engine::json_script {
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
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * GUI functions with a \c scripts object.
		 * @sa \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Sets the GUI menu to display.
		 * No widgets are actually destroyed: they are only made visible/invisible.
		 * @param newPanel  The name of the menu to display. If a menu with the
		 *                  given name does not exist, an error will be logged and
		 *                  no other changes will occur.
		 * @param callClose If @c TRUE, the current menu's @c Close() script
		 *                  function will be called, if it exists.
		 * @param callOpen  If @c TRUE, the new menu's @c Open() script function
		 *                  will be called, if it exists.
		 */
		void setGUI(const std::string& newPanel, const bool callClose = true,
			const bool callOpen = true) noexcept;

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
		 * Allows the current menu to handle input via AngelScript.
		 * Each menu should have a function called
		 * <tt><em>MenuName</em>HandleInput(const dictionary in)</tt> which accepts
		 * a dictionary of control inputs (as defined by the @c user_input instance
		 * given) which map to <tt>bool</tt>s. For example, if some control
		 * @c "select" maps to @c TRUE, then you can handle the @c "select"
		 * control. @c user_input handles all the details of when a control is
		 * triggered, so you don't have to worry about tracking the state of a
		 * particular control, e.g. to ensure that it is only triggered "once."
		 * You can just check for it directly and perform code whenever it happens.
		 * @param ui Pointer to the @c user_input instance to send information on.
		 *           Note that the @c user_input::update() method should already
		 *           have been called before a call to @c handleInput().
		 */
		void handleInput(const std::shared_ptr<sfx::user_input>& ui) noexcept;

		/**
		 * Handles all widget signals by attempting to invoke the necessary script
		 * function.
		 * In order to react to widget signals (such as button presses), nothing
		 * has to be changed within any JSON script. Instead, within one of your
		 * \c engine::scripts, the following function should be defined:
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
		 * If a language dictionary is given, all GUI captions will be fed into it
		 * for translation purposes, during the call to \c animate(). If one is not
		 * given, which is the default, GUI text will not be translated or amended.
		 * @param lang Pointer to the \c language_dictionary to use. \c nullptr can
		 *             be given to disable translation.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& lang) noexcept;

		/**
		 * Sets the \c fonts object to use with these GUI menus.
		 * If a \c fonts object isn't given to a \c gui object, then the default
		 * font will be used for all GUI text.\n
		 * Setting a new \c fonts object will not update any of the fonts in use,
		 * i.e. the old fonts will still be active. Fonts will have to be
		 * manually reapplied as necessary.
		 * @param fonts Pointer to the \c fonts object to use. \c nullptr can be
		 *              given is so desired.
		 */
		void setFonts(const std::shared_ptr<sfx::fonts>& fonts) noexcept;

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
		 * @param target    The target which the GUI will be drawn to later.
		 * @param scaling   Scaling factor which will be applied when drawing.
		 * @param container Pointer to the container widget.
		 */
		void _animate(const sf::RenderTarget& target, const double scaling,
			tgui::Container::Ptr container) noexcept;

		/**
		 * Reapplies all the translations to a widget recursively.
		 * @param container Pointer to the widget to translate.
		 */
		void _translateWidget(tgui::Widget::Ptr widget) noexcept;

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
		 * The value should be an array of string names, each one naming a menu.\n
		 * For each menu defined, the script function <tt><em>MenuName</em>
		 * SetUp()</tt> is called, which accepts no parameters and does not return
		 * any value. This is intended to be used to setup that menu, such as
		 * adding all of its controls, and setting the background. If an
		 * <tt>engine::script</tt> object was not given, then this function will
		 * not be called, and the menu will remain empty.
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
		 * Determines whether or not the given widget type is a container type.
		 * @param  type The TGUI widget type.
		 * @return \c TRUE if the widget type is a container, \c FALSE otherwise.
		 */
		static inline bool _isContainerWidget(tgui::String type) noexcept;

		/**
		 * Removes widgets from \c _gui.
		 * @warning A grid within a grid may cause the game to crash!
		 * @param widget    Pointer to the widget to delete.
		 * @param container Pointer to the container which holds the widget.
		 * @param removeIt  For containers: if \c FALSE, this will only remove all
		 *                  the widgets within a container, but not the container
		 *                  itself. If \c TRUE, this will ensure that the container
		 *                  itself is also deleted, as well as all of its child
		 *                  widgets.\n
		 *                  For widgets: if \c FALSE, the widget's associated data
		 *                  will be erased, but the widget itself won't be erased.
		 *                  If \c TRUE, the widget and its data will be erased.
		 *                  This is required for \c Grid widgets, who cause the
		 *                  game to crash if it attempts to remove a grid whose
		 *                  widgets have already been erased.
		 */
		void _removeWidgets(const tgui::Widget::Ptr& widget,
			const tgui::Container::Ptr& container, const bool removeIt) noexcept;

		/**
		 * Extracts a widget's short name from its full name.
		 * @param  fullname The full name of the widget.
		 * @return The short name of the widget.
		 */
		static inline std::string _extractWidgetName(const tgui::String& fullname)
			noexcept;

		/**
		 * Creates a widget of a given type and returns it.
		 * Logs an error if the given type isn't supported. In this case, no widget
		 * will be created.
		 * @param  wType The type of widget to create. Must be all lower case.
		 * @param  name  The fullname of the widget, which will be assigned later.
		 * @param  menu  The name of the menu which the widget will later be added
		 *               to.
		 * @return Pointer to the new widget, or \c nullptr if it couldn't be
		 *         created.
		 */
		tgui::Widget::Ptr _createWidget(const std::string& wType,
			const std::string& name, const std::string& menu) noexcept;

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
		 * @param  fullname  The full name of the widget as a string. This is
		 *                   generated by the method and it can be dropped by
		 *                   passing \c nullptr.
		 * @return The pointer to the widget.
		 */
		template<typename T>
		typename T::Ptr _findWidget(std::string name,
			std::vector<std::string>* namesList = nullptr,
			std::string* fullname = nullptr) const noexcept;

		//////////////////////
		// SCRIPT INTERFACE //
		//////////////////////

		// SET GUI //

		/**
		 * Calls @c setGUI(), passing @c TRUE to both @c callClose and @c callOpen.
		 * @sa @c sfx::gui::setGUI()
		 */
		void _setGUI(const std::string& name) noexcept;

		// BACKGROUND //

		/**
		 * Removes a menu's background.
		 * @param menu The name of the menu whose background is to be removed. If a
		 *             blank string is given, the current menu is chosen.
		 */
		void _noBackground(std::string menu) noexcept;

		/**
		 * Makes a menu have an animated sprite for a background.
		 * @param menu   The name of the menu to apply the animated sprite to the
		 *               background of. If a blank string is given, the current
		 *               menu is chosen.
		 * @param sheet  The name of the spritesheet which contains the sprite to
		 *               draw.
		 * @param sprite The name of the sprite to draw.
		 */
		void _spriteBackground(std::string menu, const std::string& sheet,
			const std::string& sprite) noexcept;

		/**
		 * Makes a menu have a solid colour background.
		 * @param menu The name of the menu to apply the colour to the background
		 *             of. If a blank string is given, the current menu is chosen.
		 * @param r    The red attribute.
		 * @param g    The green attribute.
		 * @param b    The blue attribute.
		 * @param a    The alpha attribute.
		 */
		void _colourBackground(std::string menu, const unsigned int r,
			const unsigned int g, const unsigned int b, const unsigned int a)
			noexcept;

		// WIDGETS //

		/**
		 * Determines if a widget exists.
		 * @param  name The name of the widget to search for.
		 * @return \c TRUE if a widget with the given name exists, \c FALSE
		 *         otherwise.
		 */
		bool _widgetExists(const std::string& name) noexcept;

		/**
		 * Creates a new widget and adds it to a container.
		 * If no menu is specified, the currently displayed menu is chosen. This is
		 * true for all GUI global script functions.\n
		 * If the @c type parameter was invalid, or if a widget with the specified
		 * name already existed at the time of the call, then an error will be
		 * logged and no widget will be created.
		 * @param widgetType The type of widget to create.
		 * @param name       The name of the new widget.
		 */
		void _addWidget(const std::string& widgetType, const std::string& name)
			noexcept;

		/**
		 * Creates a widget and adds it to a grid.
		 * An error will be logged in the following circumstances:
		 * <ul><li>If the @c type parameter was invalid.</li>
		 *     <li>If the second to last short name in the full name of the new
		 *         widget did not identify a grid.</li>
		 *     <li>If there was already a widget within the specified cell.
		 *     </li></ul>
		 * If an error is logged, no widget will be created or added.
		 * @param widgetType The type of widget to create.
		 * @param name       The name of the new widget. Must include the grid.
		 * @param row        The row to add the widget to.
		 * @param col        The column to add the widget to.
		 */
		void _addWidgetToGrid(const std::string& widgetType,
			const std::string& name, const std::size_t row, const std::size_t col)
			noexcept;

		/**
		 * Removes a specified widget, and all the widgets that are within it.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be removed.
		 * @param name The name of the widget to remove.
		 */
		void _removeWidget(const std::string& name) noexcept;

		/**
		 * Removes all the widgets from a given container.
		 * If no widget exists with the given name, or if the widget wasn't a
		 * container, then an error will be logged and no widget will be removed.
		 * @param name The name of the container whose widgets are to be removed.
		 */
		void _removeWidgetsFromContainer(const std::string& name) noexcept;

		/**
		 * Sets a widget's font.
		 * If no widget or font exists with the given name, then an error will be
		 * logged and no font will be changed.
		 * @param name     The name of the widget to change.
		 * @param fontName The name of the font to assign to the widget.
		 */
		void _setWidgetFont(const std::string& name, const std::string& fontName)
			noexcept;

		/**
		 * Sets the global UI font.
		 * If no font exists with the given name, then an error will be logged and
		 * no font will be changed.
		 * @param fontName The name of the font to assign.
		 */
		void _setGlobalFont(const std::string& fontName) noexcept;

		/**
		 * Updates a widget's location.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be moved.
		 * @param name The name of the widget to relocate.
		 * @param x    The expression to assign to the widget's X coordinate.
		 * @param y    The expression to assign to the widget's Y coordinate.
		 */
		void _setWidgetPosition(const std::string& name, const std::string& x,
			const std::string& y) noexcept;

		/**
		 * Updates a widget's origin.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be updated.
		 * @param name The name of the widget to update.
		 * @param x    The new origin to set along the widget's X axis (0.0-1.0).
		 * @param y    The new origin to set along the widget's Y axis (0.0-1.0).
		 */
		void _setWidgetOrigin(const std::string& name, const float x,
			const float y) noexcept;

		/**
		 * Updates a widget's size.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be resized.
		 * @param name The name of the widget to resize.
		 * @param w    The expression to assign to the widget's width.
		 * @param h    The expression to assign to the widget's height.
		 */
		void _setWidgetSize(const std::string& name, const std::string& w,
			const std::string& h) noexcept;

		/**
		 * Updates a widget's visibility property.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.
		 * @param name    The name of the widget to show/hide.
		 * @param visible \c TRUE if the widget should be shown, \c FALSE if it
		 *                should be hidden.
		 */
		void _setWidgetVisibility(const std::string& name, const bool visible)
			noexcept;

		/**
		 * Updates a widget's caption.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name The name of the widget to change.
		 * @param text The new caption.
		 */
		void _setWidgetText(const std::string& name, const std::string& text)
			noexcept;

		/**
		 * Sets a widget's character size.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no size will be changed.
		 * @param name The name of the widget to change.
		 * @param size The new character size.
		 */
		void _setWidgetTextSize(const std::string& name, const unsigned int size)
			noexcept;

		/**
		 * Sets a widget's text colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name   The name of the widget to change.
		 * @param colour The new text colour.
		 */
		void _setWidgetTextColour(const std::string& name, const sf::Color& colour)
			noexcept;

		/**
		 * Sets a widget's text outline colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name   The name of the widget to change.
		 * @param colour The new text outline colour.
		 */
		void _setWidgetTextOutlineColour(const std::string& name,
			const sf::Color& colour) noexcept;

		/**
		 * Sets a widget's text outline thickness.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name      The name of the widget to change.
		 * @param thickness The new text outline thickness.
		 */
		void _setWidgetTextOutlineThickness(const std::string& name,
			const float thickness) noexcept;

		/**
		 * Updates a widget's sprite.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no sprite will be set.
		 * @param name  The name of the widget to change.
		 * @param sheet The name of the spritesheet which contains the sprite to
		 *              set.
		 * @param key   The name of the sprite to set.
		 */
		void _setWidgetSprite(const std::string& name, const std::string& sheet,
			const std::string& key) noexcept;

		/**
		 * Configures a widget to always resize to match its sprite's size.
		 * If you set this to \c FALSE for a widget who used to be set to \c TRUE,
		 * then the size of the widget will be left at the last set sprite and will
		 * no longer automatically update. If you set this to \c TRUE, and you
		 * attempt to manually set the size to something else, that will be
		 * overriden by the sprite's size.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no change will be made.
		 * @param name            The name of the widget to change.
		 * @param overrideSetSize The new value.
		 */
		void _matchWidgetSizeToSprite(const std::string& name,
			const bool overrideSetSize) noexcept;

		/**
		 * Sets a widget's background colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no colour will be set.
		 * @param name   The name of the widget to change.
		 * @param colour The colour to set to the background of the widget.
		 */
		void _setWidgetBgColour(const std::string& name, const sf::Color& colour)
			noexcept;

		/**
		 * Sets a widget's border size.
		 * Sets each side's border to the same size.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no border size will be set.
		 * @param name The name of the widget to change.
		 * @param size The size to set to the widget's border.
		 */
		void _setWidgetBorderSize(const std::string& name, const float size)
			noexcept;

		/**
		 * Sets a widget's border colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no colour will be set.
		 * @param name   The name of the widget to change.
		 * @param colour The colour to set to the widget's border.
		 */
		void _setWidgetBorderColour(const std::string& name,
			const sf::Color& colour) noexcept;

		/**
		 * Sets a widget's rounded border radius.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no radius will be set.
		 * @param name   The name of the widget to change.
		 * @param radius The radius to set to the widget's rounded border.
		 */
		void _setWidgetBorderRadius(const std::string& name, const float radius)
			noexcept;

		/**
		 * Sets a widget's index within its container.
		 * Used to reorder widgets within the container.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no index will be set. An
		 * error will also be logged if a given index is too high.
		 * @param name  The name of the widget to change.
		 * @param index The new index to set to the widget.
		 */
		void _setWidgetIndex(const std::string& name, const std::size_t index)
			noexcept;

		/**
		 * Updates a widget's index within a given container.
		 * Used to reorder widgets within the container.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no index will be set. Errors
		 * will also be logged if either index is invalid.
		 * @param name     The name of the container containing the widget to
		 *                 update.
		 * @param oldIndex The index of the widget to update.
		 * @param newIndex The new index of the widget.
		 */
		void _setWidgetIndexInContainer(const std::string& name,
			const std::size_t oldIndex, const std::size_t newIndex) noexcept;

		/**
		 * Sets a widget's size in relation to others in its layout.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no ratio will be set. An
		 * error will also be logged if the given index was too high.
		 * @param name  The name of the vertical or horizontal layout to change.
		 * @param index The 0-based index of the widget to readjust.
		 * @param ratio The ratio to apply.
		 */
		void _setWidgetRatioInLayout(const std::string& name,
			const std::size_t index, const float ratio) noexcept;

		/**
		 * Adds an item to a widget.
		 * E.g. appends an item to a listbox.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be added.
		 * @param name The name of the widget to add the item to.
		 * @param text The text of the new item.
		 */
		void _addItem(const std::string& name, const std::string& text) noexcept;

		/**
		 * Clears all items from a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be removed.
		 * @param name The name of the widget to add the item to.
		 */
		void _clearItems(const std::string& name) noexcept;

		/**
		 * Selects an item by index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be selected. An
		 * error will also be reported if the given index was out of range.
		 * @param name  The name of the widget which contains the item to select.
		 * @param index The 0-based index of the item to select.
		 */
		void _setSelectedItem(const std::string& name, const std::size_t index)
			noexcept;

		/**
		 * Gets the currently selected item's index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c -1 will be returned. If
		 * no item was selected, \c -1 will also be returned.
		 * @param  name The name of the widget to query.
		 * @return The index of the currently selected item.
		 */
		int _getSelectedItem(const std::string& name) noexcept;

		/**
		 * Retrieves the text of the currently selected item of a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and a blank string will be
		 * returned.
		 * @param  name The name of the widget to query.
		 * @return The translated text of the item.
		 */
		std::string _getSelectedItemText(const std::string& name) noexcept;

		/**
		 * Returns the number of widgets within a container.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and 0 will be returned.
		 * @param  name The name of the container to query.
		 * @return The number of widgets within the container.
		 */
		std::size_t _getWidgetCount(const std::string& name) noexcept;

		/**
		 * Sets the scroll amount for a scrollable panel's horizontal scrollbar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no amount will be changed.
		 * @param name   The name of the widget to edit.
		 * @param amount The amount the scrollbar should scroll by.
		 */
		void _setHorizontalScrollbarAmount(const std::string& name,
			const unsigned int amount) noexcept;

		/**
		 * Sets the padding applied to a group of widgets.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no padding will be changed.
		 * @param name    The name of the widget to edit.
		 * @param padding The padding to set.
		 */
		void _setGroupPadding(const std::string& name, const std::string& padding)
			noexcept;

		/**
		 * Sets the alignment applied to a widget within a grid.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no alignment will be
		 * changed. An error will also be logged if the \c row and \c col
		 * parameters are out of range.
		 * @param name      The name of the grid to edit.
		 * @param row       The 0-based row ID of the widget to edit.
		 * @param col       The 0-based column ID of the widget to edit.
		 * @param alignment The alignment to set to the widget.
		 */
		void _setWidgetAlignmentInGrid(const std::string& name,
			const std::size_t row, const std::size_t col,
			const tgui::Grid::Alignment alignment) noexcept;

		/**
		 * Sets the space between widgets in Vertical and Horizontal Layouts.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no space will be applied.
		 * @param name  The name of the layout to edit.
		 * @param space The new distance to apply.
		 */
		void _setSpaceBetweenWidgets(const std::string& name, const float space)
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
		 * Stores the name of the GUI that was set before the current one.
		 */
		std::string _previousGUI;

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
		std::unordered_map<std::string, tgui::Texture> _widgetPictures;

		/**
		 * Stores a list of animated sprites associated with each animated widget.
		 * Cleared upon a call to \c setGUI(). Refilled once upon the first call to
		 * \c animate() since \c setGUI() was last called.
		 */
		std::unordered_map<std::string, sfx::animated_sprite> _widgetSprites;

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
		 * The picture widgets whose size should NOT always match with their sprite
		 * size.
		 */
		std::unordered_set<std::string> _dontOverridePictureSizeWithSpriteSize;

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

		/**
		 * Pointer to the fonts used with this GUI.
		 * Potential optimisation: if the same font is given for a widget more than
		 * once, then pull it from an internal list rather than constructing it
		 * again.
		 */
		std::shared_ptr<sfx::fonts> _fonts = nullptr;

		/**
		 * Ensures that handleInput() errors are only written to the log once at a
		 * time.
		 */
		bool _handleInputErrorLogged = false;
	};
}

#include "tpp/gui.tpp"