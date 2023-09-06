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
 * Input handling code common to both Map and MoveUnitMenu.
 * @param  controls         The control map given by the engine.
 * @param  mouseInputs      Stores which controls are being triggered by the
 *                          mouse.
 * @param  previousPosition The previous mouse position.
 * @param  currentPosition  The current mouse position.
 * @return \c TRUE if this function updated the selected tile, \c FALSE if not.
 */
bool HandleCommonGameInput(const dictionary@ controls,
    const dictionary@ mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
	bool ret = false;

	// Handle mouse input. Ignore the mouse if the game doesn't have focus.
	if (currentPosition != INVALID_MOUSE) {
		// Ignore the mouse if it's outside of the window.
		Vector2 windowSize = getWindowSize();
		if (currentPosition.x >= 0 && currentPosition.y >= 0
			&& currentPosition.x <= int(windowSize.x)
			&& currentPosition.y <= int(windowSize.y)) {
			// Only consider the mouse if it has moved.
			if (currentPosition != previousPosition) {
				game.setSelectedTileByPixel(currentPosition);
				ret = true;
			}
		}
	}

	// Handle controls.
	if (bool(controls["up"])) {
		game.moveSelectedTileUp();
		ret = true;
	} else if (bool(controls["down"])) {
		game.moveSelectedTileDown();
		ret = true;
	} else if (bool(controls["left"])) {
		game.moveSelectedTileLeft();
		ret = true;
	} else if (bool(controls["right"])) {
		game.moveSelectedTileRight();
		ret = true;
	}
	if (bool(controls["zoomout"])) {
		game.zoomOut();
	}
	if (bool(controls["zoomin"])) {
		game.zoomIn();
	}

	// Make sure this stays at the bottom.
	if (bool(controls["info"])) {
		setGUI("DetailedInfoMenu");
	}
	return ret;
}

/**
 * Used to make sure that the enter key doesn't trigger the dev command menu to
 * reappear immediately after sending a command.
 */
bool CANCEL_DEV_INPUT = false;

/**
 * Handles input specific to the \c Map menu.
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void MapHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
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

	HandleCommonGameInput(controls, mouseInputs, previousPosition,
        currentPosition);
	
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
	const auto selectedTile = game.map.getSelectedTile();
	tileWidget.update(selectedTile);
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
	// the unit on the currently selected tile and display it. Otherwise, allow a
	// unit to be selected.
	const auto unitID = game.map.getUnitOnTile(selectedTile);
	game.showAttackRangeOfUnit(NO_UNIT);
	if (bool(controls["range"])) {
		game.enableClosedList(false);
        if (unitID != NO_UNIT) game.showAttackRangeOfUnit(unitID);
        else game.showAttackRangeOfTile(selectedTile);
	} else if (bool(controls["select"])) {
        // If the select control is being made by the mouse, and it is not inside
        // the map's graphic, then drop it.
        if (bool(mouseInputs["select"]) &&
            !game.map.getMapBoundingBox().contains(currentPosition)) return;

		const auto currentArmy = game.map.getSelectedArmy();
		if (unitID == NO_UNIT) {
            if (!game.map.isOutOfBounds(selectedTile)) {
                ArmyID owner = game.map.getTileOwner(selectedTile);
                string type = game.map.getTileType(selectedTile).type.scriptName;
                if (owner == currentArmy) {
                    if (type == "BASE" || type == "AIRPORT" || type == "PORT") {
                        setGUI("BaseMenu");
                        return;
                    }
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
			game.enableClosedList(true);
			game.map.removeAllPreviewUnits();
			setGUI("MoveUnitMenu");
			return;
		}
	}
}
