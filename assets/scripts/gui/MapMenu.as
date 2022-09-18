void MapMenuSetUp() {
	addWidget("VerticalLayout", "Menu");

	addWidget("BitmapButton", "MapMenu.Menu.Save");
	setWidgetSprite("MapMenu.Menu.Save", "icon", "saveicon");
	setWidgetText("MapMenu.Menu.Save", "save");

	addWidget("BitmapButton", "MapMenu.Menu.EndTurn");
	setWidgetSprite("MapMenu.Menu.EndTurn", "icon", "endturnicon");
	setWidgetText("MapMenu.Menu.EndTurn", "endturn");

	addWidget("BitmapButton", "MapMenu.Menu.Quit");
	setWidgetSprite("MapMenu.Menu.Quit", "icon", "quiticon");
	setWidgetText("MapMenu.Menu.Quit", "quit");
	
	string height = formatUInt(getWidgetCount("MapMenu.Menu") * 25) + "px";
	setWidgetSize("MapMenu.Menu", "25%", height);
	setWidgetPosition("MapMenu.Menu", "50%", "10px");
	setWidgetOrigin("MapMenu.Menu", 0.5, 0.0);
}

void MapMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		setGUI("Map");
	}
}

void MapMenu_Save_Pressed() {
	saveMap();
	setGUI("Map");
}

void MapMenu_EndTurn_Pressed() {
	game.endTurn();
	setGUI("Map");
}

void MapMenu_Quit_Pressed() {
	quitMap();
}