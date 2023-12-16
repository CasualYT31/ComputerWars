/**
 * @file ScriptsWindow.as
 * Defines code that allows the user to edit a map's script files.
 */

/**
 * Represents the scripts window.
 */
class ScriptsWindow : Observer, ChildWindow {
    /**
     * Sets up the scripts child window.
     */
    ScriptsWindow() {
        // Setup the child window.
        setText("scripts");
        close(false);

        // Setup the menu bar.
        menuBar.setName("ScriptsMenuBar");
        menuBar.add("file");
        FILE_NEW = menuBar.addItem("new");
        FILE_RENAME = menuBar.addItem("rename");
        FILE_SAVE_MAP = menuBar.addItem("savemap");
        FILE_DELETE = menuBar.addItem("delete");
        menuBar.add("view");
        VIEW_SET_FONT_SIZE = menuBar.addItem("setfontsize");
        menuBar.add("code");
        CODE_BUILD = menuBar.addItem("build");
        CODE_LAST_RESULT = menuBar.addItem("lastresult");
        menuBar.setAutoLayout(AutoLayout::Top);
        menuBar.connect(MenuItemClicked,
            SingleSignalHandler(this.menuItemClicked));
        add(menuBar);

        // Setup the status bar.
        statusBar.setSize("", "ScriptsMenuBar.height");
        statusBar.setAutoLayout(AutoLayout::Bottom);
        add(statusBar);

        // Setup the tab container.
        scripts.setAutoLayout(AutoLayout::Fill);
        scripts.connect(SelectionChanged,
            SingleSignalHandler(this.tabContainerSelectionChanged));
        add(scripts);
    }

    /**
     * Restore the scripts window.
     */
    void open(const string&in x = "125", const string&in y = "50") override {
        ChildWindow::open(x, y);
        refresh();
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
    }

    /**
     * Refreshes the scripts window, if it's open.
     */
    void refresh(any&in data = any()) override {
        if (!isOpen()) return;

        // If there is data, this means a child text area's caret position has
        // updated, so update the status bar.
        Vector2 lineAndColumn;
        if (data.retrieve(lineAndColumn)) {
            statusBar.setCaretPosition(lineAndColumn.x, lineAndColumn.y);
            return;
        }

        const auto newScripts = edit.map.getScriptNames();

        // Every script that doesn't exist anymore is to be removed.
        // Go backwards to prevent index out of bounds errors.
        for (int i = int(scriptNameCache.length()) - 1; i >= 0; --i) {
            const auto scriptName = scriptNameCache[i];
            if (newScripts.find(scriptName) < 0) {
                // Delete script.
                scriptNameCache.removeAt(i);
                ScriptPanel@ panel;
                panels.get(scriptName, @panel);
                panel.cleanUp();
                panels.delete(scriptName);
            }
        }

        // Every new script must be added to this window.
        // And every script's content in this window must be updated.
        for (uint i = 0, len = newScripts.length(); i < len; ++i) {
            const auto scriptName = newScripts[i];
            ScriptPanel@ panel;
            if (scriptNameCache.find(scriptName) < 0) {
                // New script.
                scriptNameCache.insertLast(scriptName);
                @panel = ScriptPanel(this, scripts, scriptName);
                panels.set(scriptName, @panel);
                panel.refresh(any(fontSize));
                panel.refresh(any(edit.map.getScript(scriptName)));
                // Make sure to select the tab after the panels dictionary has
                // been updated, as the signal handler relies on there being a
                // valid entry for the panel.
                scripts.setSelectedTab(scripts.getTabCount() - 1);
            } else {
                // Existing script.
                panels.get(scriptName, @panel);
                panel.refresh(any(edit.map.getScript(scriptName)));
            }
        }
        
        // Update caret position and last known build result.
        tabContainerSelectionChanged();
        statusBar.setCompilerResult(edit.getLastKnownBuildResult());
    }

    /**
     * When a different file is selected in the tab container, update the caret
     * position in the status bar and set the focus to the text area.
     */
    private void tabContainerSelectionChanged() {
        // If there is no panel, there are no script files, so clear the caret
        // position.
        if (scripts.getTabCount() == 0) {
            statusBar.clearCaretPosition();
            return;
        }
        const string scriptName =
            scripts.getTabText(uint64(scripts.getSelectedTab()));
        ScriptPanel@ panel;
        panels.get(scriptName, @panel);
        const auto caretPosition = panel.getCaretPosition();
        statusBar.setCaretPosition(caretPosition.x, caretPosition.y);
        panel.setFocus();
    }

    /**
     * Validates the script name given to the name query window and enables or
     * disables the ok button as appropriate.
     */
    private void nameQueryWindowTextChanged() {
        const auto name = nameQueryWindow.getName();
        nameQueryWindow.enableOk(!name.isEmpty() &&
            scriptNameCache.find(name) < 0);
    }

