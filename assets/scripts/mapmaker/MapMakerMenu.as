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

string TILE_DIALOG;

MenuItemID MAP_MAKER_FILE_NEW_MAP;
MenuItemID MAP_MAKER_FILE_SAVE_MAP;
MenuItemID MAP_MAKER_FILE_SAVE_MAP_AS;
MenuItemID MAP_MAKER_FILE_QUIT;

MenuItemID MAP_MAKER_MAP_SET_PROPS;

MenuItemID MAP_MAKER_VIEW_TILE_DIALOG;

/**
 * Sets up the map maker menu.
 */
void MapMakerMenuSetUp() {
    // MessageBox group.

    addWidget("Group", MESSAGE_BOX_GROUP);

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
    MAP_MAKER_FILE_SAVE_MAP = addMenuItem(MENU, "savemap");
    MAP_MAKER_FILE_SAVE_MAP_AS = addMenuItem(MENU, "savemapas");
    MAP_MAKER_FILE_QUIT = addMenuItem(MENU, "quit");

    addMenu(MENU, "map");
    MAP_MAKER_MAP_SET_PROPS = addMenuItem(MENU, "setmapprops");

    addMenu(MENU, "view");
    MAP_MAKER_VIEW_TILE_DIALOG = addMenuItem(MENU, "tiledialog");

    // Dialogs.
    
    TILE_DIALOG = TileDialogSetUp(CLIENT_AREA);

    // Map properties child window.

    MapPropertiesSetUp(CLIENT_AREA);
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
 * Quits \c editmap.
 */
void quitEditMap() {
    if (!(editmap is null)) {
        quitMap();
        @editmap = null;
    }
}

/**
 * Handles menu item selection signals.
 * @param id The ID of the menu or menu item selected.
 */
void MapMakerMenu_Menu_MenuItemClicked(const MenuItemID id) {
    if (id == MAP_MAKER_FILE_NEW_MAP) {
        awe::OpenFileDialog("NewMap", "newmap", "create", "./map", false);
        addFileDialogFileTypeFilter("NewMap", "mapfiles", null, { "*.cwm" });
        setFileDialogDefaultFileFilter("NewMap", 1);
    } else if (id == MAP_MAKER_FILE_SAVE_MAP) {
        if (!(editmap is null)) editmap.save();
    } else if (id == MAP_MAKER_FILE_SAVE_MAP_AS) {
        if (!(editmap is null)) {
            awe::OpenFileDialog("SaveMap", "savemapas", "save", "./map", false);
            addFileDialogFileTypeFilter("SaveMap", "mapfiles", null, { "*.cwm" });
            setFileDialogDefaultFileFilter("SaveMap", 1);
        }
    } else if (id == MAP_MAKER_FILE_QUIT) {
        quitEditMap();
        setGUI("MainMenu");
    } else if (id == MAP_MAKER_MAP_SET_PROPS) {
        OpenMapProperties();
    } else if (id == MAP_MAKER_VIEW_TILE_DIALOG) {
        DockTileDialog(TILE_DIALOG);
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
    quitEditMap();
    @editmap = createMap(FileDialogFile);
    OpenMapProperties();
}

/**
 * Open a new map for editting.
 */
void MapMakerMenu_NewMap_FileSelected() {
    FileDialogFile = getFileDialogSelectedPaths("NewMap")[0];
    if (doesPathExist(FileDialogFile)) {
        awe::OpenMessageBox(FILE_ALREADY_EXISTS, "alert", "mapfilealreadyexists",
            {any(FileDialogFile)}, BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(FILE_ALREADY_EXISTS, "cancel");
        addMessageBoxButton(FILE_ALREADY_EXISTS, "ok");
        @FileDialogOverwrite = function(){ createNewMap(); };
    } else createNewMap();
}

/**
 * Save a map to a given location.
 */
void MapMakerMenu_SaveMap_FileSelected() {
    FileDialogFile = getFileDialogSelectedPaths("SaveMap")[0];
    if (doesPathExist(FileDialogFile)) {
        awe::OpenMessageBox(FILE_ALREADY_EXISTS, "alert", "mapfilealreadyexists",
            {any(FileDialogFile)}, BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(FILE_ALREADY_EXISTS, "cancel");
        addMessageBoxButton(FILE_ALREADY_EXISTS, "ok");
        @FileDialogOverwrite = function(){ editmap.save(FileDialogFile); };
    } else editmap.save(FileDialogFile);
}

/**
 * Close the message box once it has been acknowledged.
 * If OK is selected, overwrite selected file.
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
