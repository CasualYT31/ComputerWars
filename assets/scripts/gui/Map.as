/**
 * The army widget.
 */
ArmyWidget armyWidget;

/**
 * The tile widget.
 */
TileWidget tileWidget;

/**
 * The menu used when a game is in play only has the army widget and the tile
 * widget.
 */
void MapSetUp() {
	armyWidget = ArmyWidget("Map.ArmyWidget", 128);
	tileWidget = TileWidget("Map.TileWidget");
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
			if (currentPosition != previousPosition) {
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
	if (bool(controls["info"])) {
		setGUI("DetailedInfoMenu");
		return;
	}
}

/**
 * Used to make sure that the enter key doesn't trigger the dev command menu to
 * reappear immediately after sending a command.
 */
bool CANCEL_DEV_INPUT = false;

/**
 * Handles input specific to the \c Map menu.
 * @param controls The control map given by the engine.
 */
void MapHandleInput(const dictionary controls) {
	// If the dev key was hit, open the debug menu.
	if (bool(controls["dev"])) {
		if (CANCEL_DEV_INPUT) {
			CANCEL_DEV_INPUT = false;
		} else {
			CANCEL_DEV_INPUT = true;
			setGUI("DevCommandMenu");
			return;
		}
	}

	HandleCommonGameInput(controls);
	
	// Update army widget.
	armyWidget.update(game.map.getSelectedArmy());
	if (!game.map.isCursorOnLeftSide()) {
		armyWidget.setAlignment(ArmyWidgetAlignment::Left);
		setWidgetOrigin(armyWidget.panel, 0.0, 0.0);
		setWidgetPosition(armyWidget.panel, "0%-" +
			formatFloat(ARMYWIDGET_RADIUS) +
			"px-" + formatFloat(ARMYWIDGET_BORDER_SIZE) + "px",
			"0%-" + formatFloat(ARMYWIDGET_BORDER_SIZE) + "px");
	} else {
		armyWidget.setAlignment(ArmyWidgetAlignment::Right);
		setWidgetOrigin(armyWidget.panel, 1.0, 0.0);
		setWidgetPosition(armyWidget.panel, "100%", "0%");
		setWidgetPosition(armyWidget.panel, "100%+" +
			formatFloat(ARMYWIDGET_RADIUS) +
			"px+" + formatFloat(ARMYWIDGET_BORDER_SIZE) + "px",
			"0%-" + formatFloat(ARMYWIDGET_BORDER_SIZE) + "px");
	}

	// Update tile widget.
	tileWidget.update(game.map.getSelectedTile());
	if (!game.map.isCursorOnLeftSide()) {
		tileWidget.setAlignment(TileWidgetAlignment::Left);
		setWidgetOrigin(tileWidget.layout, 0.0, 1.0);
		setWidgetPosition(tileWidget.layout, "0%", "100%");
	} else {
		tileWidget.setAlignment(TileWidgetAlignment::Right);
		setWidgetOrigin(tileWidget.layout, 1.0, 1.0);
		setWidgetPosition(tileWidget.layout, "100%", "100%");
	}

	// If the user is holding the correct control, calculate the attack range of
	// the unit on the currently selected tile and display it.
	if (bool(controls["range"])) {
		
	} else if (bool(controls["select"])) {
		auto cursor = game.map.getSelectedTile();
		auto unitID = game.map.getUnitOnTile(cursor);
		const auto currentArmy = game.map.getSelectedArmy();
		if (unitID == 0) {
			ArmyID owner = game.map.getTileOwner(cursor);
			string type = game.map.getTileType(cursor).type.scriptName;
			if (owner == currentArmy) {
				if (type == "BASE" || type == "AIRPORT" || type == "PORT") {
					setGUI("BaseMenu");
					return;
				}
			}
			setGUI("MapMenu");
			return;
		} else if (game.map.isUnitWaiting(unitID) ||
			!game.map.isUnitVisible(unitID, currentArmy)) {
			setGUI("MapMenu");
			return;
		} else if (game.map.getArmyOfUnit(unitID) == game.map.getSelectedArmy() &&
			!game.map.isUnitWaiting(unitID)) {
			game.selectUnit(unitID);
			game.map.removeAllPreviewUnits();
			setGUI("MoveUnitMenu");
			return;
		}
	}
}
