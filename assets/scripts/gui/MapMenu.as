/**
 * Sets up the menu that is displayed when a user selects a vacant tile.
 */
void MapMenuSetUp() {
	addWidget("VerticalLayout", "Menu");
}

void MapMenuOpen() {
	addWidget("BitmapButton", "MapMenu.Menu.Save");
	setWidgetSprite("MapMenu.Menu.Save", "icon", "saveicon");
	setWidgetText("MapMenu.Menu.Save", "save");

	if (game.map.tagCOIsPresent(game.map.getSelectedArmy())) {
		addWidget("BitmapButton", "MapMenu.Menu.Tag");
		setWidgetSprite("MapMenu.Menu.Tag", "icon", "tagicon");
		setWidgetText("MapMenu.Menu.Tag", "tag");
	}

	addWidget("BitmapButton", "MapMenu.Menu.EndTurn");
	setWidgetSprite("MapMenu.Menu.EndTurn", "icon", "endturnicon");
	setWidgetText("MapMenu.Menu.EndTurn", "endturn");

	addWidget("BitmapButton", "MapMenu.Menu.Quit");
	setWidgetSprite("MapMenu.Menu.Quit", "icon", "quiticon");
	setWidgetText("MapMenu.Menu.Quit", "quit");
	
	string height = formatUInt(getWidgetCount("MapMenu.Menu") * 25) + "px";
	setWidgetSize("MapMenu.Menu", "25%", height);
	setWidgetPosition("MapMenu.Menu", "50%", "10px");
	setWidgetOrigin("MapMenu.Menu", 0.5, 0.0);
}

void MapMenuClose() {
	removeWidgetsFromContainer("MapMenu.Menu");
}

/**
 * Used to allow the user to shut down the menu.
 * @param controls Control map given by the engine.
 */
void MapMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		setGUI("Map");
	}
}

/**
 * Allows the user to save the map in its current state.
 */
void MapMenu_Save_Pressed() {
	game.map.save();
	setGUI("Map");
}

/**
 *
 */
void MapMenu_Tag_Pressed() {
	game.tagCOs();
	setGUI("Map");
}

/**
 * Allows the current army to end their turn.
 */
void MapMenu_EndTurn_Pressed() {
	game.endTurn();
	setGUI("Map");
}

/**
 * Quits the map without saving and goes back to the menu the user was on before
 * loading the map.
 */
void MapMenu_Quit_Pressed() {
	quitMap();
}