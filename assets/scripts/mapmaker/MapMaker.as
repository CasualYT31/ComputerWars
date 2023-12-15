/**
 * @file MapMaker.as
 * The main map maker menu.
 */

/**
 * Empty callback signature.
 */
funcdef void EmptyCallback();

/**
 * When a message box closes, a callback of this signature will be invoked.
 * @param <tt>const uint64</tt> The ID of the button that was pressed.
 */
funcdef void MessageBoxCallback(const uint64);

/**
 * Represents the map maker.
 */
class MapMaker : Menu, Group {
    /**
     * Set up the menu.
     * Also sets up the root widget and performs other one-time setup tasks.
     */
    MapMaker() {
        // Setup the menu.
        menuBar.setName("MenuBar");
        menuBar.add("file");
        FILE_NEW_MAP = menuBar.addItem("newmap");
        FILE_OPEN_MAP = menuBar.addItem("openmap");
        FILE_SAVE_MAP = menuBar.addItem("savemap");
        FILE_SAVE_MAP_AS = menuBar.addItem("savemapas");
        FILE_QUIT = menuBar.addItem("quit");
        menuBar.add("edit");
        EDIT_UNDO = menuBar.addItem("undo");
        EDIT_REDO = menuBar.addItem("redo");
        EDIT_MEMENTO_WINDOW = menuBar.addItem("mementowindow");
        menuBar.add("map");
        MAP_SET_PROPS = menuBar.addItem("mapprops");
        MAP_FILL = menuBar.addItem("fill");
        MAP_FIX_TILES = menuBar.addItem("fixtiles");
        MAP_SCRIPTS = menuBar.addItem("scripts");
        menuBar.add("view");
        VIEW_TOOLBAR = menuBar.addItem("toolbar");
        VIEW_OBJECT_DIALOG = menuBar.addItem("objectdialog");
        VIEW_TILE_PROPS = menuBar.addItem("tileprops");
        VIEW_ARMY_PROPS = menuBar.addItem("armyprops");
        menuBar.connect(MenuItemClicked,
            SingleSignalHandler(this.menuItemClicked));
        
        // Setup the client area.
        clientArea.add(mementoWindow);
        clientArea.add(mapPropertiesWindow);
        clientArea.add(fillWindow);
        selectedTileType.attach(fillWindow);

        // Setup the base group.
        mainStatusBar.setSize("", "MenuBar.height");
        menuBar.setAutoLayout(AutoLayout::Top);
        clientArea.setAutoLayout(AutoLayout::Fill);
        mainStatusBar.setAutoLayout(AutoLayout::Bottom);
        baseGroup.add(menuBar);
        baseGroup.add(clientArea);
        baseGroup.add(mainStatusBar);

        // Setup the root group.
        dialogGroup.connect(SizeChanged,
            SingleSignalHandler(this.showMessageBox));
        add(dialogGroup);
        add(baseGroup);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Ensures the confirm fill window is detached from the selected tile type
     * object to allow for the window to be deleted.
     */
    ~MapMaker() {
        selectedTileType.detach(fillWindow);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        @mapMaker = this;
        // Debug {
        fileDialogFile = "./map/islandx.cwm";
        openEditMapAuthorised();
        // }
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
    }

    /**
     * Called periodically whilst the menu is open.
     * @param ui            Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered.
     * @param mouse         Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered by
     *                      the mouse.
     * @param previousMouse The position of the mouse during the previous
     *                      iteration of the game loop.
     * @param currentMouse  The position of the mouse right now.
     */
    void Periodic(const dictionary ui, const dictionary mouse,
        const MousePosition&in previousMouse,
        const MousePosition&in currentMouse) {
        // If there currently isn't a map loaded, or it is 0x0 tiles, then don't
        // try selecting any tiles or zooming in or out.
        // Also, if a MessageBox is open, we must prevent input until it is gone.
        if (edit is null || edit.map.getMapSize().x == 0 ||
            edit.map.getMapSize().y == 0 || messageBox !is null) return;

        // Handle mouse input. Ignore the mouse if the game doesn't have focus.
        bool mouseInMap = edit.map.getMapBoundingBox().contains(currentMouse);
        if (currentMouse != INVALID_MOUSE) {
            // Ignore the mouse if it's outside of the window.
            Vector2 windowSize = getWindowSize();
            if (currentMouse.x >= 0 && currentMouse.y >= 0
                && currentMouse.x <= int(windowSize.x)
                && currentMouse.y <= int(windowSize.y)) {
                // Only consider the mouse if it has moved.
                if (currentMouse != previousMouse)
                    edit.setSelectedTileByPixel(currentMouse);
            }
        }
    
        // If the user is currently typing into a textbox, cancel most input.
        if (editBoxOrTextAreaHasFocus()) return;

        // Only allow a mouse button to paint if the mouse is not hovering over a
        // widget. It's not perfect but it gets the job done. Case that doesn't
        // work: if the mouse is hovering over a widget, the user paints with the
        // keyboard buttons, and is issuing the paint mouse button at the same
        // time. Actually, testing it now, that feels intuitive. And no one will
        // be doing it anyway.
        const bool mouseNotUnderWidget = getWidgetUnderMouse() == NO_WIDGET;
        const bool modifier = bool(ui["modifier"]);
        const bool action = (bool(ui["action"]) && !modifier) && (
            !bool(mouse["action"]) || (mouseNotUnderWidget && mouseInMap)
        );
        const bool structAction = (bool(ui["structaction"]) && !modifier) && (
            !bool(mouse["structaction"]) || (mouseNotUnderWidget && mouseInMap)
        );
        const bool pick = (bool(ui["pick"]) && !modifier) && (
            !bool(mouse["pick"]) || (mouseNotUnderWidget && mouseInMap)
        );
        const bool tileinfo = (bool(ui["tileinfo"]) && !modifier) && (
            !bool(mouse["tileinfo"]) || (mouseNotUnderWidget && mouseInMap)
        );
        const bool undoControl = modifier && bool(ui["undo"]);
        const bool redoControl = modifier && bool(ui["redo"]);

        // Handle controls.
        if (bool(ui["up"])) {
            edit.moveSelectedTileUp();
        } else if (bool(ui["down"])) {
            edit.moveSelectedTileDown();
        } else if (bool(ui["left"])) {
            edit.moveSelectedTileLeft();
        } else if (bool(ui["right"])) {
            edit.moveSelectedTileRight();
        }
        if (bool(ui["zoomout"])) {
            edit.zoomOut();
        }
        if (bool(ui["zoomin"])) {
            edit.zoomIn();
        }
        // Undo and redo.
        if (undoControl) {
            edit.undo();
            return;
        }
        if (redoControl) {
            edit.redo();
            return;
        }
    }
    
    /////////////////
    // MESSAGE BOX //
    /////////////////
    
    /**
     * Opens a message box widget.
     * When a message box is opened, the map maker will be disabled, and the
     * dialog group will be enabled, with it being moved to the front.
     * @param callback When a button is pressed on the message box, this handler
     *                 will be called, after the message box is closed.
     * @param buttons  An array containing the text to apply to each button.
     * @param text     The text to display inside the message box.
     * @param vars     The variables to insert into the text, if any.
     */
    void openMessageBox(MessageBoxCallback@ const callback,
        const array<string>&in buttons, const string&in text,
        array<any>@ vars = null) {
        @messageBoxCallback = callback;
        baseGroup.setEnabled(false);
        @messageBox = MessageBox();
        messageBox.setStrings("alert", null, text, vars);
        for (uint64 b = 0, len = buttons.length(); b < len; ++b)
            messageBox.addButton(buttons[b]);
        showMessageBox();
        dialogGroup.setEnabled(true);
        dialogGroup.moveToFront();
        dialogGroup.add(messageBox);
        messageBox.connect(ButtonPressed,
            SingleSignalHandler(this.closeMessageBox));
    }

    /**
     * Cache of the callback given to \c openMessageBox().
     */
    private MessageBoxCallback@ messageBoxCallback;

    /**
     * Closes the opened message box widget.
     * When a message box is closed, the dialog group will be disabled, and the
     * map maker will be enabled, with it being moved to the front.
     */
    private void closeMessageBox() {
        const auto btn = messageBox.getPressedButton();
        @messageBox = null;
        dialogGroup.setEnabled(false);
        baseGroup.setEnabled(true);
        baseGroup.moveToFront();
        if (messageBoxCallback !is null) {
            messageBoxCallback(btn);
            @messageBoxCallback = null;
        }
    }

    /**
     * Brings the message box back into view.
     */
    private void showMessageBox() {
        if (messageBox is null) return;
        messageBox.setOrigin(0.5, 0.5);
        messageBox.setPosition("50%", "50%");
    }

    /**
     * Opens the "no map is open right now" message box.
     */
    private void openNoMapIsOpenMessageBox() {
        openMessageBox(null, { "ok" }, "nomapisopen");
    }

    /////////////////
    // FILE DIALOG //
    /////////////////

    /**
     * Opens a file dialog.
     * When a file dialog is opened, the map maker will be disabled, and the
     * dialog group will be enabled, with it being moved to the front.
     * @param handler   When a file is selected, this handler will be invoked.
     * @param title     The title of the file dialog.
     * @param confirm   The text to display on the "confirm" button.
     * @param mustExist \c TRUE if the selected path/s must exist, \c FALSE if
     *                  they are permitted to not exist.
     */
    private void openFileDialog(SingleSignalHandler@ const handler,
        const string&in title, const string&in confirm,
        const bool mustExist = false) {
        baseGroup.setEnabled(false);
        @fileDialog = FileDialog();
        fileDialog.setStrings(title, null,
            confirm, null,
            "cancel", null,
            "createfolder", null,
            "filename", null,
            "name", null,
            "size", null,
            "modified", null,
            "allfiles", null
        );
        fileDialog.mustExist(mustExist);
        fileDialog.setPath("./map");
        fileDialog.addFilter("mapfiles", null, { "*.cwm" });
        fileDialog.setDefaultFilter(1);
        dialogGroup.setEnabled(true);
        dialogGroup.moveToFront();
        dialogGroup.add(fileDialog);
        fileDialog.connect(FileSelected, handler);
        fileDialog.connectClosing(
            ChildWindowClosingSignalHandler(this.fileDialogClosing));
    }

    /**
     * When a file dialog closes, we need to re-enable the map maker, unless a
     * message box was created before the file dialog was closed.
     */
    private void fileDialogClosing(const WidgetID id, bool&out neverAbort) {
        @fileDialog = null;
        if (messageBox !is null) return;
        dialogGroup.setEnabled(false);
        baseGroup.setEnabled(true);
        baseGroup.moveToFront();
    }

    /**
     * Initialise a new edit map.
     * @param mapToHandle Points to the loaded map.
     */
    private void initEditMap(Map@ const mapToHandle) {
        @edit = EditableMap(mapToHandle);
        edit.map.setMementoStateChangedCallback(
            MementoStateChangedCallback(this.mementosHaveChanged));
        edit.setObserver(Subject::Properties, @mapPropertiesWindow);
        edit.setObserver(Subject::Status, @mainStatusBar);
        mementosHaveChanged();
    }

    /**
     * Cache of the selected path from the file dialog.
     */
    string fileDialogFile;

    //////////////
    // NEW FILE //
    //////////////

    /**
     * A path has been selected to write a new file into, so attempt to write it.
     */
    private void newEditMap() {
        fileDialogFile = fileDialog.getSelectedPaths()[0];
        if (doesPathExist(fileDialogFile))
            openMessageBox(MessageBoxCallback(this.newEditMapMessageBox),
                { "cancel", "ok" }, "mapfilealreadyexists",
                { any(fileDialogFile) });
        else newEditMapAuthorised();
    }

    /**
     * The user's response to the message box produced when attempting to
     * overwrite an existing map file with a new file.
     */
    private void newEditMapMessageBox(const uint64 id) {
        if (id == 1) newEditMapAuthorised();
    }

    /**
     * We can now try to open the new map.
     */
    private void newEditMapAuthorised() {
        quitEditMap(EmptyCallback(this.newEditMapAuthorisedWithAuthorisedQuit));
    }

    /**
     * The closure of the old map has been authorised, load new map.
     */
    private void newEditMapAuthorisedWithAuthorisedQuit() {
        initEditMap(createMap(fileDialogFile, PLAYABLE_MAP_TYPENAME));
        mapPropertiesWindow.open();
    }

    /////////////
    // OPENING //
    /////////////

    /**
     * A file has been selected for opening, so attempt to open it.
     */
    private void openEditMap() {
        fileDialogFile = fileDialog.getSelectedPaths()[0];
        quitEditMap(EmptyCallback(this.openEditMapAuthorised));
    }

    /**
     * We can now open the map file.
     */
    private void openEditMapAuthorised() {
        initEditMap(loadMap(fileDialogFile, PLAYABLE_MAP_TYPENAME));
    }

    ///////////////
    // SAVING AS //
    ///////////////

    /**
     * A path has been selected to write the map file to, so try to write to it.
     */
    private void saveEditMapAs() {
        fileDialogFile = fileDialog.getSelectedPaths()[0];
        if (doesPathExist(fileDialogFile))
            openMessageBox(MessageBoxCallback(this.saveEditMapAsMessageBox),
                { "cancel", "ok" }, "mapfilealreadyexists",
                { any(fileDialogFile) });
        else saveEditMapAsAuthorised();
    }

    /**
     * The user's response to the message box produced when attempting to
     * overwrite an existing map file with the currently open file.
     */
    private void saveEditMapAsMessageBox(const uint64 id) {
        if (id == 1) saveEditMapAsAuthorised();
    }

    /**
     * We can now try to open the new map.
     */
    private void saveEditMapAsAuthorised() {
        edit.map.save(fileDialogFile);
    }

    //////////////
    // QUITTING //
    //////////////

    /**
     * Cache of the callback given to \c quitEditMap().
     */
    private EmptyCallback@ quitCallback;

    /**
     * Attempts to quit \c edit.
     * @param callback If the quit operation was carried out, execute this code.
     */
    private void quitEditMap(EmptyCallback@ const callback) {
        // If there is no map to quit, then simply invoke the callback.
        if (edit is null) {
            callback();
            return;
        }
        // If the edit map hasn't changed, then automatically authorise the quit.
        @quitCallback = callback;
        if (!edit.map.hasChanged()) {
            quitEditMapAuthorised();
            return;
        }
        // Otherwise, we need to ask for the user's go-ahead.
        openMessageBox(MessageBoxCallback(this.quitEditMapMessageBox),
            { "yes", "no", "cancel" }, "savebeforequit");
    }

    /**
     * The user's response to the message box produced when attempting to quit
     * \c edit.
     */
    private void quitEditMapMessageBox(const uint64 id) {
        if (id == 0) {
            // Yes, do save.
            edit.map.save();
            quitEditMapAuthorised();
        } else if (id == 1) {
            // No, don't save.
            quitEditMapAuthorised();
        }
        // Cancel, don't quit.
    }

    /**
     * Quits \c edit.
     */
    private void quitEditMapAuthorised() {
        ::quitMap();
        mementoWindow.close();
        mapPropertiesWindow.close();
        fillWindow.close();
        @edit = null;
        mementoWindow.refresh();
        if (quitCallback !is null) quitCallback();
    }

    //////////////
    // MEMENTOS //
    //////////////

    /**
     * When the state of the mementos change, update the MementoWindow and the
     * status bar.
     */
    private void mementosHaveChanged() {
        mementoWindow.refresh();
        mainStatusBar.setUndoAction(edit.map.getNextUndoMementoName());
        mainStatusBar.setRedoAction(edit.map.getNextRedoMementoName());
    }

    //////////
    // FILL //
    //////////

    /**
     * The user has authorised the fill operation.
     */
    private void fillWindowYes() {
        fillWindow.close();
        edit.fillMap(selectedTileType.type, selectedTileType.owner);
    }

    /**
     * The user has cancelled the fill operation.
     */
    private void fillWindowNo() {
        fillWindow.close();
    }

    /////////////
    // WIDGETS //
    /////////////

    /**
     * The group in which all message boxes and file dialogs are created.
     */
    private Group dialogGroup;

    /**
     * Points to the single message box that can be open at any one time.
     */
    private MessageBox@ messageBox;

    /**
     * Points to the single file dialog that can be open at any one time.
     */
    private FileDialog@ fileDialog;
    
    /**
     * The base group within which every widget is contained (except for message
     * boxes and file dialogs).
     */
    private Group baseGroup;

    /**
     * The menu bar.
     */
    private MenuBar menuBar;

    /**
     * The client area group.
     */
    private Group clientArea;

    /**
     * The memento window.
     */
    private MementoWindow mementoWindow;

    /**
     * The map properties window.
     */
    private MapPropertiesWindow mapPropertiesWindow;

    /**
     * The window displayed when confirming a fill operation.
     */
    private ConfirmTileTypeWindow fillWindow("fillmapconfirmationnotile",
        "fillmapconfirmationtile", SingleSignalHandler(this.fillWindowYes),
        SingleSignalHandler(this.fillWindowNo));

    /**
     * The status bar.
     */
    private MainStatusBar mainStatusBar;

    ////////////////
    // MENU ITEMS //
    ////////////////

    private MenuItemID FILE_NEW_MAP;
    private MenuItemID FILE_OPEN_MAP;
    private MenuItemID FILE_SAVE_MAP;
    private MenuItemID FILE_SAVE_MAP_AS;
    private MenuItemID FILE_QUIT;
    private MenuItemID EDIT_UNDO;
    private MenuItemID EDIT_REDO;
    private MenuItemID EDIT_MEMENTO_WINDOW;
    private MenuItemID MAP_SET_PROPS;
    private MenuItemID MAP_FILL;
    private MenuItemID MAP_FIX_TILES;
    private MenuItemID MAP_SCRIPTS;
    private MenuItemID VIEW_TOOLBAR;
    private MenuItemID VIEW_OBJECT_DIALOG;
    private MenuItemID VIEW_TILE_PROPS;
    private MenuItemID VIEW_ARMY_PROPS;

    /**
     * When a menu item is clicked, run this handler.
     */
    private void menuItemClicked() {
        const auto i = menuBar.getSelectedItem();
        if (i == FILE_NEW_MAP) {
            openFileDialog(SingleSignalHandler(this.newEditMap), "newmap",
                "create");

        } else if (i == FILE_OPEN_MAP) {
            openFileDialog(SingleSignalHandler(this.openEditMap), "openmap",
                "open", true);

        } else if (i == FILE_SAVE_MAP && edit !is null) {
            edit.map.save();

        } else if (i == FILE_SAVE_MAP_AS && edit !is null) {
            openFileDialog(SingleSignalHandler(this.saveEditMapAs), "savemapas",
                "save");

        } else if (i == FILE_QUIT) {
            quitEditMap(function(){ setGUI("MainMenu"); });

        } else if (i == EDIT_UNDO && edit !is null) {
            edit.undo();

        } else if (i == EDIT_REDO && edit !is null) {
            edit.redo();

        } else if (i == EDIT_MEMENTO_WINDOW) {
            mementoWindow.open();

        } else if (i == MAP_SET_PROPS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else mapPropertiesWindow.open();

        } else if (i == MAP_FILL) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else fillWindow.open("fillmapconfirmation");

        } else if (i == MAP_FIX_TILES) {

        } else if (i == MAP_SCRIPTS) {

        } else if (i == VIEW_TOOLBAR) {

        } else if (i == VIEW_OBJECT_DIALOG) {

        } else if (i == VIEW_TILE_PROPS) {

        } else if (i == VIEW_ARMY_PROPS) {

        } else {
            critical("Unrecognised menu item ID " + formatMenuItemID(i) + " "
                "received in the Map Maker menu!");
        }
    }
}

