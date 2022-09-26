void PreviewMoveUnitMenuSetUp() {
	addWidget("BitmapButton", "Wait");
	setWidgetText("Wait", "wait");
	setWidgetSize("Wait", "100px", "30px");
	setWidgetSprite("Wait", "icon", "waiticon");
}

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

void PreviewMoveUnitMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		game.togglePreviewMoveMode(false);
		setGUI("MoveUnitMenu");
		return;
	}
}

void PreviewMoveUnitMenu_Wait_Pressed() {
	game.moveUnit();
	setGUI("Map");
}