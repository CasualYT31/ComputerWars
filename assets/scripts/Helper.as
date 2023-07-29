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
}
