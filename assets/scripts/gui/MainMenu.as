/**
 * This version of the main menu simply lists the map files currently
 * stored in the assets folder.
 * It also allows the user to supply game options (but only when playing
 * on Island X), via the \c NewGameOptions menu.
 */
void MainMenuSetUp() {
	setGlobalFont("AW2");
    setDirectionalFlowAngleBracketSprite("UL", "icon", "ulanglebracket");
    setDirectionalFlowAngleBracketSprite("UR", "icon", "uranglebracket");
    setDirectionalFlowAngleBracketSprite("LL", "icon", "llanglebracket");
    setDirectionalFlowAngleBracketSprite("LR", "icon", "lranglebracket");

	setBackground("", 200, 200, 200, 0);
	addWidget("ListBox", "FileSelect");
	setWidgetSize("FileSelect", "50%", "50%");
	setWidgetPosition("FileSelect", "25%", "50%");
	setWidgetOrigin("FileSelect", 0.25, 0.5);
	addWidget("Button", "NewGame");
	setWidgetPosition("NewGame", "10%", "10%");
	setWidgetText("NewGame", "~New Game");
	addWidget("Button", "MapMaker");
	setWidgetPosition("MapMaker", "90%", "10%");
	setWidgetOrigin("MapMaker", 1.0, 0.0);
	setWidgetText("MapMaker", "~Map Maker");

    setWidgetDirectionalFlow(
        "NewGame", "FileSelect", "FileSelect", "MapMaker", "MapMaker");
    setWidgetDirectionalFlow(
        "MapMaker", "FileSelect", "FileSelect", "NewGame", "NewGame");
    setWidgetDirectionalFlow("FileSelect", "~", "", "NewGame", "MapMaker");
    setWidgetDirectionalFlowStart("NewGame");
}

/**
 * Every time the main menu is opened, the map file list will be
 * updated.
 */
void MainMenuOpen() {
	// Collect a list of available map files and add them to the listbox.
	clearItems("FileSelect");
	filesystem cwd;
	cwd.changeCurrentPath("./map");
	array<string> files = cwd.getFiles();
	for (uint f = 0; f < files.length(); ++f) {
		// Only look for *.cwm files!
		if (files[f].findLast(".") >= 0) {
			if (files[f].substr(files[f].findLast(".")) == ".cwm") {
				addItem("FileSelect", "~" + files[f]);
			}
		}
	}
}

/**
 * Global point of access to the map in-play.
 */
PlayableMap game;

/**
 * Loads a map for play.
 * The map was loaded successfully, the \c Map menu will be opened.
 * @param  file The path of the map file to load.
 * @return The name of the menu to set, if the load was successful. If the load
 *         was unsuccessful, an empty string will be returned.
 */
string PlayMap(const string&in file) {
	game = PlayableMap(loadMap(file));
	if (game.map is null) {
		error("Failed to load map, will not switch menus.");
		return "";
	} else {
		return "Map";
	}
}

/**
 * When a file is selected from the list, the map file will be loaded.
 * If the map could be loaded successfully, then switch to the \c Map
 * menu.
 */
void MainMenu_FileSelect_MouseReleased() {
	const string menu = PlayMap("map/" + getSelectedItemText("FileSelect"));
	if (!menu.isEmpty()) setGUI(menu);
}

/**
 * Go to the \c NewGameOptions menu when the user clicks the right
 * button.
 */
void MainMenu_NewGame_MouseReleased() {
	setGUI("NewGameOptions");
}

/**
 * Opens the map maker.
 */
void MainMenu_MapMaker_MouseReleased() {
	setGUI("MapMakerMenu");
}
