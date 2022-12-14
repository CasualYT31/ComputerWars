/**
 * The \c DeleteUnitMenu has no widgets.
 */
void DeleteUnitMenuSetUp() {}

/**
 * When the menu is opened, the cursor graphic will change.
 */
void DeleteUnitMenuOpen() {
	game.setDeleteCursorSprites();
}

/**
 * When the menu is closed, the cursor graphic will revert back to normal.
 */
void DeleteUnitMenuClose() {
	game.setNormalCursorSprites();
}

/**
 * If the user selects one of their own units, it will be deleted.
 * @param controls The map of controls given by the engine.
 */
void DeleteUnitMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	auto unitID = game.map.getUnitOnTile(game.map.getSelectedTile());
	if (bool(controls["select"]) && unitID > 0 &&
		game.map.getArmyOfUnit(unitID) == game.map.getSelectedArmy()) {
		game.map.deleteUnit(unitID);
	} else if (bool(controls["back"])) {
		setGUI("Map");
	}
}
