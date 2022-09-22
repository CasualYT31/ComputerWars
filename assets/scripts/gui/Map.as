void MapSetUp() {}

// Holds the previous mouse position.
MousePosition previousPosition;

void MapHandleInput(const dictionary controls) {
	// Handle mouse input.
	MousePosition currentPosition = mousePosition();
	// Ignore the mouse if the game doesn't have focus.
	if (currentPosition != INVALID_MOUSE) {
		// Ignore the mouse if it's outside of the window.
		Vector2 windowSize = getWindowSize();
		if (currentPosition.x >= 0 && currentPosition.y >= 0
			&& currentPosition.x <= int(windowSize.x)
			&& currentPosition.y <= int(windowSize.y)) {
			// Only consider the mouse if it has moved.
			if (currentPosition.x != previousPosition.x) {
				game.setSelectedTileByPixel(currentPosition);
			}
		}
	}
	previousPosition = currentPosition;

	// Handle controls.
	if (bool(controls["up"])) {
		game.moveSelectedTileUp();
	} else if (bool(controls["down"])) {
		game.moveSelectedTileDown();
	} else if (bool(controls["left"])) {
		game.moveSelectedTileLeft();
	} else if (bool(controls["right"])) {
		game.moveSelectedTileRight();
	}
	if (bool(controls["zoomout"])) {
		game.zoomOut();
	}
	if (bool(controls["zoomin"])) {
		game.zoomIn();
	}
	if (bool(controls["select"])) {
		auto cursor = game.getSelectedTile();
		if (game.getUnitOnTile(cursor) == 0) {
			ArmyID owner = game.getTileOwner(cursor);
			ArmyID currentArmy = game.getCurrentArmy();
			string type = game.getTerrainOfTile(cursor).scriptName;
			if (owner == currentArmy) {
				if (type == "BASE" || type == "AIRPORT" || type == "PORT") {
					setGUI("BaseMenu");
					return;
				}
			}
			setGUI("MapMenu");
			return;
		}
	}
	if (bool(controls["info"])) {
		setGUI("DetailedInfoMenu");
		return;
	}

	// Test move mode rendering.
	game.enableMoveMode();
}