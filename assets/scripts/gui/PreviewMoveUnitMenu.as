/**
 * Sets up the menu that is shown whilst previewing a unit move operation.
 */
void PreviewMoveUnitMenuSetUp() {
	addWidget("BitmapButton", "Wait");
	setWidgetText("Wait", "wait");
	setWidgetSize("Wait", "100px", "30px");
	setWidgetSprite("Wait", "icon", "waiticon");
}

/**
 * Positions the command menu relative to the current mouse position.
 */
void PreviewMoveUnitMenuOpen() {
	int xOffset = 10;
	if (mousePosition().x >= int(getWindowSize().x / 2)) {
		xOffset -= 120;
	}
	int yOffset = 10;
	if (mousePosition().y >= int(getWindowSize().y / 2)) {
		yOffset -= 50;
	}
	auto mouse = scaledMousePosition();
	if (mouse == INVALID_MOUSE) {
		mouse.x = 0;
		mouse.y = 0;
	}
	setWidgetPosition("Wait", formatInt(mouse.x + xOffset) + "px",
		formatInt(mouse.y + yOffset) + "px");
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