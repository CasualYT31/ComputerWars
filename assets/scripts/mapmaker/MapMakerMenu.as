/**
 * @file MapMakerMenu.as
 * The main map maker menu.
 */

/**
 * The map object that the \c MapMakerMenu interacts with.
 */
Map@ editmap;

const auto BASE_GROUP = "BaseGroup";
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

MenuItemID MAP_MAKER_VIEW_TILE_DIALOG;
MenuItemID MAP_MAKER_VIEW_UNIT_DIALOG;

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
    MAP_MAKER_MAP_SET_PROPS = addMenuItem(MENU, "setmapprops");

    addMenu(MENU, "view");
    MAP_MAKER_VIEW_TILE_DIALOG = addMenuItem(MENU, "tiledialog");
    MAP_MAKER_VIEW_UNIT_DIALOG = addMenuItem(MENU, "unitdialog");

    // Dialogs.
    
    TileDialog.setUp(DefaultTileDialogData(CLIENT_AREA));

    UnitDialog.setUp(DefaultUnitDialogData(CLIENT_AREA));

    // Map properties child window.

    MapPropertiesSetUp(CLIENT_AREA);
}

/**
 * The map scaling factor.
 */
float MapMakerScalingFactor = 2.0f;

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
    if (editmap is null) return;

    bool mouseInMap = editmap.getMapBoundingBox().contains(currentPosition);
    // Handle mouse input. Ignore the mouse if the game doesn't have focus.
	if (currentPosition != INVALID_MOUSE) {
		// Ignore the mouse if it's outside of the window.
		Vector2 windowSize = getWindowSize();
		if (currentPosition.x >= 0 && currentPosition.y >= 0
			&& currentPosition.x <= int(windowSize.x)
			&& currentPosition.y <= int(windowSize.y)) {
			// Only consider the mouse if it has moved.
			if (currentPosition != previousPosition) {
                editmap.setSelectedTileByPixel(currentPosition);
			}
		}
	}

    // Handle controls.
	if (bool(controls["up"])) {
		editmap.moveSelectedTileUp();
	} else if (bool(controls["down"])) {
		editmap.moveSelectedTileDown();
	} else if (bool(controls["left"])) {
		editmap.moveSelectedTileLeft();
	} else if (bool(controls["right"])) {
		editmap.moveSelectedTileRight();
	}
	if (bool(controls["zoomout"])) {
        MapMakerScalingFactor -= 1.0f;
        if (MapMakerScalingFactor < 1.0f) MapMakerScalingFactor = 1.0f;
		editmap.setMapScalingFactor(MapMakerScalingFactor);
	}
	if (bool(controls["zoomin"])) {
        MapMakerScalingFactor += 1.0f;
        if (MapMakerScalingFactor > 5.0f) MapMakerScalingFactor = 5.0f;
		editmap.setMapScalingFactor(MapMakerScalingFactor);
	}

    // If there isn't a tile currently selected that is in bounds, return now.
    const auto curTile = editmap.getSelectedTile();
    if (editmap.isOutOfBounds(curTile)) return;
    const auto curTileType = editmap.getTileType(curTile);
    const auto curTileOwner = editmap.getTileOwner(curTile);
    const auto curTileUnit = editmap.getUnitOnTile(curTile);
    const auto curTileUnitType = curTileUnit == 0 ? null :
        editmap.getUnitType(curTileUnit);
    const auto curTileUnitArmy = curTileUnit == 0 ? NO_ARMY :
        editmap.getArmyOfUnit(curTileUnit);

    const auto tileTypeSel = cast<TileType>(CurrentlySelectedTileType.object);
    const auto tileOwnerSel = CurrentlySelectedTileType.owner;
    ArmyID tileOwnerSelID = NO_ARMY;
    if (!tileOwnerSel.isEmpty()) tileOwnerSelID = country[tileOwnerSel].turnOrder;

    const auto unitTypeSel = cast<UnitType>(CurrentlySelectedUnitType.object);
    const auto unitArmySel = CurrentlySelectedUnitType.owner;
    ArmyID unitArmySelID = NO_ARMY;
    if (!unitArmySel.isEmpty()) unitArmySelID = country[unitArmySel].turnOrder;

    // Only allow a mouse button to paint if the mouse is not hovering over a
    // widget. It's not perfect but it gets the job done.
    /* Case that doesn't work: if the mouse is hovering over a widget, the user
    paints with the keyboard buttons, and is issuing the paint mouse button at the
    same time. Actually, testing it now, that feels intuitive. And no one will be
    doing it anyway. */
    const bool mouseNotUnderWidget = getWidgetUnderMouse().isEmpty();
    const bool paintTile = bool(controls["painttile"]) && (
        !bool(mouseInputs["painttile"]) || (mouseNotUnderWidget && mouseInMap)
    );
    const bool paintUnit = bool(controls["paintunit"]) && (
        !bool(mouseInputs["paintunit"]) || (mouseNotUnderWidget && mouseInMap)
    );

    // If there isn't a currently selected tile type, do not paint with it.
    if (paintTile && tileTypeSel !is null) {
        if (curTileType.scriptName != tileTypeSel.scriptName) {
            editmap.setTileType(curTile, tileTypeSel.scriptName);
        }
        if (curTileOwner != tileOwnerSelID) {
            editmap.setTileOwner(curTile, tileOwnerSelID);
        }
    }
    // If there isn't a currently selected unit type, do not paint with it.
    if (paintUnit && unitTypeSel !is null) {
        if (curTileUnitType is null ||
            curTileUnitType.scriptName != unitTypeSel.scriptName ||
            curTileUnitArmy != unitArmySelID) {
            // This logic really needs consolidating into an EditableMap class.
            if (curTileUnitType !is null) {
                editmap.deleteUnit(curTileUnit);
                if (editmap.getUnitsOfArmy(curTileUnitArmy).isEmpty()) {
                    // If that was the last unit belonging to an army, delete the
                    // army, too.
                    editmap.deleteArmy(curTileUnitArmy);
                }
            }
            // If the army doesn't exist yet, create it.
            if (!editmap.isArmyPresent(unitArmySelID)) {
                editmap.createArmy(unitArmySel);
            }
            const auto newUnit =
                editmap.createUnit(unitTypeSel.scriptName, unitArmySelID);
		    editmap.setUnitPosition(newUnit, curTile);
            editmap.replenishUnit(newUnit, true);
            editmap.waitUnit(newUnit, false);
        }
    }
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
    CloseMapProperties();
    @editmap = null;
    if (!(QuitCallback is null)) QuitCallback();
};

