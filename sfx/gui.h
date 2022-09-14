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
#include "userinput.h"

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
		 * @param widget    Pointer to the widget to delete.
		 * @param container Pointer to the container which holds the widget.
		 * @param removeIt  This parameter is ignored if the given widget is not a
		 *                  container. If \c FALSE, this will only remove all the
		 *                  widgets within a container, but not the container
		 *                  itself. If \c TRUE, this will ensure that the container
		 *                  itself is also deleted, as well as all of its child
		 *                  widgets.
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
		 * Sets a widget's rounded border radius.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no radius will be set.
		 * @param name   The name of the widget to change.
		 * @param radius The radius to set to the widget's rounded border.
		 */
		void _setWidgetBorderRadius(const std::string& name, const float radius)
			noexcept;

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
		 * Ensures that handleInput() errors are only written to the log once at a
		 * time.
		 */
		bool _handleInputErrorLogged = false;
	};
}

template<typename T>
typename T::Ptr sfx::gui::_findWidget(std::string name,
	std::vector<std::string>* namesList, std::string* fullname) const noexcept {
	// Split string.
	std::vector<std::string> names;
	std::size_t pos = 0;
	do {
		pos = name.find('.');
		names.push_back(name.substr(0, pos));
		if (pos != std::string::npos) name = name.substr(pos + 1);
	} while (pos != std::string::npos);
	// If group name was not given, insert it.
	tgui::Group::Ptr groupPtr = _gui.get<tgui::Group>(names[0]);
	if (!groupPtr) {
		names.insert(names.begin(), _currentGUI);
	} else if (groupPtr && names.size() == 1) {
		// If the group name is all that was given, then simply return the group.
		if (namesList) {
			namesList->clear();
			*namesList = names;
		}
		if (fullname) *fullname = name;
		return _gui.get<T>(names[0]);
	}
	if (namesList) {
		namesList->clear();
		*namesList = names;
	}
	if (fullname) {
		*fullname = "";
		for (auto& name : names) *fullname += name + ".";
		if (fullname->size() > 0) fullname->pop_back();
	}
	// Find it.
	tgui::Container::Ptr container = _gui.get<tgui::Container>(names[0]);
	if (names.size() > 2) {
		for (std::size_t w = 1; w < names.size() - 1; ++w) {
			if (!container) return nullptr;
			tgui::String containerName;
			for (std::size_t v = 0; v <= w; ++v) {
				containerName += names[v] + ".";
			}
			containerName.pop_back();
			container = container->get<tgui::Container>(containerName);
		}
	}
	if (container) {
		tgui::String widgetName;
		for (std::size_t v = 0; v < names.size(); ++v) {
			widgetName += names[v] + ".";
		}
		widgetName.pop_back();
		return container->get<T>(widgetName);
	} else {
		return nullptr;
	}
}