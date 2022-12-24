/**
 * @file ExplodePreviewMenu.as
 * Contains code which runs the menu that displays when a user is previewing an
 * explosion.
 */

/**
 * The HP of damage to deal in the explosion, in \em displayed format.
 */
HP EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 0;

/**
 * The range of the explosion.
 */
Vector2 EXPLODE_PREVIEW_MENU_RANGE(0, 0);

/**
 * Allow the user to move the cursor?
 */
bool EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = true;

/**
 * Defines the signature that the \c EXPLODE_PREVIEW_MENU_CALLBACK anonymous
 * function should take.
 */
funcdef void EXPLODE_PREVIEW_MENU_EXPLOSION_CALLBACK();

/**
 * Allows code that invokes this menu to call custom code after performing an
 * explosion.
 */
EXPLODE_PREVIEW_MENU_EXPLOSION_CALLBACK@ EXPLODE_PREVIEW_MENU_CALLBACK = null;

/**
 * Cache of the tile that was selected when the menu was opened.
 */
Vector2 EXPLODE_PREVIEW_MENU_SELECTED_TILE;

/**
 * This menu has no widgets.
 */
void ExplodePreviewMenuSetUp() {}

/**
 * When the menu is opened, push select the currently selected unit, add the tiles
 * in the configured range to the available list, and disable the closed list.
 */
void ExplodePreviewMenuOpen() {
	EXPLODE_PREVIEW_MENU_SELECTED_TILE = game.map.getSelectedTile();
	game.enableClosedList(false);
	game.setAttackCursorSprites();
	game.map.pushSelectedUnit(game.map.getSelectedUnit());
	game.map.setAvailableTileShader(AvailableTileShader::Red);
	// Fix the range vector if the values aren't in the correct fields.
	if (EXPLODE_PREVIEW_MENU_RANGE.x > EXPLODE_PREVIEW_MENU_RANGE.y) {
		uint copy = EXPLODE_PREVIEW_MENU_RANGE.y;
		EXPLODE_PREVIEW_MENU_RANGE.y = EXPLODE_PREVIEW_MENU_RANGE.x;
		EXPLODE_PREVIEW_MENU_RANGE.x = copy;
	}
	// Find all tiles within the range and add them to the available tile list.
	const auto availableTiles = game.map.getAvailableTiles(
		EXPLODE_PREVIEW_MENU_SELECTED_TILE, EXPLODE_PREVIEW_MENU_RANGE.x,
		EXPLODE_PREVIEW_MENU_RANGE.y);
	for (uint i = 0, len = availableTiles.length(); i < len; ++i) {
		game.map.addAvailableTile(availableTiles[i]);
	}
}

/**
 * When the menu is closed, reverse the effects made when the menu was opened.
 */
void ExplodePreviewMenuClose() {
	// TODO-2: for black bombs, who are deleted as they explode, popping the black
	// bomb here will attempt to reselect it, but as it's been deleted, a warning
	// will be logged, and the selected unit data will be reset. This is fine, but
	// I need to find a way to prevent the warning in some way in the future.
	game.map.popSelectedUnit();
	game.enableClosedList(true);
	game.setNormalCursorSprites();
	// As a safety precaution, reset the state of the global variables.
	EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 0;
	EXPLODE_PREVIEW_MENU_RANGE.x = 0;
	EXPLODE_PREVIEW_MENU_RANGE.y = 0;
	EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = true;
	@EXPLODE_PREVIEW_MENU_CALLBACK = null;
}

/**
 * Allows the user to change the selected tile, if configured to, and allows the
 * them to cancel the preview, or go ahead with the explosion.
 * @param controls The control map given by the engine.
 */
void ExplodePreviewMenuHandleInput(const dictionary controls) {
	if (EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE) {
		HandleCommonGameInput(controls);
	}
	if (bool(controls["back"])) {
		// Force the selection to go back to the originally selected unit (as the
		// currently selected tile could be moved whilst in this menu, and this
		// menu relies on the selected tile being on the original unit as the menu
		// opens).
		game.map.setSelectedTile(EXPLODE_PREVIEW_MENU_SELECTED_TILE);
		setGUI(PREVIOUS_MENU);
	} else if (bool(controls["select"])) {
		// Perform explosion.
		game.damageUnitsInRange(game.map.getSelectedTile(),
			EXPLODE_PREVIEW_MENU_RANGE.x, EXPLODE_PREVIEW_MENU_RANGE.y,
			EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL, { "OOZIUM" });
		// Perform any custom code, then switch back to the previous menu.
		if (EXPLODE_PREVIEW_MENU_CALLBACK is null) {
			setGUI("Map");
		} else {
			EXPLODE_PREVIEW_MENU_CALLBACK();
			setGUI("Map");
		}
	}
}
