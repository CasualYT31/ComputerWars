/**
 * @file ScriptsWindow.as
 * Window allowing you to edit a map's scripts.
 */

/**
 * Stores the menu item IDs of the Script Window.
 * Must improve later.
 */
namespace scripts_window_internal {
}

/**
 * Represents the window used to edit map scripts.
 */
class ScriptsWindow {
    /**
     * Sets up the scripts window.
     * @param parent                 The container to add the scripts window to.
     * @param simpleMsgBox           The name of the simple message box to bring
     *                               up when the user queries the result of the
     *                               last build. The next two parameters will also
     *                               be used to enable and disable widgets when
     *                               opening this \c MessageBox.
     * @param disableThis            When the Scripts Window queries the user for
     *                               a script file name, this widget should be
     *                               disabled. It will be re-enabled once the
     *                               query window is shutdown.
     * @param baseGroupOfQueryWindow The query window will be added to this
     *                               container, and it will be enabled when the
     *                               window is opened and disable when the window
     *                               is closed.
     */
    void setUp(const string&in parent, const string&in simpleMsgBox,
        const string&in disableThis, const string&in baseGroupOfQueryWindow) {
        baseGroup = disableThis;
        buildResultMsgBox = simpleMsgBox;
        queryWindowBaseGroup = baseGroupOfQueryWindow;
        deleteFileConfirmationMsgBox =
            queryWindowBaseGroup + ".DeleteScriptFileMsgBox";

        _setUpQueryWindow();

        // Scripts Window.
        window = parent + ".ScriptsWindow";
        addWidget(ChildWindow, window);
        setWidgetText(window, "scripts");
        setWidgetSize(window, "600px", "400px");
        close();

        const auto menuBarHeight = _setUpMenuBar();

        tabContainer = window + ".TabContainer";
        addWidget(TabContainer, tabContainer);
        setWidgetSize(tabContainer, "100%", "100%-2*" + menuBarHeight);
        setWidgetPosition(tabContainer, "50%", "50%");
        setWidgetOrigin(tabContainer, 0.5f, 0.5f);
        connectSignalHandler(tabContainer,
            SignalHandler(this.tabContainerSignalHandler));

        statusBar.setUp(window, menuBarHeight);
    }

    /**
     * Sets up the query window.
     */
    private void _setUpQueryWindow() {
        // ChildWindow.
        queryWindow = queryWindowBaseGroup + ".ScriptNameQueryWindow";
        addWidget(ChildWindow, queryWindow);
        closeChildWindow(queryWindow);
        setChildWindowTitleButtons(queryWindow, TitleButton::Close);
        setWidgetResizable(queryWindow, false);
        setWidgetSize(queryWindow, "300", "175");
        setWidgetOrigin(queryWindow, 0.5f, 0.5f);
        setWidgetPosition(queryWindow, "50%", "50%");
        setWidgetPositionLocked(queryWindow, true);

        // Script Name Group.
        scriptNameQueryGroup = queryWindow + ".ScriptNameGroup";
        addWidget(Group, scriptNameQueryGroup);

        const auto nameLayout = scriptNameQueryGroup + ".Layout";
        addWidget(VerticalLayout, nameLayout);

        const auto labelGroup = nameLayout + ".LabelGroup";
        addWidget(Group, labelGroup);
        setWidgetRatioInLayout(nameLayout, 0, 1.5f);

        const auto label = labelGroup + ".Label";
        addWidget(Label, label);
        setWidgetText(label, "enterscriptnameex");
        setWidgetPosition(label, "50%", "50%");
        setWidgetOrigin(label, 0.5f, 0.5f);
        setWidgetTextAlignment(label, HorizontalAlignment::Centre,
            VerticalAlignment::Centre);

        const auto editGroup = nameLayout + ".EditGroup";
        addWidget(Group, editGroup);

        scriptNameQueryGroupEditBox = editGroup + ".Edit";
        addWidget(EditBox, scriptNameQueryGroupEditBox);
        setWidgetPosition(scriptNameQueryGroupEditBox, "50%", "50%");
        setWidgetOrigin(scriptNameQueryGroupEditBox, 0.5f, 0.5f);
        setEditBoxRegexValidator(scriptNameQueryGroupEditBox, VALIDATOR_TAB_NAME);
        connectSignalHandler(scriptNameQueryGroupEditBox,
            SignalHandler(this.scriptNameEditBoxSignalHandler));

        const auto buttonGroup = nameLayout + ".ButtonGroup";
        addWidget(Group, buttonGroup);

        scriptNameQueryGroupButton = buttonGroup + ".Button";
        addWidget(Button, scriptNameQueryGroupButton);
        setWidgetPosition(scriptNameQueryGroupButton, "50%", "50%");
        setWidgetOrigin(scriptNameQueryGroupButton, 0.5f, 0.5f);
        setWidgetText(scriptNameQueryGroupButton, "ok");
        setWidgetEnabled(scriptNameQueryGroupButton, false);
        connectSignalHandler(scriptNameQueryGroupButton,
            SignalHandler(this.scriptNameButtonSignalHandler));

        // Font Size Group.
        fontSizeQueryGroup = queryWindow + ".FontSizeGroup";
        addWidget(Group, fontSizeQueryGroup);

        fontSizeQueryGroupSpinControl = fontSizeQueryGroup + ".SpinControl";
        addWidget(SpinControl, fontSizeQueryGroupSpinControl);
        setWidgetPosition(fontSizeQueryGroupSpinControl, "50%", "50%");
        setWidgetOrigin(fontSizeQueryGroupSpinControl, 0.5f, 0.5f);
        setWidgetMinMaxValues(fontSizeQueryGroupSpinControl, 8.f, 64.f);
        setWidgetValue(fontSizeQueryGroupSpinControl, fontSize);
        connectSignalHandler(fontSizeQueryGroupSpinControl,
            SignalHandler(this.fontSizeSpinControlSignalHandler));
    }

