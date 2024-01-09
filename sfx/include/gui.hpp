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
#include "tgui/Layout.hpp"

namespace sfx {
	/**
	 * Represents a widget ID.
	 */
	typedef std::size_t WidgetID;

	/**
	 * Represents a reference to a widget ID.
	 * Largely unnecessary so long as a widget ID is numeric, but if it should
	 * turn into an object for whatever reason, it will be really easy to turn
	 * <tt>const WidgetID</tt>s into <tt>const WidgetID&</tt>s.
	 */
	typedef WidgetID WidgetIDRef;

	/**
	 * Represents a menu item ID.
	 */
	typedef std::size_t MenuItemID;

	/**
	 * Represents "no widget."
	 * This value is intended to be constant.
	 */
	static WidgetID NO_WIDGET = 0;

	/**
	 * Represents the root widget that all widgets that are displayed are children
	 * of.
	 * This widget will always have the "first" slot in the container.
	 * This value is intended to be constant.
	 */
	static WidgetID ROOT_WIDGET = 1;

	/**
	 * Represents "no menu item."
	 * This value is intended to be constant.
	 */
	static MenuItemID NO_MENU_ITEM_ID = std::numeric_limits<MenuItemID>::max();

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
		 * @param  newMenu   The name of the menu to display. If a menu with the
		 *                   given name does not exist, an error will be logged and
		 *                   no other changes will occur. If the current menu is
		 *                   given, no changes will occur.
		 * @param  callClose If @c TRUE, the current menu's @c Close() script
		 *                   function will be called, if it exists.
		 * @param  callOpen  If @c TRUE, the new menu's @c Open() script function
		 *                   will be called, if it exists.
		 * @safety No guarantee.
		 */
		void setGUI(const std::string& newMenu, const bool callClose = true,
			const bool callOpen = true);

		/**
		 * Gets the name of the menu currently showing.
		 * @return The menu currently being drawn.
		 */
		inline std::string getGUI() const {
			return _currentGUI;
		}

		/**
		 * Adds spritesheets which can be used with the GUI menus.
		 * @param sheets A pointer to the \c animated_spritesheets to use with the
		 *               GUI menus.
		 */
		void setSpritesheets(
			const std::shared_ptr<sfx::animated_spritesheets>& sheets);

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
		 * Each menu has a method called \c Periodic(). This method will invoke it
		 * on the current menu, if there is one. See the script interface
		 * documentation for the \c Menu interface for more information.\n
		 * This method also handles directional controls selecting different
		 * widgets based on the current menu's configuration. If "select" is
		 * triggered, a \c MouseReleased signal will be issued to the currently
		 * selected widget in the current menu, if there is any, and if that widget
		 * is of type \c ButtonBase, \c Button, \c BitmapButton, or \c ListBox.
		 * @param  ui Pointer to the @c user_input instance to send information on.
		 *            Note that the @c user_input::update() method should already
		 *            have been called before a call to @c handleInput().
		 * @safety No guarantee.
		 */
		void handleInput(const std::shared_ptr<sfx::user_input>& ui);

		/**
		 * Handles all widget signals by attempting to invoke the necessary script
		 * methods.
		 * For a list of available signals, you can view the \c signal namespace in
		 * in \c guiconstants.hpp.
		 * @warning You must assume that this method will invalidate any iterators,
		 *          pointers, and references to widget data! This is because a
		 *          signal handler may indirectly cause a reallocation of the
		 *          widget data container.
		 * @param   widget     The widget sending the signal.
		 * @param   signalName The name of the signal being sent.
		 * @return  \c TRUE if a signal handler was called, \c FALSE if not.
		 * @safety  No guarantee.
		 */
		bool signalHandler(tgui::Widget::Ptr widget,
			const tgui::String& signalName);

		/**
		 * Handles \c MenuItemClicked signals.
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
		 * menu item IDs, all starting from 0.
		 * @param menuBarID The ID of the \c MenuBar whose item was selected.
		 * @param index     The 0-based index of the item selected.
		 */
		void menuItemClickedSignalHandler(const sfx::WidgetID menuBarID,
			const MenuItemID index);

		/**
		 * Handles \c ButtonPressed signals from \c MessageBox widgets.
		 * The 0-based ID of the button that was clicked will be set to the
		 * \c MessageBox widget's data. The first button added will have an ID of
		 * 0, the second button will have an ID of 1, and so on. If the button's
		 * index couldn't be found, the number of buttons will be stored, but this
		 * should never happen.
		 * @param id      The ID of the \c MessageBox whose button was pressed.
		 * @param caption The caption of the selected button.
		 */
		void messageBoxButtonPressedSignalHandler(const sfx::WidgetID id,
			const tgui::String& caption);

		/**
		 * Handles \c Closing signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before invoking the
		 * script's signal handler. If the \c ChildWindow was configured not to
		 * handle this logic, then the appropriate script signal handler will still
		 * be invoked.
		 * @warning This engine should never emit a Closed signal for normal
		 *          \c ChildWindow widgets! Use the \c Closing signal instead!
		 * @param   window Pointer to the \c ChildWindow that was closed.
		 * @param   abort  Will always be set to \c TRUE.
		 */
		void closingSignalHandler(const tgui::ChildWindow::Ptr& window,
			bool* abort);

