/**
 * @file SelectTargetMenu.as
 * Code that's run when the user is selecting a unit or tile to attack.
 */

/**
 * In the future, the damage tooltip will be widgets created here.
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
 * Cache of the handle which points to the type of the attacking unit.
 */
const UnitType@ ATTACKING_UNIT_TYPE;

/**
 * When the menu is opened, the currently selected unit will be push selected
 * again, and its available targets will be added to the available tiles list.
 */
void SelectTargetMenuOpen() {
	game.enableClosedList(false);
	SELECT_TARGET_MENU_SELECTED_TILE = game.map.getSelectedTile();
	TILES_WITH_TARGETS.deleteAll();
	@ATTACKING_UNIT_TYPE = game.map.getUnitType(game.map.getSelectedUnit());
	game.findTilesWithTargets(TILES_WITH_TARGETS, game.map.getSelectedUnit(),
		SELECT_TARGET_MENU_SELECTED_TILE);
	game.map.pushSelectedUnit(game.map.getSelectedUnit());
	const auto keys = TILES_WITH_TARGETS.getKeys();
	for (uint i = 0, len = keys.length(); i < len; ++i) {
		game.map.addAvailableTile(Vector2(keys[i]));
	}
	game.map.setAvailableTileShader(AvailableTileShader::Red);
	game.map.disableShaderForAvailableUnits(true);
	game.setAttackCursorSprites();
}

/**
 * Always ensure to pop the attack unit data and re-enable the closed list.
 */
void SelectTargetMenuClose() {
	// Always ensure the damage tooltip is hidden before leaving the menu.
	game.map.TOOLTIP_visible(false);
	game.enableClosedList(true);
	game.setNormalCursorSprites();
}

/**
 * Allows the user to move the selection, as well as cancel the attack.
 * @param controls The control map given by the engine.
 */
void SelectTargetMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);

	// If the currently selected tile is hovering over a target, find the base
	// damage the selected weapon deals to the target and update the damage
	// tooltip.
	const auto selectedTile = game.map.getSelectedTile();
	if (game.map.isAvailableTile(selectedTile)) {
		string weaponName;
		TILES_WITH_TARGETS.get(selectedTile.toString(), weaponName);
		int baseDamage = game.calculateDamage(game.map.getSelectedUnit(),
			weaponName, selectedTile, true);
		game.map.TOOLTIP_setDamage(baseDamage);
		game.map.TOOLTIP_visible(true);
	} else {
		game.map.TOOLTIP_visible(false);
	}

	if (bool(controls["back"])) {
		game.map.popSelectedUnit();
		// Force the selection to go back to the originally selected unit (as the
		// currently selected tile could be moved whilst in this menu, and this
		// menu relies on the selected tile being on the original unit as the menu
		// opens).
		game.map.setSelectedTile(SELECT_TARGET_MENU_SELECTED_TILE);
		setGUI("PreviewMoveUnitMenu");
	} else if (bool(controls["select"])) {
		// Ignore the selection if a non-available tile was selected.
		if (game.map.isAvailableTile(selectedTile)) {
			game.map.popSelectedUnit();
			// Perform move operation.
			const auto attackingUnit = game.map.getSelectedUnit();
			game.moveUnit();
			// Perform attack.
			game.attack(TILES_WITH_TARGETS, attackingUnit, selectedTile);
			setGUI("Map");
		}
	}
}
