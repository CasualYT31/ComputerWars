/**
 * @file Helper.as
 * Contains helper functions.
 */

namespace awe {
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