		/**
		 * Handles \c Closing signals for subclasses of \c ChildWindow.
		 * @param window Pointer to the subclass of \c ChildWindow that was closed.
		 * @param abort  Will be updated by the script handler, if there is one.
		 */
		void basicClosingSignalHandler(const tgui::Widget::Ptr& window,
			bool* abort);

		/**
		 * Handles \c Minimized signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before invoking the
		 * signal handler/s. If the \c ChildWindow was configured not to handle
		 * this logic, then the script's signal handlers will still be invoked.
		 * @param window Pointer to the \c ChildWindow that was minimised.
		 */
		void minimizedSignalHandler(const tgui::ChildWindow::Ptr& window);

		/**
		 * Handles \c Maximized signals.
		 * If a \c ChildWindow is configured to automatically handle minimise and
		 * maximise logic, this handler will carry it out, before invoking the
		 * signal handler/s. If the \c ChildWindow was configured not to handle
		 * this logic, then the script's signal handlers will still be invoked.
		 * @param window Pointer to the \c ChildWindow that was maximised.
		 */
		void maximizedSignalHandler(const tgui::ChildWindow::Ptr& window);

		/**
		 * If an \c EditBox or \c TextArea gains focus, an internal flag is set to
		 * \c TRUE.
		 * This can be used to cancel game input when the user intends to type.
		 * @param widget Pointer to the widget that gained focus.
		 */
		void textBoxFocusedSignalHandler(const tgui::Widget::Ptr& widget);

		/**
		 * If an \c EditBox or \c TextArea loses focus, an internal flag is set to
		 * \c FALSE.
		 * This can be used to re-enable game input when the user no longer intends
		 * to type.
		 * @param widget Pointer to the widget that gained focus.
		 */
		void textBoxUnfocusedSignalHandler(const tgui::Widget::Ptr& widget);

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
		 * Returns the configured select control.
		 * @return The name of the control that is used to trigger "select."
		 */
		std::string getSelectControl() const;

		/**
		 * Finds out if a widget is under the current mouse position.
		 * If a user input object hasn't been given, then \c FALSE will be returned
		 * and an error will be logged.
		 * @return \c TRUE if the mouse is over a visible widget, \c FALSE if there
		 *         isn't a visible one. Widgets that are not themselves rendered
		 *         count as invisible, e.g. this method will always return \c FALSE
		 *         if the mouse is only hovering over a \c Group.
		 */
		bool isWidgetUnderMouse() const;

		/**
		 * Animates the current GUI menu.
		 * Any sprites are animated. If a langauge dictionary has been given, all
		 * captions will also be translated.
		 * @return Always returns \c FALSE.
		 * @safety No guarantee.
		 * @sa     \c sfx::gui::setLanguageDictionary()
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		///////////
		// TYPES //
		///////////

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
			 * Was the \c ChildWindow resizable before minimising or maximising?
			 */
			bool isResizable = false;

			/**
			 * Was the \c ChildWindow position locked before minimising or
			 * maximising?
			 */
			bool isPositionLocked = false;

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
			 * @param  id The ID of the \c ChildWindow.
			 * @return The X \c Layout2d coordinate that the \c ChildWindow should
			 *         be moved to when minimised.
			 */
			tgui::String minimise(const WidgetIDRef id);

			/**
			 * Removes a \c ChildWindow from the list.
			 * @param id The ID of the \c ChildWindow.
			 */
			void restore(const WidgetIDRef id);
		private:
			/**
			 * The list of \c ChildWindows.
			 */
			std::vector<WidgetID> _windows;
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
		 * Stores data associated with a widget.
		 */
		struct widget_data {
			/**
			 * Pointer to the widget.
			 * Use \c _getWidgetID() to retrieve the ID of the widget.
			 */
			tgui::Widget::Ptr ptr;

			/**
			 * Dynamically casts the widget pointer to a given type.
			 * @tparam T The type to down cast to.
			 * @return The down cast pointer.
			 */
			template<typename T>
			inline typename T::Ptr castPtr() const {
				return std::dynamic_pointer_cast<T>(ptr);
			}

			/**
			 * Stores the original caption/s assigned to this widget.
			 */
			std::variant<std::monostate, SingleCaption, ListOfCaptions>
				originalCaption;

			/**
			 * Dictates where setfocus should move to when the setfocus is on a
			 * given widget.
			 */
			struct directional_flow {
				/**
				 * The ID of the widget to move the setfocus to when pressing "up."
				 */
				WidgetID up = NO_WIDGET;

				/**
				 * The ID of the widget to move the setfocus to when pressing
				 * "down."
				 */
				WidgetID down = NO_WIDGET;

				/**
				 * The ID of the widget to move the setfocus to when pressing
				 * "left."
				 */
				WidgetID left = NO_WIDGET;

				/**
				 * The ID of the widget to move the setfocus to when pressing
				 * "right."
				 */
				WidgetID right = NO_WIDGET;
				/// Stores the directional flow information for this widget.
			} directionalFlow;

