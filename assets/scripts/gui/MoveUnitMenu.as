/**
 * The menu that is displayed when a user has selected a unit is completely empty.
 */
void MoveUnitMenuSetUp() {}

/**
 * Allows the user to move their selection whilst moving a unit.
 * Also allows the user to cancel moving a unit, or allows them to preview a move.
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void MoveUnitMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
	HandleCommonGameInput(controls, mouseInputs, previousPosition,
        currentPosition);
	if (bool(controls["back"])) {
		game.selectUnit(0);
		setGUI("Map");
		return;
	} else if (bool(controls["select"])) {
		// If the select control is being made by the mouse, and it is not inside
        // the map's graphic, then drop it.
        if (bool(mouseInputs["select"]) &&
            !game.map.getMapBoundingBox().contains(currentPosition)) return;

		// Only allow selection if an available tile was selected.
		const auto selectedTile = game.map.getSelectedTile();
		const auto closedListLength = game.map.closedList.length();
		for (uint i = 0; i < closedListLength; ++i) {
			if (game.map.closedList[i].tile == selectedTile) {
				const auto unitID = game.map.getSelectedUnit();
				const auto otherUnit = game.map.getUnitOnTile(selectedTile);
				if (otherUnit == 0 || otherUnit == unitID ||
					game.canJoin(otherUnit, unitID) ||
					game.canLoad(unitID, otherUnit) ||
					!game.map.isUnitVisible(otherUnit,
						game.map.getArmyOfUnit(unitID))) {
					const auto obstructionIndex =
						game.map.scanPath(game.map.closedList, unitID);
					if (obstructionIndex >= 0) {
						// Trim closed list and move unit.
						game.map.closedList.removeRange(obstructionIndex,
							game.map.closedList.length() - obstructionIndex);
						game.moveUnit();
						setGUI("Map");
					} else {
						game.map.disableSelectedUnitRenderingEffects(true);
						game.map.addPreviewUnit(unitID, selectedTile);
						setGUI("PreviewMoveUnitMenu");
					}
					return;
				}
			}
		}
	}
}