    /**
     * The name for the new script file has been given, create it now.
     */
    private void nameQueryWindowCreateNewScript() {
        // Hold handle to nameQueryWindow temporarily.
        const auto temp = @nameQueryWindow;
        // Close the query window (making sure to emit the Closing signal).
        // The original nameQueryWindow handle has now been cleared.
        nameQueryWindow.close(true);
        // This will re-enable the rest of the map maker, meaning the refresh()
        // invoked by this EditableMap operation will now be able to set the focus
        // onto the TextArea.
        edit.addScriptFile(temp.getName(), "");
    }

    /**
     * The name for the existing script file has been given, set it now.
     */
    private void nameQueryWindowRenameScript() {
        // Hold handle to nameQueryWindow temporarily.
        const auto temp = @nameQueryWindow;
        // Close the query window (making sure to emit the Closing signal).
        // The original nameQueryWindow handle has now been cleared.
        nameQueryWindow.close(true);
        // This will re-enable the rest of the map maker, meaning the refresh()
        // invoked by this EditableMap operation will now be able to set the focus
        // onto the TextArea.
        edit.renameScriptFile(scripts.getTabText(scripts.getSelectedTab()),
            temp.getName());
    }

    /**
     * Clear the \c nameQueryWindow handle to allow the GC to delete the object
     * ASAP.
     */
    private void nameQueryWindowClosed() {
        @nameQueryWindow = null;
    }

    /**
     * The user has responded to the delete script confirmation message box.
     */
    private void deleteScriptMessageBox(const uint64 btn) {
        if (btn == 0) {
            // Yes, delete.
            edit.removeScriptFile(scripts.getTabText(scripts.getSelectedTab()));
        } else {
            // No, cancel deletion. Place the focus back onto the text area.
            tabContainerSelectionChanged();
        }
    }

    /**
     * When the user requests a new font size, cache it and apply it to all
     * existing \c TextArea widgets.
     */
    private void fontSizeQueryWindowValueChanged() {
        fontSize = fontSizeQueryWindow.getValue();
        const auto keys = panels.getKeys();
        for (uint64 i = 0, len = keys.length(); i < len; ++i) {
            ScriptPanel@ panel;
            panels.get(keys[i], @panel);
            panel.refresh(any(fontSize));
        }
    }

    /**
     * Clear the \c fontSizeQueryWindow handle to allow the GC to delete the
     * object ASAP.
     */
    private void fontSizeQueryWindowClosed() {
        @fontSizeQueryWindow = null;
    }

    /**
     * Shows the last build result to the user in a message box.
     */
    private void showLastBuildResult() {
        string result = "~" + edit.getLastKnownBuildResult();
        if (result.findFirst(")") >= 0) {
            // Insert some newlines so that the message sits more comfortably in
            // the MessageBox.
            result = awe::join(awe::split(result, "), "), "),\n") + ")";
        }
        mapMaker.openMessageBox(null, { "ok" }, result);
    }

    /**
     * Cache of the current map's script names.
     * There is no guaranteed order to this list.
     */
    private array<string> scriptNameCache;

    /**
     * The size of the font in all of the text areas.
     */
    private uint fontSize = 13;

    /**
     * The menu bar.
     */
    private MenuBar menuBar;

    /**
     * The tab container which contains each script.
     */
    private TabContainer scripts;

    /**
     * The panels currently stored in the tab container.
     * \c ScriptPanel@ values keyed on script names.
     */
    private dictionary panels;

    /**
     * The status bar.
     */
    private ScriptsStatusBar statusBar;

    /**
     * Points to the script name query window, if it is open.
     * If the X button is clicked, the window is closed, but it is still
     * technically alive via this handle. When a new window is created, the old
     * handle will be replaced, though.
     */
    private ScriptNameQueryWindow@ nameQueryWindow;

    /**
     * Points to the font size query window, if it is open.
     * If the X button is clicked, the window is closed, but it is still
     * technically alive via this handle. When a new window is created, the old
     * handle will be replaced, though.
     */
    private ScriptFontSizeQueryWindow@ fontSizeQueryWindow;

    private MenuItemID FILE_NEW;
    private MenuItemID FILE_RENAME;
    private MenuItemID FILE_SAVE_MAP;
    private MenuItemID FILE_DELETE;
    private MenuItemID VIEW_SET_FONT_SIZE;
    private MenuItemID CODE_BUILD;
    private MenuItemID CODE_LAST_RESULT;

