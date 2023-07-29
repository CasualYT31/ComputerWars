/**
 * @file MapMakerMenu.as
 * The main map maker menu.
 */

/**
 * The map object that the \c MapMakerMenu interacts with.
 */
Map@ editmap;

const auto MENU = "Menu";
const auto CLIENT_AREA = "Main";

string TILE_DIALOG;

MenuItemID MAP_MAKER_FILE_NEW_MAP;
MenuItemID MAP_MAKER_FILE_QUIT;

MenuItemID MAP_MAKER_MAP_SET_PROPS;

MenuItemID MAP_MAKER_VIEW_TILE_DIALOG;

/**
 * Sets up the map maker menu.
 */
void MapMakerMenuSetUp() {
    addWidget("MenuBar", MENU);
    addWidget("Group", CLIENT_AREA);
    setWidgetSize(CLIENT_AREA, "100%", "100%-" +
        formatFloat(getWidgetFullSize(MENU).y) + "px");
    setWidgetPosition(CLIENT_AREA, "50%", "100%");
    setWidgetOrigin(CLIENT_AREA, 0.5f, 1.0f);

    // Menu bar.

    addMenu(MENU, "file");
    MAP_MAKER_FILE_NEW_MAP = addMenuItem(MENU, "newmap");
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
 * Handles menu item selection signals.
 * @param id The ID of the menu or menu item selected.
 */
void MapMakerMenu_Menu_MenuItemClicked(const MenuItemID id) {
    if (id == MAP_MAKER_FILE_NEW_MAP) {
        
    } else if (id == MAP_MAKER_FILE_QUIT) {
        if (!(editmap is null)) quitMap();
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
