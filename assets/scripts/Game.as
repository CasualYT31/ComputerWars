void handleInput(const dictionary@ controls) {
	// We can assume that we are currently in the "Map" menu here.
	if (bool(controls["select"])) {
		if (getUnitOnTile(getSelectedTile()) == 0) {
			setGUI("MapMenu");
		}
	} else if (bool(controls["up"])) {
		moveSelectedTileUp();
	} else if (bool(controls["down"])) {
		moveSelectedTileDown();
	} else if (bool(controls["left"])) {
		moveSelectedTileLeft();
	} else if (bool(controls["right"])) {
		moveSelectedTileRight();
	}
}