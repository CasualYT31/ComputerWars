/**
 * @file Helper.as
 * Contains helper functions.
 */

namespace awe {
    /**
     * Empty callback signature.
     */
    funcdef void EmptyCallback();
    
	/**
	 * Writes a dictionary to the logs, assuming all of its values are strings.
	 * @param dict Handle to the dictionary to log.
	 */
	void logDictionary(const dictionary@ dict) {
		info("Dictionary size: " + formatUInt(dict.getSize()));
		const auto keys = dict.getKeys();
		for (uint i = 0, len = keys.length(); i < len; ++i) {
			string value;
			dict.get(keys[i], value);
			info(keys[i] + " = " + value);
		}
	}

    /**
     * Formats a \c MenuItemID.
     * @param id The menu item ID to format.
     */
    string formatMenuItemID(const MenuItemID id) {
        return formatUInt(id);
    }

    /**
     * Stores information on a button to add to a parent.
     */
    class ParentButton {
        /**
         * Constructs a \c ParentButton.
         * @param w Assigned to \c widgetName.
         * @param c Assigned to \c caption.
         */
        ParentButton(const string&in w, const string&in c) {
            widgetName = w;
            caption = c;
        }

        /**
         * The short name of the button.
         */
        string widgetName;

        /**
         * The caption to assign to the button.
         */
        string caption;
    }

    /**
     * Adds a list of buttons to the bottom right corner of the given parent.
     * @param parent  The name of the widget to add the buttons to.
     * @param buttons An array of button information.
     * @param width   The width of each button.
     * @param height  The height of each button.
     */
    void addButtonsToParent(const string&in parent,
        const array<ParentButton@>@ buttons, const uint width,
        const uint height) {
        const uint PADDING = 5;
        const string PADDING_S = formatUInt(PADDING),
            WIDTH = formatUInt(width), HEIGHT = formatUInt(height);
        uint x = PADDING;
        for (uint i = 0, len = buttons.length(); i < len; ++i) {
            const auto btn = parent + "." + buttons[i].widgetName;
            addWidget("Button", btn);
            setWidgetText(btn, buttons[i].caption);
            setWidgetOrigin(btn, 1.0f, 1.0f);
            setWidgetPosition(btn, "100%-" + formatUInt(x), "100%-" + PADDING_S);
            setWidgetSize(btn, WIDTH, HEIGHT);
            x += width + PADDING;
        }
    }

    /**
     * Opens a \c FileDialog.
     * @param name      The widget name of the \c FileDialog.
     * @param title     The title of the \c FileDialog.
     * @param confirm   The text to display on the "confirm" button.
     * @param path      The path to open the dialog in. If left blank, the default
     *                  path will be used (Current Working Directory).
     * @param mustExist \c TRUE if the selected path/s must exist, \c FALSE if
     *                  they are permitted to not exist.
     */
    void OpenFileDialog(const string&in name, const string&in title,
        const string&in confirm, const string&in path = "",
        const bool mustExist = true) {
        addWidget("FileDialog", name);
        setFileDialogStrings(name, title, null,
            confirm, null,
            "cancel", null,
            "createfolder", null,
            "filename", null,
            "name", null,
            "size", null,
            "modified", null,
            "allfiles", null
        );
        setFileDialogFileMustExist(name, mustExist);
        if (!path.isEmpty()) setFileDialogPath(name, path);
    }

    namespace internal {
        /**
         * The name of the last opened message box.
         * May experience odd behaviour if full name is not stored and the menu is
         * changed.
         */
        string lastOpenedMessageBox;
    }

    /**
     * Opens a \c MessageBox.
     * \c disableThis and \c enableThis can be used with extra \c Group widgets to
     * disable an entire menu whilst a \c MessageBox is open in a second group.
     * E.g. you can put your entire menu into a base \c Group, which you can
     * disable, whilst putting your \c MessageBox in a separate base group that
     * you can then enable.
     * @param name        The widget name of the \c MessageBox.
     * @param title       The title of the \c MessageBox.
     * @param text        The text of the \c MessageBox.
     * @param vars        The variables to insert into \c text.
     * @param disableThis The name of a widget to disable. If empty, no widget
     *                    will be disabled.
     * @param enableThis  The name of a widget to enable. If empty, no widget will
     *                    be enabled. It will also be brought to the front.
     */
    void OpenMessageBox(const string&in name, const string&in title,
        const string&in text, array<any>@ vars, const string&in disableThis = "",
        const string&in enableThis = "") {
        internal::lastOpenedMessageBox = name;
        if (!disableThis.isEmpty()) setWidgetEnabled(disableThis, false);
        addWidget("MessageBox", name);
        ShowMessageBox();
        setMessageBoxStrings(name, title, null, text, vars);
        if (!enableThis.isEmpty()) {
            setWidgetEnabled(enableThis, true);
            moveWidgetToFront(enableThis);
        }
    }

    /**
     * Closes a \c MessageBox.
     * @param name        The widget name of the \c MessageBox.
     * @param disableThis The name of a widget to disable. If empty, no widget
     *                    will be disabled.
     * @param enableThis  The name of a widget to enable. If empty, no widget will
     *                    be enabled. It will also be brought to the front.
     */
    void CloseMessageBox(const string&in name, const string&in disableThis = "",
        const string&in enableThis = "") {
        internal::lastOpenedMessageBox = "";
        if (!disableThis.isEmpty()) setWidgetEnabled(disableThis, false);
        removeWidget(name);
        if (!enableThis.isEmpty()) {
            setWidgetEnabled(enableThis, true);
            moveWidgetToFront(enableThis);
        }
    }

    /**
     * Brings the last opened \c MessageBox back into view.
     * @warning This code assumes one \c MessageBox is open at a time!
     */
    void ShowMessageBox() {
        if (!internal::lastOpenedMessageBox.isEmpty()) {
            setWidgetOrigin(internal::lastOpenedMessageBox, 0.5f, 0.5f);
            setWidgetPosition(internal::lastOpenedMessageBox, "50%", "50%");
        }
    }
}