    /**
     * Sets up the Script Window's \c MenuBar.
     * @return The height of the \c MenuBar.
     */
    private string _setUpMenuBar() {
        const auto menu = window + ".ScriptsWindowMenu";
        addWidget(MenuBar, menu);

        addMenu(menu, "file");
        file_new = addMenuItem(menu, "new");
        file_rename = addMenuItem(menu, "rename");
        file_save = addMenuItem(menu, "savemap");
        file_delete = addMenuItem(menu, "delete");

        addMenu(menu, "view");
        view_setFontSize = addMenuItem(menu, "setfontsize");

        addMenu(menu, "code");
        code_build = addMenuItem(menu, "build");
        code_test = addMenuItem(menu, "test");
        code_lastResult = addMenuItem(menu, "lastresult");

        return formatFloat(getWidgetFullSize(menu).y);
    }

    /// Opens and/or restores the scripts window to a known location.
    void restore() {
        openChildWindow(window, "125px", "50px");
        refresh();
    }

    /// If the window is open, refresh it with up-to-date data.
    void refresh() {
        if (isChildWindowOpen(window)) {
            // PLEASE NOTE THAT currentScript WILL NOT HOLD A VALID VALUE UNTIL
            // THE END!

            const auto newScripts = edit.map.getScriptNames();
            // Every script that doesn't exist anymore is to be removed.
            // Go backwards to prevent index out of bounds errors.
            for (int i = int(scriptNameCache.length()) - 1; i >= 0; --i) {
                const auto scriptName = scriptNameCache[i];
                if (newScripts.find(scriptName) < 0) {
                    // Delete script.
                    scriptNameCache.removeAt(i);
                    string panelName;
                    scriptPanels.get(scriptName, panelName);
                    scriptPanels.delete(scriptName);
                    textAreaScripts.delete(panelName + TEXT_AREA);
                    removeTabAndPanel(panelName);
                }
            }

            // Every new script must be added to this window.
            // And every script's content in this window must be updated.
            for (uint i = 0, len = newScripts.length(); i < len; ++i) {
                const auto scriptName = newScripts[i];
                string panelName;
                if (scriptNameCache.find(scriptName) < 0) {
                    // New script.
                    scriptNameCache.insertLast(scriptName);
                    panelName = addTabAndPanel(tabContainer, "~" + scriptName);
                    scriptPanels.set(scriptName, panelName);
                    const auto textArea = panelName + TEXT_AREA;
                    addWidget(TextArea, textArea);
                    textAreaScripts.set(textArea, scriptName);
                    setWidgetTextSize(textArea, fontSize);
                    setWidgetSize(textArea, "100%", "100%");
                    setWidgetFont(textArea, "Monospace");
                    optimiseTextAreaForMonospaceFont(textArea, true);
                    setWidgetText(textArea, edit.map.getScript(scriptName));
                    connectSignalHandler(textArea,
                        SignalHandler(this.textAreaSignalHandler));
                    // addTabAndPanel() used to be able to select the new tab for
                    // us, however I discovered that the SelectionChanged signal
                    // would be emitted before the text of the tab could get
                    // translated, so I decided to separate the selection from
                    // creation.
                    setSelectedTab(tabContainer, getTabCount(tabContainer) - 1);
                } else {
                    // Existing script.
                    scriptPanels.get(scriptName, panelName);
                    const auto textArea = panelName + TEXT_AREA;
                    disconnectSignalHandlers({ textArea });
                    setWidgetText(textArea, edit.map.getScript(scriptName));
                    connectSignalHandler(textArea,
                        SignalHandler(this.textAreaSignalHandler));
                }
            }
            
            // Update currentScript, caret position, and last known build result.
            tabContainerSignalHandler(tabContainer, "SelectionChanged");
            statusBar.setCompilerResult(edit.getLastKnownBuildResult());
        }
    }