/**
 * The status bar of the map maker menu.
 */
class MainStatusBar : Observer, StatusBar {
    /**
     * Initialises the main status bar with the labels it needs.
     */
    MainStatusBar() {
        super({ 1.0, 1.0, 1.0, 1.0 });
    }

    /**
     * Refreshes the status bar.
     */
    void refresh(any&in data = any()) override {
        setTileXY(edit.map.getSelectedTile());
        float scalingFactor = 0.0;
        data.retrieve(scalingFactor);
        setZoom(scalingFactor);
    }

    /**
     * Sets the text of the undo action label.
     * @param label The new text.
     */
    void setUndoAction(const string&in label) {
        setLabel(0, label);
    }

    /**
     * Sets the text of the redo action label.
     * @param label The new text.
     */
    void setRedoAction(const string&in label) {
        setLabel(1, label);
    }

    /**
     * Sets the X and Y location of the currently selected tile to the XY label.
     * @param pos The new X, Y location.
     */
    void setTileXY(const Vector2&in pos) {
        setLabel(2, "~" + pos.toString());
    }

    /**
     * Sets the map scaling factor to the zoom label.
     * @param factor The map scaling factor.
     */
    void setZoom(const float factor) {
        setLabel(3, "zoom", { any(factor) });
    }
}
