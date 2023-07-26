/**
 * @file MapMakerMenu.as
 * The main map maker menu.
 */

uint64 MAP_MAKER_QUIT;

/**
 * Sets up the map maker menu.
 */
void MapMakerMenuSetUp() {
    const auto MENU = "Menu";
    addWidget("MenuBar", MENU);

    addMenu(MENU, "file");
    addMenuItem(MENU, "new");
    MAP_MAKER_QUIT = addMenuItem(MENU, "quit");
}

/**
 * Handles menu item selection signals.
 * @param id The ID of the menu or menu item selected.
 */
void MapMakerMenu_Menu_MenuItemClicked(const uint64 id) {
    if (id == MAP_MAKER_QUIT) {
        setGUI("MainMenu");
    } else {
        error("Unrecognised menu item ID " + formatUInt(id) + " received in the "
            "Map Maker menu!");
    }
}