    /// Closes the scripts window.
    void close() {
        closeChildWindow(window);
    }

    /**
     * Disables the rest of the GUI and asks the user for the new file's name via
     * the query window.
     */
    void newFile() {
        setWidgetVisibility(fontSizeQueryGroup, false);
        setWidgetVisibility(scriptNameQueryGroup, true);
        setWidgetText(scriptNameQueryGroupEditBox, "");
        scriptNameForNewScript = true;
        queryWindowIsOpening("enterscriptname");
    }

    /**
     * Disables the rest of the GUI and asks the user for the current file's new
     * name via the query window, if there is a current file.
     */
    void renameFile() {
        if (currentScript.isEmpty()) return;
        setWidgetVisibility(fontSizeQueryGroup, false);
        setWidgetVisibility(scriptNameQueryGroup, true);
        setWidgetText(scriptNameQueryGroupEditBox, "");
        scriptNameForNewScript = false;
        queryWindowIsOpening("enterscriptname");
    }

    /**
     * Asks the user if they wish to delete the current script.
     */
    void deleteFile() {
        if (currentScript.isEmpty()) return;
        awe::OpenMessageBox(deleteFileConfirmationMsgBox, "alert",
            "deletescriptconfirmation", { any(currentScript) }, baseGroup,
            queryWindowBaseGroup);
        addMessageBoxButton(deleteFileConfirmationMsgBox, "yes");
        addMessageBoxButton(deleteFileConfirmationMsgBox, "no");
    }

    /**
     * Deletes the given file and automatically opens the next file, if there is
     * one.
     * @param  scriptName Name of the script file to delete.
     * @throws If the given script name was not in the internal cache.
     */
    void deleteFileConfirmed(const string&in scriptName) {
        awe::CloseMessageBox(deleteFileConfirmationMsgBox, queryWindowBaseGroup,
            baseGroup);
        edit.removeScriptFile(currentScript);
    }

    /**
     * Cancels deleting the current script file.
     */
    void deleteFileCancelled() {
        awe::CloseMessageBox(deleteFileConfirmationMsgBox, queryWindowBaseGroup,
            baseGroup);
        tabSelected(getCurrentPanel());
    }

    /**
     * Disables the rest of the GUI and askes the user for the new font size to
     * apple to all \c TextArea widgets.
     */
    void setFontSize() {
        setWidgetVisibility(fontSizeQueryGroup, true);
        setWidgetVisibility(scriptNameQueryGroup, false);
        queryWindowIsOpening("setfontsize");
    }

    /**
     * Builds all scripts.
     * @return \c TRUE if the build was successful, \c FALSE otherwise. Will
     *         always return \c FALSE if there are no scripts to build.
     */
    bool build() {
        if (currentScript.isEmpty()) return false;
        const bool result = edit.buildScriptFiles();
        if (!result) showLastBuildResult();
        return result;
    }

    /**
     * Builds all Scripts, and if the build was successful, begins a testing
     * session.
     */
    void test() {
        if (build()) {
        }
    }

    /**
     * Show the last known build result in a \c MessageBox.
     */
    void showLastBuildResult() {
        string result = "~" + edit.getLastKnownBuildResult();
        if (result.findFirst(")") >= 0) {
            // Insert some newlines so that the message sits more comfortably in
            // the MessageBox.
            result = awe::join(awe::split(result, "), "), "),\n") + ")";
        }
        awe::OpenMessageBox(buildResultMsgBox, "alert", result, null, baseGroup,
            queryWindowBaseGroup);
        addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
    }

    /**
     * Common code performed when the query window is opened.
     * @param title The title to give the query window.
     */
    void queryWindowIsOpening(const string&in title) {
        setWidgetEnabled(baseGroup, false);
        setWidgetEnabled(queryWindowBaseGroup, true);
        setWidgetText(queryWindow, title);
        openChildWindow(queryWindow, "50%", "50%");
        moveWidgetToFront(queryWindowBaseGroup);
    }

