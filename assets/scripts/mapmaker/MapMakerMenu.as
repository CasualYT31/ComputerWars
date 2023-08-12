/**
 * @file MapMakerMenu.as
 * The main map maker menu.
 */

/**
 * Global point of access to the map being editted in the map maker.
 */
EditableMap@ edit;

const auto BASE_GROUP = "MapMakerMenu.BaseGroup";
const auto MENU = BASE_GROUP + ".Menu";
const auto CLIENT_AREA = BASE_GROUP + ".Main";

const auto MESSAGE_BOX_GROUP = "MessageBoxGroup";
/// Used with \c MessageBoxes that shouldn't do anything special when a button is
/// pressed beyond closing the \c MessageBox.
const auto SIMPLE_MESSAGE_BOX = MESSAGE_BOX_GROUP + ".SimpleMessageBox";
const auto FILE_ALREADY_EXISTS = MESSAGE_BOX_GROUP + ".FileAlreadyExists";
const auto SAVE_BEFORE_QUIT = MESSAGE_BOX_GROUP + ".SaveBeforeQuit";

const auto FILE_DIALOG_GROUP = "FileDialogGroup";
const auto NEW_MAP = FILE_DIALOG_GROUP + ".NewMap";
const auto OPEN_MAP = FILE_DIALOG_GROUP + ".OpenMap";
const auto SAVE_MAP = FILE_DIALOG_GROUP + ".SaveMap";

MenuItemID MAP_MAKER_FILE_NEW_MAP;
MenuItemID MAP_MAKER_FILE_OPEN_MAP;
MenuItemID MAP_MAKER_FILE_SAVE_MAP;
MenuItemID MAP_MAKER_FILE_SAVE_MAP_AS;
MenuItemID MAP_MAKER_FILE_QUIT;

MenuItemID MAP_MAKER_MAP_SET_PROPS;
MenuItemID MAP_MAKER_MAP_FILL;

MenuItemID MAP_MAKER_VIEW_TOOLBAR;
MenuItemID MAP_MAKER_VIEW_OBJECT_DIALOG;
MenuItemID MAP_MAKER_VIEW_TILE_PROPS;
MenuItemID MAP_MAKER_VIEW_ARMY_PROPS;

ToolBar TOOLBAR;
ToolBarButtonSetUpData PAINT_TOOL("paint", "painttool");
ToolBarButtonSetUpData DELETE_TOOL("delete", "deletetool");
ToolBarButtonSetUpData RECT_TOOL("rect", "recttool");

MapPropertiesWindow MapPropertiesDialog(SIMPLE_MESSAGE_BOX, BASE_GROUP,
    MESSAGE_BOX_GROUP);
TilePropertiesWindow TilePropertiesDialog(SIMPLE_MESSAGE_BOX, BASE_GROUP,
    MESSAGE_BOX_GROUP);
ArmyPropertiesWindow ArmyPropertiesDialog(SIMPLE_MESSAGE_BOX, BASE_GROUP,
    MESSAGE_BOX_GROUP);
ConfirmTileTypeWindow FillWindow;

/**
 * Sets up the map maker menu.
 */
