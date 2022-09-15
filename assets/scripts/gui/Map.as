TileWidget test;

void MapSetUp() {
	test = TileWidget("test");
	setWidgetOrigin(test.layout, 0.5, 0.5);
	setWidgetPosition(test.layout, "50%", "50%");
}

void MapOpen() {
}

// Holds the previous mouse position.
MousePosition previousPosition;

void MapHandleInput(const dictionary controls) {
	// Test.
	test.update(game.getSelectedTile(), TileWidgetAlignment::Left);

	// Handle mouse input.
	MousePosition currentPosition = mousePosition();
	// Ignore the mouse if the game doesn't have focus.
	if (currentPosition != INVALID_MOUSE) {
		// Ignore the mouse if it's outside of the window.
		Vector2 windowSize = getWindowSize();
		if (currentPosition.x >= 0 && currentPosition.y >= 0
			&& currentPosition.x <= windowSize.x
			&& currentPosition.y <= windowSize.y) {
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
		if (game.getUnitOnTile(game.getSelectedTile()) == 0) {
			setGUI("MapMenu");
		}
	}
	if (bool(controls["info"])) {
		setGUI("DetailedInfoMenu");
	}
}