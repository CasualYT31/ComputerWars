/**
 * @file MapMenu.as
 * Code that's run when the map's main command menu is shown.
 */

/**
 * The command menu widget.
 */
CommandWidget MapMenuCommands;

/**
 * Sets up the menu that is displayed when a user selects a vacant tile.
 */
void MapMenuSetUp() {
	MapMenuCommands = CommandWidget("Menu");
}

/**
 * Fills in the commands that are available based on the current state of the
 * game.
 */
void MapMenuOpen() {
	MapMenuCommands.addCommand("Save", "save", "saveicon");
	MapMenuCommands.addCommand("Delete", "delete", "deleteicon");
	if (game.map.tagCOIsPresent(game.map.getSelectedArmy())) {
		MapMenuCommands.addCommand("Tag", "tag", "tagicon");
	}
	MapMenuCommands.addCommand("EndTurn", "endturn", "endturnicon");
	MapMenuCommands.addCommand("Quit", "quit", "quiticon");
	
	setWidgetPosition(MapMenuCommands.layout, "50%", "10px");
	setWidgetOrigin(MapMenuCommands.layout, 0.5, 0.0);
}

/**
 * Removes all commands from the menu.
 */
void MapMenuClose() {
	MapMenuCommands.removeAllCommands();
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
 * Allows the user to delete units that belong to them.
 */
void MapMenu_Delete_Pressed() {
	setGUI("DeleteUnitMenu");
}

/**
 * Allows the current army to tag their COs and end their turn.
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