/**
 * Quits \c editmap.
 * @param callback If the quit operation wasn't cancelled, or the quit did not
 *                 need to be carried out, perform this code.
 */
void quitEditMap(awe::EmptyCallback@ callback = null) {
    if (!(editmap is null)) {
        @QuitCallback = callback;
        if (editmap.hasChanged()) {
            // Save? Or not? Or cancel?
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
        editmap.save();
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
        if (!(editmap is null)) editmap.save();

    } else if (id == MAP_MAKER_FILE_SAVE_MAP_AS) {
        if (!(editmap is null)) {
            setWidgetEnabled(BASE_GROUP, false);
            moveWidgetToFront(FILE_DIALOG_GROUP);
            awe::OpenFileDialog(SAVE_MAP, "savemapas", "save", "./map", false);
            addFileDialogFileTypeFilter(SAVE_MAP, "mapfiles", null, { "*.cwm" });
            setFileDialogDefaultFileFilter(SAVE_MAP, 1);
        }

    } else if (id == MAP_MAKER_FILE_QUIT) {
        quitEditMap(function() { setGUI("MainMenu"); });

    } else if (id == MAP_MAKER_MAP_SET_PROPS) {
        OpenMapProperties();

    } else if (id == MAP_MAKER_VIEW_TILE_DIALOG) {
        TileDialog.dock();

    } else if (id == MAP_MAKER_VIEW_UNIT_DIALOG) {
        UnitDialog.dock();

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
        @editmap = createMap(FileDialogFile);
        editmap.alwaysShowHiddenUnits(true);
        editmap.setMapScalingFactor(MapMakerScalingFactor);
		editmap.setULCursorSprite("ulcursor");
		editmap.setURCursorSprite("urcursor");
		editmap.setLLCursorSprite("llcursor");
		editmap.setLRCursorSprite("lrcursor");
        OpenMapProperties();
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
        @editmap = loadMap(FileDialogFile);
        editmap.alwaysShowHiddenUnits(true);
        editmap.setMapScalingFactor(MapMakerScalingFactor);
		editmap.setULCursorSprite("ulcursor");
		editmap.setURCursorSprite("urcursor");
		editmap.setLLCursorSprite("llcursor");
		editmap.setLRCursorSprite("lrcursor");
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
        @FileDialogOverwrite = function(){ editmap.save(FileDialogFile); };
    } else editmap.save(FileDialogFile);
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