			/**
			 * The name of the spritesheet containing the sprite to display with
			 * this widget.
			 */
			std::string spritesheetKey;

			/**
			 * The name of the sprite to display with this widget.
			 */
			std::string spriteKey;

			/**
			 * Invoked when this widget emits specific signals.
			 * Each signal that is configured to have a handler, will have an entry
			 * in this map.
			 */
			std::unordered_map<std::string,
				engine::CScriptWrapper<asIScriptFunction>> singleSignalHandlers;

			/**
			 * Invoked when this widget emits any signal.
			 * The function will be given the widget's ID, as well as the name of
			 * the signal that was emitted.
			 */
			std::unique_ptr<engine::CScriptWrapper<asIScriptFunction>>
				multiSignalHandler;

			/**
			 * The picture widgets whose size should NOT always match with their
			 * sprite size.
			 */
			bool doNotOverridePictureSizeWithSpriteSize = false;

			/**
			 * The hierarchy of the last added menu or menu item, if this widget is
			 * a \c MenuBar.
			 * @warning Unfortunately we can't easily let the scripts add menu
			 *          items outside of \c SetUp() functions, since menu items are
			 *          translated in \c animate().
			 */
			std::vector<tgui::String> hierarchyOfLastMenuItem;

			/**
			 * The number of menus and menu items in this \c MenuBar.
			 */
			MenuItemID menuCounter = 0;

			/**
			 * The ID of the last menu item that was selected from this \c MenuBar.
			 */
			MenuItemID lastMenuItemClicked = NO_MENU_ITEM_ID;

			/**
			 * The ID of the last button pressed from this \c MessageBox.
			 */
			std::size_t lastMessageBoxButtonClicked = 0;

			/**
			 * \c ChildWindow property cache used to handle minimise and maximise
			 * logic.
			 * If this optional has a value, it means that the engine should \b not
			 * automatically handle minimise and maximise logic.
			 */
			std::optional<child_window_properties> childWindowData;

			/**
			 * Invoked when this \c ChildWindow is emitting the \c Closing signal
			 * (which is emitted just before the \c Closed signal, and is used to
			 * determine if the \c ChildWindow should actually close).
			 * The function will be given the widget's ID, as well as a \c bool&out
			 * parameter. If this parameter is set to \c FALSE, the \c ChildWindow
			 * will not close.
			 */
			std::unique_ptr<engine::CScriptWrapper<asIScriptFunction>>
				childWindowClosingHandler;

			/**
			 * A list of minimised \c ChildWindows that are in this container
			 * widget.
			 */
			minimised_child_window_list minimisedChildWindowList;

			/**
			 * Dump this widget's data to a string.
			 * @return The widget's data in string form.
			 */
			operator std::string() const;
		};

		/**
		 * The container in which all of the widgets are stored.
		 */
		typedef std::vector<widget_data> WidgetCollection;

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
		 * @return The ID of the widget that is selected after the call is
		 *         finished.
		 */
		WidgetID _moveDirectionalFlow(const std::shared_ptr<sfx::user_input>& ui);

		/**
		 * Update the current directional flow selection.
		 * @param newsel The ID of the widget to select.
		 * @param menu   The name of the menu to update.
		 */
		void _makeNewDirectionalSelection(const WidgetIDRef newsel,
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
			const widget_data& widgetData,
			const std::vector<tgui::MenuBar::GetMenusElement>& items,
			std::vector<tgui::String>& hierarchy, std::size_t& index);

		/**
		 * The JSON load method for this class.
		 * There should be a key called "reserve", with an unsigned integer for a
		 * value. This number should be set to the maximum number of widgets that
		 * will be active at one time across all menus. It shouldn't be exact,
		 * rather a rough estimate.\n
		 * For the GUI engine to know which menu to open first, the JSON script
		 * needs to store the class name of the \c Menu subclass to open as a
		 * string. The key for this value is "main".\n
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
		 * The game control signals will be resaved. Additionally, the "reserve"
		 * value shall be increased if the \c _widgets container has had to be
		 * reallocated during execution.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Connects all signals of a widget to this class' \c signalHandler()
		 * method.
		 * @param  widget The widget whose signals are to be connected.
		 * @safety No guarantee.
		 */
		void _connectSignals(tgui::Widget::Ptr widget);

		/**
		 * Deletes a widget from the GUI.
		 * The following happens when a widget is deleted:
		 * <ol><li>If the widget is a \c TabContainer, all of its \c Panel widgets
		 *     need to be manually deleted, as these are managed by the engine and
		 *     not the scripts.</li>
		 *     <li>If the widget is a \c ChildWindow, remove it from its parent's
		 *     \c ChildWindowList, if it has a parent.</li>
		 *     <li>Remove the widget's ID from every other widget's and every
		 *     menu's directional flow data.</li>
		 *     <li>Remove the widget's sprite, if it has one.</li>
		 *     <li>Clear the widget's data, and free up the ID for later use.</li>
		 *     <li>Actually remove the widget itself from its parent, if it has
		 *     one.</li></ol>
		 * This method will \em not delete widgets recursively. This decision was
		 * made because resource management with the object-oriented interface
		 * would be a pain otherwise. When a container is deleted, you do not
		 * necessarily want to delete all of its widgets, too.
		 * @param   widget ID of the widget to delete.
		 * @safety  No guarantee.
		 */
		void _deleteWidget(const WidgetIDRef widget);