    /**
     * Must be called when the query window is closing.
     */
    void queryWindowIsClosing() {
        setWidgetEnabled(queryWindowBaseGroup, false);
        setWidgetEnabled(baseGroup, true);
        moveWidgetToFront(baseGroup);
    }

    /**
     * Handles the \c TabContainer widget's signals.
     * @param widgetName The full name of the \c TabContainer.
     * @param signalName The name of the signal emitted.
     */
    private void tabContainerSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "SelectionChanged") {
            const auto index = getSelectedTab(tabContainer);
            if (index < 0) currentScript = "";
            else currentScript = getTabText(tabContainer, uint64(index));
            tabSelected(getCurrentPanel());
        }
    }

    /**
     * A script has just been selected.
     * Sets focus to its \c TextArea and update the caret position label, assuming
     * the tab has a \c TextArea.
     * @param panelName The name of the tab's panel.
     */
    void tabSelected(const string&in panelName) {
        updateCaretPosition(panelName);
        if (!panelName.isEmpty()) setWidgetFocus(panelName + TEXT_AREA);
    }

    /**
     * Handles every \c TextArea widget's signals.
     * @param widgetName The full name of the \c TextArea.
     * @param signalName The name of the signal emitted.
     */
    private void textAreaSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "CaretPositionChanged")
            updateCaretPosition(getCurrentPanel());
        else if (signalName == "TextChanged") {
            string changedScriptName;
            textAreaScripts.get(widgetName, changedScriptName);
            edit.updateScriptFile(changedScriptName, getWidgetText(widgetName));
        }
    }

    /// Updates the Status Bar's caret position.
    /// @param panelName The name of the tab's panel.
    private void updateCaretPosition(const string&in panelName) {
        if (panelName.isEmpty()) statusBar.clearCaretPosition();
        else {
            uint64 line = 0, column = 0;
            getCaretLineAndColumn(panelName + TEXT_AREA, line, column);
            statusBar.setCaretPosition(line, column);
        }
    }

    /**
     * Gets the panel of the currently open script.
     * @return The name of the \c Panel widget. An empty string if there isn't a
     *         script open right now.
     */
    string getCurrentPanel() {
        if (currentScript.isEmpty()) return "";
        string panelName;
        scriptPanels.get(currentScript, panelName);
        return panelName;
    }

    /**
     * Handles the script name query window's \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void scriptNameEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto text = getWidgetText(widgetName);
            if (text.length() == 0) {
                setWidgetEnabled(scriptNameQueryGroupButton, false);
            } else if (scriptNameCache.find(text) >= 0) {
                setWidgetEnabled(scriptNameQueryGroupButton, false);
            } else {
                setWidgetEnabled(scriptNameQueryGroupButton, true);
            }
        } else if (signalName == "ReturnKeyPressed") {
            if (getWidgetEnabled(scriptNameQueryGroupButton)) {
                scriptNameButtonSignalHandler(scriptNameQueryGroupButton,
                    "Pressed");
            }
        }
    }

    /**
     * Handles the script name query window's \c Button signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void scriptNameButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            queryWindowIsClosing();
            const auto text = getWidgetText(scriptNameQueryGroupEditBox);
            if (scriptNameForNewScript) edit.addScriptFile(text, "");
            else edit.renameScriptFile(currentScript, text);
            closeChildWindow(queryWindow);
        }
    }

    /**
     * Handles the font size query window's \c SpinControl signals.
     * @param widgetName The full name of the \c SpinControl.
     * @param signalName The name of the signal emitted.
     */
    private void fontSizeSpinControlSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ValueChanged") {
            fontSize = uint(getWidgetValue(fontSizeQueryGroupSpinControl));
            for (uint i = 0, len = scriptNameCache.length(); i < len; ++i) {
                string panelName;
                scriptPanels.get(scriptNameCache[i], panelName);
                setWidgetTextSize(panelName + TEXT_AREA, fontSize);
            }
        }
    }

    /// Cache of the current map's script names.
    /// There is no guaranteed order to this list.
    private array<string> scriptNameCache;

    /// The name of the script currently open for editting.
    private string currentScript = "";

    /// @return The name of the currently selected script.
    string getCurrentScript() const {
        return currentScript;
    }

    /// Maps script names to their panel names.
    private dictionary scriptPanels;

    /// Maps TextArea names to their script names.
    private dictionary textAreaScripts;

    /// Append a script's panel's name with this to access its \c TextArea.
    private string TEXT_AREA = ".TextArea";

    /// The name given to the script delete confirmation message box.
    private string deleteFileConfirmationMsgBox;

    /// The name of the \c MessageBox that is opened when the last build result is
    /// queried.
    private string buildResultMsgBox;

    /// Disable this widget when the query window is opened.
    private string baseGroup;

    /// Enable this widget when the query window is opened.
    private string queryWindowBaseGroup;

    /// Caches the name of the query \c ChildWindow.
    private string queryWindow;

    /// Caches the name of the script name \c Group in the query window.
    private string scriptNameQueryGroup;

    /// Caches the name of the script name group's \c EditBox.
    private string scriptNameQueryGroupEditBox;

    /// Caches the name of the script name group's \c Button.
    private string scriptNameQueryGroupButton;

    /// If \c TRUE, the user has given a name for a new script file.
    /// If \c FALSE, the user has given a new name for an existing script file.
    private bool scriptNameForNewScript = false;

    /// Caches the name of the font size \c Group in the query window.
    private string fontSizeQueryGroup;

    /// Caches the name of the \c SpinControl used to input the font size.
    private string fontSizeQueryGroupSpinControl;

    /// The font size applied to all \c TextArea widgets.
    private uint fontSize = 13;

    /// Caches the name of the \c ChildWindow.
    private string window;

    /// Caches the name of the \c TabContainer.
    private string tabContainer;

    /// The status bar.
    private ScriptsStatusBar statusBar;

    /// Menu item IDs. Should implement a way to receive MenuItemClicked signals
    /// via a given signal handler.
    private MenuItemID file_new;
    MenuItemID FILE_NEW { get const { return file_new; } }
    private MenuItemID file_rename;
    MenuItemID FILE_RENAME { get const { return file_rename; } }
    private MenuItemID file_save;
    MenuItemID FILE_SAVE { get const { return file_save; } }
    private MenuItemID file_delete;
    MenuItemID FILE_DELETE { get const { return file_delete; } }

    private MenuItemID view_setFontSize;
    MenuItemID VIEW_SET_FONT_SIZE { get const { return view_setFontSize; } }

    private MenuItemID code_build;
    MenuItemID CODE_BUILD { get const { return code_build; } }
    private MenuItemID code_test;
    MenuItemID CODE_TEST { get const { return code_test; } }
    private MenuItemID code_lastResult;
    MenuItemID CODE_LAST_RESULT { get const { return code_lastResult; } }
}

