TileWidget tileWidget;

/**
 * The menu used when a game is in play is completely empty.
 */
void MapSetUp() {
	tileWidget = TileWidget("Map.Test");
}

/**
 * Holds the previous mouse position.
 */
MousePosition previousPosition;

/**
 * Input handling code common to both Map and MoveUnitMenu.
 * @param controls The control map given by the engine.
 */
void HandleCommonGameInput(const dictionary@ controls) {
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
	tileWidget.update(game.map.getSelectedTile());

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
	if (bool(controls["info"])) {
		setGUI("DetailedInfoMenu");
		return;
	}
}

/**
 * Handles input specific to the \c Map menu.
 * @param controls The control map given by the engine.
 */
void MapHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["select"])) {
		auto cursor = game.map.getSelectedTile();
		auto unitID = game.map.getUnitOnTile(cursor);
		if (unitID == 0) {
			ArmyID owner = game.map.getTileOwner(cursor);
			ArmyID currentArmy = game.map.getSelectedArmy();
			string type = game.map.getTileType(cursor).type.scriptName;
			if (owner == currentArmy) {
				if (type == "BASE" || type == "AIRPORT" || type == "PORT") {
					setGUI("BaseMenu");
					return;
				}
			}
			setGUI("MapMenu");
			return;
		} else if (game.map.isUnitWaiting(unitID)) {
			setGUI("MapMenu");
			return;
		} else if (game.map.getArmyOfUnit(unitID) == game.map.getSelectedArmy() &&
			!game.map.isUnitWaiting(unitID)) {
			game.selectUnit(unitID);
			setGUI("MoveUnitMenu");
			return;
		}
	}
}