		/**
		 * Adds a translatable caption to a widget, or updates an existing one.
		 * Used for widgets that have only a single caption.
		 * @param  data      The data of the widget to assign the caption to.
		 * @param  text      The translation key of the string to use.
		 * @param  variables Optional list of variables to later insert into the
		 *                   caption when translating.
		 * @safety No guarantee.
		 */
		void _setTranslatedString(widget_data& data, const std::string& text,
			CScriptArray* variables);

		/**
		 * Adds a translatable caption to a widget, or updates an existing one.
		 * Used for widgets that have a list of captions that are most easily
		 * identified via a numeric index, e.g. ListBoxes.\n
		 * Note that if an out of range index is given, the internal lists will
		 * grow in order to accomodate for the new entry.
		 * @param  data      The data of the widget to assign the caption to.
		 * @param  text      The translation key of the string to use.
		 * @param  variables Optional list of variables to later insert into the
		 *                   caption when translating.
		 * @param  index     Which caption to set.
		 * @safety No guarantee.
		 */
		void _setTranslatedString(widget_data& data, const std::string& text,
			CScriptArray* variables, const std::size_t index);

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
		 * @param  data The data of the widget whose caption needs to be
		 *              translated.
		 * @return The translated string with any configured variables inserted.
		 * @safety No guarantee.
		 */
		std::string _getTranslatedText(const widget_data& data) const;

		/**
		 * Computes the translated widget text for a given widget.
		 * Used for widgets with a list of captions, such as a ListBox.
		 * @param  data The data of the widget whose caption needs to be
		 *              translated.
		 * @param  index    The specific caption to translate.
		 * @return The translated string with any configured variables inserted.
		 * @safety No guarantee.
		 */
		std::string _getTranslatedText(const widget_data& data,
			const std::size_t index) const;

		/**
		 * Applies a new sprite to a widget.
		 * @param widget Pointer to the data of the widget to apply the sprite to.
		 * @param sheet  Name of the sheet containing the sprite to apply.
		 * @param key    Key of the sprite to apply.
		 */
		void _applySprite(widget_data& widget,
			const std::string& sheet, const std::string& key);

		/**
		 * Creates a widget of a given type and returns it.
		 * Logs an error if the given type isn't supported. In this case, no widget
		 * will be created.
		 * @param  wType The type of widget to create. Must be all lower case.
		 * @return Pointer to the new widget, or \c nullptr if it couldn't be
		 *         created.
		 */
		tgui::Widget::Ptr _widgetFactory(const std::string& wType) const;

		/**
		 * Stores a given widget in the \c _widgets container.
		 * @warning You must assume that this method will invalidate any iterators,
		 *          pointers, and references to widget data!
		 * @param   w Pointer to the widget to store. Cannot be \c nullptr.
		 * @return  The ID of the widget.
		 */
		WidgetID _storeWidget(const tgui::Widget::Ptr& w);

		/**
		 * Stores a given widget in the \c _widgets container and configures its
		 * signal handlers.
		 * @warning You must assume that this method will invalidate any iterators,
		 *          pointers, and references to widget data!
		 * @param   w Pointer to the widget to store. Cannot be \c nullptr.
		 * @return  The ID of the widget.
		 */
		WidgetID _storeWidgetAndConnectSignals(const tgui::Widget::Ptr& w);

		/**
		 * Creates a widget of a given type, stores it, and connects the signal
		 * handler/s.
		 * @warning You must assume that this method will invalidate any iterators,
		 *          pointers, and references to widget data!
		 * @param   wType         The type of widget to create.
		 * @return  The ID of the new widget. \c sfx::NO_WIDGET if the widget could
		 *          not be created.
		 */
		WidgetID _createWidget(const std::string& wType);

		/**
		 * Add a child window to a parent.
		 * @param parent Reference to the parent widget's data.
		 * @param child  Reference to the child widget's data.
		 */
		void _addWidgetToParent(widget_data& parent, const widget_data& child);

		/**
		 * Add a child window to a grid.
		 * @param parent Reference to the grid widget's data.
		 * @param child  Reference to the child widget's data.
		 * @param row    The row to add the widget to.
		 * @param col    The column to add the widget to.
		 */
		void _addWidgetToGrid(widget_data& parent, const widget_data& child,
			const std::size_t row, const std::size_t col);

		/**
		 * Remove a child window from its parent.
		 * @param parent Reference to the parent widget's data.
		 * @param child  Reference to the child widget's data.
		 */
		void _removeWidgetFromParent(widget_data& parent,
			const widget_data& child);