void MapMakerMenuSetUp() {
    // Dialog groups.

    addWidget("Group", MESSAGE_BOX_GROUP);
    setWidgetEnabled(MESSAGE_BOX_GROUP, false);
    addWidget("Group", FILE_DIALOG_GROUP);

    // Menu.

    addWidget("Group", BASE_GROUP);
    addWidget("MenuBar", MENU);
    addWidget("Group", CLIENT_AREA);
    setWidgetSize(CLIENT_AREA, "100%", "100%-" +
        formatFloat(getWidgetFullSize(MENU).y) + "px");
    setWidgetPosition(CLIENT_AREA, "50%", "100%");
    setWidgetOrigin(CLIENT_AREA, 0.5f, 1.0f);

    // Menu bar.

    addMenu(MENU, "file");
    MAP_MAKER_FILE_NEW_MAP = addMenuItem(MENU, "newmap");
    MAP_MAKER_FILE_OPEN_MAP = addMenuItem(MENU, "openmap");
    MAP_MAKER_FILE_SAVE_MAP = addMenuItem(MENU, "savemap");
    MAP_MAKER_FILE_SAVE_MAP_AS = addMenuItem(MENU, "savemapas");
    MAP_MAKER_FILE_QUIT = addMenuItem(MENU, "quit");

    addMenu(MENU, "map");
    MAP_MAKER_MAP_SET_PROPS = addMenuItem(MENU, "mapprops");
    MAP_MAKER_MAP_FILL = addMenuItem(MENU, "fill");

    addMenu(MENU, "view");
    MAP_MAKER_VIEW_TOOLBAR = addMenuItem(MENU, "toolbar");
    MAP_MAKER_VIEW_OBJECT_DIALOG = addMenuItem(MENU, "objectdialog");
    MAP_MAKER_VIEW_TILE_PROPS = addMenuItem(MENU, "tileprops");
    MAP_MAKER_VIEW_ARMY_PROPS = addMenuItem(MENU, "armyprops");

    // ToolBar.

    TOOLBAR.setUp(CLIENT_AREA + ".ToolBar",
        {PAINT_TOOL, DELETE_TOOL, RECT_TOOL });

    // Dialogs.

    PaletteWindow.setUp(DefaultObjectDialogData(CLIENT_AREA));
    PaletteWindow.dock();
    MapPropertiesDialog.setUp(CLIENT_AREA);
    TilePropertiesDialog.setUp(CLIENT_AREA);
    ArmyPropertiesDialog.setUp(CLIENT_AREA);
    FillWindow.setUp(CLIENT_AREA + ".ConfirmTileTypeForFillWindow",
        "fillmapconfirmationnotile", "fillmapconfirmationtile",
        function(widgetName, signalName) {
            if (signalName == "Pressed") {
                if (edit is null) { // Safe guard: code should never get here.
                    awe::OpenMessageBox(SIMPLE_MESSAGE_BOX, "alert",
                        "nomapisopen", null, BASE_GROUP, MESSAGE_BOX_GROUP);
                    addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
                } else {
                    edit.map.fillMap(
                        cast<TileType>(CurrentlySelectedTileType.object).
                            scriptName,
                        CurrentlySelectedTileType.owner.isEmpty() ? NO_ARMY :
                            country[CurrentlySelectedTileType.owner].turnOrder
                    );
                    FillWindow.close();
                }
            }
        },
        function(widgetName, signalName) {
            if (signalName == "Pressed") FillWindow.close();
        }
    );
}

/// Keeps track of the action control's signal state during the previous
/// iteration of the HandleInput() function.
bool prevAction = false;

