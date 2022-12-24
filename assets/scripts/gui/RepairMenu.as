/**
 * @file RepairMenu.as
 * Defines the menu which allows the user to select a unit to repair.
 */

/**
 * This menu has no widgets.
 */
void RepairMenuSetUp() {}

/**
 * Cache of the tile that was selected when the menu was opened.
 */
Vector2 REPAIR_MENU_SELECTED_TILE;

/**
 * When this menu is opened, push select the current unit (Black Boat), and add
 * the available units to the available tile list.
 * Also disable the closed list.
 */
void RepairMenuOpen() {
	REPAIR_MENU_SELECTED_TILE = game.map.getSelectedTile();
	game.enableClosedList(false);
	const auto selectedUnit = game.map.getSelectedUnit();
	game.map.pushSelectedUnit(selectedUnit);
	game.map.setAvailableTileShader(AvailableTileShader::Yellow);
	game.map.disableShaderForAvailableUnits(true);
	const auto availableUnits = game.findDamagedOrDepletedArmyUnitsAdjacentTo(
		REPAIR_MENU_SELECTED_TILE, game.map.getArmyOfUnit(selectedUnit),
		{ selectedUnit });
	for (uint i = 0, len = availableUnits.length(); i < len; ++i) {
		game.map.addAvailableTile(game.map.getUnitPosition(availableUnits[i]));
	}
}

/**
 * Always ensure the closed list is re-enabled when closing the menu.
 */
void RepairMenuClose() {
	game.enableClosedList(true);
}

/**
 * Allows the user to cancel the repair, or select a unit to repair.
 * @param controls The control map given by the engine.
 */
void RepairMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["back"])) {
		game.map.popSelectedUnit();
		// Force the selection to go back to the originally selected unit (as the
		// currently selected tile could be moved whilst in this menu, and this
		// menu relies on the selected tile being on the original unit as the menu
		// opens).
		game.map.setSelectedTile(REPAIR_MENU_SELECTED_TILE);
		setGUI("PreviewMoveUnitMenu");
	} else if (bool(controls["select"])) {
		if (game.map.isAvailableTile(game.map.getSelectedTile())) {
			// Move black boat unit.
			const auto blackBoatsArmy =
				game.map.getArmyOfUnit(game.map.getSelectedUnit());
			game.map.popSelectedUnit();
			game.moveUnit();
			// Perform repair and replenish.
			const auto unitToRepair =
				game.map.getUnitOnTile(game.map.getSelectedTile());
			game.replenishUnit(unitToRepair);
			game.healUnit(unitToRepair, 1, blackBoatsArmy);
			// Go back to the map menu.
			setGUI("Map");
		}
	}
}