		/**
		 * Finds a widget in the internal widget container.
		 * @param  id The ID of the widget to find.
		 * @return An iterator to the widget's data (which includes its pointer).
		 *         <tt>_widgets.end()</tt> if the ID could not identify a widget.
		 */
		inline WidgetCollection::iterator _findWidget(const WidgetIDRef id) {
			return _widgetExists(id) ? _widgets.begin() + id : _widgets.end();
		}

		/**
		 * Finds a widget in the internal widget container.
		 * @param  id The ID of the widget to find.
		 * @return A const iterator to the widget's data (which includes its
		 *         pointer). <tt>_widgets.cend()</tt> if the ID could not identify
		 *         a widget.
		 */
		inline WidgetCollection::const_iterator _findWidget(
			const WidgetIDRef id) const {
			return _widgetExists(id) ? _widgets.cbegin() + id : _widgets.cend();
		}

		/**
		 * Finds the next available widget ID.
		 * If the final widget ID has been found, then log a warning and allocate
		 * x1.5 more cells.
		 * @warning In the case that a reallocation does occur, any existing
		 *          iterators (as well as pointers or references) to widget data
		 *          will be invalidated!
		 * @return  A widget ID that is not currently in use.
		 */
		WidgetID _findNextWidgetID();

		/**
		 * Retrieve a widget's ID.
		 * @param  w Pointer to the widget.
		 * @return The ID of the widget. In cases where a widget is a TGUI
		 *         placeholder, \c sfx::NO_WIDGET will be returned.
		 */
		static inline WidgetID _getWidgetID(const tgui::Widget::Ptr& w) {
			try {
				return w->getUserData<WidgetID>();
			} catch (const std::bad_cast&) {
				return NO_WIDGET;
			}
		}

		/**
		 * Retrieve a widget's ID.
		 * @sa \c _getWidgetID().
		 */
		static inline WidgetID _getWidgetID(const tgui::Widget* const w) {
			try {
				return w->getUserData<WidgetID>();
			} catch (const std::bad_cast&) {
				return NO_WIDGET;
			}
		}

		/**
		 * Checks if a given widget is visible and/or enabled, and that the same
		 * can be said for all of its parents.
		 * @warning An assertion is made that at least one of the boolean
		 *          parameters is \c TRUE!
		 * @warning Note that if a widget is not attached to the root widget in
		 *          some way then it will never be visible to the user!
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
		 *         found), \c sfx::NO_WIDGET and \c nullptr. If there is a widget
		 *         currently selected, and it can be found, then its ID and widget
		 *         object shall be returned.
		 */
		std::pair<WidgetID, tgui::Widget::Ptr> _findCurrentlySelectedWidget();

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
		 * @param widgetCount The number of widget cells to reserve.
		 */
		void _clearState(const std::size_t widgetCount);

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

