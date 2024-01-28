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
 * When a query window closes, a callback of this signature will be invoked.
 */
funcdef void QueryWindowCallback();

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
        clientArea.add(scriptsWindow);
        clientArea.add(toolBar);
        clientArea.add(paletteWindow);
        clientArea.add(armyPropertiesWindow);
        clientArea.add(tilePropertiesWindow);
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
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        @mapMaker = this;
        toolBar.dock();
        paletteWindow.dock();
        mainStatusBar.clear();
        setVisibility(true);
        play("music", "mapmaker", 0.0);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        setVisibility(false);
        stop("music", 0.0);
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
        
        // If the environment spritesheet has changed, force an update to the
        // currently selected terrain and tile type. This will regenerate sprites
        // where ever it is necessary.
        if (lastKnownEnvironmentSpritesheet !=
            edit.map.getEnvironmentSpritesheet()) {
            lastKnownEnvironmentSpritesheet = edit.map.getEnvironmentSpritesheet();
            selectedTerrain.update();
            selectedTileType.update();
        }
        if (lastKnownEnvironmentPictureSpritesheet !=
            edit.map.getEnvironmentPictureSpritesheet()) {
            lastKnownEnvironmentPictureSpritesheet =
                edit.map.getEnvironmentPictureSpritesheet();
            edit.refreshTileProps();
        }
        if (lastKnownEnvironmentStructureIconSpritesheet !=
            edit.map.getEnvironmentStructureIconSpritesheet()) {
            lastKnownEnvironmentStructureIconSpritesheet =
                edit.map.getEnvironmentStructureIconSpritesheet();
            selectedStructure.update();
        }

        // Update the viewport of the map to make sure no tiles are hiding behind
        // the menu or status bars.
        const Vector2 windowSize = getWindowSize();
        edit.map.setViewport(0.0, menuBar.getScaledFullSize().y / windowSize.y, 0.0,
            mainStatusBar.getScaledFullSize().y / windowSize.y);

        // Handle mouse input. Ignore the mouse if the game doesn't have focus.
        bool mouseInMap = edit.map.getMapBoundingBox().contains(currentMouse);
        if (currentMouse != INVALID_MOUSE) {
            // Ignore the mouse if it's outside of the window.
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
        const bool mouseNotUnderWidget = !isWidgetUnderMouse();
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
        
        // If there isn't a tile currently selected that is in bounds, return now.
        const auto curTile = edit.map.getSelectedTile();
        if (edit.map.isOutOfBounds(curTile)) return;
        const auto curTileOwner = edit.map.getTileOwner(curTile);
        const auto curUnit = edit.map.getUnitOnTile(curTile);
        const auto terrainPaletteIsOpen =
            paletteWindow.getSelectedPalette() == Palette::Terrain,
            tileTypePaletteIsOpen =
                paletteWindow.getSelectedPalette() == Palette::TileType,
            unitTypePaletteIsOpen =
                paletteWindow.getSelectedPalette() == Palette::UnitType,
            structurePaletteIsOpen =
                paletteWindow.getSelectedPalette() == Palette::Structure;

        // Highlight the selected structure's tiles on the map if one is selected.
        if (structurePaletteIsOpen)
            edit.setStructurePaintedTiles(selectedStructure.type);
        else edit.setStructurePaintedTiles(null);

        // If the paint tool has just been triggered, disable mementos.
        // This is so that an entire paint stroke can be saved in one memento.
        // It's not strickly needed for the rectangle tools but it's a lot easier
        // to handle it this way.
        if (!prevAction && action && !structurePaletteIsOpen) {
            edit.map.disableMementos();
            mementoName = "";
        }

        // Normal paint tool with non-structures.
        if (action && toolBar.selected(Tool::Paint)) {
            if (terrainPaletteIsOpen && selectedTerrain.type !is null) {
                edit.setTerrain(curTile, selectedTerrain.type,
                    selectedTerrain.owner);
                mementoName = OPERATION[Operation::PAINT_TERRAIN_TOOL];
            } else if (tileTypePaletteIsOpen && selectedTileType.type !is null) {
                edit.setTile(curTile, selectedTileType.type,
                    selectedTileType.owner);
                mementoName = OPERATION[Operation::PAINT_TILE_TOOL];
            } else if (unitTypePaletteIsOpen && selectedUnitType.type !is null) {
                edit.createUnit(curTile, selectedUnitType.type,
                    selectedUnitType.owner);
                mementoName = OPERATION[Operation::PAINT_UNIT_TOOL];
            }

        // Normal paint tool with structures.
        } else if (structAction && toolBar.selected(Tool::Paint)) {
            if (structurePaletteIsOpen && selectedStructure.type !is null) {
                edit.paintStructure(curTile, selectedStructure.type,
                    selectedStructure.owner, selectedStructure.destroyed);
            }

        // Pick tool.
        } else if (pick) {
            if (terrainPaletteIsOpen) {
                const auto terrainType = edit.map.getTileType(curTile).type;
                if (terrainType.isPaintable) {
                    @selectedTerrain.type = terrainType;
                    selectedTerrain.owner = curTileOwner == NO_ARMY ? "" :
                        country.scriptNames[curTileOwner];
                }

            } else if (tileTypePaletteIsOpen) {
                const auto tileType = edit.map.getTileType(curTile);
                if (tileType.isPaintable) {
                    @selectedTileType.type = tileType;
                    selectedTileType.owner = curTileOwner == NO_ARMY ? "" :
                        country.scriptNames[curTileOwner];
                }

            } else if (unitTypePaletteIsOpen && curUnit != NO_UNIT) {
                @selectedUnitType.type = edit.map.getUnitType(curUnit);
                selectedUnitType.owner =
                    country.scriptNames[edit.map.getArmyOfUnit(curUnit)];

            } else if (structurePaletteIsOpen &&
                edit.map.isTileAStructureTile(curTile)) {
                const auto structureObject = edit.map.getTileStructure(curTile);
                if (structureObject.isPaintable) {
                    @selectedStructure.type = structureObject;
                    const auto offset = edit.map.getTileStructureOffset(curTile);
                    const Vector2 rootTile(curTile.x - offset.x,
                        curTile.y - offset.y);
                    const auto rootTileOwner = edit.map.getTileOwner(rootTile);
                    selectedStructure.owner = rootTileOwner == NO_ARMY ? "" :
                        country.scriptNames[rootTileOwner];
                }
            }

        // Delete tool.
        } else if (action && toolBar.selected(Tool::Delete)) {
            edit.deleteUnit(curUnit);
            if (mementoName.isEmpty() && curUnit != NO_UNIT)
                mementoName = OPERATION[Operation::DELETE_UNIT_TOOL];
        
        // Rectangle paint tool.
        } else if (toolBar.selected(Tool::RectPaint)) {
            Vector2 start, end;
            if (drawRectangle(prevAction, action, curTile, start, end)) {
                if (terrainPaletteIsOpen && selectedTerrain.type !is null) {
                    edit.rectangleFillTiles(start, end, selectedTerrain.type,
                        selectedTerrain.owner);
                    mementoName = OPERATION[Operation::RECT_PAINT_TERRAIN_TOOL];
                }
                if (tileTypePaletteIsOpen && selectedTileType.type !is null) {
                    edit.rectangleFillTiles(start, end,
                        selectedTileType.type.scriptName,
                        selectedTileType.owner.isEmpty() ? NO_ARMY :
                            country[selectedTileType.owner].turnOrder);
                    mementoName = OPERATION[Operation::RECT_PAINT_TILE_TOOL];
                }
                if (unitTypePaletteIsOpen && selectedUnitType.type !is null) {
                    edit.rectangleFillUnits(start, end,
                        selectedUnitType.type.scriptName,
                        country[selectedUnitType.owner].turnOrder);
                    mementoName = OPERATION[Operation::RECT_PAINT_UNIT_TOOL];
                }
            }
        
        // Rectangle delete tool.
        } else if (toolBar.selected(Tool::RectDelete)) {
            Vector2 start, end;
            if (drawRectangle(prevAction, action, curTile, start, end)) {
                if (edit.rectangleDeleteUnits(start, end) > 0)
                    mementoName = OPERATION[Operation::RECT_DELETE_UNIT_TOOL];
            }
        }

        // If the paint tool is finished with, re-enable mementos.
        if (prevAction && !action && !structurePaletteIsOpen)
            edit.map.enableMementos(mementoName);

        // If more detail on the current tile is desired, display its information.
        if (tileinfo) edit.selectTile(curTile);

        prevAction = action;
    }

    /**
     * Continuously draw a rectangle on the map.
     * @param  prevAction \c TRUE if the action control was being triggered during
     *                    the previous iteration of the game loop.
     * @param  action     \c TRUE if the action control is being triggered during
     *                    this iteration of the game loop.
     * @param  curTile    The currently selected tile.
     * @param  start      If \c TRUE is returned, the start of the rectangle is
     *                    stored here.
     * @param  end        If \c TRUE is returned, the end of the rectangle is
     *                    stored here.
     * @return \c TRUE if the rectangle is finished, \c FALSE if not.
     */
    private bool drawRectangle(const bool prevAction, const bool action,
        const Vector2&in curTile, Vector2&out start, Vector2&out end) {
        if (!prevAction && action) {        // Start rectangle.
            edit.map.setRectangleSelectionStart(curTile);
        } else if (prevAction && action) {  // Continue rectangle.
            edit.map.setRectangleSelectionEnd(curTile);
        } else if (prevAction && !action) { // Finish rectangle.
            start = edit.map.getRectangleSelectionStart();
            end = edit.map.getRectangleSelectionEnd();
            edit.map.removeRectangleSelection();
            return true;
        }
        return false;
    }

    /**
     * Stores the action control's signal state from the previous \c Periodic()
     * call.
     */
    private bool prevAction = false;

    /**
     * After a tool has finished, what should its name be? Assign to an empty
     * string to discard the memento.
     */
    private string mementoName;
    
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
    private void fileDialogClosing(bool&out neverAbort) {
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
        // The normal cursor will be set right now, so only check for the deletion
        // tools here.
        if (toolBar.selected(Tool::Delete) || toolBar.selected(Tool::RectDelete))
            edit.setDeleteCursorSprites();
        edit.map.addMementoStateChangedCallback(
            MementoStateChangedCallback(this.mementosHaveChanged));
        edit.setObserver(Subject::Properties, @mapPropertiesWindow);
        edit.setObserver(Subject::Scripts, @scriptsWindow);
        edit.setObserver(Subject::Status, @mainStatusBar);
        edit.setObserver(Subject::Armies, @armyPropertiesWindow);
        edit.setObserver(Subject::Tiles, @tilePropertiesWindow);
        mementosHaveChanged();
    }

    /**
     * Cache of the selected path from the file dialog.
     */
    string fileDialogFile;

    //////////////////
    // QUERY WINDOW //
    //////////////////

    /**
     * Opens a query window and disables the rest of the map maker until it
     * closes.
     * @param queryWindow The \c QueryWindow setup by the caller. Note that its
     *                    \c Closing signal handler will be updated by this
     *                    method!
     * @param callback    After the query window is closed, invoke this callback.
     */
    void openQueryWindow(QueryWindow@ const queryWindow,
        QueryWindowCallback@ const callback) {
        @queryWindowCallback = callback;
        baseGroup.setEnabled(false);
        dialogGroup.setEnabled(true);
        dialogGroup.moveToFront();
        dialogGroup.add(queryWindow);
        queryWindow.connectClosing(
            ChildWindowClosingSignalHandler(this.queryWindowClosing));
    }

    /**
     * Cache of the callback given to \c openQueryWindow().
     */
    private QueryWindowCallback@ queryWindowCallback;

    /**
     * When the opened query window is closed, re-enable the rest of the map
     * maker.
     */
    private void queryWindowClosing(bool&out neverAbort) {
        dialogGroup.setEnabled(false);
        baseGroup.setEnabled(true);
        baseGroup.moveToFront();
        if (queryWindowCallback !is null) {
            queryWindowCallback();
            @queryWindowCallback = null;
        }
    }

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
     * We can now save the map.
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
        ::quitMap("MapMaker");
        mementoWindow.close();
        mapPropertiesWindow.close();
        fillWindow.close();
        scriptsWindow.close();
        armyPropertiesWindow.close();
        tilePropertiesWindow.close();
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

    //////////
    // DATA //
    //////////

    /**
     * Stores the current map's last known environment spritesheet.
     * @remark This is another good reason why awe::map should emit signals.
     */
    private string lastKnownEnvironmentSpritesheet;

    /**
     * Stores the current map's last known environment tile picture spritesheet.
     */
    private string lastKnownEnvironmentPictureSpritesheet;

    /**
     * Stores the current map's last known environment structure icon spritesheet.
     */
    private string lastKnownEnvironmentStructureIconSpritesheet;

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
     * The scripts window.
     */
    private ScriptsWindow scriptsWindow;

    /**
     * The window containing the painting tools available.
     */
    private ToolBar toolBar;

    /**
     * The palette window.
     */
    private PaletteWindow paletteWindow(@toolBar);

    /**
     * The army properties window.
     */
    private ArmyPropertiesWindow armyPropertiesWindow;

    /**
     * The tile properties window.
     */
    private TilePropertiesWindow tilePropertiesWindow;

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

        } else if (i == FILE_SAVE_MAP) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else edit.map.save();

        } else if (i == FILE_SAVE_MAP_AS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else openFileDialog(SingleSignalHandler(this.saveEditMapAs),
                "savemapas", "save");

        } else if (i == FILE_QUIT) {
            quitEditMap(function(){ setGUI("MainMenu"); });

        } else if (i == EDIT_UNDO) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else edit.undo();

        } else if (i == EDIT_REDO) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else edit.redo();

        } else if (i == EDIT_MEMENTO_WINDOW) {
            mementoWindow.open();

        } else if (i == MAP_SET_PROPS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else mapPropertiesWindow.open();

        } else if (i == MAP_FILL) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else fillWindow.open("fillmapconfirmation");

        } else if (i == MAP_FIX_TILES) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else openMessageBox(
                function(const uint64 id){ if (id == 0) edit.fixTiles(); },
                { "yes", "no" }, "tilefixconfirmation");

        } else if (i == MAP_SCRIPTS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else scriptsWindow.open();

        } else if (i == VIEW_TOOLBAR) {
            toolBar.dock();

        } else if (i == VIEW_OBJECT_DIALOG) {
            paletteWindow.dock();

        } else if (i == VIEW_TILE_PROPS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else tilePropertiesWindow.dock();

        } else if (i == VIEW_ARMY_PROPS) {
            if (edit is null) openNoMapIsOpenMessageBox();
            else armyPropertiesWindow.open();

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
