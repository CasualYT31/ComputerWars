/**
 * The menu that is displayed when a user has selected a unit is completely empty.
 */
void MoveUnitMenuSetUp() {}

/**
 * Allows the user to move their selection whilst moving a unit.
 * Also allows the user to cancel moving a unit, or allows them to preview a move.
 * @param controls The control map given by the engine.
 */
void MoveUnitMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["back"])) {
		game.selectUnit(0);
		setGUI("Map");
		return;
	} else if (bool(controls["select"])) {
		const auto unitID = game.map.getSelectedUnit();
		const auto otherUnit = game.map.getUnitOnTile(game.map.getSelectedTile());
		if (otherUnit == 0 || otherUnit == unitID ||
			game.canJoin(otherUnit, unitID) || game.canLoad(unitID, otherUnit) ||
			!game.map.isUnitVisible(otherUnit, game.map.getArmyOfUnit(unitID))) {
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
				game.map.addPreviewUnit(unitID, game.map.getSelectedTile());
				setGUI("PreviewMoveUnitMenu");
			}
			return;
		}
	}
}