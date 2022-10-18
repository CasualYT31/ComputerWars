/**
 * @file PreviewMoveUnitMenu.as
 * Code that's run when the user is previewing a unit move operation.
 */

/**
 * Stores the commands available for the moving unit.
 */
CommandWidget PreviewCommands;

/**
 * Sets up the menu that is shown whilst previewing a unit move operation.
 */
void PreviewMoveUnitMenuSetUp() {
	PreviewCommands = CommandWidget("Menu", "100px");
}

/**
 * Positions the command menu relative to the current mouse position.
 * Also adds all the buttons to the command menu.
 */
void PreviewMoveUnitMenuOpen() {
	// Gather data.
	const auto unit = game.map.getSelectedUnit();
	const auto tile = game.map.getSelectedTile();
	const auto unitOnTile = game.map.getUnitOnTile(tile);

	// Add commands here.
	if (game.canJoin(unitOnTile, unit)) {
		PreviewCommands.addCommand("Join", "join", "joinicon");
	} else {
		if (game.canCapture(unit, tile)) {
			PreviewCommands.addCommand("Capture", "capture", "captureicon");
		}
		if (game.canLoad(unit, unitOnTile)) {
			PreviewCommands.addCommand("Load", "load", "loadicon");
		}
		if (game.map.getUnitType(unit).scriptName == "APC" &&
			game.areThereDepletedArmyUnitsAdjacentTo(tile,
				game.map.getArmyOfUnit(unit))) {
			PreviewCommands.addCommand("Supply", "supply", "replenishicon");
		}
		PreviewCommands.addCommand("Wait", "wait", "waiticon");
	}

	// Position the command menu differently depending on the quadrant of the
	// screen that the mouse is in.
	const Vector2f commandMenuSize = getWidgetFullSize(PreviewCommands.layout);
	int xOffset = 10;
	if (mousePosition().x >= int(getWindowSize().x / 2)) {
		xOffset = xOffset - int(commandMenuSize.x) - 20;
	}
	int yOffset = 10;
	if (mousePosition().y >= int(getWindowSize().y / 2)) {
		yOffset = yOffset - int(commandMenuSize.y) - 20;
	}
	auto mouse = scaledMousePosition();
	if (mouse == INVALID_MOUSE) {
		mouse.x = 0;
		mouse.y = 0;
	}
	setWidgetPosition(PreviewCommands.layout, formatInt(mouse.x + xOffset) + "px",
		formatInt(mouse.y + yOffset) + "px");
}

/**
 * Removes all buttons from the command menu.
 */
void PreviewMoveUnitMenuClose() {
	PreviewCommands.removeAllCommands();
}

/**
 * Allows the user to cancel a unit move operation.
 * @param controls The control map given by the engine.
 */
void PreviewMoveUnitMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		game.map.disableSelectedUnitRenderingEffects(false);
		game.map.removePreviewUnit(game.map.getSelectedUnit());
		setGUI("MoveUnitMenu");
		return;
	}
}

/**
 * Moves a unit and makes it wait at its new destination.
 */
void PreviewMoveUnitMenu_Wait_Pressed() {
	game.moveUnit();
	setGUI("Map");
}

/**
 * Joins two units together.
 */
void PreviewMoveUnitMenu_Join_Pressed() {
	game.joinUnits(
		game.map.getUnitOnTile(game.map.getSelectedTile()),
		game.map.getSelectedUnit(),
		game.map.closedList[game.map.closedList.length() - 1].g
	);
	// Deselect the unit or else the game will crash, since we just deleted it!
	game.selectUnit(0);
	setGUI("Map");
}

/**
 * Moves a unit and make it capture a property.
 */
void PreviewMoveUnitMenu_Capture_Pressed() {
	game.moveUnitAndCapture();
	setGUI("Map");
}

/**
 * Resupplies adjacent units and makes the selected unit wait.
 */
void PreviewMoveUnitMenu_Supply_Pressed() {
	// Cache selected unit, as moveUnit() unselects it.
	const auto selectedUnit = game.map.getSelectedUnit();
	game.moveUnit();
	game.APCReplenishUnits(selectedUnit);
	setGUI("Map");
}

/**
 * Loads the moving unit onto the stationary one.
 */
void PreviewMoveUnitMenu_Load_Pressed() {
	game.loadUnit(
		game.map.getSelectedUnit(),
		game.map.getUnitOnTile(game.map.getSelectedTile()),
		game.map.closedList[game.map.closedList.length() - 1].g
	);
	// Deselect the unit!
	game.selectUnit(0);
	setGUI("Map");
}