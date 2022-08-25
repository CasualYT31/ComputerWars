void MapSetUp() {}

void MapHandleInput(const dictionary controls) {
	if (bool(controls["select"])) {
		if (game.getUnitOnTile(game.getSelectedTile()) == 0) {
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