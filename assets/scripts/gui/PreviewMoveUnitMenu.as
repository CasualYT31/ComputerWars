void PreviewMoveUnitMenuSetUp() {
	addWidget("BitmapButton", "Wait");
	setWidgetText("Wait", "wait");
	setWidgetSize("Wait", "100px", "30px");
	setWidgetSprite("Wait", "icon", "waiticon");
}

void PreviewMoveUnitMenuOpen() {
	int offset = 10;
	if (mousePosition().x >= int(getWindowSize().x / 2)) {
		offset -= 120;
	}
	auto mouse = scaledMousePosition();
	if (mouse == INVALID_MOUSE) {
		mouse.x = 0;
		mouse.y = 0;
	}
	setWidgetPosition("Wait", formatInt(mouse.x + offset) + "px",
		formatInt(mouse.y + 10) + "px");
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