    /**
     * When a menu item is clicked, run this handler.
     */
    private void menuItemClicked() {
        const auto i = menuBar.getSelectedItem();
        if (i == FILE_NEW) {
            @nameQueryWindow = ScriptNameQueryWindow(
                SingleSignalHandler(this.nameQueryWindowTextChanged),
                SingleSignalHandler(this.nameQueryWindowCreateNewScript)
            );
            mapMaker.openQueryWindow(nameQueryWindow,
                QueryWindowCallback(this.nameQueryWindowClosed));
            nameQueryWindow.setFocus();
            
        } else if (i == FILE_RENAME) {
            @nameQueryWindow = ScriptNameQueryWindow(
                SingleSignalHandler(this.nameQueryWindowTextChanged),
                SingleSignalHandler(this.nameQueryWindowRenameScript)
            );
            mapMaker.openQueryWindow(nameQueryWindow,
                QueryWindowCallback(this.nameQueryWindowClosed));
            nameQueryWindow.setFocus();
            
        } else if (i == FILE_SAVE_MAP) {
            edit.map.save();
            
        } else if (i == FILE_DELETE) {
            if (scripts.getTabCount() == 0) return;
            mapMaker.openMessageBox(
                MessageBoxCallback(this.deleteScriptMessageBox), { "yes", "no" },
                "deletescriptconfirmation",
                { any(scripts.getTabText(scripts.getSelectedTab())) });
            
        } else if (i == VIEW_SET_FONT_SIZE) {
            @fontSizeQueryWindow = ScriptFontSizeQueryWindow(fontSize,
                SingleSignalHandler(this.fontSizeQueryWindowValueChanged));
            mapMaker.openQueryWindow(fontSizeQueryWindow,
                QueryWindowCallback(this.fontSizeQueryWindowClosed));
            
        } else if (i == CODE_BUILD) {
            if (scripts.getTabCount() == 0) return;
            if (!edit.buildScriptFiles()) showLastBuildResult();
            
        } else if (i == CODE_LAST_RESULT) {
            showLastBuildResult();

        } else {
            critical("Unrecognised menu item ID " + formatMenuItemID(i) + " "
                "received in the scripts window!");
        }
    }
}

/**
 * Represents a single panel in the scripts window's tab container.
 * @warning We do not store full \c Widget objects in this class, as the engine is
 *          solely responsible for the lifetimes of the \c Panel widget and every
 *          widget that is within it.
 */
class ScriptPanel : Observer {
    /**
     * Creates a tab and panel in the given tab container and sets it up.
     * @param window       Points to the \c ScriptsWindow object.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param text         The text to give to the tab. Should be the name of the
     *                     script.
     */
    ScriptPanel(ScriptsWindow@ const window, TabContainer@ const tabContainer,
        const string&in text) {
        @scriptsWindow = window;
        scriptName = text;
        @parent = tabContainer;
        panelID = tabContainer.addTabAndPanel("~" + text);
        textAreaID = ::createWidget("TextArea");
        ::setWidgetName(textAreaID, "TEXT_AREA:" + text);
        ::setWidgetSize(textAreaID, "100%", "100%");
        ::setWidgetFont(textAreaID, "Monospace");
        ::optimiseTextAreaForMonospaceFont(textAreaID, true);
        ::add(panelID, textAreaID);
        ::connectSignal(textAreaID, TextChanged,
            SingleSignalHandler(this.textAreaTextChanged));
        ::connectSignal(textAreaID, CaretPositionChanged,
            SingleSignalHandler(this.textAreaCaretPositionChanged));
    }

    /**
     * When this panel is no longer needed, remove it from the tab container.
     */
    void cleanUp() {
        if (parent.get() !is null) parent.get().removeTabAndPanel(panelID);
    }

    /**
     * Used to update either the font size or the contents of the text area.
     */
    void refresh(any&in data = any()) override {
        currentlyRefreshing = true;
        uint fontSize = 0;
        string contents;
        if (data.retrieve(contents)) {
            ::setWidgetText(textAreaID, contents);
        } else if (data.retrieve(fontSize)) {
            ::setWidgetTextSize(textAreaID, fontSize);
        }
        currentlyRefreshing = false;
    }

    /**
     * Sets the focus to this panel's text area.
     */
    void setFocus() {
        ::setWidgetFocus(textAreaID);
    }

    /**
     * Gets the caret position in this panel's text area.
     * @return \c x stores the line, \c y stores the column.
     */
    Vector2 getCaretPosition() const {
        uint64 line = 0, column = 0;
        ::getCaretLineAndColumn(textAreaID, line, column);
        return Vector2(line, column);
    }

    /**
     * Notifies the parent that this text area's caret position has changed.
     */
    private void textAreaCaretPositionChanged() {
        // If the text is being updated due to a refresh, don't push the caret
        // position to the parent, as this text area may not necessarily be the
        // one currently open.
        if (currentlyRefreshing || scriptsWindow.get() is null) return;
        scriptsWindow.get().refresh(any(getCaretPosition()));
    }

