/**
 * @file SelectTargetMenu.as
 * Code that's run when the user is selecting a unit or tile to attack.
 */

/**
 * Sets up the damage tooltip which will display base damage against targets.
 */
void SelectTargetMenuSetUp() {
}

/**
 * A set of tiles that contain targets.
 */
dictionary TILES_WITH_TARGETS;

/**
 * Cache of the tile that was selected when the menu was opened.
 */
Vector2 SELECT_TARGET_MENU_SELECTED_TILE;

/**
 * When the menu is opened, the currently selected unit will be push selected
 * again, and its available targets will be added to the available tiles list.
 */
void SelectTargetMenuOpen() {
	SELECT_TARGET_MENU_SELECTED_TILE = game.map.getSelectedTile();
	TILES_WITH_TARGETS.deleteAll();
	game.findTilesWithTargets(TILES_WITH_TARGETS, game.map.getSelectedUnit(),
		SELECT_TARGET_MENU_SELECTED_TILE);
	game.map.pushSelectedUnit(game.map.getSelectedUnit());
	const auto keys = TILES_WITH_TARGETS.getKeys();
	for (uint i = 0, len = keys.length(); i < len; ++i) {
		game.map.addAvailableTile(Vector2(keys[i]));
	}
	game.map.setAvailableTileShader(AvailableTileShader::Red);
	// Deal with closed list... We won't need it for this menu, but it's causing a
	// fair amount of exceptions in _updateMoveModeClosedList().
}

/**
 * Always ensure to pop the attack unit data.
 */
void SelectTargetMenuClose() {
	game.map.popSelectedUnit();
	// Force the selection to go back to the originally selected unit (as the
	// currently selected tile could be moved whilst in this menu, and this menu
	// relies on the selected tile being on the original unit as the menu opens).
	game.map.setSelectedTile(SELECT_TARGET_MENU_SELECTED_TILE);
}

/**
 * Allows the user to move the selection, as well as cancel the attack.
 * @param controls The control map given by the engine.
 */
void SelectTargetMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["back"])) {
		setGUI("PreviewMoveUnitMenu");
	}
}