/**
 * Handles Script Window's menu item selection signals.
 * @param id The ID of the menu or menu item selected.
 */
void MapMakerMenu_ScriptsWindowMenu_MenuItemClicked(const MenuItemID id) {
    if (id == ScriptsDialog.FILE_NEW) {
        ScriptsDialog.newFile();
    
    } else if (id == ScriptsDialog.FILE_RENAME) {
        ScriptsDialog.renameFile();
    
    } else if (id == ScriptsDialog.FILE_SAVE) {
        edit.map.save();

    } else if (id == ScriptsDialog.FILE_DELETE) {
        ScriptsDialog.deleteFile();

    } else if (id == ScriptsDialog.VIEW_SET_FONT_SIZE) {
        ScriptsDialog.setFontSize();

    } else if (id == ScriptsDialog.CODE_BUILD) {
        ScriptsDialog.build();

    } else if (id == ScriptsDialog.CODE_TEST) {
        ScriptsDialog.test();

    } else if (id == ScriptsDialog.CODE_LAST_RESULT) {
        ScriptsDialog.showLastBuildResult();

    } else {
        error("Unrecognised menu item ID " + formatMenuItemID(id) + " received "
            "in the Scripts Window menu!");
    }
}

/**
 * Query window was closed.
 * @param close Always left to \c TRUE.
 */
void MapMakerMenu_ScriptNameQueryWindow_Closing(bool&out close) {
    ScriptsDialog.queryWindowIsClosing();
    ScriptsDialog.tabSelected(ScriptsDialog.getCurrentPanel());
}

/**
 * User either cancelled the current script's deletion or confirmed it.
 */
void MapMakerMenu_DeleteScriptFileMsgBox_ButtonPressed(const uint64 btn) {
    if (btn == 0)
        ScriptsDialog.deleteFileConfirmed(ScriptsDialog.getCurrentScript());
    else
        ScriptsDialog.deleteFileCancelled();
}
