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
	// Add commands here.
	PreviewCommands.addCommand("Wait", "wait", "waiticon");

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
		game.map.renderUnitAtDestination(false);
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