/**
 * Handles input specific to the \c MapMakerMenu.
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void MapMakerMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
    // If there currently isn't a map loaded, or it is 0x0 tiles, then don't try
    // selecting any tiles or zooming in or out.
    if (edit is null) return;
    // If the BASE_GROUP is disabled, we can assume a MessageBox is open, so we
    // must prevent input until it is re-enabled.
    if (!getWidgetEnabled(BASE_GROUP)) return;

    bool mouseInMap = edit.map.getMapBoundingBox().contains(currentPosition);
    // Handle mouse input. Ignore the mouse if the game doesn't have focus.
	if (currentPosition != INVALID_MOUSE) {
		// Ignore the mouse if it's outside of the window.
		Vector2 windowSize = getWindowSize();
		if (currentPosition.x >= 0 && currentPosition.y >= 0
			&& currentPosition.x <= int(windowSize.x)
			&& currentPosition.y <= int(windowSize.y)) {
			// Only consider the mouse if it has moved.
			if (currentPosition != previousPosition) {
                edit.setSelectedTileByPixel(currentPosition);
			}
		}
	}

    // Only allow a mouse button to paint if the mouse is not hovering over a
    // widget. It's not perfect but it gets the job done.
    /* Case that doesn't work: if the mouse is hovering over a widget, the user
    paints with the keyboard buttons, and is issuing the paint mouse button at the
    same time. Actually, testing it now, that feels intuitive. And no one will be
    doing it anyway. */
    const bool mouseNotUnderWidget = getWidgetUnderMouse().isEmpty();
    const bool action = bool(controls["action"]) && (
        !bool(mouseInputs["action"]) || (mouseNotUnderWidget && mouseInMap)
    );
    const bool pick = bool(controls["pick"]) && (
        !bool(mouseInputs["pick"]) || (mouseNotUnderWidget && mouseInMap)
    );
    const bool tileinfo = bool(controls["tileinfo"]) && (
        !bool(mouseInputs["tileinfo"]) || (mouseNotUnderWidget && mouseInMap)
    );

    // Handle controls.
	if (bool(controls["up"])) {
		edit.moveSelectedTileUp();
	} else if (bool(controls["down"])) {
		edit.moveSelectedTileDown();
	} else if (bool(controls["left"])) {
		edit.moveSelectedTileLeft();
	} else if (bool(controls["right"])) {
		edit.moveSelectedTileRight();
	}
    if (bool(controls["zoomout"])) {
        edit.zoomOut();
    }
    if (bool(controls["zoomin"])) {
        edit.zoomIn();
    }

    // If there isn't a tile currently selected that is in bounds, return now.
    const auto curTile = edit.map.getSelectedTile();
    if (edit.map.isOutOfBounds(curTile)) return;
    const auto curUnit = edit.map.getUnitOnTile(curTile);

    // Get the currently selected tile and unit types.
    const auto tileTypeSel = cast<TileType>(CurrentlySelectedTileType.object);
    const auto tileOwnerSel = CurrentlySelectedTileType.owner;
    const auto unitTypeSel = cast<UnitType>(CurrentlySelectedUnitType.object);
    const auto unitArmySel = CurrentlySelectedUnitType.owner;
    const auto currentPaletteWindowTab = PaletteWindow.getSelectedTab();
    
    if (action && TOOLBAR.tool == PAINT_TOOL.shortName) {
        // If there isn't a currently selected tile type, do not try to paint with
        // it.
        if (currentPaletteWindowTab == TILE_DIALOG && tileTypeSel !is null)
            edit.setTile(curTile, tileTypeSel, tileOwnerSel);

        // If there isn't a currently selected unit type, do not try to paint with
        // it.
        if (currentPaletteWindowTab == UNIT_DIALOG && unitTypeSel !is null)
            edit.createUnit(curTile, unitTypeSel, unitArmySel);

    } else if (pick) {
        if (currentPaletteWindowTab == TILE_DIALOG) {
            CurrentlySelectedTileType.object = edit.map.getTileType(curTile);
            PaletteWindow.setSelectedOwner(edit.map.getTileOwner(curTile));
        } else if (currentPaletteWindowTab == UNIT_DIALOG && curUnit != 0) {
            CurrentlySelectedUnitType.object = edit.map.getUnitType(curUnit);
            PaletteWindow.setSelectedOwner(edit.map.getArmyOfUnit(curUnit));
        }

    } else if (action && TOOLBAR.tool == DELETE_TOOL.shortName) {
        edit.deleteUnit(curUnit);
    
    } else if (TOOLBAR.tool == RECT_TOOL.shortName) {
        if (!prevAction && action) {        // Start rectangle.
            edit.map.setRectangleSelectionStart(curTile);
        } else if (prevAction && action) {  // Continue rectangle.
            edit.map.setRectangleSelectionEnd(curTile);
        } else if (prevAction && !action) { // Finish rectangle.
            const auto start = edit.map.getRectangleSelectionStart();
            const auto end = edit.map.getRectangleSelectionEnd();
            edit.map.removeRectangleSelection();
            if (currentPaletteWindowTab == TILE_DIALOG && tileTypeSel !is null) {
                edit.rectangleFillTiles(start, end, tileTypeSel.scriptName,
                    tileOwnerSel.isEmpty() ? NO_ARMY :
                        country[tileOwnerSel].turnOrder);
            }
            if (currentPaletteWindowTab == UNIT_DIALOG && unitTypeSel !is null) {
                edit.rectangleFillUnits(start, end, unitTypeSel.scriptName,
                    country[unitArmySel].turnOrder);
            }
        }
    }

    prevAction = action;

    // If more detail on the current tile is desired, display its information.
    if (tileinfo) edit.selectTile(curTile);
}

/**
 * Show the currently open \c MessageBox.
 * This prevents the \c MessageBox from being lost in the event the game's window
 * is made smaller and the \c MessageBox was previously moved to outside of the
 * new screen's size.
 */
void MapMakerMenu_MessageBoxGroup_SizeChanged() {
    awe::ShowMessageBox();
}

/**
 * Code to perform after a quit operation.
 */
awe::EmptyCallback@ QuitCallback;

/**
 * If a quit operation wasn't cancelled, perform this code.
 */
awe::EmptyCallback@ QuitMapAuthorised = function() {
    quitMap();
    MapPropertiesDialog.close();
    FillWindow.close();
    @edit = null;
    TilePropertiesDialog.refresh(Vector2(0,0));
    ArmyPropertiesDialog.refresh();
    if (!(QuitCallback is null)) QuitCallback();
};

