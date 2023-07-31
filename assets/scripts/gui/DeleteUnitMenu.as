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
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void DeleteUnitMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
	HandleCommonGameInput(controls, mouseInputs, previousPosition,
        currentPosition);
	auto unitID = game.map.getUnitOnTile(game.map.getSelectedTile());
	if (bool(controls["select"]) && unitID > 0 &&
		game.map.getArmyOfUnit(unitID) == game.map.getSelectedArmy()) {
        // If the select control is being made by the mouse, and it is not inside
        // the map's graphic, then drop it.
        if (bool(mouseInputs["select"]) &&
            !game.map.getMapBoundingBox().contains(currentPosition)) return;

		if (game.deleteUnit(unitID)) {
			setGUI("Map");
		}
	} else if (bool(controls["back"])) {
		setGUI("Map");
	}
}
