void MapSetUp() {}

void MapHandleInput(const dictionary controls) {
	if (bool(controls["select"])) {
		Vector2 sel = game.getSelectedTile();
		info(formatInt(sel.x));
		if (game.getUnitOnTile(sel) == 0) {
			setGUI("MapMenu");
		}
	} else if (bool(controls["up"])) {
		game.moveSelectedTileUp();
	} else if (bool(controls["down"])) {
		game.moveSelectedTileDown();
	} else if (bool(controls["left"])) {
		game.moveSelectedTileLeft();
	} else if (bool(controls["right"])) {
		game.moveSelectedTileRight();
	}
}