/**
 * Quits \c edit.
 * @param callback If the quit operation wasn't cancelled, or the quit did not
 *                 need to be carried out, perform this code.
 */
void quitEditMap(awe::EmptyCallback@ callback = null) {
    if (edit !is null) {
        @QuitCallback = callback;
        if (edit.map.hasChanged()) {
            awe::OpenMessageBox(SAVE_BEFORE_QUIT, "alert", "savebeforequit", null,
                BASE_GROUP, MESSAGE_BOX_GROUP);
            addMessageBoxButton(SAVE_BEFORE_QUIT, "yes");
            addMessageBoxButton(SAVE_BEFORE_QUIT, "no");
            addMessageBoxButton(SAVE_BEFORE_QUIT, "cancel");
        } else QuitMapAuthorised();
    } else callback();
}

/**
 * Close the message box once it has been acknowledged.
 * If "Yes" was pressed, save the map before quitting. If "No" was pressed, quit
 * the map without saving. If "Cancel" was pressed, don't do anything more.
 * @param btn The button that was pressed.
 */
void MapMakerMenu_SaveBeforeQuit_ButtonPressed(const uint64 btn) {
    awe::CloseMessageBox(SAVE_BEFORE_QUIT, MESSAGE_BOX_GROUP, BASE_GROUP);
    if (btn == 0) {
        edit.map.save();
        QuitMapAuthorised();
    } else if (btn == 1) {
        QuitMapAuthorised();
    }
}

/**
 * Handles menu item selection signals.
 * @param id The ID of the menu or menu item selected.
 */
void MapMakerMenu_Menu_MenuItemClicked(const MenuItemID id) {
    if (id == MAP_MAKER_FILE_NEW_MAP) {
        setWidgetEnabled(BASE_GROUP, false);
        moveWidgetToFront(FILE_DIALOG_GROUP);
        awe::OpenFileDialog(NEW_MAP, "newmap", "create", "./map", false);
        addFileDialogFileTypeFilter(NEW_MAP, "mapfiles", null, { "*.cwm" });
        setFileDialogDefaultFileFilter(NEW_MAP, 1);

    } else if (id == MAP_MAKER_FILE_OPEN_MAP) {
        setWidgetEnabled(BASE_GROUP, false);
        moveWidgetToFront(FILE_DIALOG_GROUP);
        awe::OpenFileDialog(OPEN_MAP, "openmap", "open", "./map");
        addFileDialogFileTypeFilter(OPEN_MAP, "mapfiles", null, { "*.cwm" });
        setFileDialogDefaultFileFilter(OPEN_MAP, 1);

    } else if (id == MAP_MAKER_FILE_SAVE_MAP) {
        if (edit !is null) edit.map.save();

    } else if (id == MAP_MAKER_FILE_SAVE_MAP_AS) {
        if (edit !is null) {
            setWidgetEnabled(BASE_GROUP, false);
            moveWidgetToFront(FILE_DIALOG_GROUP);
            awe::OpenFileDialog(SAVE_MAP, "savemapas", "save", "./map", false);
            addFileDialogFileTypeFilter(SAVE_MAP, "mapfiles", null, { "*.cwm" });
            setFileDialogDefaultFileFilter(SAVE_MAP, 1);
        }

    } else if (id == MAP_MAKER_FILE_QUIT) {
        quitEditMap(function() { setGUI("MainMenu"); });

    } else if (id == MAP_MAKER_MAP_SET_PROPS) {
        if (edit is null) {
            awe::OpenMessageBox(SIMPLE_MESSAGE_BOX, "alert", "nomapisopen", null,
                BASE_GROUP, MESSAGE_BOX_GROUP);
            addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
        } else MapPropertiesDialog.restore();

    } else if (id == MAP_MAKER_MAP_FILL) {
        if (edit is null) {
            awe::OpenMessageBox(SIMPLE_MESSAGE_BOX, "alert", "nomapisopen", null,
                BASE_GROUP, MESSAGE_BOX_GROUP);
            addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
        } else FillWindow.restore("fillmapconfirmation");

    } else if (id == MAP_MAKER_VIEW_TOOLBAR) {
        TOOLBAR.dock();

    } else if (id == MAP_MAKER_VIEW_OBJECT_DIALOG) {
        PaletteWindow.dock();

    } else if (id == MAP_MAKER_VIEW_TILE_PROPS) {
        TilePropertiesDialog.dock();

    } else if (id == MAP_MAKER_VIEW_ARMY_PROPS) {
        ArmyPropertiesDialog.restore();

    } else {
        error("Unrecognised menu item ID " + awe::formatMenuItemID(id) +
            " received in the Map Maker menu!");
    }
}

