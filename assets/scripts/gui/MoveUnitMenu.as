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
		const auto otherUnit = game.map.getUnitOnTile(game.map.getSelectedTile());
		if (otherUnit == 0 || otherUnit == game.map.getSelectedUnit() ||
			game.canJoin(otherUnit, game.map.getSelectedUnit()) ||
			game.canLoad(game.map.getSelectedUnit(), otherUnit)) {
			game.map.disableSelectedUnitRenderingEffects(true);
			game.map.addPreviewUnit(game.map.getSelectedUnit(),
				game.map.getSelectedTile());
			setGUI("PreviewMoveUnitMenu");
			return;
		}
	}
}