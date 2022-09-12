void MapMenuSetUp() {
	addWidget("VerticalLayout", "Menu");
	setWidgetPosition("MapMenu.Menu", "(parent.innersize-size)/2", "5px");
	setWidgetSize("MapMenu.Menu", "25%", "10%");

	addWidget("BitmapButton", "MapMenu.Menu.Save");
	setWidgetSprite("MapMenu.Menu.Save", "icon", "saveicon");
	setWidgetText("MapMenu.Menu.Save", "save");

	addWidget("BitmapButton", "MapMenu.Menu.Quit");
	setWidgetSprite("MapMenu.Menu.Quit", "icon", "quiticon");
	setWidgetText("MapMenu.Menu.Quit", "quit");
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

void MapMenu_Quit_Pressed() {
	quitMap();
}