    /**
     * When the text in the text area changes, we need to update the script in the
     * map file.
     */
    private void textAreaTextChanged() {
        // If the text is being updated due to a refresh, don't bother updating
        // the map file as the contents will be the same anyway.
        if (currentlyRefreshing) return;
        edit.updateScriptFile(scriptName, ::getWidgetText(textAreaID));
    }

    /**
     * Set to \c TRUE when \c refresh() is being called.
     */
    private bool currentlyRefreshing = false;

    /**
     * The name of the script this panel displays.
     */
    private string scriptName;

    /**
     * Weak reference to the scripts window.
     */
    private weakref<ScriptsWindow> scriptsWindow;

    /**
     * Points to the tab container that this script panel is a child of.
     */
    private weakref<TabContainer> parent;

    /**
     * Stores the ID of the panel in the tab container.
     */
    private WidgetID panelID;

    /**
     * Stores the ID of the text area in the panel.
     */
    private WidgetID textAreaID;
}

/**
 * The status bar of the scripts window.
 */
class ScriptsStatusBar : StatusBar {
    /**
     * Initialises the status bar with the labels it requires.
     */
    ScriptsStatusBar() {
        super({ 0.8, 0.2 });
    }

    /**
     * Sets the text of the compiler result label.
     * @param label The new text.
     */
    void setCompilerResult(const string&in label) {
        setLabel(0, "~" + label);
    }

    /**
     * Sets the text of the caret position label.
     * @param line The line number.
     * @param col  The column number.
     */
    void setCaretPosition(const uint64 line, const uint64 col) {
        setLabel(1, "caret", { any(line), any(col) });
    }

    /**
     * Clears the text from the caret position label.
     */
    void clearCaretPosition() {
        setLabel(1, "~");
    }
}

/**
 * The script name query window.
 */
class ScriptNameQueryWindow : QueryWindow {
    /**
     * Opens a script name query window.
     * @param textValidator When the text in the name edit box changes, this
     *                      handler will be called.
     * @param okHandler     When the ok button is pressed, or the enter key is
     *                      pressed in the edit box, invoke this handler.
     */
    ScriptNameQueryWindow(SingleSignalHandler@ const textValidator,
        SingleSignalHandler@ const okHandler) {
        super("enterscriptnameex", name, okHandler);
        setText("enterscriptname");
        @okButtonHandler = okHandler;
        name.connect(TextChanged, textValidator);
        name.connect(ReturnKeyPressed,
            SingleSignalHandler(this.nameReturnKeyPressed));
    }

    /**
     * Sets the focus on the edit box.
     */
    void setFocus() override {
        name.setFocus();
    }

    /**
     * Retrieves the text in the name edit box.
     * @return The name given by the user.
     */
    string getName() const {
        return name.getText();
    }

    /**
     * If the ok button is enabled, invoke the ok button handler as if it were
     * pressed.
     */
    private void nameReturnKeyPressed() {
        if (isOkEnabled()) okButtonHandler();
    }

    /**
     * When the ok button is pressed, or if the enter key is pressed in the edit
     * box when the ok button is enabled, run this code.
     */
    private SingleSignalHandler@ okButtonHandler;

    /**
     * The name input widget.
     */
    private EditBox name;
}

/**
 * The font size query window.
 */
class ScriptFontSizeQueryWindow : QueryWindow {
    /**
     * Opens a font size query window.
     * @param initialFontSize The current font size.
     * @param valueChanged    Invoked when the value of the spin control changes.
     */
    ScriptFontSizeQueryWindow(const uint initialFontSize,
        SingleSignalHandler@ const valueChanged) {
        super("setfontsizeex", size, SingleSignalHandler(this.okButtonPressed));
        enableOk(true);
        setText("setfontsize");
        size.setMinMaxValues(8.0, 64.0);
        size.setValue(initialFontSize);
        size.connect(ValueChanged, valueChanged);
        @valueChangedHandler = valueChanged;
    }

    /**
     * Get the font size selected by the user.
     * @return The value of the spin control cast to a \c uint.
     */
    uint getValue() const {
        return uint(size.getValue());
    }

    /**
     * When the ok button is pressed, invoke the \c ValueChanged callback, then
     * close the window and emit the \c Closing signal.
     */
    private void okButtonPressed() {
        valueChangedHandler();
        close(true);
    }

    /**
     * Cache of the \c ValueChanged handler.
     */
    private SingleSignalHandler@ valueChangedHandler;

    /**
     * The font size input widget.
     */
    private SpinControl size;
}
