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
		 * Class used to automatically handle reference counting of AngelScript
		 * objects.
		 * @tparam The AngelScript object type to handle.
		 */
		template<typename T>
		class CScriptWrapper {
		public:
			/**
			 * Initialises the wrapper object with no object.
			 */
			CScriptWrapper() = default;

			/**
			 * Initialises the wrapper object with an existing AngelScript object.
			 */
			CScriptWrapper(T* const obj);

			/**
			 * Copies the pointer and increases its reference count.
			 */
			CScriptWrapper(const CScriptWrapper<T>& obj);

			/**
			 * Moves the pointer over and increases the reference count.
			 * Even though it's a move, we are technically creating another
			 * reference to the object, so increase the reference count anyway. If
			 * you remove this, \c emplace_back() and other "move" calls will cause
			 * the reference count to fall down by one, which will cause a nasty
			 * crash later on at whatever point.
			 */
			CScriptWrapper(CScriptWrapper<T>&& obj) noexcept;

			/**
			 * Releases the reference to the stored AngelScript object.
			 */
			~CScriptWrapper() noexcept;

			/**
			 * Allows direct access to the stored AngelScript object.
			 * @return Pointer to the AngelScript object.
			 */
			T* operator->() const noexcept;
		private:
			/**
			 * The AngelScript object.
			 */
			T* _ptr = nullptr;
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
			std::vector<sfx::gui::CScriptWrapper<CScriptAny>> variables;
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
		 * @param widget The widget being drawn.
		 */
		void _drawCallback(tgui::BackendRenderTarget& target,
			tgui::Widget::ConstPtr widget) const;

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

		//////////////////////
		// SCRIPT INTERFACE //
		//////////////////////

		// SET GUI //

		/**
		 * Calls @c setGUI(), passing @c TRUE to both @c callClose and @c callOpen.
		 * @sa @c sfx::gui::setGUI()
		 */
		void _setGUI(const std::string& name);

		/**
		 * Determines if a menu exists.
		 * @param  menu The name of the menu to search for.
		 * @return \c TRUE if a menu with the given name exists, \c FALSE
		 *         otherwise.
		 */
		bool _menuExists(const std::string& menu);

		/**
		 * Gets the name of the widget under the current mouse position.
		 * If a user input object hasn't been given, then an empty string will be
		 * returned and an error will be logged.
		 * @return The full name of the widget under the current mouse position,
		 *         blank if there isn't a visible one.
		 */
		std::string _getWidgetUnderMouse();

		// BACKGROUND //

		/**
		 * Removes a menu's background.
		 * @param menu The name of the menu whose background is to be removed. If a
		 *             blank string is given, the current menu is chosen.
		 */
		void _noBackground(std::string menu);

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
			const std::string& sprite);

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
			const unsigned int g, const unsigned int b, const unsigned int a);

		// WIDGETS //

		/**
		 * Determines if a widget exists.
		 * @param  name The name of the widget to search for.
		 * @return \c TRUE if a widget with the given name exists, \c FALSE
		 *         otherwise.
		 */
		bool _widgetExists(const std::string& name);

		/**
		 * Creates a new widget and adds it to a container.
		 * If no menu is specified, the currently displayed menu is chosen. This is
		 * true for all GUI global script functions.\n
		 * If the @c type parameter was invalid, or if a widget with the specified
		 * name already existed at the time of the call, then an error will be
		 * logged and no widget will be created.
		 * @param newWidgetType The type of widget to create.
		 * @param name          The name of the new widget.
		 * @param signalHandler The name of the custom signal handler to assign to
		 *                      this widget, if any.
		 */
		void _addWidget(const std::string& newWidgetType, const std::string& name,
			const std::string& signalHandler = "");

		/**
		 * Creates a widget and adds it to a grid.
		 * An error will be logged in the following circumstances:
		 * <ul><li>If the @c type parameter was invalid.</li>
		 *     <li>If the second to last short name in the full name of the new
		 *         widget did not identify a grid.</li>
		 *     <li>If there was already a widget within the specified cell.
		 *     </li></ul>
		 * If an error is logged, no widget will be created or added.
		 * @param newWidgetType The type of widget to create.
		 * @param name          The name of the new widget. Must include the grid.
		 * @param row           The row to add the widget to.
		 * @param col           The column to add the widget to.
		 * @param signalHandler The name of the custom signal handler to assign to
		 *                      this widget, if any.
		 */
		void _addWidgetToGrid(const std::string& newWidgetType,
			const std::string& name, const std::size_t row, const std::size_t col,
			const std::string& signalHandler = "");

		/**
		 * Connects a signal handler to a given widget.
		 * See the script interface documentation in \c registerInterface() for a
		 * rundown on how signal handlers work. This method is used to provide an
		 * additional signal handler for a given widget by directly invoking a
		 * callback given to the engine. If \c nullptr is given, then the
		 * registered callback will be freed.
		 * @param name    The name of the widget to configure.
		 * @param handler The function to invoke when a widget emits a signal.
		 */
		void _connectSignalHandler(const std::string& name,
			asIScriptFunction* const handler);

		void _disconnectSignalHandlers(const CScriptArray* const names);

		/**
		 * Gets the name of the given widget's parent.
		 * If no widget exists with the given name, then an error will be logged
		 * and a blank string will be returned.
		 * @param  name The name of the widget to query.
		 * @return The full name of the parent.
		 */
		std::string _getParent(const std::string& name);

		/**
		 * Removes a specified widget, and all the widgets that are within it.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be removed.
		 * @param name The name of the widget to remove.
		 */
		void _removeWidget(const std::string& name);

		/**
		 * Removes all the widgets from a given container.
		 * If no widget exists with the given name, or if the widget wasn't a
		 * container, then an error will be logged and no widget will be removed.
		 * @param name The name of the container whose widgets are to be removed.
		 */
		void _removeWidgetsFromContainer(const std::string& name);

		/**
		 * Updates the setfocus to point to a given widget.
		 * If no widget exists with the given name, then an error will be logged
		 * and the setfocus will not be changed.
		 * @param name The name fo the widget who will have the setfocus.
		 */
		void _setWidgetFocus(const std::string& name);

		/**
		 * Sets a widget's font.
		 * If no widget or font exists with the given name, then an error will be
		 * logged and no font will be changed.
		 * @param name     The name of the widget to change.
		 * @param fontName The name of the font to assign to the widget.
		 */
		void _setWidgetFont(const std::string& name, const std::string& fontName);

		/**
		 * Sets the global UI font.
		 * If no font exists with the given name, then an error will be logged and
		 * no font will be changed.
		 * @param fontName The name of the font to assign.
		 */
		void _setGlobalFont(const std::string& fontName);

		/**
		 * Updates a widget's location.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be moved.
		 * @param name The name of the widget to relocate.
		 * @param x    The expression to assign to the widget's X coordinate.
		 * @param y    The expression to assign to the widget's Y coordinate.
		 */
		void _setWidgetPosition(const std::string& name, const std::string& x,
			const std::string& y);

		/**
		 * Retrieves a widget's absolute location.
		 * If no widget exists with the given name, then an error will be logged
		 * and <tt>(0.0f,0.0f)</tt> will be returned.
		 * @param  name The name of the widget to query.
		 * @return The absolute position of the top-left point of the widget.
		 */
		sf::Vector2f _getWidgetAbsolutePosition(const std::string& name);

		/**
		 * Updates a widget's origin.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be updated.
		 * @param name The name of the widget to update.
		 * @param x    The new origin to set along the widget's X axis (0.0-1.0).
		 * @param y    The new origin to set along the widget's Y axis (0.0-1.0).
		 */
		void _setWidgetOrigin(const std::string& name, const float x,
			const float y);

		/**
		 * Updates a widget's size.
		 * If a blank string is given for either dimension, that means that the
		 * widget should retain their size layout in that dimension.\n
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be resized.
		 * @param name The name of the widget to resize.
		 * @param w    The expression to assign to the widget's width.
		 * @param h    The expression to assign to the widget's height.
		 */
		void _setWidgetSize(const std::string& name, const std::string& w,
			const std::string& h);

		/**
		 * Gets the full size of a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and a blank vector will be
		 * returned.
		 * @param  name The name of the widget to query.
		 * @return The full size of the widget, which includes any borders, etc.
		 */
		sf::Vector2f _getWidgetFullSize(const std::string& name);

		/**
		 * Updates a widget's enabled property.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.
		 * @param name   The name of the widget to enable/disable.
		 * @param enable \c TRUE if the widget should be enabled, \c FALSE if it
		 *               should be disabled.
		 */
		void _setWidgetEnabled(const std::string& name, const bool enable);

		/**
		 * Retrieves the enabled property of a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c FALSE will be returned.
		 * @param  name The name of the widget to query.
		 * @return \c TRUE if the widget is enabled, \c FALSE otherwise.
		 */
		bool _getWidgetEnabled(const std::string& name) const;

		/**
		 * Updates a widget's visibility property.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.
		 * @param name    The name of the widget to show/hide.
		 * @param visible \c TRUE if the widget should be shown, \c FALSE if it
		 *                should be hidden.
		 */
		void _setWidgetVisibility(const std::string& name, const bool visible);

		/**
		 * Retrieves the visibility property of a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c FALSE will be returned.
		 * @param  name The name of the widget to query.
		 * @return \c TRUE if the widget is visible, \c FALSE otherwise.
		 */
		bool _getWidgetVisibility(const std::string& name) const;

		/**
		 * Moves a widget to the front of its parent container.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be moved in the Z order.
		 * @param name The name of the widget to move to the front.
		 */
		void _moveWidgetToFront(const std::string& name);

		/**
		 * Moves a widget to the back of its parent container.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be moved in the Z order.
		 * @param name The name of the widget to move to the back.
		 */
		void _moveWidgetToBack(const std::string& name);

		/**
		 * Special string used with \c _setWidgetDirectionalFlow() to represent the
		 * previous widget.
		 */
		static const std::string GOTO_PREVIOUS_WIDGET;

		/**
		 * Sets which directional controls should move the setfocus to which
		 * widgets, if the current setfocus is on a given widget.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.\n
		 * All widgets provided must be in the same menu. Otherwise, an error will
		 * be logged, and no widget will be changed.\n
		 * The second to fifth parameters can be blank, which case the given input
		 * will not change the selection. These parameters can also be the value of
		 * `GOTO_PREVIOUS_WIDGET`. This is a special value which means "navigate
		 * to the previously selected widget."
		 * @param      name The name of the widget to amend.
		 * @param    upName The name of the widget to move the selection to if "up"
		 *                  is pressed.
		 * @param  downName The name of the widget to move the selection to if
		 *                  "down" is pressed.
		 * @param  leftName The name of the widget to move the selection to if
		 *                  "left" is pressed.
		 * @param rightName The name of the widget to move the selection to if
		 *                  "right" is pressed.
		 * @sa    \c sfx::gui::GOTO_PREVIOUS_WIDGET.
		 */
		void _setWidgetDirectionalFlow(const std::string& name,
			const std::string& upName, const std::string& downName,
			const std::string& leftName, const std::string& rightName);

		/**
		 * Used to select widgets that are selected first when a directional
		 * control is input.
		 * When a menu is active, and no widget has been selected on that menu yet
		 * since the game was launched, a widget within that menu is always
		 * selected first. The given widget is configured to be that first
		 * widget.\n
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.
		 * @param name The name of the widget to assign as the first to be
		 *             selected. Or the name of the menu to prevent directional
		 *             controls for.
		 */
		void _setWidgetDirectionalFlowStart(const std::string& name);

		/**
		 * Used to prevent directional controls from selecting widgets on a given
		 * menu.
		 * This is the default behaviour for all menus.\n
		 * If the given menu doesn't exist, an error will be logged and no menu
		 * will be changed.
		 * @param menu The name of the menu to amend.
		 */
		void _clearWidgetDirectionalFlowStart(const std::string& menu);

		/**
		 * Used to manually set the directionally-selected widget.
		 * If no widget exists with the given name, then an error will be logged
		 * and no widget will be changed.
		 * @param name The name of the widget to select.
		 */
		void _setWidgetDirectionalFlowSelection(const std::string& name);

		/**
		 * Sets the sprite to use as the angle bracket which surrounds widgets
		 * currently selected via the directional controls.
		 * If neither the spritesheet or sprite exists, an error will be logged and
		 * no changes will be made.
		 * @param corner The angle bracket to amend. Either "UL", "UR", "LL", or
		 *               "LR". Input is trimmed and case-insensitive.
		 * @param sheet  The name of the spritesheet which contains the sprite to
		 *               set.
		 * @param key    The name of the sprite to set.
		 */
		void _setDirectionalFlowAngleBracketSprite(const std::string& corner,
			const std::string& sheet, const std::string& key);

		/**
		 * Updates a widget's caption.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name      The name of the widget to change.
		 * @param text      The new caption.
		 * @param variables Optional list of variables to insert into the caption.
		 */
		void _setWidgetText(const std::string& name, const std::string& text,
			CScriptArray* variables);

		/**
		 * Gets the widget's caption.
		 * For most widget types, this will return the translated caption. For
		 * \c EditBox widgets, this will return the text that the user has typed
		 * in.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and a blank string will be
		 * returned.
		 * @param  name The name of the widget to query.
		 * @return The caption/text assigned to the widget.
		 */
		std::string _getWidgetText(const std::string& name);

		void _setWidgetChecked(const std::string& name, const bool checked);

		bool _isWidgetChecked(const std::string& name);

		/**
		 * Sets an \c EditBox to only accept unsigned ints.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c EditBox to change.
		 */
		void _onlyAcceptUIntsInEditBox(const std::string& name);

		/**
		 * Sets a widget's character size.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no size will be changed.
		 * @param name The name of the widget to change.
		 * @param size The new character size.
		 */
		void _setWidgetTextSize(const std::string& name, const unsigned int size);

		void _setWidgetTextStyles(const std::string& name,
			const std::string& styles);

		/**
		 * Sets a widget's text's maximum width.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no size will be changed.
		 * @param name The name of the widget to change.
		 * @param w    If the text exceeds this width, it will wrap around to the
		 *             next line.
		 */
		void _setWidgetTextMaximumWidth(const std::string& name, const float w);

		/**
		 * Sets a widget's text colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name   The name of the widget to change.
		 * @param colour The new text colour.
		 */
		void _setWidgetTextColour(const std::string& name,
			const sf::Color& colour);

		/**
		 * Sets a widget's text outline colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name   The name of the widget to change.
		 * @param colour The new text outline colour.
		 */
		void _setWidgetTextOutlineColour(const std::string& name,
			const sf::Color& colour);

		/**
		 * Sets a widget's text outline thickness.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no caption will be changed.
		 * @param name      The name of the widget to change.
		 * @param thickness The new text outline thickness.
		 */
		void _setWidgetTextOutlineThickness(const std::string& name,
			const float thickness);

		/**
		 * Sets a widget's text alignment.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no alignment will be
		 * changed.
		 * @param name The name of the widget to change.
		 * @param h    The new horizontal alignment.
		 * @param v    The new vertical alignment.
		 */
		void _setWidgetTextAlignment(const std::string& name,
			const tgui::Label::HorizontalAlignment h,
			const tgui::Label::VerticalAlignment v);

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
			const std::string& key);

		/**
		 * Removes a widget's sprite.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no sprite will be cleared.
		 * @param name The name of the widget to change.
		 */
		void _clearWidgetSprite(const std::string& name);

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
			const bool overrideSetSize);

		/**
		 * Sets a widget's background colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no colour will be set.
		 * @param name   The name of the widget to change.
		 * @param colour The colour to set to the background of the widget.
		 */
		void _setWidgetBgColour(const std::string& name, const sf::Color& colour);

		/**
		 * Sets a widget's border size.
		 * Sets each side's border to the same size.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no border size will be set.
		 * @param name The name of the widget to change.
		 * @param size The size to set to the widget's border.
		 */
		void _setWidgetBorderSize(const std::string& name, const float size);

		/**
		 * Sets a widget's border colour.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no colour will be set.
		 * @param name   The name of the widget to change.
		 * @param colour The colour to set to the widget's border.
		 */
		void _setWidgetBorderColour(const std::string& name,
			const sf::Color& colour);

		/**
		 * Sets a widget's rounded border radius.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no radius will be set.
		 * @param name   The name of the widget to change.
		 * @param radius The radius to set to the widget's rounded border.
		 */
		void _setWidgetBorderRadius(const std::string& name, const float radius);

		/**
		 * Sets a widget's index within its container.
		 * Used to reorder widgets within the container.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no index will be set. An
		 * error will also be logged if a given index is too high.
		 * @param name  The name of the widget to change.
		 * @param index The new index to set to the widget.
		 */
		void _setWidgetIndex(const std::string& name, const std::size_t index);

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
			const std::size_t oldIndex, const std::size_t newIndex);

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
			const std::size_t index, const float ratio);
		
		/**
		 * Updates a widget's default text.
		 * E.g. the editbox will display the default text when it has no
		 * text/caption.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no text will be changed.
		 * @param name      The name of the widget to change.
		 * @param text      The new default text.
		 * @param variables Optional list of variables to insert into the text.
		 */
		void _setWidgetDefaultText(const std::string& name,
			const std::string& text, CScriptArray* variables);

		/**
		 * Adds an item to a widget.
		 * E.g. appends an item to a listbox.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be added.
		 * @param name      The name of the widget to add the item to.
		 * @param text      The text of the new item.
		 * @param variables Optional list of variables to insert into the item
		 *                  text.
		 */
		void _addItem(const std::string& name, const std::string& text,
			CScriptArray* variables);

		/**
		 * Clears all items from a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be removed.
		 * @param name The name of the widget to clear the items of.
		 */
		void _clearItems(const std::string& name);

		/**
		 * Selects an item by index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no item will be selected. An
		 * error will also be reported if the given index was out of range.
		 * @param name  The name of the widget which contains the item to select.
		 * @param index The 0-based index of the item to select.
		 */
		void _setSelectedItem(const std::string& name, const std::size_t index);

		void _deselectItem(const std::string& name);

		/**
		 * Gets the currently selected item's index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c -1 will be returned. If
		 * no item was selected, \c -1 will also be returned.
		 * @param  name The name of the widget to query.
		 * @return The index of the currently selected item.
		 */
		int _getSelectedItem(const std::string& name);

		/**
		 * Retrieves the text of the currently selected item of a widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and a blank string will be
		 * returned.
		 * @param  name The name of the widget to query.
		 * @return The translated text of the item.
		 */
		std::string _getSelectedItemText(const std::string& name);

		CScriptArray* _getSelectedItemTextHierarchy(const std::string& name);

		/**
		 * Sets the number of items to display in a \c ComboBox.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no changes will be made.
		 * @param name  The name of the \c ComboBox to edit.
		 * @param items The number of items to display at one time when the
		 *              \c ComboBox is opened. 0 means show all items.
		 */
		void _setItemsToDisplay(const std::string& name, const std::size_t items);

		/**
		 * Adds a tab to a widget.
		 * E.g. appends a tab to a \c Tabs widget.\n
		 * Adding a tab will not select it.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no tab will be added.
		 * @param name      The name of the widget to add the tab to.
		 * @param text      The text of the new tab.
		 * @param variables Optional list of variables to insert into the tab text.
		 */
		void _addTab(const std::string& name, const std::string& text,
			CScriptArray* variables);

		/**
		 * Selects a tab by index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no tab will be selected. An
		 * error will also be reported if the given index was out of range, or if
		 * the given tab is either invisible or disabled. If a selection operation
		 * failed and the widget was a \c Tabs widget, an attempt will be made to
		 * select the previously selected tab, if there was one.
		 * @param name  The name of the widget which contains the tab to select.
		 * @param index The 0-based index of the tab to select.
		 */
		void _setSelectedTab(const std::string& name, const std::size_t index);

		/**
		 * Gets the currently selected tab's index.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c -1 will be returned. If
		 * no tab was selected, \c -1 will also be returned.
		 * @param  name The name of the widget to query.
		 * @return The index of the currently selected tab.
		 */
		int _getSelectedTab(const std::string& name);

		/**
		 * Gets the number of tabs in a \c Tabs widget.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c 0 will be returned.
		 * @param  name The name of the widget to query.
		 * @return The number of tabs in the given widget.
		 */
		std::size_t _getTabCount(const std::string& name);

		/**
		 * Returns the number of widgets within a container.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and 0 will be returned.
		 * @param  name The name of the container to query.
		 * @return The number of widgets within the container.
		 */
		std::size_t _getWidgetCount(const std::string& name);

		/**
		 * Sets the scrollbar policy for a scrollable panel's horizontal scrollbar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no policy will be changed.
		 * @param name   The name of the widget to edit.
		 * @param policy The policy the scrollbar should have.
		 */
		void _setHorizontalScrollbarPolicy(const std::string& name,
			const tgui::Scrollbar::Policy policy);

		/**
		 * Sets the scroll amount for a scrollable panel's horizontal scrollbar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no amount will be changed.
		 * @param name   The name of the widget to edit.
		 * @param amount The amount the scrollbar should scroll by.
		 */
		void _setHorizontalScrollbarAmount(const std::string& name,
			const unsigned int amount);

		/**
		 * Sets the scroll amount for a scrollable panel's vertical scrollbar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no amount will be changed.
		 * @param name   The name of the widget to edit.
		 * @param amount The amount the scrollbar should scroll by.
		 */
		void _setVerticalScrollbarAmount(const std::string& name,
			const unsigned int amount);

		void _setVerticalScrollbarValue(const std::string& name,
			const unsigned int value);

		/**
		 * Gets the width of the scrollbars in a \c ScrollablePanel.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c 0.0f will be returned.
		 * @param  name The name of the widget to query.
		 * @return The width of the scrollbar.
		 */
		float _getScrollbarWidth(const std::string& name);

		/**
		 * Sets the padding applied to a group of widgets.
		 * If the name of a \c Grid is given, each of its widgets will be assigned
		 * the given padding. Note that this will only be applied to the widgets
		 * that exist in the \c Grid at the time of calling!\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no padding will be changed.
		 * @param name    The name of the widget to edit.
		 * @param padding The padding to set.
		 */
		void _setGroupPadding(const std::string& name, const std::string& padding);

		/**
		 * Sets the padding applied to a group of widgets.
		 * If the name of a \c Grid is given, each of its widgets will be assigned
		 * the given padding. Note that this will only be applied to the widgets
		 * that exist in the \c Grid at the time of calling!\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no padding will be changed.
		 * @param name   The name of the widget to edit.
		 * @param left   The padding to set to the left side.
		 * @param top    The padding to set to the top side.
		 * @param right  The padding to set to the right side.
		 * @param bottom The padding to set to the bottom side.
		 */
		void _setGroupPadding(const std::string& name, const std::string& left,
			const std::string& top, const std::string& right,
			const std::string& bottom);

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
			const tgui::Grid::Alignment alignment);

		/**
		 * Sets the padding applied to a widget within a grid.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no padding will be changed.
		 * An error will also be logged if the \c row and \c col parameters are out
		 * of range.
		 * @param name    The name of the grid to edit.
		 * @param row     The 0-based row ID of the widget to edit.
		 * @param col     The 0-based column ID of the widget to edit.
		 * @param padding The padding to set to the widget.
		 */
		void _setWidgetPaddingInGrid(const std::string& name,
			const std::size_t row, const std::size_t col,
			const std::string& padding);

		/**
		 * Sets the space between widgets in Vertical and Horizontal Layouts.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no space will be applied.
		 * @param name  The name of the layout to edit.
		 * @param space The new distance to apply.
		 */
		void _setSpaceBetweenWidgets(const std::string& name, const float space);

		/**
		 * Special method which applies a large collection of sprites to \c Picture
		 * and/or \c BitmapButton widgets within a container.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no changes will be made. An
		 * error will also be logged if \c sprites was \c nullptr or empty.
		 * @param name        Name of the container containing the widgets to edit.
		 * @param spritesheet The spritesheet to which all the \c sprites belong.
		 * @param sprites     String array containing sprite keys, one element per
		 *                    applicable widget.
		 */
		void _applySpritesToWidgetsInContainer(const std::string& name,
			const std::string& spritesheet, const CScriptArray *const sprites);

		// MENUS //

		/**
		 * Adds a new menu to the given \c MenuBar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.\n
		 * If this method is called outside of \c _load(), an error will be logged
		 * and no menu will be added.\n
		 * If no items were added to the previously added menu, then a warning will
		 * be logged.
		 * @param  name      The name of the \c MenuBar widget to add to.
		 * @param  text      The text of the new menu.
		 * @param  variables Optional list of variables to insert into the text.
		 * @return The menu item ID of the newly created menu.
		 */
		MenuItemID _addMenu(const std::string& name, const std::string& text,
			CScriptArray* variables);

		/**
		 * Adds a new menu item to the most recently added menu or submenu.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.\n
		 * If this method is called outside of \c _load(), an error will be logged
		 * and no menu item will be added.\n
		 * If no menus were added yet, an error will be logged and no item will be
		 * added.\n
		 * @param  name      The name of the \c MenuBar widget to add to.
		 * @param  text      The text of the new menu item.
		 * @param  variables Optional list of variables to insert into the text.
		 * @return The menu item ID of the newly created menu item.
		 */
		MenuItemID _addMenuItem(const std::string& name, const std::string& text,
			CScriptArray* variables);

		/**
		 * Creates a new submenu within the most recently added menu or submenu,
		 * and adds an item to it.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.\n
		 * If this method is called outside of \c _load(), an error will be logged
		 * and no menu item will be added.\n
		 * If no menus were added yet, an error will be logged and no item will be
		 * added.\n
		 * If the most recently added menu was empty, then a warning will be
		 * logged, but the call will still have the same result as if
		 * \c _addMenuItem() was called.
		 * @param  name      The name of the \c MenuBar widget to add to.
		 * @param  text      The text of the new menu item.
		 * @param  variables Optional list of variables to insert into the text.
		 * @return The menu item ID of the newly created menu item.
		 */
		MenuItemID _addMenuItemIntoLastItem(const std::string& name,
			const std::string& text, CScriptArray* variables);

		/**
		 * Exits the current submenu.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.\n
		 * If this method is called outside of \c _load(), an error will be logged
		 * and no changes will be made.\n
		 * If the most recently created menu item is not in a submenu, an error
		 * will be logged and no changes will be made.
		 * @param name      The name of the \c MenuBar widget to add to.
		 * @param text      The text of the new menu item.
		 * @param variables Optional list of variables to insert into the text.
		 */
		void _exitSubmenu(const std::string& name);

		// CHILDWINDOW //

		/**
		 * Instructs the engine to automatically handle minimise and maximise
		 * functionality when the \c Minimize and \c Maximize buttons are pressed
		 * on a \c ChildWindow.
		 * Note that if this option is set, any defined script handlers for the
		 * \c Minimize and \c Maximize signals will be called \em after the engine
		 * has completed changing the properties of the \c ChildWindow.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name   The name of the \c ChildWindow to amend.
		 * @param handle \c TRUE if the engine should handle minimise and maximise
		 *               logic, \c FALSE if not. The default is \c TRUE.
		 */
		void _autoHandleMinMax(const std::string& name, const bool handle);

		/**
		 * Sets which buttons to show in the given \c ChildWindow's titlebar.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name    The name of the \c ChildWindow to amend.
		 * @param buttons The buttons to assign.
		 */
		void _setChildWindowTitleButtons(const std::string& name,
			const unsigned int buttons);

		/**
		 * Sets a widget to be resizable or not resizable.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name      The name of the widget to amend.
		 * @param resizable \c TRUE if the widget should be resizableby the user,
		 *                  \c FALSE if not.
		 */
		void _setWidgetResizable(const std::string& name, const bool resizable);

		/**
		 * Gets a \c ChildWindow's titlebar height.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c 0.0f will be returned.
		 * @param  name The name of the widget to query.
		 * @return The titlebar height.
		 */
		float _getTitleBarHeight(const std::string& name);

		CScriptArray* _getBorderWidths(const std::string& name);

		/**
		 * Opens a \c ChildWindow to a given location.
		 * If the given \c ChildWindow was either maximised or minimised, it will
		 * also be restored.\n
		 * If the widget was already open (i.e. visible), then the widget will
		 * still be moved and brought to the front.\n
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c ChildWindow to open.
		 * @param x    The X position to move the \c ChildWindow to.
		 * @param y    The Y position to move the \c ChildWindow to.
		 */
		void _openChildWindow(const std::string& name, const std::string& x,
			const std::string& y);

		/**
		 * Closes a \c ChildWindow by making it invisible.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c ChildWindow to close.
		 */
		void _closeChildWindow(const std::string& name);

		/**
		 * Restores a \c ChildWindow if it was maximised or minimised.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c ChildWindow to restore.
		 */
		void _restoreChildWindow(const std::string& name);

		/**
		 * Implementation of \c _restoreChildWindow().
		 * Has no effect if the given window was not maximised or minimised.
		 * @param window Pointer to the \c ChildWindow.
		 */
		void _restoreChildWindowImpl(const tgui::ChildWindow::Ptr& window,
			child_window_properties& data);

		/**
		 * Checks if a \c ChildWindow is currently open.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and \c FALSE will be returned.
		 * @param  name The name of the \c ChildWindow to query.
		 * @return \c TRUE if the \c ChildWindow is visible, \c FALSE if not.
		 */
		bool _isChildWindowOpen(const std::string& name);

		// FILEDIALOG //

		/**
		 * Sets every caption in the given file dialog.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name          The name of the \c FileDialog widget.
		 * @param title         The title of the \c FileDialog.
		 * @param v0            The variables to insert into the above string.
		 * @param confirm       The text of the confirm/open button.
		 * @param v1            The variables to insert into the above string.
		 * @param cancel        The text of the cancel button.
		 * @param v2            The variables to insert into the above string.
		 * @param createFolder  The text of the create folder button.
		 * @param v3            The variables to insert into the above string.
		 * @param filenameLabel The text of the filename label.
		 * @param v4            The variables to insert into the above string.
		 * @param nameColumn    The text of the name column in the listview.
		 * @param v5            The variables to insert into the above string.
		 * @param sizeColumn    The text of the size column in the listview.
		 * @param v6            The variables to insert into the above string.
		 * @param modifyColumn  The text of the modified column in the listview.
		 * @param v7            The variables to insert into the above string.
		 * @param allFiles      The caption assigned to the All files filter.
		 * @param v8            The variables to insert into the above string.
		 */
		void _setFileDialogStrings(const std::string& name,
			const std::string& title, CScriptArray* v0, const std::string& confirm,
			CScriptArray* v1, const std::string& cancel, CScriptArray* v2,
			const std::string& createFolder, CScriptArray* v3,
			const std::string& filenameLabel, CScriptArray* v4,
			const std::string& nameColumn, CScriptArray* v5,
			const std::string& sizeColumn, CScriptArray* v6,
			const std::string& modifyColumn, CScriptArray* v7,
			const std::string& allFiles, CScriptArray* v8);

		/**
		 * Gets the selected files from the given file dialog.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param  name The name of the \c FileDialog widget.
		 * @return The selected paths.
		 */
		CScriptArray* _getFileDialogSelectedPaths(const std::string& name);

		/**
		 * Adds a file type filter to a \c FileDialog.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name      The name of the \c FileDialog to update.
		 * @param caption   The caption of the filter.
		 * @param variables The variables to insert into the caption.
		 * @param filters   The expressions that make up the filter.
		 */
		void _addFileDialogFileTypeFilter(const std::string& name,
			const std::string& caption, CScriptArray *const variables,
			CScriptArray *const filters);

		/**
		 * Clears the file type filters of a given \c FileDialog.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c FileDialog to update.
		 */
		void _clearFileDialogFileTypeFilters(const std::string& name);

		/**
		 * Sets whether a selected file in a \c FileDialog must exist or not.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name      The name of the \c FileDialog to update.
		 * @param mustExist \c TRUE if the selected path must exist, \c FALSE if
		 *                  not.
		 */
		void _setFileDialogFileMustExist(const std::string& name,
			const bool mustExist);

		/**
		 * Sets the default file filter in a \c FileDialog.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name  The name of the \c FileDialog to update.
		 * @param index The 0-based index of the file filter to set as the default.
		 */
		void _setFileDialogDefaultFileFilter(const std::string& name,
			const std::size_t index);

		/**
		 * Sets a \c FileDialog's current path.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name The name of the \c FileDialog to update.
		 * @param path The path to display the files of.
		 */
		void _setFileDialogPath(const std::string& name, const std::string& path);

		// MESSAGEBOX //

		/**
		 * Sets a \c MessageBox's title and text.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name      The name of the \c MessageBox to update.
		 * @param title     The title of the \c MessageBox.
		 * @param titleVars The variables to insert into the title.
		 * @param text      The text of the \c MessageBox.
		 * @param textVars  The variables to insert into the text.
		 */
		void _setMessageBoxStrings(const std::string& name,
			const std::string& title, CScriptArray* titleVars,
			const std::string& text, CScriptArray* textVars);

		/**
		 * Add a button to a \c MessageBox.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.
		 * @param name      The name of the \c MessageBox to update.
		 * @param text      The text of the new button.
		 * @param variables The variables to insert into the text.
		 */
		void _addMessageBoxButton(const std::string& name, const std::string& text,
			CScriptArray* variables);

		// TREEVIEW //

		/**
		 * Adds an item to a \c TreeView.
		 * If no widget exists with the given name, or if it doesn't support the
		 * operation, then an error will be logged and no widget will be changed.\n
		 * @warning Note that \c TreeView items cannot be translated!
		 * @param   name      The name of the \c TreeView to edit.
		 * @param   hierarchy The new item hierarchy. If any parent item does not
		 *                    exist, they will be created.
		 */
		void _addTreeViewItem(const std::string& name,
			const CScriptArray* const hierarchy);

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
		std::unordered_map<std::string, CScriptWrapper<asIScriptFunction>>
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