		/**
		 * Registers the \c Menu interface.
		 */
		void _registerMenuInterface(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		// NON-WIDGET GLOBAL FUNCTIONS //

		/**
		 * Register non-widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerNonWidgetGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setGUI(const std::string&);
		bool _menuExists(const std::string&);
		asIScriptObject* _getMenu(const std::string&);
		void _dumpWidgetsToString(std::string&, const sfx::gui::widget_data&,
			const std::size_t = 0) const;
		void _dumpWidgetsToLog() const;
		std::size_t _getHeightOfTallestFrame(const std::string&) const;

		// WIDGETS //

		/**
		 * Registers generic widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerWidgetGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		bool _widgetExists(const WidgetIDRef) const;
		WidgetID _getWidgetFocused(const WidgetIDRef = NO_WIDGET) const;
		WidgetID _createWidgetScriptInterface(const std::string&);
		void _connectSignal(const WidgetIDRef, const std::string&,
			asIScriptFunction* const);
		void _connectSignal(const WidgetIDRef, asIScriptFunction* const);
		void _disconnectSignals(const CScriptArray* const);
		WidgetID _getParent(const WidgetIDRef);
		void sfx::gui::_deleteWidgetScriptInterface(const sfx::WidgetIDRef id);
		void _setWidgetName(const WidgetIDRef, const std::string&);
		std::string _getWidgetName(const WidgetIDRef) const;
		void _setWidgetFocus(const WidgetIDRef);
		void _setWidgetFont(const WidgetIDRef, const std::string&);
		void _setWidgetInheritedFont(const WidgetIDRef, const std::string&);
		void _setWidgetPosition(const WidgetIDRef, const std::string&,
			const std::string&);
		sf::Vector2f _getWidgetAbsolutePosition(const WidgetIDRef);
		void _setWidgetOrigin(const WidgetIDRef, const float, const float);
		void _setWidgetSize(const WidgetIDRef, const std::string&,
			const std::string&);
		sf::Vector2f _getWidgetFullSize(const WidgetIDRef);
		void _setWidgetScale(const WidgetIDRef, const float, const float);
		void _setWidgetEnabled(const WidgetIDRef, const bool);
		bool _getWidgetEnabled(const WidgetIDRef) const;
		void _setWidgetVisibility(const WidgetIDRef, const bool);
		bool _getWidgetVisibility(const WidgetIDRef) const;
		void _moveWidgetToFront(const WidgetIDRef);
		void _moveWidgetToBack(const WidgetIDRef);
		void _setWidgetText(const WidgetIDRef, const std::string&,
			CScriptArray* const);
		void _setWidgetTextSize(const WidgetIDRef, const unsigned int);
		void _setWidgetIndex(const WidgetIDRef, const std::size_t);
		void _setWidgetAutoLayout(const WidgetIDRef, const tgui::AutoLayout);

		// DIRECTIONAL FLOW //

		/**
		 * Registers directional flow global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerDirectionalFlowGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetDirectionalFlow(const WidgetIDRef, const WidgetIDRef,
			const WidgetIDRef, const WidgetIDRef, const WidgetIDRef);
		void _setWidgetDirectionalFlowStart(const std::string&, const WidgetIDRef);
		void _setWidgetDirectionalFlowSelection(const std::string&,
			const WidgetIDRef);
		void _setDirectionalFlowAngleBracketSprite(const std::string&,
			const std::string&, const std::string&);

		// SPRITES //

		/**
		 * Registers sprite-related global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerSpriteGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetSprite(const WidgetIDRef, const std::string&,
			const std::string&);
		void _matchWidgetSizeToSprite(const WidgetIDRef, const bool);

		// LABEL //

		/**
		 * Registers global functions that act on labels within widgets.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerLabelGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetTextStyles(const WidgetIDRef, const std::string&);
		void _setWidgetTextMaximumWidth(const WidgetIDRef, const float);
		void _setWidgetTextColour(const WidgetIDRef, const sf::Color&);
		void _setWidgetTextOutlineColour(const WidgetIDRef, const sf::Color&);
		void _setWidgetTextOutlineThickness(const WidgetIDRef, const float);
		void _setWidgetTextAlignment(const WidgetIDRef,
			const tgui::Label::HorizontalAlignment,
			const tgui::Label::VerticalAlignment);

		// EDITBOX AND TEXTAREA //

		/**
		 * Registers \c EditBox and \c TextArea global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerEditBoxGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);
		
		std::string _getWidgetText(const WidgetIDRef) const;
		void _setEditBoxRegexValidator(const WidgetIDRef, const std::string&);
		void _setWidgetDefaultText(const WidgetIDRef, const std::string& text,
			CScriptArray* const variables);
		bool _editBoxOrTextAreaHasFocus() const;
		void _optimiseTextAreaForMonospaceFont(const WidgetIDRef, const bool);
		void _getCaretLineAndColumn(const WidgetIDRef, std::size_t&,
			std::size_t&) const;
		void _setEditBoxTextAlignment(const WidgetIDRef,
			const tgui::EditBox::Alignment);

		// BUTTON //

		/**
		 * Registers \c ButtonBase global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerButtonGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetDisabledBgColour(const WidgetIDRef, const sf::Color&);

		// RADIOBUTTON & CHECKBOX //

		/**
		 * Registers \c RadioButton and \c CheckBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerRadioButtonAndCheckBoxGlobalFunctions(
			asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetChecked(const WidgetIDRef, const bool);
		bool _isWidgetChecked(const WidgetIDRef) const;

		// LIST //

		/**
		 * Registers \c ListBox and \c ComboBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerListGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addItem(const WidgetIDRef, const std::string&, CScriptArray* const);
		void _clearItems(const WidgetIDRef);
		void _setSelectedItem(const WidgetIDRef, const std::size_t);
		void _deselectItem(const WidgetIDRef);
		std::size_t _getItemCount(const WidgetIDRef);
		int _getSelectedItem(const WidgetIDRef) const;
		std::string _getSelectedItemText(const WidgetIDRef) const;
		void _setItemsToDisplay(const WidgetIDRef, const std::size_t);

		// TREEVIEW //

		/**
		 * Registers \c TreeView global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTreeViewGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setSelectedItemTextHierarchy(const WidgetIDRef,
			const CScriptArray* const);
		CScriptArray* _getSelectedItemTextHierarchy(const WidgetIDRef) const;
		void _addTreeViewItem(const WidgetIDRef, const CScriptArray* const);

		// TABS //

		/**
		 * Registers \c Tabs global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTabsGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addTab(const WidgetIDRef, const std::string&, CScriptArray* const);
		void _setSelectedTab(const WidgetIDRef, const std::size_t);
		void _deselectTab(const WidgetIDRef);
		int _getSelectedTab(const WidgetIDRef) const;
		void _setTabEnabled(const WidgetIDRef, const std::size_t, const bool);
		bool _getTabEnabled(const WidgetIDRef, const std::size_t);
		std::size_t _getTabCount(const WidgetIDRef) const;
		std::string _getTabText(const WidgetIDRef, const std::size_t) const;

		// CONTAINER //

		/**
		 * Registers \c Container widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerContainerGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _add(const WidgetIDRef, const WidgetIDRef);
		void _remove(const WidgetIDRef);
		void _removeAll(const WidgetIDRef);
		void _setWidgetIndexInContainer(const WidgetIDRef, const std::size_t,
			const std::size_t);
		std::size_t _getWidgetCount(const WidgetIDRef) const;
		void _setGroupPadding(const WidgetIDRef, const std::string&);
		void _setGroupPadding(const WidgetIDRef, const std::string&,
			const std::string&, const std::string&, const std::string&);
		void _applySpritesToWidgetsInContainer(const WidgetIDRef,
			const std::string&, const CScriptArray* const);

		// PANEL //

		/**
		 * Registers \c Panel and \c ScrollablePanel widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerPanelGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetBgColour(const WidgetIDRef, const sf::Color&);
		void _setWidgetBorderSize(const WidgetIDRef, const float);
		void _setWidgetBorderSizes(const WidgetIDRef, const std::string&,
			const std::string&, const std::string&, const std::string&);
		void _setWidgetBorderColour(const WidgetIDRef, const sf::Color&);
		void _setWidgetBorderRadius(const WidgetIDRef, const float);
		void _setHorizontalScrollbarPolicy(const WidgetIDRef,
			const tgui::Scrollbar::Policy);
		void _setHorizontalScrollbarAmount(const WidgetIDRef, const unsigned int);
		bool _isHorizontalScrollbarVisible(const WidgetIDRef) const;
		void _setVerticalScrollbarAmount(const WidgetIDRef, const unsigned int);
		void _setVerticalScrollbarValue(const WidgetIDRef, const unsigned int);
		bool _isVerticalScrollbarVisible(const WidgetIDRef) const;
		float _getScrollbarWidth(const WidgetIDRef) const;

		// LAYOUT //

		/**
		 * Registers \c BoxLayout widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerLayoutGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetRatioInLayout(const WidgetIDRef, const std::size_t,
			const float);
		void _setSpaceBetweenWidgets(const WidgetIDRef, const float);

		// GRID //

		/**
		 * Registers \c Grid widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerGridGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _addWidgetToGrid(const WidgetIDRef, const WidgetIDRef,
			const std::size_t, const std::size_t);
		void _setWidgetAlignmentInGrid(const WidgetIDRef, const std::size_t,
			const std::size_t, const tgui::Grid::Alignment);
		void _setWidgetPaddingInGrid(const WidgetIDRef, const std::size_t,
			const std::size_t, const std::string&);
		std::size_t _getWidgetColumnCount(const WidgetIDRef) const;
		std::size_t _getWidgetRowCount(const WidgetIDRef) const;

		// MENUS //

		/**
		 * Registers \c MenuBar widget global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerMenuBarGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		MenuItemID _addMenu(const WidgetIDRef, const std::string&,
			CScriptArray* const);
		MenuItemID _addMenuItem(const WidgetIDRef, const std::string&,
			CScriptArray* const);
		MenuItemID _addMenuItemIntoLastItem(const WidgetIDRef,
			const std::string&, CScriptArray* const);
		void _exitSubmenu(const WidgetIDRef);
		MenuItemID _getLastSelectedMenuItem(const WidgetIDRef) const;

		// CHILDWINDOW //

		/**
		 * Registers \c ChildWindow global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerChildWindowGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _autoHandleMinMax(const WidgetIDRef, const bool);
		void _setChildWindowTitleButtons(const WidgetIDRef, const unsigned int);
		void _setWidgetResizable(const WidgetIDRef, const bool);
		void _setWidgetPositionLocked(const WidgetIDRef, const bool);
		float _getTitleBarHeight(const WidgetIDRef) const;
		CScriptArray* _getBorderWidths(const WidgetIDRef) const;
		void _openChildWindow(const WidgetIDRef, const std::string&,
			const std::string&);
		void _closeChildWindow(const WidgetIDRef);
		void _closeChildWindowAndEmitSignal(const WidgetIDRef);
		void _restoreChildWindow(const WidgetIDRef);
		void _restoreChildWindowImpl(const WidgetIDRef, widget_data&);
		void _maximiseChildWindow(const WidgetIDRef);
		void _maximiseChildWindowImpl(const tgui::ChildWindow::Ptr&, widget_data&);
		bool _isChildWindowOpen(const WidgetIDRef) const;
		void _connectChildWindowClosingSignalHandler(const WidgetIDRef,
			asIScriptFunction* const);

		// FILEDIALOG //

		/**
		 * Registers \c FileDialog global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerFileDialogGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setFileDialogStrings(const WidgetIDRef,
			const std::string& title, CScriptArray* const v0,
			const std::string& confirm, CScriptArray* const v1,
			const std::string& cancel, CScriptArray* const v2,
			const std::string& createFolder, CScriptArray* const v3,
			const std::string& filenameLabel, CScriptArray* const v4,
			const std::string& nameColumn, CScriptArray* const v5,
			const std::string& sizeColumn, CScriptArray* const v6,
			const std::string& modifyColumn, CScriptArray* const v7,
			const std::string& allFiles, CScriptArray* const v8);
		CScriptArray* _getFileDialogSelectedPaths(const WidgetIDRef) const;
		void _addFileDialogFileTypeFilter(const WidgetIDRef, const std::string&,
			CScriptArray *const, CScriptArray *const);
		void _clearFileDialogFileTypeFilters(const WidgetIDRef);
		void _setFileDialogFileMustExist(const WidgetIDRef, const bool);
		void _setFileDialogDefaultFileFilter(const WidgetIDRef, const std::size_t);
		void _setFileDialogPath(const WidgetIDRef, const std::string&);

		// MESSAGEBOX //

		/**
		 * Registers \c MessageBox global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerMessageBoxGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setMessageBoxStrings(const WidgetIDRef, const std::string&,
			CScriptArray* const, const std::string&, CScriptArray* const);
		void _addMessageBoxButton(const WidgetIDRef, const std::string&,
			CScriptArray* const);
		std::size_t _getLastSelectedButton(const WidgetIDRef) const;

		// TABCONTAINER //

		/**
		 * Registers \c TabContainer global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerTabContainerGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		WidgetID _addTabAndPanel(const WidgetIDRef,
			const std::string&, CScriptArray* const = nullptr);
		bool _removeTabAndPanel(const WidgetIDRef);

		// SPINCONTROL & SLIDER //

		/**
		 * Registers \c SpinControl and \c Slider global functions.
		 * See implementation for documentation on all of the methods used to
		 * implement these functions.
		 */
		void _registerSpinControlGlobalFunctions(asIScriptEngine* const engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		void _setWidgetMinMaxValues(const WidgetIDRef, const float, const float);
		bool _setWidgetValue(const WidgetIDRef, float val);
		float _getWidgetValue(const WidgetIDRef) const;

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
		 * The last known language set in \c _langdict.
		 */
		std::string _lastlang;

		/**
		 * Stores the name of the GUI currently being displayed.
		 */
		std::string _currentGUI;

		/**
		 * Stores the name of the GUI that was set before the current one.
		 */
		std::string _previousGUI;

		/**
		 * Pointer to the scripts object containing the signal handler functions.
		 */
		std::shared_ptr<engine::scripts> _scripts;

		/**
		 * Cache of the user input object last given to \c handleInput().
		 */
		std::shared_ptr<sfx::user_input> _ui;

		/**
		 * Pointer to the language dictionary used to translate all captions.
		 */
		std::shared_ptr<engine::language_dictionary> _langdict;

		/**
		 * Pointer to the fonts used with this GUI.
		 * Potential optimisation: if the same font is given for a widget more than
		 * once, then pull it from an internal list rather than constructing it
		 * again.
		 */
		std::shared_ptr<sfx::fonts> _fonts;

		/**
		 * Pointer to the animated spritesheets that can be used with the GUI
		 * menus.
		 */
		std::shared_ptr<sfx::animated_spritesheets> _sheets;

		/**
		 * Stores data associated with a menu.
		 */
		struct menu_data {
			/**
			 * Releases the \c Menu object.
			 */
			~menu_data() noexcept;

			/**
			 * Pointer to the script object containing the code to execute when
			 * switching between menus, and the code to execute periodically.
			 */
			asIScriptObject* object = nullptr;

			/**
			 * Stores the ID of the widget to select first when a directional input
			 * is made in this menu, and there currently isn't any widget selected.
			 */
			WidgetID selectThisWidgetFirst = NO_WIDGET;

			/**
			 * The ID of the widget previously selected using directional input.
			 */
			WidgetID previouslySelectedWidget = NO_WIDGET;

			/**
			 * The ID of the widget currently selected using directional input.
			 */
			WidgetID currentlySelectedWidget = NO_WIDGET;
		};

		/**
		 * Stores information on each menu.
		 */
		std::unordered_map<std::string, menu_data> _menus;

		/**
		 * Stores information on all of the widgets that currently exist.
		 */
		WidgetCollection _widgets;

		/**
		 * The \c WidgetID 'generator.'
		 */
		WidgetID _widgetCounter = ROOT_WIDGET;

		/**
		 * Widgets that have been removed will have their IDs put into this
		 * container.
		 */
		std::unordered_set<WidgetID> _availableCells;

		/**
		 * Stores a list of animated sprites associated with each animated widget.
		 * Cleared upon a call to \c setGUI(). Refilled once upon the first call to
		 * \c animate() since \c setGUI() was last called.
		 */
		std::unordered_map<tgui::Widget::ConstPtr, sfx::animated_sprite>
			_widgetSprites;

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
		 * The current mouse position.
		 */
		sf::Vector2i _currentMousePosition;

		/**
		 * The mouse position the last time \c handleInput() was called.
		 */
		sf::Vector2i _previousMousePosition;

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
		 * Ensures that handleInput() errors are only written to the log once at a
		 * time.
		 */
		bool _handleInputErrorLogged = false;

		/**
		 * If the mouse has moved, disable directional control influence until a
		 * new directional control has been input.
		 */
		bool _enableDirectionalFlow = true;

		/**
		 * Is set to \c TRUE whilst \c _load() is running.
		 * Will always be set to \c FALSE when not in \c _load(), even if an
		 * exception is thrown from within \c _load().
		 */
		bool _isLoading = false;

		/**
		 * Does an \c EditBox or a \c TextArea have set focus right now?
		 */
		bool _editBoxOrTextAreaHasSetFocus = false;
	};
}
