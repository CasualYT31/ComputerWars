/**
 * @file SelectTargetMenu.as
 * Code that's run when the user is selecting a unit or tile to attack.
 */

/**
 * The damage label widget.
 */
DamageWidget damageWidget;

/**
 * Sets up the damage label widget.
 */
void SelectTargetMenuSetUp() {
    damageWidget = DamageWidget("SelectTargetMenu");
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
	game.enableClosedList(true);
	game.setNormalCursorSprites();
}

/**
 * Allows the user to move the selection, as well as cancel the attack.
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void SelectTargetMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
	HandleCommonGameInput(controls, mouseInputs, previousPosition,
        currentPosition);
    game.updateDamageWidgetPosition(damageWidget);

	// If the currently selected tile is hovering over a target, find the base
	// damage the selected weapon deals to the target and update the damage label.
	const auto selectedTile = game.map.getSelectedTile();
	if (game.map.isAvailableTile(selectedTile)) {
		string weaponName;
		TILES_WITH_TARGETS.get(selectedTile.toString(), weaponName);
		int baseDamage = game.calculateDamage(game.map.getSelectedUnit(),
			weaponName, selectedTile, true);
		damageWidget.setDamage(baseDamage);
		damageWidget.setVisibility(true);
	} else {
		damageWidget.setVisibility(false);
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
		// If the select control is being made by the mouse, and it is not inside
        // the map's graphic, then drop it.
        if (bool(mouseInputs["select"]) &&
            !game.map.getMapBoundingBox().contains(currentPosition)) return;

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