/**
 * Stores the selected file across signal handlers.
 */
string FileDialogFile;

/**
 * If an overwrite was authorised by the user, perform this code.
 */
awe::EmptyCallback@ FileDialogOverwrite;

/**
 * Closes the currently open map and creates a new one.
 */
void createNewMap() {
    quitEditMap(function() {
        TilePropertiesDialog.deselect();
        @edit = EditableMap(createMap(FileDialogFile), TilePropertiesDialog,
            ArmyPropertiesDialog);
        ArmyPropertiesDialog.refresh();
        MapPropertiesDialog.restore();
    });
}

/**
 * Open a new map for editting.
 */
void MapMakerMenu_NewMap_FileSelected() {
    FileDialogFile = getFileDialogSelectedPaths(NEW_MAP)[0];
    if (doesPathExist(FileDialogFile)) {
        awe::OpenMessageBox(FILE_ALREADY_EXISTS, "alert", "mapfilealreadyexists",
            {any(FileDialogFile)}, BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(FILE_ALREADY_EXISTS, "cancel");
        addMessageBoxButton(FILE_ALREADY_EXISTS, "ok");
        @FileDialogOverwrite = function(){ createNewMap(); };
    } else createNewMap();
}

/**
 * Always make sure the map maker is re-enabled when the file dialog closes.
 * But only when there isn't a message box open.
 * @param abort Never abort.
 */
void MapMakerMenu_NewMap_Closing(bool&out abort) {
    if (!getWidgetEnabled(MESSAGE_BOX_GROUP))
        setWidgetEnabled(BASE_GROUP, true);
}

/**
 * Open an existing map for editting.
 */
void MapMakerMenu_OpenMap_FileSelected() {
    FileDialogFile = getFileDialogSelectedPaths(OPEN_MAP)[0];
    quitEditMap(function() {
        TilePropertiesDialog.deselect();
        @edit = EditableMap(loadMap(FileDialogFile), TilePropertiesDialog,
            ArmyPropertiesDialog);
        ArmyPropertiesDialog.refresh();
    });
}

/**
 * Always make sure the map maker is re-enabled when the file dialog closes.
 * But only when there isn't a message box open.
 * @param abort Never abort.
 */
void MapMakerMenu_OpenMap_Closing(bool&out abort) {
    if (!getWidgetEnabled(MESSAGE_BOX_GROUP))
        setWidgetEnabled(BASE_GROUP, true);
}

/**
 * Save a map to a given location.
 */
void MapMakerMenu_SaveMap_FileSelected() {
    FileDialogFile = getFileDialogSelectedPaths(SAVE_MAP)[0];
    if (doesPathExist(FileDialogFile)) {
        awe::OpenMessageBox(FILE_ALREADY_EXISTS, "alert", "mapfilealreadyexists",
            {any(FileDialogFile)}, BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(FILE_ALREADY_EXISTS, "cancel");
        addMessageBoxButton(FILE_ALREADY_EXISTS, "ok");
        @FileDialogOverwrite = function(){ edit.map.save(FileDialogFile); };
    } else edit.map.save(FileDialogFile);
}

/**
 * Always make sure the map maker is re-enabled when the file dialog closes.
 * But only when there isn't a message box open.
 * @param abort Never abort.
 */
void MapMakerMenu_SaveMap_Closing(bool&out abort) {
    if (!getWidgetEnabled(MESSAGE_BOX_GROUP))
        setWidgetEnabled(BASE_GROUP, true);
}

/**
 * Close the message box once it has been acknowledged.
 * If OK is selected, trigger stored callback.
 * @param btn The button that was pressed.
 */
void MapMakerMenu_FileAlreadyExists_ButtonPressed(const uint64 btn) {
    awe::CloseMessageBox(FILE_ALREADY_EXISTS, MESSAGE_BOX_GROUP, BASE_GROUP);
    if (btn == 1 && !(FileDialogOverwrite is null)) FileDialogOverwrite();
}

/**
 * Closes the message box.
 * @param btn Ignored.
 */
void MapMakerMenu_SimpleMessageBox_ButtonPressed(const uint64 btn) {
    awe::CloseMessageBox(SIMPLE_MESSAGE_BOX, MESSAGE_BOX_GROUP, BASE_GROUP);
}
