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
     * Splits a string into an array of smaller strings, separated by the given
     * delimiter.
     * The delimiter will not be included in any of the substrings. If an empty
     * \c str is given, an empty array will be returned. If an empty \c delimiter
     * is given, an array with one element will be returned. The element's value
     * will be equal to \c str.
     * @remark This is documented to be a part of the string library in
     *         AngelScript, but it just doesn't appear to be registered at all...
     * @param  str       The string to split.
     * @param  delimiter The string sequence which, if found in \c str, will cause
     *                   a split.
     * @return An array of each split string. The order of the characters will be
     *         retained.
     */
    array<string>@ split(const string&in str, const string&in delimiter) {
        array<string>@ arr = array<string>();
        int lastPos = 0, delimiterPos = str.findFirst(delimiter);
        while (delimiterPos >= 0) {
            arr.insertLast(str.substr(lastPos, delimiterPos - lastPos));
            lastPos = delimiterPos + delimiter.length();
            delimiterPos = str.findFirst(delimiter, lastPos);
        }
        if (uint64(lastPos) < str.length()) arr.insertLast(str.substr(lastPos));
        return arr;
    }

    /**
     * Concatenate a list of strings into one big string, separated by the given
     * delimiter.
     * @remark This is documented to be a part of the string library in
     *         AngelScript, but it just doesn't appear to be registered at all...
     * @param  arr       The array of strings to join together.
     * @param  delimiter The extra string sequence to append to each string in
     *                   \c arr before joining (the original strings are not
     *                   changed).
     * @return The result. An empty string is \c arr is \c null.
     */
    string join(const array<string>@ const arr, const string&in delimiter) {
        if (arr is null) return "";
        string res;
        for (uint64 i = 0, len = arr.length(); i < len; ++i) {
            res += arr[i];
            if (i < len - 1) res += delimiter;
        }
        return res;
    }

    /**
     * Stores information on a button to add to a parent.
     */
    class ParentButton {
        /**
         * Constructs a \c ParentButton.
         * @param w Assigned to \c widgetName.
         * @param c Assigned to \c caption.
         * @param h Assigned to \c handler.
         */
        ParentButton(const string&in w, const string&in c,
            SignalHandler@ const h = null) {
            widgetName = w;
            caption = c;
            @handler = h;
        }

        /**
         * The short name of the button.
         */
        string widgetName;

        /**
         * The caption to assign to the button.
         */
        string caption;

        /**
         * The signal handler to connect to the button, if any.
         */
        SignalHandler@ handler;
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
            addWidget(Button, btn);
            setWidgetText(btn, buttons[i].caption);
            setWidgetOrigin(btn, 1.0f, 1.0f);
            setWidgetPosition(btn, "100%-" + formatUInt(x), "100%-" + PADDING_S);
            setWidgetSize(btn, WIDTH, HEIGHT);
            if (buttons[i].handler !is null)
                connectSignalHandler(btn, buttons[i].handler);
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
        addWidget(FileDialog, name);
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
        addWidget(MessageBox, name);
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

    /**
     * Adds the list of countries to a \c ComboBox or \c ListBox.
     * @param widget  The widget to amend.
     * @param neutral If \c TRUE, a "Neutral" option will be added first.
     * @param filter  A list of turn order IDs of countries to exclude from the
     *                list.
     */
    void addCountriesToList(const string&in widget, const bool neutral,
        const array<ArmyID>@ const filter = {}) {
        if (neutral) addItem(widget, "neutral");
        const auto@ countryScriptNames = country.scriptNames;
        for (uint c = 0, len = countryScriptNames.length(); c < len; ++c) {
            if (filter.find(ArmyID(c)) < 0)
                addItem(widget, country[countryScriptNames[c]].name);
        }
    }

    /**
     * Adds the list of COs to a \c ComboBox or \c ListBox.
     * @param widget The widget to amend.
     * @param noCO   If \c TRUE, a "No CO" option will be added first.
     */
    void addCOsToList(const string&in widget, const bool noCO) {
        if (noCO) addItem(widget, "noco");
        const auto@ coScriptNames = commander.scriptNames;
        for (uint c = 0, len = coScriptNames.length(); c < len; ++c)
            addItem(widget, commander[coScriptNames[c]].name);
    }
}
