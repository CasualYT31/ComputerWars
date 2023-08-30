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

/**@file gui.hpp
 * Defines code related to managing GUIs.
 * The backend used with this class is TGUI.
 */

#pragma once

#include "texture.hpp"
#include "script.hpp"
#include "language.hpp"
#include "userinput.hpp"
#include "fonts.hpp"
#include "tgui/tgui.hpp"
#include "tgui/Backend/SFML-Graphics.hpp"

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
		 * @param data    The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		gui(const std::shared_ptr<engine::scripts>& scripts,
			const engine::logger::data& data);

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * GUI functions with a \c scripts object.
		 * @sa \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Sets the GUI menu to display.
		 * No widgets are actually destroyed: they are only made visible/invisible.
		 * @param  newPanel  The name of the menu to display. If a menu with the
		 *                   given name does not exist, an error will be logged and
		 *                   no other changes will occur.
		 * @param  callClose If @c TRUE, the current menu's @c Close() script
		 *                   function will be called, if it exists.
		 * @param  callOpen  If @c TRUE, the new menu's @c Open() script function
		 *                   will be called, if it exists.
		 * @safety No guarantee.
		 */
		void setGUI(const std::string& newPanel, const bool callClose = true,
			const bool callOpen = true);

		/**
		 * Gets the name of the menu currently showing.
		 * @return The menu currently being drawn.
		 */
		inline std::string getGUI() const {
			return _currentGUI;
		}

		/**
		 * Adds a spritesheet which can be uses with the GUI menus.
		 * If a spritesheet with the given name already exists, a warning will be
		 * logged and the spritesheet will be updated.
		 * @param name  The name which scripts use to reference the spritesheet.
		 * @param sheet A pointer to the \c animated_spritesheet to use with these
		 *              GUI menus.
		 */
		void addSpritesheet(const std::string& name,
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the target to draw the GUI menus to.
		 * @remark This method was introduced because of the limitations of the
		 *         TGUI. Ideally, this method should be removed altogether if at
		 *         all possible in the future. Please see \c engine::gui::draw()
		 *         for more details.
		 * @param  newTarget Reference to the new target to set.
		 * @safety No guarantee.
		 */
		void setTarget(sf::RenderTarget& newTarget);

		/**
		 * Allows the TGUI to handle SFML window events.
		 * Should be called within the event handling portion of the drawing loop.
		 * @param  e The event to handle. All polled events should be given
		 *           eventually.
		 * @return \c FALSE if all widgets ignored the given event, \c TRUE
		 *         otherwise.
		 * @safety No guarantee.
		 */
		bool handleEvent(sf::Event e);

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
		 * You can just check for it directly and perform code whenever it
		 * happens.\n
		 * This method also handles directional controls selecting different
		 * widgets based on the current menu's configuration. If "select" is
		 * triggered, a \c MouseReleased signal will be issued to the currently
		 * selected widget in the current menu, if there is any, and if that widget
		 * is of type \c Button, \c BitmapButton, or \c ListBox.
		 * @param  ui Pointer to the @c user_input instance to send information on.
		 *            Note that the @c user_input::update() method should already
		 *            have been called before a call to @c handleInput().
		 * @safety No guarantee.
		 */
		void handleInput(const std::shared_ptr<sfx::user_input>& ui);

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
		 * @param  widget     The widget sending the signal.
		 * @param  signalName The name of the signal being sent.
		 * @return \c TRUE if a signal handler was called, \c FALSE if not.
		 * @safety No guarantee.
		 */
		bool signalHandler(tgui::Widget::Ptr widget,
			const tgui::String& signalName);

		/**
		 * Represents a menu item ID.
		 */
		typedef std::size_t MenuItemID;

		/**
		 * Handles \c MenuItemClicked signals.
		 * The script function invoked follows the same format as
		 * \c signalHandler() (e.g. \c GUIName_MenuBarName_MenuItemClicked).
		 * However, a <tt>const MenuItemID</tt> parameter is also passed, which
		 * stores the 0-based ID of the menu item that was clicked.\n
		 * Menu item IDs start from the first menu, and traverse through its items,
		 * favouring depth over breadth. This means that in the following menu
		 * hierarchy:
		 * \code
		 * File        Edit
		 * \ New       \ Cut
		 *   \ File2   \ Copy
		 * 	 \ Folder
		 * \ Open
		 * \ Save
		 * 	 \ As
		 * 	   \ File3
		 * 	 \ All
		 * \endcode
		 * The menu items would go in this order: File, New, File2, Folder, Open,
		 * Save, As, File3, All, Edit, Cut, Copy. So, Copy would have an ID of 11
		 * and File3 would have an ID of 7. Each \c MenuBar has a separate list of
		 * menu item IDs.
		 * @param menuBarName The short name of the \c MenuBar whose item was
		 *                    selected.
		 * @param index       The 0-based index of the item selected.
		 */
		void menuItemClickedSignalHandler(const std::string& menuBarName,
			const MenuItemID index);

		/**
		 * Handles \c ButtonPressed signals from \c MessageBox widgets.
		 * The script function invoked follows the same format as
		 * \c signalHandler() (e.g. \c GUIName_MessageBoxName_ButtonPressed).
		 * However, a <tt>const uint64</tt> parameter is also passed, which stores
		 * the 0-based ID of the button that was clicked. The first button added
		 * will have an ID of 0, the second button will have an ID of 1, and so on.
		 * If button's index couldn't be found, the number of buttons will be
		 * given, but this should never happen.\n
		 * If a \c MessageBox button was pressed, but the script defined no signal
		 * handler for it, then a critical error will be logged.
		 * @param messageBoxName The short name of the \c MessageBox whose button
		 *                       was pressed.
		 * @param widget         Pointer to the \c MessageBox widget.
		 * @param caption        The caption of the selected button.
		 */
		void messageBoxButtonPressedSignalHandler(
			const std::string& messageBoxName, const tgui::MessageBox::Ptr& widget,
			const tgui::String& caption);

		/**
		 * Handles \c Closing signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before invoking the
		 * script's signal handler. If the \c ChildWindow was configured not to
		 * handle this logic, then the appropriate script signal handler will still
		 * be invoked.\n
		 * The script function invoked follows the same format as
		 * \c signalHandler() (e.g. \c GUIName_ChildWindowName_Closing).
		 * However, a <tt>bool&out</tt> parameter is also passed, which should be
		 * set to \c FALSE if the window should not be closed.
		 * @warning This engine should never emit a Closed signal for normal
		 *          \c ChildWindow widgets! Use the \c Closing signal instead!
		 * @param   window Pointer to the \c ChildWindow that was closed.
		 * @param   abort  Will always be set to \c TRUE.
		 */
		void closingSignalHandler(const tgui::ChildWindow::Ptr& window,
			bool* abort);

		/**
		 * Handles \c Closing signals for \c FileDialogs.
		 * The script function invoked follows the same format as
		 * \c signalHandler() (e.g. \c GUIName_FileDialogName_Closing).
		 * However, a <tt>bool&out</tt> parameter is also passed, which should be
		 * set to \c TRUE if the dialog should not be closed.
		 * @warning This engine should never emit a Closed signal for
		 *          \c FileDialog widgets! Use the \c Closing signal instead!
		 * @param   window Pointer to the \c FileDialog that was closed.
		 * @param   abort  Will be updated by the script handler, if there is one.
		 */
		void fileDialogClosingSignalHandler(const tgui::FileDialog::Ptr& window,
			bool* abort);

		/**
		 * Handles \c Minimized signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before either invoking
		 * the script's signal handler, or the widget's custom signal handler. If
		 * the \c ChildWindow was configured not to handle this logic, then the
		 * appropriate script signal handler will still be invoked.
		 * @param window Pointer to the \c ChildWindow that was minimised.
		 */
		void minimizedSignalHandler(const tgui::ChildWindow::Ptr& window);

		/**
		 * Handles \c Maximized signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before either invoking
		 * the script's signal handler, or the widget's custom signal handler. If
		 * the \c ChildWindow was configured not to handle this logic, then the
		 * appropriate script signal handler will still be invoked.
		 * @param window Pointer to the \c ChildWindow that was mazimised.
		 */
		void maximizedSignalHandler(const tgui::ChildWindow::Ptr& window);

		/**
		 * Sets the \c language_dictionary object to use with these GUI menus.
		 * If a language dictionary is given, all GUI captions will be fed into it
		 * for translation purposes, during the call to \c animate(). If one is not
		 * given, which is the default, GUI text will not be translated or amended.
		 * @param  lang Pointer to the \c language_dictionary to use. \c nullptr
		 *              can be given to disable translation.
		 * @safety Strong guarantee.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& lang);

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
		 * Sets a scaling factor that is applied to all menus when drawn.
		 * @param  factor The scaling factor.
		 * @safety No guarantee.
		 */
		void setScalingFactor(const float factor);

		/**
		 * Gets the scaling factor that is applied to all menus when drawn.
		 * @return factor The scaling factor.
		 */
		float getScalingFactor() const noexcept;

		/**
		 * Gets the name of the widget under the current mouse position.
		 * If a user input object hasn't been given, then an empty string will be
		 * returned and an error will be logged.
		 * @return The full name of the widget under the current mouse position,
		 *         blank if there isn't a visible one.
		 */
		std::string getWidgetUnderMouse() const;

		/**
		 * Animates the current GUI menu.
		 * Any sprites are animated, and the colour background (if there is one) is
		 * resized to match the size of the target. In addition, if a langauge
		 * dictionary has been given, all captions will be translated.
		 * @return Always returns \c FALSE.
		 * @safety No guarantee.
		 * @sa     sfx::gui::setLanguageDictionary()
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		///////////
		// TYPES //
		///////////

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
			gui_background() = default;

			/**
			 * Initialises the GUI background with a sprite.
			 * @param sheet The sheet containing the sprite.
			 * @param key   The name of the sprite to set.
			 */
			gui_background(
				const std::shared_ptr<const sfx::animated_spritesheet>& sheet,
				const std::string& key);

			/**
			 * Initialises the GUI background with a solid colour.
			 * @param colour The colour to set.
			 */
			gui_background(const sf::Color& colour);

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
				const std::string& key);

			/**
			 * Sets this GUI background to be a solid colour.
			 * The stored sprite ID is then ignored.
			 * @param colour The colour to set.
			 */
			void set(const sf::Color& colour);

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
			std::string getSprite() const;

			/**
			 * Retrieves the assigned colour value.
			 * This method will simply return the colour that was last assigned.
			 * Therefore, you should use \c getType() to first test if this GUI
			 * background is actually meant to represent a solid colour background.
			 * @return The colour that was last assigned.
			 */
			sf::Color getColour() const;

			/**
			 * Animates this background.
			 * Any sprites are animated, and the colour background (if there is
			 * one) is resized to match the size of the target.
			 * @return Always returns \c FALSE if a colour background. Returns the
			 *         result of the call to the sprite's \c animate() method if a
			 *         sprite background.
			 */
			bool animate(const sf::RenderTarget& target) final;
		private:
			/**
			 * Draws the GUI background.
			 */
			void draw(sf::RenderTarget& target,
				sf::RenderStates states) const final;

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
		 * Stores information on a widget caption.
		 */
		struct original_caption {
			/**
			 * Default constructor.
			 */
			original_caption() = default;

			/**
			 * Construct a new caption.
			 * @param text The original caption of the widget before translation.
			 * @param vars The variables to insert into the caption when
			 *             translating. If \c nullptr, \c variables will be left
			 *             empty.
			 */
			original_caption(const std::string& text, CScriptArray* vars);

			/**
			 * The original caption before translation.
			 */
			std::string caption;

			/**
			 * The variables to insert into the caption when translating.
			 */
			std::vector<engine::CScriptWrapper<CScriptAny>> variables;
		};

		/**
		 * Used to track whether a \c ChildWindow is minimised or maximised, as
		 * well as track its properties so that the engine can restore them.
		 */
		struct child_window_properties {
			/**
			 * The \c ChildWindow's size layout.
			 */
			tgui::Layout2d size;

			/**
			 * The \c ChildWindow's position layout.
			 */
			tgui::Layout2d position;

			/**
			 * The \c ChildWindow's origin.
			 */
			tgui::Vector2f origin;

			/**
			 * Was the \c ChildWindow resizeable before minimising or maximising?
			 */
			bool isResizeable;

			/**
			 * Was the \c ChildWindow position locked before minimising or
			 * maximising?
			 */
			bool isPositionLocked;

			/**
			 * Updates the \c ChildWindow properties, given a \c ChildWidget
			 * pointer.
			 * \c _size, \c _position, \c _origin, \c isResizable and
			 * \c isPositionLocked are all updated with the properties of the given
			 * \c ChildWindow.
			 * @param window The window to pull the properties from.
			 */
			void cache(const tgui::ChildWindow::Ptr& window);

			/**
			 * Restores a \c ChildWindow based on the properties currently stored
			 * in this object.
			 * @param window The window to restore.
			 */
			void restore(const tgui::ChildWindow::Ptr& window);

			/**
			 * Is this \c ChildWindow currently minimised?
			 */
			bool isMinimised = false;

			/**
			 * Is this \c ChildWindow currently maximised?
			 */
			bool isMaximised = false;
		};

		/**
		 * A list of \c ChildWindows that are currently minimised in a given
		 * container.
		 */
		class minimised_child_window_list {
		public:
			/**
			 * Adds a \c ChildWindow to the list.
			 * @param  name The full name of the \c ChildWindow.
			 * @return The X \c Layout2d coordinate that the \c ChildWindow should
			 *         be moved to when minimised.
			 */
			tgui::String minimise(const std::string& name);

			/**
			 * Removes a \c ChildWindow from the list.
			 * @param name The full name of the \c ChildWindow.
			 */
			void restore(const std::string& name);
		private:
			/**
			 * The list of \c ChildWindows.
			 */
			std::vector<std::string> _windows;
		};

		/**
		 * Used for widgets with a single caption.
		 */
		typedef original_caption SingleCaption;

		/**
		 * Used for widgets with a list of captions indexed using a number.
		 */
		typedef std::vector<original_caption> ListOfCaptions;

		/**
		 * Performs animation calculations on a container of widgets.
		 * @param  target    The target which the GUI will be drawn to later.
		 * @param  container Pointer to the container widget.
		 * @safety No guarantee.
		 */
		void _animate(const sf::RenderTarget& target,
			tgui::Container::Ptr container);

		/**
		 * Draws the current GUI menu.
		 * @warning It is to be noted that this implementation of \c draw()
		 *          \b ignores any given render states. This was done to remain
		 *          consistent with TGUI (which does not seem to allow
		 *          \c sf::RenderStates). The internal \c tgui::Gui object also
		 *          ignores the given \c target when drawing: it must be previously
		 *          set with \c setTarget().
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * Callback given to \c BitmapButton and \c Picture widgets to allow the
		 * GUI engine to draw animated sprites in the correct order.
		 * @param target The target to draw to.
		 * @param states The render states to apply.
		 * @param widget The widget being drawn.
		 */
		void _drawCallback(tgui::BackendRenderTarget& target,
			const tgui::RenderStates& states, tgui::Widget::ConstPtr widget) const;

		/**
		 * Handles moving the currently selected widget based on directional input.
		 * @param  ui Pointer to the user interface information to read from.
		 * @return The full name of the widget that is selected after the call is
		 *         finished.
		 */
		std::string _moveDirectionalFlow(
			const std::shared_ptr<sfx::user_input>& ui);

		/**
		 * Update the current directional flow selection.
		 * @param newsel The name of the widget to select.
		 * @param menu   The name of the menu that has the given widget.
		 */
		void _makeNewDirectionalSelection(const std::string& newsel,
			const std::string& menu);

		/**
		 * Reapplies all the translations to a widget recursively.
		 * @param  widget Pointer to the widget to translate.
		 * @safety No guarantee.
		 */
		void _translateWidget(tgui::Widget::Ptr widget);

		/**
		 * Translates a set of menu items recursively.
		 */
		void _translateMenuItems(const tgui::MenuBar::Ptr& w,
			const std::string& widgetName,
			const std::vector<tgui::MenuBar::GetMenusElement>& items,
			std::vector<tgui::String>& hierarchy, std::size_t& index);

		/**
		 * The JSON load method for this class.
		 * The value of the key "menus" should be an array of string names, each
		 * one naming a menu.\n
		 * For each menu defined, the script function <tt><em>MenuName</em>
		 * SetUp()</tt> is called, which accepts no parameters and does not return
		 * any value. This is intended to be used to setup that menu, such as
		 * adding all of its controls, and setting the background. If an
		 * <tt>engine::script</tt> object was not given, then this function will
		 * not be called, and the menu will remain empty.\n
		 * Additionally, five key-string pairs must be included, with the keys
		 * "up", "down", "left", "right", and "select". The string values should
		 * store the names of the game controls used to send those signals to the
		 * GUI engine to move the setfocus around and to select the GUI currently
		 * in setfocus.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if all existing menus before the call were deleted, \c
		 *         FALSE if not.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

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
		 * @param  widget The widget whose signals are to be connected.
		 * @param  customSignalHandler The name of the script function to call when
		 *                             handling the signal. If a blank string is
		 *                             given, the default handlers will be assumed.
		 * @safety No guarantee.
		 */
		void _connectSignals(tgui::Widget::Ptr widget,
			const std::string& customSignalHandler);

		/**
		 * Removes widgets from \c _gui.
		 * @warning A grid within a grid may cause the game to crash!
		 * @param   widget    Pointer to the widget to delete.
		 * @param   container Pointer to the container which holds the widget.
		 * @param   removeIt  For containers: if \c FALSE, this will only remove
		 *                    all the widgets within a container, but not the
		 *                    container itself. If \c TRUE, this will ensure that
		 *                    the container itself is also deleted, as well as all
		 *                    of its child widgets.\n
		 *                    For widgets: if \c FALSE, the widget's associated
		 *                    data will be erased, but the widget itself won't be
		 *                    erased. If \c TRUE, the widget and its data will be
		 *                    erased. This is required for \c Grid widgets, who
		 *                    cause the game to crash if it attempts to remove a
		 *                    grid whose widgets have already been erased.
		 * @safety  No guarantee.
		 */
		void _removeWidgets(const tgui::Widget::Ptr& widget,
			const tgui::Container::Ptr& container, const bool removeIt);

		/**
		 * Adds a translatable caption to a widget, or updates an existing one.
		 * Used for widgets that have only a single caption.
		 * @param  fullname  The full name of the widget to assign the caption to.
		 * @param  text      The translation key of the string to use.
		 * @param  variables Optional list of variables to later insert into the
		 *                   caption when translating.
		 * @safety No guarantee.
		 */
		void _setTranslatedString(const std::string& fullname,
			const std::string& text, CScriptArray* variables);

		/**
		 * Adds a translatable caption to a widget, or updates an existing one.
		 * Used for widgets that have a list of captions that are most easily
		 * identified via a numeric index, e.g. ListBoxes.\n
		 * Note that if an out of range index is given, the internal lists will
		 * grow in order to accomodate for the new entry.
		 * @param  fullname  The full name of the widget to assign the caption to.
		 * @param  text      The translation key of the string to use.
		 * @param  variables Optional list of variables to later insert into the
		 *                   caption when translating.
		 * @param  index     Which caption to set.
		 * @safety No guarantee.
		 */
		void _setTranslatedString(const std::string& fullname,
			const std::string& text, CScriptArray* variables,
			const std::size_t index);

		/**
		 * Shared \c _getTranslatedText() implementation.
		 * @warning Assumes that a language dictionary has been set!
		 * @param   caption         Caption object to translate.
		 * @param   warningCallback If a variable with an unsupported type is
		 *                          identified, a blank string will be inserted
		 *                          instead, and a warning should be logged in the
		 *                          given function. The string parameter is the
		 *                          variable's type name.
		 * @safety  No guarantee.
		 */
		std::string _getTranslatedText(const sfx::gui::original_caption& caption,
			const std::function<void(engine::logger *const, const std::string&)>&
			warningCallback) const;

		/**
		 * Computes the translated caption for a given widget.
		 * Used for widgets with a single caption.
		 * @param  fullname The name of the widget whose caption needs to be
		 *                  translated.
		 * @return The translated string with any configured variables inserted.
		 * @safety No guarantee.
		 */
		std::string _getTranslatedText(const std::string& fullname) const;

		/**
		 * Computes the translated widget text for a given widget.
		 * Used for widgets with a list of captions, such as a ListBox.
		 * @param  fullname The name of the widget whose caption needs to be
		 *                  translated.
		 * @param  index    The specific caption to translate.
		 * @return The translated string with any configured variables inserted.
		 * @safety No guarantee.
		 */
		std::string _getTranslatedText(const std::string& fullname,
			const std::size_t index) const;

		/**
		 * Applies a new sprite to a widget.
		 * @param widget Pointer to the widget to apply the sprite to.
		 * @param sheet  Name of the sheet containing the sprite to apply.
		 * @param key    Key of the sprite to apply.
		 */
		inline void _applySprite(const tgui::Widget::Ptr& widget,
			const std::string& sheet, const std::string& key) {
			const auto fullname = widget->getWidgetName().toStdString();
			// Prevent deleting sprite objects if there won't be any change.
			if (_guiSpriteKeys[fullname].first != sheet ||
				_guiSpriteKeys[fullname].second != key) {
				_guiSpriteKeys[fullname] = std::make_pair(sheet, key);
				_widgetSprites.erase(widget);
			}
		}

		/**
		 * Extracts a widget's short name from its full name.
		 * @param  fullname The full name of the widget.
		 * @return The short name of the widget.
		 */
		static std::string _extractWidgetName(const std::string& fullname);

		/**
		 * Does the given widget inherit from \c SubwidgetContainer?
		 * @warning Do not pass in \c nullptr!
		 * @param   widget Pointer to the widget to test.
		 * @return  If \c widget derives from \c SubwidgetContainer, its internal
		 *          \c Container will be extracted and returned as a \c shared_ptr.
		 *          Otherwise, \c nullptr is returned.
		 */
		static tgui::Container::Ptr _getSubwidgetContainer(
			const tgui::Widget::Ptr& widget);

		/**
		 * Does the given widget inherit from \c SubwidgetContainer?
		 * @warning Do not pass in \c nullptr!
		 * @param   widget Pointer to the widget to test.
		 * @return  If \c widget derives from \c SubwidgetContainer, its internal
		 *          \c Container will be extracted and returned as a raw pointer.
		 *          Otherwise, \c nullptr is returned.
		 */
		static tgui::Container* _getSubwidgetContainer(tgui::Widget* const widget);

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
			const std::string& name, const std::string& menu) const;

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
			std::string* fullname = nullptr) const;

		/**
		 * Retrieves the parent of a given widget.
		 * If the given widget has no parent, it could refer to a
		 * \c SubwidgetContainer, which has a \c Container that is detached from
		 * the rest of the GUI. We can use \c _findWidget() to attempt to find the
		 * parent, and if that's the case, its internal parent will be returned.
		 * @param  child The widget to find the parent of.
		 * @return Pointer to the child widget's parent if it exists, \c nullptr if
		 *         the widget has no parent.
		 */
		tgui::Container* _findParent(const tgui::Widget* const child) const;

		/**
		 * Used to calculate the absolute position of a widget.
		 * Usually, you would be able to call \c Widget::getAbsolutePosition()
		 * directly and receive the correct value. But for widgets within
		 * \c SubwidgetContainers, the result will be relative to its internal
		 * container, not the entire window. So after getting the absolute
		 * position, we will need to traverse through the widget's hierarchy and if
		 * a \c SubwidgetContainer is found, its absolute position is applied to
		 * the original widget's absolute position.
		 * @remark Ideally this would be fixed in the backend library. I might find
		 *         a way to fix it there in the future.
		 * @param  widget Pointer to the widget to get the absolute position of.
		 * @param  offset The offset to apply to the widget's
		 *                \c getAbsolutePosition() call.
		 * @return The absolute position of the widget.
		 */
		tgui::Vector2f _findWidgetAbsolutePosition(tgui::Widget* const widget,
			const tgui::Vector2f& offset = {}) const;

		/**
		 * Checks if a given widget is visible and/or enabled, and that the same
		 * can be said for all of its parents.
		 * @warning An assertion is made that at least one of the boolean
		 *          parameters is \c TRUE!
		 * @param   widget  Pointer to the widget to test.
		 * @param   visible If \c TRUE, will test each widget's visibility.
		 * @param   enabled If \c TRUE, will test each widget's enabled state.
		 * @return  \c TRUE if the given widget and all of its parents pass the
		 *          specified tests, \c FALSE if at least one of the widgets in the
		 *          hierarchy does not pass a single test.
		 */
		bool _isWidgetFullyVisibleAndEnabled(tgui::Widget* widget,
			const bool visible, const bool enabled) const;

		/**
		 * Finds the currently selected widget.
		 * In the event that there is a selected widget, but it cannot be found, an
		 * error will be logged, it will be deselected, and the selection history
		 * for the current GUI menu will also be erased.
		 * @return If there isn't a widget currently selected (or it can't be
		 *         found), a blank string and \c nullptr. If there is a widget
		 *         currently selected, and it can be found, then its full name and
		 *         widget object shall be returned.
		 */
		std::pair<std::string, tgui::Widget::Ptr> _findCurrentlySelectedWidget();

		/**
		 * Makes a widget in a \c ScrollablePanel visible by scrolling the
		 * scrollbars to make the widget fully visible.
		 * If the given widget does not have a \c ScrollablePanel ancestor, then no
		 * changes will be made.
		 * @param widget             Pointer to the widget to show.
		 * @param panelAncestryDepth Recursion parameter. Leave to default.
		 */
		void _showWidgetInScrollablePanel(const tgui::Widget::Ptr& widget,
			const unsigned int panelAncestryDepth = 0);

		/**
		 * Clears GUI state.
		 */
		void _clearState();

		/**
		 * Erases state pertaining to a single widget.
		 * @param widget   The widget to erase the state of.
		 * @param removeIt See \c _removeWidgets(). If \c TRUE, removes the widget
		 *                 from its parent.
		 * @sa    \c _removeWidgets().
		 */
		void _eraseWidgetState(const tgui::Widget::Ptr& widget,
			const bool removeIt);

		//////////////////////
		// SCRIPT INTERFACE //
		//////////////////////

		/**
		 * Documents the GUI library in general.
		 */
		void _documentGUILibrary(
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Registers and documents the types, enums, funcdefs and typedefs that can
		 * be used by the scripts.
		 */
		void _registerTypes(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Registers and documents global constants.
		 */
		void _registerConstants(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		// NON-WIDGET GLOBAL FUNCTIONS //

		/**
		 * Register non-widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerNonWidgetGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setGUI(const std::string& name);
		bool _menuExists(const std::string& menu);
		void _noBackground(std::string menu);
		void _spriteBackground(std::string menu, const std::string& sheet,
			const std::string& sprite);
		void _colourBackground(std::string menu, const sf::Color& colour);
		void _setGlobalFont(const std::string& fontName);

		// WIDGETS //

		/**
		 * Registers generic widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerWidgetGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		bool _widgetExists(const std::string& name);
		void _addWidget(const std::string& newWidgetType, const std::string& name,
			const std::string& signalHandler = "");
		void _connectSignalHandler(const std::string& name,
			asIScriptFunction* const handler);
		void _disconnectSignalHandlers(const CScriptArray* const names);
		std::string _getParent(const std::string& name);
		void _removeWidget(const std::string& name);
		void _setWidgetFocus(const std::string& name);
		void _setWidgetFont(const std::string& name, const std::string& fontName);
		void _setWidgetPosition(const std::string& name, const std::string& x,
			const std::string& y);
		sf::Vector2f _getWidgetAbsolutePosition(const std::string& name);
		void _setWidgetOrigin(const std::string& name, const float x,
			const float y);
		void _setWidgetSize(const std::string& name, const std::string& w,
			const std::string& h);
		sf::Vector2f _getWidgetFullSize(const std::string& name);
		void _setWidgetEnabled(const std::string& name, const bool enable);
		bool _getWidgetEnabled(const std::string& name) const;
		void _setWidgetVisibility(const std::string& name, const bool visible);
		bool _getWidgetVisibility(const std::string& name) const;
		void _moveWidgetToFront(const std::string& name);
		void _moveWidgetToBack(const std::string& name);
		void _setWidgetText(const std::string& name, const std::string& text,
			CScriptArray* const variables);
		void _setWidgetIndex(const std::string& name, const std::size_t index);

		// DIRECTIONAL FLOW //

		/**
		 * Registers directional flow global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerDirectionalFlowGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetDirectionalFlow(const std::string& name,
			const std::string& upName, const std::string& downName,
			const std::string& leftName, const std::string& rightName);
		void _setWidgetDirectionalFlowStart(const std::string& name);
		void _clearWidgetDirectionalFlowStart(const std::string& menu);
		void _setWidgetDirectionalFlowSelection(const std::string& name);
		void _setDirectionalFlowAngleBracketSprite(const std::string& corner,
			const std::string& sheet, const std::string& key);

		// SPRITES //

		/**
		 * Registers sprite-related global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerSpriteGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetSprite(const std::string& name, const std::string& sheet,
			const std::string& key);
		void _clearWidgetSprite(const std::string& name);
		void _matchWidgetSizeToSprite(const std::string& name,
			const bool overrideSetSize);

		// LABEL //

		/**
		 * Registers global functions that act on labels within widgets.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerLabelGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetTextSize(const std::string& name, const unsigned int size);
		void _setWidgetTextStyles(const std::string& name,
			const std::string& styles);
		void _setWidgetTextMaximumWidth(const std::string& name, const float w);
		void _setWidgetTextColour(const std::string& name,
			const sf::Color& colour);
		void _setWidgetTextOutlineColour(const std::string& name,
			const sf::Color& colour);
		void _setWidgetTextOutlineThickness(const std::string& name,
			const float thickness);
		void _setWidgetTextAlignment(const std::string& name,
			const tgui::Label::HorizontalAlignment h,
			const tgui::Label::VerticalAlignment v);

		// EDITBOX AND TEXTAREA //

		/**
		 * Registers \c EditBox and \c TextArea global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerEditBoxGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		std::string _getWidgetText(const std::string& name);
		void _onlyAcceptUIntsInEditBox(const std::string& name);
		void _setWidgetDefaultText(const std::string& name,
			const std::string& text, CScriptArray* variables);

		// RADIOBUTTON & CHECKBOX //

		/**
		 * Registers \c RadioButton and \c CheckBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerRadioButtonAndCheckBoxGlobalFunctions(
			asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetChecked(const std::string& name, const bool checked);
		bool _isWidgetChecked(const std::string& name);

		// LIST //

		/**
		 * Registers \c ListBox and \c ComboBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerListGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addItem(const std::string& name, const std::string& text,
			CScriptArray* const variables);
		void _clearItems(const std::string& name);
		void _setSelectedItem(const std::string& name, const std::size_t index);
		void _deselectItem(const std::string& name);
		int _getSelectedItem(const std::string& name);
		std::string _getSelectedItemText(const std::string& name);
		void _setItemsToDisplay(const std::string& name, const std::size_t items);

		// TREEVIEW //

		/**
		 * Registers \c TreeView global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTreeViewGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		CScriptArray* _getSelectedItemTextHierarchy(const std::string& name);
		void _addTreeViewItem(const std::string& name,
			const CScriptArray* const hierarchy);

		// TABS //

		/**
		 * Registers \c Tabs global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTabsGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addTab(const std::string& name, const std::string& text,
			CScriptArray* const variables);
		void _setSelectedTab(const std::string& name, const std::size_t index);
		int _getSelectedTab(const std::string& name);
		std::size_t _getTabCount(const std::string& name);

		// CONTAINER //

		/**
		 * Registers \c Container widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerContainerGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _removeWidgetsFromContainer(const std::string& name);
		void _setWidgetIndexInContainer(const std::string& name,
			const std::size_t oldIndex, const std::size_t newIndex);
		std::size_t _getWidgetCount(const std::string& name);
		void _setGroupPadding(const std::string& name, const std::string& padding);
		void _setGroupPadding(const std::string& name, const std::string& left,
			const std::string& top, const std::string& right,
			const std::string& bottom);
		void _applySpritesToWidgetsInContainer(const std::string& name,
			const std::string& spritesheet, const CScriptArray* const sprites);

		// PANEL //

		/**
		 * Registers \c Panel and \c ScrollablePanel widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerPanelGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetBgColour(const std::string& name, const sf::Color& colour);
		void _setWidgetBorderSize(const std::string& name, const float size);
		void _setWidgetBorderColour(const std::string& name,
			const sf::Color& colour);
		void _setWidgetBorderRadius(const std::string& name, const float radius);
		void _setHorizontalScrollbarPolicy(const std::string& name,
			const tgui::Scrollbar::Policy policy);
		void _setHorizontalScrollbarAmount(const std::string& name,
			const unsigned int amount);
		void _setVerticalScrollbarAmount(const std::string& name,
			const unsigned int amount);
		void _setVerticalScrollbarValue(const std::string& name,
			const unsigned int value);
		float _getScrollbarWidth(const std::string& name);

		// LAYOUT //

		/**
		 * Registers \c BoxLayout widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerLayoutGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetRatioInLayout(const std::string& name,
			const std::size_t index, const float ratio);
		void _setSpaceBetweenWidgets(const std::string& name, const float space);

		// GRID //

		/**
		 * Registers \c Grid widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerGridGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addWidgetToGrid(const std::string& newWidgetType,
			const std::string& name, const std::size_t row, const std::size_t col,
			const std::string& signalHandler = "");
		void _setWidgetAlignmentInGrid(const std::string& name,
			const std::size_t row, const std::size_t col,
			const tgui::Grid::Alignment alignment);
		void _setWidgetPaddingInGrid(const std::string& name,
			const std::size_t row, const std::size_t col,
			const std::string& padding);

		// MENUS //

		/**
		 * Registers \c MenuBar widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerMenuBarGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		MenuItemID _addMenu(const std::string& name, const std::string& text,
			CScriptArray* const variables);
		MenuItemID _addMenuItem(const std::string& name, const std::string& text,
			CScriptArray* const variables);
		MenuItemID _addMenuItemIntoLastItem(const std::string& name,
			const std::string& text, CScriptArray* const variables);
		void _exitSubmenu(const std::string& name);

		// CHILDWINDOW //

		/**
		 * Registers \c ChildWindow global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerChildWindowGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _autoHandleMinMax(const std::string& name, const bool handle);
		void _setChildWindowTitleButtons(const std::string& name,
			const unsigned int buttons);
		void _setWidgetResizable(const std::string& name, const bool resizable);
		float _getTitleBarHeight(const std::string& name);
		CScriptArray* _getBorderWidths(const std::string& name);
		void _openChildWindow(const std::string& name, const std::string& x,
			const std::string& y);
		void _closeChildWindow(const std::string& name);
		void _restoreChildWindow(const std::string& name);
		void _restoreChildWindowImpl(const tgui::ChildWindow::Ptr& window,
			child_window_properties& data);
		bool _isChildWindowOpen(const std::string& name);

		// FILEDIALOG //

		/**
		 * Registers \c FileDialog global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerFileDialogGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setFileDialogStrings(const std::string& name,
			const std::string& title, CScriptArray* const v0,
			const std::string& confirm, CScriptArray* const v1,
			const std::string& cancel, CScriptArray* const v2,
			const std::string& createFolder, CScriptArray* const v3,
			const std::string& filenameLabel, CScriptArray* const v4,
			const std::string& nameColumn, CScriptArray* const v5,
			const std::string& sizeColumn, CScriptArray* const v6,
			const std::string& modifyColumn, CScriptArray* const v7,
			const std::string& allFiles, CScriptArray* const v8);
		CScriptArray* _getFileDialogSelectedPaths(const std::string& name);
		void _addFileDialogFileTypeFilter(const std::string& name,
			const std::string& caption, CScriptArray *const variables,
			CScriptArray *const filters);
		void _clearFileDialogFileTypeFilters(const std::string& name);
		void _setFileDialogFileMustExist(const std::string& name,
			const bool mustExist);
		void _setFileDialogDefaultFileFilter(const std::string& name,
			const std::size_t index);
		void _setFileDialogPath(const std::string& name, const std::string& path);

		// MESSAGEBOX //

		/**
		 * Registers \c MessageBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerMessageBoxGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setMessageBoxStrings(const std::string& name,
			const std::string& title, CScriptArray* const titleVars,
			const std::string& text, CScriptArray* const textVars);
		void _addMessageBoxButton(const std::string& name, const std::string& text,
			CScriptArray* const variables);

		// TABCONTAINER //

		/**
		 * Registers \c TabContainer global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTabContainerGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		std::string _addTabAndPanel(const std::string& name,
			const std::string& text, CScriptArray* const vars = nullptr);

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
		 * Stores the scaling factor used with this GUI.
		 */
		float _scalingFactor = 1.0f;

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
		 * Cache of the user input object last given to \c handleInput().
		 */
		std::shared_ptr<sfx::user_input> _ui = nullptr;

		/**
		 * Pointer to the animated spritesheets that can be used with the GUI
		 * menus.
		 */
		std::unordered_map<std::string, std::shared_ptr<sfx::animated_spritesheet>>
			_sheet;

		/**
		 * Stores a list of animated sprites associated with each animated widget.
		 * Cleared upon a call to \c setGUI(). Refilled once upon the first call to
		 * \c animate() since \c setGUI() was last called.
		 */
		std::unordered_map<tgui::Widget::ConstPtr, sfx::animated_sprite>
			_widgetSprites;

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
		std::unordered_map<std::string,
			std::variant<SingleCaption, ListOfCaptions>> _originalCaptions;

		/**
		 * Stores the custom signal handler names associated with some widgets.
		 */
		std::unordered_map<std::string, std::string> _customSignalHandlers;

		/**
		 * Stores additional signal handlers for each widget.
		 */
		std::unordered_map<std::string, engine::CScriptWrapper<asIScriptFunction>>
			_additionalSignalHandlers;

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

		/**
		 * The game control name used to instruct the GUI engine to move the set
		 * focus "up."
		 */
		std::string _upControl;

		/**
		 * The game control name used to instruct the GUI engine to move the set
		 * focus "down."
		 */
		std::string _downControl;

		/**
		 * The game control name used to instruct the GUI engine to move the set
		 * focus "left."
		 */
		std::string _leftControl;

		/**
		 * The game control name used to instruct the GUI engine to move the set
		 * focus "right."
		 */
		std::string _rightControl;

		/**
		 * The game control name used to instruct the GUI engine to "select" the UI
		 * element with the setfocus.
		 */
		std::string _selectControl;

		/**
		 * Dictates where setfocus should move to when the setfocus is on a given
		 * widget.
		 */
		struct directional_flow {
			/**
			 * The full name of the widget to move the setfocus to when pressing
			 * "up."
			 */
			std::string up;

			/**
			 * The full name of the widget to move the setfocus to when pressing
			 * "down."
			 */
			std::string down;

			/**
			 * The full name of the widget to move the setfocus to when pressing
			 * "left."
			 */
			std::string left;

			/**
			 * The full name of the widget to move the setfocus to when pressing
			 * "right."
			 */
			std::string right;
		};

		/**
		 * Stores the directional flow information for all widgets.
		 * @sa \c sfx::gui::directional_flow.
		 */
		std::unordered_map<std::string, directional_flow> _directionalFlow;

		/**
		 * Stores the names of widgets to select first when a directional input is
		 * made.
		 * Keyed on menu name.
		 */
		std::unordered_map<std::string, std::string> _selectThisWidgetFirst;

		/**
		 * The last known selected widget based on directional input.
		 * Keyed on menu name. The second string in the value stores the currently
		 * selected widget, and the first string stored the previously selected
		 * widget.
		 */
		std::unordered_map<std::string, std::pair<std::string, std::string>>
			_currentlySelectedWidget;

		/**
		 * The current mouse position.
		 */
		sf::Vector2i _currentMousePosition;

		/**
		 * The mouse position the last time \c handleInput() was called.
		 */
		sf::Vector2i _previousMousePosition;

		/**
		 * If the mouse has moved, disable directional control influence until a
		 * new directional control has been input.
		 */
		bool _enableDirectionalFlow = true;

		/**
		 * The UL angle bracket sprite.
		 */
		sfx::animated_sprite _angleBracketUL;

		/**
		 * The UR angle bracket sprite.
		 */
		sfx::animated_sprite _angleBracketUR;

		/**
		 * The LL angle bracket sprite.
		 */
		sfx::animated_sprite _angleBracketLL;

		/**
		 * The LR angle bracket sprite.
		 */
		sfx::animated_sprite _angleBracketLR;

		/**
		 * Is set to \c TRUE whilst \c _load() is running.
		 * Will always be set to \c FALSE when not in \c _load(), even if an
		 * exception is thrown from within \c _load().
		 */
		bool _isLoading = false;

		/**
		 * The hierarchy of the last added menu or menu item, for each \c MenuBar.
		 * @warning Since menu items are translated in \c animate(), unfortunately
		 *          we can't easily let the scripts add menu items outside of
		 *          SetUp() functions.
		 */
		std::unordered_map<std::string, std::vector<tgui::String>>
			_hierarchyOfLastMenuItem;

		/**
		 * The number of menus and menu items in each \c MenuBar.
		 */
		std::unordered_map<std::string, MenuItemID> _menuCounter;

		/**
		 * \c ChildWindow property cache used to handle minimise and maximise
		 * logic.
		 * If a \c ChildWindow is in this map, it means that the engine should
		 * \b not automatically handle minimise and maximise logic.
		 */
		std::unordered_map<std::string, child_window_properties> _childWindowData;

		/**
		 * A list of container widgets with minimised \c ChildWindows in them.
		 */
		std::unordered_map<std::string, minimised_child_window_list>
			_minimisedChildWindowList;
	};
}

#include "tpp/gui.tpp"
