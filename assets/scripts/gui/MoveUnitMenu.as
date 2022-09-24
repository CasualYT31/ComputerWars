void MoveUnitMenuSetUp() {}

void MoveUnitMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["back"])) {
		game.disableMoveMode();
		setGUI("Map");
		return;
	} else if (bool(controls["select"])) {
		if (game.getUnitOnTile(game.getSelectedTile()) == 0 ||
			game.getUnitOnTile(game.getSelectedTile()) == game.getMovingUnit()) {
			game.togglePreviewMoveMode(true);
			setGUI("PreviewMoveUnitMenu");
			return;
		}
	}
}